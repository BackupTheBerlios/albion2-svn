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

/*!
 * \mainpage
 *
 * \author flo
 *
 * \date August 2004 - April 2005
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
	cam.set_position(-5.0f, 30.0f, -55.0f);

	// load the model and set a new position
	plane.load_model("../data/ground.a2m");
	plane.set_position(0.0f, -13.0f, 0.0f);
	plane.set_draw_wireframe(wireframe);

	cottage.load_model("../data/celtic_cottage.a2m");
	cottage.set_position(0.0f, 0.0f, 0.0f);
	cottage.set_draw_wireframe(wireframe);

	sphere.load_model("../data/player_sphere.a2m");
	sphere.set_position(45.0f, 20.0f, 20.0f);
	sphere.set_radius(1.0f);

	sce.add_model(&plane);
	sce.add_model(&cottage);
	sce.add_model(&sphere);

	light l1(-50.0f, 100.0f, -50.0f);
	light l2(0.0f, 50.0f, 0.0f);
	float lamb1[] = { 0.3f, 0.3f, 0.3f, 1.0f};
	float ldif1[] = { 0.7f, 0.7f, 0.7f, 1.0f};
	float lspc1[] = { 1.0f, 1.0f, 1.0f, 1.0f};
	float lamb2[] = { 0.0f, 0.1f, 0.3f, 0.5f};
	float ldif2[] = { 0.1f, 0.3f, 0.5f, 0.5f};
	float lspc2[] = { 0.0f, 0.2f, 0.6f, 0.5f};
	l1.set_lambient(lamb1);
	l1.set_ldiffuse(ldif1);
	l1.set_lspecular(lspc1);
	l2.set_lambient(lamb2);
	l2.set_ldiffuse(ldif2);
	l2.set_lspecular(lspc2);
	sce.add_light(&l1);
	sce.add_light(&l2);

	// initialize ode
	o.init();

	// pass the models to ode
	o.add_object(&plane, true, ode_object::TRIMESH);
	o.add_object(&sphere, false, ode_object::SPHERE);

	ode_object* osphere = o.get_ode_object(1);

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
						case SDLK_r: {
							osphere->reset();
							dGeomSetPosition(osphere->get_geom(), 45.0f, 20.0f, 20.0f);
						}
						break;
						case SDLK_e: {
							wireframe = wireframe ^ true;
							plane.set_draw_wireframe(wireframe);
							cottage.set_draw_wireframe(wireframe);
						}
						break;
						case SDLK_w: {
							dBodyAddForce(osphere->get_body(), 5.0f, 1.0f, 0.0f);
						}
						break;
						case SDLK_s: {
							dBodyAddForce(osphere->get_body(), -5.0f, 1.0f, 0.0f);
						}
						break;
						case SDLK_a: {
							dBodyAddForce(osphere->get_body(), 0.0f, 1.0f, 5.0f);
						}
						break;
						case SDLK_d: {
							dBodyAddForce(osphere->get_body(), 0.0f, 1.0f, -5.0f);
						}
						break;
						default:
						break;
					}
					break;
			}
		}

		// print out the fps count
		fps++;
		if(SDL_GetTicks() - fps_time > 1000) {
			sprintf(tmp, "A2E Sample - Model Loader | FPS: %u | Pos: %f %f %f", fps,
				-cam.get_position()->x, cam.get_position()->y, -cam.get_position()->z);
			//sprintf(tmp, "A2E Sample - Model Loader | FPS: %u", fps);
			fps = 0;
			fps_time = SDL_GetTicks();
		}
		e.set_caption(tmp);

		e.start_draw();

		cam.run();
		sce.draw();
		o.run(SDL_GetTicks() - refresh_time);

		e.stop_draw();
		refresh_time = SDL_GetTicks();
	}

	o.close();

	return 0;
}
