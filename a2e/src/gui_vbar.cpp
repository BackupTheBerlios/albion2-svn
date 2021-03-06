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

#include "gui_vbar.h"

/*! there is no function currently
 */
gui_vbar::gui_vbar(engine* e, gui_style* gs) : gui_object(e, gs) {
	gui_vbar::type = "vbar";

	gui_vbar::last_point = new core::pnt();
	gui_vbar::last_point->x = 0;
	gui_vbar::last_point->y = 0;

	gui_vbar::position = 0;

	gui_vbar::px_per_item = 0;

    gui_vbar::shown_lines = 0;
	gui_vbar::max_lines = 0;

	gui_vbar::slider_active = false;

	r1 = new gfx::rect();

	// get classes
	gui_vbar::e = e;
	gui_vbar::c = e->get_core();
	gui_vbar::m = e->get_msg();
	gui_vbar::g = e->get_gfx();
	gui_vbar::gs = gs;
}

/*! there is no function currently
 */
gui_vbar::~gui_vbar() {
	m->print(msg::MDEBUG, "gui_vbar.cpp", "freeing gui_vbar stuff");

	delete r1;

	m->print(msg::MDEBUG, "gui_vbar.cpp", "gui_vbar stuff freed");
}

/*! draws the vertical bar
 *  @param x specifies how much the element is moved on the x axis
 *  @param y specifies how much the element is moved on the y axis
 */
void gui_vbar::draw(unsigned int x, unsigned int y) {
	draw_object(x, y);

	// event handling
	if(gui_vbar::max_lines > gui_vbar::shown_lines) {
		slider_active = true;
	}
	else {
		slider_active = false;
	}

	if(slider_active) {
		unsigned int height_barbg = gui_vbar::rectangle->y2 - gui_vbar::rectangle->y1 - 30;
		unsigned int overflow = height_barbg % max_lines;
		gui_vbar::px_per_item = (height_barbg - overflow) / max_lines;
		if(gui_vbar::px_per_item < 1) {
			gui_vbar::px_per_item = 1;
		}
		unsigned int slider_height = px_per_item * shown_lines + overflow;
		unsigned int height_position = position * px_per_item;

		if(gui_vbar::up_button_handler->get_state() == "CLICKED") {
			gui_vbar::set_position(gui_vbar::get_position() - 1);
		}

		if(gui_vbar::down_button_handler->get_state() == "CLICKED") {
			gui_vbar::set_position(gui_vbar::get_position() + 1);
		}

		// draw slider
		g->pnt_to_rect(r1, gui_vbar::rectangle->x1,
			gui_vbar::rectangle->y1 + 16 + height_position - 1, gui_vbar::rectangle->x2,
			gui_vbar::rectangle->y1 + 16 + height_position + slider_height - 1);
		gs->render_gui_element("vbar_slider", "NORMAL", r1, x, y);
	}
}

//! returns the vertical bars max lines
unsigned int gui_vbar::get_max_lines() {
	return gui_vbar::max_lines;
}

//! returns the vertical bars shown lines
unsigned int gui_vbar::get_shown_lines() {
	return gui_vbar::shown_lines;
}

//! returns the vertical bar position
unsigned int gui_vbar::get_position() {
	return gui_vbar::position;
}

//! returns the vbars is_active bool
bool gui_vbar::get_active() {
	return gui_vbar::is_active;
}

//! returns the new mouse click point
core::pnt* gui_vbar::get_new_point() {
	return gui_vbar::new_point;
}

//! returns the slider active bool
bool gui_vbar::get_slider_active() {
	return gui_vbar::slider_active;
}

/*! sets the vertical bars max lines
 *  @param max_lines the maximal amount of lines
 */
void gui_vbar::set_max_lines(unsigned int max_lines) {
	gui_vbar::max_lines = max_lines;
	gui_vbar::set_redraw(true);
}

/*! sets the vertical bars shown lines
 *  @param shown_lines the amount of "showable" lines
 */
void gui_vbar::set_shown_lines(unsigned int shown_lines) {
	gui_vbar::shown_lines = shown_lines;
	gui_vbar::set_redraw(true);
}

/*! sets the vertical bar position
 *  @param position the position
 */
