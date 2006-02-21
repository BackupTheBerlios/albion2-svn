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

/*! there is no function currently
*/
gui::gui(engine* e) {
	p = new core::pnt();
	r = new gfx::rect();
	input_text = new char[512];
	ib_text_length = 0;

	// get classes
	gui::e = e;
	gui::c = e->get_core();
	gui::m = e->get_msg();
	gui::evt = e->get_event();
	gui::g = e->get_gfx();
	gui::gf = e->get_gui_font();
}

/*! there is no function currently
*/
gui::~gui() {
	m->print(msg::MDEBUG, "gui.cpp", "freeing gui stuff");

	delete p;
	delete r;
	delete [] input_text;

	gui_buttons.clear();
	gui_texts.clear();
	gui_input_boxes.clear();
	gui_list_boxes.clear();
	gui_vbars.clear();
	gui_check_boxes.clear();
	gui_windows.clear();

	m->print(msg::MDEBUG, "gui.cpp", "gui stuff freed");
}

/*! initializes the gui class and sets an engine and event handler
 */
void gui::init() {
	if(gui::e->get_screen() == NULL) { m->print(msg::MERROR, "gui.cpp", "SDL_Surface does not exist!"); }
	else { gui::gui_surface = gui::e->get_screen(); }

	gui::active_element = gui_elements.begin();

	// init the main window
	main_window = gui::add_window(g->pnt_to_rect(0, 0, e->get_screen()->w,
		e->get_screen()->h), 0, "main window", false);
}

/*! draws all gui elements
 */
