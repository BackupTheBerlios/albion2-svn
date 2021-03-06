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
#include <omp.h>
#include <cmath>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glext.h>

#ifdef WIN32
#include <GL/wglext.h>
#else
#include <GL/glx.h>
#endif

#include <string>
#include "msg.h"
#include "core.h"
#include "file_io.h"
#include "event.h"
#include "gfx.h"
#include "texman.h"
#include "extensions.h"
#include "lua.h"
#include "xml.h"
#include "gui_font.h"
#include "rtt.h"
#include "matrix4.h"
using namespace std;

#include "win_dll_export.h"

/*! @class engine
 *  @brief main engine
 *  @author flo
 *  @todo more functions
 *
 *  the main engine
 */

class A2E_API engine
{
public:
	engine(const char* datapath = "../data/");
	~engine();

	// graphic control functions
	void init(const char* ico = NULL);
	void init(bool console, unsigned int width = 640, unsigned int height = 400, unsigned int depth = 16, unsigned int zbuffer = 16, unsigned int stencil = 8, bool fullscreen = false, const char* ico = NULL);
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

	void set_position(float xpos, float ypos, float zpos);
	vertex3* get_position(); //! shouldn't be used from outside of the engine, use camera class function instead
	void set_rotation(float xrot, float yrot);
	vertex3* get_rotation(); //! shouldn't be used from outside of the engine, use camera class function instead

	void set_cursor_visible(bool state);
	bool get_cursor_visible();

	void set_fps_limit(unsigned int ms);
	unsigned int get_fps_limit();

	unsigned int get_init_mode();

	unsigned int get_thread_count();

	void set_data_path(const char* data_path = "../data/");
	string get_data_path();
	char* data_path(const char* str);

	unsigned int get_filtering();
	bool get_hdr();
	bool get_hdr_rgba16();

	// class return functions
	core* get_core();
	msg* get_msg();
	file_io* get_file_io();
	event* get_event();
	gfx* get_gfx();
	texman* get_texman();
	ext* get_ext();
	lua* get_lua();
	gui_font* get_gui_font();
	xml* get_xml();
	rtt* get_rtt();

    // the initialization mode is used to determine if we should load
    // or compute graphical stuff like textures or shaders
	enum INIT_MODE {
	    GRAPHICAL,
	    CONSOLE
	};

	struct server_data {
		unsigned short int port;
		unsigned int max_clients;
	};

	struct client_data {
		string server_name;
		unsigned short int port;
		unsigned short int lis_port;
		string client_name;
	};

	server_data* get_server_data();
	client_data* get_client_data();

	string* get_color_scheme();

	unsigned int get_shadow_type();

	matrix4* get_projection_matrix();
	matrix4* get_modelview_matrix();

protected:
	core* c;
	msg* m;
	file_io* f;
	event* e;
	gfx* g;
	texman* t;
	ext* exts;
	lua* l;
	xml* x;
	gui_font* gf;
	rtt* r;

	string datapath;
	string tmp_str;

	unsigned int width, height, depth, zbuffer, stencil, flags;
	bool fullscreen;
	SDL_Surface* screen;
	const SDL_VideoInfo* video_info;

	vertex3* position;
	vertex3* rotation;

	bool cursor_visible;

	unsigned int fps_limit;

	unsigned int mode;

	unsigned int key_repeat;

	unsigned int thread_count;

	server_data* server;
	client_data* client;

	string color_scheme;
	unsigned int shadow_type;

	string keyset;

	void load_ico(const char* ico);

	unsigned int filtering;
	bool hdr;
	bool hdr_rgba16;

	matrix4 projection_matrix;
	matrix4 modelview_matrix;

};

#endif
