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

#include "world_server.h"

/*!
 * \mainpage
 *
 * \author flo
 *
 * \date April 2005
 *
 * Albion 2 Engine Sample - World Server Sample
 */

void send_new_client(unsigned int snd_client, unsigned int rcv_client) {
	char* data = new char[512];

	unsigned int nm = (unsigned int)strlen((char*)n.clients[snd_client].name) + 1;
	data[0] = net::ADD;
	data[1] = snd_client;
	memcpy(&data[2], &n.clients[snd_client].ip.host, 4);
	memcpy(&data[6], &n.clients[snd_client].ip.port, 2);
	data[8] = nm;
	data[9+nm] = 0;
	memcpy(&data[9], n.clients[snd_client].name, nm);
	SDLNet_TCP_Send(n.clients[rcv_client].sock, data, 9+nm);

	delete data;
}

void handle_client(unsigned int cur_client) {
	char data[512];
	char ndata[512];
	char cdata[32];

	// checks if connection has been closed
	if(SDLNet_TCP_Recv(n.clients[cur_client].sock, data, 512) <= 0) {
        m.print(msg::MDEBUG, "world_server.cpp", "closing %s socket (%d)",
			n.clients[cur_client].is_active ? "active" : "inactive", cur_client);
		// send delete data to all clients
		if(n.clients[cur_client].is_active) {
			n.clients[cur_client].is_active = false;
			data[0] = net::DEL;
			data[1] = cur_client;
			for(unsigned int i = 0; i < MAX_CLIENTS; i++) {
				if(n.clients[i].is_active) {
					SDLNet_TCP_Send(n.clients[i].sock, data, 2);
				}
			}
		}

		delete_player(cur_client);

		// close socket
		n.close_socket(n.clients[cur_client].sock);
	}
	else {
		switch(data[0]) {
			case net::NEW: {
				// active connection

				// send max client data (for the future maybe other stuff too)
				cdata[0] = net::CDAT;
				cdata[1] = MAX_CLIENTS;
				cdata[2] = (cplayers & 0xFF); // the clients number/id
				SDLNet_TCP_Send(n.clients[cur_client].sock, cdata, 3);

				// get client data
				memcpy(&n.clients[cur_client].ip.port, &data[1], 2);
				memcpy(&n.clients[cur_client].name, &data[4], 32);
				n.clients[cur_client].name[32] = 0;
				m.print(msg::MDEBUG, "world_server.cpp", "activating socket (%d: %s)",
					cur_client, n.clients[cur_client].name);
				// send data to all clients
				for(unsigned int i = 0; i < MAX_CLIENTS; i++) {
					if(n.clients[i].is_active) {
						send_new_client(cur_client, i);
					}
				}

				n.clients[cur_client].is_active = true;
				// send data of all clients to current client
				for(unsigned int i = 0; i < MAX_CLIENTS; i++) {
					if(n.clients[i].is_active) {
						send_new_client(i, cur_client);
					}
				}

				// actualize server data
				clients[cplayers].id = cplayers;
				memcpy(clients[cplayers].name, &n.clients[cur_client].name, 31);
				clients[cplayers].name[strlen(clients[cplayers].name)] = 0;
				clients[cplayers].position->x = 0.0f;
				clients[cplayers].position->y = 2.0f;
				clients[cplayers].position->z = 0.0f;
				clients[cplayers].rotation = 0.0f;
				clients[cplayers].status = 0;

				// add player to physics engine object/player stack
				add_player(clients[cplayers].position);

				// -- not needed -- TODO: send new client data to other clients and send clients data to the new client
                break;
			}
			case net::DAT: {
				switch(data[1]) {
					case DT_MSG: {
						// set package type
						ndata[0] = net::DAT;
						ndata[1] = DT_MSG;
						// get data package length
						unsigned int len = (unsigned int)(data[1]*0xFF0000 + data[2]*0xFF00 + data[3]*0xFF + data[4]);
						ndata[2] = len & 0xFF000000;
						ndata[3] = len & 0xFF0000;
						ndata[4] = len & 0xFF00;
						ndata[5] = len & 0xFF;
						ndata[6] = cur_client;
						// put package data into new package (and into data that is printed out)
						unsigned int i;
						for(i = 0; i < len; i++) {
							ndata[i+7] = data[i+7];
						}
						ndata[i+7] = 0;

						// send package to all clients except the one who send the data
						for(unsigned int i = 0; i < MAX_CLIENTS; i++) {
							if(n.clients[i].is_active && i != cur_client) {
								SDLNet_TCP_Send(n.clients[i].sock, ndata, len+1 + 7);
							}
						}
						break;
					}
					case DT_MOVE: {
						if(SDL_GetTicks() - clients[(data[2] & 0xFF)].walk_time >= min_walk_time) {
							// data[2] = player num / id
                            const dReal* clvel = dBodyGetLinearVel(ode_players[(data[2] & 0xFF)]->get_body());
							float xrot = clients[(data[2] & 0xFF)].rotation * piover180;
							float zrot = clients[(data[2] & 0xFF)].rotation * piover180;

							// data[3] = move type
							switch((data[3] & 0xFF)) {
								case MV_FORWARD: {
									xrot = sinf(xrot) * -max_force;
									zrot = cosf(zrot) * -max_force;
									dBodySetLinearVel(ode_players[(data[2] & 0xFF)]->get_body(), xrot, clvel[1], zrot);
									clients[(data[2] & 0xFF)].walk_time = SDL_GetTicks();
									break;
								}
								case MV_BACKWARD: {
									xrot = sinf(xrot) * max_force;
									zrot = cosf(zrot) * max_force;
									dBodySetLinearVel(ode_players[(data[2] & 0xFF)]->get_body(), xrot, clvel[1], zrot);
									clients[(data[2] & 0xFF)].walk_time = SDL_GetTicks();
									break;
								}
								default:
									break;
							}
						}
						break;
					}
					case DT_UPDATE: {
						// handle update data
						unsigned int cnum = (data[2] & 0xFF);
						float rot = 0.0f;
						memcpy(&rot, &data[3], 4);
						clients[cnum].rotation = rot;

						// make new data
						ndata[0] = net::DAT;
						ndata[1] = DT_UPDATE;
						ndata[2] = cnum;
						memcpy(&ndata[3], &clients[cnum].position->x, 4);
						memcpy(&ndata[3+4], &clients[cnum].position->y, 4);
						memcpy(&ndata[3+8], &clients[cnum].position->z, 4);
						memcpy(&ndata[3+12], &clients[cnum].rotation, 4);

						// send data to all clients
						for(unsigned int i = 0; i < MAX_CLIENTS; i++) {
							if(n.clients[i].is_active) {
								SDLNet_TCP_Send(n.clients[i].sock, ndata, 3 + 16);
							}
						}
						break;
					}
					default:
						break;
				}
				break;
			}

			default: {
				// unknown packet type
				m.print(msg::MDEBUG, "world_server.cpp", "a package with an unknown package type was send! type: %u", data[0]);
			}
			break;
		}
	}
}

