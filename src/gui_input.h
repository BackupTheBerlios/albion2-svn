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
 
#ifndef __GUI_INPUT_H__
#define __GUI_INPUT_H__

#include <iostream>
#include <SDL/SDL.h>
#include "msg.h"
#include "core.h"
#include "gfx.h"
#include "event.h"
#include "engine.h"
#include "gui_text.h"
using namespace std;

#include "win_dll_export.h"

/*! @class gui_input
 *  @brief gui input box element functions
 *  @author flo
 *  @version 0.3.2
 *  @todo more functions
 *  
 *  the gui_input class
 */

class A2E_API gui_input
{
public:
	gui_input(engine* e);
	~gui_input();
	
	void draw(unsigned int x, unsigned int y);
	void set_text_handler(gui_text* itext);
	void set_blink_text_handler(gui_text* itext);
	gui_text* get_text_handler();
	gui_text* get_blink_text_handler();


	// gui input box element variables functions

	unsigned int get_id();
	gfx::rect* get_rectangle();
	char* get_text();
	unsigned int get_text_position();

	void set_id(unsigned int id);
	void set_rectangle(gfx::rect* rectangle);
	void set_text(char* text);
	void set_text_position(unsigned int position);

	void set_active(bool is_active);
	bool get_active();

	void set_notext();

protected:
	msg* m;
	core* c;
	engine* e;
	gfx* g;

	gui_text* text_handler;

	gui_text* blink_text_handler;
	unsigned int blink_time;
	bool is_in_rectangle;

	// gui input box element variables

	//! input box id
	unsigned int id;
	//! input box rectangle
	gfx::rect* rectangle;
	//! input box text
	char* text;
	//! the current text position (of the blink symbol)
	unsigned int text_pos;

	//! the current text length
	unsigned int text_length;

	// event handle stuff

	bool is_active;
};

#endif
