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
gui_list_item::gui_list_item() {
	text = (char*)malloc(256);
}

/*! there is no function currently
 */
gui_list_item::~gui_list_item() {
	free(text);
}

//! draws the list box item
void gui_list_item::draw_list_item() {
	gui_list_item::text_handler->draw_text();
}

/*! creates a text_handler -> a pointer to the gui_text class
 *  @param itext the gui_text we want to handle
 */
void gui_list_item::set_text_handler(gui_text* itext) {
	gui_list_item::text_handler = itext;
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
gfx::pnt* gui_list_item::get_point() {
	return gui_list_item::point;
}

/*! sets the list box items text
 *  @param id the id we want to set
 */
void gui_list_item::set_text(char* text) {
	gui_list_item::text = text;
	gui_list_item::text_handler->set_text(text);
}

/*! sets the list box items id
 *  @param id the id we want to set
 */
void gui_list_item::set_id(unsigned int id) {
	gui_list_item::id = id;
}

/*! sets the text starting point
 *  @param point the starting point we want to set
 */
void gui_list_item::set_point(gfx::pnt* point) {
	gui_list_item::point = point;
	gui_list_item::text_handler->set_point(point);
}

/*! "deletes" the item
 */
void gui_list_item::clear() {
	gui_list_item::id = 0xFFFFFF+gui_list_item::id;
	gui_list_item::text = "";
	text_handler->set_notext();
	delete text_handler;
}