void gui::draw() {
	// reset event stuff to camera - if there is a gui event,
	// the active class type will be overwritten
	evt->set_active(event::CAMERA);

	// start 2d drawing
	e->start_2d_draw();

	// reset active gui element
	set_active_element(gui::gui_elements.begin());

	// check if left mouse button is currently pressed (then use
	// that point otherwise use the last pressed point)
	unsigned int event_type = 0;
	if(evt->get_lm_pressed_x() == 0 && evt->get_lm_pressed_y() == 0) {
		p->x = evt->get_lm_last_pressed_x();
		p->y = evt->get_lm_last_pressed_y();
		event_type = 1;
	}
	else {
		p->x = evt->get_lm_pressed_x();
		p->y = evt->get_lm_pressed_y();
		event_type = 2;
	}

	wnd_layers.reverse();
	bool wnd_event = false;
	for(list<unsigned int>::iterator layer_iter = wnd_layers.begin(); layer_iter != wnd_layers.end(); layer_iter++) {
		list<gui_window>::reference wnd_iter = *get_window(*layer_iter);
		//if(g->is_pnt_in_rectangle(wnd_iter.get_rectangle(), p) && *layer_iter != 0) {
		if(g->is_pnt_in_rectangle(wnd_iter.get_rectangle(), p) && wnd_iter.get_border()) {
			unsigned int id = *layer_iter;
			wnd_layers.erase(layer_iter);
			wnd_layers.push_front(id);
			wnd_event = true;
			break;
		}
	}
	wnd_layers.reverse();

	unsigned int j = 0;
	for(list<unsigned int>::iterator layer_iter = wnd_layers.begin(); layer_iter != wnd_layers.end(); layer_iter++) {
		list<gui_window>::reference wnd_iter = *get_window(*layer_iter);
		unsigned int current_id = *layer_iter;

		// if window is being moved set window event flag to true
		if(wnd_iter.is_moving()) {
			wnd_event = true;
		}

		// check if we got a window with a title bar and border
		if(wnd_iter.get_border()) {
			// reposition the window if it has been moved or is moving
			memcpy(r, wnd_iter.get_rectangle(), sizeof(gfx::rect));
			r->x1 += 1;
			r->x2 -= 1;
			r->y2 = r->y1 + 17;
			r->y1 += 1;

			if(event_type == 1 && wnd_iter.is_moving()) {
				wnd_iter.set_moving(false);
			}

			if((g->is_pnt_in_rectangle(r, p) || wnd_iter.is_moving())
				&& event_type == 2 && current_id == wnd_layers.back()) {
				core::pnt* tmp_point = new core::pnt();
				evt->get_mouse_pos(tmp_point);
				// well, for more performance we could uncomment this, but the
				// windows moves smoother w/o it ;) --- "reuncommented" it ... works fine ...
				if(evt->get_lm_pressed_x() != tmp_point->x ||
					evt->get_lm_pressed_y() != tmp_point->y) {
					wnd_iter.set_moving(true);
					wnd_iter.change_position((int)tmp_point->x - (int)evt->get_lm_pressed_x(),
						(int)tmp_point->y - (int)evt->get_lm_pressed_y());
					evt->set_pressed(tmp_point->x, tmp_point->y);
					evt->set_last_pressed(0, 0);
				}
				delete tmp_point;
			}
		}

		// draw the window
		wnd_iter.draw();

		// set the "window point" that defines how much the
		// gui elements have to be moved on the x and y axis
		core::pnt* wp = new core::pnt();
		if(wnd_iter.get_border()) {
            wp->x = wnd_iter.get_rectangle()->x1 + 2;
            wp->y = wnd_iter.get_rectangle()->y1 + 19;
		}
		else {
            wp->x = wnd_iter.get_rectangle()->x1;
            wp->y = wnd_iter.get_rectangle()->y1;
		}

		// we need a rectangle object to add the windows
		// x and y coordinate to the gui elements rectangle,
		// so we can check for a button press correctly
		// --- global r object ---

		// draw the gui elements
		for(list<gui_element>::iterator ge_iter = gui::gui_elements.begin(); ge_iter != gui::gui_elements.end(); ge_iter++) {
			if(ge_iter->is_drawn == true &&
				ge_iter->wid == current_id) {
				switch(ge_iter->type) {
					case gui::BUTTON: {
						p->x = evt->get_lm_pressed_x();
						p->y = evt->get_lm_pressed_y();

						// the window button flag - defines if the button is a window button ...
						bool wnd_button = false;

						// well, it's somehow an odd way to do this, but why
						// should i add extra stuff that is just needed once ...
						list<gui_button>::reference cur_button = *get_button(ge_iter->id);
						if(cur_button.get_icon() == 3) {
							// reposition the exit button so it fits into the windows title bar
							// get the rectangle and add the windows x and y coordinate
							memcpy(r, cur_button.get_rectangle(), sizeof(gfx::rect));
							r->x1 += wnd_iter.get_rectangle()->x2 - 14;
							r->x2 += wnd_iter.get_rectangle()->x2 - 2;
							r->y1 += wp->y - 16;
							r->y2 += wp->y - 16;

							// set window button flag to true
							wnd_button = true;
						}
						else {
							// get the rectangle and add the windows x and y coordinate
							memcpy(r, cur_button.get_rectangle(), sizeof(gfx::rect));
							r->x1 += wp->x;
							r->x2 += wp->x;
							r->y1 += wp->y;
							r->y2 += wp->y;
						}

						if((current_id == gui::wnd_layers.back() || !wnd_event) &&
							g->is_pnt_in_rectangle(r, p)) {
							if(!wnd_button) {
								cur_button.draw(true, wp->x, wp->y);
							}
							else {
								cur_button.draw(true, r->x1, r->y1);
							}
							cur_button.set_pressed(true);
							set_active_element(ge_iter);
						}
						else {
							if(cur_button.get_pressed() == true) {
								cur_button.set_pressed(false);
								evt->add_gui_event(evt->BUTTON_PRESSED,
									cur_button.get_id());
							}

							if(!wnd_button) {
                                cur_button.draw(false, wp->x, wp->y);
							}
							else {
								cur_button.draw(false, r->x1, r->y1);
							}
						}
					}
					break;
					case gui::TEXT:
						gui::get_text(ge_iter->id)->draw(wp->x, wp->y);
						break;
					case gui::INPUT: {
						p->x = evt->get_lm_last_pressed_x();
						p->y = evt->get_lm_last_pressed_y();

						list<gui_input>::reference cur_input = *get_input(ge_iter->id);

						// get the rectangle and add the windows x and y coordinate
						memcpy(r, cur_input.get_rectangle(), sizeof(gfx::rect));
						r->x1 += wp->x;
						r->x2 += wp->x;
						r->y1 += wp->y;
						r->y2 += wp->y;

						if((current_id == gui::wnd_layers.back() || !wnd_event) &&
							g->is_pnt_in_rectangle(r, p)) {
							cur_input.set_active(true);
							set_active_element(ge_iter);
							evt->set_active(event::GUI);

							evt->get_input_text(input_text);
							gui::switch_input_text(input_text, cur_input);
						}
						else {
							cur_input.set_active(false);
						}

						cur_input.draw(wp->x, wp->y);
					}
					break;
					case gui::LIST: {
						p->x = evt->get_lm_pressed_x();
						p->y = evt->get_lm_pressed_y();

						list<gui_list>::reference cur_list = *get_list(ge_iter->id);

						// get the rectangle and add the windows x and y coordinate
						memcpy(r, cur_list.get_rectangle(), sizeof(gfx::rect));
						r->x1 += wp->x;
						// decrease the rects x2 coord by 15, because we don't want input from the vbar
						r->x2 += wp->x - 15;
						r->y1 += wp->y;
						r->y2 += wp->y;

						if((current_id == gui::wnd_layers.back() || !wnd_event) && g->is_pnt_in_rectangle(r, p)) {
							cur_list.set_active(true);
							cur_list.select_pos(p->x - wp->x, p->y - wp->y);
							evt->set_last_pressed(0, 0);
						}
						else {
							cur_list.set_active(false);
						}

						cur_list.draw(wp->x, wp->y);
					}
					break;
					case gui::VBAR: {
						p->x = evt->get_lm_pressed_x();
						p->y = evt->get_lm_pressed_y();

						list<gui_vbar>::reference cur_vbar = *get_vbar(ge_iter->id);

						// get the rectangle and add the windows x and y coordinate
						memcpy(r, cur_vbar.get_rectangle(), sizeof(gfx::rect));
						r->x1 += wp->x;
						r->x2 += wp->x;
						r->y1 += wp->y;
						r->y2 += wp->y;

						if((current_id == gui::wnd_layers.back() || !wnd_event) &&
							cur_vbar.get_slider_active()) {
							if(g->is_pnt_in_rectangle(r, p)) {
								int cx;
								int cy;
								core::pnt* np = new core::pnt();
								SDL_GetMouseState(&cx, &cy);
								np->x = cx;
								np->y = cy;
								cur_vbar.set_active(true);
								cur_vbar.set_new_point(np);
								delete np;
							}
							else {
								cur_vbar.set_active(false);
							}
						}

						cur_vbar.draw(wp->x, wp->y);
					}
					break;
					case gui::CHECK: {
						p->x = evt->get_lm_last_pressed_x();
						p->y = evt->get_lm_last_pressed_y();

						list<gui_check>::reference cur_check = *get_check(ge_iter->id);

						// get the rectangle and add the windows x and y coordinate
						// and reduce the "clickable" area
						memcpy(r, cur_check.get_rectangle(), sizeof(gfx::rect));
						r->x1 += wp->x;
						r->x2 = r->x1 + 14;
						r->y1 += wp->y;
						r->y2 = r->y1 + 14;

						if((current_id == gui::wnd_layers.back() || !wnd_event) && g->is_pnt_in_rectangle(r, p)) {
							cur_check.set_checked(cur_check.get_checked() ^ true);
							evt->set_last_pressed(0, 0);
						}

						cur_check.draw(wp->x, wp->y);
					}
					break;
					default:
						break;
				}
			}
		}

		delete wp;
		j++;
	}

	for(list<gui_window>::iterator wnd_iter = gui_windows.begin(); wnd_iter != gui_windows.end(); wnd_iter++) {
		// check if the windows exit button was pressed and delete the window
		if(wnd_iter->get_deleted()) {
			// delete window elements
			unsigned int* welem_ids = new unsigned int[gui::gui_elements.size()]; // not very memory sparing, but it works ... todo: make that stuff better ...
			unsigned int x = 0;
			for(list<gui_element>::iterator wid_iter = gui::gui_elements.begin(); wid_iter != gui::gui_elements.end(); wid_iter++) {
				if(wid_iter->wid == wnd_iter->get_id()) {
					// save the id, so we can delete the element later on
					welem_ids[x] = wid_iter->id;
					x++;
				}
			}
			for(unsigned int k = 0; k < x; k++) {
				gui::delete_element(welem_ids[k]);
			}
			delete [] welem_ids;

			gui::wnd_layers.remove(wnd_iter->get_id());
			gui::delete_element(wnd_iter->get_id());
			break;
		}
	}

	// stop 2d drawing
	e->stop_2d_draw();
}

