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
event::event() {
}

/*! there is no function currently
 */
event::~event() {
	cout << "DEBUG: " << "event.cpp" << "freeing event stuff" << endl;

	if(event::gui_event_stack) {
		delete event::gui_event_stack;
	}

	cout << "DEBUG: " << "event.cpp" << "event stuff freed" << endl;
}

/*! initializes the event class and sets an sdl_event handler
 *  @param ievent the sdl_event we want to handle
 */
void event::init(SDL_Event ievent) {
	event::event_handle = ievent;

	event::gui_event_stack = new event::gevent[512];
	cgui_event = 0;

	for(int i = 0; i < 512; i++) {
		event::input_text[i] = 0;
	}

	shift = false;
	alt = false;

	event::keyboard_layout = event::US;

	event::set_active(event::CAMERA);

	key_up = false;
	key_down = false;
	key_right = false;
	key_left = false;
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
	switch(event_type)
	{
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
				case event::GUI:
					switch(active_element->type) {
						// input box
						case 1: {
							for(int i = 0; i < 4; i++) {
								tmp_text[i] = 0;
							}
							switch(event::get_event().key.keysym.sym) {
								case SDLK_LEFT:
									sprintf(tmp_text, "%c", event::LEFT);
									strcat(input_text, tmp_text);
									break;
								case SDLK_RIGHT:
									sprintf(tmp_text, "%c", event::RIGHT);
									strcat(input_text, tmp_text);
									break;
								case SDLK_BACKSPACE:
									sprintf(tmp_text, "%c", event::BACK);
									strcat(input_text, tmp_text);
									break;
								case SDLK_SPACE:
									strcat(input_text, " ");
									break;
								case SDLK_DELETE:
									sprintf(tmp_text, "%c", event::DEL);
									strcat(input_text, tmp_text);
									break;
								case SDLK_HOME:
									sprintf(tmp_text, "%c", event::HOME);
									strcat(input_text, tmp_text);
									break;
								case SDLK_END:
									sprintf(tmp_text, "%c", event::END);
									strcat(input_text, tmp_text);
									break;
								case SDLK_TAB:
								case SDLK_CLEAR:
								case SDLK_RETURN:
								case SDLK_PAUSE:
								case SDLK_ESCAPE:
								case SDLK_EXCLAIM:
								case SDLK_QUOTEDBL:
								case SDLK_HASH:
								case SDLK_DOLLAR:
								case SDLK_AMPERSAND:
								case SDLK_LEFTPAREN:
								case SDLK_RIGHTPAREN:
								case SDLK_ASTERISK:
								case SDLK_PLUS:
								case SDLK_COLON:
								case SDLK_LESS:
								case SDLK_GREATER:
								case SDLK_QUESTION:
								case SDLK_AT:
								case SDLK_CARET:
								case SDLK_UNDERSCORE:
								case SDLK_KP_ENTER:
								case SDLK_KP_EQUALS:
								case SDLK_UP:
								case SDLK_DOWN:
								case SDLK_INSERT:
								case SDLK_PAGEUP:
								case SDLK_PAGEDOWN:
								case SDLK_F1:
								case SDLK_F2:
								case SDLK_F3:
								case SDLK_F4:
								case SDLK_F5:
								case SDLK_F6:
								case SDLK_F7:
								case SDLK_F8:
								case SDLK_F9:
								case SDLK_F10:
								case SDLK_F11:
								case SDLK_F12:
								case SDLK_F13:
								case SDLK_F14:
								case SDLK_F15:
								case SDLK_NUMLOCK:
								case SDLK_CAPSLOCK:
								case SDLK_SCROLLOCK:
								case SDLK_RSHIFT:
								case SDLK_LSHIFT:
								case SDLK_RCTRL:
								case SDLK_LCTRL:
								case SDLK_RALT:
								case SDLK_LALT:
								case SDLK_RMETA:
								case SDLK_LMETA:
								case SDLK_RSUPER:
								case SDLK_LSUPER:
								case SDLK_MODE:
								case SDLK_HELP:
								case SDLK_PRINT:
								case SDLK_SYSREQ:
								case SDLK_BREAK:
								case SDLK_MENU:
								case SDLK_POWER:
								case SDLK_EURO:
									//m.print(msg::MDEBUG, NULL, "%s", SDL_GetKeyName(event::get_event().key.keysym.sym));
									break;
								default:
									//m.print(msg::MDEBUG, NULL, "%s", SDL_GetKeyName(event::get_event().key.keysym.sym));
									sprintf(tmp_text, "%s", SDL_GetKeyName(event::get_event().key.keysym.sym));

									if(event::keyboard_layout == event::DE) {
										switch(event::get_event().key.keysym.sym) {
											case SDLK_QUOTE:
												sprintf(tmp_text, "%s", "ä");
												break;
											case SDLK_SEMICOLON:
												sprintf(tmp_text, "%s", "ö");
												break;
											case SDLK_LEFTBRACKET:
												sprintf(tmp_text, "%s", "ü");
												break;
											case SDLK_RIGHTBRACKET:
												sprintf(tmp_text, "%s", "+");
												break;
											case SDLK_MINUS:
												sprintf(tmp_text, "%s", "ß");
												break;
											case SDLK_SLASH:
												sprintf(tmp_text, "%s", "-");
												break;
											case SDLK_BACKSLASH:
												sprintf(tmp_text, "%s", "#");
												break;
											case SDLK_EQUALS:
												sprintf(tmp_text, "%s", "´");
												break;
											case SDLK_BACKQUOTE:
												sprintf(tmp_text, "%s", "^");
												break;
											case SDLK_z:
												sprintf(tmp_text, "%s", "y");
												break;
											case SDLK_y:
												sprintf(tmp_text, "%s", "z");
												break;
											default:
												break;
										}
									}

									switch(event::get_event().key.keysym.sym) {
										case SDLK_KP0:
											sprintf(tmp_text, "%s", "0");
											break;
										case SDLK_KP1:
											sprintf(tmp_text, "%s", "1");
											break;
										case SDLK_KP2:
											sprintf(tmp_text, "%s", "2");
											break;
										case SDLK_KP3:
											sprintf(tmp_text, "%s", "3");
											break;
										case SDLK_KP4:
											sprintf(tmp_text, "%s", "4");
											break;
										case SDLK_KP5:
											sprintf(tmp_text, "%s", "5");
											break;
										case SDLK_KP6:
											sprintf(tmp_text, "%s", "6");
											break;
										case SDLK_KP7:
											sprintf(tmp_text, "%s", "7");
											break;
										case SDLK_KP8:
											sprintf(tmp_text, "%s", "8");
											break;
										case SDLK_KP9:
											sprintf(tmp_text, "%s", "9");
											break;
										case SDLK_KP_PERIOD:
											sprintf(tmp_text, "%s", ",");
											break;
										case SDLK_KP_DIVIDE:
											sprintf(tmp_text, "%s", "/");
											break;
										case SDLK_KP_MULTIPLY:
											sprintf(tmp_text, "%s", "*");
											break;
										case SDLK_KP_MINUS:
											sprintf(tmp_text, "%s", "-");
											break;
										case SDLK_KP_PLUS:
											sprintf(tmp_text, "%s", "+");
											break;
										default:
											break;
									}

									keys = SDL_GetKeyState(NULL);
									if(keys[SDLK_RSHIFT] == SDL_PRESSED
										|| keys[SDLK_LSHIFT] == SDL_PRESSED) {
										shift = true;
									}
									if(keys[SDLK_RALT] == SDL_PRESSED
										|| keys[SDLK_LALT] == SDL_PRESSED) {
										alt = true;
									}

									if(shift) {
										key = toupper(tmp_text[0]);
										if(keyboard_layout == event::DE) {
											switch(key) {
												case 'ä':
													key = 'Ä';
													break;
												case 'ö':
													key = 'Ö';
													break;
												case 'ü':
													key = 'Ü';
													break;
												case 'ß':
													key = '?';
													break;
												case '1':
													key = '!';
													break;
												case '2':
													key = '"';
													break;
												case '3':
													key = '§';
													break;
												case '4':
													key = '$';
													break;
												case '5':
													key = '%';
													break;
												case '6':
													key = '&';
													break;
												case '7':
													key = '/';
													break;
												case '8':
													key = '(';
													break;
												case '9':
													key = ')';
													break;
												case '0':
													key = '=';
													break;
												case ',':
													key = ';';
													break;
												case '.':
													key = ':';
													break;
												case '-':
													key = '_';
													break;
												case '#':
													key = '\'';
													break;
												case '+':
													key = '*';
													break;
												case '´':
													key = '`';
													break;
												case '^':
													key = '°';
													break;
											}
										}
									}
									else if(alt) {
										key = tmp_text[0];
										if(keyboard_layout == event::DE) {
											switch(key) {
												case 'ß':
													key = '\\';
													break;
												case '2':
													key = '²';
													break;
												case '3':
													key = '³';
													break;
												case '7':
													key = '{';
													break;
												case '8':
													key = '[';
													break;
												case '9':
													key = ']';
													break;
												case '0':
													key = '}';
													break;
												case 'q':
													key = '@';
													break;
												case 'm':
													key = 'µ';
													break;
												case '+':
													key = '~';
													break;
												case 'e':
													key = '€';
													break;
											}
										}
									}
									else {
										key = tmp_text[0];
									}

									sprintf(tmp_text, "%c", key);
									strcat(input_text, tmp_text);

									shift = false;
									alt = false;
									break;
							}
						}
						break;
						default:
							break;
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

/*! returns a pointer to the currently active gui element
 */
event::gui_element* event::get_active_element() {
	return event::active_element;
}

/*! sets the currently active gui element
 *  @param active_element pointer to the new active gui element
 */
void event::set_active_element(event::gui_element* active_element) {
	event::active_element = active_element;
}

/*! gets the current input text (for any input box) and writes it to tmp_text
 *  @param tmp_text pointer to text where the new text should be written to
 */
void event::get_input_text(char* tmp_text) {
	strcpy(tmp_text, input_text);
	for(int i = 0; i < 512; i++) {
        event::input_text[i] = 0;
	}
}

/*! sets the currently used keyboard layout (just us and de support for the moment)
 *  @param layout the keyboard layout
 */
void event::set_keyboard_layout(IKEY_LAYOUT layout) {
	event::keyboard_layout = layout;
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
