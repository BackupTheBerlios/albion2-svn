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
void gui_style::init(SDL_Surface* screen) {
	gui_style::screen = screen;
}

/*! sets the style color scheme
 *  @param scheme the scheme we want to set
 */
void gui_style::set_color_scheme(COLOR_SCHEME scheme) {
	switch(scheme) {
		case gui_style::WINDOWS:
			gui_style::STYLE_WINDOW_BG = g.get_color(gui_style::screen, 0x7F7F7F);
			gui_style::STYLE_BARBG = g.get_color(gui_style::screen, 0xBFBFBF);
			gui_style::STYLE_BG = g.get_color(gui_style::screen, 0x7F7F7F);
			gui_style::STYLE_BG2 = g.get_color(gui_style::screen, 0xC4C4C4);
			gui_style::STYLE_LIGHT = g.get_color(gui_style::screen, 0xBFBFBF);
			gui_style::STYLE_DARK = g.get_color(gui_style::screen, 0x000000);
			gui_style::STYLE_DARK2 = g.get_color(gui_style::screen, 0x7F7F7F);
			gui_style::STYLE_INDARK = g.get_color(gui_style::screen, 0x555555);
			gui_style::STYLE_ARROW = g.get_color(gui_style::screen, 0x000000);
			gui_style::STYLE_FONT = g.get_color(gui_style::screen, 0x000000);
			gui_style::STYLE_FONT2 = g.get_color(gui_style::screen, 0x111111);
			break;
		case gui_style::BLUE:
			gui_style::STYLE_WINDOW_BG = g.get_color(gui_style::screen, 0x0005C0);
			gui_style::STYLE_BARBG = g.get_color(gui_style::screen, 0xA8CDFF);
			gui_style::STYLE_BG = g.get_color(gui_style::screen, 0x2E1FC0);
			gui_style::STYLE_BG2 = g.get_color(gui_style::screen, 0xD2E5FF);
			gui_style::STYLE_LIGHT = g.get_color(gui_style::screen, 0x8E67FF);
			gui_style::STYLE_DARK = g.get_color(gui_style::screen, 0x1F058B);
			gui_style::STYLE_DARK2 = g.get_color(gui_style::screen, 0x12009C);
			gui_style::STYLE_INDARK = g.get_color(gui_style::screen, 0x2809A8);
			gui_style::STYLE_ARROW = g.get_color(gui_style::screen, 0x2F66F1);
			gui_style::STYLE_FONT = g.get_color(gui_style::screen, 0x000000);
			gui_style::STYLE_FONT2 = g.get_color(gui_style::screen, 0xFFFFFF);
			break;
		case gui_style::BLACKWHITE:
			gui_style::STYLE_WINDOW_BG = g.get_color(gui_style::screen, 0xEEEEEE);
			gui_style::STYLE_BARBG = g.get_color(gui_style::screen, 0xC9C9C9);
			gui_style::STYLE_BG = g.get_color(gui_style::screen, 0x1F1F1F);
			gui_style::STYLE_BG2 = g.get_color(gui_style::screen, 0xE0E0E0);
			gui_style::STYLE_LIGHT = g.get_color(gui_style::screen, 0x7D7D7D);
			gui_style::STYLE_DARK = g.get_color(gui_style::screen, 0x4B4B4B);
			gui_style::STYLE_DARK2 = g.get_color(gui_style::screen, 0x313131);
			gui_style::STYLE_INDARK = g.get_color(gui_style::screen, 0x191919);
			gui_style::STYLE_ARROW = g.get_color(gui_style::screen, 0xFFFFFF);
			gui_style::STYLE_FONT = g.get_color(gui_style::screen, 0x000000);
			gui_style::STYLE_FONT2 = g.get_color(gui_style::screen, 0xFFFFFF);
			break;
		default:
			gui_style::set_color_scheme(gui_style::WINDOWS);
			break;
	}
}
