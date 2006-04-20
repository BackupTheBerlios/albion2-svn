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
#include <sstream>
#include <string>
#include <vector>
#include <SDL/SDL.h>
#include <cstring>
#include "msg.h"
#include "core.h"
#include "xml.h"
using namespace std;

#include "win_dll_export.h"

/*! @class event
 *  @brief (sdl) event handler
 *  @author flo
 *  @todo more functions
 *  
 *  the event handling class
 */

class A2E_API event
{
public:
	event(const char* datapath, msg* m, xml* x);
	~event();

	void init(SDL_Event ievent);
	int is_event();
	SDL_Event get_event();
	void handle_events(unsigned int event_type);

	//! specifies currently active class
	enum ACTIVE_CLASS {
		NONE,		//!< enum no class
		GUI,		//!< enum gui class
		CAMERA		//!< enum camera class
	};

	void set_active(ACTIVE_CLASS active_class);

	struct gui_element {
		unsigned int id;
		unsigned int type;
		unsigned int num;
		bool is_drawn;
	};

	void set_active_type(unsigned int type);

	//! input types
	enum INPUT_TYPE {
		IT_LEFT,	//!< enum left arrow key
		IT_RIGHT,	//!< enum right arrow key
		IT_BACK,	//!< enum backspace key
		IT_DEL,		//!< enum delete key
		IT_HOME,	//!< enum home key
		IT_END		//!< enum end key
	};
	stringstream* get_buffer();

	// gui event stuff

	enum GEVENT_TYPE {
		BUTTON_PRESSED,
		COMBO_ITEM_SELECTED
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
	void set_pressed(unsigned int x, unsigned int y);

	struct a2e_key {
		unsigned int id;
		bool ignore;
		char key;
		char shift;
		char alt;
	};
	vector<a2e_key> keyset;
	void load_keyset(const char* language); // currently supports US and DE

	bool is_key_up();
	bool is_key_down();
	bool is_key_right();
	bool is_key_left();

	//! gets the mouses position (pnt)
	void get_mouse_pos(core::pnt* pos);

protected:
	SDL_Event event_handle;
	msg* m;
	xml* x;

	string datapath;

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
	//gui_element* active_element;
	unsigned int active_type;

	//! the input text buffer of a input box
	stringstream* buffer;

	bool shift;
	bool alt;


	char key;
	Uint8 *keys;

	ACTIVE_CLASS act_class;
};

#endif
