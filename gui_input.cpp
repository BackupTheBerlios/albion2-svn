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

#include "gui_input.h"
#include "gfx.h"
#include "msg.h"
#include "core.h"
#include "engine.h"

/*! there is no function currently
 */
gui_input::gui_input() {
	blink_time = SDL_GetTicks();
	is_active = false;

	// 1024 chars
	gui_input::text = (char*)malloc(1024);
}

/*! there is no function currently
 */
gui_input::~gui_input() {
}

/*! draws the input boxs
 */
void gui_input::draw_input() {
	// draw bg
	g.draw_filled_rectangle(engine_handler->get_screen(),
		gui_input::rectangle, engine_handler->get_gstyle().STYLE_BG2);

	// draw 2 colored border
	g.draw_2colored_rectangle(engine_handler->get_screen(),
		gui_input::rectangle,
		engine_handler->get_gstyle().STYLE_INDARK,
		engine_handler->get_gstyle().STYLE_LIGHT);

	// draw 2 colored border
	gfx::rect* r1 = (gfx::rect*)malloc(sizeof(gfx::rect));
	g.pnt_to_rect(r1, gui_input::rectangle->x1+1,
		gui_input::rectangle->y1+1,
		gui_input::rectangle->x2-1,
		gui_input::rectangle->y2-1);
	g.draw_2colored_rectangle(engine_handler->get_screen(),
		r1, engine_handler->get_gstyle().STYLE_DARK,
		engine_handler->get_gstyle().STYLE_DARK2);
	free(r1);

	// get the input box text width
	float glyph_width;
	float text_width = 0;
	float after_text_width = 0;
	char* tmp_text = text_handler->get_text();
	char tmp_char[1];
	for(unsigned int i = 0; i < strlen(text_handler->get_text()); i++) {
		sprintf(tmp_char, "%c", tmp_text[i]);
		glyph_width = text_handler->get_font()->Advance(tmp_char);
		if(i < text_pos) {
            text_width += glyph_width;
		}
		else {
			after_text_width += glyph_width;
		}
	}

	// draw text
	gfx::pnt* p1 = (gfx::pnt*)malloc(sizeof(gfx::pnt));
	unsigned int width = text_handler->get_text_width();
	unsigned int heigth = text_handler->get_text_height();
	// -6, because we have a border of 2px*2 and an empty pixel*2
	unsigned int width_input_box = gui_input::rectangle->x2 - gui_input::rectangle->x1 - 6;
	unsigned int heigth_input_box = gui_input::rectangle->y2 - gui_input::rectangle->y1;
	// if the current text cursor position isn't in the available space of the text box,
	// than we define width_diff to set the position _into_ the available space
	unsigned int width_diff = 0;

	// is text surface bigger than the available space of the text box?
	if(width >= width_input_box) {
		// than we have to make the (shown) surface smaller
		
		// is the current text cursor position behind the normal text box width?
		if((unsigned int)text_width >= width_input_box) {
			width_diff = (unsigned int)text_width - width_input_box;

			gfx::rect* dstrect = (gfx::rect*)malloc(sizeof(gfx::rect));
			dstrect->x1 = gui_input::rectangle->x1 + 3;
			dstrect->y1 = gui_input::rectangle->y1 + 3;
			dstrect->x2 = gui_input::rectangle->x1 + 3 + width_input_box;
			dstrect->y2 = gui_input::rectangle->y1 + 3 + heigth_input_box;
			gfx::rect* srcrect = (gfx::rect*)malloc(sizeof(gfx::rect));
			srcrect->x1 = width_diff;
			srcrect->y1 = 0;
			srcrect->x2 = width_diff + width_input_box;
			srcrect->y2 = heigth_input_box;
			
			/*text_handler->set_blit_rectangles(srcrect, dstrect);
			text_handler->set_blit(true);*/
			text_handler->draw_text();
			
			free(dstrect);
			free(srcrect);

			is_in_rectangle = false;
		}
		else {
			gfx::rect* dstrect = (gfx::rect*)malloc(sizeof(gfx::rect));

			dstrect->x1 = gui_input::rectangle->x1 + 3;
			dstrect->y1 = gui_input::rectangle->y1 + 3;
			dstrect->x2 = width_input_box;
			dstrect->y2 = heigth_input_box;
			gfx::rect* srcrect = (gfx::rect*)malloc(sizeof(gfx::rect));
			srcrect->x1 = 0;
			srcrect->y1 = 0;
			srcrect->x2 = width_input_box;
			srcrect->y2 = heigth_input_box;

			/*text_handler->set_blit_rectangles(srcrect, dstrect);
			text_handler->set_blit(true);*/
			text_handler->draw_text();

			free(dstrect);
			free(srcrect);
		}
	}
	else {
		// first we divide the input boxes higth by 2, to get center point of the input box.
		// then we also divide the texts higth by 2, to get the length we have to
		// subtract from the input boxes center point, to make the text height centered.
		// after that we just add the position to our lengths.
		// +4, because we want the text be drawn a bit more right

		// just draw the text surface
		/*g.cord_to_pnt(p1, gui_input::rectangle->x1 + 4,
			gui_input::rectangle->y1 + (heigth_input_box/2 - heigth/2));*/
		//text_handler->set_blit(false);
		g.cord_to_pnt(p1, gui_input::rectangle->x1 + 4,
			gui_input::rectangle->y1 + (heigth_input_box/2 - 14/2) + 1);
		text_handler->set_point(p1);
		text_handler->draw_text();
	}
	free(p1);

	// draw blink text
	width = blink_text_handler->get_text_width();
	heigth = blink_text_handler->get_text_height();

	// draw blink symbol
	if(is_active) {
		if(SDL_GetTicks() - blink_time >= 500) {
			if(strcmp(blink_text_handler->get_text(), "|") == 0) {
				blink_text_handler->set_text(" ");
			}
			else {
				blink_text_handler->set_text("|");
			}
			blink_time = SDL_GetTicks();
		}
	}
	else {
		if(strcmp(blink_text_handler->get_text(), "|") == 0) {
			blink_text_handler->set_text(" ");
		}
	}
	// first we divide the input boxes higth by 2, to get center point of the input box.
	// then we also divide the texts higth by 2, to get the length we have to
	// subtract from the input boxes center point, to make the text height centered.
	// after that we just add the position to our lengths.
	// +1, because we want the text be drawn a bit more right
	// -1, because we want the text be drawn a bit more on top
	gfx::pnt* p2 = (gfx::pnt*)malloc(sizeof(gfx::pnt));
	if(is_in_rectangle) {
		/*g.cord_to_pnt(p2, gui_input::rectangle->x1 + 1 + (unsigned int)text_width,
			gui_input::rectangle->y1 + (heigth_input_box/2 - heigth/2) - 1);*/
		g.cord_to_pnt(p2, gui_input::rectangle->x1 + 2 + (unsigned int)text_width,
			gui_input::rectangle->y1 + (heigth_input_box/2 - 14/2) - 1);
	}
	else {
		/*g.cord_to_pnt(p2, gui_input::rectangle->x1 + 1 + (unsigned int)text_width - width_diff,
			gui_input::rectangle->y1 + (heigth_input_box/2 - heigth/2) - 1);*/
		g.cord_to_pnt(p2, gui_input::rectangle->x1 + 2 + (unsigned int)text_width - width_diff,
			gui_input::rectangle->y1 + (heigth_input_box/2 - 14/2) - 1);
	}
	blink_text_handler->set_point(p2);
	blink_text_handler->draw_text();
	free(p2);
	is_in_rectangle = true;
}