/*! adds a gui button element and returns it
 *  @param rectangle the buttons rectangle
 *  @param id the buttons (a2e event) id
 *  @param text the buttons text
 *  @param icon the icon id (0 = no icon)
 *  @param wid the id of the window we want the element to be in (0 = no window)
 */
GUI_OBJ gui::add_button(gfx::rect* rectangle, unsigned int icon, unsigned int id, char* text, unsigned int wid) {
	gui::gui_elements.push_back(*new gui_element());
	gui::gui_elements.back().id = id;
	gui::gui_elements.back().type = gui::BUTTON;
	gui::gui_elements.back().wid = wid;
	gui::gui_elements.back().is_drawn = true;

	// create class
	gui::gui_buttons.push_back(*new gui_button(e));

	gui::gui_buttons.back().set_text_handler(get_text(add_text("vera.ttf", 12, text,
		e->get_gui_style()->STYLE_FONT2, g->cord_to_pnt(0,0), id+0xFFFF, wid)));
	// don't draw our text automatically
	// celements-1, because our text element, is the last initialized element
	gui::gui_elements.back().is_drawn = false;

	gui::gui_buttons.back().set_id(id);
	gui::gui_buttons.back().set_rectangle(rectangle);
	gui::gui_buttons.back().set_text(text);
	gui::gui_buttons.back().set_icon(icon);

	//return &gui::gui_buttons.back();
	return id;
}

/*! adds a gui button element and returns it
 *  @param rectangle the buttons rectangle
 *  @param id the buttons (a2e event) id
 *  @param text the buttons text
 *  @param wid the id of the window we want the element to be in (0 = no window)
 */
