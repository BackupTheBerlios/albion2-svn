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

#include "model_loader.h"
/*#include <iostream>
#include <string.h>
#include <SDL_image.h>*/

/*!
 * \mainpage
 *
 * \author flo
 *
 * \date August 2004 - March 2005
 *
 * Albion 2 Engine Sample - Model Loader Sample
 */

int main(int argc, char *argv[])
{
	// initialize the engine
	e.init(800, 600, 24, false);
	e.set_caption("A2E Sample - Model Loader");

	// set a color scheme (blue)
	e.set_color_scheme(gui_style::BLUE);
	sf = e.get_screen();

	// initialize the a2e events
	aevent.init(ievent);
	aevent.set_keyboard_layout(event::DE);

	// initialize the camera
	cam.init(e, aevent);
	cam.set_position(0.0f, 0.0f, -30.0f);

	// load the model and set a new position
	plane.load_model("plane2.a2m");
	plane.set_position(0.0f, -15.0f, 0.0f);

	cube.load_model("cube.a2m");
	cube.set_position(0.0f, 10.0f, 0.0f);

	sphere.load_model("sphere.a2m");
	sphere.set_position(0.0f, 10.0f, 0.0f);
	sphere.set_radius(2.5f);

	sce.add_model(&plane);
	sce.add_model(&cube);
	sce.add_model(&sphere);

	light l1(-50.0f, 0.0f, -50.0f);
	float lamb[] = { 0.3f, 0.3f, 0.3f, 1.0f};
	float ldif[] = { 0.7f, 0.7f, 0.7f, 1.0f};
	float lspc[] = { 1.0f, 1.0f, 1.0f, 1.0f};
	l1.set_lambient(lamb);
	l1.set_ldiffuse(ldif);
	l1.set_lspecular(lspc);
	sce.add_light(&l1);

	// initialize ode
	// note: ode isn't implemented totally atm and
	// returns (many) errors, so don't use it ...
	o.init();

	// pass the models to ode
	o.add_object(&plane, true, ode_object::TRIMESH);
	o.add_object(&cube, false, ode_object::BOX);
	o.add_object(&sphere, false, ode_object::SPHERE);

	// needed for fps counting
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
						case SDLK_RETURN:
							break;
						case SDLK_w: {
							core::vertex3* lpos = l1.get_position();
							lpos->z += 1.0f;
							l1.set_position(lpos);
						}
						break;
						case SDLK_s: {
							core::vertex3* lpos = l1.get_position();
							lpos->z -= 1.0f;
							l1.set_position(lpos);
						}
						break;
						case SDLK_a: {
							core::vertex3* lpos = l1.get_position();
							lpos->x += 1.0f;
							l1.set_position(lpos);
						}
						break;
						case SDLK_d: {
							core::vertex3* lpos = l1.get_position();
							lpos->x -= 1.0f;
							l1.set_position(lpos);
						}
						break;
						case SDLK_q: {
							core::vertex3* lpos = l1.get_position();
							lpos->y += 1.0f;
							l1.set_position(lpos);
						}
						break;
						case SDLK_e: {
							core::vertex3* lpos = l1.get_position();
							lpos->y -= 1.0f;
							l1.set_position(lpos);
						}
						break;
						default:
						break;
					}
					break;
			}
		}

		// refresh every 1000/75 milliseconds (~ 75 fps)
		if(SDL_GetTicks() - refresh_time >= 1000/75) {
			cout << "inner loop - start" << endl;
			// print out the fps count
			fps++;
			if(SDL_GetTicks() - fps_time > 1000) {
				/*sprintf(tmp, "A2E Sample - Model Loader | FPS: %u | Pos: %f %f %f", fps,
					cam.get_position()->x, cam.get_position()->y, cam.get_position()->z);*/
				sprintf(tmp, "A2E Sample - Model Loader | FPS: %u", fps);
				fps = 0;
				fps_time = SDL_GetTicks();
			}
			e.set_caption(tmp);

			cout << "start drawing" << endl;
			e.start_draw();

			cout << "running cam" << endl;
			cam.run();
			cout << "drawing scene" << endl;
			sce.draw();
			cout << "running ode" << endl;
			o.run();

			cout << "stop drawing" << endl;
			e.stop_draw();
			cout << "end stop drawing" << endl;
			refresh_time = SDL_GetTicks();
			cout << "inner loop - stop" << endl;
		}
	}

	o.close();

	return 0;
}
