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
	p2 = new core::pnt();
	r = new gfx::rect();
	r2 = new gfx::rect();

	ofd_wnd_id = 0xFFFFFFFF;
	msg_ok_wnd_id = 0xFFFFFFFF;

	// get classes
	gui::e = e;
	gui::c = e->get_core();
	gui::m = e->get_msg();
	gui::evt = e->get_event();
	gui::g = e->get_gfx();
	gui::gf = e->get_gui_font();
	gui::gs = new gui_style(e);

	icon_cross = e->get_texman()->add_texture(e->data_path("icons/cross.png"), 4, GL_RGBA);
	icon_arrow_up = e->get_texman()->add_texture(e->data_path("icons/aup.png"), 4, GL_RGBA);
	icon_arrow_down = e->get_texman()->add_texture(e->data_path("icons/adown.png"), 4, GL_RGBA);
	icon_checked = e->get_texman()->add_texture(e->data_path("icons/checked.png"), 4, GL_RGBA);
}

/*! there is no function currently
*/
gui::~gui() {
	m->print(msg::MDEBUG, "gui.cpp", "freeing gui stuff");

	delete p;
	delete p2;
	delete r;
	delete r2;

	delete gs;

	gui_buttons.clear();
	gui_texts.clear();
	gui_input_boxes.clear();
	gui_list_boxes.clear();
	gui_vbars.clear();
	gui_check_boxes.clear();
	gui_combo_boxes.clear();
	gui_windows.clear();

	m->print(msg::MDEBUG, "gui.cpp", "gui stuff freed");
}

/*! initializes the gui class and sets an engine and event handler
 */
void gui::init() {
	if(gui::e->get_screen() == NULL) { m->print(msg::MERROR, "gui.cpp", "SDL_Surface does not exist!"); }
	else { gui::gui_surface = gui::e->get_screen(); }

	// init the main window
	main_window = gui::add_window(g->pnt_to_rect(0, 0, e->get_screen()->w,
		e->get_screen()->h), 0, "main window", false, false);

	gui::active_element = gui_elements.begin();
}

/*! draws all gui elements
 */
