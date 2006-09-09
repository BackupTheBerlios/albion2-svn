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
 * \date April 2006
 *
 * Albion 2 Client
 */

int main(int argc, char *argv[])
{
	// initialize the engine
	e = new engine("../../data/");
	e->init();
	e->set_caption("A2 Client");

	// init class pointers
	c = e->get_core();
	m = e->get_msg();
	aevent = e->get_event();
	agfx = e->get_gfx();
	s = new shader(e);
	sce = new scene(e, s);
	cam = new camera(e);
	agui = new gui(e, s);
	agui->init();
	cm = new cmap(e, sce);
	cs = new csystem(e, cam, cm);
	cs->sf = e->get_screen();
	cs->n = new net(e);
	cs->agui = agui;
	cg = new cgui(e, cs);
	cg->init();
	cn = new cnet(e, cs, cg);

	// initialize the a2e events
	aevent->init(ievent);

	// initialize the camera
	cam->set_position(0.0f, 0.0f, 0.0f);
	cam->set_rotation_speed(100.0f);
	cam->set_mouse_input(false);
	cam->set_cam_input(false);

	light* l1 = new light(e, 0.0f, 80.0f, 0.0f);
	float lamb1[] = { 0.12f, 0.12f, 0.12f, 1.0f};
	float ldif1[] = { 0.9f, 0.9f, 0.9f, 1.0f};
	float lspc1[] = { 1.0f, 1.0f, 1.0f, 1.0f};
	l1->set_lambient(lamb1);
	l1->set_ldiffuse(ldif1);
	l1->set_lspecular(lspc1);
	sce->add_light(l1);


	cs->init_net();


	// needed for fps counting
	unsigned int fps = 0;
	unsigned int fps_time = 0;
	char tmp[512];
	for(int i = 0; i < 512; i++) {
		tmp[i] = 0;
	}

	int act_skts;
	unsigned int act_time = SDL_GetTicks();
	while(!cs->done)
	{
		while(aevent->is_event())
		{
			aevent->handle_events(aevent->get_event().type);
			switch(aevent->get_event().type) {
				case SDL_QUIT:
					cs->done = true;
					break;
				case SDL_MOUSEBUTTONDOWN:
					switch(aevent->get_event().button.button) {
						case SDL_BUTTON_RIGHT:
							cam->set_mouse_input(cam->get_mouse_input() ^ true);
							break;
						default:
						break;
					}
					break;
				case SDL_KEYDOWN:
					switch(aevent->get_event().key.keysym.sym) {
						case SDLK_ESCAPE:
							cs->done = true;
							break;
						case SDLK_w:
						case SDLK_UP:
							cs->move_forward = true;
							break;
						case SDLK_s:
						case SDLK_DOWN:
							cs->move_back = true;
							break;
						default:
						break;
					}
					break;
				case SDL_KEYUP:
					switch(aevent->get_event().key.keysym.sym) {
						case SDLK_w:
						case SDLK_UP:
							cs->move_forward = false;
							break;
						case SDLK_s:
						case SDLK_DOWN:
							cs->move_back = false;
							break;
						default:
						break;
					}
					break;
					break;
			}
		}

		// print out the fps count
		fps++;
		if(SDL_GetTicks() - fps_time > 1000) {
			sprintf(tmp, "A2 Client | FPS: %u | Pos: %f %f %f | Rot: %f %f %f", fps,
				-cam->get_position()->x, -cam->get_position()->y, -cam->get_position()->z,
				cam->get_rotation()->x, cam->get_rotation()->y, cam->get_rotation()->z);
			fps = 0;
			fps_time = SDL_GetTicks();
		}
		e->set_caption(tmp);

		if(cs->new_client) {
			cn->send_packet(cnet::PT_NEW_CLIENT);
			cs->new_client = false;
		}

		// client stuff
		if(cs->netinit) {
			act_skts = SDLNet_CheckSockets(cs->n->socketset, 0);
			if(act_skts == -1 || act_skts > 2) {
				m->print(msg::MDEBUG, "main.cpp", "main(): server/client socket error (%d) (-> program termination)!", act_skts);
				//cs->done = true;
				cs->netinit = false;
				cs->disconnected = true;
				cs->logged_in = false;
				cn->close();
			}
			if(SDLNet_SocketReady(cs->n->tcp_server_sock)) {
				cn->handle_server();
			}
			if(cs->n->tcp_server_sock == NULL) {
				m->print(msg::MDEBUG, "main.cpp", "main(): server socket has been set to NULL (-> program termination)!");
				//cs->done = true;
				cs->netinit = false;
				cs->disconnected = true;
				cs->logged_in = false;
				cn->close();
			}

			// send test package every 25 seconds to preven disconnection
			// due to client inactivity (server side inactivity timeout is 30 sec)
			if(SDL_GetTicks() - act_time > 25000) {
				cn->send_packet(cnet::PT_TEST);
				act_time = SDL_GetTicks();
			}

			cn->run();
			cs->run();
		}

		e->start_draw();

		cam->run();
		sce->draw();
		cg->run();
		agui->draw();

		e->stop_draw();

		if(cs->done && cs->netinit && cs->n->tcp_server_sock != NULL) {
			cn->send_packet(cnet::PT_QUIT_CLIENT);
		}
	}

	// delete everything
/*	delete o;
	delete sce;
	delete scale_mat;
	delete ground_mat;
	delete cc_mat;
	delete l1;
//	delete l2;
	delete s;
	delete cam;
	delete e;*/

	return 0;
}
