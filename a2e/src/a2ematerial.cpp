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

/*! there is no function currently
 */
a2ematerial::a2ematerial(engine* e) {
	// get classes
	a2ematerial::e = e;
	a2ematerial::c = e->get_core();
	a2ematerial::m = e->get_msg();
	a2ematerial::file = e->get_file_io();
	a2ematerial::t = e->get_texman();
}

/*! there is no function currently
 */
a2ematerial::~a2ematerial() {
	m->print(msg::MDEBUG, "a2ematerial.cpp", "freeing a2ematerial stuff");

    for(vector<texture_elem>::iterator titer = textures.begin(); titer != textures.end(); titer++) {
        delete [] titer->tex_names;
        delete [] titer->textures;
    }
	textures.clear();

	m->print(msg::MDEBUG, "a2ematerial.cpp", "a2ematerial stuff freed");
}

/*! loads an .a2mtl material file
 *  @param filename the materials filename
 */
void a2ematerial::load_material(char* filename) {
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
		if(textures.back().mat_type > 0x04) {
			m->print(msg::MERROR, "a2ematerial.cpp", "load_material(): unknown material type (obj: %u, type: %u)!",
				i, (unsigned int)textures.back().mat_type);
		}

		switch(textures.back().mat_type) {
			case 0x00:
				textures.back().tex_count = 0; // ???
				break;
			case 0x01:
				textures.back().tex_count = 1;
				break;
			case 0x02:
				textures.back().tex_count = 2;
				break;
			case 0x03:
				textures.back().tex_count = 3;
				break;
			case 0x04:
				textures.back().tex_count = 0;
				break;
		}

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
				if(textures.back().col_type[j] > 0x01) {
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
			unsigned char c = file->get_char();
			if(c == 0xFF) {
				textures.back().tex_names[x] = textures.back().tex_names[x];
				x++;
			}
			else {
				textures.back().tex_names[x] += c;
			}
		}
	}

	// close file
	file->close_file();

	// load the textures
	a2ematerial::load_textures();
}

/*! loads the textures (just .png)
 */
void a2ematerial::load_textures() {
	if(e->get_init_mode() == engine::GRAPHICAL) {
		for(vector<texture_elem>::iterator tex_iter = textures.begin(); tex_iter != textures.end(); tex_iter++) {
			for(unsigned int i = 0; i < tex_iter->tex_count; i++) {
				if(tex_iter->col_type[i] == 0x01) {
					tex_iter->textures[i] = t->add_texture(e->data_path((char*)tex_iter->tex_names[i].c_str()), 4, GL_RGBA);
				}
				else {
					tex_iter->textures[i] = t->add_texture(e->data_path((char*)tex_iter->tex_names[i].c_str()), 3, GL_RGB);
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

unsigned int a2ematerial::get_texture_count(unsigned int obj_num) {
	for(vector<texture_elem>::iterator tex_iter = textures.begin(); tex_iter != textures.end(); tex_iter++) {
		if(tex_iter->obj_num == obj_num) {
			return tex_iter->tex_count;
		}
	}
	m->print(msg::MERROR, "a2ematerial.cpp", "get_texture_count(): texture element/object #%u does not exist!", obj_num);
	return 0;
}

void a2ematerial::enable_texture(unsigned int obj_num) {
	switch(textures.at(obj_num).mat_type) {
		case a2ematerial::DIFFUSE:
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, textures.at(obj_num).textures[0]);
			if(textures.at(obj_num).col_type[0] == 0x01) { glEnable(GL_BLEND); }
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

void a2ematerial::disable_texture(unsigned int obj_num) {
	switch(textures.at(obj_num).mat_type) {
		case a2ematerial::DIFFUSE:
			if(textures.at(obj_num).col_type[0] == 0x01) { glDisable(GL_BLEND); }
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

a2ematerial::texture_elem* a2ematerial::get_tex_elem(unsigned int obj_num) {
	return &textures[obj_num];
}
