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

#ifndef __ENGINE_H__
#define __ENGINE_H__

#ifdef WIN32
#include <windows.h>
#include <winnt.h>
#endif

#include <iostream>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <cmath>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glext.h>

#ifdef WIN32
#include <GL/wglext.h>
#else
#include <GL/glx.h>
#endif

#include "msg.h"
#include "core.h"
#include "gui_style.h"
#include "file_io.h"
#include "event.h"
#include "gfx.h"
#include "texman.h"
#include "extensions.h"
#include "lua.h"
#include "xml.h"
using namespace std;

#include "win_dll_export.h"

/*! @class engine
 *  @brief main engine
 *  @author flo
 *  @author laxity
 *  @todo more functions
 *
 *  the main engine
 */

class A2E_API engine
{
public:
	engine();
	~engine();

	// graphic control functions
	void init();
	void init(bool console, unsigned int width = 640, unsigned int height = 400, unsigned int depth = 16, bool fullscreen = false);
	void set_width(unsigned int width);
	void set_height(unsigned int height);
	void start_draw();
	void stop_draw();
	void start_2d_draw();
	void stop_2d_draw();
	SDL_Surface* get_screen();
	bool draw_gl_scene();
	bool init_gl();
	bool resize_window();

	// miscellaneous control functions
	void set_caption(char* caption);
	char* get_caption();

	void set_color_scheme(gui_style::COLOR_SCHEME scheme);

	void set_position(float xpos, float ypos, float zpos);
	vertex3* get_position();

	void set_cursor_visible(bool state);
	bool get_cursor_visible();

	void set_fps_limit(unsigned int ms);
	unsigned int get_fps_limit();

	unsigned int get_init_mode();

	// class return functions
	core* get_core();
	msg* get_msg();
	file_io* get_file_io();
	event* get_event();
	gui_style* get_gui_style();
	gfx* get_gfx();
	texman* get_texman();
	ext* get_ext();
	lua* get_lua();

    // the initialization mode is used to determine if we should load
    // or compute graphical stuff like textures or shaders
	enum INIT_MODE {
	    GRAPHICAL,
	    CONSOLE
	};

protected:
	core* c;
	msg* m;
	gui_style* gstyle;
	file_io* f;
	event* e;
	gfx* g;
	texman* t;
	ext* exts;
	lua* l;
	xml* x;

	unsigned int width, height, depth, flags;
	bool fullscreen;
	SDL_Surface* screen;
	const SDL_VideoInfo* video_info;

	vertex3* position;

	bool cursor_visible;

	unsigned int fps_limit;

	unsigned int mode;

	unsigned int key_repeat;

	gui_style::COLOR_SCHEME color_scheme;
};

#endif
