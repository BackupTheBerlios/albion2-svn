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

#include "event.h"

/*! there is no function currently
 */
event::event(const char* datapath, msg* m, xml* x) {
	event::datapath = datapath;
	event::gui_event_stack = NULL;

	buffer = new stringstream(stringstream::in | stringstream::out);

	event::lm_pressed_x = 0;
	event::lm_pressed_y = 0;
	event::lm_last_pressed_x = 0;
	event::lm_last_pressed_y = 0;

	event::m = m;
	event::x = x;

	event::gui_event_stack = new event::gevent[512];
	cgui_event = 0;

	shift = false;
	alt = false;

	load_keyset("US");

	event::set_active(event::CAMERA);

	key_up = false;
	key_down = false;
	key_right = false;
	key_left = false;
}

/*! there is no function currently
 */
event::~event() {
	cout << "DEBUG: " << "event.cpp" << " freeing event stuff" << endl;

	if(event::gui_event_stack != NULL) {
		delete [] event::gui_event_stack;
	}

	delete buffer;

	keyset.clear();

	cout << "DEBUG: " << "event.cpp" << " event stuff freed" << endl;
}

/*! initializes the event class and sets an sdl_event handler
 *  @param ievent the sdl_event we want to handle
 */
void event::init(SDL_Event ievent) {
	event::event_handle = ievent;
}

/*! returns 1 if the are any sdl events, otherwise it will return 0
 */
int event::is_event() {
	return SDL_PollEvent(&event_handle);
}

/*! returns the sdl_event handle
 */
SDL_Event event::get_event() {
	return event_handle;
}

/*! returns the left mouse button pressed x cord
 */
unsigned int event::get_lm_pressed_x() {
	return event::lm_pressed_x;
}

/*! returns the left mouse button pressed y cord
 */
unsigned int event::get_lm_pressed_y() {
	return event::lm_pressed_y;
}

/*! returns the last left mouse button pressed x cord
 */
unsigned int event::get_lm_last_pressed_x() {
	return event::lm_last_pressed_x;
}

/*! returns the last left mouse button pressed y cord
 */
unsigned int event::get_lm_last_pressed_y() {
	return event::lm_last_pressed_y;
}

/*! handles the sdl_events
 *  @param event_type the event type, we want to handle
 */
void event::handle_events(unsigned int event_type) {
	// get events
	switch(event_type) {
		// internal engine event handler
		case SDL_MOUSEBUTTONDOWN:
			switch(event::get_event().button.button) {
				case SDL_BUTTON_LEFT:
					if(event::get_event().button.state == SDL_PRESSED) {
						event::lm_pressed_x = event::get_event().button.x;
						event::lm_pressed_y = event::get_event().button.y;
						event::lm_last_pressed_x = event::lm_pressed_x;
						event::lm_last_pressed_y = event::lm_pressed_y;
					}
					break;
			}
			break;
		case SDL_MOUSEBUTTONUP:
			switch(event::get_event().button.button) {
				case SDL_BUTTON_LEFT:
					if(event::get_event().button.state == SDL_RELEASED) {
						event::lm_pressed_x = 0;
						event::lm_pressed_y = 0;
					}
					break;
			}
			break;
		case SDL_KEYUP:
			switch(act_class) {
				case event::NONE:
					event::set_active(event::CAMERA);
					break;
				case event::CAMERA:
					switch(event::get_event().key.keysym.sym) {
						case SDLK_UP:
							key_up = false;
							break;
						case SDLK_DOWN:
							key_down = false;
							break;
						case SDLK_RIGHT:
							key_right = false;
							break;
						case SDLK_LEFT:
							key_left = false;
							break;
						default:
							break;
					}
					break;
				default:
					break;
			}
			break;
		case SDL_KEYDOWN:
			switch(act_class) {
				case event::NONE:
					event::set_active(event::CAMERA);
					break;
				case event::CAMERA:
					switch(event::get_event().key.keysym.sym) {
						case SDLK_UP:
							key_up = true;
							break;
						case SDLK_DOWN:
							key_down = true;
							break;
						case SDLK_RIGHT:
							key_right = true;
							break;
						case SDLK_LEFT:
							key_left = true;
							break;
						default:
							break;
					}
					break;
				case event::GUI: {
					if(active_type != 1) break;

					keys = SDL_GetKeyState(NULL);
					if(keys[SDLK_RSHIFT] == SDL_PRESSED
						|| keys[SDLK_LSHIFT] == SDL_PRESSED) {
						shift = true;
					}
					else if(keys[SDLK_RALT] == SDL_PRESSED
						|| keys[SDLK_LALT] == SDL_PRESSED) {
						alt = true;
					}

					switch(event::get_event().key.keysym.sym) {
						case SDLK_LEFT:
							*buffer << (char)event::IT_LEFT;
							break;
						case SDLK_RIGHT:
							*buffer << (char)event::IT_RIGHT;
							break;
						case SDLK_BACKSPACE:
							*buffer << (char)event::IT_BACK;
							break;
						case SDLK_DELETE:
							*buffer << (char)event::IT_DEL;
							break;
						case SDLK_HOME:
							*buffer << (char)event::IT_HOME;
							break;
						case SDLK_END:
							*buffer << (char)event::IT_END;
							break;
						default:
							break;
					}

					for(vector<a2e_key>::iterator iter = keyset.begin(); iter != keyset.end(); iter++) {
						if(iter->id == event::get_event().key.keysym.sym && !iter->ignore) {
							if(shift) {
								*buffer << iter->shift;
							}
							else if(alt) {
								*buffer << iter->alt;
							}
							else {
								*buffer << iter->key;
							}
						}
					}

					shift = false;
					alt = false;
				}
				break;
				default:
					break;
		}
	}
}