GUI_OBJ gui::add_button(gfx::rect* rectangle, unsigned int id, char* text, unsigned int wid) {
	return gui::add_button(rectangle, 0, id, text, wid);
}

/*! adds a gui text element and returns it
 *  @param font_name the font that should be used
 *  @param font_size the font size (in px?)
 *  @param text the text itself
 *  @param color the color of the text
 *  @param point the starting point of the text
 *  @param id the texts (a2e event) id
 *  @param wid the id of the window we want the element to be in (0 = no window)
 */
GUI_OBJ gui::add_text(char* font_name, unsigned int font_size, char* text,
				   unsigned int color, core::pnt* point, unsigned int id, unsigned int wid) {
	gui::gui_elements.push_back(*new gui_element());
	gui::gui_elements.back().id = id;
	gui::gui_elements.back().type = gui::TEXT;
	gui::gui_elements.back().wid = wid;
	gui::gui_elements.back().is_drawn = true;

	// create class
	gui::gui_texts.push_back(*new gui_text(e));

	gui::gui_texts.back().set_init(false);
	gui::gui_texts.back().set_id(id);

	// font stuff
	gui::gui_texts.back().set_font(gf->add_font(font_name, font_size, color));
	gui::gui_texts.back().set_point(point);
	gui::gui_texts.back().set_text(text);
	gui::gui_texts.back().set_init(true);

	//return &gui::gui_texts.back();
	return id;
}

/*! adds a gui input box element and returns it
 *  @param rectangle the input boxes rectangle
 *  @param id the input boxes (a2e event) id
 *  @param text the input boxes text
 *  @param wid the id of the window we want the element to be in (0 = no window)
 */
GUI_OBJ gui::add_input_box(gfx::rect* rectangle, unsigned int id, char* text, unsigned int wid) {
	gui::gui_elements.push_back(*new gui_element());
	gui::gui_elements.back().id = id;
	gui::gui_elements.back().type = gui::INPUT;
	gui::gui_elements.back().wid = wid;
	gui::gui_elements.back().is_drawn = true;

	// create class
	gui::gui_input_boxes.push_back(*new gui_input(e));

	gui::gui_input_boxes.back().set_text_handler(get_text(add_text("vera.ttf", 12, text,
		e->get_gui_style()->STYLE_FONT, g->cord_to_pnt(0,0), id+0xFFFF)));
	// don't draw our text automatically
	// celements-1, because our text element, is the last initialized element
	gui::gui_elements.back().is_drawn = false;

	gui::gui_input_boxes.back().set_blink_text_handler(get_text(add_text("vera.ttf", 12, " ",
		e->get_gui_style()->STYLE_FONT, g->cord_to_pnt(0,0), id+0xFFFFFF)));
	// don't draw our text automatically
	// celements-1, because our text element, is the last initialized element
	gui::gui_elements.back().is_drawn = false;

	gui::gui_input_boxes.back().set_id(id);
	gui::gui_input_boxes.back().set_rectangle(rectangle);
	gui::gui_input_boxes.back().set_text(text);
	gui::gui_input_boxes.back().set_text_position((unsigned int)strlen(text));

	//return &gui::gui_input_boxes.back();
	return id;
}

/*! adds a gui list box element and returns it
 *  @param rectangle the list boxes rectangle
 *  @param id the list boxes (a2e event) id
 *  @param wid the id of the window we want the element to be in (0 = no window)
 */
GUI_OBJ gui::add_list_box(gfx::rect* rectangle, unsigned int id, unsigned int wid) {
	gui::gui_elements.push_back(*new gui_element());
	gui::gui_elements.back().id = id;
	gui::gui_elements.back().type = gui::LIST;
	gui::gui_elements.back().wid = wid;
	gui::gui_elements.back().is_drawn = true;

	// create class
	gui::gui_list_boxes.push_back(*new gui_list(e));

	// add vertical bar
	gui::gui_list_boxes.back().set_vbar_handler(get_vbar(add_vbar(g->pnt_to_rect(rectangle->x2-14,
		rectangle->y1+2, rectangle->x2-2, rectangle->y2-2), id+0xFFFFFF, wid)));

	gui::gui_list_boxes.back().set_id(id);
	gui::gui_list_boxes.back().set_rectangle(rectangle);
	gui::gui_list_boxes.back().set_position(0);

	//return &gui::gui_list_boxes.back();
	return id;
}

/*! adds a gui vertical bar element and returns it
 *  @param rectangle the vbars rectangle
 *  @param id the vertical bar (a2e event) id
 *  @param wid the id of the window we want the element to be in (0 = no window)
 */
