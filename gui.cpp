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
#include "gui_input.h"
#include "gui_list.h"
#include "gui_vbar.h"

/*! there is no function currently
*/
gui::gui() {
	p = (gfx::pnt*)malloc(sizeof(gfx::pnt));
	input_text = (char*)malloc(512);

	//char* ib_text = (char*)malloc(1024);
	ib_text_length = 0;
	//char* set_text = (char*)malloc(1028);
	for(unsigned int i = 0; i < 1028; i++) {
        set_text[i] = 0;
	}
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
	cinput_boxes = 0;

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

	// reserve memory for 128 gui input box elements
	for(unsigned int i = 0; i < 128; i++) {
        gui_input_boxes[i] = new gui_input();
	}

	// reserve memory for 128 list input box elements
	for(unsigned int i = 0; i < 128; i++) {
        gui_list_boxes[i] = new gui_list();
	}

	// reserve memory for 16 vertical bar elements
	for(unsigned int i = 0; i < 16; i++) {
        gui_vbars[i] = new gui_vbar();
	}

	gui::active_element = (gui::gui_element*)malloc(sizeof(gui::gui_element));

	if(TTF_Init()==-1) {
		m.print(msg::MERROR, "gui.cpp", "TTF_Init: %s", TTF_GetError());
	}

	// empty element
	gui::gui_elements[celements].id = 1;
	gui::gui_elements[celements].type = gui::EMPTY;
	gui::gui_elements[celements].num = celements;
	gui::gui_elements[celements].is_drawn = false;
	celements++;
}

/*! draws all gui elements
 */
void gui::draw() {
	// start 2d drawing
	engine_handler->start_2d_draw();

	set_active_element(&gui::gui_elements[0]);
	for(unsigned int i = 0; i < celements; i++) {
		if(gui::gui_elements[i].is_drawn == true) {
			switch(gui::gui_elements[i].type) {
				case gui::BUTTON: {
					p->x = event_handler->get_lm_pressed_x();
					p->y = event_handler->get_lm_pressed_y();
					if(g.is_pnt_in_rectangle(gui::gui_buttons[gui::gui_elements[i].num]->get_rectangle(), p)) {
						gui::gui_buttons[gui::gui_elements[i].num]->draw_button(true);
						gui::gui_buttons[gui::gui_elements[i].num]->set_pressed(true);
						set_active_element(&gui::gui_elements[i]);
					}
					else {
						if(gui::gui_buttons[gui::gui_elements[i].num]->get_pressed() == true) {
							gui::gui_buttons[gui::gui_elements[i].num]->set_pressed(false);
							event_handler->add_gui_event(event_handler->BUTTON_PRESSED,
								gui::gui_buttons[gui::gui_elements[i].num]->get_id());
						}

						gui::gui_buttons[gui::gui_elements[i].num]->draw_button(false);
					}
				}
				break;
				case gui::TEXT:
					gui::gui_texts[gui::gui_elements[i].num]->draw_text();
					break;
				case gui::INPUT: {
					p->x = event_handler->get_lm_last_pressed_x();
					p->y = event_handler->get_lm_last_pressed_y();
					if(g.is_pnt_in_rectangle(gui::gui_input_boxes[gui::gui_elements[i].num]->get_rectangle(), p)) {
						gui::gui_input_boxes[gui::gui_elements[i].num]->set_active(true);
						set_active_element(&gui::gui_elements[i]);
						event_handler->set_active(event::GUI);
					}
					else {
						gui::gui_input_boxes[gui::gui_elements[i].num]->set_active(false);
						event_handler->set_active(event::NONE);
					}
					
					event_handler->get_input_text(input_text);
					gui::switch_input_text(input_text,
						gui::gui_input_boxes[gui::gui_elements[i].num]);
                    gui::gui_input_boxes[gui::gui_elements[i].num]->draw_input();
				}
				break;
				case gui::LIST: {					
                    gui::gui_list_boxes[gui::gui_elements[i].num]->draw_list();
				}
				break;
				case gui::VBAR: {
					p->x = event_handler->get_lm_pressed_x();
					p->y = event_handler->get_lm_pressed_y();
					if(gui::gui_vbars[gui::gui_elements[i].num]->get_slider_active()) {
						if(g.is_pnt_in_rectangle(gui::gui_vbars[gui::gui_elements[i].num]->get_rectangle(), p)) {
							int cx;
							int cy;
							gfx::pnt* np = (gfx::pnt*)malloc(sizeof(gfx::pnt));
							SDL_GetMouseState(&cx, &cy);
							np->x = cx;
							np->y = cy;
							gui::gui_vbars[gui::gui_elements[i].num]->set_active(true);
							gui::gui_vbars[gui::gui_elements[i].num]->set_new_point(np);
							free(np);
						}
						else {
							gui::gui_vbars[gui::gui_elements[i].num]->set_active(false);
						}
					}

					gui::gui_vbars[gui::gui_elements[i].num]->draw_vbar();
				}
				break;
				default:
					break;
			}
		}
	}

	// stop 2d drawing
	engine_handler->stop_2d_draw();
}

