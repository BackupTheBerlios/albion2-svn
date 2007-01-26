/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include "a2ematerial.h"

/*! a2ematerial constructor
 */
a2ematerial::a2ematerial(engine* e) {
	// get classes
	a2ematerial::e = e;
	a2ematerial::c = e->get_core();
	a2ematerial::m = e->get_msg();
	a2ematerial::file = e->get_file_io();
	a2ematerial::t = e->get_texman();
	a2ematerial::exts = e->get_ext();
}

/*! a2ematerial destructor
 */
a2ematerial::~a2ematerial() {
	m->print(msg::MDEBUG, "a2ematerial.cpp", "freeing a2ematerial stuff");

	// needed?
    /*for(vector<texture_elem>::iterator titer = textures.begin(); titer != textures.end(); titer++) {
        delete [] titer->tex_names;
        delete [] titer->textures;
    }*/
	textures.clear();

	m->print(msg::MDEBUG, "a2ematerial.cpp", "a2ematerial stuff freed");
}

/*! loads an .a2mtl material file
 *  @param filename the materials filename
 */
void a2ematerial::load_material(char* filename) {
	textures.clear();

	file->open_file(filename, file_io::OT_READ_BINARY);

	// get file type
	char* file_type = new char[12];
	file->get_block(file_type, 11);
	file_type[11] = 0;
	if(strcmp(file_type, "A2EMATERIAL") != 0) {
		m->print(msg::MERROR, "a2ematerial.cpp", "%s is no a2e material file!", filename);
		delete file_type;
		return;
	}
	delete [] file_type;

	unsigned int celems = file->get_uint();
	for(unsigned int i = 0; i < celems; i++) {
		textures.push_back(*new a2ematerial::texture_elem());
		textures.back().obj_num = i;

		// get material type
		textures.back().mat_type = file->get_char();
		if(textures.back().mat_type > MAT_TYPE_COUNT) {
			m->print(msg::MERROR, "a2ematerial.cpp", "load_material(): unknown material type (obj: %u, type: %u)!",
				i, (unsigned int)textures.back().mat_type);
		}

		textures.back().tex_count = get_mat_type_tex_count((a2ematerial::MAT_TYPE)textures.back().mat_type);

		// multi texturing stuff

		// reserve memory for a maximum of 8 textures for avoiding memory (re-)allocation
		// during runtime (if the material is change during runtime)
		textures.back().col_type = new char[8];
		textures.back().rgb_combine = new char[8];
		textures.back().alpha_combine = new char[8];
		textures.back().rgb_source = new char*[8];
		textures.back().rgb_operand = new char*[8];
		textures.back().alpha_source = new char*[8];
		textures.back().alpha_operand = new char*[8];

		// as above, reserve maximum of needed memory
		for(unsigned int j = 0; j < 8; j++) {
			textures.back().rgb_source[j] = new char[3];
			textures.back().rgb_operand[j] = new char[3];
			textures.back().alpha_source[j] = new char[3];
			textures.back().alpha_operand[j] = new char[3];

			// set stuff to zero
			textures.back().col_type[j] = 0;
			textures.back().rgb_combine[j] = (char)0xFF;
			textures.back().alpha_combine[j] = (char)0xFF;
			for(unsigned int k = 0; k < 3; k++) {
				textures.back().rgb_source[j][k] = 0;
				textures.back().rgb_operand[j][k] = 0;
				textures.back().alpha_source[j][k] = 0;
				textures.back().alpha_operand[j][k] = 0;
			}
		}

		if(textures.back().mat_type == 0x04) {
			textures.back().tex_count = file->get_uint();
			/*textures.back().col_type = new char[textures.back().tex_count];
			textures.back().rgb_combine = new char[textures.back().tex_count];
			textures.back().alpha_combine = new char[textures.back().tex_count];
			textures.back().rgb_source = new char*[textures.back().tex_count];
			textures.back().rgb_operand = new char*[textures.back().tex_count];
			textures.back().alpha_source = new char*[textures.back().tex_count];
			textures.back().alpha_operand = new char*[textures.back().tex_count];*/

			unsigned int argc_rgb = 0;
			unsigned int argc_alpha = 0;
			for(unsigned int j = 0; j < textures.back().tex_count; j++) {
				textures.back().col_type[j] = (file->get_char() & 0xFF);
				textures.back().rgb_combine[j] = (file->get_char() & 0xFF);
				textures.back().alpha_combine[j] = (file->get_char() & 0xFF);

				switch(textures.back().rgb_combine[j] & 0xFF) {
					case 0x00:
						argc_rgb = 1;
						break;
					case 0x01:
					case 0x02:
					case 0x03:
					case 0x04:
						argc_rgb = 2;
						break;
					case 0x05:
						argc_rgb = 3;
						break;
					case 0xFF:
						argc_rgb = 0;
						break;
				}

				switch(textures.back().alpha_combine[j] & 0xFF) {
					case 0x00:
						argc_alpha = 1;
						break;
					case 0x01:
					case 0x02:
					case 0x03:
					case 0x04:
						argc_alpha = 2;
						break;
					case 0x05:
						argc_alpha = 3;
						break;
					case 0xFF:
						argc_alpha = 0;
						break;
				}

				/*textures.back().rgb_source[j] = new char[argc_rgb];
				textures.back().rgb_operand[j] = new char[argc_rgb];
				textures.back().alpha_source[j] = new char[argc_alpha];
				textures.back().alpha_operand[j] = new char[argc_alpha];*/

				for(unsigned int k = 0; k < argc_rgb; k++) {
					textures.back().rgb_source[j][k] = file->get_char();
					textures.back().rgb_operand[j][k] = file->get_char();
				}

				for(unsigned int k = 0; k < argc_alpha; k++) {
					textures.back().alpha_source[j][k] = file->get_char();
					textures.back().alpha_operand[j][k] = file->get_char();
				}
			}
		}
		else {
			// get color type
			//textures.back().col_type = new char[textures.back().tex_count];
			for(unsigned int j = 0; j < textures.back().tex_count; j++) {
				textures.back().col_type[j] = file->get_char();
				if(textures.back().col_type[j] > COL_TYPE_COUNT) {
					m->print(msg::MERROR, "a2ematerial.cpp", "load_material(): unknown color type (obj: %u, type: %u)!",
						i, (unsigned int)textures.back().col_type[j]);
				}
			}
		}

		// create tex name pointers
		//textures.back().tex_names = new string[textures.back().tex_count];
		textures.back().tex_names = new string[8];

		// create textures
		//textures.back().textures = new GLuint[textures.back().tex_count];
		textures.back().textures = new GLuint[8];

		// get texture file names
		unsigned int x = 0;
		while(x < textures.back().tex_count) {
			file->get_terminated_block(&textures.back().tex_names[x], (char)0xFF);
			x++;
		}
	}

	// close file
	file->close_file();

	// load the textures
	a2ematerial::load_textures();
}

