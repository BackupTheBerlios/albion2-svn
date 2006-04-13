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

#include "main.h"

/*!
 * \mainpage
 *
 * \author flo
 *
 * \date December 2005
 *
 * Albion 2 Engine - Mapeditor
 */

int main(int argc, char *argv[])
{
	// initialize the engine
	e = new engine("../../data/");
	e->init();

	// init class pointers
	c = e->get_core();
	m = e->get_msg();
	aevent = e->get_event();
	agfx = e->get_gfx();
	s = new shader(e);
	sce = new scene(e, s);
	cam = new camera(e);
	agui = new gui(e);
	x = new xml(m);
	l = e->get_lua();
	sf = e->get_screen();
	model = new mdl(e, sce, cam);
	mg = new mgui(e, agui, model, sce);

	// initialize the a2e events
	aevent->init(ievent);
	aevent->load_keyset("DE");

	// initialize gui and chat sutff
	agui->init();

	// initialize the camera
	cam->set_position(0.0f, 0.0f, 0.0f);
	cam->set_rotation_speed(100.0f);
	cam->set_mouse_input(false);
	//cam->set_cam_speed(5.0f);

	// set the light
	light* l1 = new light(e, 0.0f, 80.0f, 0.0f);
	/*float lamb[] = { 0.12f, 0.12f, 0.12f, 1.0f};
	float ldif[] = { 0.9f, 0.9f, 0.9f, 1.0f};
	float lspc[] = { 1.0f, 1.0f, 1.0f, 1.0f};*/
	float lamb[] = { 0.5f, 0.5f, 0.5f, 1.0f};
	float ldif[] = { 0.9f, 0.9f, 0.9f, 1.0f};
	float lspc[] = { 1.0f, 1.0f, 1.0f, 1.0f};
	l1->set_lambient(lamb);
	l1->set_ldiffuse(ldif);
	l1->set_lspecular(lspc);
	sce->add_light(l1);
	// for testing purposes
	sce->set_light(false);

	// needed for fps counting
	unsigned int fps = 0;
	unsigned int fps_time = 0;
	char* tmp = new char[512];
	sprintf(tmp, "A2E Material Editor - v0.01");

	mg->load_main_gui();

	// main loop
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
				case SDL_MOUSEBUTTONDOWN:
					switch(aevent->get_event().button.button) {
						case SDL_BUTTON_RIGHT:
							cam->set_mouse_input(cam->get_mouse_input() ^ true);
							e->set_cursor_visible(e->get_cursor_visible() ^ true);
							break;
						default:
							break;
					}
					break;
				default:
					break;
			}
		}

		// print out the fps count
		fps++;
		if(SDL_GetTicks() - fps_time > 1000) {
			sprintf(tmp, "A2E Material Editor - v0.01 | FPS: %u | Pos: %f %f %f | Rot: %f %f %f", fps,
				-cam->get_position()->x, -cam->get_position()->y, -cam->get_position()->z,
				cam->get_rotation()->x, cam->get_rotation()->y, cam->get_rotation()->z);
			fps = 0;
			fps_time = SDL_GetTicks();
		}
		e->set_caption(tmp);

		// start drawing the scene
		e->start_draw();

		cam->run();
		sce->draw();
		if(!cam->get_mouse_input()) {
			agui->draw();
			mg->run();
		}

		e->stop_draw();
	}

	delete mg;

	delete [] tmp;

	delete sce;
	delete cam;
	delete s;
	delete agui;
	delete e;

	return 0;
}