void update_players() {
	// set velocity to 0 if the last time the player was walking
	// is at least _min_walk_time_ (100 ms) ago
	for(unsigned int i = 0; i < cplayers; i++) {
		if(clients[i].walk_time >= min_walk_time) {
			const dReal* clvel = dBodyGetLinearVel(ode_players[i]->get_body());
			dBodySetLinearVel(ode_players[i]->get_body(), 0.0f, clvel[1], 0.0f);
		}
	}

	// update player positions
	for(unsigned int i = 0; i < cplayers; ++i) {
		const dReal* clpos = dBodyGetPosition(ode_players[i]->get_body());
		clients[i].position->x = clpos[0];
		clients[i].position->y = clpos[1];
		clients[i].position->z = clpos[2];
	}
}

void check_events() {
	for(unsigned int i = 0; i < MAX_CLIENTS; ++i) {
		if(SDLNet_SocketReady(n.clients[i].sock)) {
			handle_client(i);
		}
	}
}

void add_model(char* name, core::vertex3* pos, bool fixed, ode_object::OTYPE type, float radius) {
	objects[cobjects] = new a2emodel();
	objects[cobjects]->load_model(name);
	objects[cobjects]->set_position(pos->x, pos->y, pos->z);
	if(type == ode_object::SPHERE) {
		objects[cobjects]->set_radius(radius);
	}

	ode_objects[cobjects] = o.add_object(objects[cobjects], fixed, type);

	cobjects++;
}

