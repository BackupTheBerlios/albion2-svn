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
#include "gui_style.h"
#include "gui_list_item.h"
#include "gui_vbar.h"
#include "gfx.h"
#include "msg.h"
#include "core.h"
#include "engine.h"

/*! there is no function currently
 */
gui_list::gui_list() {
	is_active = false;

	citems = 0;

	drawable_items = 0;
}

/*! there is no function currently
 */
gui_list::~gui_list() {
}

/*! draws the list box
 */
void gui_list::draw_list() {
	// draw bg
	g.draw_filled_rectangle(engine_handler->get_screen(),
		gui_list::rectangle, gstyle.STYLE_BG2);

	// draw 2 colored border
	g.draw_2colored_rectangle(engine_handler->get_screen(),
		gui_list::rectangle,
		gstyle.STYLE_INDARK, gstyle.STYLE_LIGHT);

	// draw 2 colored border
	gfx::rect* r1 = (gfx::rect*)malloc(sizeof(gfx::rect));
	g.pnt_to_rect(r1, gui_list::rectangle->x1+1,
		gui_list::rectangle->y1+1,
		gui_list::rectangle->x2-1,
		gui_list::rectangle->y2-1);
	g.draw_2colored_rectangle(engine_handler->get_screen(),
		r1, gstyle.STYLE_DARK, gstyle.STYLE_DARK2);
	free(r1);

	// draw items
	// we need a height of 22px for each item
	unsigned int list_box_heigth = gui_list::rectangle->y2 - gui_list::rectangle->y1;
	gui_list::drawable_items = (list_box_heigth - (list_box_heigth % 22)) / 22;
	gui_list::vbar_handler->set_shown_lines(gui_list::drawable_items);
	unsigned int* ids = (unsigned int*)malloc(sizeof(unsigned int) * citems);
	unsigned int* new_ids = (unsigned int*)malloc(sizeof(unsigned int) * citems);
	for(unsigned int i = 0; i < citems; i++) {
		ids[i] = items[i]->get_id();
	}
	unsigned int c = citems;
	while(c != 0) {
		unsigned int highest = 0;
		unsigned int tmp_id = 0;
		unsigned int tmp_num = 0;
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

	gfx::pnt* p1 = (gfx::pnt*)malloc(sizeof(gfx::pnt));
	for(unsigned int i = 0; i < drawable_items; i++) {
		for(unsigned int j = 0; j < citems; j++) {
			if(items[j]->get_id() == new_ids[i + position]) {
				g.cord_to_pnt(p1, gui_list::rectangle->x1 + 3,
					gui_list::rectangle->y1 + 3 + i*22);
				items[j]->set_point(p1);
				items[j]->draw_list_item();
			}
		}
	}
	free(p1);

	free(ids);
	free(new_ids);

	// vbar handling
	gui_list::set_position(vbar_handler->get_position());
}

/*! creates a engine_handler -> a pointer to the engine class
 *  @param iengine the engine we want to handle
 */
void gui_list::set_engine_handler(engine* iengine) {
	gui_list::engine_handler = iengine;
	gstyle.init(gui_list::engine_handler);
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
		gui_list::position = gui_list::citems - gui_list::drawable_items;
		vbar_handler->set_position(gui_list::position);
	}
	else {
        gui_list::position = position;
		vbar_handler->set_position(gui_list::position);
	}
}

gui_list_item* gui_list::add_item(char* text, unsigned int id) {
	for(unsigned int i = 0; i < citems; i++) {
		if(items[i]->get_id() == id) {
			m.print(m.MERROR, "gui_list.cpp", "gui list item with such an id already exists!");
			return 0;
		}
	}

	gui_text* gtext = (gui_text*)malloc(sizeof(gui_text));

	TTF_Font* font = gtext->open_font("vera.ttf", 12);
	SDL_Color col;
	col.b = 0;
	col.g = 0;
	col.r = 0;

	gtext->set_init(false);
	gtext->set_engine_handler(gui_list::engine_handler);
	gtext->set_id(id);
	// there were problems with TTF_RenderText_Solid, so it was just
	// changed to TTF_RenderText_Blended ;) its a bit slower, but nicer ;)
	gtext->set_surface(TTF_RenderText_Blended(font, text, col));
	gtext->set_point(g.cord_to_pnt(0,0));
	gtext->set_text(text);
	gtext->set_color(col);
	gtext->set_font_name("vera.ttf");
	gtext->set_font_size(12);
	gtext->set_init(true);
	gtext->close_font(font);

	items[citems] = new gui_list_item();
	items[citems]->set_text_handler(gtext);
	items[citems]->set_text(text);
	items[citems]->set_id(id);
	items[citems]->set_point(g.cord_to_pnt(0,0));

	// set position, so that we see the last line
	//gui_list::set_position(gui_list::get_position() + 1);

	citems++;

	gui_list::vbar_handler->set_max_lines(citems);

	return items[citems-1];
}

void gui_list::delete_item(unsigned int id) {
	unsigned int tmp_count = citems;

	for(unsigned int i = 0; i < citems; i++) {
		if(items[i]->get_id() == id) {
			delete items[i];
			citems--;
		}
	}

	if(tmp_count == citems) {
		m.print(m.MERROR, "gui_list.cpp", "can't delete item with such an id!");
	}
}

unsigned int gui_list::get_citems() {
	return gui_list::citems;
}
