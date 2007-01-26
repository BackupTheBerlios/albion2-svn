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

#include "gui_list.h"

/*! there is no function currently
 */
gui_list::gui_list(engine* e, gui_style* gs) : gui_object(e, gs) {
	gui_list::type = "listbox";

	is_active = false;

	drawable_items = 0;
	position = 0;
	top_item = 0;

	sid = 0;

	r1 = new gfx::rect();

	// get classes
	gui_list::e = e;
	gui_list::c = e->get_core();
	gui_list::m = e->get_msg();
	gui_list::g = e->get_gfx();
	gui_list::gf = e->get_gui_font();
	gui_list::gs = gs;
}

/*! there is no function currently
 */
gui_list::~gui_list() {
	m->print(msg::MDEBUG, "gui_list.cpp", "freeing gui_list stuff");

	delete r1;

	m->print(msg::MDEBUG, "gui_list.cpp", "gui_list stuff freed");
}

/*! draws the list box
 *  @param x specifies how much the element is moved on the x axis
 *  @param y specifies how much the element is moved on the y axis
 */
void gui_list::draw(unsigned int x, unsigned int y) {
	draw_object(x, y);

	// draw items
	for(unsigned int i = top_item; i < (top_item + drawable_items > items.size() ? items.size() : top_item + drawable_items); i++) {
		if(items[i].id != sid) {
			text_handler->draw(items[i].text.c_str(), x + gui_list::rectangle->x1 + 4, y + gui_list::rectangle->y1 + 5 + (i-top_item)*18);
		}
		else {
			text_handler->set_color(gs->get_color("FONT2"));

			g->pnt_to_rect(r1, gui_list::rectangle->x1 + x + 1, gui_list::rectangle->y1 + y + 1 + (i-top_item)*18,
				gui_list::rectangle->x2 + x - 20, gui_list::rectangle->y1 + y + 1 + (i-top_item)*18 + 17);
			g->draw_filled_rectangle(r1, gs->get_color("SELECTED1"));

			text_handler->draw(items[i].text.c_str(), x + gui_list::rectangle->x1 + 4, y + gui_list::rectangle->y1 + 5 + (i-top_item)*18);

			text_handler->set_color(gs->get_color("FONT"));
		}
	}

	// vbar handling
	//gui_list::set_position(vbar_handler->get_position());
}

/*! creates a vbar -> a pointer to the vbar class
 *  @param ivbar the vertical bar we want to handle
 */
void gui_list::set_vbar_handler(gui_vbar* ivbar) {
	gui_list::vbar_handler = ivbar;
}

//! returns the vbar handler
gui_vbar* gui_list::get_vbar_handler() {
	return gui_list::vbar_handler;
}

//! returns the list boxes is_active bool
bool gui_list::get_active() {
	return gui_list::is_active;
}

//! returns the list position
unsigned int gui_list::get_position() {
	return gui_list::position;
}

/*! sets the list boxes rectangle
 *  @param rectangle the rectangle we want to set
 */
void gui_list::set_rectangle(gfx::rect* rectangle) {
	gui_list::rectangle = rectangle;
	drawable_items = (unsigned int)(gui_list::rectangle->y2 - gui_list::rectangle->y1) / 18;
	vbar_handler->set_shown_lines(drawable_items);
}

/*! sets the is_active bool
 *  @param is_active bool if the list box is pressed currently
 */
void gui_list::set_active(bool is_active) {
	gui_list::is_active = is_active;
}

/*! sets the list position
 *  @param position the position (the number of the item that is shown in the first line)
 */
void gui_list::set_position(unsigned int position) {
	gui_list::position = position;
	if(position > (unsigned int)items.size() - drawable_items) {
		gui_list::top_item = (unsigned int)items.size() - drawable_items;
	}
	else if((unsigned int)items.size() > drawable_items) {
		gui_list::top_item = position;
	}
	else {
		gui_list::top_item = 0;
	}
	vbar_handler->set_position(position);

	gui_list::set_redraw(true);
}

/*! adds an item to the list
 *  @param text the text of the item
 *  @param id the id of the added item
 */
gui_list::item* gui_list::add_item(char* text, unsigned int id) {
	for(unsigned int i = 0; i < items.size(); i++) {
		if(items[i].id == id) {
			m->print(msg::MERROR, "gui_list.cpp", "gui list item with such an id already exists!");
			return 0;
		}
	}

	items.push_back(*new gui_list::item());
	items.back().text = text;
	items.back().id = id;

	// set position, so that we see the last line
	//gui_list::set_position(gui_list::get_position() + 1);

	gui_list::vbar_handler->set_max_lines((unsigned int)items.size());

	gui_list::set_redraw(true);

	return &items.back();
}

/*! deletes an item of the list
 *  @param id the id of the item
 */
void gui_list::delete_item(unsigned int id) {
	for(unsigned int i = 0; i < items.size(); i++) {
		if(items[i].id == id) {
			items.erase(items.begin()+i);
		}
	}
	gui_list::set_redraw(true);
}

/*! returns the amount of list items
 */
unsigned int gui_list::get_citems() {
	return (unsigned int)gui_list::items.size();
}

/*! selects an element of the list box
 *  @param x the x pos
 *  @param y the y pos
 */
void gui_list::select_pos(unsigned int x, unsigned int y) {
	y = y - rectangle->y1 - 2;
	unsigned int sel_item = (unsigned int)(y / 18) + top_item;
	if(sel_item < items.size()) { sid = items[sel_item].id; }
	else {
		// item out of range ...
		//m->print(msg::MERROR, "gui_list.cpp", "select_pos(): selected item (%u) out of range!", sel_item);
	}
	gui_list::set_redraw(true);
}

/*! returns the id of the selected element
 */
unsigned int gui_list::get_selected_id() {
	return gui_list::sid;
}

/*! selects an element of the list box
 *  @param sid the elements id
 */
void gui_list::set_selected_id(unsigned int sid) {
	gui_list::sid = sid;
	gui_list::set_redraw(true);
}

/*! returns the selected list box item
 */
gui_list::item* gui_list::get_selected_item() {
	return gui_list::get_item(gui_list::sid);
}

/*! returns the list box item with the id specified by 'id'
 *  @param id the items id
 */
gui_list::item* gui_list::get_item(unsigned int id) {
	for(unsigned int i = 0; i < items.size(); i++) {
		if(items[i].id == id) {
			return &items[i];
		}
	}
	return NULL;
}

//! clears the list, deletes all items
void gui_list::clear() {
	if(items.size() == 0) return;

	items.clear();
	top_item = 0;
	sid = 0;
	position = 0;
	vbar_handler->set_max_lines(0);

	redraw = true;
}
