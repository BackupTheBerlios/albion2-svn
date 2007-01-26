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

#include "gui_toggle.h"

/*! there is no function currently
 */
gui_toggle::gui_toggle(engine* e, gui_style* gs) : gui_object(e, gs) {
	gui_toggle::type = "toggle";

	gui_toggle::toggled = false;

	gui_toggle::img_scale = true;

	// get classes
	gui_toggle::e = e;
	gui_toggle::c = e->get_core();
	gui_toggle::m = e->get_msg();
	gui_toggle::g = e->get_gfx();
	gui_toggle::gs = gs;
}

/*! there is no function currently
 */
gui_toggle::~gui_toggle() {
	m->print(msg::MDEBUG, "gui_toggle.cpp", "freeing gui_toggle stuff");

	m->print(msg::MDEBUG, "gui_toggle.cpp", "gui_toggle stuff freed");
}

/*! sets image scaling to state
 *  @param state the scaling state
 */
void gui_toggle::set_image_scaling(bool state) {
	if(gui_toggle::img_scale != state) gui_toggle::redraw = true;
	gui_toggle::img_scale = state;
	if(img != NULL) {
		img->set_scaling(state);
	}
}

//! returns the image scale flag
bool gui_toggle::get_image_scaling() {
	return gui_toggle::img_scale;
}

//! returns true if the check box is checked
bool gui_toggle::get_toggled() {
	return gui_toggle::toggled;
}

/*! sets the check boxes checked flag
 *  @param state the state of the check box we want to set
 */
void gui_toggle::set_toggled(bool state) {
	if(gui_toggle::toggled != state) redraw = true;
	gui_toggle::toggled = state;
	gui_toggle::toggled ? set_state("CLICKED") : set_state("NORMAL");
}
