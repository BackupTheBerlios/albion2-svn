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

#include "gui_image.h"

/*! there is no function currently
 */
gui_image::gui_image(engine* e, gui_style* gs) : gui_object(e, gs) {
	gui_image::type = "image";

	// get classes
	gui_image::e = e;
	gui_image::c = e->get_core();
	gui_image::m = e->get_msg();
	gui_image::g = e->get_gfx();
	gui_image::gs = gs;
}

/*! there is no function currently
 */
gui_image::~gui_image() {
	m->print(msg::MDEBUG, "gui_image.cpp", "freeing gui_image stuff");

	m->print(msg::MDEBUG, "gui_image.cpp", "gui_image stuff freed");
}
