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
gui::gui(engine* e, shader* s) {
	p = new core::pnt();
	p2 = new core::pnt();
	r1 = new gfx::rect();
	r2 = new gfx::rect();

	ofd_wnd_id = 0xFFFFFFFF;

	start_id = 0xFFFF;

	// get classes
	gui::e = e;
	gui::c = e->get_core();
	gui::m = e->get_msg();
	gui::evt = e->get_event();
	gui::g = e->get_gfx();
	gui::gf = e->get_gui_font();
	gui::gs = new gui_style(e);
	gui::r = e->get_rtt();
	gui::s = s;

	shadow_type = e->get_shadow_type();
	// set shadows to disabled if we don't have shader support
	if(shadow_type != 0 && !e->get_ext()->is_shader_support()) shadow_type = 0;

	tcs = new float[50];

	if(shadow_type == 1) {
		xcorrect = 14;
		ycorrect = 9;
		xadd = 13.0f;
		yadd = 13.0f;
	}
	else if(shadow_type == 2) {
		xcorrect = 15;
		ycorrect = 10;
		xadd = 15.0f;
		yadd = 15.0f;
	}

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
	delete r1;
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
	gui_mltexts.clear();
	gui_images.clear();

	for(list<gui::window_buffer*>::iterator iter = window_buffers.begin(); iter != window_buffers.end(); iter++) {
		r->delete_buffer((*iter)->buffer);
		if((*iter)->shadow != NULL) r->delete_buffer((*iter)->shadow);
	}
	window_buffers.clear();

	delete [] tcs;

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
		list<gui_window>::reference wnd_iter = *get_object<gui_window>(*layer_iter);
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
		list<gui_window>::reference wnd_iter = *get_object<gui_window>(*layer_iter);
		current_id = *layer_iter;

		// if window is being moved set window event flag to true
		if(wnd_iter.is_moving()) {
			wnd_event = true;
		}

		// check if we got a window with a title bar and border
		if(wnd_iter.get_border()) {
			// reposition the window if it has been moved or is moving
			memcpy(r1, wnd_iter.get_rectangle(), sizeof(gfx::rect));
			r1->x1 += 1;
			r1->x2 -= 1;
			r1->y2 = r1->y1 + 17;
			r1->y1 += 1;

			if(event_type == 1 && wnd_iter.is_moving()) {
				wnd_iter.set_moving(false);
			}

			if((g->is_pnt_in_rectangle(r1, p) || wnd_iter.is_moving())
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
		//if(get_element(wnd_iter.get_id())->is_drawn) wnd_iter.draw();

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

		// obsolete comment?
		// we need a rectangle object to add the windows
		// x and y coordinate to the gui elements rectangle,
		// so we can check for a button press correctly
		// --- global r1 object ---

		// handle the windows gui elements
		bool act_elem = false;
		bool act_elem_tab = false;
		bool changed = false;
		for(list<gui_element>::iterator ge_iter = gui::gui_elements.begin(); ge_iter != gui::gui_elements.end(); ge_iter++) {
			if(ge_iter->is_drawn == true &&
				ge_iter->wid == current_id &&
				ge_iter->tid == 0) {
				if(handle_element(wp, ge_iter, wnd_iter) && !changed) changed = true;

				// if current element is a tab, handle all (active) tab elements
				if(ge_iter->type == gui::TAB) {
					gui_tab* tab = get_object<gui_tab>(ge_iter->id);
					unsigned int active_id = tab->get_active_tab();
					// add tab position
					wp->x += tab->get_rectangle()->x1 + 3;
					wp->y += tab->get_rectangle()->y1 + 21;
					for(list<gui_element>::iterator tge_iter = gui::gui_elements.begin(); tge_iter != gui::gui_elements.end(); tge_iter++) {
						if(tge_iter->is_drawn == true &&
							tge_iter->wid == current_id &&
							tge_iter->tid != 0) {
							// since gui_text isn't derived from gui_object, we have to do some special treatment for it
							// check if the element is in the active tab
							if((tge_iter->type != gui::TEXT && get_object(tge_iter->id)->get_tab() == active_id)
								|| (tge_iter->type == gui::TEXT && get_object<gui_text>(tge_iter->id)->get_tab() == active_id)) {
								if(handle_element(wp, tge_iter, wnd_iter) && !changed) changed = true;
							}
						}
					}
					// subtract tab position
					wp->x -= tab->get_rectangle()->x1 + 3;
					wp->y -= tab->get_rectangle()->y1 + 21;
				}
			}
		}

		// handle window itself
		wnd_iter.handle_window();

		// get window redraw flag
		if(!changed) changed = wnd_iter.get_redraw();

		// get window buffer
		rtt::fbo* buffer = NULL;
		for(list<gui::window_buffer*>::iterator iter = window_buffers.begin(); iter != window_buffers.end(); iter++) {
			if((*iter)->wid == wnd_iter.get_id()) {
				buffer = (*iter)->buffer;
				break;
			}
		}

		if(changed) {
			// new window point (b/c of using rtt)
			if(wnd_iter.get_border()) {
				wp->x = 2;
				wp->y = 21;
			}
			else {
				wp->x = 0;
				wp->y = 0;
			}

			// start rendering
			r->start_draw(buffer);

			// draw all gui elements again if we have to redraw the window,
			// otherwise only render those elements again that have been updated
			bool wnd_redraw = wnd_iter.get_redraw();

			if(wnd_redraw) r->clear();
			r->start_2d_draw();

			// draw the window
			if(wnd_redraw && get_element(wnd_iter.get_id())->is_drawn) wnd_iter.draw();

			// draw the windows elements
			for(list<gui_element>::iterator ge_iter = gui::gui_elements.begin(); ge_iter != gui::gui_elements.end(); ge_iter++) {
				if(ge_iter->is_drawn == true &&
					ge_iter->wid == current_id &&
					(is_redraw(ge_iter) || wnd_redraw || ge_iter->type == gui::TAB) &&
					ge_iter != active_element &&
					ge_iter->tid == 0) {
					bool tab_redraw = true;
					gui_tab* tab;
					if(ge_iter->type == gui::TAB) {
						tab = get_object<gui_tab>(ge_iter->id);
						tab_redraw = tab->get_redraw();
					}
					if(tab_redraw || wnd_redraw) draw_element(wp, ge_iter, wnd_iter);

					// if current element is a tab, draw all (active) tab elements
					if(ge_iter->type == gui::TAB) {
						unsigned int active_id = tab->get_active_tab();
						// add tab position
						wp->x += tab->get_rectangle()->x1 + 3;
						wp->y += tab->get_rectangle()->y1 + 21;
						for(list<gui_element>::iterator tge_iter = gui::gui_elements.begin(); tge_iter != gui::gui_elements.end(); tge_iter++) {
							if(tge_iter->is_drawn == true &&
								tge_iter->wid == current_id &&
								(is_redraw(tge_iter) || wnd_redraw || tab_redraw) &&
								tge_iter->tid != 0) {
								// since gui_text isn't derived from gui_object, we have to do some special treatment for it
								// check if the element is in the active tab
								if((tge_iter->type != gui::TEXT && get_object(tge_iter->id)->get_tab() == active_id)
									|| (tge_iter->type == gui::TEXT && get_object<gui_text>(tge_iter->id)->get_tab() == active_id)) {
									if(tge_iter != active_element) draw_element(wp, tge_iter, wnd_iter);
									// "remember" active element
									else act_elem_tab = true;
								}
							}

							// if last element in this layer is drawn, draw active element (if there is one)
							if(act_elem_tab && tge_iter->id == gui::gui_elements.back().id) {
								draw_element(wp, active_element, wnd_iter);
							}
						}
						// subtract tab position
						wp->x -= tab->get_rectangle()->x1 + 3;
						wp->y -= tab->get_rectangle()->y1 + 21;
					}
				}
				// "remember" active element
				else if(ge_iter->is_drawn == true &&
					ge_iter->wid == current_id &&
					(is_redraw(ge_iter) || wnd_redraw) &&
					ge_iter == active_element &&
					ge_iter->tid == 0) {
					act_elem = true;
				}

				// if last element in this layer is drawn, draw active element (if there is one)
				if(act_elem && ge_iter->id == gui::gui_elements.back().id) {
					draw_element(wp, active_element, wnd_iter);
				}
			}

			r->stop_2d_draw();

			// set the windows redraw flag to false
			if(wnd_redraw) wnd_iter.set_redraw(false);

			// stop rendering
			r->stop_draw();
		}

		delete wp;
		j++;
	}

	// draw windows
	// start 2d drawing
	e->start_2d_draw();
	rtt::fbo* buffer = NULL;
	rtt::fbo* shadow = NULL;
	for(list<unsigned int>::iterator layer_iter = wnd_layers.begin(); layer_iter != wnd_layers.end(); layer_iter++) {
		list<gui_window>::reference wnd_iter = *get_object<gui_window>(*layer_iter);

		// get buffer
		for(list<gui::window_buffer*>::iterator iter = window_buffers.begin(); iter != window_buffers.end(); iter++) {
			if((*iter)->wid == wnd_iter.get_id()) {
				buffer = (*iter)->buffer;
				if(shadow_type != 0 && wnd_iter.get_bg() && wnd_iter.get_border() && (*iter)->shadow != NULL) shadow = (*iter)->shadow;
				else shadow = NULL;
				break;
			}
		}

		// render shadow
		if(shadow != NULL) {
			gfx::rect* wnd_rect = wnd_iter.get_rectangle();
			glTranslatef(0.0f, 0.0f, 0.0f);
			glEnable(GL_TEXTURE_2D);
			glEnable(GL_BLEND);
			glBindTexture(GL_TEXTURE_2D, shadow->tex_id);
			glColor3f(1.0f, 1.0f, 1.0f);
			glBegin(GL_QUADS);
				glTexCoord2f(0.0f, 0.0f);
				glVertex2i(wnd_rect->x1-xcorrect, wnd_rect->y1-ycorrect);
				glTexCoord2f(0.0f, 1.0f);
				glVertex2i(wnd_rect->x1-xcorrect, wnd_rect->y1-ycorrect + shadow->height);
				glTexCoord2f(1.0f, 1.0f);
				glVertex2i(wnd_rect->x1-xcorrect + shadow->width, wnd_rect->y1-ycorrect + shadow->height);
				glTexCoord2f(1.0f, 0.0f);
				glVertex2i(wnd_rect->x1-xcorrect + shadow->width, wnd_rect->y1-ycorrect);
			glEnd();
			glDisable(GL_BLEND);
			glDisable(GL_TEXTURE_2D);
		}

		// render buffer to screen
		gfx::rect* wnd_rect = wnd_iter.get_rectangle();
		glTranslatef(0.0f, 0.0f, 0.0f);
		glEnable(GL_TEXTURE_2D);
		glEnable(GL_BLEND);
		glBindTexture(GL_TEXTURE_2D, buffer->tex_id);
		glColor3f(1.0f, 1.0f, 1.0f);
		glBegin(GL_QUADS);
			glTexCoord2f(0.0f, 0.0f);
			glVertex2i(wnd_rect->x1, wnd_rect->y1);
			glTexCoord2f(0.0f, 1.0f);
			glVertex2i(wnd_rect->x1, wnd_rect->y1 + buffer->height);
			glTexCoord2f(1.0f, 1.0f);
			glVertex2i(wnd_rect->x1 + buffer->width, wnd_rect->y1 + buffer->height);
			glTexCoord2f(1.0f, 0.0f);
			glVertex2i(wnd_rect->x1 + buffer->width, wnd_rect->y1);
		glEnd();
		glDisable(GL_BLEND);
		glDisable(GL_TEXTURE_2D);
	}
	// stop 2d drawing
	e->stop_2d_draw();

	for(list<gui_window*>::iterator wnd_iter = gui_windows.begin(); wnd_iter != gui_windows.end(); wnd_iter++) {
		// check if the windows exit button was pressed and delete the window
		if((*wnd_iter)->get_deleted()) {
			// delete window elements
			unsigned int* welem_ids = new unsigned int[gui::gui_elements.size()]; // not very memory sparing, but it works ... todo: make that stuff better ...
			unsigned int x = 0;
			for(list<gui_element>::iterator wid_iter = gui::gui_elements.begin(); wid_iter != gui::gui_elements.end(); wid_iter++) {
				if(wid_iter->wid == (*wnd_iter)->get_id()) {
					// save the id, so we can delete the element later on
					welem_ids[x] = wid_iter->id;
					x++;
				}
			}
			for(unsigned int k = 0; k < x; k++) {
				gui::delete_element(welem_ids[k]);
			}
			delete [] welem_ids;

			gui::wnd_layers.remove((*wnd_iter)->get_id());
			gui::delete_element((*wnd_iter)->get_id());

			ae_reset = true;
			break;
		}
	}

	if(ae_reset) {
		set_active_element(gui::gui_elements.begin());
	}
}

/*! draws a specific gui element at position wp
 *  @param wp the position on the screen where we want to draw the element
 */
void gui::draw_element(core::pnt* wp, list<gui::gui_element>::iterator ge_iter, list<gui_window>::reference wnd_iter) {
	switch(ge_iter->type) {
		case gui::BUTTON: {
			// the window button flag - defines if the button is a window button ...
			bool wnd_button = false;

			// well, it's somehow an odd way to do this, but why
			// should i add extra stuff that is just needed once ...
			list<gui_button>::reference cur_button = *get_object<gui_button>(ge_iter->id);
			if(cur_button.get_image() != NULL && cur_button.get_image()->get_texture() == icon_cross) {
				wnd_button = true;
			}

			if(!wnd_button) {
				cur_button.draw(wp->x, wp->y);
			}
			else {
				memcpy(r1, cur_button.get_rectangle(), sizeof(gfx::rect));
				r1->x1 += (wnd_iter.get_rectangle()->x2 - wnd_iter.get_rectangle()->x1) - 16 - 3;
				r1->x2 += (wnd_iter.get_rectangle()->x2 - wnd_iter.get_rectangle()->x1) - 16 - 3;
				r1->y1 += 2;
				r1->y2 += 2;
				cur_button.draw(r1->x1, r1->y1);
			}

			cur_button.set_redraw(false);
		}
		break;
		case gui::TEXT: {
			list<gui_text>::reference cur_text = *get_object<gui_text>(ge_iter->id);
			cur_text.draw(wp->x, wp->y);
			cur_text.set_redraw(false);
		}
		break;
		case gui::INPUT: {
			list<gui_input>::reference cur_input = *get_object<gui_input>(ge_iter->id);
			cur_input.draw(wp->x, wp->y);
			cur_input.set_redraw(false);
		}
		break;
		case gui::LIST: {
			list<gui_list>::reference cur_list = *get_object<gui_list>(ge_iter->id);
			cur_list.draw(wp->x, wp->y);
			cur_list.set_redraw(false);
		}
		break;
		case gui::VBAR: {
			list<gui_vbar>::reference cur_vbar = *get_object<gui_vbar>(ge_iter->id);
			cur_vbar.draw(wp->x, wp->y);
			cur_vbar.set_redraw(false);
		}
		break;
		case gui::CHECK: {
			list<gui_check>::reference cur_check = *get_object<gui_check>(ge_iter->id);
			cur_check.draw(wp->x, wp->y);
			cur_check.set_redraw(false);
		}
		break;
		case gui::COMBO: {
			list<gui_combo>::reference cur_combo = *get_object<gui_combo>(ge_iter->id);
			cur_combo.draw(wp->x, wp->y);
			draw_element(wp, get_element_iter(cur_combo.get_list_button()->get_id()), wnd_iter);
			cur_combo.set_redraw(false);
		}
		break;
		case gui::MLTEXT: {
			list<gui_mltext>::reference cur_mltext = *get_object<gui_mltext>(ge_iter->id);
			cur_mltext.draw(wp->x, wp->y);
			cur_mltext.set_redraw(false);
		}
		break;
		case gui::IMAGE: {
			list<gui_image>::reference cur_image = *get_object<gui_image>(ge_iter->id);
			cur_image.draw(wp->x, wp->y);
			cur_image.set_redraw(false);
		}
		break;
		case gui::TAB: {
			gui_tab* cur_tab = get_object<gui_tab>(ge_iter->id);
			cur_tab->draw(wp->x, wp->y);
			cur_tab->set_redraw(false);
		}
		break;
		default:
			break;
	}
}

/*! handles a specific gui element at position wp and returns true if something has changed
 */
bool gui::handle_element(core::pnt* wp, list<gui::gui_element>::iterator ge_iter, list<gui_window>::reference wnd_iter) {
	bool changed = false;
	switch(ge_iter->type) {
		case gui::BUTTON: {
			p->x = evt->get_lm_pressed_x();
			p->y = evt->get_lm_pressed_y();

			// the window button flag - defines if the button is a window button ...
			bool wnd_button = false;

			// well, it's somehow an odd way to do this, but why
			// should i add extra stuff that is just needed once ...
			list<gui_button>::reference cur_button = *get_object<gui_button>(ge_iter->id);
			if(cur_button.get_image() != NULL && cur_button.get_image()->get_texture() == icon_cross) {
				// reposition the exit button so it fits into the windows title bar
				// get the rectangle and add the windows x and y coordinate
				memcpy(r1, cur_button.get_rectangle(), sizeof(gfx::rect));
				r1->x1 += wnd_iter.get_rectangle()->x2 - 16 - 3;
				r1->x2 += wnd_iter.get_rectangle()->x2 - 16 - 3;
				r1->y1 += wnd_iter.get_rectangle()->y1 + 2;
				r1->y2 += wnd_iter.get_rectangle()->y1 + 2;

				// set window button flag to true
				wnd_button = true;
			}
			else {
				// get the rectangle and add the windows x and y coordinate
				memcpy(r1, cur_button.get_rectangle(), sizeof(gfx::rect));
				r1->x1 += wp->x;
				r1->x2 += wp->x;
				r1->y1 += wp->y;
				r1->y2 += wp->y;
			}

			if((current_id == gui::wnd_layers.back() || !wnd_event) &&
				g->is_pnt_in_rectangle(r1, p) &&
				active_element->type != gui::COMBO) {
				if(cur_button.get_state() != "CLICKED") {
					changed = true;
					cur_button.set_redraw(true);
				}
				cur_button.set_state("CLICKED");
				if(wnd_button) {
					evt->set_last_pressed(0, 0);
					evt->set_pressed(0, 0);
				}
				set_active_element(ge_iter);
			}
			else {
				if(cur_button.get_state() == "CLICKED") {
					changed = true;
					cur_button.set_redraw(true);
					cur_button.set_state("NORMAL");
					evt->add_gui_event(evt->BUTTON_PRESSED, cur_button.get_id());
				}
			}

			if(cur_button.get_redraw()) changed = true;
		}
		break;
		case gui::TEXT:
			changed = get_object<gui_text>(ge_iter->id)->get_redraw();
			break;
		case gui::INPUT: {
			p->x = evt->get_lm_last_pressed_x();
			p->y = evt->get_lm_last_pressed_y();

			list<gui_input>::reference cur_input = *get_object<gui_input>(ge_iter->id);

			// get the rectangle and add the windows x and y coordinate
			memcpy(r1, cur_input.get_rectangle(), sizeof(gfx::rect));
			r1->x1 += wp->x;
			r1->x2 += wp->x;
			r1->y1 += wp->y;
			r1->y2 += wp->y;

			if(cur_input.get_redraw()) changed = true;

			if((current_id == gui::wnd_layers.back() || !wnd_event) &&
				g->is_pnt_in_rectangle(r1, p) &&
				active_element->type != gui::COMBO) {
				if(!cur_input.get_active()) {
					changed = true;
					cur_input.set_redraw(true);
				}
				cur_input.set_active(true);
				set_active_element(ge_iter);
				evt->set_active(event::GUI);

				gui::handle_input(cur_input);
			}
			else {
				if(cur_input.get_active()) {
					changed = true;
					cur_input.set_redraw(true);
				}
				cur_input.set_active(false);
			}

			cur_input.do_blink();
		}
		break;
		case gui::LIST: {
			p->x = evt->get_lm_pressed_x();
			p->y = evt->get_lm_pressed_y();

			list<gui_list>::reference cur_list = *get_object<gui_list>(ge_iter->id);

			// get the rectangle and add the windows x and y coordinate
			memcpy(r1, cur_list.get_rectangle(), sizeof(gfx::rect));
			r1->x1 += wp->x;
			// decrease the rects x2 coord by 15, because we don't want input from the vbar
			r1->x2 += wp->x - 20;
			r1->y1 += wp->y;
			r1->y2 += wp->y;

			if(cur_list.get_redraw()) changed = true;

			if((current_id == gui::wnd_layers.back() || !wnd_event) && g->is_pnt_in_rectangle(r1, p) &&
				active_element->type != gui::COMBO) {
				if(!cur_list.get_active()) {
					changed = true;
					cur_list.set_redraw(true);
				}
				cur_list.set_active(true);
				cur_list.select_pos(p->x - wp->x, p->y - wp->y);
				evt->set_last_pressed(0, 0);
				evt->set_pressed(0, 0);
				evt->add_gui_event(evt->LISTBOX_ITEM_SELECTED, cur_list.get_id());
			}
			else {
				if(cur_list.get_active()) {
					changed = true;
					cur_list.set_redraw(true);
				}
				cur_list.set_active(false);
			}

			if(cur_list.get_vbar_handler()->get_position() != cur_list.get_position()) {
				cur_list.set_position(cur_list.get_vbar_handler()->get_position());
			}
		}
		break;
		case gui::VBAR: {
			p->x = evt->get_lm_pressed_x();
			p->y = evt->get_lm_pressed_y();

			list<gui_vbar>::reference cur_vbar = *get_object<gui_vbar>(ge_iter->id);

			// get the rectangle and add the windows x and y coordinate
			memcpy(r1, cur_vbar.get_rectangle(), sizeof(gfx::rect));
			r1->x1 += wp->x;
			r1->x2 += wp->x;
			r1->y1 += wp->y;
			r1->y2 += wp->y;

			if(cur_vbar.get_redraw()) changed = true;

			if((current_id == gui::wnd_layers.back() || !wnd_event) &&
				cur_vbar.get_slider_active() &&
				active_element->type != gui::COMBO) {
				if(g->is_pnt_in_rectangle(r1, p)) {
					changed = true;
					cur_vbar.set_redraw(true);
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
					if(cur_vbar.get_active()) {
						changed = true;
						cur_vbar.set_redraw(true);
					}
					cur_vbar.set_active(false);
				}
			}
		}
		break;
		case gui::CHECK: {
			p->x = evt->get_lm_last_pressed_x();
			p->y = evt->get_lm_last_pressed_y();

			list<gui_check>::reference cur_check = *get_object<gui_check>(ge_iter->id);

			// get the rectangle and add the windows x and y coordinate
			// and reduce the "clickable" area
			memcpy(r1, cur_check.get_rectangle(), sizeof(gfx::rect));
			r1->x1 += wp->x;
			r1->x2 = r1->x1 + 12;
			r1->y1 += wp->y;
			r1->y2 = r1->y1 + 12;

			if(cur_check.get_redraw()) changed = true;

			if((current_id == gui::wnd_layers.back() || !wnd_event) && g->is_pnt_in_rectangle(r1, p) &&
				active_element->type != gui::COMBO) {
				changed = true;
				cur_check.set_redraw(true);
				cur_check.set_checked(cur_check.get_checked() ^ true);
				evt->add_gui_event(evt->CHECKBOX_CHECKED, cur_check.get_id());
				evt->set_last_pressed(0, 0);
			}
		}
		break;
		case gui::COMBO: {
			p->x = evt->get_lm_last_pressed_x();
			p->y = evt->get_lm_last_pressed_y();

			list<gui_combo>::reference cur_combo = *get_object<gui_combo>(ge_iter->id);

			if((current_id == gui::wnd_layers.back() || !wnd_event) &&
				cur_combo.is_list_visible()) {
				r1->x1 = wp->x + cur_combo.get_rectangle()->x1 + 1;
				r1->x2 = wp->x + cur_combo.get_rectangle()->x2 - 1;
				r1->y1 = wp->y + cur_combo.get_rectangle()->y1 + 21;
				r1->y2 = wp->y + cur_combo.get_rectangle()->y2 - 1;
				// check, if we clicked somewhere outside of the combo box list ...
				if(!g->is_pnt_in_rectangle(r1, p)) {
					memcpy(r2, cur_combo.get_list_button()->get_rectangle(), sizeof(gfx::rect));
					r2->x1 += wp->x;
					r2->x2 += wp->x;
					r2->y1 += wp->y;
					r2->y2 += wp->y;
					// ... if so, check if it was inside the buttons rectangle
					if(!g->is_pnt_in_rectangle(r2, p)) {
						// if so, "close" the list
						changed = true;
						//cur_combo.set_redraw(true);
						cur_combo.set_list_visible(false);
						wnd_iter.set_redraw(true);
						evt->set_last_pressed(0, 0);
						evt->set_pressed(0, 0);
						ae_reset = true;
					}
					else {
						// if not, select an item
						evt->get_mouse_pos(p);
						if(g->is_pnt_in_rectangle(r1, p)) {
							p->x -= wp->x;
							p->y -= wp->y;
							cur_combo.select_pos(p);
						}
						changed = true;
						cur_combo.set_redraw(true);
						set_active_element(ge_iter);
					}
				}
				else {
					// ... if not, close the list and set the new selected item
					changed = true;
					//cur_combo.set_redraw(true);
					wnd_iter.set_redraw(true);
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

			if(changed) handle_element(wp, get_element_iter(cur_combo.get_list_button()->get_id()), wnd_iter);
			else changed = handle_element(wp, get_element_iter(cur_combo.get_list_button()->get_id()), wnd_iter);

			if(!changed) changed = cur_combo.get_redraw();

			cur_combo.handle_combo();
		}
		break;
		case gui::MLTEXT: {
			if(get_object<gui_mltext>(ge_iter->id)->get_redraw()) changed = true;
		}
		break;
		case gui::IMAGE: {
			if(get_object<gui_image>(ge_iter->id)->get_redraw()) changed = true;
		}
		break;
		case gui::TAB: {
			p->x = evt->get_lm_pressed_x();
			p->y = evt->get_lm_pressed_y();

			gui_tab* cur_tab = get_object<gui_tab>(ge_iter->id);

			// get the rectangle and add the windows x and y coordinate
			memcpy(r1, cur_tab->get_rectangle(), sizeof(gfx::rect));
			r1->x1 += wp->x;
			r1->x2 += wp->x;
			r1->y1 += wp->y;
			r1->y2 = r1->y1 + 20; // we are just interessted in the input from 0 to 20px on the y axis

			if((current_id == gui::wnd_layers.back() || !wnd_event) &&
				g->is_pnt_in_rectangle(r1, p) &&
				active_element->type != gui::COMBO) {
				p->x -= wp->x;
				p->y -= wp->y;
				unsigned int active_tab = cur_tab->get_active_tab();
				cur_tab->select_pos(p->x, p->y);
				evt->set_pressed(0, 0);
				if(active_tab != cur_tab->get_active_tab()) {
					evt->add_gui_event(event::TAB_SELECTED, ge_iter->id);
				}
			}

			if(cur_tab->get_redraw()) changed = true;
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
			if(exist(ge_iter->id)) {
				gui::msg_ok_wnd* mwnd = get_msgbox(ge_iter->id+1);
				if(mwnd->ok->get_state() == "CLICKED") {
					mwnd->wnd->set_deleted(true);
					evt->set_last_pressed(0, 0);
					evt->set_pressed(0, 0);
				}
			}
		}
		break;
		default:
			break;
	}
	return changed;
}

/*! adds a gui button element and returns it
 *  @param rectangle the buttons rectangle
 *  @param id the buttons (a2e event) id
 *  @param text the buttons text
 *  @param image_texture the image texture we want to set/use
 *  @param wid the id of the window we want the element to be in (0 = no window)
 */
GUI_OBJ gui::add_button(gfx::rect* rectangle, unsigned int id, char* text, GLuint image_texture, unsigned int wid, unsigned int tid) {
	gui::gui_elements.push_back(*new gui_element());
	gui::gui_elements.back().id = id;
	gui::gui_elements.back().type = gui::BUTTON;
	gui::gui_elements.back().wid = wid;
	gui::gui_elements.back().tid = tid;
	gui::gui_elements.back().is_drawn = true;

	// create class
	gui::gui_buttons.push_back(*new gui_button*());
	gui::gui_buttons.back() = new gui_button(e, gs);

	gui::gui_buttons.back()->set_text_handler(get_object<gui_text>(add_text("STANDARD", 12, text,
		gs->get_color("FONT2"), g->cord_to_pnt(0,0), get_free_id(), 0xFFFFFFFF)));
	// don't draw our text automatically
	gui::gui_elements.back().is_drawn = false;

	// set/create the image
	if(image_texture != 0) {
		gui::gui_buttons.back()->set_image(new image(e));
		gui::gui_buttons.back()->get_image()->set_texture(image_texture);
		gui::gui_buttons.back()->get_image()->set_gui_img(true);
		if(image_texture == icon_cross) {
			gui::gui_buttons.back()->get_image()->set_color(gs->get_color("ICON1"));
		}
		else if(image_texture == icon_arrow_up || image_texture == icon_arrow_down) {
			gui::gui_buttons.back()->get_image()->set_color(gs->get_color("ICON2"));
		}
	}

	gui::gui_buttons.back()->set_id(id);
	gui::gui_buttons.back()->set_rectangle(rectangle);
	gui::gui_buttons.back()->set_text(text);

	// redraw window
	if(wid != 0xFFFFFFFF) get_object<gui_window>(wid)->set_redraw(true);

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
				   unsigned int color, core::pnt* point, unsigned int id, unsigned int wid, unsigned int tid) {
	gui::gui_elements.push_back(*new gui_element());
	gui::gui_elements.back().id = id;
	gui::gui_elements.back().type = gui::TEXT;
	gui::gui_elements.back().wid = wid;
	gui::gui_elements.back().tid = tid;
	gui::gui_elements.back().is_drawn = true;

	// create class
	gui::gui_texts.push_back(*new gui_text*());
	gui::gui_texts.back() = new gui_text(e);

	gui::gui_texts.back()->set_init(false);
	gui::gui_texts.back()->set_id(id);

	// font stuff
	if(strcmp(font_name, "STANDARD") == 0) {
		gui::gui_texts.back()->set_font(gf->add_font(e->data_path("vera.ttf"), font_size));
		gui::gui_texts.back()->set_color(color);
	}
	else {
		gui::gui_texts.back()->set_font(gf->add_font(font_name, font_size));
		gui::gui_texts.back()->set_color(color);
	}
	gui::gui_texts.back()->set_point(point);
	gui::gui_texts.back()->set_text(text);
	gui::gui_texts.back()->set_init(true);
	if(wid != 0xFFFFFFFF && get_object<gui_window>(wid)->get_bg()) gui::gui_texts.back()->set_background_color(gs->get_color("WINDOW_BG1"));

	// redraw window
	if(wid != 0xFFFFFFFF) get_object<gui_window>(wid)->set_redraw(true);

	//return &gui::gui_texts.back();
	return id;
}

/*! adds a gui input box element and returns it
 *  @param rectangle the input boxes rectangle
 *  @param id the input boxes (a2e event) id
 *  @param text the input boxes text
 *  @param wid the id of the window we want the element to be in (0 = no window)
 */
GUI_OBJ gui::add_input_box(gfx::rect* rectangle, unsigned int id, char* text, unsigned int wid, unsigned int tid) {
	gui::gui_elements.push_back(*new gui_element());
	gui::gui_elements.back().id = id;
	gui::gui_elements.back().type = gui::INPUT;
	gui::gui_elements.back().wid = wid;
	gui::gui_elements.back().tid = tid;
	gui::gui_elements.back().is_drawn = true;

	// create class
	gui::gui_input_boxes.push_back(*new gui_input*());
	gui::gui_input_boxes.back() = new gui_input(e, gs);

	gui::gui_input_boxes.back()->set_text_handler(get_object<gui_text>(add_text("STANDARD", 12, text,
		gs->get_color("FONT"), g->cord_to_pnt(0,0), get_free_id(), 0xFFFFFFFF)));
	// don't draw our text automatically
	// celements-1, because our text element, is the last initialized element
	gui::gui_elements.back().is_drawn = false;

	gui::gui_input_boxes.back()->set_blink_text_handler(get_object<gui_text>(add_text("STANDARD", 12, " ",
		gs->get_color("FONT"), g->cord_to_pnt(0,0), get_free_id(), 0xFFFFFFFF)));
	// don't draw our text automatically
	// celements-1, because our text element, is the last initialized element
	gui::gui_elements.back().is_drawn = false;

	gui::gui_input_boxes.back()->set_id(id);
	gui::gui_input_boxes.back()->set_rectangle(rectangle);
	gui::gui_input_boxes.back()->set_text(text);
	gui::gui_input_boxes.back()->set_text_position((unsigned int)strlen(text));

	// redraw window
	if(wid != 0xFFFFFFFF) get_object<gui_window>(wid)->set_redraw(true);

	//return &gui::gui_input_boxes.back();
	return id;
}

/*! adds a gui list box element and returns it
 *  @param rectangle the list boxes rectangle
 *  @param id the list boxes (a2e event) id
 *  @param wid the id of the window we want the element to be in (0 = no window)
 */
GUI_OBJ gui::add_list_box(gfx::rect* rectangle, unsigned int id, unsigned int wid, unsigned int tid) {
	gui::gui_elements.push_back(*new gui_element());
	gui::gui_elements.back().id = id;
	gui::gui_elements.back().type = gui::LIST;
	gui::gui_elements.back().wid = wid;
	gui::gui_elements.back().tid = tid;
	gui::gui_elements.back().is_drawn = true;

	// create class
	gui::gui_list_boxes.push_back(*new gui_list*());
	gui::gui_list_boxes.back() = new gui_list(e, gs);

	// add text
	gui::gui_list_boxes.back()->set_text_handler(get_object<gui_text>(add_text("STANDARD", 12, "",
		gs->get_color("FONT"), g->cord_to_pnt(0,0), get_free_id(), 0xFFFFFFFF)));
	// don't draw our text automatically
	gui::gui_elements.back().is_drawn = false;

	// add vertical bar
	gui::gui_list_boxes.back()->set_vbar_handler(get_object<gui_vbar>(add_vbar(g->pnt_to_rect(rectangle->x2-18,
		rectangle->y1, rectangle->x2, rectangle->y2), get_free_id(), wid)));

	gui::gui_list_boxes.back()->set_id(id);
	gui::gui_list_boxes.back()->set_rectangle(rectangle);
	gui::gui_list_boxes.back()->set_position(0);

	// redraw window
	if(wid != 0xFFFFFFFF) get_object<gui_window>(wid)->set_redraw(true);

	//return &gui::gui_list_boxes.back();
	return id;
}

/*! adds a gui vertical bar element and returns it
 *  @param rectangle the vbars rectangle
 *  @param id the vertical bar (a2e event) id
 *  @param wid the id of the window we want the element to be in (0 = no window)
 */
GUI_OBJ gui::add_vbar(gfx::rect* rectangle, unsigned int id, unsigned int wid, unsigned int tid) {
	gui::gui_elements.push_back(*new gui_element());
	gui::gui_elements.back().id = id;
	gui::gui_elements.back().type = gui::VBAR;
	gui::gui_elements.back().wid = wid;
	gui::gui_elements.back().tid = tid;
	gui::gui_elements.back().is_drawn = true;

	// create class
	gui::gui_vbars.push_back(*new gui_vbar*());
	gui::gui_vbars.back() = new gui_vbar(e, gs);

	// add up button
	gui::gui_vbars.back()->set_up_button_handler(get_object<gui_button>(add_button(g->pnt_to_rect(0,0,1,1),
		get_free_id(), "", icon_arrow_up, wid)));
	gui::gui_vbars.back()->get_up_button_handler()->set_image_scaling(false);
	gui::gui_vbars.back()->get_up_button_handler()->set_type("vbar_up_button");

	// add down button
	gui::gui_vbars.back()->set_down_button_handler(get_object<gui_button>(add_button(g->pnt_to_rect(0,0,1,1),
		get_free_id(), "", icon_arrow_down, wid)));
	gui::gui_vbars.back()->get_down_button_handler()->set_image_scaling(false);
	gui::gui_vbars.back()->get_down_button_handler()->set_type("vbar_down_button");

	gui::gui_vbars.back()->set_id(id);
	gui::gui_vbars.back()->set_rectangle(rectangle);

	// redraw window
	if(wid != 0xFFFFFFFF) get_object<gui_window>(wid)->set_redraw(true);

	//return &gui::gui_vbars.back();
	return id;
}

/*! adds a gui check box element and returns it
 *  @param rectangle the check boxes rectangle
 *  @param id the check boxes (a2e event) id
 *  @param text the check boxes text
 *  @param wid the id of the window we want the element to be in (0 = no window)
 */
GUI_OBJ gui::add_check_box(gfx::rect* rectangle, unsigned int id, char* text, unsigned int wid, unsigned int tid) {
	gui::gui_elements.push_back(*new gui_element());
	gui::gui_elements.back().id = id;
	gui::gui_elements.back().type = gui::CHECK;
	gui::gui_elements.back().wid = wid;
	gui::gui_elements.back().tid = tid;
	gui::gui_elements.back().is_drawn = true;

	// create class
	gui::gui_check_boxes.push_back(*new gui_check*());
	gui::gui_check_boxes.back() = new gui_check(e, gs);

	gui::gui_check_boxes.back()->set_text_handler(get_object<gui_text>(add_text("STANDARD", 12, text,
		gs->get_color("FONT2"), g->cord_to_pnt(0,0), get_free_id(), 0xFFFFFFFF)));
	// don't draw our text automatically
	// celements-1, because our text element, is the last initialized element
	gui::gui_elements.back().is_drawn = false;

	gui::gui_check_boxes.back()->set_image(new image(e));
	gui::gui_check_boxes.back()->get_image()->set_texture(icon_checked);
	gui::gui_check_boxes.back()->get_image()->set_color(gs->get_color("ICON3"));
	gui::gui_check_boxes.back()->get_image()->set_scaling(false);
	gui::gui_check_boxes.back()->get_image()->set_gui_img(true);

	gui::gui_check_boxes.back()->set_id(id);
	gui::gui_check_boxes.back()->set_rectangle(rectangle);
	gui::gui_check_boxes.back()->set_text(text);

	// redraw window
	if(wid != 0xFFFFFFFF) get_object<gui_window>(wid)->set_redraw(true);

	//return &gui::gui_check_boxes.back();
	return id;
}

/*! adds a gui combo box element and returns it
 *  @param rectangle the combo boxes rectangle
 *  @param id the combo boxes (a2e event) id
 *  @param wid the id of the window we want the element to be in (0 = no window)
 */
GUI_OBJ gui::add_combo_box(gfx::rect* rectangle, unsigned int id, unsigned int wid, unsigned int tid) {
	gui::gui_elements.push_back(*new gui_element());
	gui::gui_elements.back().id = id;
	gui::gui_elements.back().type = gui::COMBO;
	gui::gui_elements.back().wid = wid;
	gui::gui_elements.back().tid = tid;
	gui::gui_elements.back().is_drawn = true;

	// create class
	gui::gui_combo_boxes.push_back(*new gui_combo*());
	gui::gui_combo_boxes.back() = new gui_combo(e, gs);

	gui::gui_combo_boxes.back()->set_text_handler(get_object<gui_text>(add_text("STANDARD", 12, "",
		gs->get_color("FONT"), g->cord_to_pnt(0,0), get_free_id(), 0xFFFFFFFF)));
	gui::gui_elements.back().is_drawn = false; // text shouldn't be drawn automatically
	gui::gui_combo_boxes.back()->set_list_button(get_object<gui_button>(add_button(g->pnt_to_rect(0,0,1,1),
		get_free_id(), "", icon_arrow_down, 0xFFFFFFFF)));
	gui::gui_combo_boxes.back()->get_list_button()->set_image_scaling(false);
	gui::gui_elements.back().is_drawn = false; // button shouldn't be drawn automatically

	gui::gui_combo_boxes.back()->set_id(id);
	gui::gui_combo_boxes.back()->set_rectangle(rectangle);

	// redraw window
	if(wid != 0xFFFFFFFF) get_object<gui_window>(wid)->set_redraw(true);

	return id;
}

/*! adds a gui mltext element and returns it
 *  @param rectangle the mltexts rectangle
 *  @param id the mltexts (a2e event) id
 *  @param text the buttons text
 *  @param wid the id of the window we want the element to be in (0 = no window)
 */
GUI_OBJ gui::add_mltext(gfx::rect* rectangle, unsigned int id, char* text, unsigned int wid, unsigned int tid) {
	gui::gui_elements.push_back(*new gui_element());
	gui::gui_elements.back().id = id;
	gui::gui_elements.back().type = gui::MLTEXT;
	gui::gui_elements.back().wid = wid;
	gui::gui_elements.back().tid = tid;
	gui::gui_elements.back().is_drawn = true;

	// create class
	gui::gui_mltexts.push_back(*new gui_mltext*());
	gui::gui_mltexts.back() = new gui_mltext(e, gs);

	gui::gui_mltexts.back()->set_text_handler(get_object<gui_text>(add_text("STANDARD", 12, text,
		gs->get_color("FONT"), g->cord_to_pnt(0,0), get_free_id(), 0xFFFFFFFF)));
	// don't draw our text automatically
	gui::gui_elements.back().is_drawn = false;

	gui::gui_mltexts.back()->set_id(id);
	gui::gui_mltexts.back()->set_rectangle(rectangle);
	gui::gui_mltexts.back()->set_text(text);

	// redraw window
	if(wid != 0xFFFFFFFF) get_object<gui_window>(wid)->set_redraw(true);

	return id;
}

/*! adds a gui image element and returns it
 *  @param rectangle the images rectangle
 *  @param id the images (a2e event) id
 *  @param image_filename the images filename we want to use/load
 *  @param image_obj the image object we want to set/use
 *  @param wid the id of the window we want the element to be in (0 = no window)
 */
GUI_OBJ gui::add_image(gfx::rect* rectangle, unsigned int id, const char* image_filename, image* image_obj, unsigned int wid, unsigned int tid) {
	gui::gui_elements.push_back(*new gui_element());
	gui::gui_elements.back().id = id;
	gui::gui_elements.back().type = gui::IMAGE;
	gui::gui_elements.back().wid = wid;
	gui::gui_elements.back().tid = tid;
	gui::gui_elements.back().is_drawn = true;

	// create class
	gui::gui_images.push_back(*new gui_image*());
	gui::gui_images.back() = new gui_image(e, gs);

	// set/create the image
	if(image_obj != NULL) {
		gui::gui_images.back()->set_image(image_obj);
	}
	else {
		gui::gui_images.back()->set_image(new image(e));
		if(image_filename != NULL) {
			gui::gui_images.back()->get_image()->open_image(e->data_path(image_filename));
		}
	}

	gui::gui_images.back()->set_id(id);
	gui::gui_images.back()->set_rectangle(rectangle);

	// redraw window
	if(wid != 0xFFFFFFFF) get_object<gui_window>(wid)->set_redraw(true);

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
	gui::gui_windows.push_back(*new gui_window*());
	gui::gui_windows.back() = new gui_window(e, gs);

	gui::gui_windows.back()->set_id(id);
	gui::gui_windows.back()->set_rectangle(rectangle);
	gui::gui_windows.back()->set_caption(caption);
	gui::gui_windows.back()->set_lid((unsigned int)gui_windows.size());
	gui::gui_windows.back()->set_border(border);
	gui::gui_windows.back()->set_bg(bg);
	gui::gui_windows.back()->set_redraw(true);

	// add exit button if we got a border
	if(border) {
		gui::gui_windows.back()->set_exit_button_handler(get_object<gui_button>(add_button(g->pnt_to_rect(0,0,16,16),
			get_free_id(), "", icon_cross, id)));
		gui::gui_windows.back()->get_exit_button_handler()->set_image_scaling(false);
		gui::gui_windows.back()->get_exit_button_handler()->set_type("window_button");
	}

	// add window caption
	gui::gui_windows.back()->set_text_handler(get_object<gui_text>(add_text("STANDARD", 12, caption,
		gs->get_color("FONT2"), g->cord_to_pnt(0,0), get_free_id(), id)));
	// don't draw our text automatically
	// celements-1, because our text element, is the last initialized element
	gui::gui_elements.back().is_drawn = false;

	// add the windows to the layer stack/list
	// it will automatically become active / the one in the front
	wnd_layers.push_back(id);

	evt->set_pressed(rectangle->x1, rectangle->y1);

	// add rtt buffer
	window_buffers.push_back(*new gui::window_buffer*());
	window_buffers.back() = new gui::window_buffer();
	// seems like we need a fbo/texture with a pot size (at least for some graphic cards ...)
	unsigned int width = c->next_pot(rectangle->x2 - rectangle->x1);
	unsigned int height = c->next_pot(rectangle->y2 - rectangle->y1);
	window_buffers.back()->buffer = r->add_buffer(width, height);
	window_buffers.back()->wid = id;
	// also add a shadow buffer if we want window shadows
	if(shadow_type != 0 && border && bg) {
		width = c->next_pot((rectangle->x2 - rectangle->x1)+30);
		height = c->next_pot((rectangle->y2 - rectangle->y1)+30);
		// add +30 px width and +25 px height to the buffers size
		window_buffers.back()->shadow = r->add_buffer(width, height);
		rtt::fbo* tmp_buffer = r->add_buffer(width, height);

		// render the shadow
		r->start_draw(window_buffers.back()->shadow);
		r->clear();
		r->start_2d_draw();
		/*r1->x1 = 0;
		r1->y1 = 0;
		r1->x2 = width;
		r1->y2 = height;
		g->draw_filled_rectangle(r1, 0x00FFFFFF);*/
		r1->x1 = 0;
		r1->y1 = 0;
		r1->x2 = (rectangle->x2 - rectangle->x1);
		r1->y2 = (rectangle->y2 - rectangle->y1);
		g->draw_filled_rectangle(r1, 0x00000000);
		r->stop_2d_draw();
		r->stop_draw();

		// calc blur tex coordinates
		unsigned int q = 5;
		float xInc = 1.0f / (float)width;
		float yInc = 1.0f / (float)height;
		for(unsigned int i = 0; i < q; i++) {
			for(unsigned int j = 0; j < q; j++) {
				tcs[(((i*q)+j)*2)+0] = (-1.0f * xInc) + ((float)i * xInc);
				tcs[(((i*q)+j)*2)+1] = (-1.0f * yInc) + ((float)j * yInc);
			}
		}

		// blur (32 or 64 passes) with a 5x5 kernel
		unsigned int passes = 0;
		if(shadow_type == 1) passes = 32;
		else if(shadow_type == 2) passes = 64;

		for(unsigned int i = 0; i < passes; i++) {
			// copy the buffer
			r->copy_buffer(window_buffers.back()->shadow, tmp_buffer);

			// blur effect
			r->start_draw(window_buffers.back()->shadow);
			r->start_2d_draw();
			s->use_shader(shader::BLUR5X5);

			s->set_uniform2fv(0, tcs, 25);
			s->set_uniform1i(1, 0);

			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, tmp_buffer->tex_id);

			glBegin(GL_QUADS);
				s->set_attribute2f(0, 0.0f, 0.0f);
				glVertex2f(-1.0f, -1.0f);

				s->set_attribute2f(0, 1.0f, 0.0f);
				glVertex2f(1.0f, -1.0f);

				s->set_attribute2f(0, 1.0f, 1.0f);
				glVertex2f(1.0f, 1.0f);

				s->set_attribute2f(0, 0.0f, 1.0f);
				glVertex2f(-1.0f, 1.0f);
			glEnd();

			glDisable(GL_TEXTURE_2D);

			s->use_shader(shader::NONE);
			r->stop_2d_draw();
			r->stop_draw();
		}

		// copy the buffer
		r->copy_buffer(window_buffers.back()->shadow, tmp_buffer);

		// move the texture
		r->start_draw(window_buffers.back()->shadow);
		r->start_2d_draw();

		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, tmp_buffer->tex_id);

		float mvx = -((float)passes+xadd)*xInc;
		float mvy = -((float)passes+yadd)*yInc;

		glBegin(GL_QUADS);
			glTexCoord2f(mvx, mvy);
			glVertex2i(0, 0);

			glTexCoord2f(1.0f+mvx, mvy);
			glVertex2i(width, 0);

			glTexCoord2f(1.0f+mvx, 1.0f+mvy);
			glVertex2i(width, height);

			glTexCoord2f(mvx, 1.0f+mvy);
			glVertex2i(0, height);
		glEnd();

		glDisable(GL_TEXTURE_2D);

		r->stop_2d_draw();
		r->stop_draw();

		// cut out the window
		r->start_draw(window_buffers.back()->shadow);
		r->start_2d_draw();

		glPushMatrix();
		glTranslatef((float)xcorrect, (float)ycorrect, 0.0f);
		string wnd_state = string(gui::gui_windows.back()->get_state());
		gui::gui_windows.back()->set_state("SHAPE");
		gui::gui_windows.back()->set_scissor(false);
		gui::gui_windows.back()->draw();
		gui::gui_windows.back()->set_scissor(true);
		gui::gui_windows.back()->set_state(wnd_state.c_str());
		glPopMatrix();

		r->stop_2d_draw();
		r->stop_draw();

		// delete tmp buffer
		r->delete_buffer(tmp_buffer);
	}
	else {
		window_buffers.back()->shadow = NULL;
	}

	//return &gui::gui_windows.back();
	return id;
}

/*! adds a gui tab element and returns it
 *  @param rectangle the tabs rectangle
 *  @param id the tabs (a2e event) id
 *  @param wid the id of the window we want the element to be in (0 = no window)
 */
GUI_OBJ gui::add_tab(gfx::rect* rectangle, unsigned int id, unsigned int wid) {
	gui::gui_elements.push_back(*new gui_element());
	gui::gui_elements.back().id = id;
	gui::gui_elements.back().type = gui::TAB;
	gui::gui_elements.back().wid = wid;
	gui::gui_elements.back().tid = 0;
	gui::gui_elements.back().is_drawn = true;

	// create class
	gui::gui_tabs.push_back(*new gui_tab*());
	gui::gui_tabs.back() = new gui_tab(e, gs);

	gui::gui_tabs.back()->set_text_handler(get_object<gui_text>(add_text("STANDARD", 12, "",
		gs->get_color("FONT"), g->cord_to_pnt(0,0), get_free_id(), 0xFFFFFFFF)));
	gui::gui_elements.back().is_drawn = false; // text shouldn't be drawn automatically

	gui::gui_tabs.back()->set_id(id);
	gui::gui_tabs.back()->set_rectangle(rectangle);
	gui::gui_tabs.back()->set_redraw(true);

	// redraw window
	if(wid != 0xFFFFFFFF) get_object<gui_window>(wid)->set_redraw(true);

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
	static stringstream* buffer = evt->get_buffer();
	static char x;

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
					gui_button* del_button = get_object<gui_button>(id);
					if(&del_button == NULL) {
						m->print(msg::MERROR, "gui.cpp", "delete_element(): can not delete element (id: %u)", id);
						return false;
					}
					if(del_button->get_text_handler() != NULL) {
						delete_element(del_button->get_text_handler()->get_id());
					}
					gui_buttons.remove(del_button);
					gui_elements.erase(elem_iter);
					return true;
				}
				break;
				case gui::CHECK: {
					gui_check* del_check = get_object<gui_check>(id);
					if(&del_check == NULL) {
						m->print(msg::MERROR, "gui.cpp", "delete_element(): can not delete element (id: %u)", id);
						return false;
					}
					if(del_check->get_text_handler() != NULL) {
						delete_element(del_check->get_text_handler()->get_id());
					}
					gui_check_boxes.remove(del_check);
					gui_elements.erase(elem_iter);
					return true;
				}
				break;
				case gui::COMBO: {
					gui_combo* del_combo = get_object<gui_combo>(id);
					if(&del_combo == NULL) {
						m->print(msg::MERROR, "gui.cpp", "delete_element(): can not delete element (id: %u)", id);
						return false;
					}
					if(del_combo->get_text_handler() != NULL) {
						delete_element(del_combo->get_text_handler()->get_id());
					}
					delete_element(del_combo->get_list_button()->get_id());
					gui_combo_boxes.remove(del_combo);
					gui_elements.erase(elem_iter);
					return true;
				}
				break;
				case gui::INPUT: {
					gui_input* del_input = get_object<gui_input>(id);
					if(&del_input == NULL) {
						m->print(msg::MERROR, "gui.cpp", "delete_element(): can not delete element (id: %u)", id);
						return false;
					}
					if(del_input->get_text_handler() != NULL) {
						delete_element(del_input->get_text_handler()->get_id());
					}
					delete_element(del_input->get_blink_text_handler()->get_id());
					gui_input_boxes.remove(del_input);
					gui_elements.erase(elem_iter);
					return true;
				}
				break;
				case gui::LIST: {
					gui_list* del_list = get_object<gui_list>(id);
					if(&del_list == NULL) {
						m->print(msg::MERROR, "gui.cpp", "delete_element(): can not delete element (id: %u)", id);
						return false;
					}
					if(del_list->get_text_handler() != NULL) {
						delete_element(del_list->get_text_handler()->get_id());
					}
					delete_element(del_list->get_vbar_handler()->get_id());
					gui_list_boxes.remove(del_list);
					gui_elements.erase(elem_iter);
					return true;
				}
				break;
				case gui::TEXT: {
					gui_text* del_text = get_object<gui_text>(id);
					if(&del_text== NULL) {
						m->print(msg::MERROR, "gui.cpp", "delete_element(): can not delete element (id: %u)", id);
						return false;
					}
					gui_texts.remove(del_text);
					gui_elements.erase(elem_iter);
					return true;
				}
				break;
				case gui::VBAR: {
					gui_vbar* del_vbar = get_object<gui_vbar>(id);
					if(&del_vbar == NULL) {
						m->print(msg::MERROR, "gui.cpp", "delete_element(): can not delete element (id: %u)", id);
						return false;
					}
					if(del_vbar->get_text_handler() != NULL) {
						delete_element(del_vbar->get_text_handler()->get_id());
					}
					delete_element(del_vbar->get_down_button_handler()->get_id());
					delete_element(del_vbar->get_up_button_handler()->get_id());
					gui_vbars.remove(del_vbar);
					gui_elements.erase(elem_iter);
					return true;
				}
				break;
				case gui::WINDOW: {
					gui_window* del_window = get_object<gui_window>(id);
					if(&del_window == NULL) {
						m->print(msg::MERROR, "gui.cpp", "delete_element(): can not delete element (id: %u)", id);
						return false;
					}
					if(del_window->get_text_handler() != NULL) {
						delete_element(del_window->get_text_handler()->get_id());
					}
					gui_windows.remove(del_window);
					gui_elements.erase(elem_iter);

					// delete rtt buffer
					window_buffer* del_buffer = NULL;
					for(list<gui::window_buffer*>::iterator iter = window_buffers.begin(); iter != window_buffers.end(); iter++) {
						if((*iter)->wid == id) {
							r->delete_buffer((*iter)->buffer);
							del_buffer = *iter;
						}
					}
					delete del_buffer;
					window_buffers.remove(del_buffer);
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
					gui::msg_ok_wnd* del_msg = get_msgbox(id+1);
					if(&del_msg== NULL) {
						m->print(msg::MERROR, "gui.cpp", "delete_element(): can not delete element (id: %u)", id);
						return false;
					}
					delete_element(del_msg->text->get_id());
					delete_element(del_msg->ok->get_id());
					msg_boxes.remove(del_msg);
					gui_elements.erase(elem_iter);
					return true;
				}
				break;
				case gui::MLTEXT: {
					gui_mltext* del_mltext = get_object<gui_mltext>(id);
					if(&del_mltext == NULL) {
						m->print(msg::MERROR, "gui.cpp", "delete_element(): can not delete element (id: %u)", id);
						return false;
					}
					if(del_mltext->get_text_handler() != NULL) {
						delete_element(del_mltext->get_text_handler()->get_id());
					}
					gui_mltexts.remove(del_mltext);
					gui_elements.erase(elem_iter);
					return true;
				}
				break;
				case gui::IMAGE: {
					gui_image* del_image = get_object<gui_image>(id);
					if(&del_image == NULL) {
						m->print(msg::MERROR, "gui.cpp", "delete_element(): can not delete element (id: %u)", id);
						return false;
					}
					gui_images.remove(del_image);
					gui_elements.erase(elem_iter);
					return true;
				}
				break;
				case gui::TAB: {
					gui_tab* del_tab = get_object<gui_tab>(id);
					if(&del_tab == NULL) {
						m->print(msg::MERROR, "gui.cpp", "delete_element(): can not delete element (id: %u)", id);
						return false;
					}
					gui_tabs.remove(del_tab);
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

list<gui::gui_element>::iterator gui::get_element_iter(unsigned int id) {
	for(list<gui_element>::iterator iter = gui_elements.begin(); iter != gui_elements.end(); iter++) {
		if(id == iter->id) {
			return iter;
		}
	}
	m->print(msg::MERROR, "gui.cpp", "get_element_iter(): no gui element with such an id (%u) exists!", id);
	return gui_elements.end();
}

gui::msg_ok_wnd* gui::get_msgbox(unsigned int id) {
	for(list<gui::msg_ok_wnd*>::iterator iter = msg_boxes.begin(); iter != msg_boxes.end(); iter++) {
		if(id == (*iter)->id) {
			return *iter;
		}
	}
	m->print(msg::MERROR, "gui.cpp", "get_msgbox(): no msg box with such an id (%u) exists!", id);
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
			gui::set_visibility(gui::get_object<gui_list>(id)->get_text_handler()->get_id(), state);
			gui::set_visibility(gui::get_object<gui_list>(id)->get_vbar_handler()->get_id(), state, true);
			break;
		case gui::VBAR:
			gui::set_visibility(gui::get_object<gui_vbar>(id)->get_down_button_handler()->get_id(), state, true);
			gui::set_visibility(gui::get_object<gui_vbar>(id)->get_up_button_handler()->get_id(), state, true);
			break;
		case gui::COMBO:
			gui::set_visibility(gui::get_object<gui_combo>(id)->get_list_button()->get_id(), state, true);
			break;
		case gui::WINDOW: {
			gui_window* wnd = gui::get_object<gui_window>(id);
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
		case gui::TAB:
			// TODO!
			break;
		case gui::MSGBOX_OK: {
			gui::set_visibility(gui::get_msgbox(id)->wnd->get_id(), state);
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
GUI_OBJ gui::add_open_dialog(unsigned int id, char* caption, char* dir, char* ext, unsigned int x, unsigned int y, gui::file_filter* callback) {
	if(exist(ofd_wnd_id)) {
		if(!ofd_wnd->get_deleted()) return 0;
	}

	bool cb = (callback == NULL) ? false : true;

	string path = dir;
	gui::gui_elements.push_back(*new gui_element());
	gui::gui_elements.back().id = id;
	gui::gui_elements.back().type = gui::OPENDIALOG;
	gui::gui_elements.back().wid = 0;
	gui::gui_elements.back().is_drawn = true;

	ofd_wnd_id = add_window(e->get_gfx()->pnt_to_rect(x, y, 370+x, 221+y), id+1, caption, true, true);
	ofd_wnd = get_object<gui_window>(ofd_wnd_id);
	ofd_open = get_object<gui_button>(add_button(e->get_gfx()->pnt_to_rect(302, 0, 365, 20), id+2, "Open", 0, id+1));
	ofd_cancel = get_object<gui_button>(add_button(e->get_gfx()->pnt_to_rect(302, 20, 365, 40), 1, "Cancel", 0, id+1));
	ofd_dirlist = get_object<gui_list>(add_list_box(e->get_gfx()->pnt_to_rect(0, 0, 300, 198), id+3, id+1));

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
				// if a callback method is defined ...
				if(cb) {
					// ... call the callback and only add the file to the list if it returns true
					if(callback(c_file.name)) {
						ofd_dirlist->add_item(c_file.name, i);
						i++;
					}
				}
				else {
					ofd_dirlist->add_item(c_file.name, i);
					i++;
				}
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
	for(int j = 1; j < n; j++) {
		unsigned int fnlen = (unsigned int)strlen(namelist[j]->d_name);
		if(namelist[j]->d_type != DT_DIR &&
			namelist[j]->d_name[fnlen-3] == ext[strlen(ext)-3] &&
			namelist[j]->d_name[fnlen-2] == ext[strlen(ext)-2] &&
			namelist[j]->d_name[fnlen-1] == ext[strlen(ext)-1]) {
			// if a callback method is defined ...
			if(cb) {
				// ... call the callback and only add the file to the list if it returns true
				if(callback(namelist[j]->d_name)) {
					ofd_dirlist->add_item(namelist[j]->d_name, i);
					i++;
				}
			}
			else {
				ofd_dirlist->add_item(namelist[j]->d_name, i);
				i++;
			}
		}
	}
	delete [] namelist;
#endif

	return ofd_wnd_id;
}

gui_list* gui::get_open_diaolg_list() {
	return ofd_dirlist;
}

GUI_OBJ gui::add_msgbox_ok(char* caption, char* text) {
	unsigned int wnd_id = get_free_id();
	gui::gui_elements.push_back(*new gui_element());
	gui::gui_elements.back().id = wnd_id;
	gui::gui_elements.back().type = gui::MSGBOX_OK;
	gui::gui_elements.back().wid = wnd_id+1;
	gui::gui_elements.back().is_drawn = true;

	unsigned int sw = (unsigned int)((float)e->get_screen()->w * 0.2f);
	unsigned int sh = (unsigned int)((float)e->get_screen()->h * 0.4f);
	unsigned int w = (unsigned int)((float)e->get_screen()->w * 0.6f);
	unsigned int h = (unsigned int)((float)e->get_screen()->h * 0.2f);

	msg_boxes.push_back(*new gui::msg_ok_wnd*());
	msg_boxes.back() = new gui::msg_ok_wnd();
	msg_boxes.back()->id = add_window(e->get_gfx()->pnt_to_rect(sw, sh, sw + w, sh + h), wnd_id+1, caption, true, true);
	msg_boxes.back()->wnd = get_object<gui_window>(msg_boxes.back()->id);
	msg_boxes.back()->ok = get_object<gui_button>(add_button(e->get_gfx()->pnt_to_rect((unsigned int)((float)w * 0.5f) - 32,
		h - 50, (unsigned int)((float)w * 0.5f) + 32, h - 30), get_free_id(), "OK", 0, wnd_id+1));
	msg_boxes.back()->text = get_object<gui_text>(add_text("STANDARD", 12, text, gs->get_color("FONT"),
		e->get_gfx()->cord_to_pnt(0, 0), get_free_id(), wnd_id+1));
	unsigned int tw = (unsigned int)((float)(w - msg_boxes.back()->text->get_text_width()) / 2.0f);
	unsigned int th = (unsigned int)((float)h / 2.0f) - 30;
	msg_boxes.back()->text->get_point()->x = tw;
	msg_boxes.back()->text->get_point()->y = th;

	return msg_boxes.back()->id;
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
			tex = get_object<gui_button>(iter->id)->get_image()->get_texture();
			if(tex == icon_cross) {
				get_object<gui_button>(iter->id)->get_image()->set_color(gs->get_color("ICON1"));
			}
			else if(tex == icon_arrow_up || tex == icon_arrow_down) {
				get_object<gui_button>(iter->id)->get_image()->set_color(gs->get_color("ICON2"));
			}
		}
		else if(iter->type == gui::CHECK) {
			get_object<gui_check>(iter->id)->get_image()->set_color(gs->get_color("ICON3"));
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
		for(list<gui_window*>::iterator iter = gui_windows.begin(); iter != gui_windows.end(); iter++) {
			if(id == (*iter)->get_id()) {
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

unsigned int gui::get_free_id() {
	unsigned int id = start_id;
	// check if an element with such an id already exists
	while(exist(id) && id < 0xFFFFFFFF) {
		// if so, increment id and check next id ...
		id++;
	}
	// if we didn't find any id until 0xFFFFFFFF-1, start again at 0xFFFF (i don't think this will happen often ...)
	if(id == 0xFFFFFFFF-1) {
		id = 0xFFFF;
		while(exist(id) && id < start_id) {
			id++;
		}
		// if we still couldn't find a free id, print out an error msg ...
		if(id == start_id-1) {
			m->print(msg::MERROR, "gui.cpp", "get_free_id(): couldn't find a free id!");
			return 0;
		}
	}

	// if we found a free id, set the start_id to id+1 (next id) and return our found id
	start_id = id+1;
	return id;
}

bool gui::is_redraw(list<gui::gui_element>::iterator ge_iter) {
	switch(ge_iter->type) {
		case gui::BUTTON:
			return gui::get_object<gui_button>(ge_iter->id)->get_redraw();
			break;
		case gui::TEXT:
			return gui::get_object<gui_text>(ge_iter->id)->get_redraw();
			break;
		case gui::INPUT:
			return gui::get_object<gui_input>(ge_iter->id)->get_redraw();
			break;
		case gui::LIST:
			return gui::get_object<gui_list>(ge_iter->id)->get_redraw();
			break;
		case gui::VBAR:
			return gui::get_object<gui_vbar>(ge_iter->id)->get_redraw();
			break;
		case gui::CHECK:
			return gui::get_object<gui_check>(ge_iter->id)->get_redraw();
			break;
		case gui::COMBO:
			return gui::get_object<gui_combo>(ge_iter->id)->get_redraw();
			break;
		case gui::MLTEXT:
			return gui::get_object<gui_mltext>(ge_iter->id)->get_redraw();
			break;
		case gui::IMAGE:
			return gui::get_object<gui_image>(ge_iter->id)->get_redraw();
			break;
		case gui::TAB:
			return gui::get_object<gui_tab>(ge_iter->id)->get_redraw();
			break;
		default:
			break;
	}
	return false;
}

gui_object* gui::get_object(unsigned int id) {
	list<gui_object*>* objects = NULL;

	// so many cryptic letters, so less sense :>
	switch(get_element(id)->type) {
		case BUTTON:
			objects = dynamic_cast<list<gui_object*>*>((list<gui_object*>*)&gui_buttons);
			break;
		case INPUT:
			objects = dynamic_cast<list<gui_object*>*>((list<gui_object*>*)&gui_input_boxes);
			break;
		case TEXT:
			objects = dynamic_cast<list<gui_object*>*>((list<gui_object*>*)&gui_texts);
			break;
		case LIST:
			objects = dynamic_cast<list<gui_object*>*>((list<gui_object*>*)&gui_list_boxes);
			break;
		case VBAR:
			objects = dynamic_cast<list<gui_object*>*>((list<gui_object*>*)&gui_vbars);
			break;
		case CHECK:
			objects = dynamic_cast<list<gui_object*>*>((list<gui_object*>*)&gui_check_boxes);
			break;
		case COMBO:
			objects = dynamic_cast<list<gui_object*>*>((list<gui_object*>*)&gui_combo_boxes);
			break;
		case WINDOW:
			objects = dynamic_cast<list<gui_object*>*>((list<gui_object*>*)&gui_windows);
			break;
		case MLTEXT:
			objects = dynamic_cast<list<gui_object*>*>((list<gui_object*>*)&gui_mltexts);
			break;
		case IMAGE:
			objects = dynamic_cast<list<gui_object*>*>((list<gui_object*>*)&gui_images);
			break;
		case TAB:
			objects = dynamic_cast<list<gui_object*>*>((list<gui_object*>*)&gui_tabs);
			break;
		default:
			break;
	}

	for(list<gui_object*>::iterator iter = objects->begin(); iter != objects->end(); iter++) {
		if((*iter)->get_id() == id) {
			return *iter;
		}
	}

	m->print(msg::MERROR, "gui.cpp", "get_object(): no object with such an id (%u) exists!", id);

	return NULL;
}
