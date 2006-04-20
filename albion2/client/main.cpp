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
	cs = new csystem(e);
	c = e->get_core();
	m = e->get_msg();
	aevent = e->get_event();
	agfx = e->get_gfx();
	s = new shader(e);
	sce = new scene(e, s);
	cam = new camera(e);
	agui = new gui(e);
	agui->init();
	cs->n = new net(e);
	cn = new cnet(e, cs);
	cg = new cgui(e, agui, cs, cn);
	cg->init();
	cm = new cmap(e, sce);

	// set a color scheme (blue)
	e->set_color_scheme(gui_style::BLUE);
	cs->sf = e->get_screen();

	// initialize the a2e events
	aevent->init(ievent);
	aevent->load_keyset("DE");


	// initialize the camera
	cam->set_position(-5.0f, -30.0f, -55.0f);
	cam->set_rotation_speed(100.0f);
	cam->set_mouse_input(false);

	//a2emap* map = new a2emap(e, sce, NULL);
	//map->load_map(e->data_path("klouta.a2map"), true);

	light* l1 = new light(e, 0.0f, 200.0f, 0.0f);
	float lamb1[] = { 0.22f, 0.22f, 0.22f, 1.0f};
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
				case SDL_KEYDOWN:
					switch(aevent->get_event().key.keysym.sym) {
						case SDLK_ESCAPE:
							cs->done = true;
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
			sprintf(tmp, "A2 Client | FPS: %u | Pos: %f %f %f", fps,
				cam->get_position()->x, cam->get_position()->y, cam->get_position()->z);
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
				cn->close();
			}

			// send test package every 25 seconds to preven disconnection
			// due to client inactivity (server side inactivity timeout is 30 sec)
			if(SDL_GetTicks() - act_time > 25000) {
				cn->send_packet(cnet::PT_TEST);
				act_time = SDL_GetTicks();
			}
		}

		e->start_draw();

		cg->run();
		agui->draw();
		cam->run();
		sce->draw();

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
