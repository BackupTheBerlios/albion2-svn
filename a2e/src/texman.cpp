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
	for(unsigned int i = 0; i < ctextures; i++) {
		glDeleteTextures(1, &textures[i].tex);
	}

	delete [] textures;
}

/*! adds a texture and returns its (internal?) number
 *  @param filename the textures file name
 *  @param components the textures components (number of colors)
 *  @param format the textures format (GL_RGB, GL_LUMINANCE, ...)
 */
unsigned int texman::add_texture(const char* filename, GLint components, GLenum format) {
	// create a sdl surface and load the texture
	SDL_Surface* tex_surface = IMG_LoadPNG_RW(SDL_RWFromFile(filename, "rb"));
	if(tex_surface == NULL) {
		m->print(msg::MERROR, "texman.cpp", "add_texture(): error loading texture file \"%s\"!", filename);
		return 0;
	}

	// look if we already loaded this texture
	for(unsigned int i = 0; i < ctextures; i++) {
		if(strcmp(textures[i].filename, filename) == 0 &&
			textures[i].height == (unsigned int)tex_surface->h &&
			textures[i].width == (unsigned int)tex_surface->w &&
			components == textures[i].components &&
			format == textures[i].format) {
			// we already loaded the texture, so just return its number
			SDL_FreeSurface(tex_surface);
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
	new_textures[ctextures].components = components;
	new_textures[ctextures].format = format;
	delete [] textures;
	textures = new_textures;

	// now create/generate an opengl texture and bind it
	glGenTextures(1, &texman::textures[ctextures].tex);
	glBindTexture(GL_TEXTURE_2D, texman::textures[ctextures].tex);

	// build mipmaps
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	gluBuild2DMipmaps(GL_TEXTURE_2D, components, tex_surface->w, tex_surface->h,
		format, GL_UNSIGNED_BYTE, tex_surface->pixels);

	// delete the sdl surface, b/c it isn't needed any more
	SDL_FreeSurface(tex_surface);
	//delete tex_surface;

	// increment texture count
	ctextures++;

	return textures[ctextures-1].tex;
}

/*! adds a cubemap texture and returns its (internal?) number
 *  @param filenames the textures file names
 *  @param components the textures components (number of colors)
 *  @param format the textures format (GL_RGB, GL_LUMINANCE, ...)
 */
unsigned int texman::add_cubemap_texture(const char** filenames, GLint components, GLenum format) {
	// create a sdl surface and load the texture
	SDL_Surface** tex_surface = new SDL_Surface*[6];
	for(unsigned int i = 0; i < 6; i++) {
		tex_surface[i] = new SDL_Surface();
		tex_surface[i] = IMG_LoadPNG_RW(SDL_RWFromFile(filenames[i], "rb"));
		if(!tex_surface[i]) {
			m->print(msg::MERROR, "texman.cpp", "add_cubemap_texture(): error loading texture file \"%s\"!", filenames[i]);
			return 0;
		}
	}

	// look if we already loaded this texture
	/*for(unsigned int i = 0; i < ctextures; i++) {
		if(strcmp(textures[i].filename, filename) == 0 &&
			textures[i].height == (unsigned int)tex_surface[i]->h &&
			textures[i].width == (unsigned int)tex_surface[i]->w) {
			// we already loaded the texture, so just return its number
			SDL_FreeSurface(tex_surface);
			return textures[i].tex;
		}
	}*/

	// create a new texture element
	texture* new_textures = new texture[ctextures+1];
	for(unsigned int i = 0; i < ctextures; i++) {
		new_textures[i] = textures[i];
	}
	new_textures[ctextures].filename = filenames[0];
	new_textures[ctextures].height = tex_surface[0]->h;
	new_textures[ctextures].width = tex_surface[0]->w;
	new_textures[ctextures].tex = 0;
	delete [] textures;
	textures = new_textures;

	// now create/generate an opengl texture and bind it
	glGenTextures(1, &texman::textures[ctextures].tex);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texman::textures[ctextures].tex);

	// build mipmaps
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_REPEAT);

	GLenum cmap[6] = { GL_TEXTURE_CUBE_MAP_POSITIVE_X, GL_TEXTURE_CUBE_MAP_POSITIVE_Y, GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_X, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z };

	for(unsigned int i = 0; i < 6; i++) {
		gluBuild2DMipmaps(cmap[i], components, tex_surface[i]->w, tex_surface[i]->h,
			format, GL_UNSIGNED_BYTE, tex_surface[i]->pixels);
	}

	// delete the sdl surface, b/c it isn't needed any more
	for(unsigned int i = 0; i < 6; i++) {
		if(tex_surface[i]) {
			SDL_FreeSurface(tex_surface[i]);
		}
	}

	// increment texture count
	ctextures++;

	return textures[ctextures-1].tex;
	return 0;
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
