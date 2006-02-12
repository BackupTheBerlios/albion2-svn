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

/*! there is no function currently
 */
gui_style::gui_style(gfx* g, msg* m) {
	gui_style::g = g;
	gui_style::m = m;
}

/*! there is no function currently
 */
gui_style::~gui_style() {
}

/*! initializes the gui style class and sets the screen surface
 *  @param screen the surface screen
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
			gui_style::STYLE_WINDOW_BG = g->get_color(0x7F7F7F);
			gui_style::STYLE_BARBG = g->get_color(0xBFBFBF);
			gui_style::STYLE_BG = g->get_color(0x7F7F7F);
			gui_style::STYLE_BG2 = g->get_color(0xC4C4C4);
			gui_style::STYLE_LIGHT = g->get_color(0xBFBFBF);
			gui_style::STYLE_DARK = g->get_color(0x000000);
			gui_style::STYLE_DARK2 = g->get_color(0x7F7F7F);
			gui_style::STYLE_INDARK = g->get_color(0x555555);
			gui_style::STYLE_ARROW = g->get_color(0x000000);
			gui_style::STYLE_FONT = g->get_color(0x000000);
			gui_style::STYLE_FONT2 = g->get_color(0x111111);
			gui_style::STYLE_SELECTED = g->get_color(0x8A8A8A);
			break;
		case gui_style::BLUE:
			gui_style::STYLE_WINDOW_BG = g->get_color(0x0005C0);
			gui_style::STYLE_BARBG = g->get_color(0xA8CDFF);
			gui_style::STYLE_BG = g->get_color(0x2E1FC0);
			gui_style::STYLE_BG2 = g->get_color(0xD2E5FF);
			gui_style::STYLE_LIGHT = g->get_color(0x8E67FF);
			gui_style::STYLE_DARK = g->get_color(0x1F058B);
			gui_style::STYLE_DARK2 = g->get_color(0x12009C);
			gui_style::STYLE_INDARK = g->get_color(0x2809A8);
			gui_style::STYLE_ARROW = g->get_color(0x2F66F1);
			gui_style::STYLE_FONT = g->get_color(0x000000);
			gui_style::STYLE_FONT2 = g->get_color(0xFFFFFF);
			gui_style::STYLE_SELECTED = g->get_color(0x0A1CC6);
			break;
		case gui_style::BLACKWHITE:
			gui_style::STYLE_WINDOW_BG = g->get_color(0xEEEEEE);
			gui_style::STYLE_BARBG = g->get_color(0xC9C9C9);
			gui_style::STYLE_BG = g->get_color(0x1F1F1F);
			gui_style::STYLE_BG2 = g->get_color(0xE0E0E0);
			gui_style::STYLE_LIGHT = g->get_color(0x7D7D7D);
			gui_style::STYLE_DARK = g->get_color(0x4B4B4B);
			gui_style::STYLE_DARK2 = g->get_color(0x313131);
			gui_style::STYLE_INDARK = g->get_color(0x191919);
			gui_style::STYLE_ARROW = g->get_color(0xFFFFFF);
			gui_style::STYLE_FONT = g->get_color(0x000000);
			gui_style::STYLE_FONT2 = g->get_color(0xFFFFFF);
			gui_style::STYLE_SELECTED = g->get_color(0xC9C9C9);
			break;
		default:
			gui_style::set_color_scheme(gui_style::WINDOWS);
			break;
	}
}

/*! returns the color specified by type
 *  @param type the colors type
 */
unsigned int gui_style::get_color(COLOR_TYPE type) {
	switch(type) {
		case gui_style::ARROW:
			return gui_style::STYLE_ARROW;
			break;
		case gui_style::BARBG:
			return gui_style::STYLE_BARBG;
			break;
		case gui_style::BG:
			return gui_style::STYLE_BG;
			break;
		case gui_style::BG2:
			return gui_style::STYLE_BG2;
			break;
		case gui_style::DARK:
			return gui_style::STYLE_DARK;
			break;
		case gui_style::DARK2:
			return gui_style::STYLE_DARK2;
			break;
		case gui_style::FONT:
			return gui_style::STYLE_FONT;
			break;
		case gui_style::FONT2:
			return gui_style::STYLE_FONT2;
			break;
		case gui_style::INDARK:
			return gui_style::STYLE_INDARK;
			break;
		case gui_style::LIGHT:
			return gui_style::STYLE_LIGHT;
			break;
		case gui_style::SELECTED:
			return gui_style::STYLE_SELECTED;
			break;
		case gui_style::WINDOW_BG:
			return gui_style::STYLE_WINDOW_BG;
			break;
		default:
			break;
	}

	return 0;
}
