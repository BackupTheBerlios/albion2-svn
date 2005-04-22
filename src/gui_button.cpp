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
	m.print(msg::MDEBUG, "gui_button.cpp", "freeing gui_button stuff");


	m.print(msg::MDEBUG, "gui_button.cpp", "gui_button stuff freed");
}

/*! draws the button
 *  @param is_pressed bool, if the button should be drawn as pressed or unpressed
 *  @param x specifies how much the element is moved on the x axis
 *  @param y specifies how much the element is moved on the y axis
 */
void gui_button::draw(bool is_pressed, unsigned int x, unsigned int y) {
	gfx::pnt* p1 = new gfx::pnt();
	gfx::pnt* p2 = new gfx::pnt();
	gfx::rect* r1 = new gfx::rect();

	if(!is_pressed) {
		g.pnt_to_rect(r1, gui_button::rectangle->x1 + x, gui_button::rectangle->y1 + y,
			gui_button::rectangle->x2 + x, gui_button::rectangle->y2 + y);

		// draw bg
		g.draw_filled_rectangle(engine_handler->get_screen(), r1,
			engine_handler->get_gstyle().STYLE_BG);

		// draw 2 colored border
		g.draw_2colored_rectangle(engine_handler->get_screen(), r1,
			engine_handler->get_gstyle().STYLE_LIGHT,
			engine_handler->get_gstyle().STYLE_DARK);

		g.cord_to_pnt(p1, gui_button::rectangle->x2-1 + x, gui_button::rectangle->y1+1 + y);
		g.cord_to_pnt(p2, gui_button::rectangle->x2-1 + x, gui_button::rectangle->y2-1 + y);

		// draw first line inside of the button
		g.draw_line(engine_handler->get_screen(), p1, p2,
			engine_handler->get_gstyle().STYLE_INDARK);

		g.cord_to_pnt(p1, gui_button::rectangle->x1+1 + x, gui_button::rectangle->y2-1 + y);
		g.cord_to_pnt(p2, gui_button::rectangle->x2 + x, gui_button::rectangle->y2-1 + y);

		// draw second line inside of the button
		g.draw_line(engine_handler->get_screen(), p1, p2,
			engine_handler->get_gstyle().STYLE_INDARK);

		if(icon == 0) {
			// draw text
			float fx, fy, z, ux, uy, uz = 0.0f;
			text_handler->get_font()->BBox(text_handler->get_text(), fx, fy, z, ux, uy, uz);
			unsigned int width = (unsigned int)(ux - fx);
			unsigned int heigth = (unsigned int)(uy - fy);
			unsigned int width_button = gui_button::rectangle->x2 - gui_button::rectangle->x1;
			unsigned int heigth_button = gui_button::rectangle->y2 - gui_button::rectangle->y1;
			// first we divide the buttons width/higth by 2, to get center point of the button.
			// then we also divide the texts width/higth by 2, to get the length we have to
			// subtract from the buttons center point, to make the text centered.
			// after that we just add the position to our lengths.
			g.cord_to_pnt(p1, gui_button::rectangle->x1 + (width_button/2 - width/2),
				gui_button::rectangle->y1 + (heigth_button/2 - heigth/2));
			text_handler->set_point(p1);
			text_handler->draw(x, y);
		}
		else {
			// draw icon
			unsigned int width_button = gui_button::rectangle->x2 - gui_button::rectangle->x1;
			unsigned int heigth_button = gui_button::rectangle->y2 - gui_button::rectangle->y1;

			switch(icon) {
				// up arrow icon
				case 1:
					// first line
					g.cord_to_pnt(p1, x + gui_button::rectangle->x1 + width_button/2 - 3,
						y + gui_button::rectangle->y1 + heigth_button/2 + 1);
					g.cord_to_pnt(p2, x + gui_button::rectangle->x1 + width_button/2 + 3,
						y + gui_button::rectangle->y1 + heigth_button/2 + 1);
					g.draw_line(engine_handler->get_screen(), p1, p2,
						engine_handler->get_gstyle().STYLE_ARROW);

					// second line
					g.cord_to_pnt(p1, x + gui_button::rectangle->x1 + width_button/2 - 2,
						y + gui_button::rectangle->y1 + heigth_button/2);
					g.cord_to_pnt(p2, x + gui_button::rectangle->x1 + width_button/2 + 2,
						y + gui_button::rectangle->y1 + heigth_button/2);
					g.draw_line(engine_handler->get_screen(), p1, p2,
						engine_handler->get_gstyle().STYLE_ARROW);

					// third line
					g.cord_to_pnt(p1, x + gui_button::rectangle->x1 + width_button/2 - 1,
						y + gui_button::rectangle->y1 + heigth_button/2 - 1);
					g.cord_to_pnt(p2, x + gui_button::rectangle->x1 + width_button/2 + 1,
						y + gui_button::rectangle->y1 + heigth_button/2 - 1);
					g.draw_line(engine_handler->get_screen(), p1, p2,
						engine_handler->get_gstyle().STYLE_ARROW);

					// fourth "line" (point)
					g.cord_to_pnt(p1, x + gui_button::rectangle->x1 + width_button/2,
						y + gui_button::rectangle->y1 + heigth_button/2 - 2);
					g.draw_point(engine_handler->get_screen(), p1,
						engine_handler->get_gstyle().STYLE_ARROW);
					break;
				case 2:
					// first line
					g.cord_to_pnt(p1, x + gui_button::rectangle->x1 + width_button/2 - 3,
						y + gui_button::rectangle->y1 + heigth_button/2);
					g.cord_to_pnt(p2, x + gui_button::rectangle->x1 + width_button/2 + 3,
						y + gui_button::rectangle->y1 + heigth_button/2);
					g.draw_line(engine_handler->get_screen(), p1, p2,
						engine_handler->get_gstyle().STYLE_ARROW);

					// second line
					g.cord_to_pnt(p1, x + gui_button::rectangle->x1 + width_button/2 - 2,
						y + gui_button::rectangle->y1 + heigth_button/2 + 1);
					g.cord_to_pnt(p2, x + gui_button::rectangle->x1 + width_button/2 + 2,
						y + gui_button::rectangle->y1 + heigth_button/2 + 1);
					g.draw_line(engine_handler->get_screen(), p1, p2,
						engine_handler->get_gstyle().STYLE_ARROW);

					// third line
					g.cord_to_pnt(p1, x + gui_button::rectangle->x1 + width_button/2 - 1,
						y + gui_button::rectangle->y1 + heigth_button/2 + 2);
					g.cord_to_pnt(p2, x + gui_button::rectangle->x1 + width_button/2 + 1,
						y + gui_button::rectangle->y1 + heigth_button/2 + 2);
					g.draw_line(engine_handler->get_screen(), p1, p2,
						engine_handler->get_gstyle().STYLE_ARROW);

					// fourth "line" (point)
					g.cord_to_pnt(p1, x + gui_button::rectangle->x1 + width_button/2,
						y + gui_button::rectangle->y1 + heigth_button/2 + 3);
					g.draw_point(engine_handler->get_screen(), p1,
						engine_handler->get_gstyle().STYLE_ARROW);
					break;
				case 3:
					// first line
					g.cord_to_pnt(p1, x + gui_button::rectangle->x1 + 3,
						y + gui_button::rectangle->y1 + 3);
					g.cord_to_pnt(p2, x + gui_button::rectangle->x2 - 3,
						y + gui_button::rectangle->y2 - 4);
					g.draw_line(engine_handler->get_screen(), p1, p2,
						engine_handler->get_gstyle().STYLE_FONT2);

					// second line
					g.cord_to_pnt(p1, x + gui_button::rectangle->x1 + 3,
						y + gui_button::rectangle->y2 - 3);
					g.cord_to_pnt(p2, x + gui_button::rectangle->x2 - 3,
						y + gui_button::rectangle->y1 + 2);
					g.draw_line(engine_handler->get_screen(), p1, p2,
						engine_handler->get_gstyle().STYLE_FONT2);
					break;
				default:
					break;
			}
		}
	}
	else {
		g.pnt_to_rect(r1, gui_button::rectangle->x1 + x, gui_button::rectangle->y1 + y,
		gui_button::rectangle->x2 + x, gui_button::rectangle->y2 + y);

		// draw bg
		g.draw_filled_rectangle(engine_handler->get_screen(), r1,
			engine_handler->get_gstyle().STYLE_BG);

		// draw 2 colored border
		g.draw_2colored_rectangle(engine_handler->get_screen(), r1,
			engine_handler->get_gstyle().STYLE_DARK,
			engine_handler->get_gstyle().STYLE_LIGHT);

		g.pnt_to_rect(r1, gui_button::rectangle->x1+1 + x,
			gui_button::rectangle->y1+1 + y,
			gui_button::rectangle->x2-1 + x,
			gui_button::rectangle->y2-1 + y);

		// draw 2 colored border
		g.draw_2colored_rectangle(engine_handler->get_screen(),
			r1, engine_handler->get_gstyle().STYLE_INDARK,
			engine_handler->get_gstyle().STYLE_BG);

		if(icon == 0) {
			// draw text
			float fx, fy, z, ux, uy, uz = 0.0f;
			text_handler->get_font()->BBox(text_handler->get_text(), fx, fy, z, ux, uy, uz);
			unsigned int width = (unsigned int)(ux - fx - 1.0f);
			unsigned int heigth = (unsigned int)(uy - fy - 1.0f);
			unsigned int width_button = gui_button::rectangle->x2 - gui_button::rectangle->x1;
			unsigned int heigth_button = gui_button::rectangle->y2 - gui_button::rectangle->y1;
			// first we divide the buttons width/higth by 2, to get center point of the button.
			// then we also divide the texts width/higth by 2, to get the length we have to
			// subtract from the buttons center point, to make the text centered.
			// after that we just add the position to our lengths.
			// +1, because we want the text drawn a bit more right and bottom
			g.cord_to_pnt(p1, gui_button::rectangle->x1 + (width_button/2 - width/2) + 1,
				gui_button::rectangle->y1 + (heigth_button/2 - heigth/2) + 1);
			text_handler->set_point(p1);
			text_handler->draw(x, y);
		}
		else {
			// draw icon
			unsigned int width_button = gui_button::rectangle->x2 - gui_button::rectangle->x1;
			unsigned int heigth_button = gui_button::rectangle->y2 - gui_button::rectangle->y1;

			switch(icon) {
				// up arrow icon
				case 1:
					// first line
					g.cord_to_pnt(p1, x + gui_button::rectangle->x1 + width_button/2 - 2,
						y + gui_button::rectangle->y1 + heigth_button/2 + 2);
					g.cord_to_pnt(p2, x + gui_button::rectangle->x1 + width_button/2 + 4,
						y + gui_button::rectangle->y1 + heigth_button/2 + 2);
					g.draw_line(engine_handler->get_screen(), p1, p2,
						engine_handler->get_gstyle().STYLE_ARROW);

					// second line
					g.cord_to_pnt(p1, x + gui_button::rectangle->x1 + width_button/2 - 1,
						y + gui_button::rectangle->y1 + heigth_button/2 + 1);
					g.cord_to_pnt(p2, x + gui_button::rectangle->x1 + width_button/2 + 3,
						y + gui_button::rectangle->y1 + heigth_button/2 + 1);
					g.draw_line(engine_handler->get_screen(), p1, p2,
						engine_handler->get_gstyle().STYLE_ARROW);

					// third line
					g.cord_to_pnt(p1, x + gui_button::rectangle->x1 + width_button/2,
						y + gui_button::rectangle->y1 + heigth_button/2);
					g.cord_to_pnt(p2, x + gui_button::rectangle->x1 + width_button/2 + 2,
						y + gui_button::rectangle->y1 + heigth_button/2);
					g.draw_line(engine_handler->get_screen(), p1, p2,
						engine_handler->get_gstyle().STYLE_ARROW);

					// fourth "line" (point)
					g.cord_to_pnt(p1, x + gui_button::rectangle->x1 + width_button/2 + 1,
						y + gui_button::rectangle->y1 + heigth_button/2 - 1);
					g.draw_point(engine_handler->get_screen(), p1,
						engine_handler->get_gstyle().STYLE_ARROW);
					break;
				case 2:
					// first line
					g.cord_to_pnt(p1, x + gui_button::rectangle->x1 + width_button/2 - 2,
						y + gui_button::rectangle->y1 + heigth_button/2 + 1);
					g.cord_to_pnt(p2, x + gui_button::rectangle->x1 + width_button/2 + 4,
						y + gui_button::rectangle->y1 + heigth_button/2 + 1);
					g.draw_line(engine_handler->get_screen(), p1, p2,
						engine_handler->get_gstyle().STYLE_ARROW);

					// second line
					g.cord_to_pnt(p1, x + gui_button::rectangle->x1 + width_button/2 - 1,
						y + gui_button::rectangle->y1 + heigth_button/2 + 2);
					g.cord_to_pnt(p2, x + gui_button::rectangle->x1 + width_button/2 + 3,
						y + gui_button::rectangle->y1 + heigth_button/2 + 2);
					g.draw_line(engine_handler->get_screen(), p1, p2,
						engine_handler->get_gstyle().STYLE_ARROW);

					// third line
					g.cord_to_pnt(p1, x + gui_button::rectangle->x1 + width_button/2,
						y + gui_button::rectangle->y1 + heigth_button/2 + 3);
					g.cord_to_pnt(p2, x + gui_button::rectangle->x1 + width_button/2 + 2,
						y + gui_button::rectangle->y1 + heigth_button/2 + 3);
					g.draw_line(engine_handler->get_screen(), p1, p2,
						engine_handler->get_gstyle().STYLE_ARROW);

					// fourth "line" (point)
					g.cord_to_pnt(p1, x + gui_button::rectangle->x1 + width_button/2 + 1,
						y + gui_button::rectangle->y1 + heigth_button/2 + 4);
					g.draw_point(engine_handler->get_screen(), p1,
						engine_handler->get_gstyle().STYLE_ARROW);
					break;
				case 3:
					// first line
					g.cord_to_pnt(p1, x + gui_button::rectangle->x1 + 4,
						y + gui_button::rectangle->y1 + 4);
					g.cord_to_pnt(p2, x + gui_button::rectangle->x2 - 2,
						y + gui_button::rectangle->y2 - 3);
					g.draw_line(engine_handler->get_screen(), p1, p2,
						engine_handler->get_gstyle().STYLE_FONT2);

					// second line
					g.cord_to_pnt(p1, x + gui_button::rectangle->x1 + 4,
						y + gui_button::rectangle->y2 - 2);
					g.cord_to_pnt(p2, x + gui_button::rectangle->x2 - 2,
						y + gui_button::rectangle->y1 + 3);
					g.draw_line(engine_handler->get_screen(), p1, p2,
						engine_handler->get_gstyle().STYLE_FONT2);
					break;
				default:
					break;
			}
		}
	}


	delete p1;
	delete p2;
	delete r1;
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

//! returns the buttons icon id
unsigned int gui_button::get_icon() {
	return gui_button::icon;
}

//! returns the buttons pressed bool
bool gui_button::get_pressed() {
	return gui_button::pressed;
}

/*! sets the buttons id
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
	gui_button::text_handler->set_text(text);
}

/*! sets the buttons icon
 *  @param icon the icon we want to set
 */
void gui_button::set_icon(unsigned int icon) {
	gui_button::icon = icon;
}

/*! sets the pressed bool
 *  @param pressed bool if the button is pressed currently
 */
void gui_button::set_pressed(bool pressed) {
	gui_button::pressed = pressed;
}
