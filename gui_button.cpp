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
#include "gui_style.h"
#include "gfx.h"
#include "msg.h"
#include "core.h"
#include "engine.h"

/*! there is no function currently
 */
gui_button::gui_button() {
}

/*! there is no function currently
 */
gui_button::~gui_button() {
}

/*! draws the button
 *  @param is_pressed bool, if the button should be drawn as pressed or unpressed
 */
void gui_button::draw_button(bool is_pressed) {
	gstyle.init(engine_handler);

	if(!is_pressed) {
		// draw bg
		g.draw_filled_rectangle(engine_handler->get_screen(),
			g.pnt_to_rect(gui_button::rectangle->x1,
			gui_button::rectangle->y1,
			gui_button::rectangle->x2,
			gui_button::rectangle->y2), gstyle.STYLE_BG);

		// draw 2 colored border
		g.draw_2colored_rectangle(engine_handler->get_screen(),
			g.pnt_to_rect(gui_button::rectangle->x1,
			gui_button::rectangle->y1,
			gui_button::rectangle->x2,
			gui_button::rectangle->y2),
			gstyle.STYLE_LIGHT, gstyle.STYLE_DARK);

		// draw first line inside of the button
		g.draw_line(engine_handler->get_screen(),
			g.cord_to_pnt(gui_button::rectangle->x2,
			gui_button::rectangle->y1+1),
			g.cord_to_pnt(gui_button::rectangle->x2,
			gui_button::rectangle->y2), gstyle.STYLE_INDARK);

		// draw second line inside of the button
		g.draw_line(engine_handler->get_screen(),
			g.cord_to_pnt(gui_button::rectangle->x1+1,
			gui_button::rectangle->y2),
			g.cord_to_pnt(gui_button::rectangle->x2-1,
			gui_button::rectangle->y2), gstyle.STYLE_INDARK);

		// draw text
		unsigned int width = text_handler->get_surface()->w;
		unsigned int heigth = text_handler->get_surface()->h;
		unsigned int width_button = gui_button::rectangle->x2 - gui_button::rectangle->x1;
		unsigned int heigth_button = gui_button::rectangle->y2 - gui_button::rectangle->y1;
		// first we divide the buttons width/higth by 2, to get center point of the button.
		// then we also divide the texts width/higth by 2, to get the length we have to
		// subtract from the buttons center point, to make the text centered.
		// after that we just add the position to our lengths.
		text_handler->set_point(g.cord_to_pnt(gui_button::rectangle->x1 + (width_button/2 - width/2),
			gui_button::rectangle->y1 + (heigth_button/2 - heigth/2)));
		text_handler->draw_text();

	}
	else {

		// draw bg
		g.draw_filled_rectangle(engine_handler->get_screen(),
			g.pnt_to_rect(gui_button::rectangle->x1,
			gui_button::rectangle->y1,
			gui_button::rectangle->x2,
			gui_button::rectangle->y2), gstyle.STYLE_BG);

		// draw 2 colored border
		g.draw_2colored_rectangle(engine_handler->get_screen(),
			g.pnt_to_rect(gui_button::rectangle->x1,
			gui_button::rectangle->y1,
			gui_button::rectangle->x2,
			gui_button::rectangle->y2),
			gstyle.STYLE_DARK, gstyle.STYLE_LIGHT);

		// draw 2 colored border
		g.draw_2colored_rectangle(engine_handler->get_screen(),
			g.pnt_to_rect(gui_button::rectangle->x1+1,
			gui_button::rectangle->y1+1,
			gui_button::rectangle->x2-1,
			gui_button::rectangle->y2-1),
			gstyle.STYLE_INDARK, gstyle.STYLE_BG);

		// draw outline border
		/*g.draw_rectangle(engine_handler->get_screen(),
			g.pnt_to_rect(gui_button::rectangle->x1-1,
			gui_button::rectangle->y1-1,
			gui_button::rectangle->x2+1,
			gui_button::rectangle->y2+1),
			gstyle.STYLE_DARK);*/

		// draw text
		unsigned int width = text_handler->get_surface()->w;
		unsigned int heigth = text_handler->get_surface()->h;
		unsigned int width_button = gui_button::rectangle->x2 - gui_button::rectangle->x1;
		unsigned int heigth_button = gui_button::rectangle->y2 - gui_button::rectangle->y1;
		// first we divide the buttons width/higth by 2, to get center point of the button.
		// then we also divide the texts width/higth by 2, to get the length we have to
		// subtract from the buttons center point, to make the text centered.
		// after that we just add the position to our lengths.
		// +1, because we want the text drawn a bit more right and bottom
		text_handler->set_point(g.cord_to_pnt(gui_button::rectangle->x1 + (width_button/2 - width/2) + 1,
			gui_button::rectangle->y1 + (heigth_button/2 - heigth/2) + 1));
		text_handler->draw_text();

	}
}

/*! creates a engine_handler -> a pointer to the engine class
 *  @param iengine the engine we want to handle
 */
void gui_button::set_engine_handler(engine* iengine) {
	gui_button::engine_handler = iengine;
}

/*! creates a text_handler -> a pointer to the gui_text class
 *  @param itext the gui_text we want to handle
 */
void gui_button::set_text_handler(gui_text* itext) {
	gui_button::text_handler = itext;
}

//! returns the buttons id
unsigned int gui_button::get_id() {
	return gui_button::id;
}

//! returns the buttons rectangle
gfx::rect* gui_button::get_rectangle() {
	return gui_button::rectangle;
}

//! returns the buttons text
char* gui_button::get_text() {
	return gui_button::text;
}

/*! sets the text id
 *  @param id the id we want to set
 */
void gui_button::set_id(unsigned int id) {
	gui_button::id = id;
}

/*! sets the buttons rectangle
 *  @param rectangle the rectangle we want to set
 */
void gui_button::set_rectangle(gfx::rect* rectangle) {
	gui_button::rectangle = rectangle;
}

/*! sets the buttons text
 *  @param text the text we want to set
 */
void gui_button::set_text(char* text) {
	gui_button::text = text;
}