/*! loads the textures
 */
void a2ematerial::load_textures() {
	if(e->get_init_mode() == engine::GRAPHICAL) {
		for(vector<texture_elem>::iterator tex_iter = textures.begin(); tex_iter != textures.end(); tex_iter++) {
			for(unsigned int i = 0; i < tex_iter->tex_count; i++) {
				switch(tex_iter->col_type[i]) {
					// RGB
					case 0x00:
						tex_iter->textures[i] = t->add_texture(e->data_path(tex_iter->tex_names[i].c_str()), 3, GL_RGB);
						break;
					// RGBA
					case 0x01:
						tex_iter->textures[i] = t->add_texture(e->data_path(tex_iter->tex_names[i].c_str()), 4, GL_RGBA);
						break;
					// LUMINANCE
					case 0x02:
						tex_iter->textures[i] = t->add_texture(e->data_path(tex_iter->tex_names[i].c_str()), 1, GL_LUMINANCE);
						break;
					default:
						tex_iter->textures[i] = t->add_texture(e->data_path(tex_iter->tex_names[i].c_str()), 3, GL_RGB);
						break;
				}
			}
		}
	}
}

/*! returns a texture
 *  @param obj_num the number of the object we want to get texture data from
 *  @param num the textures num we want to get
 */
GLuint a2ematerial::get_texture(unsigned int obj_num, unsigned int num) {
	for(vector<texture_elem>::iterator tex_iter = textures.begin(); tex_iter != textures.end(); tex_iter++) {
		if(tex_iter->obj_num == obj_num) {
			return tex_iter->textures[num];
		}
	}
	m->print(msg::MERROR, "a2ematerial.cpp", "get_texture(): texture element/object #%u or texture #%u does not exist!", obj_num, num);
	return 0;
}

