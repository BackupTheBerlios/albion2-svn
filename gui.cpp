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

#include "gui.h"
#include "gfx.h"
#include "msg.h"
#include "core.h"
#include "engine.h"
#include "gui_button.h"

/*! there is no function currently
*/
gui::gui() {
}

/*! there is no function currently
*/
gui::~gui() {
}

/*! initializes the gui class and sets an engine and event handler
 *  @param iengine the engine we want to handle
 *  @param ievent the event we want to handle
 */
void gui::init(engine &iengine, event &ievent) {
	gui::event_handler = &ievent;
	gui::engine_handler = &iengine;

	celements = 0;
	cbuttons = 0;
	ctexts = 0;

	if(gui::engine_handler->get_screen() == NULL) { m.print(msg::MERROR, "gui.cpp", "SDL_Surface does not exist!"); }
	else { gui::gui_surface = gui::engine_handler->get_screen(); }

	// reserve memory for 512 gui elements
	gui::gui_elements = (gui::gui_element*)malloc(sizeof(gui::gui_element)*512);

	// reserve memory for 128 gui button elements
	for(unsigned int i = 0; i < 128; i++) {
        gui_buttons[i] = new gui_button();
	}

	// reserve memory for 128 gui text elements
	for(unsigned int i = 0; i < 128; i++) {
        gui_texts[i] = new gui_text();
	}

	if(TTF_Init()==-1) {
		m.print(msg::MERROR, "gui.cpp", "TTF_Init: %s", TTF_GetError());
	}
}

/*! draws all gui elements
 */
void gui::draw() {
	for(unsigned int i = 0; i < celements; i++) {
		if(gui::gui_elements[i].is_drawn == true) {
			switch(gui::gui_elements[i].type) {
				case gui::BUTTON:
					if(g.is_pnt_in_rectangle(gui::gui_buttons[gui::gui_elements[i].num]->get_rectangle(),
						g.cord_to_pnt(event_handler->get_lm_pressed_x(),
						event_handler->get_lm_pressed_y()))) {
						gui::gui_buttons[gui::gui_elements[i].num]->draw_button(true);
					}
					else {
						gui::gui_buttons[gui::gui_elements[i].num]->draw_button(false);
					}
					break;
				case gui::TEXT:
					gui::gui_texts[gui::gui_elements[i].num]->draw_text();
					break;
			}
		}
	}
}

/*! adds a gui button element and returns it
 *  @param rectangle the buttons rectangle
 *  @param id the buttons (a2e event) id
 *  @param text the buttons text
 */
gui_button* gui::add_button(gfx::rect* rectangle, unsigned int id, char* text) {
	gui::gui_elements[celements].id = id;
	gui::gui_elements[celements].type = gui::BUTTON;
	gui::gui_elements[celements].num = cbuttons;
	gui::gui_elements[celements].is_drawn = true;
	
	// celements has to be incremented _before_ we add the text, otherwise
	// our button stuff will be overwritten
	celements++;

	gui::gui_buttons[cbuttons]->set_text_handler(add_text("vera.ttf", 12, text, 0x000000, g.cord_to_pnt(0,0), id+0xFFFF));
	// don't draw our text automatically
	// celements-1, because our text element, is the last initialized element
	gui::gui_elements[celements-1].is_drawn = false;

	gui::gui_buttons[cbuttons]->set_engine_handler(gui::engine_handler);
	gui::gui_buttons[cbuttons]->set_id(id);
	gui::gui_buttons[cbuttons]->set_rectangle(rectangle);
	gui::gui_buttons[cbuttons]->set_text(text);

	cbuttons++;

	return gui_buttons[cbuttons-1];
}

/*! adds a gui text element and returns it
 *  @param font_name the font that should be used
 *  @param font_size the font size (in px?)
 *  @param text the text itself
 *  @param color the color of the text
 *  @param point the starting point of the text
 *  @param id the texts (a2e event) id
 */
gui_text* gui::add_text(char* font_name, unsigned int font_size, char* text,
				   unsigned int color, gfx::pnt* point, unsigned int id) {
	TTF_Font* font = gui_texts[ctexts]->open_font(font_name, font_size);
	SDL_Color col;
	col.r = (color & 0xFF0000) >> 16;
	col.g = (color & 0x00FF00) >> 8;
	col.b = color & 0x0000FF;

	gui::gui_elements[celements].id = id;
	gui::gui_elements[celements].type = gui::TEXT;
	gui::gui_elements[celements].num = ctexts;
	gui::gui_elements[celements].is_drawn = true;

	gui::gui_texts[ctexts]->set_engine_handler(gui::engine_handler);
	gui::gui_texts[ctexts]->set_id(id);
	// there were problems with TTF_RenderText_Solid, so it was just
	// changed to TTF_RenderText_Blended ;) its a bit slower, but nicer ;)
	gui::gui_texts[ctexts]->set_surface(TTF_RenderText_Blended(font, text, col));
	gui::gui_texts[ctexts]->set_point(point);

	gui::gui_texts[ctexts]->close_font(font);

	celements++;
	ctexts++;

	return gui::gui_texts[ctexts-1];
}

//! returns the guis surface
SDL_Surface* gui::get_gui_surface() {
	return gui::gui_surface;
}
