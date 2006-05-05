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

#include "gui_sample.h"
#include <iostream>
#include <cstring>

/*!
 * \mainpage
 *
 * \author flo
 *
 * \date July 2004 - April 2006
 *
 * Albion 2 Engine Sample - GUI Sample
 */

int main(int argc, char *argv[])
{
	// initialize the engine
	e = new engine("../../data/");
	e->init();
	e->set_caption("A2E Sample - GUI Sample");

	// init class pointers
	m = e->get_msg();
	c = e->get_core();
	aevent = e->get_event();
	agfx = e->get_gfx();
	agui = new gui(e);
	gui_style* gs = agui->get_gui_style();

	// initialize the a2e events
	aevent->init(sevent);
	aevent->load_keyset("DE");

	// initialize the gui
	agui->init();

	GUI_OBJ output_text = agui->add_text("STANDARD", 12, "-", 0x000000, agfx->cord_to_pnt(10, 580), 103, 0);
	gui_text* output_text_obj = agui->get_text(output_text);

	GUI_OBJ t1 = agui->add_text("STANDARD", 14, "test text", 0xFFFFFF, agfx->cord_to_pnt(100, 5), 102, 0);
	GUI_OBJ i1 = agui->add_input_box(agfx->pnt_to_rect(10, 300, 100, 320), 105, "input text", 0);
	GUI_OBJ l1 = agui->add_list_box(agfx->pnt_to_rect(400, 200, 750, 440), 106, 0);
	gui_text* t1_obj = agui->get_text(t1);
	gui_input* i1_obj = agui->get_input(i1);
	gui_list* l1_obj = agui->get_list(l1);
	GUI_OBJ c1 = agui->add_combo_box(agfx->pnt_to_rect(100, 30, 220, 50), 112, 0);
	gui_combo* c1_obj = agui->get_combo(c1);
	c1_obj->add_item("item #1", 0);
	c1_obj->add_item("another item", 1);
	c1_obj->add_item("one more", 2);
	c1_obj->add_item("and so on ...", 3);
	c1_obj->add_item("and on ...", 4);
	c1_obj->add_item("item #6", 5);
	c1_obj->add_item("test", 6);
	c1_obj->add_item("last one", 7);

	GUI_OBJ wnd = agui->add_window(agfx->pnt_to_rect(150, 100, 370, 450), 110, "test window", true);
	GUI_OBJ xpos_text = agui->add_text("STANDARD", 12, "0", 0x000000, agfx->cord_to_pnt(20, 20), 107, 110);
	GUI_OBJ ypos_text = agui->add_text("STANDARD", 12, "0", 0x000000, agfx->cord_to_pnt(20, 40), 108, 110);
	GUI_OBJ cbox = agui->add_check_box(agfx->pnt_to_rect(10, 70, 200, 90), 109, "Test Check Box", 110);
	gui_window* wnd_obj = agui->get_window(wnd);
	gui_text* xpos_text_obj = agui->get_text(xpos_text);
	gui_text* ypos_text_obj = agui->get_text(ypos_text);
	gui_check* cbox_obj = agui->get_check(cbox);

	GUI_OBJ wnd2 = agui->add_window(agfx->pnt_to_rect(560, 100, 780, 450), 111, "test window 2", true);
	GUI_OBJ b1 = agui->add_button(agfx->pnt_to_rect(10, 10, 200, 40), 100, "test button", 0, 111);
	GUI_OBJ b2 = agui->add_button(agfx->pnt_to_rect(10, 50, 200, 80), 101, "test button 2", 0, 111);
	gui_window* wnd2_obj = agui->get_window(wnd2);
	gui_button* b1_obj = agui->get_button(b1);
	gui_button* b2_obj = agui->get_button(b2);

	// add 32 items
	for(unsigned int i = 1; i <= 32; i++) {
		tmp << "test " << i;
		l1_obj->add_item((char*)tmp.str().c_str(), i);
		c->reset(&tmp);
	}

	img = new image(e);
	img->open_image(e->data_path("engine_logo.png"));
	img->set_position(800 - 446, 600 - 130);

	// needed for fps counting
	unsigned int fps = 0;
	unsigned int fps_time = 0;

	core::pnt* mpos = new core::pnt();
	gfx::rect* r1 = new gfx::rect();
	r1->x1 = 0;
	r1->y1 = 0;
	r1->x2 = e->get_screen()->w;
	r1->y2 = e->get_screen()->h;
	while(!done)
	{
		while(aevent->is_event())
		{
			aevent->handle_events(aevent->get_event().type);
			switch(aevent->get_event().type) {
				case SDL_QUIT:
					done = true;
					break;
				case SDL_KEYDOWN:
					switch(aevent->get_event().key.keysym.sym) {
						case SDLK_ESCAPE:
							done = true;
							break;
						default:
						break;
					}
					break;
				default:
				break;
			}
		}

		while(aevent->is_gui_event()) {
			switch(aevent->get_gui_event().type) {
				case event::BUTTON_PRESSED: {
						tmp << "you pressed the button with the id #" << aevent->get_gui_event().id << "!";
						output_text_obj->set_text((char*)tmp.str().c_str());
						c->reset(&tmp);
					}
					break;
				case event::COMBO_ITEM_SELECTED:
					switch(aevent->get_gui_event().id) {
						case 112: {
							tmp << "you have selected item #" << c1_obj->get_selected_id() << "!";
							output_text_obj->set_text((char*)tmp.str().c_str());
							c->reset(&tmp);
						}
						break;
						default:
							break;
					}
					break;
				default:
				break;
			}
		}

		if(agui->exist(xpos_text) && agui->exist(ypos_text)) {
			aevent->get_mouse_pos(mpos);
			xpos << mpos->x;
			ypos << mpos->y;
			xpos_text_obj->set_text((char*)xpos.str().c_str());
			ypos_text_obj->set_text((char*)ypos.str().c_str());
			c->reset(&xpos);
			c->reset(&ypos);
		}

		// print out the fps count
		fps++;
		if(SDL_GetTicks() - fps_time > 1000) {
			caption << "A2E Sample - GUI Sample | FPS: " << fps;
			e->set_caption((char*)caption.str().c_str());
			c->reset(&caption);

			fps = 0;
			fps_time = SDL_GetTicks();
		}

		e->start_draw();

		e->start_2d_draw();
		agfx->draw_filled_rectangle(r1, gs->get_color("WINDOW_BG1"));
		e->stop_2d_draw();
		img->draw();
		agui->draw();

		e->stop_draw();
	}
	delete r1;

	delete mpos;

	delete img;
	delete agui;
	delete e;

	return 0;
}
