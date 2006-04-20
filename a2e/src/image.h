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
 
#ifndef __IMAGE_H__
#define __IMAGE_H__

#include <iostream>
#include <SDL/SDL_image.h>
#include "msg.h"
#include "engine.h"
#include <GL/gl.h>
#include <GL/glu.h>
using namespace std;

#include "win_dll_export.h"

/*! @class image
 *  @brief functions to display an image
 *  @author flo
 *  @todo more functions
 *  
 *  the image class
 */

class A2E_API image
{
public:
	image(engine* e);
	~image();

	void draw();
	void draw(unsigned int scale_x, unsigned int scale_y);
	void open_image(char* filename, GLint components = 3, GLenum format = GL_RGB);

	void set_position(unsigned int x, unsigned int y);
	void set_position(core::pnt* position);

	GLuint get_texture();
	void set_texture(GLuint tex);

	void set_scaling(bool state);
	bool get_scaling();

	core::pnt* get_position();

protected:
	msg* m;
	engine* e;
	texman* t;

	GLuint texture;

	bool alpha;

	core::pnt* position;
	unsigned int width;
	unsigned int heigth;

	bool scale;

};

#endif