/*! returns true if there is any gui event in stack
 */
bool event::is_gui_event() {
	// if stack counter is equal to 0, then there are no gui events
	if(cgui_event == 0) {
		return false;
	}
	// else there is a gui event - decrement stack pointer
	else {
		cgui_event--;
		return true;
	}
}

/*! returns the current gui event
 */
event::gevent event::get_gui_event() {
	return gui_event_stack[cgui_event+1];
}

/*! adds a gui event to the stack
 *  @param event_type the event type we want to add
 *  @param id the id of the gui element
 */
void event::add_gui_event(GEVENT_TYPE event_type, unsigned int id) {
	cgui_event++;
	gui_event_stack[cgui_event].type = event_type;
	gui_event_stack[cgui_event].id = id;
}

/*! sets the currently active gui element type
 *  @param type the active gui element's type
 */
void event::set_active_type(unsigned int type) {
	event::active_type = type;
}

/*! gets the current input text (for any input box) and writes it to tmp_text
 *  @param tmp_text pointer to text where the new text should be written to
 */
void event::get_buffer(char* tmp_text) {
	strcpy(tmp_text, buffer->str().c_str());
	buffer->clear();
	buffer->str("");
}

/*! sets the position of where clicked the last time
 *  @param x the x coordinate
 *  @param y the y coordinate
 */
void event::set_last_pressed(unsigned int x, unsigned int y) {
	event::lm_last_pressed_x = x;
	event::lm_last_pressed_y = y;
}

/*! sets the position of a current click
 *  @param x the x coordinate
 *  @param y the y coordinate
 */
void event::set_pressed(unsigned int x, unsigned int y) {
	event::lm_pressed_x = x;
	event::lm_pressed_y = y;
}

/*! sets the active class (gui, event or none - for internal usage only!)
 *! needed for sending the keyboard or mouse input to the right class (handler)
 *  @param active_class the new active_class
 */
void event::set_active(ACTIVE_CLASS active_class) {
	event::act_class = active_class;
}

/*! returns true if the up arrow key is pressed
 */
bool event::is_key_up() {
	return key_up;
}

/*! returns true if the down arrow key is pressed
 */
bool event::is_key_down() {
	return key_down;
}

/*! returns true if the right arrow key is pressed
 */
bool event::is_key_right() {
	return key_right;
}

/*! returns true if the left arrow key is pressed
 */
bool event::is_key_left() {
	return key_left;
}

/*! gets the mouses position (pnt)
 */
void event::get_mouse_pos(core::pnt* pos) {
	SDL_GetMouseState((int*)&pos->x, (int*)&pos->y);
}

/*! loads the keyset
 */
void event::load_keyset(const char* language) {
	if(!x->open((char*)string(datapath + "keyset.xml").c_str())) {
		m->print(msg::MERROR, "event.cpp", "load_keyset(): can't open keyset.xml!");
		return;
	}

	keyset.clear();
	while(x->process()) {
		if(strcmp(x->get_node_name(), "keyset") == 0) {
		}
		else if(strcmp(x->get_node_name(), "set") == 0) {
			if(strcmp(x->get_attribute("language"), language) != 0) {
				while(x->process() && strcmp(x->get_node_name(), "set") != 0) {
				}
			}
		}
		else if(strcmp(x->get_node_name(), "key") == 0) {
			keyset.push_back(*new a2e_key());

			if(x->get_attribute("ignore") != NULL) {
				keyset.back().id = (unsigned int)atoi(x->get_attribute("ignore"));
				keyset.back().ignore = true;
			}

			if(x->get_attribute("id") != NULL) {
				keyset.back().id = (unsigned int)atoi(x->get_attribute("id"));
			}

			if(x->get_attribute("tokey") != NULL) {
				char* c = x->get_attribute("tokey");
				keyset.back().key = strlen(c) <= 1 ? c[0] : (char)atoi(x->get_attribute("tokey"));
			}

			if(x->get_attribute("shift") != NULL) {
				char* c = x->get_attribute("shift");
				keyset.back().shift = strlen(c) <= 1 ? c[0] : (char)atoi(x->get_attribute("shift"));
			}

			if(x->get_attribute("alt") != NULL) {
				char* c = x->get_attribute("alt");
				keyset.back().alt = strlen(c) <= 1 ? c[0] : (char)atoi(x->get_attribute("alt"));
			}
		}
	}
	x->close();

	/*for(vector<a2e_key>::iterator iter = keyset.begin(); iter != keyset.end(); iter++) {
		if(!iter->ignore) {
			cout << iter->id << " to " << iter->key << " / " << iter->shift << " / " << iter->alt << endl;
		}
		else {
			cout << "ignore " << iter->id << endl;
		}
	}*/
}
