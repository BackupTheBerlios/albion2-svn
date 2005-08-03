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

#include "gui_list_item.h"
#include "gui_style.h"
#include "gui_text.h"
#include "gfx.h"
#include "msg.h"
#include "core.h"
#include "engine.h"

/*! there is no function currently
 */
gui_list_item::gui_list_item(engine* e) {
	text = new char[256];

	point = new core::pnt();

	text_handler = NULL;

	// get classes
	gui_list_item::e = e;
	gui_list_item::c = e->get_core();
	gui_list_item::m = e->get_msg();
	gui_list_item::gstyle = e->get_gui_style();
	gui_list_item::g = e->get_gfx();
}

/*! there is no function currently
 */
gui_list_item::~gui_list_item() {
	m->print(msg::MDEBUG, "gui_list_item.cpp", "freeing gui_list_item stuff");

	delete text;
	delete point;

	if(text_handler) {
		delete text_handler;
	}

	m->print(msg::MDEBUG, "gui_list_item.cpp", "gui_list_item stuff freed");
}

/*! draws the list box item
 *  @param x specifies how much the element is moved on the x axis
 *  @param y specifies how much the element is moved on the y axis
 */
void gui_list_item::draw(unsigned int x, unsigned int y) {
	gui_list_item::text_handler->draw(x, y);
}

/*! creates a text_handler -> a pointer to the gui_text class
 *  @param itext the gui_text we want to handle
 */
void gui_list_item::set_text_handler(gui_text* itext) {
	gui_list_item::text_handler = itext;
}

/*! returns the text_handler
 */
gui_text* gui_list_item::get_text_handler() {
	return gui_list_item::text_handler;
}

//! returns the list box items text
char* gui_list_item::get_text() {
	return gui_list_item::text;
}

//! returns the list box items id
unsigned int gui_list_item::get_id() {
	return gui_list_item::id;
}

//! returns the text starting point
core::pnt* gui_list_item::get_point() {
	return gui_list_item::point;
}

/*! sets the list box items text
 *  @param text the text we want to set
 */
void gui_list_item::set_text(char* text) {
	memcpy(gui_list_item::text, text, strlen(text));
	gui_list_item::text[strlen(text)] = 0;
	gui_list_item::text_handler->set_text(gui_list_item::text);
}

/*! sets the list box items id
 *  @param id the id we want to set
 */
void gui_list_item::set_id(unsigned int id) {
	gui_list_item::id = id;
}

/*! sets the text starting point
 *  @param x the starting x position we want to set
 *  @param y the starting y position we want to set
 */
void gui_list_item::set_point(unsigned int x, unsigned int y) {
	gui_list_item::point->x = x;
	gui_list_item::point->y = y;
	gui_list_item::text_handler->set_point(gui_list_item::point);
}

/*! "deletes" the item
 */
void gui_list_item::clear() {
	gui_list_item::id = 0xFFFFFF+gui_list_item::id;
	gui_list_item::text = "";
	text_handler->set_notext();
	delete text_handler;
}
