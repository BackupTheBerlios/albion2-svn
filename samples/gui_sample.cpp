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
#include <string.h>

/*!
 * \mainpage
 *
 * \author flo
 *
 * \date July - August 2004
 *
 * Albion 2 Engine Sample - GUI Sample
 */

int main(int argc, char *argv[])
{
	e.init(800, 600, 32, false);
	aevent.init(ievent);

	sf = e.get_screen();
	unsigned int black = SDL_MapRGB(sf->format, 0, 0, 0);
	unsigned int blue = SDL_MapRGB(sf->format, 0, 0, 255);
	unsigned int red = SDL_MapRGB(sf->format, 255, 0, 0);
	unsigned int white = SDL_MapRGB(sf->format, 255, 255, 255);

	agfx.draw_point(sf, agfx.cord_to_pnt(10, 10), black);
	agfx.draw_line(sf, agfx.cord_to_pnt(10, 20), agfx.cord_to_pnt(50, 40), black);
	agfx.draw_rectangle(sf, agfx.pnt_to_rect(100, 100, 200, 120), black);
	agfx.draw_2colored_rectangle(sf, agfx.pnt_to_rect(210, 210, 300, 300), blue, red);

	agui.init(e, aevent);
	gui_button* b1 = agui.add_button(agfx.pnt_to_rect(300, 50, 480, 80), 100, "test button");
	gui_button* b2 = agui.add_button(agfx.pnt_to_rect(520, 50, 700, 80), 101, "‰ˆ¸ƒ÷‹ﬂÈËÍ");
	gui_text* t1 = agui.add_text("vera.ttf", 14, "test text", 0x000000, agfx.cord_to_pnt(100, 5), 102);
	gui_text* output_text = agui.add_text("vera.ttf", 12, "-", 0x000000, agfx.cord_to_pnt(10, 580), 103);
	gui_input* i1 = agui.add_input_box(agfx.pnt_to_rect(10, 300, 100, 320), 105, "input text");
	gui_list* l1 = agui.add_list_box(agfx.pnt_to_rect(400, 200, 750, 450), 106, "blah");
	// add 32 items
	for(unsigned int i = 1; i <= 8; i++) {
		char tmp[16];
		sprintf(tmp, "test %u", i);
		l1->add_item(tmp, i);
	}
	aevent.set_keyboard_layout(event::DE);

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
					}
					break;
			}
		}

		while(aevent.is_gui_event()) {
			switch(aevent.get_gui_event().type) {
				case event::BUTTON_PRESSED:
					switch(aevent.get_gui_event().id) {
						case 100: {
							gfx::rect* r1 = (gfx::rect*)malloc(sizeof(gfx::rect));
							agfx.pnt_to_rect(r1, 10, 580, 799, 599);
							agfx.draw_filled_rectangle(sf, r1, white);
							output_text->set_text("button with the id #100 was pressed!");
							free(r1);
						}
						break;
						case 101: {
							gfx::rect* r1 = (gfx::rect*)malloc(sizeof(gfx::rect));
							agfx.pnt_to_rect(r1, 10, 580, 799, 599);
							agfx.draw_filled_rectangle(sf, r1, white);
							output_text->set_text("button with the id #101 was pressed!");
							free(r1);
						}
						break;
						default:
							break;
					}
					break;
			}
		}

		// refresh every 1000/60 milliseconds (~ 60 fps)
		if(SDL_GetTicks() - refresh_time >= 1000/60) {
			e.start_draw();
			agui.draw();
			e.stop_draw();
			refresh_time = SDL_GetTicks();
		}
	}

	return 0;
}
