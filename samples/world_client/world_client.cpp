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

#include "world_client.h"

/*!
 * \mainpage
 *
 * \author flo
 *
 * \date April 2005
 *
 * Albion 2 Engine Sample - World Client Sample
 */

int HandleServerData(char *data) {
	int used = 0;
	switch ((data[0] & 0xFF)) {
		case net::CDAT: {
			max_clients = (data[1] & 0xFF);
			client_num = (data[2] & 0xFF);

			players = new a2emodel[max_clients];

			// init clients
			clients = new client[max_clients];
			for(unsigned int i = 0; i < max_clients; i++) {
				clients[i].id = 0;
				clients[i].name = new char[32];
				sprintf(clients[i].name, "unknown");
				clients[i].status = 0;
				clients[i].position = new core::vertex3();
				clients[i].position->x = 0.0f;
				clients[i].position->y = 0.0f;
				clients[i].position->z = 0.0f;
				clients[i].rotation = 0.0f;
				clients[i].host = 0;
				clients[i].port = 0;

				players[i].load_model("../data/player.a2m");
				players[i].set_visible(false);
				sce.add_model(&players[i]);
			}
			used = 3;
			break;
		}
		case net::DAT: {
			switch((data[1] & 0xFF)) {
				case DT_MSG: {
					// get data package length
					unsigned int plen = (unsigned int)((data[2] & 0xFF)*0xFF0000 + (data[3] & 0xFF)*0xFF00 + (data[4] & 0xFF)*0xFF + (data[5] & 0xFF));
					char* message = new char[plen];
					unsigned int j;
					for(j = 0; j < plen; j++) {
						message[j] = (data[j+7] & 0xFF);
					}
					message[j] = 0;
					m.print(msg::MDEBUG, "world_client.cpp", "%s: %s", clients[(data[6] & 0xFF)].name, message);
					delete message;
					used = 7 + plen;
					break;
				}
				case DT_UPDATE: {
					unsigned int cnum = (data[2] & 0xFF);
					memcpy(&clients[cnum].position->x, &data[3], 4);
					memcpy(&clients[cnum].position->y, &data[3+4], 4);
					memcpy(&clients[cnum].position->z, &data[3+8], 4);
					memcpy(&clients[cnum].rotation, &data[3+12], 4);

					used = 3 + 16;
					break;
				}
				default:
					break;
			}
			break;
		}
		case net::ADD: {
			unsigned int cur_client;

			cur_client = (data[1] & 0xFF);
			if((cur_client >= max_clients) || clients[cur_client].status == 1) {
				// client doesn't exist / all client "ports" are in use -> break
				break;
			}

			// save client data
			unsigned int len = (unsigned int)(data[8] & 0xFF);
			memcpy(clients[cur_client].name, &data[9], len);
			clients[cur_client].name[len+1] = 0;
			clients[cur_client].status = 1;
			clients[cur_client].id = cur_client;
			clients[cur_client].host = (unsigned int)SDLNet_Read32(&data[2]);
			clients[cur_client].port = (unsigned int)SDLNet_Read16(&data[6]);

			// print out some data
			m.print(msg::MDEBUG, "world_client.cpp", "new client on %d from %d.%d.%d.%d:%d (%s)",
				cur_client, (clients[cur_client].host >> 24) & 0xFF,
				(clients[cur_client].host >> 16) & 0xFF,
				(clients[cur_client].host >> 8) & 0xFF,
				clients[cur_client].host & 0xFF,
				clients[cur_client].port, clients[cur_client].name);

			cplayers++;
			players[cur_client].set_visible(true);

			used = 9 + len;
			break;
		}
		case net::DEL: {
			unsigned int cur_client;

			// which client should be deleted?
			cur_client = (data[1] & 0xFF);
			if ((cur_client >= max_clients) || clients[cur_client].status != 1) {
				// client doesn't exist / all client "ports" are in use -> break
				break;
			}
			clients[cur_client].status = 0;

			// print out what happened
			m.print(msg::MDEBUG, "world_client.cpp", "lost client %d: %s", cur_client, clients[cur_client].name);
		}
		used = 2;
		break;
		case net::KICK: {
			// there are some errors with that, but i absolutely dunno y ...
			m.print(msg::MDEBUG, "world_client.cpp", "sorry, but the server is full!");
		}
		used = 1;
		break;
		default: {
			// unknown packet type
		}
		used = 0;
		break;
	}
	return used;
}

void HandleServer(void) {
	char* data = new char[512];
	int pos, len, used;

	// checks if client is still connected to the server
	len = SDLNet_TCP_Recv(n.tcp_ssock, (char*)data, 512);
	if (len <= 0) {
		n.close_socket(n.tcp_ssock);
	}
	else {
		pos = 0;
		while(len > 0) {
			used = HandleServerData(&data[pos]);
			pos += used;
			len -= used;
			if(used == 0) {
				// data was lost ...
				len = 0;
			}
		}
	}
	delete data;
}