GUI_OBJ gui::add_vbar(gfx::rect* rectangle, unsigned int id, unsigned int wid) {
	gui::gui_elements.push_back(*new gui_element());
	gui::gui_elements.back().id = id;
	gui::gui_elements.back().type = gui::VBAR;
	gui::gui_elements.back().wid = wid;
	gui::gui_elements.back().is_drawn = true;

	// create class
	gui::gui_vbars.push_back(*new gui_vbar(e));

	// add up button
	gui::gui_vbars.back().set_up_button_handler(get_button(add_button(g->pnt_to_rect(0,0,1,1), 1,
		id+0xFFFFFF, " ", wid)));

	// add down button
	gui::gui_vbars.back().set_down_button_handler(get_button(add_button(g->pnt_to_rect(0,0,1,1), 2,
		id+0xFFFFFE, " ", wid)));

	gui::gui_vbars.back().set_id(id);
	gui::gui_vbars.back().set_rectangle(rectangle);

	//return &gui::gui_vbars.back();
	return id;
}

/*! adds a gui check box element and returns it
 *  @param rectangle the check boxes rectangle
 *  @param id the check boxes (a2e event) id
 *  @param text the check boxes text
 *  @param wid the id of the window we want the element to be in (0 = no window)
 */
GUI_OBJ gui::add_check_box(gfx::rect* rectangle, unsigned int id, char* text, unsigned int wid) {
	gui::gui_elements.push_back(*new gui_element());
	gui::gui_elements.back().id = id;
	gui::gui_elements.back().type = gui::CHECK;
	gui::gui_elements.back().wid = wid;
	gui::gui_elements.back().is_drawn = true;

	// create class
	gui::gui_check_boxes.push_back(*new gui_check(e));

	gui::gui_check_boxes.back().set_text_handler(get_text(add_text("vera.ttf", 12, text,
		e->get_gui_style()->STYLE_FONT2, g->cord_to_pnt(0,0), id+0xFFFF, wid)));
	// don't draw our text automatically
	// celements-1, because our text element, is the last initialized element
	gui::gui_elements.back().is_drawn = false;

	gui::gui_check_boxes.back().set_id(id);
	gui::gui_check_boxes.back().set_rectangle(rectangle);
	gui::gui_check_boxes.back().set_text(text);

	//return &gui::gui_check_boxes.back();
	return id;
}

/*! adds a gui window element and returns it
 *  @param rectangle the windows rectangle
 *  @param id the windows (a2e event) id
 *  @param caption the windows caption
 *  @param border a bool if the windows border and titlebar should be drawn
 */
GUI_OBJ gui::add_window(gfx::rect* rectangle, unsigned int id, char* caption, bool border) {
	gui::gui_elements.push_back(*new gui_element());
	gui::gui_elements.back().id = id;
	gui::gui_elements.back().type = gui::WINDOW;
	gui::gui_elements.back().wid = 0;
	gui::gui_elements.back().is_drawn = true;

	// create class
	gui::gui_windows.push_back(*new gui_window(e));

	// add exit button if we got a border
	if(border) {
		gui::gui_windows.back().set_exit_button_handler(get_button(add_button(g->pnt_to_rect(0,0,12,12), 3,
			id+0xFFFFFF, "x", id)));
	}

	gui::gui_windows.back().set_text_handler(get_text(add_text("vera.ttf", 12, caption,
		e->get_gui_style()->STYLE_FONT2, g->cord_to_pnt(0,0), id+0xFFFF, id)));
	// don't draw our text automatically
	// celements-1, because our text element, is the last initialized element
	gui::gui_elements.back().is_drawn = false;

	gui::gui_windows.back().set_id(id);
	gui::gui_windows.back().set_rectangle(rectangle);
	gui::gui_windows.back().set_caption(caption);
	gui::gui_windows.back().set_lid((unsigned int)gui_windows.size());
	gui::gui_windows.back().set_border(border);

	// add the windows to the layer stack/list
	// it will automatically become active / the one in the front
	wnd_layers.push_back(id);

	//return &gui::gui_windows.back();
	return id;
}

//! returns the guis surface
SDL_Surface* gui::get_gui_surface() {
	return gui::gui_surface;
}

/*! returns the currently active gui element
 */
list<gui::gui_element>::iterator gui::get_active_element() {
	return gui::active_element;
}

/*! sets the currently active gui element via gui_element pointer
 *  @param active_element pointer to the new active element
 */
void gui::set_active_element(list<gui_element>::iterator active_element) {
	gui::active_element = active_element;
	evt->set_active_type(active_element->type);
}

/*! sets the currently active gui element via the gui element's id
 *  @param id the id of the new active element
 */
void gui::set_active_element(unsigned int id) {
	for(list<gui_element>::iterator iter = gui_elements.begin(); iter != gui_elements.end(); iter++) {
		if(iter->id == id) {
			gui::set_active_element(iter);
			return;
		}
	}
	m->print(msg::MERROR, "gui.cpp", "set_active_element(): couldn't find a gui object with the id #%u!", id);
	return;
}

