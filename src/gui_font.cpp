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

#include "gui_font.h"

/*! there is no function currently
 */
gui_font::gui_font(msg* m) {

	// get classes
	gui_font::m = m;
}

/*! there is no function currently
 */
gui_font::~gui_font() {
	m->print(msg::MDEBUG, "gui_text.cpp", "freeing gui_font stuff");

	fonts.clear();

	m->print(msg::MDEBUG, "gui_text.cpp", "gui_font stuff freed");
}

gui_font::font* gui_font::add_font(char* filename, unsigned int size, unsigned int color) {
	// check if we already loaded that font
	for(list<gui_font::font>::iterator fiter = fonts.begin(); fiter != fonts.end(); fiter++) {
		if(strcmp(fiter->font_name.c_str(), filename) == 0 &&
			fiter->font_size == size &&
			fiter->color == color) {
			return &*fiter;
		}
	}

	fonts.push_back(*new gui_font::font());
	fonts.back().font_name = filename;
	fonts.back().ttf_font = new FTGLTextureFont(filename);
	fonts.back().font_size = size;
	fonts.back().ttf_font->FaceSize(size);
	fonts.back().color = color;

	return &fonts.back();
}
