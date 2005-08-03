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
	texture_count = 0;
	tex_names = NULL;
	textures = NULL;

	// get classes
	a2ematerial::e = e;
	a2ematerial::c = e->get_core();
	a2ematerial::m = e->get_msg();
	a2ematerial::file = e->get_file_io();
}

/*! there is no function currently
 */
a2ematerial::~a2ematerial() {
	m->print(msg::MDEBUG, "a2ematerial.cpp", "freeing a2ematerial stuff");

	if(tex_names != NULL) {
		delete [] tex_names;
	}

	if(textures != NULL) {
		delete textures;
	}

	m->print(msg::MDEBUG, "a2ematerial.cpp", "a2ematerial stuff freed");
}

/*! loads an .a2mtl material file
 *  @param filename the materials filename
 */
void a2ematerial::load_material(char* filename) {
	file->open_file(filename, true);

	// get file type
	char* file_type = new char[12];
	file->get_block(file_type, 11);
	file_type[11] = 0;
	if(strcmp(file_type, "A2EMATERIAL") != 0) {
		m->print(msg::MERROR, "a2ematerial.cpp", "%s is no a2e material file!", filename);
		delete file_type;
		return;
	}
	delete file_type;

	// get texture count
	a2ematerial::texture_count = file->get_uint();

	// create tex name pointers
	a2ematerial::tex_names = new char*[a2ematerial::texture_count];

	// create textures
	a2ematerial::textures = new GLuint[a2ematerial::texture_count];

	// get filesize
	unsigned int size = file->get_filesize();

	// the remaining data ...
	char* data = new char[size - 14];
	file->get_block(data, size - 16);
	data[size - 15] = 0;

	// load the texture names
	unsigned int x = 0;
	char** tokens = new char*[a2ematerial::texture_count + 1];
	tokens[x] = strtok(data, "ÿ");
	while(tokens[x] != NULL) {
		x++;
		tokens[x] = strtok(NULL, "ÿ");
	}

	for(unsigned int i = 0; i < x; i++) {
		a2ematerial::tex_names[i] = new char[strlen(tokens[i])+1];
		memcpy(a2ematerial::tex_names[i], tokens[i], strlen(tokens[i]));
		a2ematerial::tex_names[i][strlen(tokens[i])] = 0;
		if(i == x - 1) { a2ematerial::tex_names[i][strlen(tokens[i])-1] = 0; }
	}

	delete data;

	// load the textures
	a2ematerial::load_textures();
}

/*! loads the textures (just .png)
 */
void a2ematerial::load_textures() {
	SDL_Surface** tex_surface = new SDL_Surface*[a2ematerial::texture_count];

	for(unsigned int i = 0; i < a2ematerial::texture_count; i++) {
		tex_surface[i] = IMG_LoadPNG_RW(SDL_RWFromFile(a2ematerial::tex_names[i], "rb"));
		if(!tex_surface) {
			m->print(msg::MERROR, "a2ematerial.cpp", "error loading texture file \"%s\"!", a2ematerial::tex_names[i]);
			return;
		}
	}

	glGenTextures(a2ematerial::texture_count, &a2ematerial::textures[0]);
	for(unsigned int i = 0; i < a2ematerial::texture_count; i++) {
		glBindTexture(GL_TEXTURE_2D, a2ematerial::textures[i]);	

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);

		gluBuild2DMipmaps(GL_TEXTURE_2D, 3, tex_surface[i]->w, tex_surface[i]->h,
			GL_RGB, GL_UNSIGNED_BYTE, tex_surface[i]->pixels);
	}

	for(unsigned int i = 0; i < a2ematerial::texture_count; i++) {
		if(tex_surface[i]) {
			SDL_FreeSurface(tex_surface[i]);
		}
	}
}

/*! returns a texture
 *  @param num the textures num we want to get
 */
GLuint* a2ematerial::get_texture(unsigned int num) {
	return &a2ematerial::textures[num];
}
