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
 * \date July 2004 - March 2005
 *
 * Albion 2 Engine Sample - GUI Sample
 */

int main(int argc, char *argv[])
{
	// initialize the engine
	e.init(800, 600, 24, false);
	e.set_caption("A2E Sample - GUI Sample");

	// set a color scheme (blue)
	e.set_color_scheme(gui_style::BLUE);
	sf = e.get_screen();

	// initialize the a2e events
	aevent.init(ievent);
	aevent.set_keyboard_layout(event::DE);

	// initialize the gui
	agui.init(e, aevent);

	gui_text* t1 = agui.add_text("vera.ttf", 14, "test text", 0xFFFFFF, agfx.cord_to_pnt(100, 5), 102, 0);
	gui_text* output_text = agui.add_text("vera.ttf", 12, "-", 0xFFFFFF, agfx.cord_to_pnt(10, 580), 103, 0);
	gui_input* i1 = agui.add_input_box(agfx.pnt_to_rect(10, 300, 100, 320), 105, "input text", 0);
	gui_list* l1 = agui.add_list_box(agfx.pnt_to_rect(400, 200, 750, 440), 106, "blah", 0);

	gui_window* wnd = agui.add_window(agfx.pnt_to_rect(150, 100, 370, 450), 110, "test window", true);
	gui_text* xpos_text = agui.add_text("vera.ttf", 12, "0", 0x000000, agfx.cord_to_pnt(20, 20), 107, 110);
	gui_text* ypos_text = agui.add_text("vera.ttf", 12, "0", 0x000000, agfx.cord_to_pnt(20, 40), 108, 110);
	gui_check* cbox = agui.add_check_box(agfx.pnt_to_rect(10, 70, 200, 90), 109, "Test Check Box", 110);

	gui_window* wnd2 = agui.add_window(agfx.pnt_to_rect(560, 100, 780, 450), 111, "test window 2", true);
	gui_button* b1 = agui.add_button(agfx.pnt_to_rect(10, 10, 200, 40), 100, "test button", 111);
	gui_button* b2 = agui.add_button(agfx.pnt_to_rect(10, 50, 200, 80), 101, "test button 2", 111);

	// add 32 items
	for(unsigned int i = 1; i <= 32; i++) {
		char tmp[16];
		sprintf(tmp, "test %u", i);
		l1->add_item(tmp, i);
	}

	image img(&e);
	img.open_image("../data/engine_logo.png");
	img.set_position(800 - 446, 600 - 130);

	gfx::pnt* mpos = (gfx::pnt*)malloc(sizeof(gfx::pnt));
	char* xpos = (char*)malloc(8);
	char* ypos = (char*)malloc(8);
	refresh_time = SDL_GetTicks();
	while(!done)
	{
		while(aevent.is_event())
		{
			aevent.handle_events(aevent.get_event().type);
			switch(aevent.get_event().type) {
				case SDL_QUIT:
					done = true;
					break;
				case SDL_KEYDOWN:
					switch(aevent.get_event().key.keysym.sym) {
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

		while(aevent.is_gui_event()) {
			switch(aevent.get_gui_event().type) {
				case event::BUTTON_PRESSED:
					switch(aevent.get_gui_event().id) {
						case 100: {
							output_text->set_text("button with the id #100 was pressed!");
						}
						break;
						case 101: {
							output_text->set_text("button with the id #101 was pressed!");
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

		// refresh every 1000/60 milliseconds (~ 60 fps)
		//if(SDL_GetTicks() - refresh_time >= 1000/60) {
			aevent.get_mouse_pos(mpos);
			snprintf(xpos, 8, "%u", mpos->x);
			snprintf(ypos, 8, "%u", mpos->y);
			xpos_text->set_text(xpos);
			ypos_text->set_text(ypos);

			e.start_draw();

			img.draw();
			agui.draw();

			e.stop_draw();
			refresh_time = SDL_GetTicks();
		//}
	}

	free(mpos);
	free(xpos);
	free(ypos);

	SDL_FreeSurface(sf);

	return 0;
}
