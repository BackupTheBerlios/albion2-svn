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
 
#ifndef __GUI_WINDOW_H__
#define __GUI_WINDOW_H__

#include <iostream>
#include <SDL/SDL.h>
#include "msg.h"
#include "core.h"
#include "gfx.h"
#include "engine.h"
#include "gui_text.h"
#include "gui_button.h"
#include "gui_object.h"
#include "gui_style.h"
using namespace std;

#include "win_dll_export.h"

/*! @class gui_window
 *  @brief gui window element functions
 *  @author flo
 *  @todo more functions
 *  
 *  the gui_window class
 */

class A2E_API gui_window : public gui_object
{
public:
	gui_window(engine* e, gui_style* gs);
	~gui_window();

	void draw();

	unsigned int get_lid();
	bool get_border();

	void set_lid(unsigned int id);
	void set_caption(const char* caption);
	void set_border(bool state);

	void change_position(int x, int y);

	bool is_moving();
	void set_moving(bool state);

	gui_button* get_exit_button_handler();
	void set_exit_button_handler(gui_button* ibutton);

	bool get_deleted();
	void set_deleted(bool state);

	bool get_bg();
	void set_bg(bool state);

	void set_redraw(bool state);
	bool get_redraw();

	void handle_window();

protected:
	msg* m;
	core* c;
	engine* e;
	gfx* g;
	event* evt;
	
	gui_button* exit_button_handler;

	//! window layer id
	unsigned int lid;
	//! window border draw flag
	bool border;

	bool bg;

	bool moving;

	bool deleted;

	bool redraw;

};

#endif