/*! handles the input that we get from the event class for the input box
 *  @param input_text chars written to the input box
 *  @param input_box pointer to the input box
 */
void gui::switch_input_text(char* input_text, list<gui_input>::reference input_box) {
	for(unsigned int i = 0; i < strlen(input_text); i++) {
		switch(input_text[i]) {
			case event::LEFT:
				input_box.set_text_position(input_box.get_text_position() - 1);
				break;
			case event::RIGHT:
				input_box.set_text_position(input_box.get_text_position() + 1);
				break;
			case event::BACK: {
				ib_text_length = (unsigned int)strlen(input_box.get_text());
				ib_text = input_box.get_text();

				char* tok1 = new char[ib_text_length+4];
				for(unsigned int a = 0; a < ib_text_length+4; a++) {
					tok1[a] = 0;
				}

				char* tok2 = new char[ib_text_length+4];
				for(unsigned int a = 0; a < ib_text_length+4; a++) {
					tok2[a] = 0;
				}

				if(ib_text_length != input_box.get_text_position() && input_box.get_text_position() > 0) {
					unsigned int j;
					for(j = 0; j < input_box.get_text_position(); j++) {
						tok1[j] = ib_text[j];
					}
					tok1[j-1] = 0;
					unsigned int k = 0;
					for(j = input_box.get_text_position(); j < ib_text_length; j++) {
						tok2[k] = ib_text[j];
						k++;
					}
					sprintf(set_text, "%s%s", tok1, tok2);
				}
				else {
					// no text exception
					if(ib_text_length != 0) {
						sprintf(tok1, "%s", ib_text);
						//tok1[ib_text_length - 1] = 0;
						tok1[ib_text_length] = 0;
						sprintf(set_text, "%s", tok1);
					}
					else {
						sprintf(set_text, "%s", tok1);
					}
				}

				// no text exception
				if(strlen(set_text) != 0) {
					input_box.set_text(set_text);
					input_box.set_text_position(input_box.get_text_position() - 1);
				}
				else {
					input_box.set_notext();
					input_box.set_text_position(0);
				}

				delete tok1;
				delete tok2;
			}
			break;
			case event::DEL: {
				ib_text_length = (unsigned int)strlen(input_box.get_text());
				ib_text = input_box.get_text();

				char* tok1 = new char[ib_text_length+4];
				for(unsigned int a = 0; a < ib_text_length+4; a++) {
					tok1[a] = 0;
				}

				char* tok2 = new char[ib_text_length+4];
				for(unsigned int a = 0; a < ib_text_length+4; a++) {
					tok2[a] = 0;
				}

				if(ib_text_length != input_box.get_text_position()) {
					unsigned int j;
					for(j = 0; j < input_box.get_text_position(); j++) {
						tok1[j] = ib_text[j];
					}
					unsigned int k = 0;
					for(j = input_box.get_text_position(); j < ib_text_length; j++) {
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
					input_box.set_text(set_text);
				}
				else {
					input_box.set_notext();
				}

				delete tok1;
				delete tok2;
			}
			break;
			case event::HOME:
				input_box.set_text_position(0);
				break;
			case event::END:
				input_box.set_text_position((unsigned int)strlen(input_box.get_text()));
				break;
			default: {
				ib_text_length = (unsigned int)strlen(input_box.get_text());
				ib_text = input_box.get_text();

				char* tok1 = new char[ib_text_length+4];
				for(unsigned int a = 0; a < ib_text_length+4; a++) {
					tok1[a] = 0;
				}

				char* tok2 = new char[ib_text_length+4];
				for(unsigned int a = 0; a < ib_text_length+4; a++) {
					tok2[a] = 0;
				}

				if(ib_text_length != input_box.get_text_position()) {
					for(unsigned int j = 0; j < input_box.get_text_position(); j++) {
						tok1[j] = ib_text[j];
					}
					unsigned int k = 0;
					for(unsigned int j = input_box.get_text_position(); j < ib_text_length; j++) {
						tok2[k] = ib_text[j];
						k++;
					}
					sprintf(set_text, "%s%c%s", tok1, input_text[i], tok2);
				}
				else {
                    sprintf(set_text, "%s%c", ib_text, input_text[i]);
				}

				input_box.set_text(set_text);
				input_box.set_text_position(input_box.get_text_position() + 1);

				delete tok1;
				delete tok2;
			}
			break;
		}
	}
}

/*! deletes a gui element
 *  @param id the id of the gui element we want to delete
 */
bool gui::delete_element(unsigned int id) {
	// routine goes through all gui elements and compares
	// the id of current element withe searched one. if
	// the right id is found, first the specific gui element
	// will be deleted and a deleted flag will be set that
	// on the one side returns true and on the other side
	// overwrites the gui_element of the specific element
	// with the last one in the "stack".

	for(list<gui_element>::iterator elem_iter = gui::gui_elements.begin(); elem_iter != gui::gui_elements.end(); elem_iter++) {
		if(elem_iter->id == id) {
			switch(elem_iter->type) {
				case gui::BUTTON: {
					list<gui_button>::iterator del_button = get_button_iter(id);
					if(&del_button == NULL) {
						m->print(msg::MERROR, "gui.cpp", "delete_element(): can not delete element (id: %u)", id);
						return false;
					}
					delete_element(del_button->get_text_handler()->get_id());
					gui_buttons.erase(del_button);
					gui_elements.erase(elem_iter);
					return true;
				}
				break;
				case gui::CHECK: {
					list<gui_check>::iterator del_check = get_check_iter(id);
					if(&del_check == NULL) {
						m->print(msg::MERROR, "gui.cpp", "delete_element(): can not delete element (id: %u)", id);
						return false;
					}
					delete_element(del_check->get_text_handler()->get_id());
					gui_check_boxes.erase(del_check);
					gui_elements.erase(elem_iter);
					return true;
				}
				break;
				case gui::INPUT: {
					list<gui_input>::iterator del_input = get_input_iter(id);
					if(&del_input == NULL) {
						m->print(msg::MERROR, "gui.cpp", "delete_element(): can not delete element (id: %u)", id);
						return false;
					}
					delete_element(del_input->get_blink_text_handler()->get_id());
					delete_element(del_input->get_text_handler()->get_id());
					gui_input_boxes.erase(del_input);
					gui_elements.erase(elem_iter);
					return true;
				}
				break;
				case gui::LIST: {
					list<gui_list>::iterator del_list = get_list_iter(id);
					if(&del_list == NULL) {
						m->print(msg::MERROR, "gui.cpp", "delete_element(): can not delete element (id: %u)", id);
						return false;
					}
					delete_element(del_list->get_vbar_handler()->get_id());
					gui_list_boxes.erase(del_list);
					gui_elements.erase(elem_iter);
					return true;
				}
				break;
				case gui::TEXT: {
					list<gui_text>::iterator del_text = get_text_iter(id);
					if(&del_text== NULL) {
						m->print(msg::MERROR, "gui.cpp", "delete_element(): can not delete element (id: %u)", id);
						return false;
					}
					gui_texts.erase(del_text);
					gui_elements.erase(elem_iter);
					return true;
				}
				break;
				case gui::VBAR: {
					list<gui_vbar>::iterator del_vbar = get_vbar_iter(id);
					if(&del_vbar == NULL) {
						m->print(msg::MERROR, "gui.cpp", "delete_element(): can not delete element (id: %u)", id);
						return false;
					}
					delete_element(del_vbar->get_down_button_handler()->get_id());
					delete_element(del_vbar->get_up_button_handler()->get_id());
					gui_vbars.erase(del_vbar);
					gui_elements.erase(elem_iter);
					return true;
				}
				break;
				case gui::WINDOW: {
					list<gui_window>::iterator del_window = get_window_iter(id);
					if(&del_window == NULL) {
						m->print(msg::MERROR, "gui.cpp", "delete_element(): can not delete element (id: %u)", id);
						return false;
					}
					gui_windows.erase(del_window);
					gui_elements.erase(elem_iter);
					return true;
				}
				break;
				default:
					m->print(msg::MERROR, "gui.cpp", "delete_element(): can not delete element (id: %u) with unknown type (%u)!", id, elem_iter->type);
					return false;
					break;
			}
		}
	}

	return false;
}

list<gui_button>::iterator gui::get_button_iter(unsigned int id) {
	for(list<gui_button>::iterator iter = gui_buttons.begin(); iter != gui_buttons.end(); iter++) {
		if(id == iter->get_id()) {
			return iter;
		}
	}
	m->print(msg::MERROR, "gui.cpp", "get_button(): no button with such an id (%u) exists!", id);
	return gui_buttons.end();
}

list<gui_input>::iterator gui::get_input_iter(unsigned int id) {
	for(list<gui_input>::iterator iter = gui_input_boxes.begin(); iter != gui_input_boxes.end(); iter++) {
		if(id == iter->get_id()) {
			return iter;
		}
	}
	m->print(msg::MERROR, "gui.cpp", "get_input(): no input box with such an id (%u) exists!", id);
	return gui_input_boxes.end();
}

list<gui_text>::iterator gui::get_text_iter(unsigned int id) {
	for(list<gui_text>::iterator iter = gui_texts.begin(); iter != gui_texts.end(); iter++) {
		if(id == iter->get_id()) {
			return iter;
		}
	}
	m->print(msg::MERROR, "gui.cpp", "get_text(): no text with such an id (%u) exists!", id);
	return gui_texts.end();
}

list<gui_list>::iterator gui::get_list_iter(unsigned int id) {
	for(list<gui_list>::iterator iter = gui_list_boxes.begin(); iter != gui_list_boxes.end(); iter++) {
		if(id == iter->get_id()) {
			return iter;
		}
	}
	m->print(msg::MERROR, "gui.cpp", "get_list(): no list box with such an id (%u) exists!", id);
	return gui_list_boxes.end();
}

list<gui_vbar>::iterator gui::get_vbar_iter(unsigned int id) {
	for(list<gui_vbar>::iterator iter = gui_vbars.begin(); iter != gui_vbars.end(); iter++) {
		if(id == iter->get_id()) {
			return iter;
		}
	}
	m->print(msg::MERROR, "gui.cpp", "get_vbar(): no vertical bar with such an id (%u) exists!", id);
	return gui_vbars.end();
}

list<gui_check>::iterator gui::get_check_iter(unsigned int id) {
	for(list<gui_check>::iterator iter = gui_check_boxes.begin(); iter != gui_check_boxes.end(); iter++) {
		if(id == iter->get_id()) {
			return iter;
		}
	}
	m->print(msg::MERROR, "gui.cpp", "get_check(): no check box with such an id (%u) exists!", id);
	return gui_check_boxes.end();
}

list<gui_window>::iterator gui::get_window_iter(unsigned int id) {
	for(list<gui_window>::iterator iter = gui_windows.begin(); iter != gui_windows.end(); iter++) {
		if(id == iter->get_id()) {
			return iter;
		}
	}
	m->print(msg::MERROR, "gui.cpp", "get_window(): no window with such an id (%u) exists!", id);
	return gui_windows.end();
}

gui_button* gui::get_button(unsigned int id) {
	for(list<gui_button>::iterator iter = gui_buttons.begin(); iter != gui_buttons.end(); iter++) {
		if(id == iter->get_id()) {
			return &*iter; // what the ...?? return iter; doesn't work, but this does ...
		}
	}
	m->print(msg::MERROR, "gui.cpp", "get_button(): no button with such an id (%u) exists!", id);
	return NULL;
}

gui_input* gui::get_input(unsigned int id) {
	for(list<gui_input>::iterator iter = gui_input_boxes.begin(); iter != gui_input_boxes.end(); iter++) {
		if(id == iter->get_id()) {
			return &*iter;
		}
	}
	m->print(msg::MERROR, "gui.cpp", "get_input(): no input box with such an id (%u) exists!", id);
	return NULL;
}

gui_text* gui::get_text(unsigned int id) {
	for(list<gui_text>::iterator iter = gui_texts.begin(); iter != gui_texts.end(); iter++) {
		if(id == iter->get_id()) {
			return &*iter;
		}
	}
	m->print(msg::MERROR, "gui.cpp", "get_text(): no text with such an id (%u) exists!", id);
	return NULL;
}

gui_list* gui::get_list(unsigned int id) {
	for(list<gui_list>::iterator iter = gui_list_boxes.begin(); iter != gui_list_boxes.end(); iter++) {
		if(id == iter->get_id()) {
			return &*iter;
		}
	}
	m->print(msg::MERROR, "gui.cpp", "get_list(): no list box with such an id (%u) exists!", id);
	return NULL;
}

gui_vbar* gui::get_vbar(unsigned int id) {
	for(list<gui_vbar>::iterator iter = gui_vbars.begin(); iter != gui_vbars.end(); iter++) {
		if(id == iter->get_id()) {
			return &*iter;
		}
	}
	m->print(msg::MERROR, "gui.cpp", "get_vbar(): no vertical bar with such an id (%u) exists!", id);
	return NULL;
}

gui_check* gui::get_check(unsigned int id) {
	for(list<gui_check>::iterator iter = gui_check_boxes.begin(); iter != gui_check_boxes.end(); iter++) {
		if(id == iter->get_id()) {
			return &*iter;
		}
	}
	m->print(msg::MERROR, "gui.cpp", "get_check(): no check box with such an id (%u) exists!", id);
	return NULL;
}

gui_window* gui::get_window(unsigned int id) {
	for(list<gui_window>::iterator iter = gui_windows.begin(); iter != gui_windows.end(); iter++) {
		if(id == iter->get_id()) {
			return &*iter;
		}
	}
	m->print(msg::MERROR, "gui.cpp", "get_window(): no window with such an id (%u) exists!", id);
	return NULL;
}

/*! returns true if an gui element with the id 'id' exists, if not, return false
 *  @param id the id we want to check
 */
bool gui::exist(unsigned int id) {
	for(list<gui_element>::iterator iter = gui_elements.begin(); iter != gui_elements.end(); iter++) {
		if(id == iter->id) {
			return true;
		}
	}
	return false;
}
