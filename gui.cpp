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
gui::gui() {
	p = (gfx::pnt*)malloc(sizeof(gfx::pnt));
	r = (gfx::rect*)malloc(sizeof(gfx::rect));
	input_text = (char*)malloc(512);

	//char* ib_text = (char*)malloc(1024);
	ib_text_length = 0;
	//char* set_text = (char*)malloc(1028);
	for(unsigned int i = 0; i < 1028; i++) {
        set_text[i] = 0;
	}

	// set current active window id to zero
	gui::awid = 0;
}

/*! there is no function currently
*/
gui::~gui() {
	m.print(msg::MDEBUG, "gui.cpp", "freeing gui stuff");

	free(p);
	free(r);
	free(input_text);

	for(unsigned int i = 0; i < MAX_ELEMENTS; i++) {
		delete gui_buttons[i];
	}

	for(unsigned int i = 0; i < MAX_ELEMENTS; i++) {
        delete gui_texts[i];
	}

	for(unsigned int i = 0; i < MAX_ELEMENTS; i++) {
        delete gui_input_boxes[i];
	}

	for(unsigned int i = 0; i < MAX_ELEMENTS; i++) {
        delete gui_list_boxes[i];
	}

	for(unsigned int i = 0; i < MAX_ELEMENTS; i++) {
        delete gui_vbars[i];
	}

	for(unsigned int i = 0; i < MAX_ELEMENTS; i++) {
        delete gui_check_boxes[i];
	}

	for(unsigned int i = 0; i < MAX_ELEMENTS; i++) {
		delete gui_windows[i];
	}

	m.print(msg::MDEBUG, "gui.cpp", "gui stuff freed");
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
	clist_boxes = 0;
	cvbars = 0;
	ccboxes = 0;

	if(gui::engine_handler->get_screen() == NULL) { m.print(msg::MERROR, "gui.cpp", "SDL_Surface does not exist!"); }
	else { gui::gui_surface = gui::engine_handler->get_screen(); }

	// reserve memory for 512 gui elements
	gui::gui_elements = (gui::gui_element*)malloc(sizeof(gui::gui_element) * 512);

	gui::active_element = (gui::gui_element*)malloc(sizeof(gui::gui_element));

	// init the main window
	main_window = gui::add_window(g.pnt_to_rect(0, 0, engine_handler->get_screen()->w,
		engine_handler->get_screen()->h), 0, "main window", false);

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
	// reset event stuff to camera - if there is a gui event,
	// the active class type will be overwritten
	event_handler->set_active(event::CAMERA);

	// start 2d drawing
	engine_handler->start_2d_draw();

	// reset active gui element
	set_active_element(&gui::gui_elements[0]);

	// sort from low to high id
	unsigned int* wnds_lid = (unsigned int*)malloc(sizeof(unsigned int) * cwindows);
	unsigned int* wnds_num = (unsigned int*)malloc(sizeof(unsigned int) * cwindows);

	for(unsigned int i = 0; i < cwindows; i++) {
		wnds_lid[i] = gui::gui_windows[i]->get_lid();
		wnds_num[i] = i;
	}

	bool end = false;
	unsigned int tmp = 0;
	for(unsigned int i = 0; i < (cwindows - 1); i++) {
		end = true;
		for(unsigned int j = 0; j < cwindows - i; j++) {
			if(wnds_lid[j] > wnds_lid[j+1]) {
				tmp = wnds_lid[j];
				wnds_lid[j] = wnds_lid[j+1];
				wnds_lid[j+1] = tmp;

				tmp = wnds_num[j];
				wnds_num[j] = wnds_num[j+1];
				wnds_num[j+1] = tmp;

				end = false;
			}
		}

		if(end) {
			i = cwindows;
		}
	}

	// get event window - or if none or if event is not
	// in a window, the background (lid = 0)

	// check if left mouse button is currently pressed (then use
	// that point otherwise use the last pressed point)
	unsigned int event_type = 0;
	if(event_handler->get_lm_pressed_x() == 0 && event_handler->get_lm_pressed_y() == 0) {
		p->x = event_handler->get_lm_last_pressed_x();
		p->y = event_handler->get_lm_last_pressed_y();
		event_type = 1;
	}
	else {
		p->x = event_handler->get_lm_pressed_x();
		p->y = event_handler->get_lm_pressed_y();
		event_type = 2;
	}

	unsigned int awlid = 0;
	unsigned int start_num = 0;
	for(unsigned int i = 0; i < cwindows; i++) {
		gfx::rect* rect = gui::gui_windows[wnds_num[(cwindows-1)-i]]->get_rectangle();
		if(g.is_pnt_in_rectangle(rect, p)) {
			gui::awid = gui::gui_windows[wnds_num[(cwindows-1)-i]]->get_id();
			awlid = gui::gui_windows[wnds_num[(cwindows-1)-i]]->get_lid();
			start_num = (cwindows-1)-i;
			i = cwindows;
		}
	}

	// sort the windows for their layer ids and move the
	// active window to the top of the "layer id stack"
	if(awlid != wnds_lid[gui::cwindows-1] && awlid != 0) {
		unsigned int tmp_lid = wnds_lid[cwindows-1];
		unsigned int diff = (cwindows-1) - start_num;
		for(unsigned int i = 0; i < diff; i++) {
			wnds_lid[start_num + (diff - i)] = wnds_lid[start_num + (diff - i) - 1];
			gui::gui_windows[wnds_num[start_num + (diff - i)]]->set_lid(wnds_lid[start_num + (diff - i) - 1]);
		}
		wnds_lid[start_num] = tmp_lid;
		gui::gui_windows[wnds_num[start_num]]->set_lid(tmp_lid);
	}

	// render gui elements - window wise: the background first and the frontest window last
	for(unsigned int j = 0; j < cwindows; j++) {
		unsigned int current_id = gui::gui_windows[wnds_num[j]]->get_id();

		// check if we got a window with a title bar and border
		if(gui::gui_windows[wnds_num[j]]->get_border()) {
			// reposition the window if it has been moved or is moving
			gfx::rect* r = (gfx::rect*)malloc(sizeof(gfx::rect));
			memcpy(r, gui::gui_windows[wnds_num[j]]->get_rectangle(), sizeof(gfx::rect));
			r->x1 += 1;
			r->x2 -= 1;
			r->y2 = r->y1 + 17;
			r->y1 += 1;

			if(event_type == 1 && gui::gui_windows[wnds_num[j]]->is_moving()) {
				gui::gui_windows[wnds_num[j]]->set_moving(false);
			}

			if((g.is_pnt_in_rectangle(r, p) || gui::gui_windows[wnds_num[j]]->is_moving())
				&& event_type == 2) {
				gfx::pnt* tmp_point = (gfx::pnt*)malloc(sizeof(gfx::pnt));
				event_handler->get_mouse_pos(tmp_point);
				// well, for more performance we could uncomment this, but the
				// windows moves smoother w/o it ;)
				//if(event_handler->get_lm_pressed_x() != tmp_point->x ||
				//	event_handler->get_lm_pressed_y() != tmp_point->y) {
					gui::gui_windows[wnds_num[j]]->set_moving(true);
					gui::gui_windows[wnds_num[j]]->change_position((int)tmp_point->x - (int)event_handler->get_lm_pressed_x(),
						(int)tmp_point->y - (int)event_handler->get_lm_pressed_y());
					event_handler->set_pressed(tmp_point->x, tmp_point->y);
				//}
				free(tmp_point);
			}

			free(r);
		}

		// draw the window
		gui::gui_windows[wnds_num[j]]->draw();

		// set the "window point" that defines how much the
		// gui elements have to be moved on the x and y axis
		gfx::pnt* wp = (gfx::pnt*)malloc(sizeof(gfx::pnt));
		if(gui::gui_windows[wnds_num[j]]->get_border()) {
            wp->x = gui::gui_windows[wnds_num[j]]->get_rectangle()->x1 + 2;
            wp->y = gui::gui_windows[wnds_num[j]]->get_rectangle()->y1 + 19;
		}
		else {
            wp->x = gui::gui_windows[wnds_num[j]]->get_rectangle()->x1;
            wp->y = gui::gui_windows[wnds_num[j]]->get_rectangle()->y1;
		}

		// we need a rectangle object to add the windows
		// x and y coordinate to the gui elements rectangle,
		// so we can check for a button press correctly
		gfx::rect* r = (gfx::rect*)malloc(sizeof(gfx::rect));

		// draw the gui elements
		for(unsigned int i = 0; i < celements; i++) {
			if(gui::gui_elements[i].is_drawn == true &&
				gui::gui_elements[i].wid == current_id) {
				switch(gui::gui_elements[i].type) {
					case gui::BUTTON: {
						p->x = event_handler->get_lm_pressed_x();
						p->y = event_handler->get_lm_pressed_y();

						// the window button flag - defines if the button is a window button ...
						bool wnd_button = false;

						// well, it's somehow an odd way to do this, but why
						// should i add extra stuff that is just needed once ...
						if(gui::gui_buttons[gui::gui_elements[i].num]->get_icon() == 3) {
							// reposition the exit button so it fits into the windows title bar
							// get the rectangle and add the windows x and y coordinate
							memcpy(r, gui::gui_buttons[gui::gui_elements[i].num]->get_rectangle(), sizeof(gfx::rect));
							r->x1 += gui::gui_windows[wnds_num[j]]->get_rectangle()->x2 - 14;
							r->x2 += gui::gui_windows[wnds_num[j]]->get_rectangle()->x2 - 2;
							r->y1 += wp->y - 16;
							r->y2 += wp->y - 16;

							// set window button flag to true
							wnd_button = true;
						}
						else {
							// get the rectangle and add the windows x and y coordinate
							memcpy(r, gui::gui_buttons[gui::gui_elements[i].num]->get_rectangle(), sizeof(gfx::rect));
							r->x1 += wp->x;
							r->x2 += wp->x;
							r->y1 += wp->y;
							r->y2 += wp->y;
						}

						if(current_id == gui::awid &&
							g.is_pnt_in_rectangle(r, p)) {
							if(!wnd_button) {
								gui::gui_buttons[gui::gui_elements[i].num]->draw(true, wp->x, wp->y);
							}
							else {
								gui::gui_buttons[gui::gui_elements[i].num]->draw(true, r->x1, r->y1);
							}
							gui::gui_buttons[gui::gui_elements[i].num]->set_pressed(true);
							set_active_element(&gui::gui_elements[i]);
						}
						else {
							if(gui::gui_buttons[gui::gui_elements[i].num]->get_pressed() == true) {
								gui::gui_buttons[gui::gui_elements[i].num]->set_pressed(false);
								event_handler->add_gui_event(event_handler->BUTTON_PRESSED,
									gui::gui_buttons[gui::gui_elements[i].num]->get_id());
							}

							if(!wnd_button) {
                                gui::gui_buttons[gui::gui_elements[i].num]->draw(false, wp->x, wp->y);
							}
							else {
								gui::gui_buttons[gui::gui_elements[i].num]->draw(false, r->x1, r->y1);
							}
						}
					}
					break;
					case gui::TEXT:
						gui::gui_texts[gui::gui_elements[i].num]->draw(wp->x, wp->y);
						break;
					case gui::INPUT: {
						p->x = event_handler->get_lm_last_pressed_x();
						p->y = event_handler->get_lm_last_pressed_y();

						// get the rectangle and add the windows x and y coordinate
						memcpy(r, gui::gui_input_boxes[gui::gui_elements[i].num]->get_rectangle(), sizeof(gfx::rect));
						r->x1 += wp->x;
						r->x2 += wp->x;
						r->y1 += wp->y;
						r->y2 += wp->y;

						if(current_id == gui::awid &&
							g.is_pnt_in_rectangle(r, p)) {
							gui::gui_input_boxes[gui::gui_elements[i].num]->set_active(true);
							set_active_element(&gui::gui_elements[i]);
							event_handler->set_active(event::GUI);

							event_handler->get_input_text(input_text);
							gui::switch_input_text(input_text,
								gui::gui_input_boxes[gui::gui_elements[i].num]);
						}
						else {
							gui::gui_input_boxes[gui::gui_elements[i].num]->set_active(false);
						}
						
						gui::gui_input_boxes[gui::gui_elements[i].num]->draw(wp->x, wp->y);
					}
					break;
					case gui::LIST: {
						p->x = event_handler->get_lm_last_pressed_x();
						p->y = event_handler->get_lm_last_pressed_y();

						// get the rectangle and add the windows x and y coordinate
						memcpy(r, gui::gui_list_boxes[gui::gui_elements[i].num]->get_rectangle(), sizeof(gfx::rect));
						r->x1 += wp->x;
						// decrease the rects x2 coord by 15, because we don't want input from the vbar
						r->x2 += wp->x - 15;
						r->y1 += wp->y;
						r->y2 += wp->y;

						if(current_id == gui::awid && g.is_pnt_in_rectangle(r, p)) {
							gui::gui_list_boxes[gui::gui_elements[i].num]->set_active(true);
							gui::gui_list_boxes[gui::gui_elements[i].num]->select_pos(p->x - wp->x, p->y - wp->y);
							event_handler->set_last_pressed(0, 0);
						}
						else {
							gui::gui_list_boxes[gui::gui_elements[i].num]->set_active(false);
						}

						gui::gui_list_boxes[gui::gui_elements[i].num]->draw(wp->x, wp->y);
					}
					break;
					case gui::VBAR: {
						p->x = event_handler->get_lm_pressed_x();
						p->y = event_handler->get_lm_pressed_y();

						// get the rectangle and add the windows x and y coordinate
						memcpy(r, gui::gui_vbars[gui::gui_elements[i].num]->get_rectangle(), sizeof(gfx::rect));
						r->x1 += wp->x;
						r->x2 += wp->x;
						r->y1 += wp->y;
						r->y2 += wp->y;

						if(current_id == gui::awid &&
							gui::gui_vbars[gui::gui_elements[i].num]->get_slider_active()) {
							if(g.is_pnt_in_rectangle(r, p)) {
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

						gui::gui_vbars[gui::gui_elements[i].num]->draw(wp->x, wp->y);
					}
					break;
					case gui::CHECK: {
						p->x = event_handler->get_lm_last_pressed_x();
						p->y = event_handler->get_lm_last_pressed_y();

						// get the rectangle and add the windows x and y coordinate
						// and reduce the "clickable" area
						memcpy(r, gui::gui_check_boxes[gui::gui_elements[i].num]->get_rectangle(), sizeof(gfx::rect));
						r->x1 += wp->x;
						r->x2 = r->x1 + 14;
						r->y1 += wp->y;
						r->y2 = r->y1 + 14;

						if(current_id == gui::awid && g.is_pnt_in_rectangle(r, p)) {
							gui::gui_check_boxes[gui::gui_elements[i].num]->set_checked(gui::gui_check_boxes[gui::gui_elements[i].num]->get_checked() ^ true);
							event_handler->set_last_pressed(0, 0);
						}

						gui::gui_check_boxes[gui::gui_elements[i].num]->draw(wp->x, wp->y);
					}
					break;
					default:
						break;
				}
			}
		}

		free(wp);
		free(r);
	}

	// window deleting routine
	for(unsigned int j = 0; j < cwindows; j++) {
		// check if the windows exit button was pressed and delete the window
		if(gui::gui_windows[wnds_num[j]]->get_deleted()) {
			// todo: delete the windows gui elements (?)
			gui::delete_element(gui::gui_windows[wnds_num[j]]->get_id());
		}
	}

	free(wnds_lid);
	free(wnds_num);

	// stop 2d drawing
	engine_handler->stop_2d_draw();
}

/*! adds a gui button element and returns it
 *  @param rectangle the buttons rectangle
 *  @param id the buttons (a2e event) id
 *  @param text the buttons text
 *  @param icon the icon id (0 = no icon)
 *  @param wid the id of the window we want the element to be in (0 = no window)
 */
gui_button* gui::add_button(gfx::rect* rectangle, unsigned int icon, unsigned int id, char* text, unsigned int wid) {
	gui::gui_elements[celements].id = id;
	gui::gui_elements[celements].type = gui::BUTTON;
	gui::gui_elements[celements].num = cbuttons;
	gui::gui_elements[celements].wid = wid;
	gui::gui_elements[celements].is_drawn = true;
	
	// celements has to be incremented _before_ we add the text, otherwise
	// our button stuff will be overwritten
	celements++;

	// create class
	gui::gui_buttons[cbuttons] = new gui_button();

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
 *  @param wid the id of the window we want the element to be in (0 = no window)
 */
gui_button* gui::add_button(gfx::rect* rectangle, unsigned int id, char* text, unsigned int wid) {
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
gui_text* gui::add_text(char* font_name, unsigned int font_size, char* text,
				   unsigned int color, gfx::pnt* point, unsigned int id, unsigned int wid) {
	SDL_Color col;
	col.r = (color & 0xFF0000) >> 16;
	col.g = (color & 0x00FF00) >> 8;
	col.b = color & 0x0000FF;

	gui::gui_elements[celements].id = id;
	gui::gui_elements[celements].type = gui::TEXT;
	gui::gui_elements[celements].num = ctexts;
	gui::gui_elements[celements].wid = wid;
	gui::gui_elements[celements].is_drawn = true;

	// create class
	gui::gui_texts[ctexts] = new gui_text();

	gui::gui_texts[ctexts]->set_init(false);
	gui::gui_texts[ctexts]->set_engine_handler(gui::engine_handler);
	gui::gui_texts[ctexts]->set_id(id);
	gui::gui_texts[ctexts]->new_text(font_name, font_size);
	gui::gui_texts[ctexts]->set_point(point);
	gui::gui_texts[ctexts]->set_text(text);
	gui::gui_texts[ctexts]->set_color(col);
	gui::gui_texts[ctexts]->set_init(true);

	celements++;
	ctexts++;

	return gui::gui_texts[ctexts-1];
}

/*! adds a gui input box element and returns it
 *  @param rectangle the input boxes rectangle
 *  @param id the input boxes (a2e event) id
 *  @param text the input boxes text
 *  @param wid the id of the window we want the element to be in (0 = no window)
 */
gui_input* gui::add_input_box(gfx::rect* rectangle, unsigned int id, char* text, unsigned int wid) {
	gui::gui_elements[celements].id = id;
	gui::gui_elements[celements].type = gui::INPUT;
	gui::gui_elements[celements].num = cinput_boxes;
	gui::gui_elements[celements].wid = wid;
	gui::gui_elements[celements].is_drawn = true;
	
	// celements has to be incremented _before_ we add the text, otherwise
	// our input box stuff will be overwritten
	celements++;

	// create class
	gui::gui_input_boxes[cinput_boxes] = new gui_input();

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
 *  @param wid the id of the window we want the element to be in (0 = no window)
 */
gui_list* gui::add_list_box(gfx::rect* rectangle, unsigned int id, char* text, unsigned int wid) {
	gui::gui_elements[celements].id = id;
	gui::gui_elements[celements].type = gui::LIST;
	gui::gui_elements[celements].num = clist_boxes;
	gui::gui_elements[celements].wid = wid;
	gui::gui_elements[celements].is_drawn = true;

	celements++;

	// create class
	gui::gui_list_boxes[clist_boxes] = new gui_list();

	// add vertical bar
	gui::gui_list_boxes[clist_boxes]->set_vbar_handler(add_vbar(g.pnt_to_rect(rectangle->x2-14,
		rectangle->y1+2, rectangle->x2-2, rectangle->y2-2), id+0xFFFFFF, wid));

	gui::gui_list_boxes[clist_boxes]->set_engine_handler(gui::engine_handler);
	gui::gui_list_boxes[clist_boxes]->set_id(id);
	gui::gui_list_boxes[clist_boxes]->set_rectangle(rectangle);
	gui::gui_list_boxes[clist_boxes]->set_position(0);

	clist_boxes++;

	return gui_list_boxes[clist_boxes-1];
}

gui_vbar* gui::add_vbar(gfx::rect* rectangle, unsigned int id, unsigned int wid) {
	gui::gui_elements[celements].id = id;
	gui::gui_elements[celements].type = gui::VBAR;
	gui::gui_elements[celements].num = cvbars;
	gui::gui_elements[celements].wid = wid;
	gui::gui_elements[celements].is_drawn = true;

	// celements has to be incremented _before_ we add the buttons, otherwise
	// our vertical bar stuff will be overwritten
	celements++;

	// create class
	gui::gui_vbars[cvbars] = new gui_vbar();

	// add up button
	gui::gui_vbars[cvbars]->set_up_button_handler(add_button(g.pnt_to_rect(0,0,1,1), 1,
		id+0xFFFFFF, " ", wid));

	// add down button
	gui::gui_vbars[cvbars]->set_down_button_handler(add_button(g.pnt_to_rect(0,0,1,1), 2,
		id+0xFFFFFE, " ", wid));

	gui::gui_vbars[cvbars]->set_engine_handler(gui::engine_handler);
	gui::gui_vbars[cvbars]->set_id(id);
	gui::gui_vbars[cvbars]->set_rectangle(rectangle);

	cvbars++;

	return gui_vbars[cvbars-1];
}

/*! adds a gui check box element and returns it
 *  @param rectangle the check boxes rectangle
 *  @param id the check boxes (a2e event) id
 *  @param text the check boxes text
 *  @param wid the id of the window we want the element to be in (0 = no window)
 */
gui_check* gui::add_check_box(gfx::rect* rectangle, unsigned int id, char* text, unsigned int wid) {
	gui::gui_elements[celements].id = id;
	gui::gui_elements[celements].type = gui::CHECK;
	gui::gui_elements[celements].num = ccboxes;
	gui::gui_elements[celements].wid = wid;
	gui::gui_elements[celements].is_drawn = true;
	
	// celements has to be incremented _before_ we add the text, otherwise
	// our check box stuff will be overwritten
	celements++;

	// create class
	gui::gui_check_boxes[ccboxes] = new gui_check();

	gui::gui_check_boxes[ccboxes]->set_text_handler(add_text("vera.ttf", 12, text,
		engine_handler->get_gstyle().STYLE_FONT2, g.cord_to_pnt(0,0), id+0xFFFF, wid));
	// don't draw our text automatically
	// celements-1, because our text element, is the last initialized element
	gui::gui_elements[celements-1].is_drawn = false;

	gui::gui_check_boxes[ccboxes]->set_engine_handler(gui::engine_handler);
	gui::gui_check_boxes[ccboxes]->set_id(id);
	gui::gui_check_boxes[ccboxes]->set_rectangle(rectangle);
	gui::gui_check_boxes[ccboxes]->set_text(text);

	ccboxes++;

	return gui_check_boxes[ccboxes-1];
}

/*! adds a gui window element and returns it
 *  @param rectangle the windows rectangle
 *  @param id the windows (a2e event) id
 *  @param caption the windows caption
 *  @param border a bool if the windows border and titlebar should be drawn
 */
gui_window* gui::add_window(gfx::rect* rectangle, unsigned int id, char* caption, bool border) {
	gui::gui_elements[celements].id = id;
	gui::gui_elements[celements].type = gui::WINDOW;
	gui::gui_elements[celements].num = cwindows;
	gui::gui_elements[celements].wid = id;
	gui::gui_elements[celements].is_drawn = true;
	
	// celements has to be incremented _before_ we add the text, otherwise
	// our button stuff will be overwritten
	celements++;

	// create class
	gui::gui_windows[cwindows] = new gui_window();

	// add exit button if we got a border
	if(border) {
		gui::gui_windows[cwindows]->set_exit_button_handler(add_button(g.pnt_to_rect(0,0,12,12), 3,
			id+0xFFFFFF, "x", id));
	}

	gui::gui_windows[cwindows]->set_text_handler(add_text("vera.ttf", 12, caption,
		engine_handler->get_gstyle().STYLE_FONT2, g.cord_to_pnt(0,0), id+0xFFFF));
	// don't draw our text automatically
	// celements-1, because our text element, is the last initialized element
	gui::gui_elements[celements-1].is_drawn = false;

	gui::gui_windows[cwindows]->set_engine_handler(gui::engine_handler);
	gui::gui_windows[cwindows]->set_id(id);
	gui::gui_windows[cwindows]->set_rectangle(rectangle);
	gui::gui_windows[cwindows]->set_caption(caption);
	gui::gui_windows[cwindows]->set_lid(cwindows);
	gui::gui_windows[cwindows]->set_border(border);

	cwindows++;

	return gui_windows[cwindows-1];
}

//! returns the guis surface
SDL_Surface* gui::get_gui_surface() {
	return gui::gui_surface;
}

/*! returns the currently active gui element
 */
gui::gui_element* gui::get_active_element() {
	return gui::active_element;
}

/*! sets the currently active gui element via gui_element pointer
 *  @param active_element pointer to the new active element
 */
void gui::set_active_element(gui_element* active_element) {
	gui::active_element = active_element;
	event_handler->set_active_element((event::gui_element*)active_element);
}

/*! sets the currently active gui element via the gui element's id
 *  @param id the id of the new active element
 */
void gui::set_active_element(unsigned int id) {
	for(unsigned int i = 0; i < celements; i++) {
		if(gui::gui_elements[i].id == id) {
			gui::set_active_element(&gui::gui_elements[i]);
			return;
		}
	}
}

/*! handles the input that we get from the event class for the input box
 *  @param input_text chars written to the input box
 *  @param input_box pointer to the input box
 */
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

	bool deleted = false;
	unsigned int del_num = 0;

	for(unsigned int i = 0; i < gui::celements; i++) {
		if(gui::gui_elements[i].id == id) {
			switch(gui::gui_elements[i].type) {
				case gui::BUTTON:
					for(unsigned int j = 0; j < gui::cbuttons; j++) {
						if(gui::gui_elements[i].id == gui::gui_buttons[j]->get_id()) {
							delete gui::gui_buttons[j];
							gui::gui_buttons[j] = gui::gui_buttons[gui::cbuttons];
							gui::gui_buttons[gui::cbuttons] = NULL;
							gui::cbuttons--;

							del_num = i;
							j = gui::cbuttons;
							i = gui::celements;
							deleted = true;
						}
					}
					break;
				case gui::CHECK:
					for(unsigned int j = 0; j < gui::ccboxes; j++) {
						if(gui::gui_elements[i].id == gui::gui_check_boxes[j]->get_id()) {
							delete gui::gui_check_boxes[j];
							gui::gui_check_boxes[j] = gui::gui_check_boxes[gui::ccboxes];
							gui::gui_check_boxes[gui::ccboxes] = NULL;
							gui::ccboxes--;

							del_num = i;
							j = gui::ccboxes;
							i = gui::celements;
							deleted = true;
						}
					}
					break;
				case gui::INPUT:
					for(unsigned int j = 0; j < gui::cinput_boxes; j++) {
						if(gui::gui_elements[i].id == gui::gui_input_boxes[j]->get_id()) {
							delete gui::gui_input_boxes[j];
							gui::gui_input_boxes[j] = gui::gui_input_boxes[gui::cinput_boxes];
							gui::gui_input_boxes[gui::cinput_boxes] = NULL;
							gui::cinput_boxes--;

							del_num = i;
							j = gui::cinput_boxes;
							i = gui::celements;
							deleted = true;
						}
					}
					break;
				case gui::LIST:
					for(unsigned int j = 0; j < gui::clist_boxes; j++) {
						if(gui::gui_elements[i].id == gui::gui_list_boxes[j]->get_id()) {
							delete gui::gui_list_boxes[j];
							gui::gui_list_boxes[j] = gui::gui_list_boxes[gui::clist_boxes];
							gui::gui_list_boxes[gui::clist_boxes] = NULL;
							gui::clist_boxes--;

							del_num = i;
							j = gui::clist_boxes;
							i = gui::celements;
							deleted = true;
						}
					}
					break;
				case gui::TEXT:
					for(unsigned int j = 0; j < gui::ctexts; j++) {
						if(gui::gui_elements[i].id == gui::gui_texts[j]->get_id()) {
							delete gui::gui_texts[j];
							gui::gui_texts[j] = gui::gui_texts[gui::ctexts];
							gui::gui_texts[gui::ctexts] = NULL;
							gui::ctexts--;

							del_num = i;
							j = gui::ctexts;
							i = gui::celements;
							deleted = true;
						}
					}
					break;
				case gui::VBAR:
					for(unsigned int j = 0; j < gui::cvbars; j++) {
						if(gui::gui_elements[i].id == gui::gui_vbars[j]->get_id()) {
							delete gui::gui_vbars[j];
							gui::gui_vbars[j] = gui::gui_vbars[gui::cvbars];
							gui::gui_vbars[gui::cvbars] = NULL;
							gui::cvbars--;

							del_num = i;
							j = gui::cvbars;
							i = gui::celements;
							deleted = true;
						}
					}
					break;
				case gui::WINDOW:
					for(unsigned int j = 0; j < gui::cwindows; j++) {
						if(gui::gui_elements[i].id == gui::gui_windows[j]->get_id()) {
							delete gui::gui_windows[j];
							gui::gui_windows[j] = gui::gui_windows[gui::cwindows-1];
							gui::gui_windows[gui::cwindows-1] = NULL;
							gui::cwindows--;

							del_num = i;
							j = gui::cwindows;
							i = gui::celements;
							deleted = true;
						}
					}
					break;
				default:
					break;
			}
		}
	}

	if(deleted) {
		gui::gui_elements[del_num].id = gui::gui_elements[gui::celements].id;
		gui::gui_elements[del_num].is_drawn = gui::gui_elements[gui::celements].is_drawn;
		gui::gui_elements[del_num].num = gui::gui_elements[gui::celements].num;
		gui::gui_elements[del_num].type = gui::gui_elements[gui::celements].type;
		gui::gui_elements[del_num].wid = gui::gui_elements[gui::celements].wid;
		gui::celements--;

		return true;
	}
	else {
		return false;
	}
}