void add_player(core::vertex3* pos) {
	players[cplayers] = new a2emodel();
	players[cplayers]->load_model("../data/player_sphere.a2m");
	players[cplayers]->set_position(pos->x, pos->y, pos->z);
	players[cplayers]->set_radius(2.0f);
	players[cplayers]->set_visible(true);

	ode_players[cplayers] = o.add_object(players[cplayers], false, ode_object::SPHERE);

	cplayers++;
}

void delete_player(unsigned int num) {
	// delete player stuff
	delete players[num];
	for(unsigned int i = num; i < (cplayers-1); i++) {
		players[i] = players[i+1];
	}
	players[(cplayers-1)] = NULL;

	// delete ode player (object)
	unsigned int oo_num = 0;
	for(unsigned int i = 0; i < o.get_object_count(); i++) {
		if(o.get_ode_object(i) == ode_players[num]) {
			oo_num = i;
		}
	}
	o.delete_object(oo_num);

	// reset client stuff
	clients[num].id = 0;
	sprintf(clients[num].name, "unknown");
	clients[num].status = 0;
	clients[num].position->x = 0.0f;
	clients[num].position->y = 2.0f;
	clients[num].position->z = 0.0f;
	clients[num].rotation = 0.0f;
	clients[num].walk_time = 0;

	client* ctmp = &clients[num];
	for(unsigned int i = num; i < (cplayers-1); i++) {
		clients[i] = clients[i+1];
	}
	clients[(cplayers-1)] = *ctmp;

	// decrease player count
	cplayers--;
}

int main(int argc, char *argv[])
{
	// init clients
	clients = new client[MAX_CLIENTS];
	for(unsigned int i = 0; i < MAX_CLIENTS; i++) {
		clients[i].id = 0;
		clients[i].name = new char[32];
		clients[i].status = 0;
		clients[i].position = new core::vertex3();
		clients[i].rotation = 0.0f;
		clients[i].walk_time = 0;
	}

	// init ode
	o.init();

	// add plane
	core::vertex3* pos = new core::vertex3();
	pos->x = 0.0f;
	pos->y = 0.0f;
	pos->z = 0.0f;
	add_model("../data/move_level.a2m", pos, true, ode_object::TRIMESH);
	delete pos;

	// init network stuff
	bool is_networking = false;
	if(n.init()) {
		cout << "net class initialized" << endl;
		if(n.create_server(net::TCP, PORT, MAX_CLIENTS)) {
			cout << "server created" << endl;
			is_networking = true;
		}
	}

	refresh_time = SDL_GetTicks();
	// main loop
	while(!done) {
		if(is_networking) {
			// server stuff
			SDLNet_CheckSockets(n.socketset, ~0);

			if(SDLNet_SocketReady(n.tcp_ssock)) {
				n.handle_server();
			}

			check_events();
		}
		else {
			done = true;
		}

		// refresh ode every 1000/40 milliseconds (~ 40 fps)
		if(SDL_GetTicks() - refresh_time >= 1000/40) {
			// run ode
			o.run(SDL_GetTicks() - refresh_time);
			refresh_time = SDL_GetTicks();

			update_players();
			if(cplayers != 0) {
				cout << "player 0 pos: " << players[0]->get_position()->x << ", "
					<< players[0]->get_position()->y << ", " << players[0]->get_position()->z
					<< " - rot: " << clients[0].rotation << endl;
			}
		}
	}

	n.exit();
	o.close();

	return 0;
}
