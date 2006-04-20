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
 
#ifndef __CGUI_H__
#define __CGUI_H__

#ifdef WIN32
#include <windows.h>
#endif

#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <engine.h>
#include <msg.h>
#include <core.h>
#include <net.h>
#include <gfx.h>
#include <gui.h>
#include <event.h>
#include <camera.h>
#include <a2emodel.h>
#include <scene.h>
#include <ode.h>
#include <ode_object.h>
#include <light.h>
#include <shader.h>
#include <lua.h>
#include <xml.h>
#include <image.h>
#include "csystem.h"
#include "cnet.h"
using namespace std;

class cgui {
public:
	cgui(engine* e, gui* agui, csystem* cs, cnet* cn);
	~cgui();

	enum GUI_STATE {
		GS_MAIN,
		GS_GAME
	};

	void init();

	void run();
	void load_main_gui();
	void load_login_wnd();
	void load_options_wnd();
	void load_credits_wnd();

	void load_game_gui();

	void load_chat_wnd();
	void add_chat_msg(cnet::CHAT_TYPE type, const char* name, const char* msg);

protected:
	engine* e;
	gui* agui;
	msg* m;
	core* c;
	file_io* f;
	csystem* cs;
	cnet* cn;

	GUI_STATE gui_state;
	unsigned int font_size;
	image* bg_img;
	stringstream* buffer;
	gui_text* txt;

	/***********
	GUI ID Chart
	0 - system
	50 - open file dialog
	60 - msg box stuff
	100 - main gui
	200 - login
	300 - options
	400 - credits
	/// game windows ///
	1000 main ?
	1100 chat window
	***********/

	//gui_window* ofd_wnd;

	GUI_OBJ mmenu_id;
	gui_window* mmenu;
	gui_button* mm_login;
	gui_button* mm_options;
	gui_button* mm_credits;
	gui_button* mm_exit;

	GUI_OBJ mlogin_id;
	gui_window* mlogin;
	gui_button* ml_back;
	gui_button* ml_login;
	gui_input* ml_iname;
	gui_input* ml_ipw;
	gui_text* ml_name;
	gui_text* ml_pw;

	GUI_OBJ moptions_id;
	gui_window* moptions;
	gui_button* mo_back;
	gui_button* mo_save;

	GUI_OBJ mcredits_id;
	gui_window* mcredits;
	gui_button* mc_back;

	/// game windows ///
	GUI_OBJ gchat_id;
	gui_window* gchat;
	gui_button* gc_send;
	gui_button* gc_all;
	gui_button* gc_world;
	gui_button* gc_region;
	gui_button* gc_party;
	gui_input* gc_imsg;
	gui_list* gc_msg_box;

};

#endif
