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

#define MAX_ELEMENTS 128

#include <iostream>
#include <SDL.h>
#include "msg.h"
#include "core.h"
#include "net.h"
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

#include "win_dll_export.h"

/*! @class gui
 *  @brief graphical user interface functions
 *  @author flo
 *  @version 0.5
 *  @date 2005/02/23
 *  @todo more functions
 *  
 *  the gui class
 */

class A2E_API gui
{
public:
	gui();
	~gui();

	struct gui_element {
		unsigned int id;
		unsigned int type;
		unsigned int num;
		unsigned int wid;
		bool is_drawn;
	};

	void init(engine &iengine, event &ievent);
	void draw();

	bool delete_element(unsigned int id);

	gui_button* add_button(gfx::rect* rectangle, unsigned int icon, unsigned int id, char* text, unsigned int wid = 0);
	gui_button* add_button(gfx::rect* rectangle, unsigned int id, char* text, unsigned int wid = 0);
	gui_text* add_text(char* font_name, unsigned int font_size, char* text,
				   unsigned int color, core::pnt* point, unsigned int id, unsigned int wid = 0);	
	gui_input* add_input_box(gfx::rect* rectangle, unsigned int id, char* text, unsigned int wid = 0);
	gui_list* add_list_box(gfx::rect* rectangle, unsigned int id, char* text, unsigned int wid = 0);
	gui_vbar* add_vbar(gfx::rect* rectangle, unsigned int id, unsigned int wid = 0);
	gui_check* add_check_box(gfx::rect* rectangle, unsigned int id, char* text, unsigned int wid = 0);
	gui_window* add_window(gfx::rect* rectangle, unsigned int id, char* caption, bool border = true);

	void switch_input_text(char* input_text, gui_input* input_box);

	SDL_Surface* get_gui_surface();

	gui_element* get_active_element();
	void set_active_element(gui_element* active_element);
	void set_active_element(unsigned int id);

protected:
	msg m;
	core c;
	gfx g;

	event* event_handler;
	engine* engine_handler;
	
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
	gui_element* gui_elements;
	//! current amount of gui elements
	unsigned int celements;

	//! gui windows
	gui_window* gui_windows[MAX_ELEMENTS];
	//! current amount of gui window elements
	unsigned int cwindows;

	//! gui buttons
	gui_button* gui_buttons[MAX_ELEMENTS];
	//! current amount of gui button elements
	unsigned int cbuttons;

	//! gui texts
	gui_text* gui_texts[MAX_ELEMENTS];
	//! current amount of gui text elements
	unsigned int ctexts;

	//! gui input boxes
	gui_input* gui_input_boxes[MAX_ELEMENTS];
	//! current amount of gui input box elements
	unsigned int cinput_boxes;

	//! gui list boxes
	gui_list* gui_list_boxes[MAX_ELEMENTS];
	//! current amount of gui input box elements
	unsigned int clist_boxes;

	//! gui vertical bars
	gui_vbar* gui_vbars[MAX_ELEMENTS];
	//! current amount of gui vertical bar elements
	unsigned int cvbars;

	//! gui check boxes
	gui_check* gui_check_boxes[MAX_ELEMENTS];
	//! current amount of gui check box elements
	unsigned int ccboxes;

	//! current active gui element
	gui_element* active_element;

	//! current active window (id)
	unsigned int awid;

	core::pnt* p;
	gfx::rect* r;
	char* input_text;
	char* ib_text;
	unsigned int ib_text_length;
	char set_text[1028];
	gui_window* main_window;
};

#endif
