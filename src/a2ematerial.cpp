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

    for(list<texture_elem>::iterator titer = textures.begin(); titer != textures.end(); titer++) {
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
		if(textures.back().mat_type > 0x03) {
			m->print(msg::MERROR, "a2ematerial.cpp", "load_material(): unknown material type (obj: %u, type: %u)!",
				i, (unsigned int)textures.back().mat_type);
		}

		// get color type
		textures.back().col_type = file->get_char();
		if(textures.back().col_type > 0x01) {
			m->print(msg::MERROR, "a2ematerial.cpp", "load_material(): unknown color type (obj: %u, type: %u)!",
				i, (unsigned int)textures.back().col_type);
		}

		// create tex name pointers
		textures.back().tex_names = new string[textures.back().mat_type];

		// create textures
		textures.back().textures = new GLuint[textures.back().mat_type];

		// get texture file names
		unsigned int x = 0;
		while(x < (unsigned int)textures.back().mat_type) {
			unsigned char c = file->get_char();
			if(c == 0xFF) {
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
		for(list<texture_elem>::iterator tex_iter = textures.begin(); tex_iter != textures.end(); tex_iter++) {
			for(unsigned int i = 0; i < (unsigned int)tex_iter->mat_type; i++) {
				if(tex_iter->col_type == 0x01) {
					tex_iter->textures[i] = t->add_texture(tex_iter->tex_names[i].c_str(), 4, GL_RGBA);
				}
				else {
					tex_iter->textures[i] = t->add_texture(tex_iter->tex_names[i].c_str(), 3, GL_RGB);
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
	for(list<texture_elem>::iterator tex_iter = textures.begin(); tex_iter != textures.end(); tex_iter++) {
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
	for(list<texture_elem>::iterator tex_iter = textures.begin(); tex_iter != textures.end(); tex_iter++) {
		if(tex_iter->obj_num == obj_num) {
			return tex_iter->mat_type;
		}
	}
	m->print(msg::MERROR, "a2ematerial.cpp", "get_material_type(): texture element/object #%u does not exist!", obj_num);
	return 0;
}

/*! returns the color type (0x00 = RGB, 0x01 = RGBA)
 */
char a2ematerial::get_color_type(unsigned int obj_num) {
	for(list<texture_elem>::iterator tex_iter = textures.begin(); tex_iter != textures.end(); tex_iter++) {
		if(tex_iter->obj_num == obj_num) {
			return tex_iter->col_type;
		}
	}
	m->print(msg::MERROR, "a2ematerial.cpp", "get_color_type(): texture element/object #%u does not exist!", obj_num);
	return 0;
}