void gui::draw() {
	// reset event stuff to camera - if there is a gui event,
	// the active class type will be overwritten
	evt->set_active(event::CAMERA);

	// start 2d drawing
	e->start_2d_draw();

	// flag that specifies if the active gui element is reset at the end
	ae_reset = false;

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
	wnd_event = false;
	for(list<unsigned int>::iterator layer_iter = wnd_layers.begin(); layer_iter != wnd_layers.end(); layer_iter++) {
		list<gui_window>::reference wnd_iter = *get_window(*layer_iter);
		if(g->is_pnt_in_rectangle(wnd_iter.get_rectangle(), p) && wnd_iter.get_border()
			&& get_element(wnd_iter.get_id())->is_drawn) {
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
		current_id = *layer_iter;

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
		if(get_element(wnd_iter.get_id())->is_drawn) wnd_iter.draw();

		// set the "window point" that defines how much the
		// gui elements have to be moved on the x and y axis
		core::pnt* wp = new core::pnt();
		if(wnd_iter.get_border()) {
            wp->x = wnd_iter.get_rectangle()->x1 + 2;
            wp->y = wnd_iter.get_rectangle()->y1 + 21;
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
		bool act_elem = false;
		for(list<gui_element>::iterator ge_iter = gui::gui_elements.begin(); ge_iter != gui::gui_elements.end(); ge_iter++) {
			if(ge_iter->is_drawn == true &&
				ge_iter->wid == current_id &&
				ge_iter != active_element) { // active element will be drawn at the end
				draw_element(wp, ge_iter, wnd_iter);
			}
			// "remember" active element
			else if(ge_iter->is_drawn == true &&
				ge_iter->wid == current_id &&
				ge_iter == active_element) {
				act_elem = true;
			}

			// if last element in this layer is drawn, draw active element (if there is one)
			if(act_elem && ge_iter->id == gui::gui_elements.back().id) {
				draw_element(wp, active_element, wnd_iter);
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

			ae_reset = true;
			break;
		}
	}

	if(ae_reset) {
		set_active_element(gui::gui_elements.begin());
	}

	// delete gui elements ...
	/*list<unsigned int> del_ids;
	for(list<gui_element>::iterator ge_iter = gui::gui_elements.begin(); ge_iter != gui::gui_elements.end(); ge_iter++) {
		if(ge_iter->del && ge_iter->type != gui::OPENDIALOG && ge_iter->type != gui::MSGBOX_OK) {
			// if del, delete element
			del_ids.push_back(ge_iter->id);
		}
		else if(ge_iter->del && ge_iter->type == gui::OPENDIALOG) {
			del_ids.push_back(ge_iter->id);
			ofd_wnd = NULL;
		}
		else if(ge_iter->del && ge_iter->type == gui::MSGBOX_OK) {
			del_ids.push_back(ge_iter->id);
			msg_ok_wnd = NULL;
		}
	}*/

	// stop 2d drawing
	e->stop_2d_draw();
}

/*! draws a specific gui element at position wp
 *  @param wp the position on the screen where we want to draw the element
 */
void gui::draw_element(core::pnt* wp, list<gui::gui_element>::iterator ge_iter, list<gui_window>::reference wnd_iter) {
	switch(ge_iter->type) {
		case gui::BUTTON: {
			p->x = evt->get_lm_pressed_x();
			p->y = evt->get_lm_pressed_y();

			// the window button flag - defines if the button is a window button ...
			bool wnd_button = false;

			// well, it's somehow an odd way to do this, but why
			// should i add extra stuff that is just needed once ...
			list<gui_button>::reference cur_button = *get_button(ge_iter->id);
			if(cur_button.get_image() != NULL && cur_button.get_image()->get_texture() == icon_cross) {
				// reposition the exit button so it fits into the windows title bar
				// get the rectangle and add the windows x and y coordinate
				memcpy(r, cur_button.get_rectangle(), sizeof(gfx::rect));
				r->x1 += wnd_iter.get_rectangle()->x2 - 16 - 3;
				r->x2 += wnd_iter.get_rectangle()->x2 - 16 - 3;
				r->y1 += wnd_iter.get_rectangle()->y1 + 2;
				r->y2 += wnd_iter.get_rectangle()->y1 + 2;

				//cout << r->x1 << ", " << r->y1 << " | " << r->x2 << ", " << r->y2 << endl;

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
				g->is_pnt_in_rectangle(r, p) &&
				active_element->type != gui::COMBO) {
				cur_button.set_state("CLICKED");
				if(!wnd_button) {
					cur_button.draw(wp->x, wp->y);
				}
				else {
					cur_button.draw(r->x1, r->y1);
				}
				set_active_element(ge_iter);
			}
			else {
				if(cur_button.get_state() == "CLICKED") {
					cur_button.set_state("NORMAL");
					evt->add_gui_event(evt->BUTTON_PRESSED, cur_button.get_id());
				}

				if(!wnd_button) {
					cur_button.draw(wp->x, wp->y);
				}
				else {
					cur_button.draw(r->x1, r->y1);
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
				g->is_pnt_in_rectangle(r, p) &&
				active_element->type != gui::COMBO) {
				cur_input.set_active(true);
				set_active_element(ge_iter);
				evt->set_active(event::GUI);

				gui::handle_input(cur_input);
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
			r->x2 += wp->x - 20;
			r->y1 += wp->y;
			r->y2 += wp->y;

			if((current_id == gui::wnd_layers.back() || !wnd_event) && g->is_pnt_in_rectangle(r, p) &&
				active_element->type != gui::COMBO) {
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
				cur_vbar.get_slider_active() &&
				active_element->type != gui::COMBO) {
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
			r->x2 = r->x1 + 12;
			r->y1 += wp->y;
			r->y2 = r->y1 + 12;

			if((current_id == gui::wnd_layers.back() || !wnd_event) && g->is_pnt_in_rectangle(r, p)	&&
				active_element->type != gui::COMBO) {
				cur_check.set_checked(cur_check.get_checked() ^ true);
				evt->set_last_pressed(0, 0);
			}

			cur_check.draw(wp->x, wp->y);
		}
		break;
		case gui::COMBO: {
			p->x = evt->get_lm_last_pressed_x();
			p->y = evt->get_lm_last_pressed_y();

			list<gui_combo>::reference cur_combo = *get_combo(ge_iter->id);

			if((current_id == gui::wnd_layers.back() || !wnd_event) &&
				cur_combo.is_list_visible()) {
				r->x1 = wp->x + cur_combo.get_rectangle()->x1 + 1;
				r->x2 = wp->x + cur_combo.get_rectangle()->x2 - 1;
				r->y1 = wp->y + cur_combo.get_rectangle()->y1 + 21;
				r->y2 = wp->y + cur_combo.get_rectangle()->y2 - 1;
				// check, if we clicked somewhere outside of the combo box list ...
				if(!g->is_pnt_in_rectangle(r, p)) {
					memcpy(r2, cur_combo.get_list_button()->get_rectangle(), sizeof(gfx::rect));
					r2->x1 += wp->x;
					r2->x2 += wp->x;
					r2->y1 += wp->y;
					r2->y2 += wp->y;
					// ... if so, check if it was inside the buttons rectangle
					if(!g->is_pnt_in_rectangle(r2, p)) {
						// if so, "close" the list
						cur_combo.set_list_visible(false);
						evt->set_last_pressed(0, 0);
						evt->set_pressed(0, 0);
						ae_reset = true;
					}
					else {
						// if not, select an item
						evt->get_mouse_pos(p);
						if(g->is_pnt_in_rectangle(r, p)) {
							p->x -= wp->x;
							p->y -= wp->y;
							cur_combo.select_pos(p);
						}
						set_active_element(ge_iter);
					}
				}
				else {
					// ... if not, close the list and set the new selected item
					evt->get_mouse_pos(p);
					p->x -= wp->x;
					p->y -= wp->y;
					cur_combo.click_pos(p);
					cur_combo.set_list_visible(false);
					evt->set_last_pressed(0, 0);
					evt->set_pressed(0, 0);
					ae_reset = true;

					evt->add_gui_event(evt->COMBO_ITEM_SELECTED, cur_combo.get_id());
				}
			}

			cur_combo.draw(wp->x, wp->y);
			draw_element(wp, get_element_iter(cur_combo.get_list_button()->get_id()), wnd_iter);
		}
		break;
		case gui::OPENDIALOG: {
			if(exist(ofd_wnd_id)) {
				if(ofd_cancel->get_state() == "CLICKED") {
					ofd_wnd->set_deleted(true);
				}
			}
		}
		break;
		case gui::MSGBOX_OK: {
			if(exist(msg_ok_wnd_id)) {
				if(msg_ok->get_state() == "CLICKED") {
					msg_ok_wnd->set_deleted(true);
				}
			}
		}
		break;
		default:
			break;
	}
}

/*! adds a gui button element and returns it
 *  @param rectangle the buttons rectangle
 *  @param id the buttons (a2e event) id
 *  @param text the buttons text
 *  @param image_texture the image texture we want to set/use
 *  @param wid the id of the window we want the element to be in (0 = no window)
 */
GUI_OBJ gui::add_button(gfx::rect* rectangle, unsigned int id, char* text, GLuint image_texture, unsigned int wid) {
	gui::gui_elements.push_back(*new gui_element());
	gui::gui_elements.back().id = id;
	gui::gui_elements.back().type = gui::BUTTON;
	gui::gui_elements.back().wid = wid;
	gui::gui_elements.back().is_drawn = true;

	// create class
	gui::gui_buttons.push_back(*new gui_button(e, gs));

	gui::gui_buttons.back().set_text_handler(get_text(add_text("STANDARD", 12, text,
		gs->get_color("FONT2"), g->cord_to_pnt(0,0), id+0xFFFF, 0xFFFFFFFF)));
	// don't draw our text automatically
	gui::gui_elements.back().is_drawn = false;

	// set/create the image
	if(image_texture != 0) {
		gui::gui_buttons.back().set_image(new image(e));
		gui::gui_buttons.back().get_image()->set_texture(image_texture);
		if(image_texture == icon_cross) {
			gui::gui_buttons.back().get_image()->set_color(gs->get_color("ICON1"));
		}
		else if(image_texture == icon_arrow_up || image_texture == icon_arrow_down) {
			gui::gui_buttons.back().get_image()->set_color(gs->get_color("ICON2"));
		}
	}

	gui::gui_buttons.back().set_id(id);
	gui::gui_buttons.back().set_rectangle(rectangle);
	gui::gui_buttons.back().set_text(text);

	return id;
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
	if(strcmp(font_name, "STANDARD") == 0) {
		gui::gui_texts.back().set_font(gf->add_font(e->data_path("vera.ttf"), font_size));
		gui::gui_texts.back().set_color(color);
	}
	else {
		gui::gui_texts.back().set_font(gf->add_font(font_name, font_size));
		gui::gui_texts.back().set_color(color);
	}
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
	gui::gui_input_boxes.push_back(*new gui_input(e, gs));

	gui::gui_input_boxes.back().set_text_handler(get_text(add_text("STANDARD", 12, text,
		gs->get_color("FONT"), g->cord_to_pnt(0,0), id+0xFFFF, 0xFFFFFFFF)));
	// don't draw our text automatically
	// celements-1, because our text element, is the last initialized element
	gui::gui_elements.back().is_drawn = false;

	gui::gui_input_boxes.back().set_blink_text_handler(get_text(add_text("STANDARD", 12, " ",
		gs->get_color("FONT"), g->cord_to_pnt(0,0), id+0xFFFFFF, 0xFFFFFFFF)));
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
	gui::gui_list_boxes.push_back(*new gui_list(e, gs));

	// add text
	gui::gui_list_boxes.back().set_text_handler(get_text(add_text("STANDARD", 12, "",
		gs->get_color("FONT"), g->cord_to_pnt(0,0), id+0xFFFF, 0xFFFFFFFF)));
	// don't draw our text automatically
	gui::gui_elements.back().is_drawn = false;

	// add vertical bar
	gui::gui_list_boxes.back().set_vbar_handler(get_vbar(add_vbar(g->pnt_to_rect(rectangle->x2-18,
		rectangle->y1, rectangle->x2, rectangle->y2), id+0xFFFFF, wid)));

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
	gui::gui_vbars.push_back(*new gui_vbar(e, gs));

	// add up button
	gui::gui_vbars.back().set_up_button_handler(get_button(add_button(g->pnt_to_rect(0,0,1,1),
		id+0xFFFF, "", icon_arrow_up, wid)));
	gui::gui_vbars.back().get_up_button_handler()->set_image_scaling(false);
	gui::gui_vbars.back().get_up_button_handler()->set_type("vbar_up_button");

	// add down button
	gui::gui_vbars.back().set_down_button_handler(get_button(add_button(g->pnt_to_rect(0,0,1,1),
		id+0xFFFFF, "", icon_arrow_down, wid)));
	gui::gui_vbars.back().get_down_button_handler()->set_image_scaling(false);
	gui::gui_vbars.back().get_down_button_handler()->set_type("vbar_down_button");

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
	gui::gui_check_boxes.push_back(*new gui_check(e, gs));

	gui::gui_check_boxes.back().set_text_handler(get_text(add_text("STANDARD", 12, text,
		gs->get_color("FONT2"), g->cord_to_pnt(0,0), id+0xFFFF, 0xFFFFFFFF)));
	// don't draw our text automatically
	// celements-1, because our text element, is the last initialized element
	gui::gui_elements.back().is_drawn = false;

	gui::gui_check_boxes.back().set_image(new image(e));
	gui::gui_check_boxes.back().get_image()->set_texture(icon_checked);
	gui::gui_check_boxes.back().get_image()->set_color(gs->get_color("ICON3"));
	gui::gui_check_boxes.back().get_image()->set_scaling(false);

	gui::gui_check_boxes.back().set_id(id);
	gui::gui_check_boxes.back().set_rectangle(rectangle);
	gui::gui_check_boxes.back().set_text(text);

	//return &gui::gui_check_boxes.back();
	return id;
}

/*! adds a gui combo box element and returns it
 *  @param rectangle the combo boxes rectangle
 *  @param id the combo boxes (a2e event) id
 *  @param wid the id of the window we want the element to be in (0 = no window)
 */
GUI_OBJ gui::add_combo_box(gfx::rect* rectangle, unsigned int id, unsigned int wid) {
	gui::gui_elements.push_back(*new gui_element());
	gui::gui_elements.back().id = id;
	gui::gui_elements.back().type = gui::COMBO;
	gui::gui_elements.back().wid = wid;
	gui::gui_elements.back().is_drawn = true;

	// create class
	gui::gui_combo_boxes.push_back(*new gui_combo(e, gs));

	gui::gui_combo_boxes.back().set_text_handler(get_text(add_text("STANDARD", 12, "",
		gs->get_color("FONT"), g->cord_to_pnt(0,0), id+0xFFFF, 0xFFFFFFFF)));
	gui::gui_elements.back().is_drawn = false; // text shouldn't be drawn automatically
	gui::gui_combo_boxes.back().set_list_button(get_button(add_button(g->pnt_to_rect(0,0,1,1),
		id+0xFFFFF, "", icon_arrow_down, 0xFFFFFFFF)));
	gui::gui_combo_boxes.back().get_list_button()->set_image_scaling(false);
	gui::gui_elements.back().is_drawn = false; // button shouldn't be drawn automatically

	gui::gui_combo_boxes.back().set_id(id);
	gui::gui_combo_boxes.back().set_rectangle(rectangle);

	return id;
}

/*! adds a gui window element and returns it
 *  @param rectangle the windows rectangle
 *  @param id the windows (a2e event) id
 *  @param caption the windows caption
 *  @param border a bool if the windows border and titlebar should be drawn
 */
GUI_OBJ gui::add_window(gfx::rect* rectangle, unsigned int id, char* caption, bool border, bool bg) {
	gui::gui_elements.push_back(*new gui_element());
	gui::gui_elements.back().id = id;
	gui::gui_elements.back().type = gui::WINDOW;
	gui::gui_elements.back().wid = 0;
	gui::gui_elements.back().is_drawn = true;

	// create class
	gui::gui_windows.push_back(*new gui_window(e, gs));

	// add exit button if we got a border
	if(border) {
		gui::gui_windows.back().set_exit_button_handler(get_button(add_button(g->pnt_to_rect(0,0,16,16),
			id+0xFFFFFF, "", icon_cross, id)));
		gui::gui_windows.back().get_exit_button_handler()->set_image_scaling(false);
		gui::gui_windows.back().get_exit_button_handler()->set_type("window_button");
	}

	gui::gui_windows.back().set_text_handler(get_text(add_text("STANDARD", 12, caption,
		gs->get_color("FONT2"), g->cord_to_pnt(0,0), id+0xFFFF, id)));
	// don't draw our text automatically
	// celements-1, because our text element, is the last initialized element
	gui::gui_elements.back().is_drawn = false;

	gui::gui_windows.back().set_id(id);
	gui::gui_windows.back().set_rectangle(rectangle);
	gui::gui_windows.back().set_caption(caption);
	gui::gui_windows.back().set_lid((unsigned int)gui_windows.size());
	gui::gui_windows.back().set_border(border);
	gui::gui_windows.back().set_bg(bg);

	// add the windows to the layer stack/list
	// it will automatically become active / the one in the front
	wnd_layers.push_back(id);

	evt->set_pressed(rectangle->x1, rectangle->y1);

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
void gui::handle_input(list<gui_input>::reference input_box) {
	stringstream* buffer = evt->get_buffer();
	char x;

	for(unsigned int i = 0; i < (unsigned int)buffer->str().size(); i++) {
		x = c->get_char(buffer);
		switch(x) {
			case event::IT_LEFT:
				input_box.set_text_position(input_box.get_text_position() - 1);
				break;
			case event::IT_RIGHT:
				input_box.set_text_position(input_box.get_text_position() + 1);
				break;
			case event::IT_BACK: {
				unsigned int pos = input_box.get_text_position();
				string* text = input_box.get_text();

				if(pos == 0) break;
				text->erase(pos-1, 1);
				input_box.set_text((char*)text->c_str());
				input_box.set_text_position(pos - 1);
			}
			break;
			case event::IT_DEL: {
				unsigned int pos = input_box.get_text_position();
				string* text = input_box.get_text();

				if(pos == text->length()) break;
				text->erase(pos, 1);
				input_box.set_text((char*)text->c_str());
			}
			break;
			case event::IT_HOME:
				input_box.set_text_position(0);
				break;
			case event::IT_END:
				input_box.set_text_position((unsigned int)input_box.get_text()->length());
				break;
			default: {
				// add char to input text
				unsigned int pos = input_box.get_text_position();
				string* text = input_box.get_text();

				text->insert(text->begin()+pos, x);
				input_box.set_text(text->c_str());
				input_box.set_text_position(pos+1);
			}
			break;
		}
	}

	buffer->str("");
	buffer->clear();
}

/*! deletes a gui element
 *  @param id the id of the gui element we want to delete
 */
bool gui::delete_element(unsigned int id) {
	// routine goes through all gui elements and compares
	// the id of current element with the searched one. if
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
					if(del_button->get_text_handler() != NULL) {
						delete_element(del_button->get_text_handler()->get_id());
					}
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
					if(del_check->get_text_handler() != NULL) {
						delete_element(del_check->get_text_handler()->get_id());
					}
					gui_check_boxes.erase(del_check);
					gui_elements.erase(elem_iter);
					return true;
				}
				break;
				case gui::COMBO: {
					list<gui_combo>::iterator del_combo = get_combo_iter(id);
					if(&del_combo == NULL) {
						m->print(msg::MERROR, "gui.cpp", "delete_element(): can not delete element (id: %u)", id);
						return false;
					}
					if(del_combo->get_text_handler() != NULL) {
						delete_element(del_combo->get_text_handler()->get_id());
					}
					delete_element(del_combo->get_list_button()->get_id());
					gui_combo_boxes.erase(del_combo);
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
					if(del_input->get_text_handler() != NULL) {
						delete_element(del_input->get_text_handler()->get_id());
					}
					delete_element(del_input->get_blink_text_handler()->get_id());
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
					if(del_list->get_text_handler() != NULL) {
						delete_element(del_list->get_text_handler()->get_id());
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
					if(del_vbar->get_text_handler() != NULL) {
						delete_element(del_vbar->get_text_handler()->get_id());
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
					if(del_window->get_text_handler() != NULL) {
						delete_element(del_window->get_text_handler()->get_id());
					}
					gui_windows.erase(del_window);
					gui_elements.erase(elem_iter);
					return true;
				}
				break;
				case gui::OPENDIALOG: {
					if(ofd_wnd == NULL) {
						m->print(msg::MERROR, "gui.cpp", "delete_element(): can not delete element (id: %u)", id);
						return false;
					}
					delete_element(ofd_open->get_id());
					delete_element(ofd_cancel->get_id());
					delete_element(ofd_dirlist->get_id());
					gui_elements.erase(elem_iter);
					return true;
				}
				break;
				case gui::MSGBOX_OK: {
					if(msg_ok_wnd == NULL) {
						m->print(msg::MERROR, "gui.cpp", "delete_element(): can not delete element (id: %u)", id);
						return false;
					}
					delete_element(msg_text->get_id());
					delete_element(msg_ok->get_id());
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
	m->print(msg::MERROR, "gui.cpp", "get_button_iter(): no button with such an id (%u) exists!", id);
	return gui_buttons.end();
}

list<gui_input>::iterator gui::get_input_iter(unsigned int id) {
	for(list<gui_input>::iterator iter = gui_input_boxes.begin(); iter != gui_input_boxes.end(); iter++) {
		if(id == iter->get_id()) {
			return iter;
		}
	}
	m->print(msg::MERROR, "gui.cpp", "get_input_iter(): no input box with such an id (%u) exists!", id);
	return gui_input_boxes.end();
}

list<gui_text>::iterator gui::get_text_iter(unsigned int id) {
	for(list<gui_text>::iterator iter = gui_texts.begin(); iter != gui_texts.end(); iter++) {
		if(id == iter->get_id()) {
			return iter;
		}
	}
	m->print(msg::MERROR, "gui.cpp", "get_text_iter(): no text with such an id (%u) exists!", id);
	return gui_texts.end();
}

list<gui_list>::iterator gui::get_list_iter(unsigned int id) {
	for(list<gui_list>::iterator iter = gui_list_boxes.begin(); iter != gui_list_boxes.end(); iter++) {
		if(id == iter->get_id()) {
			return iter;
		}
	}
	m->print(msg::MERROR, "gui.cpp", "get_list_iter(): no list box with such an id (%u) exists!", id);
	return gui_list_boxes.end();
}

list<gui_vbar>::iterator gui::get_vbar_iter(unsigned int id) {
	for(list<gui_vbar>::iterator iter = gui_vbars.begin(); iter != gui_vbars.end(); iter++) {
		if(id == iter->get_id()) {
			return iter;
		}
	}
	m->print(msg::MERROR, "gui.cpp", "get_vbar_iter(): no vertical bar with such an id (%u) exists!", id);
	return gui_vbars.end();
}

list<gui_check>::iterator gui::get_check_iter(unsigned int id) {
	for(list<gui_check>::iterator iter = gui_check_boxes.begin(); iter != gui_check_boxes.end(); iter++) {
		if(id == iter->get_id()) {
			return iter;
		}
	}
	m->print(msg::MERROR, "gui.cpp", "get_check_iter(): no check box with such an id (%u) exists!", id);
	return gui_check_boxes.end();
}

list<gui_combo>::iterator gui::get_combo_iter(unsigned int id) {
	for(list<gui_combo>::iterator iter = gui_combo_boxes.begin(); iter != gui_combo_boxes.end(); iter++) {
		if(id == iter->get_id()) {
			return iter;
		}
	}
	m->print(msg::MERROR, "gui.cpp", "get_combo_iter(): no combo box with such an id (%u) exists!", id);
	return gui_combo_boxes.end();
}

list<gui_window>::iterator gui::get_window_iter(unsigned int id) {
	for(list<gui_window>::iterator iter = gui_windows.begin(); iter != gui_windows.end(); iter++) {
		if(id == iter->get_id()) {
			return iter;
		}
	}
	m->print(msg::MERROR, "gui.cpp", "get_window_iter(): no window with such an id (%u) exists!", id);
	return gui_windows.end();
}

list<gui::gui_element>::iterator gui::get_element_iter(unsigned int id) {
	for(list<gui_element>::iterator iter = gui_elements.begin(); iter != gui_elements.end(); iter++) {
		if(id == iter->id) {
			return iter;
		}
	}
	m->print(msg::MERROR, "gui.cpp", "get_element_iter(): no gui element with such an id (%u) exists!", id);
	return gui_elements.end();
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

gui_combo* gui::get_combo(unsigned int id) {
	for(list<gui_combo>::iterator iter = gui_combo_boxes.begin(); iter != gui_combo_boxes.end(); iter++) {
		if(id == iter->get_id()) {
			return &*iter;
		}
	}
	m->print(msg::MERROR, "gui.cpp", "get_combo(): no combo box with such an id (%u) exists!", id);
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

gui::gui_element* gui::get_element(unsigned int id) {
	for(list<gui_element>::iterator iter = gui_elements.begin(); iter != gui_elements.end(); iter++) {
		if(id == iter->id) {
			return &*iter;
		}
	}
	m->print(msg::MERROR, "gui.cpp", "get_element(): no gui element with such an id (%u) exists!", id);
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

/*! sets the visibilty of the gui element specified by id to state
 *  @param id the id of the gui element
 *  @param state the visibility state we want to set
 */
void gui::set_visibility(unsigned int id, bool state, bool force) {
	if(id >= 0xFFFF && !force) return; // exit routine for every gui element that has an id bigger than 0xFFFF (-> all non automatically drawn elements)
	gui::get_element(id)->is_drawn = state;

	switch(gui::get_element(id)->type) {
		case gui::LIST:
			gui::set_visibility(gui::get_list(id)->get_text_handler()->get_id(), state);
			gui::set_visibility(gui::get_list(id)->get_vbar_handler()->get_id(), state, true);
			break;
		case gui::VBAR:
			gui::set_visibility(gui::get_vbar(id)->get_down_button_handler()->get_id(), state, true);
			gui::set_visibility(gui::get_vbar(id)->get_up_button_handler()->get_id(), state, true);
			break;
		case gui::COMBO:
			gui::set_visibility(gui::get_combo(id)->get_list_button()->get_id(), state, true);
			break;
		case gui::WINDOW: {
			gui_window* wnd = gui::get_window(id);
			if(wnd->get_border()) {
				gui::set_visibility(wnd->get_exit_button_handler()->get_id(), state, true);
			}
			// set the visibility of all window elements to state
			for(list<gui_element>::iterator iter = gui_elements.begin(); iter != gui_elements.end(); iter++) {
				if(iter->wid == wnd->get_id()) {
					set_visibility(iter->id, state);
				}
			}
		}
		break;
		case gui::MSGBOX_OK: {
			if(!exist(msg_ok_wnd_id)) break;
			gui::set_visibility(msg_ok_wnd_id, state);
		}
		break;
		case gui::OPENDIALOG: {
			if(!exist(ofd_wnd_id)) break;
			gui::set_visibility(ofd_wnd_id, state);
		}
		break;
		default:
			break;
	}
}

/*! creates an open file dialog
 *  @param id the id of the open file dialog
 *  @param caption the windows caption
 *  @param dir the directory where the file dialog should "start"
 *  @param ext the extension of the files to be displayed
 *  @param x the x position of the window
 *  @param y the y position of the window
 */
GUI_OBJ gui::add_open_dialog(unsigned int id, char* caption, char* dir, char* ext, unsigned int x, unsigned int y) {
	if(exist(ofd_wnd_id)) {
		if(!ofd_wnd->get_deleted()) return 0;
	}

	string path = dir;
	gui::gui_elements.push_back(*new gui_element());
	gui::gui_elements.back().id = id;
	gui::gui_elements.back().type = gui::OPENDIALOG;
	gui::gui_elements.back().wid = 0;
	gui::gui_elements.back().is_drawn = true;

	ofd_wnd_id = add_window(e->get_gfx()->pnt_to_rect(x, y, 370+x, 221+y), id+1, caption, true, true);
	ofd_wnd = get_window(ofd_wnd_id);
	ofd_open = get_button(add_button(e->get_gfx()->pnt_to_rect(302, 0, 365, 20), id+2, "Open", 0, id+1));
	ofd_cancel = get_button(add_button(e->get_gfx()->pnt_to_rect(302, 20, 365, 40), 1, "Cancel", 0, id+1));
	ofd_dirlist = get_list(add_list_box(e->get_gfx()->pnt_to_rect(0, 0, 300, 198), id+3, id+1));

#ifdef WIN32
	struct _finddata_t c_file;
	intptr_t hFile;
	unsigned int i = 0;

	char* fstring = new char[strlen(path.c_str()) + 2 + strlen(ext) + 1];
	strcpy(fstring, path.c_str());
	strcat(fstring, "*.");
	strcat(fstring, ext);
	if((hFile = _findfirst(fstring, &c_file)) != -1L) {
		do {
			unsigned int fnlen = (unsigned int)strlen(c_file.name);
			if(c_file.size != 0 &&
				c_file.name[fnlen-3] == ext[strlen(ext)-3] &&
				c_file.name[fnlen-2] == ext[strlen(ext)-2] &&
				c_file.name[fnlen-1] == ext[strlen(ext)-1]) {
				ofd_dirlist->add_item(c_file.name, i);
				i++;
			}
		}
		while(_findnext(hFile, &c_file) == 0);

		_findclose(hFile);
	}
	delete [] fstring;
#else
	struct dirent **namelist;
	unsigned int i = 0;

	string fstring = path;
	fstring += ".";
	int n = scandir(fstring.c_str(), &namelist, 0, alphasort);
	if (n >= 0) {
		while(n--) {
			unsigned int fnlen = (unsigned int)strlen(namelist[n]->d_name);
			if(namelist[n]->d_type != DT_DIR &&
				namelist[n]->d_name[fnlen-3] == ext[strlen(ext)-3] &&
				namelist[n]->d_name[fnlen-2] == ext[strlen(ext)-2] &&
				namelist[n]->d_name[fnlen-1] == ext[strlen(ext)-1]) {
				ofd_dirlist->add_item(namelist[n]->d_name, i);
				i++;
			}
			free(namelist[n]);
		}
		free(namelist);
	}
#endif

	return ofd_wnd_id;
}

gui_list* gui::get_open_diaolg_list() {
	return ofd_dirlist;
}

GUI_OBJ gui::add_msgbox_ok(unsigned int id, char* caption, char* text) {
	if(exist(msg_ok_wnd_id)) {
		if(!msg_ok_wnd->get_deleted()) return 0;
	}

	gui::gui_elements.push_back(*new gui_element());
	gui::gui_elements.back().id = id;
	gui::gui_elements.back().type = gui::MSGBOX_OK;
	gui::gui_elements.back().wid = 0;
	gui::gui_elements.back().is_drawn = true;

	unsigned int sw = (unsigned int)((float)e->get_screen()->w * 0.2f);
	unsigned int sh = (unsigned int)((float)e->get_screen()->h * 0.4f);
	unsigned int w = (unsigned int)((float)e->get_screen()->w * 0.6f);
	unsigned int h = (unsigned int)((float)e->get_screen()->h * 0.2f);

	msg_ok_wnd_id = add_window(e->get_gfx()->pnt_to_rect(sw, sh, sw + w, sh + h), id+1, caption, true, true);
	msg_ok_wnd = get_window(msg_ok_wnd_id);
	msg_ok = get_button(add_button(e->get_gfx()->pnt_to_rect((unsigned int)((float)w * 0.5f) - 32,
		h - 50, (unsigned int)((float)w * 0.5f) + 32, h - 30), id+2, "OK", 0, id+1));
	msg_text = get_text(add_text("STANDARD", 12, text, gs->get_color("FONT"),
		e->get_gfx()->cord_to_pnt(0, 0), id+3, id+1));
	unsigned int tw = (unsigned int)((float)(w - msg_text->get_text_width()) / 2.0f);
	unsigned int th = (unsigned int)((float)h / 2.0f) - 30;
	msg_text->get_point()->x = tw;
	msg_text->get_point()->y = th;

	return msg_ok_wnd_id;
}

gui_style* gui::get_gui_style() {
	return gs;
}

void gui::set_color_scheme(const char* scheme) {
	if(!gs->set_color_scheme(scheme)) return;

	// reset the button image color of each "system button"
	GLuint tex;
	for(list<gui_element>::iterator iter = gui_elements.begin(); iter != gui_elements.end(); iter++) {
		if(iter->type == gui::BUTTON) {
			tex = get_button(iter->id)->get_image()->get_texture();
			if(tex == icon_cross) {
				get_button(iter->id)->get_image()->set_color(gs->get_color("ICON1"));
			}
			else if(tex == icon_arrow_up || tex == icon_arrow_down) {
				get_button(iter->id)->get_image()->set_color(gs->get_color("ICON2"));
			}
		}
		else if(iter->type == gui::CHECK) {
			get_check(iter->id)->get_image()->set_color(gs->get_color("ICON3"));
		}
	}
}

/*! sets the focus on the object with the specified id
 *  @param id the objects id
 */
void gui::set_focus(unsigned int id) {
	//TODO: actually write this routine ;P ... dunno if this already works ...
	// is object a window?
	if(get_element(id)->type == gui::WINDOW) {
		// check, if the specified id is the id of a window
		bool found = false;
		for(list<gui_window>::iterator iter = gui_windows.begin(); iter != gui_windows.end(); iter++) {
			if(id == iter->get_id()) {
				found = true;
				break;
			}
		}
		if(!found) {
			m->print(msg::MDEBUG, "gui.cpp", "set_focus(): window: no window with such an id (%u) exists!", id);
			return;
		}

		wnd_layers.remove(id);
		wnd_layers.push_back(id);
	}
}