/*! returns the material type (MAT_TYPES)
 *  @param obj_num the number of the object we want to get material type from
 */
char a2ematerial::get_material_type(unsigned int obj_num) {
	for(vector<texture_elem>::iterator tex_iter = textures.begin(); tex_iter != textures.end(); tex_iter++) {
		if(tex_iter->obj_num == obj_num) {
			return tex_iter->mat_type;
		}
	}
	m->print(msg::MERROR, "a2ematerial.cpp", "get_material_type(): texture element/object #%u does not exist!", obj_num);
	return 0;
}

/*! returns the color type (0x00 = RGB, 0x01 = RGBA)
 *  @param obj_num the number of the object we want to get color type from
 *  @param texture the number of the objects texture
 */
char a2ematerial::get_color_type(unsigned int obj_num, unsigned int texture) {
	for(vector<texture_elem>::iterator tex_iter = textures.begin(); tex_iter != textures.end(); tex_iter++) {
		if(tex_iter->obj_num == obj_num) {
			return tex_iter->col_type[texture];
		}
	}
	m->print(msg::MERROR, "a2ematerial.cpp", "get_color_type(): texture element/object #%u does not exist!", obj_num);
	return 0;
}

/*! returns the texture count of the specified object
 *  @param obj_num the number of the object we want to get texture count from
 */
unsigned int a2ematerial::get_texture_count(unsigned int obj_num) {
	for(vector<texture_elem>::iterator tex_iter = textures.begin(); tex_iter != textures.end(); tex_iter++) {
		if(tex_iter->obj_num == obj_num) {
			return tex_iter->tex_count;
		}
	}
	m->print(msg::MERROR, "a2ematerial.cpp", "get_texture_count(): texture element/object #%u does not exist!", obj_num);
	return 0;
}

/*! enables all texture of the specified object (in and w/o shader mode)
 *  @param obj_num the number of the object
 *  @param no_shader flag that specifies if shaders are being used
 */
