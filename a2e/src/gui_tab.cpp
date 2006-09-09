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

#include "gui_tab.h"

/*! there is no function currently
 */
gui_tab::gui_tab(engine* e, gui_style* gs) : gui_object(e, gs) {
	gui_tab::type = "tab_border";

	gui_tab::active_tab = 0;

	gui_tab::tab_rect = new gfx::rect();

	// get classes
	gui_tab::e = e;
	gui_tab::c = e->get_core();
	gui_tab::m = e->get_msg();
	gui_tab::g = e->get_gfx();
	gui_tab::evt = e->get_event();
	gui_tab::gs = gs;
}

/*! there is no function currently
 */
gui_tab::~gui_tab() {
	m->print(msg::MDEBUG, "gui_tab.cpp", "freeing gui_tab stuff");

	delete gui_tab::tab_rect;

	gui_tab::tabs.clear();

	m->print(msg::MDEBUG, "gui_tab.cpp", "gui_tab stuff freed");
}

/*! draws the tab
 *  @param x specifies how much the element is moved on the x axis
 *  @param y specifies how much the element is moved on the y axis
 */
void gui_tab::draw(unsigned int x, unsigned int y) {
	draw_object(x, y);

	unsigned int width = 0;
	unsigned int text_width = 0;
	tab_rect->x1 = 0;
	tab_rect->x2 = 0;
	tab_rect->y1 = 0;
	tab_rect->y2 = 20;
	for(list<gui_tab::tab*>::iterator iter = tabs.begin(); iter != tabs.end(); iter++) {
		text_handler->set_text((char*)(*iter)->title.c_str());
		text_width = text_handler->get_text_width()+20;
		tab_rect->x2 = text_width;
		gs->render_gui_element("tab", ((*iter)->id == active_tab ? "CLICKED" : "NORMAL"), tab_rect, rectangle->x1+x+width, rectangle->y1+y);
		width += text_width;
	}
}

unsigned int gui_tab::add_tab(const char* title) {
	tabs.push_back(*new gui_tab::tab*());
	tabs.back() = new gui_tab::tab();

	tabs.back()->title = title;
	tabs.back()->id = (unsigned int)tabs.size()-1;

	return tabs.back()->id;
}

unsigned int gui_tab::get_tab_id(const char* title) {
	for(list<gui_tab::tab*>::iterator iter = tabs.begin(); iter != tabs.end(); iter++) {
		if(strcmp((*iter)->title.c_str(), title) == 0) {
			return (*iter)->id;
		}
	}
	m->print(msg::MERROR, "gui_tab.cpp", "get_tab_id(): no tab with the title \"%s\" exists!", title);
	return 0;
}

void gui_tab::set_active_tab(unsigned int id) {
	if(gui_tab::active_tab != id) {
		set_redraw(true);
		evt->add_gui_event(event::TAB_SELECTED, gui_tab::id);
	}
	gui_tab::active_tab = id;
}

unsigned int gui_tab::get_active_tab() {
	return gui_tab::active_tab;
}

void gui_tab::select_pos(unsigned int x, unsigned int y) {
	x -= rectangle->x1;
	y -= rectangle->y1;
	unsigned int text_width = 0;
	for(list<gui_tab::tab*>::iterator iter = tabs.begin(); iter != tabs.end(); iter++) {
		text_handler->set_text((char*)(*iter)->title.c_str());
		text_width += text_handler->get_text_width()+20;
		// check if the x coordinate lays inside of that tab width
		if(x <= text_width) {
			// if so, make this the new active tab
			if(active_tab != (*iter)->id) set_redraw(true);
			active_tab = (*iter)->id;
			return;
		}
	}
}