void gui_vbar::set_position(unsigned int position) {
	// 0xFFFFFFFF, because position is a uint and we decrease it by 0
	if(position == 0xFFFFFFFF) {
		gui_vbar::position = 0;
	}
	else if(position > gui_vbar::max_lines) {
		gui_vbar::position = gui_vbar::max_lines;
	}
	else if(gui_vbar::max_lines - position < gui_vbar::shown_lines && gui_vbar::max_lines > gui_vbar::shown_lines) {
		gui_vbar::position = gui_vbar::max_lines - gui_vbar::shown_lines;
	}
	else {
        gui_vbar::position = position;
	}
	gui_vbar::set_redraw(true);
}

/*! sets the is_active bool
 *  @param is_active bool if the vbar is pressed currently
 */
void gui_vbar::set_active(bool is_active) {
	gui_vbar::is_active = is_active;
}

/*! sets the up button handler
 *  @param ibutton the button handler
 */
void gui_vbar::set_up_button_handler(gui_button* ibutton) {
	gui_vbar::up_button_handler = ibutton;
}

/*! sets the down button handler
 *  @param ibutton the button handler
 */
void gui_vbar::set_down_button_handler(gui_button* ibutton) {
	gui_vbar::down_button_handler = ibutton;
}

//! returns the up button handler
gui_button* gui_vbar::get_up_button_handler() {
	return gui_vbar::up_button_handler;
}

//! returns the down button handler
gui_button* gui_vbar::get_down_button_handler() {
	return gui_vbar::down_button_handler;
}

/*! sets the new mouse click point
 *  @param new_point the new mouse click point point we want to set
 */
void gui_vbar::set_new_point(core::pnt* new_point) {
	if(gui_vbar::slider_active) {
		gui_vbar::new_point = new_point;
		if(gui_vbar::last_point->x == 0 && gui_vbar::last_point->y == 0) {
			gui_vbar::last_point->x = gui_vbar::new_point->x;
			gui_vbar::last_point->y = gui_vbar::new_point->y;
		}

		// gui_vbar::px_per_item / 128
		if(gui_vbar::new_point->y > gui_vbar::last_point->y) {
			if(gui_vbar::new_point->y - gui_vbar::last_point->y > gui_vbar::px_per_item) {
				unsigned int overflow = (gui_vbar::new_point->y - gui_vbar::last_point->y) % gui_vbar::px_per_item;
				unsigned int times = ((gui_vbar::new_point->y - gui_vbar::last_point->y - overflow) / gui_vbar::px_per_item) / 8;
				if(times < 1) { times = 1; }
				gui_vbar::set_position(gui_vbar::get_position() + times);
				gui_vbar::last_point->x = 0;
				gui_vbar::last_point->y = gui_vbar::last_point->y + overflow;
			}
		}
		else {
			if(gui_vbar::last_point->y - gui_vbar::new_point->y > gui_vbar::px_per_item) {
				unsigned int overflow = (gui_vbar::last_point->y - gui_vbar::new_point->y) % gui_vbar::px_per_item;
				unsigned int times = ((gui_vbar::last_point->y - gui_vbar::new_point->y - overflow) / gui_vbar::px_per_item) / 8;
				if(times < 1) { times = 1; }
				gui_vbar::set_position(gui_vbar::get_position() - times);
				gui_vbar::last_point->x = 0;
				gui_vbar::last_point->y = gui_vbar::last_point->y - overflow;
			}
		}
	}
	gui_vbar::set_redraw(true);
}

/*! sets the slider active bool
 *  @param state the state of slider active
 */
void gui_vbar::set_slider_active(bool state) {
	gui_vbar::slider_active = state;
}

/*! sets the objects rectangle
 *  @param rectangle the rectangle we want to set
 */
void gui_vbar::set_rectangle(gfx::rect* rectangle) {
	gui_vbar::rectangle = rectangle;

	gfx::rect* ubrect = gui_vbar::up_button_handler->get_rectangle();
	g->pnt_to_rect(ubrect, gui_vbar::rectangle->x1 + 3, gui_vbar::rectangle->y1,
		gui_vbar::rectangle->x1 + 18, gui_vbar::rectangle->y1 + 15);
	gui_vbar::up_button_handler->set_rectangle(ubrect);

	gfx::rect* dbrect = gui_vbar::down_button_handler->get_rectangle();
	g->pnt_to_rect(dbrect, gui_vbar::rectangle->x1 + 3, gui_vbar::rectangle->y2 - 15,
		gui_vbar::rectangle->x1 + 18, gui_vbar::rectangle->y2);
	gui_vbar::down_button_handler->set_rectangle(dbrect);
}
