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

#include "gui_style.h"
#include "gfx.h"
#include "msg.h"
#include "core.h"
#include "engine.h"

/*! there is no function currently
 */
gui_style::gui_style() {
}

/*! there is no function currently
 */
gui_style::~gui_style() {
}

/*! initializes the gui style class and sets an engine handler
 *  @param iengine the engine we want to handle
 */
void gui_style::init(engine* iengine) {
	gui_style::engine_handler = iengine;

	gui_style::STYLE_BG = g.get_color(engine_handler->get_screen(), 0x7F7F7F);
	gui_style::STYLE_LIGHT = g.get_color(engine_handler->get_screen(), 0xBFBFBF);
	gui_style::STYLE_DARK = g.get_color(engine_handler->get_screen(), 0x000000);
	gui_style::STYLE_INDARK = g.get_color(engine_handler->get_screen(), 0x555555);
}
