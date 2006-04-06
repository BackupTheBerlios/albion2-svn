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
 
#ifndef __GUI_VBAR_H__
#define __GUI_VBAR_H__

#include <iostream>
#include <SDL/SDL.h>
#include "msg.h"
#include "core.h"
#include "gfx.h"
#include "engine.h"
#include "gui_button.h"
using namespace std;

#include "win_dll_export.h"

/*! @class gui_vbar
 *  @brief gui vertical bar element functions
 *  @author flo
 *  @todo more functions
 *  
 *  the gui_vbar class
 */

class A2E_API gui_vbar
{
public:
	gui_vbar(engine* e);
	~gui_vbar();
	
	void draw();
	void draw(unsigned int x, unsigned int y);


	// gui vertical bar element variables functions

	unsigned int get_id();
	gfx::rect* get_rectangle();
	unsigned int get_max_lines();
	unsigned int get_shown_lines();
	unsigned int get_position();
	bool get_active();
	//core::pnt* get_last_point();
	core::pnt* get_new_point();
	bool get_slider_active();

	void set_id(unsigned int id);
	void set_rectangle(gfx::rect* rectangle);
	void set_max_lines(unsigned int max_lines);
	void set_shown_lines(unsigned int shown_lines);
	void set_position(unsigned int position);
	void set_active(bool is_active);
	//void set_last_point(core::pnt* last_point);
	void set_new_point(core::pnt* new_point);
	void set_slider_active(bool state);

	void set_up_button_handler(gui_button* ibutton);
	void set_down_button_handler(gui_button* ibutton);
	gui_button* get_up_button_handler();
	gui_button* get_down_button_handler();

protected:
	msg* m;
	core* c;
	engine* e;
	gfx* g;

	gui_button* up_button_handler;
	gui_button* down_button_handler;

	unsigned int px_per_item;

	// gui vertical bar element variables

	//! vertical bar id
	unsigned int id;
	//! vertical bars rectangle
	gfx::rect* rectangle;
	//! vertical bar max lines
	unsigned int max_lines;
	//! vertical bar shown lines
	unsigned int shown_lines;
	//! vertical bar position
	unsigned int position;
	//! bool if we draw the slider
	bool slider_active;

	// event handle stuff

	bool is_active;
	core::pnt* last_point;
	core::pnt* new_point;
};

#endif
