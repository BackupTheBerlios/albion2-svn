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

#include "gui_window.h"

/*! there is no function currently
 */
gui_window::gui_window(engine* e, gui_style* gs) : gui_object(e, gs) {
	gui_window::type = "window";

	gui_window::lid = 0;

	gui_window::border = true;
	gui_window::moving = false;
	gui_window::deleted = false;
	gui_window::bg = false;

	// get classes
	gui_window::e = e;
	gui_window::c = e->get_core();
	gui_window::m = e->get_msg();
	gui_window::g = e->get_gfx();
	gui_window::gs = gs;
}

/*! there is no function currently
 */
gui_window::~gui_window() {
	m->print(msg::MDEBUG, "gui_window.cpp", "freeing gui_window stuff");

	m->print(msg::MDEBUG, "gui_window.cpp", "gui_window stuff freed");
}

/*! draws the window
 */
void gui_window::draw() {
	if(gui_window::border) {
		draw_object(0, 0);

		// button event handling
		if(gui_window::exit_button_handler->get_state() == "CLICKED") {
			gui_window::deleted = true;
		}
	}

	if(!gui_window::border && bg) {
		draw_object(0, 0);
	}
}

//! returns the windows layer id
unsigned int gui_window::get_lid() {
	return gui_window::lid;
}

//! returns true if the border draw flag is set
bool gui_window::get_border() {
	return gui_window::border;
}

//! returns true if the window moving flag is set
bool gui_window::is_moving() {
	return gui_window::moving;
}

//! returns true if the window delete flag is set
bool gui_window::get_deleted() {
	return gui_window::deleted;
}

//! returns true if the window bg flag is set
bool gui_window::get_bg() {
	return gui_window::bg;
}

/*! sets the windows layer id
 *  @param lid the layer id we want to set
 */
void gui_window::set_lid(unsigned int lid) {
	gui_window::lid = lid;
}

/*! sets the windows caption
 *  @param caption the caption we want to set
 */
void gui_window::set_caption(const char* caption) {
	text = caption;
}

/*! sets the windows border draw flag
 *  @param state the state of the border draw flag we want to set
 */
void gui_window::set_border(bool state) {
	gui_window::border = state;
}

/*! sets the windows moving flag
 *  @param state the state of the moving flag we want to set
 */
void gui_window::set_moving(bool state) {
	gui_window::moving = state;
}

/*! sets the windows delete flag
 *  @param state the state of the delete flag we want to set
 */
void gui_window::set_deleted(bool state) {
	gui_window::deleted = state;
}

/*! sets the windows background draw flag
 *  @param state the state of the background draw flag we want to set
 */
void gui_window::set_bg(bool state) {
	gui_window::bg = state;
	if(!border) state ? set_state("BACKGROUND") : set_state("NORMAL");
	else set_state("NORMAL");
}

/*! changes the position of the window
 *  @param x defines how far to move the window on the x axis
 *  @param y defines how far to move the window on the y axis
 */
void gui_window::change_position(int x, int y) {
	// check if window is moved out of the clipping window
	if(!((int)gui_window::rectangle->x1 + x < 0)) {
        gui_window::rectangle->x1 += x;
        gui_window::rectangle->x2 += x;
	}
	else {
		gui_window::rectangle->x2 = gui_window::rectangle->x2 - gui_window::rectangle->x1;
		gui_window::rectangle->x1 = 0;
	}

	// check if window is moved out of the clipping window
	if(!((int)gui_window::rectangle->y1 + y < 0)) {
        gui_window::rectangle->y1 += y;
        gui_window::rectangle->y2 += y;
	}
	else {
		gui_window::rectangle->y2 = gui_window::rectangle->y2 - gui_window::rectangle->y1;
		gui_window::rectangle->y1 = 0;
	}
}

/*! sets the exit button handler
 *  @param ibutton the button handler
 */
void gui_window::set_exit_button_handler(gui_button* ibutton) {
	gui_window::exit_button_handler = ibutton;
}

//! returns the exit button handler
gui_button* gui_window::get_exit_button_handler() {
	return gui_window::exit_button_handler;
}
