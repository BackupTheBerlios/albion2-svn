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
#include <iostream>

/*!
 * \mainpage
 *
 * \author flo
 *
 * \date July 2004
 *
 * Albion 2 Engine Sample - GUI
 */

int main(int argc, char *argv[])
{
	e.init(800, 600, 16, false);

	sf = e.get_screen();
	unsigned int white = SDL_MapRGB(sf->format, 255, 255, 255);
	g.draw_point(sf, 10, 10, white);
	g.draw_line(sf, 10, 20, 50, 40, white);
	g.draw_rectangle(sf, 100, 100, 200, 120, white);

	refresh_time = SDL_GetTicks();
	while(!done)
	{
		while(SDL_PollEvent(&event))
		{
			switch(event.type)
			{
				case SDL_QUIT:
					done = true;
					break;
				case SDL_KEYDOWN:
					switch(event.key.keysym.sym) {
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
			refresh_time = SDL_GetTicks();
		}
	}

	return 0;
}