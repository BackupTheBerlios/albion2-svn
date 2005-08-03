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
#include "gui_list_item.h"
#include "gui_vbar.h"
#include "gfx.h"
#include "msg.h"
#include "core.h"
#include "engine.h"

/*! there is no function currently
 */
gui_list::gui_list(engine* e) {
	is_active = false;

	citems = 0;

	drawable_items = 0;

	rectangle = new gfx::rect();
	rectangle->x1 = 0;
	rectangle->y1 = 0;
	rectangle->x2 = 0;
	rectangle->y2 = 0;

	sid = 0;

	// get classes
	gui_list::e = e;
	gui_list::c = e->get_core();
	gui_list::m = e->get_msg();
	gui_list::g = e->get_gfx();
}

/*! there is no function currently
 */
gui_list::~gui_list() {
	m->print(msg::MDEBUG, "gui_list.cpp", "freeing gui_list stuff");

	delete rectangle;

	m->print(msg::MDEBUG, "gui_list.cpp", "gui_list stuff freed");
}

/*! draws the list box
 *  @param x specifies how much the element is moved on the x axis
 *  @param y specifies how much the element is moved on the y axis
 */
void gui_list::draw(unsigned int x, unsigned int y) {
	gfx::rect* r1 = new gfx::rect();

	g->pnt_to_rect(r1, gui_list::rectangle->x1 + x, gui_list::rectangle->y1 + y,
		gui_list::rectangle->x2 + x, gui_list::rectangle->y2 + y);

	// draw bg
	g->draw_filled_rectangle(e->get_screen(), r1,
		e->get_gui_style()->STYLE_BG2);

	// draw 2 colored border
	g->draw_2colored_rectangle(e->get_screen(), r1,
		e->get_gui_style()->STYLE_INDARK,
		e->get_gui_style()->STYLE_LIGHT);

	// draw 2 colored border
	g->pnt_to_rect(r1, gui_list::rectangle->x1 + x + 1, gui_list::rectangle->y1 + y + 1,
		gui_list::rectangle->x2 + x - 1, gui_list::rectangle->y2 + y - 1);
	g->draw_2colored_rectangle(e->get_screen(),
		r1, e->get_gui_style()->STYLE_DARK,
		e->get_gui_style()->STYLE_DARK2);

	// draw items
	// we need a height of 18px for each item
	unsigned int list_box_heigth = gui_list::rectangle->y2 - gui_list::rectangle->y1;
	gui_list::drawable_items = (list_box_heigth - (list_box_heigth % 18)) / 18;
	gui_list::vbar_handler->set_shown_lines(gui_list::drawable_items);
	unsigned int* ids = new unsigned int[citems];
	unsigned int* new_ids = new unsigned int[citems];
	for(unsigned int i = 0; i < citems; i++) {
		ids[i] = items[i]->get_id();
	}
	unsigned int c = citems;
	while(c != 0) {
		unsigned int highest = 0;
		unsigned int tmp_id = 0;
		for(unsigned int i = 0; i < citems; i++) {
			if(ids[i] > highest) {
				tmp_id = i;
				highest = ids[i];
			}
		}
		ids[tmp_id] = 0;
		new_ids[c-1] = highest;
		c--;
	}

	core::pnt* p1 = new core::pnt();
	// just loop for as many items as we have
	unsigned int loop = 0;
	if(drawable_items > citems) {
		loop = citems;
	}
	else {
		loop = drawable_items;
	}


	// reset points
	for(unsigned int i = 0; i < citems; i++) {
		items[i]->set_point(0, 0);
	}

	for(unsigned int i = 0; i < loop; i++) {
		for(unsigned int j = 0; j < citems; j++) {
			if(items[j]->get_id() == new_ids[i + position]) {
				g->cord_to_pnt(p1, gui_list::rectangle->x1 + 3,
					gui_list::rectangle->y1 + 6 + i*18);
				items[j]->set_point(p1->x, p1->y);

				// is item selected?
				if(sid == items[j]->get_id()) {
					r1->x1 = p1->x - 1 + x;
					r1->y1 = p1->y - 4 + y;
					r1->x2 = gui_list::rectangle->x2 - 15 + x;
					r1->y2 = p1->y + 13 + y;
					g->draw_filled_rectangle(e->get_screen(),
						r1, e->get_gui_style()->STYLE_SELECTED);
					g->draw_rectangle(e->get_screen(),
						r1, e->get_gui_style()->STYLE_DARK);
				}

				items[j]->draw(x, y);
			}
		}
	}
	delete p1;
	delete r1;

	delete ids;
	delete new_ids;

	// vbar handling
	gui_list::set_position(vbar_handler->get_position());
}

/*! creates a vbar -> a pointer to the vbar class
 *  @param ivbar the vertical bar we want to handle
 */
void gui_list::set_vbar_handler(gui_vbar* ivbar) {
	gui_list::vbar_handler = ivbar;
}