void a2ematerial::enable_texture(unsigned int obj_num, bool no_shader) {
	switch(textures.at(obj_num).mat_type) {
		case a2ematerial::DIFFUSE: {
			// used by the shader pipeline
			if(!no_shader && exts->is_multitexture_support() && exts->is_shader_support()) {
				exts->glActiveTextureARB(GL_TEXTURE0_ARB);
				glBindTexture(GL_TEXTURE_2D, get_texture(obj_num, 0));
				glEnable(GL_TEXTURE_2D);

				exts->glActiveTextureARB(GL_TEXTURE1_ARB);
				glBindTexture(GL_TEXTURE_2D, get_texture(obj_num, 1));
				glEnable(GL_TEXTURE_2D);
			}
			// used by the fixed pipeline
			else {
				glEnable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, get_texture(obj_num, 0));
				if(textures.at(obj_num).col_type[0] == 0x01) { glEnable(GL_BLEND); }
			}
		}
		break;
		case a2ematerial::BUMP: {
				e->get_ext()->glActiveTextureARB(GL_TEXTURE0_ARB);
				glBindTexture(GL_TEXTURE_2D, get_texture(obj_num, 0));
				glEnable(GL_TEXTURE_2D);

				e->get_ext()->glActiveTextureARB(GL_TEXTURE1_ARB);
				glBindTexture(GL_TEXTURE_2D, get_texture(obj_num, 1));
				glEnable(GL_TEXTURE_2D);

				glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_ARB);
				glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB_ARB, GL_DOT3_RGB_ARB);

				glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB_ARB, GL_TEXTURE);
				glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB_ARB, GL_DOT3_RGB_ARB);
				glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB_ARB, GL_PREVIOUS);
				glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB_ARB, GL_DOT3_RGB_ARB);
		}
		break;
		case a2ematerial::MULTITEXTURE: {
			for(unsigned int i = 0; i < textures.at(obj_num).tex_count; i++) {
				e->get_ext()->glActiveTextureARB(GL_TEXTURE0_ARB+i);
				glBindTexture(GL_TEXTURE_2D, get_texture(obj_num, i));
				glEnable(GL_TEXTURE_2D);

				// since we can't set any tex env combine stuff in the first texture,
				// we skip just skip this part ...
				if(i != 0) {
					unsigned int argc_rgb = 0;
					unsigned int argc_alpha = 0;

					switch(textures.at(obj_num).rgb_combine[i] & 0xFF) {
						case 0x00:
							argc_rgb = 1;
							break;
						case 0x01:
						case 0x02:
						case 0x03:
						case 0x04:
							argc_rgb = 2;
							break;
						case 0x05:
							argc_rgb = 3;
							break;
						case 0xFF:
							argc_rgb = 0;
							break;
					}

					switch(textures.at(obj_num).alpha_combine[i] & 0xFF) {
						case 0x00:
							argc_alpha = 1;
							break;
						case 0x01:
						case 0x02:
						case 0x03:
						case 0x04:
							argc_alpha = 2;
							break;
						case 0x05:
							argc_alpha = 3;
							break;
						case 0xFF:
							argc_alpha = 0;
							break;
					}

					//if((argc_rgb | argc_alpha) != 0) { glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_ARB); }
					glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_ARB);

					if((textures.at(obj_num).rgb_combine[i] & 0xFF) != 0xFF) {
						glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB_ARB, get_combine(textures.at(obj_num).rgb_combine[i]));
					}
					else {
						// use replace as standard
						glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB_ARB, GL_REPLACE);
						glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB_ARB, GL_TEXTURE);
						glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB_ARB, GL_SRC_COLOR);
					}

					for(unsigned int j = 0; j < argc_rgb; j++) {
						glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB_ARB+j, get_rgb_source(textures.at(obj_num).rgb_source[i][j]));
						glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB_ARB+j, get_rgb_operand(textures.at(obj_num).rgb_operand[i][j]));
					}

					if((textures.at(obj_num).alpha_combine[i] & 0xFF) != 0xFF) {
						glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_ALPHA_ARB, get_combine(textures.at(obj_num).alpha_combine[i]));
					}
					else {
						// use replace as standard
						glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_ALPHA_ARB, GL_REPLACE);
						glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_ALPHA_ARB, GL_TEXTURE);
						glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_ALPHA_ARB, GL_SRC_ALPHA);
					}

					for(unsigned int j = 0; j < argc_alpha; j++) {
						glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_ALPHA_ARB+j, get_alpha_source(textures.at(obj_num).alpha_source[i][j]));
						glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_ALPHA_ARB+j, get_alpha_operand(textures.at(obj_num).alpha_operand[i][j]));
					}
				}
			}
			//if(textures.at(obj_num).col_type[0] == 0x01) { glEnable(GL_BLEND); }

		}
		break;
	}
}

/*! disables all texture of the specified object (in and w/o shader mode)
 *  @param obj_num the number of the object
 *  @param no_shader flag that specifies if shaders are being used
 */
void a2ematerial::disable_texture(unsigned int obj_num, bool no_shader) {
	switch(textures.at(obj_num).mat_type) {
		case a2ematerial::DIFFUSE:
			// used by the shader pipeline
			if(!no_shader && exts->is_multitexture_support() && exts->is_shader_support()) {
				e->get_ext()->glActiveTextureARB(GL_TEXTURE1_ARB);
				glDisable(GL_TEXTURE_2D);
				e->get_ext()->glActiveTextureARB(GL_TEXTURE0_ARB);
				glDisable(GL_TEXTURE_2D);
			}
			// used by the fixed pipeline
			else {
				glDisable(GL_BLEND);
				glDisable(GL_TEXTURE_2D);
			}
			break;
		case a2ematerial::BUMP:
			e->get_ext()->glActiveTextureARB(GL_TEXTURE1_ARB);
			glDisable(GL_TEXTURE_2D);
			e->get_ext()->glActiveTextureARB(GL_TEXTURE0_ARB);
			glDisable(GL_TEXTURE_2D);
			break;
		case a2ematerial::MULTITEXTURE:
			//if(textures.at(obj_num).col_type[0] == 0x01) { glDisable(GL_BLEND); }
			for(int i = (int)(textures.at(obj_num).tex_count-1); i >= 0; i--) {
				e->get_ext()->glActiveTextureARB(GL_TEXTURE0_ARB+i);
				glDisable(GL_TEXTURE_2D);
			}
			break;
	}
}

/*! returns the combine mode (used by multi-texturing) associated with a specific id (char)
 *  @param c the combine modes id
 */
