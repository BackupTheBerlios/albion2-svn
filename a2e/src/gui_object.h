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
 
#ifndef __GUI_OBJECT_H__
#define __GUI_OBJECT_H__

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <SDL/SDL.h>
#include "msg.h"
#include "core.h"
#include "gfx.h"
#include "event.h"
#include "engine.h"
#include "image.h"
#include "gui_style.h"
#include "gui_text.h"
using namespace std;

#include "win_dll_export.h"

/*! @class gui_object
 *  @brief gui object element functions
 *  @author flo
 *  @todo -
 *  
 *  the gui_object class
 */

class A2E_API gui_object
{
public:
	gui_object(engine* e, gui_style* gs);
	~gui_object();
	
	void draw(int x, int y);
	void draw_object(int x, int y);


	// gui button element variables functions
	unsigned int get_id();
	void set_id(unsigned int id);

	gfx::rect* get_rectangle();
	void set_rectangle(gfx::rect* rectangle);

	void set_state(const char* state);
	string get_state();

	void set_text_handler(gui_text* text);
	gui_text* get_text_handler();

	void set_text(const char* text);
	string* get_text();

	void set_image(image* img);
	image* get_image();
	void set_image_texture(unsigned int tex);
	unsigned int get_image_texture();

	void set_type(const char* type);
	string* get_type();

	void set_redraw(bool state);
	bool get_redraw();

	void set_scissor(bool state);
	bool get_scissor();

	void set_tab(unsigned int id);
	unsigned int get_tab();

protected:
	msg* m;
	core* c;
	engine* e;
	gfx* g;
	gui_style* gs;

	// gui object element variables

	//! the object's id
	unsigned int id;
	//! the object's rectangle
	gfx::rect* rectangle;
	//! the object's state
	string state;
	//! the object's type (name)
	string type;
	//! the object's text handler
	gui_text* text_handler;
	//! the object's text
	string text;
	//! the object's image
	image* img;
	//! redraw flag
	bool redraw;
	//! scissor
	bool scissor;
	//! the object's tab
	unsigned int tab;

};

#endif