//! returns the list boxes id
unsigned int gui_list::get_id() {
	return gui_list::id;
}

//! returns the list boxes rectangle
gfx::rect* gui_list::get_rectangle() {
	return gui_list::rectangle;
}

//! returns the list boxes is_active bool
bool gui_list::get_active() {
	return gui_list::is_active;
}

//! returns the list position
unsigned int gui_list::get_position() {
	return gui_list::position;
}

/*! sets the list boxes id
 *  @param id the id we want to set
 */
void gui_list::set_id(unsigned int id) {
	gui_list::id = id;
}

/*! sets the list boxes rectangle
 *  @param rectangle the rectangle we want to set
 */
void gui_list::set_rectangle(gfx::rect* rectangle) {
	gui_list::rectangle = rectangle;
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
	// 0xFFFFFFFF, because position is a uint and we decrease it by 0
	if(position == 0xFFFFFFFF) {
		gui_list::position = 0;
		vbar_handler->set_position(gui_list::position);
	}
	else if(position > gui_list::citems) {
		gui_list::position = 0;
		vbar_handler->set_position(gui_list::position);
	}
	else if(gui_list::citems - position < gui_list::drawable_items) {
		if(gui_list::citems < gui_list::drawable_items) {
			gui_list::position = 0;
		}
		else {
			gui_list::position = gui_list::citems - gui_list::drawable_items;
		}
		vbar_handler->set_position(gui_list::position);
	}
	else {
        gui_list::position = position;
		vbar_handler->set_position(gui_list::position);
	}
}

/*! adds an item to the list
 *  @param text the text of the item
 *  @param id the id of the added item
 */
gui_list_item* gui_list::add_item(char* text, unsigned int id) {
	for(unsigned int i = 0; i < citems; i++) {
		if(items[i]->get_id() == id) {
			m->print(msg::MERROR, "gui_list.cpp", "gui list item with such an id already exists!");
			return 0;
		}
	}

	SDL_Color col;
	col.b = e->get_gui_style()->STYLE_FONT & 0xFF;
	col.g = (e->get_gui_style()->STYLE_FONT & 0xFF00) >> 8;
	col.r = (e->get_gui_style()->STYLE_FONT & 0xFF0000) >> 16;

	gui_text* gtext = new gui_text(e);

	gtext->set_init(false);
	gtext->set_id(id);
	gtext->new_text("vera.ttf", 12);
	gtext->set_point(g->cord_to_pnt(0,0));
	gtext->set_text(text);
	gtext->set_color(col);
	gtext->set_init(true);

	items[citems] = new gui_list_item(e);
	items[citems]->set_text_handler(gtext);
	items[citems]->set_text(text);
	items[citems]->set_id(id);
	items[citems]->set_point(0, 0);

	// set position, so that we see the last line
	gui_list::set_position(gui_list::get_position() + 1);

	citems++;

	gui_list::vbar_handler->set_max_lines(citems);

	return items[citems-1];
}

/*! deletes an item of the list
 *  @param id the id of the item
 */
void gui_list::delete_item(unsigned int id) {
	for(unsigned int i = 0; i < citems; i++) {
		if(items[i]->get_id() == id) {
			delete items[i]->get_text_handler();
			items[i]->clear();
		}
	}
}

/*! returns the amount of list items
 */
unsigned int gui_list::get_citems() {
	return gui_list::citems;
}

/*! selects an element of the list box
 *  @param x the x pos
 *  @param y the y pos
 */
void gui_list::select_pos(unsigned int x, unsigned int y) {
	for(unsigned int i = 0; i < citems; i++) {
		if(items[i]->get_id() == sid) {
            items[i]->get_text_handler()->set_color(e->get_gui_style()->STYLE_FONT);
		}
	}

	gfx::rect* r = new gfx::rect();
	core::pnt* pos = new core::pnt();
	pos->x = x;
	pos->y = y;
	r->x2 = gui_list::rectangle->x2 - 15;

	for(unsigned int i = 0; i < gui_list::citems; i++) {
			r->x1 = items[i]->get_point()->x - 1;
			r->y1 = items[i]->get_point()->y - 4;
			r->y2 = items[i]->get_point()->y + 13;

			if(g->is_pnt_in_rectangle(r, pos)) {
				sid = items[i]->get_id();
				items[i]->get_text_handler()->set_color(e->get_gui_style()->STYLE_FONT2);
				i = citems;
			}
	}

	delete r;
	delete pos;
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
	for(unsigned int i = 0; i < citems; i++) {
		if(items[i]->get_id() == gui_list::sid) {
            items[i]->get_text_handler()->set_color(e->get_gui_style()->STYLE_FONT);
		}
		else if(items[i]->get_id() == sid) {
			items[i]->get_text_handler()->set_color(e->get_gui_style()->STYLE_FONT2);
		}
	}

	gui_list::sid = sid;
}