/*! adds a gui button element and returns it
 *  @param rectangle the buttons rectangle
 *  @param id the buttons (a2e event) id
 *  @param text the buttons text
 *  @param icon the icon id (0 = no icon)
 */
gui_button* gui::add_button(gfx::rect* rectangle, unsigned int id, char* text, unsigned int icon) {
	gui::gui_elements[celements].id = id;
	gui::gui_elements[celements].type = gui::BUTTON;
	gui::gui_elements[celements].num = cbuttons;
	gui::gui_elements[celements].is_drawn = true;
	
	// celements has to be incremented _before_ we add the text, otherwise
	// our button stuff will be overwritten
	celements++;

	gui::gui_buttons[cbuttons]->set_text_handler(add_text("vera.ttf", 12, text,
		engine_handler->get_gstyle().STYLE_FONT2, g.cord_to_pnt(0,0), id+0xFFFF));
	// don't draw our text automatically
	// celements-1, because our text element, is the last initialized element
	gui::gui_elements[celements-1].is_drawn = false;

	gui::gui_buttons[cbuttons]->set_engine_handler(gui::engine_handler);
	gui::gui_buttons[cbuttons]->set_id(id);
	gui::gui_buttons[cbuttons]->set_rectangle(rectangle);
	gui::gui_buttons[cbuttons]->set_text(text);
	gui::gui_buttons[cbuttons]->set_icon(icon);

	cbuttons++;

	return gui_buttons[cbuttons-1];
}

/*! adds a gui button element and returns it
 *  @param rectangle the buttons rectangle
 *  @param id the buttons (a2e event) id
 *  @param text the buttons text
 */
gui_button* gui::add_button(gfx::rect* rectangle, unsigned int id, char* text) {
	return gui::add_button(rectangle, id, text, 0);
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

	gui::gui_texts[ctexts]->set_init(false);
	gui::gui_texts[ctexts]->set_engine_handler(gui::engine_handler);
	gui::gui_texts[ctexts]->set_id(id);
	// there were problems with TTF_RenderText_Solid, so it was just
	// changed to TTF_RenderText_Blended ;) its a bit slower, but nicer ;)
	gui::gui_texts[ctexts]->set_surface(TTF_RenderText_Blended(font, text, col));
	gui::gui_texts[ctexts]->set_point(point);
	gui::gui_texts[ctexts]->set_text(text);
	gui::gui_texts[ctexts]->set_color(col);
	gui::gui_texts[ctexts]->set_font_name(font_name);
	gui::gui_texts[ctexts]->set_font_size(font_size);
	gui::gui_texts[ctexts]->set_init(true);

	gui::gui_texts[ctexts]->close_font(font);

	celements++;
	ctexts++;

	return gui::gui_texts[ctexts-1];
}

/*! adds a gui input box element and returns it
 *  @param rectangle the input boxes rectangle
 *  @param id the input boxes (a2e event) id
 *  @param text the input boxes text
 */
