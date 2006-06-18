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

#include "gui_mltext.h"

/*! there is no function currently
 */
gui_mltext::gui_mltext(engine* e, gui_style* gs) : gui_object(e, gs) {
	gui_mltext::type = "mltext";

	// get classes
	gui_mltext::e = e;
	gui_mltext::c = e->get_core();
	gui_mltext::m = e->get_msg();
	gui_mltext::g = e->get_gfx();
	gui_mltext::gs = gs;
}

/*! there is no function currently
 */
gui_mltext::~gui_mltext() {
	m->print(msg::MDEBUG, "gui_mltext.cpp", "freeing gui_mltext stuff");

	m->print(msg::MDEBUG, "gui_mltext.cpp", "gui_mltext stuff freed");
}

/*! draws the multi line text
 *  @param x specifies how much the element is moved on the x axis
 *  @param y specifies how much the element is moved on the y axis
 */
void gui_mltext::draw(unsigned int x, unsigned int y) {
	draw_object(x, y);

	g->set_scissor(x + rectangle->x1, y + rectangle->y1, x + rectangle->x2, y + rectangle->y2);
	g->begin_scissor();
	size_t pos = 0;
	size_t npos = 0;
	unsigned int l = 0;
	while((pos = text.find("\n", npos)) != string::npos) {
		text_handler->draw(text.substr(npos, pos-npos).c_str(), x + rectangle->x1 + 4, y + rectangle->y1 + 5 + l * 18);
		npos = pos+1;
		l++;
	}
	text_handler->draw(text.substr(npos, (l > 0 ? pos-npos : text.length())).c_str(), x + rectangle->x1 + 4, y + rectangle->y1 + 5 + l * 18);
	g->end_scissor();
}
