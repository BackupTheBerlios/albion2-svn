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

#include "gui_check.h"
#include "gfx.h"
#include "msg.h"
#include "core.h"
#include "engine.h"

/*! there is no function currently
 */
gui_check::gui_check() {
	gui_check::text = new char[1024];

	gui_check::checked = false;
}

/*! there is no function currently
 */
gui_check::~gui_check() {
	m.print(msg::MDEBUG, "gui_check.cpp", "freeing gui_check stuff");

	delete gui_check::text;

	m.print(msg::MDEBUG, "gui_check.cpp", "gui_check stuff freed");
}

/*! draws the check box
 *  @param x specifies how much the element is moved on the x axis
 *  @param y specifies how much the element is moved on the y axis
 */
void gui_check::draw(unsigned int x, unsigned int y) {
	gfx::rect* r1 = new gfx::rect();
	core::pnt* p1 = new core::pnt();
	
	g.pnt_to_rect(r1, gui_check::rectangle->x1 + x,
		gui_check::rectangle->y1 + y,
		gui_check::rectangle->x1 + 14 + x,
		gui_check::rectangle->y1 + 14 + y);

	// draw bg
	g.draw_filled_rectangle(engine_handler->get_screen(),
		r1, engine_handler->get_gstyle().STYLE_BG2);

	// draw 2 colored border
	g.draw_2colored_rectangle(engine_handler->get_screen(), r1,
		engine_handler->get_gstyle().STYLE_INDARK,
		engine_handler->get_gstyle().STYLE_LIGHT);

	// draw 2 colored border
	g.pnt_to_rect(r1, gui_check::rectangle->x1 + 1 + x,
		gui_check::rectangle->y1 + 1 + y,
		gui_check::rectangle->x1 + 13 + x,
		gui_check::rectangle->y1 + 13 + y);
	g.draw_2colored_rectangle(engine_handler->get_screen(),
		r1, engine_handler->get_gstyle().STYLE_DARK,
		engine_handler->get_gstyle().STYLE_DARK2);

	if(gui_check::checked) {
		// draw the check mark
		core::pnt* p2 = new core::pnt();

		g.cord_to_pnt(p1, gui_check::rectangle->x1 + 5 + x, gui_check::rectangle->y1 + 11 + y);
		g.cord_to_pnt(p2, gui_check::rectangle->x1 + 12 + x, gui_check::rectangle->y1 + 3 + y);

		g.draw_line(engine_handler->get_screen(), p1, p2,
			engine_handler->get_gstyle().STYLE_DARK);

		g.cord_to_pnt(p1, gui_check::rectangle->x1 + 6 + x, gui_check::rectangle->y1 + 11 + y);
		g.cord_to_pnt(p2, gui_check::rectangle->x1 + 12 + x, gui_check::rectangle->y1 + 4 + y);

		g.draw_line(engine_handler->get_screen(), p1, p2,
			engine_handler->get_gstyle().STYLE_DARK);

		g.cord_to_pnt(p1, gui_check::rectangle->x1 + 6 + x, gui_check::rectangle->y1 + 12 + y);
		g.cord_to_pnt(p2, gui_check::rectangle->x1 + 12 + x, gui_check::rectangle->y1 + 5 + y);

		g.draw_line(engine_handler->get_screen(), p1, p2,
			engine_handler->get_gstyle().STYLE_DARK);



		g.cord_to_pnt(p1, gui_check::rectangle->x1 + 3 + x, gui_check::rectangle->y1 + 6 + y);
		g.cord_to_pnt(p2, gui_check::rectangle->x1 + 3 + x, gui_check::rectangle->y1 + 8 + y);

		g.draw_line(engine_handler->get_screen(), p1, p2,
			engine_handler->get_gstyle().STYLE_DARK);

		g.cord_to_pnt(p1, gui_check::rectangle->x1 + 4 + x, gui_check::rectangle->y1 + 7 + y);
		g.cord_to_pnt(p2, gui_check::rectangle->x1 + 4 + x, gui_check::rectangle->y1 + 9 + y);

		g.draw_line(engine_handler->get_screen(), p1, p2,
			engine_handler->get_gstyle().STYLE_DARK);

		g.cord_to_pnt(p1, gui_check::rectangle->x1 + 5 + x, gui_check::rectangle->y1 + 8 + y);
		g.cord_to_pnt(p2, gui_check::rectangle->x1 + 5 + x, gui_check::rectangle->y1 + 10 + y);

		g.draw_line(engine_handler->get_screen(), p1, p2,
			engine_handler->get_gstyle().STYLE_DARK);

		delete p2;
	}


	// draw the text
	g.cord_to_pnt(p1, gui_check::rectangle->x1 + 20, gui_check::rectangle->y1 + 2);
	gui_check::text_handler->set_point(p1);
	gui_check::text_handler->draw(x, y);


	delete r1;
	delete p1;
}

/*! draws the check box
 */
void gui_check::draw() {
	gui_check::draw(0, 0);
}

/*! creates a engine_handler -> a pointer to the engine class
 *  @param iengine the engine we want to handle
 */
void gui_check::set_engine_handler(engine* iengine) {
	gui_check::engine_handler = iengine;
}

/*! creates a text_handler -> a pointer to the gui_text class
 *  @param itext the gui_text we want to handle
 */
void gui_check::set_text_handler(gui_text* itext) {
	gui_check::text_handler = itext;
}

//! returns the check boxes id
unsigned int gui_check::get_id() {
	return gui_check::id;
}

//! returns the check boxes rectangle
gfx::rect* gui_check::get_rectangle() {
	return gui_check::rectangle;
}

//! returns the check boxes text
char* gui_check::get_text() {
	return gui_check::text;
}

//! returns true if the check box is checked
bool gui_check::get_checked() {
	return gui_check::checked;
}

/*! sets the text id
 *  @param id the id we want to set
 */
void gui_check::set_id(unsigned int id) {
	gui_check::id = id;
}

/*! sets the check boxes rectangle
 *  @param rectangle the rectangle we want to set
 */
void gui_check::set_rectangle(gfx::rect* rectangle) {
	gui_check::rectangle = rectangle;
}

/*! sets the check boxes text
 *  @param text the text we want to set
 */
void gui_check::set_text(char* text) {
	memcpy(gui_check::text, text, strlen(text));
	gui_check::text[strlen(text)] = 0;
	gui_check::text_handler->set_text(text);
}

/*! sets the check boxes checked flag
 *  @param state the state of the check box we want to set
 */
void gui_check::set_checked(bool state) {
	gui_check::checked= state;
}