gui_input* gui::add_input_box(gfx::rect* rectangle, unsigned int id, char* text) {
	gui::gui_elements[celements].id = id;
	gui::gui_elements[celements].type = gui::INPUT;
	gui::gui_elements[celements].num = cinput_boxes;
	gui::gui_elements[celements].is_drawn = true;
	
	// celements has to be incremented _before_ we add the text, otherwise
	// our input box stuff will be overwritten
	celements++;

	gui::gui_input_boxes[cinput_boxes]->set_text_handler(add_text("vera.ttf", 12, text,
		engine_handler->get_gstyle().STYLE_FONT, g.cord_to_pnt(0,0), id+0xFFFF));
	// don't draw our text automatically
	// celements-1, because our text element, is the last initialized element
	gui::gui_elements[celements-1].is_drawn = false;

	gui::gui_input_boxes[cinput_boxes]->set_blink_text_handler(add_text("vera.ttf", 12, " ",
		engine_handler->get_gstyle().STYLE_FONT, g.cord_to_pnt(0,0), id+0xFFFFFF));
	// don't draw our text automatically
	// celements-1, because our text element, is the last initialized element
	gui::gui_elements[celements-1].is_drawn = false;

	gui::gui_input_boxes[cinput_boxes]->set_engine_handler(gui::engine_handler);
	gui::gui_input_boxes[cinput_boxes]->set_id(id);
	gui::gui_input_boxes[cinput_boxes]->set_rectangle(rectangle);
	gui::gui_input_boxes[cinput_boxes]->set_text(text);
	gui::gui_input_boxes[cinput_boxes]->set_text_position(strlen(text));

	cinput_boxes++;

	return gui_input_boxes[cinput_boxes-1];
}

/*! adds a gui list box element and returns it
 *  @param rectangle the list boxes rectangle
 *  @param id the list boxes (a2e event) id
 *  @param text the list boxes text
 */
gui_list* gui::add_list_box(gfx::rect* rectangle, unsigned int id, char* text) {
	gui::gui_elements[celements].id = id;
	gui::gui_elements[celements].type = gui::LIST;
	gui::gui_elements[celements].num = clist_boxes;
	gui::gui_elements[celements].is_drawn = true;

	celements++;

	// add vertical bar
	gui::gui_list_boxes[clist_boxes]->set_vbar_handler(add_vbar(g.pnt_to_rect(rectangle->x2-14,
		rectangle->y1+2, rectangle->x2-2, rectangle->y2-2), id+0xFFFFFF));

	gui::gui_list_boxes[clist_boxes]->set_engine_handler(gui::engine_handler);
	gui::gui_list_boxes[clist_boxes]->set_id(id);
	gui::gui_list_boxes[clist_boxes]->set_rectangle(rectangle);
	gui::gui_list_boxes[clist_boxes]->set_position(0);

	clist_boxes++;

	return gui_list_boxes[clist_boxes-1];
}

gui_vbar* gui::add_vbar(gfx::rect* rectangle, unsigned int id) {
	gui::gui_elements[celements].id = id;
	gui::gui_elements[celements].type = gui::VBAR;
	gui::gui_elements[celements].num = cvbars;
	gui::gui_elements[celements].is_drawn = true;

	// celements has to be incremented _before_ we add the buttons, otherwise
	// our vertical bar stuff will be overwritten
	celements++;

	// add up button
	gui::gui_vbars[cvbars]->set_up_button_handler(add_button(g.pnt_to_rect(0,0,1,1),
		id+0xFFFFFF, " ", 1));

	// add down button
	gui::gui_vbars[cvbars]->set_down_button_handler(add_button(g.pnt_to_rect(0,0,1,1),
		id+0xFFFFFE, " ", 2));

	gui::gui_vbars[cvbars]->set_engine_handler(gui::engine_handler);
	gui::gui_vbars[cvbars]->set_id(id);
	gui::gui_vbars[cvbars]->set_rectangle(rectangle);

	cvbars++;

	return gui_vbars[cvbars-1];
}

//! returns the guis surface
SDL_Surface* gui::get_gui_surface() {
	return gui::gui_surface;
}

gui::gui_element* gui::get_active_element() {
	return gui::active_element;
}

void gui::set_active_element(gui_element* active_element) {
	gui::active_element = active_element;
	event_handler->set_active_element((event::gui_element*)active_element);
}

void gui::set_active_element(unsigned int id) {
	for(unsigned int i = 0; i < celements; i++) {
		if(gui::gui_elements[i].id == id) {
			gui::set_active_element(&gui::gui_elements[i]);
			return;
		}
	}
}

