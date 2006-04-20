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
gui_input::gui_input(engine* e) {
	blink_time = SDL_GetTicks();
	is_active = false;

	gui_input::text.reserve(128);

	gui_input::is_in_rectangle = false;

	// get classes
	gui_input::e = e;
	gui_input::c = e->get_core();
	gui_input::m = e->get_msg();
	gui_input::g = e->get_gfx();
}

/*! there is no function currently
 */
gui_input::~gui_input() {
	m->print(msg::MDEBUG, "gui_input.cpp", "freeing gui_input stuff");

	gui_input::text.clear();

	m->print(msg::MDEBUG, "gui_input.cpp", "gui_input stuff freed");
}

/*! draws the input boxs
 *  @param x specifies how much the element is moved on the x axis
 *  @param y specifies how much the element is moved on the y axis
 */
void gui_input::draw(unsigned int x, unsigned int y) {
	gfx::rect* r1 = new gfx::rect();

	g->pnt_to_rect(r1, gui_input::rectangle->x1 + x, gui_input::rectangle->y1 + y,
		gui_input::rectangle->x2 + x, gui_input::rectangle->y2 + y);

	// draw bg
	g->draw_filled_rectangle(r1,
		e->get_gui_style()->STYLE_BG2);

	// draw 2 colored border
	g->draw_2colored_rectangle(r1,
		e->get_gui_style()->STYLE_INDARK,
		e->get_gui_style()->STYLE_LIGHT);

	// draw 2 colored border
	g->pnt_to_rect(r1, gui_input::rectangle->x1 + x + 1, gui_input::rectangle->y1 + y + 1,
		gui_input::rectangle->x2 + x - 1, gui_input::rectangle->y2 + y - 1);
	g->draw_2colored_rectangle(r1, e->get_gui_style()->STYLE_DARK,
		e->get_gui_style()->STYLE_DARK2);

	// width chart:
	// text_width specifies the texts width -before- the marker
	// after_text_width specifies the texts width -after- the marker
	// width specifies the whole text width
	// width_input_box specefies the available (drawable) width of the input box

	// get the input box text width
	float glyph_width;
	float text_width = 0;
	float after_text_width = 0;
	char* tmp_text = (char*)text_handler->get_text();
	char* tmp_char = new char[2];
	for(unsigned int i = 0; i < strlen(text_handler->get_text()); i++) {
		//tmp_char[0] = tmp_text[i];
		//snprintf(tmp_char, 1, "%c", tmp_text[i]);
		memcpy(tmp_char, &tmp_text[i], 1);
		tmp_char[1] = 0;
		glyph_width = text_handler->get_font()->ttf_font->Advance(tmp_char);
		if(i < text_pos) {
			text_width += glyph_width;
		}
		else {
			after_text_width += glyph_width;
		}
	}
	delete [] tmp_char;

	// draw text
	core::pnt* p1 = new core::pnt();
	unsigned int width = text_handler->get_text_width();
	unsigned int height = text_handler->get_text_height();
	// -6, because we have a border of 2px*2 and an empty pixel*2
	unsigned int width_input_box = gui_input::rectangle->x2 - gui_input::rectangle->x1 - 6;
	unsigned int height_input_box = gui_input::rectangle->y2 - gui_input::rectangle->y1;
	// if the current text cursor position isn't in the available space of the text box,
	// than we define width_diff to set the position _into_ the available space
	unsigned int width_diff = 0;

	// if the texts surface is bigger than the available space of the text box
	if(width >= width_input_box) {
		// than we have to make the (shown) surface smaller

		// is the current marker position behind the normal text box width?
		if(text_pos == strlen(text_handler->get_text())) {
			width_diff = (unsigned int)text_width - width_input_box;

			char* ptext = (char*)text_handler->get_text();
			unsigned int len = (unsigned int)strlen(ptext);

			char* tmp_text = new char[len+1];
			// set each byte of the string to 0
			for(unsigned int i = 0; i < len+1; i++) {
				tmp_text[i] = 0;
			}

			unsigned int pos = 0;
			float cur_width = 0.0f;
			while(cur_width < (float)width_input_box) {
				tmp_text[pos] = ptext[(len-1) - pos];
				cur_width = text_handler->get_font()->ttf_font->Advance(tmp_text);
				pos++;
			}
			pos--;
			// the last char of tmp_text made the string too long, so
			// we need to delete it
			tmp_text[pos] = 0;

			// now, we just have to flip the string
			len = (unsigned int)strlen(tmp_text);
			char* new_text = new char[len+1];
			new_text[len] = 0;
			for(unsigned int i = 0; i < len; i++) {
				new_text[i] = tmp_text[(len-1) - i];
			}

			// and now render the text
			g->cord_to_pnt(p1, gui_input::rectangle->x1 + 2 + width_input_box - (unsigned int)text_handler->get_font()->ttf_font->Advance(new_text),
				gui_input::rectangle->y1 + (height_input_box/2 - 14/2) + 2);
			/*g->cord_to_pnt(p1, gui_input::rectangle->x1 + 2,
				gui_input::rectangle->y1 + (height_input_box/2 - 14/2) + 2);*/
			text_handler->set_point(p1);
			text_handler->draw(new_text, x, y);

			delete [] new_text;
			delete [] tmp_text;

			// we need to set a bool, if the marker is out of the drawable
			// input box rectangle, so we can set it to the "end" of the input box
			is_in_rectangle = false;
		}
		// if it's not, then we check ...
		else {
			// .. if the texts width is bigger or equals the input boxes width ...
			if(text_width >= width_input_box) {
				width_diff = (unsigned int)text_width - width_input_box;

				char* ptext = (char*)text_handler->get_text();
				//unsigned int len = strlen(ptext);
				unsigned int len = text_pos;

				char* tmp_text = new char[len+1];
				// set each byte of the string to 0
				for(unsigned int i = 0; i < len+1; i++) {
					tmp_text[i] = 0;
				}

				unsigned int pos = 0;
				float cur_width = 0.0f;
				while(cur_width < (float)width_input_box) {
					tmp_text[pos] = ptext[(len-1) - pos];
					cur_width = text_handler->get_font()->ttf_font->Advance(tmp_text);
					pos++;
				}
				pos--;
				// the last char of tmp_text made the string too long, so
				// we need to delete it
				tmp_text[pos] = 0;

				// now, we just have to flip the string
				len = (unsigned int)strlen(tmp_text);
				char* new_text = new char[len+1];
				new_text[len] = 0;
				for(unsigned int i = 0; i < len; i++) {
					new_text[i] = tmp_text[(len-1) - i];
				}

				// and now render the text
				g->cord_to_pnt(p1, gui_input::rectangle->x1 + 2 + width_input_box - (unsigned int)text_handler->get_font()->ttf_font->Advance(new_text),
					gui_input::rectangle->y1 + (height_input_box/2 - 14/2) + 2);
				/*g->cord_to_pnt(p1, gui_input::rectangle->x1 + 2,
					gui_input::rectangle->y1 + (height_input_box/2 - 14/2) + 2);*/
				text_handler->set_point(p1);
				text_handler->draw(new_text, x, y);

				delete [] new_text;
				delete [] tmp_text;

				// we need to set a bool, if the marker is out of the drawable
				// input box rectangle, so we can set it to the "end" of the input box
				is_in_rectangle = false;
			}
			// ... or not
			else  {
				char* ptext = (char*)text_handler->get_text();
				unsigned int len = (unsigned int)strlen(ptext);

				char* new_text = new char[len+1];
				// set each byte of the string to 0
				for(unsigned int i = 0; i < len+1; i++) {
					new_text[i] = 0;
				}

				unsigned int pos = 0;
				float cur_width = 0.0f;
				while(cur_width < (float)width_input_box) {
					new_text[pos] = ptext[pos];
					cur_width = text_handler->get_font()->ttf_font->Advance(new_text);
					pos++;
				}
				pos--;
				// the last char of tmp_text made the string too long, so
				// we need to delete it
				new_text[pos] = 0;

				// and now render the text
				g->cord_to_pnt(p1, gui_input::rectangle->x1 + 2 + width_input_box - (unsigned int)text_handler->get_font()->ttf_font->Advance(new_text),
					gui_input::rectangle->y1 + (height_input_box/2 - 14/2) + 2);
				/*g->cord_to_pnt(p1, gui_input::rectangle->x1 + 2,
					gui_input::rectangle->y1 + (height_input_box/2 - 14/2) + 2);*/
				text_handler->set_point(p1);
				text_handler->draw(new_text, x, y);

				delete [] new_text;
			}
		}
	}
	else {
		// otherwise just draw the text

		// first we divide the input boxes higth by 2, to get center point of the input box.
		// then we also divide the texts higth by 2, to get the length we have to
		// subtract from the input boxes center point, to make the text height centered.
		// after that we just add the position to our lengths.
		// +4, because we want the text be drawn a bit more right

		// just draw the text surface
		/*g->cord_to_pnt(p1, gui_input::rectangle->x1 + 4,
			gui_input::rectangle->y1 + (height_input_box/2 - height/2));*/
		//text_handler->set_blit(false);
		g->cord_to_pnt(p1, gui_input::rectangle->x1 + 4,
			gui_input::rectangle->y1 + (height_input_box/2 - 14/2) + 2);
		text_handler->set_point(p1);
		text_handler->draw(x, y);
	}
	delete p1;

	// draw blink text
	width = blink_text_handler->get_text_width();
	height = blink_text_handler->get_text_height();

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

	// first we divide the input boxes height by 2, to get center point of the input box.
	// then we also divide the texts higth by 2, to get the length we have to
	// subtract from the input boxes center point, to make the text height centered.
	// after that we just add the position to our lengths.
	// +1, because we want the text be drawn a bit more right
	// -1, because we want the text be drawn a bit more on top
	core::pnt* p2 = new core::pnt();
	if(is_in_rectangle) {
		/*g->cord_to_pnt(p2, gui_input::rectangle->x1 + 1 + (unsigned int)text_width,
			gui_input::rectangle->y1 + (height_input_box/2 - height/2) - 2);*/
		g->cord_to_pnt(p2, gui_input::rectangle->x1 + 2 + (unsigned int)text_width,
			gui_input::rectangle->y1 + (height_input_box/2 - 14/2));
	}
	else {
		/*g->cord_to_pnt(p2, gui_input::rectangle->x1 + 1 + (unsigned int)text_width - width_diff,
			gui_input::rectangle->y1 + (height_input_box/2 - height/2) - 2);*/
		g->cord_to_pnt(p2, gui_input::rectangle->x1 + 2 + (unsigned int)text_width - width_diff,
			gui_input::rectangle->y1 + (height_input_box/2 - 14/2));
	}
	blink_text_handler->set_point(p2);
	blink_text_handler->draw(x, y);
	delete p2;
	is_in_rectangle = true;

	delete r1;
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

//! returns the text_handler
gui_text* gui_input::get_text_handler() {
	return gui_input::text_handler;
}

//! returns the blink_text_handler
gui_text* gui_input::get_blink_text_handler() {
	return gui_input::blink_text_handler;
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
string* gui_input::get_text() {
	return &(gui_input::text);
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
	gui_input::text_length = (unsigned int)gui_input::text.size();

	if(text_pos > gui_input::text.length()) {
		text_pos = (unsigned int)gui_input::text.length();
	}
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