/*! creates a engine_handler -> a pointer to the engine class
 *  @param iengine the engine we want to handle
 */
void gui_input::set_engine_handler(engine* iengine) {
	gui_input::engine_handler = iengine;
}

/*! creates a text_handler -> a pointer to the gui_text class
 *  @param itext the gui_text we want to handle
 */
void gui_input::set_text_handler(gui_text* itext) {
	gui_input::text_handler = itext;
}

/*! creates a blink_text_handler -> a pointer to the gui_text class
 *  @param itext the gui_text we want to handle
 */
void gui_input::set_blink_text_handler(gui_text* itext) {
	gui_input::blink_text_handler = itext;
}

//! returns the input boxes id
unsigned int gui_input::get_id() {
	return gui_input::id;
}

//! returns the input boxes rectangle
gfx::rect* gui_input::get_rectangle() {
	return gui_input::rectangle;
}

//! returns the input boxes text
char* gui_input::get_text() {
	return gui_input::text;
}

//! returns the input boxes is_active bool
bool gui_input::get_active() {
	return gui_input::is_active;
}

unsigned int gui_input::get_text_position() {
	return gui_input::text_pos;
}

/*! sets the text id
 *  @param id the id we want to set
 */
void gui_input::set_id(unsigned int id) {
	gui_input::id = id;
}

/*! sets the input boxes rectangle
 *  @param rectangle the rectangle we want to set
 */
void gui_input::set_rectangle(gfx::rect* rectangle) {
	gui_input::rectangle = rectangle;
}

/*! sets the input boxes text
 *  @param text the text we want to set
 */
void gui_input::set_text(char* text) {
	gui_input::text = text;
	gui_input::text_handler->set_text(text);
	gui_input::text_length = strlen(text);
}

/*! sets the is_active bool
 *  @param is_active bool if the input box is pressed currently
 */
void gui_input::set_active(bool is_active) {
	gui_input::is_active = is_active;
}

/*! sets the text position (of the blink symbol)
 *  @param position the text position (where we want to have the blink symbol)
 */
void gui_input::set_text_position(unsigned int position) {
	// 0xFFFFFFFF, because position is a uint and we decrease 0
	if(position == 0xFFFFFFFF) {
		gui_input::text_pos = 0;
	}
	else if(position > gui_input::text_length) {
		gui_input::text_pos = gui_input::text_length;
	}
	else {
        gui_input::text_pos = position;
	}
}

/*! sets "no" text - text length equals zero 
 */
void gui_input::set_notext() {
	gui_input::text_handler->set_notext();
	gui_input::text = "";
	gui_input::text_length = 0;
}

/*! returns the text handler
 */
gui_text* gui_input::get_text_handler() {
	return gui_input::text_handler;
}
