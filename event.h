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
 
#ifndef __EVENT_H__
#define __EVENT_H__

#include <iostream>
#include <SDL.h>
#include <string.h>
#include "msg.h"
#include "core.h"
#include "engine.h"
using namespace std;

#include "win_dll_export.h"

/*! @class event
 *  @brief (sdl) event handler
 *  @author flo
 *  @version 0.3.1
 *  @date 2005/02/03
 *  @todo more functions
 *  
 *  the event handling class
 */

class A2E_API event
{
public:
	event();
	~event();

	void init(SDL_Event ievent);
	int is_event();
	SDL_Event get_event();
	void handle_events(unsigned int event_type);

	//! specifies currently active class
	enum ACTIVE_CLASS {
		NONE,		//!< @enum no class
		GUI,		//!< @enum gui class
		CAMERA		//!< @enum camera class
	};

	void set_active(ACTIVE_CLASS active_class);

	struct gui_element {
		unsigned int id;
		unsigned int type;
		unsigned int num;
		bool is_drawn;
	};

	gui_element* get_active_element();
	void set_active_element(gui_element* active_element);

	void get_input_text(char* tmp_text);
	//! input types
	enum ITYPE {
		LEFT = 1,	//!< @enum left arrow key
		RIGHT,		//!< @enum right arrow key
		BACK,		//!< @enum backspace key
		DEL,		//!< @enum delete key
		HOME,		//!< @enum home key
		END			//!< @enum end key
	};

	// gui event stuff

	enum GEVENT_TYPE {
		BUTTON_PRESSED
	};

	struct gevent {
		GEVENT_TYPE type;
		unsigned int id;
	};

	bool is_gui_event();
	gevent get_gui_event();
	void add_gui_event(GEVENT_TYPE event_type, unsigned int id);

	unsigned int cgui_event;
	gevent* gui_event_stack;


	unsigned int get_lm_pressed_x();
	unsigned int get_lm_pressed_y();
	unsigned int get_lm_last_pressed_x();
	unsigned int get_lm_last_pressed_y();
	//! is needed to fake a click ;)
	void set_last_pressed(unsigned int x, unsigned int y);

	//! the keyboard input layout
	enum IKEY_LAYOUT {
		US,	//!< @enum us keyboard layout
		DE	//!< @enum de keyboard layout
	};
	void set_keyboard_layout(IKEY_LAYOUT layout);

	bool is_key_up();
	bool is_key_down();
	bool is_key_right();
	bool is_key_left();

	//! gets the mouses position (pnt)
	void get_mouse_pos(gfx::pnt* pos);

protected:
	msg m;
	SDL_Event event_handle;

	//! left mouse button pressed (x coordinate)
	unsigned int lm_pressed_x;
	//! left mouse button pressed (y coordinate)
	unsigned int lm_pressed_y;
	//! left mouse button pressed (x coordinate)
	unsigned int lm_last_pressed_x;
	//! left mouse button pressed (y coordinate)
	unsigned int lm_last_pressed_y;
	//! key up pressed
	bool key_up;
	//! key down pressed
	bool key_down;
	//! key right pressed
	bool key_right;
	//! key left pressed
	bool key_left;

	//! current active gui element
	gui_element* active_element;

	//! the input text of a input box
	char input_text[512];

	bool shift;
	bool alt;

	IKEY_LAYOUT keyboard_layout;


	char key;
	Uint8 *keys;
	char tmp_text[4];

	ACTIVE_CLASS act_class;
};

#endif
