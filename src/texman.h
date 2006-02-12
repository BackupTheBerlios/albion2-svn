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
#include "msg.h"
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
	texman(msg* m);
	~texman();

	unsigned int add_texture(const char* filename, GLint components = 3, GLenum format = GL_RGB);
	unsigned int add_cubemap_texture(const char** filenames, GLint components = 3, GLenum format = GL_RGB);
	texture* get_texture(unsigned int num);

protected:
	msg* m;

	struct texture {
		const char* filename;
		GLuint tex;
		unsigned int width;
		unsigned int height;
	};

	unsigned int ctextures;
	texture* textures;

};

#endif
