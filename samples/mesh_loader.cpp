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

#include "mesh_loader.h"
#include <iostream>
#include <string.h>

/*!
 * \mainpage
 *
 * \author flo
 *
 * \date August 2004
 *
 * Albion 2 Engine Sample - Mesh Loader Sample
 */

int main(int argc, char *argv[])
{
	e.init(800, 600, 32, false);
	e.set_caption("A2E Sample - Mesh Loader");
	
	e.set_color_scheme(gui_style::BLUE);
	sf = e.get_screen();

	aevent.init(ievent);
	agui.init(e, aevent);
	aevent.set_keyboard_layout(event::DE);

	mesh.load_mesh("text.a2m");

	unsigned int fps = 0;
	unsigned int fps_time = 0;
	char tmp[512];
	for(int i = 0; i < 512; i++) {
		tmp[i] = 0;
	}

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

		// refresh every 1000/75 milliseconds (~ 75 fps)
		if(SDL_GetTicks() - refresh_time >= 1000/75) {
			fps++;
			if(SDL_GetTicks() - fps_time > 1000) {
				sprintf(tmp, "A2E Sample - Mesh Loader | fps: %u", fps);
				fps = 0;
				fps_time = SDL_GetTicks();
			}
			e.set_caption(tmp);

			e.start_draw();
			mesh.draw_mesh();
			agui.draw();
			e.stop_draw();
			refresh_time = SDL_GetTicks();
		}
	}

	return 0;
}