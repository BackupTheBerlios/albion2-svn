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
 * Albion 2 Server
 */

int main(int argc, char *argv[])
{
	// initialize the engine
	e = new engine("../../data/");
	e->init(true);
	e->set_caption("A2 Server");

	// init class pointers
	c = e->get_core();
	m = e->get_msg();
	aevent = e->get_event();
	s = new shader(e);
	sce = new scene(e, s);
	n = new net(e);
	o = new ode(e);
	o->init();
	sn = new snet(e, n);

	// open map
	a2emap* map = new a2emap(e, sce, o);
	map->load_map(e->data_path("klouta.a2map"), true);

	// net init
	if(n->init()) {
		m->print(msg::MDEBUG, "main.cpp", "main(): net class successfully initialized!");
		if(n->create_server(e->get_server_data()->port, e->get_server_data()->max_clients)) {
			m->print(msg::MDEBUG, "main.cpp", "main(): successfully created server!");
			netinit = true;
		}
		else {
			m->print(msg::MERROR, "main.cpp", "main(): couldn't create server!");
			done = true;
		}
	}
	else {
		m->print(msg::MERROR, "main.cpp", "main(): couldn't initialize net class!");
		done = true;
	}


	// ode stuff
	unsigned int ode_timer = SDL_GetTicks();
	unsigned int ode_timer2 = 0;
	ode_timestep = 1000 / 40; // 1000 / updates per second

	while(!done)
	{
		while(aevent->is_event())
		{
			aevent->handle_events(aevent->get_event().type);
			switch(aevent->get_event().type) {
				case SDL_QUIT:
					done = true;
					break;
			}
		}

		SDLNet_CheckSockets(n->socketset, 0);
		if(SDLNet_SocketReady(n->tcp_server_sock)) {
			n->add_client();
		}
		n->handle_server();
		sn->manage_clients();

		// ode ...
		if(SDL_GetTicks() - ode_timer >= ode_timestep) {
			/*// if ode_timer would be updated after ode->run(), we would lose the time needed
			// by ode->run() (-> unequal time steps / "pausing/halting" of time during ode->run())
			ode_timer2 = ode_timer;
			ode_timer = SDL_GetTicks();
			o->run(SDL_GetTicks() - ode_timer2);*/

			// use a fixed value of 40 updates per second
			o->run(ode_timestep);
		}
	}

	o->close();

	return 0;
}