unsigned int a2ematerial::get_combine(char c) {
	switch(c) {
		case 0x00:
			return GL_REPLACE;
			break;
		case 0x01:
			return GL_MODULATE;
			break;
		case 0x02:
			return GL_ADD;
			break;
		case 0x03:
			return GL_ADD_SIGNED;
			break;
		case 0x04:
			return GL_SUBTRACT;
			break;
		case 0x05:
			return GL_INTERPOLATE;
			break;
	}
	m->print(msg::MERROR, "a2ematerial.cpp", "get_combine(): unknown combine mode %u!", (unsigned int)(c & 0xFF));
	return 0;
}

/*! returns the rgb sourc (used by multi-texturing) associated with a specific id (char)
 *  @param c the rgb sources id
 */
unsigned int a2ematerial::get_rgb_source(char c) {
	switch(c) {
		case 0x00:
			return GL_PRIMARY_COLOR_ARB;
			break;
		case 0x01:
			return GL_TEXTURE;
			break;
		case 0x02:
			return GL_CONSTANT_ARB;
			break;
		case 0x03:
			return GL_PREVIOUS_ARB;
			break;
	}
	m->print(msg::MERROR, "a2ematerial.cpp", "get_rgb_source(): unknown rgb source %u!", (unsigned int)(c & 0xFF));
	return 0;
}

/*! returns the rgb operand (used by multi-texturing) associated with a specific id (char)
 *  @param c the rgb operands id
 */
unsigned int a2ematerial::get_rgb_operand(char c) {
	switch(c) {
		case 0x00:
			return GL_SRC_COLOR;
			break;
		case 0x01:
			return GL_ONE_MINUS_SRC_COLOR;
			break;
		case 0x02:
			return GL_SRC_ALPHA;
			break;
		case 0x03:
			return GL_ONE_MINUS_SRC_ALPHA;
			break;
	}
	m->print(msg::MERROR, "a2ematerial.cpp", "get_rgb_operand(): unknown rgb operand %u!", (unsigned int)(c & 0xFF));
	return 0;
}

/*! returns the alpha sourc (used by multi-texturing) associated with a specific id (char)
 *  @param c the alpha sources id
 */
unsigned int a2ematerial::get_alpha_source(char c) {
	switch(c) {
		case 0x00:
			return GL_PRIMARY_COLOR_ARB;
			break;
		case 0x01:
			return GL_TEXTURE;
			break;
		case 0x02:
			return GL_CONSTANT_ARB;
			break;
		case 0x03:
			return GL_PREVIOUS_ARB;
			break;
	}
	m->print(msg::MERROR, "a2ematerial.cpp", "get_alpha_source(): unknown alpha source %u!", (unsigned int)(c & 0xFF));
	return 0;
}

/*! returns the alpha operand (used by multi-texturing) associated with a specific id (char)
 *  @param c the alpha operands id
 */
unsigned int a2ematerial::get_alpha_operand(char c) {
	switch(c) {
		case 0x00:
			return GL_SRC_ALPHA;
			break;
		case 0x01:
			return GL_ONE_MINUS_SRC_ALPHA;
			break;
	}
	m->print(msg::MERROR, "a2ematerial.cpp", "get_alpha_operand(): unknown alpha operand %u!", (unsigned int)(c & 0xFF));
	return 0;
}

/*! returns the texture elem (struct) of the specified object
 *  @param obj_num the objects number from which we want to get the texture element
 */
a2ematerial::texture_elem* a2ematerial::get_tex_elem(unsigned int obj_num) {
	return &textures[obj_num];
}

/*! returns the materials sub-object count
 */
unsigned int a2ematerial::get_object_count() {
	return (unsigned int)a2ematerial::textures.size();
}

/*! returns the texture count of the specified material type
 *  @param type the material type
 */
unsigned int a2ematerial::get_mat_type_tex_count(MAT_TYPE type) {
	unsigned int ret = 0;
	switch(type) {
		case a2ematerial::NONE:
			ret = 0;
			break;
		case a2ematerial::DIFFUSE:
			ret = 2;
			break;
		case a2ematerial::BUMP:
			ret = 3;
			break;
		case a2ematerial::PARALLAX:
			ret = 4;
			break;
		case a2ematerial::MULTITEXTURE:
			ret = 8;
			break;
		default:
			break;
	}
	return ret;
}