void gui::switch_input_text(char* input_text, gui_input* input_box) {
	for(unsigned int i = 0; i < strlen(input_text); i++) {
		switch(input_text[i]) {
			case event::LEFT:
				input_box->set_text_position(input_box->get_text_position() - 1);
				break;
			case event::RIGHT:
				input_box->set_text_position(input_box->get_text_position() + 1);
				break;
			case event::BACK: {
				ib_text_length = strlen(input_box->get_text());
				ib_text = input_box->get_text();

				char* tok1 = (char*)malloc(ib_text_length+4);
				for(unsigned int a = 0; a < ib_text_length+4; a++) {
					tok1[a] = 0;
				}

				char* tok2 = (char*)malloc(ib_text_length+4);
				for(unsigned int a = 0; a < ib_text_length+4; a++) {
					tok2[a] = 0;
				}

				if(ib_text_length != input_box->get_text_position()) {
					unsigned int j;
					for(j = 0; j < input_box->get_text_position(); j++) {
						tok1[j] = ib_text[j];
					}
					tok1[j-1] = 0;
					unsigned int k = 0;
					for(j = input_box->get_text_position(); j < ib_text_length; j++) {
						tok2[k] = ib_text[j];
						k++;
					}
					sprintf(set_text, "%s%s", tok1, tok2);
				}
				else {
					sprintf(tok1, "%s", ib_text);
					tok1[ib_text_length - 1] = 0;
                    sprintf(set_text, "%s", tok1);
				}

				// no text exception
				if(strlen(set_text) != 0) {
					input_box->set_text(set_text);
					input_box->set_text_position(input_box->get_text_position() - 1);
				}
				else {
					input_box->set_notext();
					input_box->set_text_position(0);
				}

				free(tok1);
				free(tok2);
			}
			break;
			case event::DEL: {
				ib_text_length = strlen(input_box->get_text());
				ib_text = input_box->get_text();

				char* tok1 = (char*)malloc(ib_text_length+4);
				for(unsigned int a = 0; a < ib_text_length+4; a++) {
					tok1[a] = 0;
				}

				char* tok2 = (char*)malloc(ib_text_length+4);
				for(unsigned int a = 0; a < ib_text_length+4; a++) {
					tok2[a] = 0;
				}

				if(ib_text_length != input_box->get_text_position()) {
					unsigned int j;
					for(j = 0; j < input_box->get_text_position(); j++) {
						tok1[j] = ib_text[j];
					}
					unsigned int k = 0;
					for(j = input_box->get_text_position(); j < ib_text_length; j++) {
						tok2[k] = ib_text[j+1];
						k++;
					}
					sprintf(set_text, "%s%s", tok1, tok2);
				}
				else {
                    sprintf(set_text, "%s", ib_text);
				}

				// no text exception
				if(strlen(set_text) != 0) {
					input_box->set_text(set_text);
				}
				else {
					input_box->set_notext();
				}

				free(tok1);
				free(tok2);
			}
			break;
			case event::HOME:
				input_box->set_text_position(0);
				break;
			case event::END:
				input_box->set_text_position(strlen(input_box->get_text()));
				break;
			default: {
				ib_text_length = strlen(input_box->get_text());
				ib_text = input_box->get_text();

				char* tok1 = (char*)malloc(ib_text_length+4);
				for(unsigned int a = 0; a < ib_text_length+4; a++) {
					tok1[a] = 0;
				}

				char* tok2 = (char*)malloc(ib_text_length+4);
				for(unsigned int a = 0; a < ib_text_length+4; a++) {
					tok2[a] = 0;
				}

				if(ib_text_length != input_box->get_text_position()) {
					for(unsigned int j = 0; j < input_box->get_text_position(); j++) {
						tok1[j] = ib_text[j];
					}
					unsigned int k = 0;
					for(unsigned int j = input_box->get_text_position(); j < ib_text_length; j++) {
						tok2[k] = ib_text[j];
						k++;
					}
					sprintf(set_text, "%s%c%s", tok1, input_text[i], tok2);
				}
				else {
                    sprintf(set_text, "%s%c", ib_text, input_text[i]);
				}

				input_box->set_text(set_text);
				input_box->set_text_position(input_box->get_text_position() + 1);

				free(tok1);
				free(tok2);
			}
			break;
		}
	}
}
