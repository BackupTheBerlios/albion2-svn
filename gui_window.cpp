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
#include "gfx.h"
#include "msg.h"
#include "core.h"
#include "engine.h"

/*! there is no function currently
 */
gui_window::gui_window() {
	gui_window::lid = 0;

	gui_window::border = true;
	gui_window::moving = false;
	gui_window::deleted = false;

	// 1024 chars
	gui_window::caption = (char*)malloc(64);
}

/*! there is no function currently
 */
gui_window::~gui_window() {
	m.print(msg::MDEBUG, "gui_window.cpp", "freeing gui_window stuff");

	free(gui_window::caption);

	m.print(msg::MDEBUG, "gui_window.cpp", "gui_window stuff freed");
}

/*! draws the window
 */
void gui_window::draw() {
	if(gui_window::border) {
		// draw bg
		g.draw_filled_rectangle(engine_handler->get_screen(),
			gui_window::rectangle,
			engine_handler->get_gstyle().STYLE_BG);

		// draw 2 colored border
		g.draw_2colored_rectangle(engine_handler->get_screen(),
			gui_window::rectangle,
			engine_handler->get_gstyle().STYLE_LIGHT,
			engine_handler->get_gstyle().STYLE_DARK);

		gfx::rect* r = (gfx::rect*)malloc(sizeof(gfx::rect));
		memcpy(r, gui_window::rectangle, sizeof(gfx::rect));

		r->x1 += 1;
		r->x2 -= 1;
		r->y2 = r->y1 + 17;
		r->y1 += 1;
		g.draw_filled_rectangle(engine_handler->get_screen(), r,
			engine_handler->get_gstyle().STYLE_DARK);

		r->y1 = r->y2 + 1;
		r->y2 = gui_window::rectangle->y2 - 1;
		g.draw_2colored_rectangle(engine_handler->get_screen(), r,
			engine_handler->get_gstyle().STYLE_LIGHT,
			engine_handler->get_gstyle().STYLE_DARK);

		free(r);

		gfx::pnt* p = (gfx::pnt*)malloc(sizeof(gfx::pnt));
		p->x = gui_window::rectangle->x1 + 2;
		p->y = gui_window::rectangle->y1 + 3;
		gui_window::text_handler->set_point(p);
		gui_window::text_handler->draw(0, 0);
		free(p);

		// button event handling
		if(gui_window::exit_button_handler->get_pressed() == true) {
			gui_window::deleted = true;
		}
	}
}

/*! creates a engine_handler -> a pointer to the engine class
 *  @param iengine the engine we want to handle
 */
void gui_window::set_engine_handler(engine* iengine) {
	gui_window::engine_handler = iengine;
}

/*! creates a text_handler -> a pointer to the gui_text class
 *  @param itext the gui_text we want to handle
 */
void gui_window::set_text_handler(gui_text* itext) {
	gui_window::text_handler = itext;
}

//! returns the windows id
unsigned int gui_window::get_id() {
	return gui_window::id;
}

//! returns the windows rectangle
gfx::rect* gui_window::get_rectangle() {
	return gui_window::rectangle;
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

/*! sets the windows id
 *  @param id the id we want to set
 */
void gui_window::set_id(unsigned int id) {
	gui_window::id = id;
}

/*! sets the windows rectangle
 *  @param rectangle the rectangle we want to set
 */
void gui_window::set_rectangle(gfx::rect* rectangle) {
	gui_window::rectangle = rectangle;
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
void gui_window::set_caption(char* caption) {
	memcpy(gui_window::caption, caption, strlen(caption));
	gui_window::caption[strlen(caption)] = 0;
	gui_window::text_handler->set_text(caption);
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
 *  @param state the state of thedelete flag we want to set
 */
void gui_window::set_deleted(bool state) {
	gui_window::deleted = state;
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
