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
 
#ifndef __RENDER_TO_TEXTURE_H__
#define __RENDER_TO_TEXTURE_H__

#include <iostream>
#include <list>
#include <SDL/SDL.h>
#include "msg.h"
#include "core.h"
#include "gfx.h"
#include "extensions.h"
using namespace std;

#include "win_dll_export.h"

/*! @class rtt
 *  @brief render to texture class
 *  @author flo
 *  @todo more functions
 *  
 *  the render to texture class
 */

class A2E_API rtt
{
public:
	rtt(msg* m, core* c, gfx* g, ext* exts, unsigned int screen_width, unsigned int screen_height);
	~rtt();

	struct fbo {
		unsigned int fbo_id;
		unsigned int tex_id;
		unsigned int width;
		unsigned int height;
		bool depth;
		unsigned int depth_buffer;
		bool auto_mipmap;
	};

	enum TEXTURE_FILTERING {
		TF_POINT,
		TF_LINEAR,
		TF_BILINEAR,
		TF_TRILINEAR
	};

	rtt::fbo* add_buffer(unsigned int width, unsigned int height, TEXTURE_FILTERING filtering = TF_POINT, GLint wrap_s = GL_REPEAT, GLint wrap_t = GL_REPEAT, GLint internal_format = GL_RGBA8, GLenum format = GL_RGBA, GLenum type = GL_UNSIGNED_BYTE, bool depth = false);
	void delete_buffer(rtt::fbo* buffer);
	void start_draw(rtt::fbo* buffer);
	void stop_draw();
	void start_2d_draw();
	void stop_2d_draw();
	void clear();
	void copy_buffer(rtt::fbo* src_buffer, rtt::fbo* dest_buffer);
	void check_fbo();

protected:
	msg* m;
	core* c;
	gfx* g;
	ext* exts;

	list<fbo*> buffers;
	fbo* current_buffer;

	unsigned int filter[4];

	unsigned int screen_width;
	unsigned int screen_height;

};

#endif
