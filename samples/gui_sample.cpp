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

/*!
 * \mainpage
 *
 * \author flo
 *
 * \date July 2004
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
	agfx.draw_point(sf, agfx.cord_to_pnt(10, 10), black);
	agfx.draw_line(sf, agfx.cord_to_pnt(10, 20), agfx.cord_to_pnt(50, 40), black);
	agfx.draw_rectangle(sf, agfx.pnt_to_rect(100, 100, 200, 120), black);
	agfx.draw_2colored_rectangle(sf, agfx.pnt_to_rect(210, 210, 300, 300), blue, red);

	agui.init(e, aevent);
	gui_button* b1 = agui.add_button(agfx.pnt_to_rect(520, 50, 700, 80), 100, "test button");
	gui_button* b2 = agui.add_button(agfx.pnt_to_rect(300, 50, 480, 80), 101, "töst zwei ;P");
	gui_text* t1 = agui.add_text("vera.ttf", 14, "test text", 0x000000, agfx.cord_to_pnt(100, 5), 102);

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

		// refresh every 1000/60 milliseconds (~ 60 fps)
		if(SDL_GetTicks() - refresh_time >= 1000/60) {
			e.draw();
			agui.draw();
			refresh_time = SDL_GetTicks();
		}
	}

	return 0;
}