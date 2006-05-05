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

#include "gui_button.h"

/*! there is no function currently
 */
gui_button::gui_button(engine* e, gui_style* gs) : gui_object(e, gs) {
	gui_button::type = "button";

	// get classes
	gui_button::e = e;
	gui_button::c = e->get_core();
	gui_button::m = e->get_msg();
	gui_button::g = e->get_gfx();
	gui_button::gs = gs;

	gui_button::img_scale = true;
}

/*! there is no function currently
 */
gui_button::~gui_button() {
	m->print(msg::MDEBUG, "gui_button.cpp", "freeing gui_button stuff");

	m->print(msg::MDEBUG, "gui_button.cpp", "gui_button stuff freed");
}

/*! sets image scaling to state
 *  @param state the scaling state
 */
void gui_button::set_image_scaling(bool state) {
	gui_button::img_scale = state;
	if(img != NULL) {
		img->set_scaling(state);
	}
}

//! returns the image scale flag
bool gui_button::get_image_scaling() {
	return gui_button::img_scale;
}
