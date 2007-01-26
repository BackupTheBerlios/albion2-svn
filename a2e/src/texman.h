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
 
#ifndef __TEXMAN_H__
#define __TEXMAN_H__

#ifdef WIN32
#include <windows.h>
#endif

#include <iostream>
#include <string>
#include <sstream>
#include "msg.h"
#include "file_io.h"
#include "vertex3.h"
#include <SDL/SDL_image.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glext.h>
using namespace std;

#include "win_dll_export.h"

/*! @class texman
 *  @brief texture management routines
 *  @author flo
 *  @todo -
 *  
 *  the texman class
 */

class A2E_API texman
{
protected:
	struct texture;
public:
	texman(msg* m, file_io* f);
	~texman();

	enum TEXTURE_FILTERING {
		TF_POINT,
		TF_LINEAR,
		TF_BILINEAR,
		TF_TRILINEAR,
		TF_AUTOMATIC
	};

	unsigned int add_texture(void* pixel_data, unsigned int width, unsigned int height, GLint components = 3, GLenum format = GL_RGB, TEXTURE_FILTERING filtering = TF_AUTOMATIC, GLint wrap_s = GL_REPEAT, GLint wrap_t = GL_REPEAT, GLenum type = GL_UNSIGNED_BYTE, const char* filename = "");
	unsigned int add_texture(const char* filename, GLint components = 3, GLenum format = GL_RGB, TEXTURE_FILTERING filtering = TF_AUTOMATIC, GLint wrap_s = GL_REPEAT, GLint wrap_t = GL_REPEAT, GLenum type = GL_UNSIGNED_BYTE);

	unsigned int add_cubemap_texture(const char** filenames, GLint components = 3, GLenum format = GL_RGB, TEXTURE_FILTERING filtering = TF_AUTOMATIC, GLint wrap_s = GL_REPEAT, GLint wrap_t = GL_REPEAT, GLenum type = GL_UNSIGNED_BYTE);
	unsigned int add_cubemap_texture(const char* filename, GLint components = 3, GLenum format = GL_RGB, TEXTURE_FILTERING filtering = TF_AUTOMATIC, GLint wrap_s = GL_REPEAT, GLint wrap_t = GL_REPEAT, GLenum type = GL_UNSIGNED_BYTE);
	unsigned int add_cubemap_texture(void** pixel_data, unsigned int width, unsigned int height, GLint components = 3, GLenum format = GL_RGB, TEXTURE_FILTERING filtering = TF_AUTOMATIC, GLint wrap_s = GL_REPEAT, GLint wrap_t = GL_REPEAT, GLenum type = GL_UNSIGNED_BYTE);

	texture* get_texture(unsigned int num);

	void set_filtering(unsigned int filtering);

protected:
	msg* m;
	file_io* f;

	struct texture {
		string filename;
		GLuint tex;
		unsigned int width;
		unsigned int height;
		GLint components;
		GLenum format;
		TEXTURE_FILTERING filtering;
		GLint wrap_s;
		GLint wrap_t;
		GLenum type;
		float max_value;
	};

	unsigned int ctextures;
	texture* textures;

	unsigned int filter[4];

	TEXTURE_FILTERING standard_filtering;

	SDL_Surface* cur_texture;

};

#endif
