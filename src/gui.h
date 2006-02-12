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
 
#ifndef __GUI_H__
#define __GUI_H__

#include <iostream>
#include <list>
#include <SDL/SDL.h>
#include "msg.h"
#include "core.h"
#include "engine.h"
#include "gfx.h"
#include "event.h"
#include "gui_text.h"
#include "gui_button.h"
#include "gui_input.h"
#include "gui_list.h"
#include "gui_vbar.h"
#include "gui_check.h"
#include "gui_window.h"
using namespace std;
typedef unsigned int GUI_OBJ;

#include "win_dll_export.h"

/*! @class gui
 *  @brief graphical user interface functions
 *  @author flo
 *  @todo needs some more restructuring
 *  
 *  the gui class
 */

class A2E_API gui
{
public:
	gui(engine* e);
	~gui();

	struct gui_element {
		unsigned int id;
		unsigned int type;
		unsigned int num;
		unsigned int wid;
		bool is_drawn;
	};

	void init();
	void draw();

	bool delete_element(unsigned int id);

	GUI_OBJ add_button(gfx::rect* rectangle, unsigned int icon, unsigned int id, char* text, unsigned int wid = 0);
	GUI_OBJ add_button(gfx::rect* rectangle, unsigned int id, char* text, unsigned int wid = 0);
	GUI_OBJ add_text(char* font_name, unsigned int font_size, char* text,
				   unsigned int color, core::pnt* point, unsigned int id, unsigned int wid = 0);	
	GUI_OBJ add_input_box(gfx::rect* rectangle, unsigned int id, char* text, unsigned int wid = 0);
	GUI_OBJ add_list_box(gfx::rect* rectangle, unsigned int id, unsigned int wid = 0);
	GUI_OBJ add_vbar(gfx::rect* rectangle, unsigned int id, unsigned int wid = 0);
	GUI_OBJ add_check_box(gfx::rect* rectangle, unsigned int id, char* text, unsigned int wid = 0);
	GUI_OBJ add_window(gfx::rect* rectangle, unsigned int id, char* caption, bool border = true);

	void switch_input_text(char* input_text, list<gui_input>::reference input_box);

	SDL_Surface* get_gui_surface();

	list<gui_element>::iterator get_active_element();
	void set_active_element(list<gui_element>::iterator active_element);
	void set_active_element(unsigned int id);

	list<gui_button>::iterator get_button_iter(unsigned int id);
	list<gui_input>::iterator get_input_iter(unsigned int id);
	list<gui_text>::iterator get_text_iter(unsigned int id);
	list<gui_list>::iterator get_list_iter(unsigned int id);
	list<gui_vbar>::iterator get_vbar_iter(unsigned int id);
	list<gui_check>::iterator get_check_iter(unsigned int id);
	list<gui_window>::iterator get_window_iter(unsigned int id);

	gui_button* get_button(unsigned int id);
	gui_input* get_input(unsigned int id);
	gui_text* get_text(unsigned int id);
	gui_list* get_list(unsigned int id);
	gui_vbar* get_vbar(unsigned int id);
	gui_check* get_check(unsigned int id);
	gui_window* get_window(unsigned int id);

	bool exist(unsigned int id);

protected:
	msg* m;
	core* c;
	event* evt;
	engine* e;
	gfx* g;

	SDL_Surface* gui_surface;

	//! gui element types
	enum GTYPE {
		BUTTON,	//!< enum button type
		INPUT,	//!< enum input box type
		TEXT,	//!< enum static text type
		EMPTY,	//!< enum empty type
		LIST,	//!< enum list box type
		VBAR,	//!< enum vertical bar type
		CHECK,	//!< enum check box type
		WINDOW	//!< enum window type
	};

	//! gui elements
	list<gui_element> gui_elements;

	//! current active gui element
	list<gui_element>::iterator active_element;

	//! gui buttons
	list<gui_button> gui_buttons;

	//! gui texts
	list<gui_text> gui_texts;

	//! gui input boxes
	list<gui_input> gui_input_boxes;

	//! gui list boxes
	list<gui_list> gui_list_boxes;

	//! gui vertical bars
	list<gui_vbar> gui_vbars;

	//! gui check boxes
	list<gui_check> gui_check_boxes;

	//! gui windows
	list<gui_window> gui_windows;

	//! list of all window ids (the id at the end is the id of the "most"
	//! front window and the one in the beginning the "most" far window)
	list<unsigned int> wnd_layers;

	core::pnt* p;
	gfx::rect* r;
	char* input_text;
	char* ib_text;
	unsigned int ib_text_length;
	char set_text[1028];
	GUI_OBJ main_window;
};

#endif
