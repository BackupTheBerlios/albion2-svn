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

#include "texman.h"

/*! creates the texman object
 */
texman::texman(msg* m) {
	ctextures = 0;
	textures = NULL;

	// get the msg class ...
	texman::m = m;
}

/*! deletes the texman object
 */
texman::~texman() {
}

/*! adds a texture and returns its (internal?) number
 *  @param filename the textures file name
 *  @param components the textures components (number of colors)
 *  @param format the textures format (GL_RGB, GL_LUMINANCE, ...)
 */
unsigned int texman::add_texture(char* filename, GLint components, GLenum format) {
	// create a sdl surface and load the texture
	SDL_Surface* tex_surface = new SDL_Surface();
	tex_surface = IMG_LoadPNG_RW(SDL_RWFromFile(filename, "rb"));
	if(!tex_surface) {
		m->print(msg::MERROR, "texman.cpp", "add_texture(): error loading texture file \"%s\"!", filename);
		return 0;
	}

	// look if we already loaded this texture
	for(unsigned int i = 0; i < ctextures; i++) {
		if(strcmp(textures[i].filename, filename) == 0 &&
			textures[i].height == (unsigned int)tex_surface->h &&
			textures[i].width == (unsigned int)tex_surface->w) {
			// we already loaded the texture, so just return its number
			return textures[i].tex;
		}
	}

	// create a new texture element
	texture* new_textures = new texture[ctextures+1];
	for(unsigned int i = 0; i < ctextures; i++) {
		new_textures[i] = textures[i];
	}
	new_textures[ctextures].filename = filename;
	new_textures[ctextures].height = tex_surface->h;
	new_textures[ctextures].width = tex_surface->w;
	new_textures[ctextures].tex = 0;
	delete [] textures;
	textures = new_textures;

	// now create/generate an opengl texture and bind it
	glGenTextures(1, &texman::textures[ctextures].tex);
	glBindTexture(GL_TEXTURE_2D, texman::textures[ctextures].tex);

	// build mipmaps
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	gluBuild2DMipmaps(GL_TEXTURE_2D, components, tex_surface->w, tex_surface->h,
		format, GL_UNSIGNED_BYTE, tex_surface->pixels);

	// delete the sdl surface, b/c it isn't needed any more
	if(tex_surface) {
		SDL_FreeSurface(tex_surface);
	}

	// increment texture count
	ctextures++;

	return textures[ctextures-1].tex;
}

/*! returns the texture with the tex number num
 *  @param num the textures num we want to search for
 */
texman::texture* texman::get_texture(unsigned int num) {
	for(unsigned int i = 0; i < ctextures; i++) {
		if(textures[i].tex == num) {
			return &textures[i];
		}
	}
	m->print(msg::MERROR, "texman.cpp", "get_texture(): couldn't find texture #%u", num);
	return 0;
}
