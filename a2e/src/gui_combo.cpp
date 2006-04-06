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
gui_combo::gui_combo(engine* e) {
	gui_combo::item_text = NULL;
	gui_combo::list_button = NULL;

	gui_combo::selected_id = 0;
	gui_combo::selected_item = 0;
	gui_combo::marked_item = 0;

	// get classes
	gui_combo::e = e;
	gui_combo::c = e->get_core();
	gui_combo::m = e->get_msg();
	gui_combo::g = e->get_gfx();
	gui_combo::gf = e->get_gui_font();

	gui_combo::list_visible = false;
	gui_combo::list_button_pressed = false;
}

/*! there is no function currently
 */
gui_combo::~gui_combo() {
	m->print(msg::MDEBUG, "gui_combo.cpp", "freeing gui_button stuff");

	delete gui_combo::rectangle;
	items.clear();

	m->print(msg::MDEBUG, "gui_combo.cpp", "gui_button stuff freed");
}

/*! draws the combo box
 *  @param x specifies how much the element is moved on the x axis
 *  @param y specifies how much the element is moved on the y axis
 */
void gui_combo::draw(unsigned int x, unsigned int y) {
	gfx::rect* r1 = new gfx::rect();
	gfx::rect* r2 = new gfx::rect();

	g->pnt_to_rect(r1, gui_combo::rectangle->x1 + x, gui_combo::rectangle->y1 + y,
		gui_combo::rectangle->x2 + x, gui_combo::rectangle->y2 + y);

	// draw bg
	g->draw_filled_rectangle(r1,
		e->get_gui_style()->STYLE_BG2);

	// draw 2 colored border
	g->draw_2colored_rectangle(r1,
		e->get_gui_style()->STYLE_INDARK,
		e->get_gui_style()->STYLE_LIGHT);

	// draw 2 colored border
	g->pnt_to_rect(r1, gui_combo::rectangle->x1 + x + 1, gui_combo::rectangle->y1 + y + 1,
		gui_combo::rectangle->x2 + x - 1, gui_combo::rectangle->y2 + y - 1);
	g->draw_2colored_rectangle(r1, e->get_gui_style()->STYLE_DARK,
		e->get_gui_style()->STYLE_DARK2);

	// draw list button
	g->pnt_to_rect(gui_combo::list_button->get_rectangle(),
		gui_combo::rectangle->x2 - 15 - 2, gui_combo::rectangle->y1 + 2,
		gui_combo::rectangle->x2 - 2, gui_combo::rectangle->y1 + 16 + 2);

	if(gui_combo::list_button->get_pressed() && !list_button_pressed) {
		list_button_pressed = true;
		list_visible = list_visible ^ true;
	}
	else if(!gui_combo::list_button->get_pressed()) {
		list_button_pressed = false;
	}

	g->begin_scissor();
	if(items.size() != 0) {
		g->set_scissor(x + gui_combo::rectangle->x1 + 2, y + gui_combo::rectangle->y1 + 2,
			x + gui_combo::rectangle->x2 - 16 - 2, y + gui_combo::rectangle->y2 - 2);
		item_text->draw(items[selected_item].text.c_str(),
			gui_combo::rectangle->x1 + x + 4, gui_combo::rectangle->y1 + y + 5);

		if(list_visible) {
			g->pnt_to_rect(r1, x + gui_combo::rectangle->x1, y + gui_combo::rectangle->y2 + 1,
				x + gui_combo::rectangle->x2, y + gui_combo::rectangle->y2 + 2 + items.size() * 18);

			g->end_scissor();
			// draw bg
			g->draw_filled_rectangle(r1, e->get_gui_style()->STYLE_BG2);

			// draw border
			g->draw_rectangle(r1, e->get_gui_style()->STYLE_INDARK);
			g->begin_scissor();

			g->set_scissor(r1->x1 + 1, r1->y1 + 1, r1->x2, r1->y2);
			unsigned int i = 0;
			for(vector<gui_combo::item>::iterator iiter = items.begin(); iiter != items.end(); iiter++) {
				if(iiter->id != items[marked_item].id) {
					item_text->draw(iiter->text.c_str(), r1->x1 + 2, r1->y1 + 2 + 18 * i);
				}
				else {
					// draw marked item
					item_text->set_font(gf->add_font(e->data_path("vera.ttf"), 12, e->get_gui_style()->STYLE_FONT2));
					r2->x1 = r1->x1 + 1;
					r2->x2 = r1->x2;
					r2->y1 = r1->y1 + 1 + 18*i;
					r2->y2 = r1->y1 + 18*(i+1);
					g->draw_filled_rectangle(r2, e->get_gui_style()->STYLE_SELECTED);
					item_text->draw(iiter->text.c_str(), r1->x1 + 2, r1->y1 + 2 + 18 * i);
					item_text->set_font(gf->add_font(e->data_path("vera.ttf"), 12, e->get_gui_style()->STYLE_FONT));
				}
				i++;
			}
		}
	}

	g->end_scissor();
	delete r1;
	delete r2;
}

//! returns the combo boxes id
unsigned int gui_combo::get_id() {
	return gui_combo::id;
}

//! returns the combo boxes rectangle
gfx::rect* gui_combo::get_rectangle() {
	return gui_combo::rectangle;
}

//! returns true if the combo boxes list is visible and false if it's not
bool gui_combo::is_list_visible() {
	return gui_combo::list_visible;
}

/*! sets the combo boxes id
 *  @param id the id we want to set
 */
void gui_combo::set_id(unsigned int id) {
	gui_combo::id = id;
}

/*! sets the combo boxes rectangle
 *  @param rectangle the rectangle we want to set
 */
void gui_combo::set_rectangle(gfx::rect* rectangle) {
	gui_combo::rectangle = rectangle;
}

/*! sets the list_visible flag
 *  @param visible flag that specifies if the combo boxes list is visible or not
 */
void gui_combo::set_list_visible(bool visible) {
	gui_combo::list_visible = visible;
}

void gui_combo::set_item_text(gui_text* text) {
	gui_combo::item_text = text;
}

void gui_combo::set_list_button(gui_button* button) {
	gui_combo::list_button = button;
}

gui_text* gui_combo::get_item_text() {
	return gui_combo::item_text;
}

gui_button* gui_combo::get_list_button() {
	return gui_combo::list_button;
}

void gui_combo::add_item(char* text, unsigned int id) {
	gui_combo::items.push_back(*new gui_combo::item());
	gui_combo::items.back().id = id;
	gui_combo::items.back().text = text;
}

void gui_combo::set_selected_item(unsigned int item) {
	gui_combo::selected_item = item;
	gui_combo::selected_id = items[gui_combo::selected_item].id;
}

void gui_combo::set_selected_id(unsigned int id) {
	gui_combo::selected_id = id;
	unsigned int i = 0;
	for(vector<gui_combo::item>::iterator iiter = items.begin(); iiter != items.end(); iiter++) {
		if(iiter->id == id) {
			gui_combo::selected_item = i;
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
	unsigned int y = pos->y - rectangle->y2 - 2;
	//set_selected_id(items[y / 18].id);
	marked_item = y / 18;
}

void gui_combo::click_pos(core::pnt* pos) {
	unsigned int y = pos->y - rectangle->y2 - 2;
	set_selected_id(items[y / 18].id);
}
