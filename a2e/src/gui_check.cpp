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

#include "gui_check.h"

/*! there is no function currently
 */
gui_check::gui_check(engine* e, gui_style* gs) : gui_object(e, gs) {
	gui_check::type = "checkbox";

	gui_check::checked = false;

	// get classes
	gui_check::e = e;
	gui_check::c = e->get_core();
	gui_check::m = e->get_msg();
	gui_check::g = e->get_gfx();
	gui_check::gs = gs;
}

/*! there is no function currently
 */
gui_check::~gui_check() {
	m->print(msg::MDEBUG, "gui_check.cpp", "freeing gui_check stuff");

	m->print(msg::MDEBUG, "gui_check.cpp", "gui_check stuff freed");
}

//! returns true if the check box is checked
bool gui_check::get_checked() {
	return gui_check::checked;
}

/*! sets the check boxes checked flag
 *  @param state the state of the check box we want to set
 */
void gui_check::set_checked(bool state) {
	if(gui_check::checked != state) redraw = true;
	gui_check::checked = state;
	gui_check::checked ? set_state("CLICKED") : set_state("NORMAL");
}