void update() {
	// create package
	char* data = new char[7];
	data[0] = net::DAT;
	data[1] = DT_UPDATE;
	data[2] = client_num;
	memcpy(&data[3], &cam.get_rotation()->y, 4);

	// send data
	if(n.tcp_ssock != NULL) {
        SDLNet_TCP_Send(n.tcp_ssock, data, 7);
	}
	delete data;

	// update players data
	for(unsigned int i = 0; i < cplayers; i++) {
		players[i].set_position(clients[i].position->x, clients[i].position->y - 2.0f, clients[i].position->z);
		players[i].set_rotation(0.0f, clients[i].rotation - 90.0f, 0.0f);
	}

	// update the players cam
	cam.set_position(-clients[client_num].position->x - sinf(clients[client_num].rotation * piover180) * 15.0f,
		clients[client_num].position->y + 10.0f,
		-clients[client_num].position->z - cosf(clients[client_num].rotation * piover180) * 15.0f);
}

void move(MOVE_TYPE type) {
	// create package
	char* data = new char[4];

	data[0] = net::DAT;
	data[1] = DT_MOVE;
	data[2] = client_num;
	data[3] = type;

	// send data
	if(n.tcp_ssock != NULL) {
        SDLNet_TCP_Send(n.tcp_ssock, data, 4);
	}
	delete data;
}

int main(int argc, char *argv[])
{
	// initialize the engine
	e.init(800, 600, 24, false);
	e.set_caption("A2E Sample - World Client Sample");
	e.set_cursor_visible(false);

	// set a color scheme (blue)
	e.set_color_scheme(gui_style::BLUE);
	sf = e.get_screen();

	// initialize the a2e events
	aevent.init(ievent);
	aevent.set_keyboard_layout(event::DE);

	// initialize the camera
	cam.init(e, aevent);
	cam.set_position(0.0f, 2.0f, 0.0f);
	cam.set_cam_input(false);
	cam.set_rotation_speed(50.0f);

	// load the models and set new positions
	level.load_model("../data/move_level.a2m");

	sce.add_model(&level);

	light l1(-50.0f, 0.0f, -50.0f);
	float lamb[] = { 0.3f, 0.3f, 0.3f, 1.0f};
	float ldif[] = { 0.7f, 0.7f, 0.7f, 1.0f};
	float lspc[] = { 1.0f, 1.0f, 1.0f, 1.0f};
	l1.set_lambient(lamb);
	l1.set_ldiffuse(ldif);
	l1.set_lspecular(lspc);
	sce.add_light(&l1);

	// init network stuff
	if(n.init()) {
		m.print(msg::MDEBUG, "world_client.cpp", "net class initialized!");
		SDL_Delay(1000);
		// we just want a client program
		if(n.create_client("localhost", net::TCP, 1337, "user")) { // TODO: name should be chosable
			m.print(msg::MDEBUG, "world_client.cpp", "client created!");
			is_networking = true;
		}
		else {
			m.print(msg::MDEBUG, "world_client.cpp", "client couldn't be created!");
		}
	}
	else {
		m.print(msg::MDEBUG, "world_client.cpp", "net class couldn't be initialized!");
	}

	// needed for fps counting
	unsigned int fps = 0;
	unsigned int fps_time = 0;
	char* tmp = new char[512];
	sprintf(tmp, "A2E Sample - World Client Sample");

	refresh_time = SDL_GetTicks();
	// main loop
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
						case SDLK_UP:
							move(MV_FORWARD);
							break;
						case SDLK_DOWN:
							move(MV_BACKWARD);
							break;
						default:
						break;
					}
					break;
			}
		}

		if(is_networking) {
			// client stuff
			SDLNet_CheckSockets(n.socketset, 0);
			if(SDLNet_SocketReady(n.tcp_ssock)) {
				HandleServer();
			}

			if(n.tcp_ssock == NULL) {
				done = true;
			}
		}
		else {
			done = true;
		}

		// refresh every 1000/75 milliseconds (~ 75 fps)
		if(SDL_GetTicks() - refresh_time >= 1000/75) {
			// print out the fps count
			fps++;
			if(SDL_GetTicks() - fps_time > 1000) {
				sprintf(tmp, "A2E Sample - World Client Sample | FPS: %u | Pos: %f %f %f", fps,
					cam.get_position()->x, cam.get_position()->y, cam.get_position()->z);
				fps = 0;
				fps_time = SDL_GetTicks();
			}
			e.set_caption(tmp);

			// send new client data to server
			update();

			// start drawing the scene
			e.start_draw();

			cam.run();
			sce.draw();

			e.stop_draw();
			refresh_time = SDL_GetTicks();
		}
	}

	delete tmp;

	n.exit();

	for(unsigned int i = 0; i < max_clients; i++) {
		sce.delete_model(&players[i]);
	}

	delete clients;

	unsigned int blubb = 0;

	return 0;
}
