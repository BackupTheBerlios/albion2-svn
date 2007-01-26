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

#include "gui_combo.h"

/*! there is no function currently
 */
gui_combo::gui_combo(engine* e, gui_style* gs) : gui_object(e, gs) {
	gui_combo::type = "combobox";

	gui_combo::list_button = NULL;

	gui_combo::selected_id = 0;
	gui_combo::selected_item = 0;
	gui_combo::marked_item = 0;

	r1 = new gfx::rect();
	r2 = new gfx::rect();

	// get classes
	gui_combo::e = e;
	gui_combo::c = e->get_core();
	gui_combo::m = e->get_msg();
	gui_combo::g = e->get_gfx();
	gui_combo::gf = e->get_gui_font();
	gui_combo::evt = e->get_event();
	gui_combo::gs = gs;

	gui_combo::list_visible = false;
	gui_combo::list_button_pressed = false;
}

/*! there is no function currently
 */
gui_combo::~gui_combo() {
	m->print(msg::MDEBUG, "gui_combo.cpp", "freeing gui_button stuff");

	items.clear();

	delete r1;
	delete r2;

	m->print(msg::MDEBUG, "gui_combo.cpp", "gui_button stuff freed");
}

/*! draws the combo box
 *  @param x specifies how much the element is moved on the x axis
 *  @param y specifies how much the element is moved on the y axis
 */
void gui_combo::draw(unsigned int x, unsigned int y) {
	// update combo box button
	g->pnt_to_rect(gui_combo::list_button->get_rectangle(),
		gui_combo::rectangle->x2 - 14 - 2, gui_combo::rectangle->y1 + 1,
		gui_combo::rectangle->x2 - 1, gui_combo::rectangle->y1 + 16 + 2);

	if(list_visible) {
		set_state("CLICKED");
		get_rectangle()->y2 = get_rectangle()->y1 + 19 + (unsigned int)items.size() * 18 + 2;
	}
	else {
		set_state("NORMAL");
		get_rectangle()->y2 = get_rectangle()->y1 + 19;
	}

	draw_object(x, y);

	// render list texts if the list is visible and item count is greater than 0
	if(items.size() != 0 && list_visible) {
		g->pnt_to_rect(r1, x + gui_combo::rectangle->x1, y + gui_combo::rectangle->y1 + 20,
			x + gui_combo::rectangle->x2, y + gui_combo::rectangle->y2);
		unsigned int i = 0;
		for(vector<gui_combo::item>::iterator iiter = items.begin(); iiter != items.end(); iiter++) {
			if(iiter->id != items[marked_item].id) {
				text_handler->draw(iiter->text.c_str(), r1->x1 + 3, r1->y1 + 4 + 18 * i);
			}
			else {
				// draw marked item
				text_handler->set_color(gs->get_color("FONT2"));
				r2->x1 = r1->x1 + 1;
				r2->x2 = r1->x2;
				r2->y1 = r1->y1 + 1 + 18*i;
				r2->y2 = r1->y1 + 18*(i+1);
				g->draw_filled_rectangle(r2, gs->get_color("SELECTED1"));
				text_handler->draw(iiter->text.c_str(), r1->x1 + 3, r1->y1 + 4 + 18 * i);
				text_handler->set_color(gs->get_color("FONT"));
			}
			i++;
		}
	}
}

//! returns true if the combo boxes list is visible and false if it's not
bool gui_combo::is_list_visible() {
	return gui_combo::list_visible;
}

/*! sets the list_visible flag
 *  @param visible flag that specifies if the combo boxes list is visible or not
 */
void gui_combo::set_list_visible(bool visible) {
	gui_combo::list_visible = visible;
}

void gui_combo::set_list_button(gui_button* button) {
	gui_combo::list_button = button;
}

gui_button* gui_combo::get_list_button() {
	return gui_combo::list_button;
}

void gui_combo::add_item(char* text, unsigned int id) {
	gui_combo::items.push_back(*new gui_combo::item());
	gui_combo::items.back().id = id;
	gui_combo::items.back().text = text;

	if(gui_combo::items.size() == 1) {
		set_selected_id(id);
	}
}

void gui_combo::set_selected_item(unsigned int item) {
	if(gui_combo::selected_item != item) redraw = true;
	gui_combo::selected_item = item;
	gui_combo::selected_id = items[gui_combo::selected_item].id;

	text_handler->set_text((char*)items[gui_combo::selected_item].text.c_str());
}

void gui_combo::set_selected_id(unsigned int id) {
	if(gui_combo::selected_id != id) redraw = true;
	gui_combo::selected_id = id;
	unsigned int i = 0;
	for(vector<gui_combo::item>::iterator iiter = items.begin(); iiter != items.end(); iiter++) {
		if(iiter->id == id) {
			gui_combo::selected_item = i;
			text_handler->set_text((char*)items[gui_combo::selected_item].text.c_str());
			return;
		}
		i++;
	}
	m->print(msg::MERROR, "gui_combo.cpp", "set_selected_id(): an item with the id #%u doesn't exist!", id);
}

unsigned int gui_combo::get_selected_id() {
	return gui_combo::selected_id;
}

gui_combo::item* gui_combo::get_selected_item() {
	for(vector<gui_combo::item>::iterator iiter = items.begin(); iiter != items.end(); iiter++) {
		if(iiter->id == gui_combo::selected_id) {
			return &*iiter;
		}
	}
	m->print(msg::MERROR, "gui_combo.cpp", "get_selected_item(): an item with the id #%u doesn't exist!", gui_combo::selected_id);
	return NULL;
}

unsigned int gui_combo::get_item_count() {
	return (unsigned int)gui_combo::items.size();
}

void gui_combo::select_pos(core::pnt* pos) {
	unsigned int y = pos->y - rectangle->y1 - 21;
	marked_item = y / 18;
}

void gui_combo::click_pos(core::pnt* pos) {
	unsigned int y = pos->y - rectangle->y1 - 21;
	set_selected_id(items[y / 18].id);
}

unsigned int gui_combo::get_marked_item() {
	return marked_item;
}

//! handles the combo box (its events)
void gui_combo::handle_combo() {
	// event handling
	if(evt->is_gui_event(event::BUTTON_PRESSED, gui_combo::list_button->get_id()) && !list_button_pressed) {
		list_button_pressed = true;
		list_visible = list_visible ^ true;
		set_redraw(true);
	}
	else if(gui_combo::list_button->get_state() != "CLICKED") {
		list_button_pressed = false;
	}
}
