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
 * \date April - May 2005
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
	cout << "add size: " << 9+nm << endl;
	SDLNet_TCP_Send(n.clients[rcv_client].sock, data, 9+nm);

	delete data;
}

void handle_client(unsigned int cur_client) {
	char data[512];
	char ndata[512];
	char cdata[32];

	// checks if connection has been closed
	if(SDLNet_TCP_Recv(n.clients[cur_client].sock, data, 512) <= 0) {
		cout << "deleting routine ..." << endl;
        m.print(msg::MDEBUG, "world_server.cpp", "closing %s socket (%d)",
			n.clients[cur_client].is_active ? "active" : "inactive", cur_client);
		// send delete data to all clients
		if(n.clients[cur_client].is_active) {
			n.clients[cur_client].is_active = false;
			data[0] = net::DEL;
			data[1] = cur_client;
			for(unsigned int i = 0; i < MAX_CLIENTS; i++) {
				if(n.clients[i].is_active) {
					cout << "delete size: " << 2 << endl;
					SDLNet_TCP_Send(n.clients[i].sock, data, 2);
				}
			}
		}

		delete_player(cur_client);

		// close socket
		n.close_socket(n.clients[cur_client].sock);
		n.delete_client(cur_client);
	}
	else {
		cout << "receiving routine ..." << endl;
		cout << (unsigned int)(data[0] & 0xFF) << "-" << (unsigned int)(data[1] & 0xFF) << "-" << (unsigned int)(data[2] & 0xFF) << endl;
		switch(data[0]) {
			case net::NEW: {
				// active connection

				// send max client data (for the future maybe other stuff too)
				cdata[0] = net::CDAT;
				cdata[1] = MAX_CLIENTS;
				cdata[2] = (cplayers & 0xFF); // the clients number/id
				cout << "cdat size: " << 3 << endl;
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
				clients[cplayers].position->x = start_pos->x;
				clients[cplayers].position->y = start_pos->y + 2.0f;
				clients[cplayers].position->z = start_pos->z;
				clients[cplayers].rotation = 0.0f;
				clients[cplayers].status = 1;

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
						unsigned int len = (unsigned int)(data[2]*0xFF0000 + data[3]*0xFF00 + data[4]*0xFF + data[5]);
						ndata[2] = len & 0xFF000000;
						ndata[3] = len & 0xFF0000;
						ndata[4] = len & 0xFF00;
						ndata[5] = len & 0xFF;
						ndata[6] = cur_client;
						// put package data into new package (and into data that is printed out)
						char* message = new char[len+1];
						unsigned int i;
						for(i = 0; i < len; i++) {
							ndata[i+7] = data[i+7];
							message[i] = data[i+7];
						}
						ndata[i+7] = 0;
						message[len] = 0;

						// send package to all clients except the one who send the data
						cout << "dt_msg size: " << len+1 + 7 << endl;
						for(unsigned int i = 0; i < MAX_CLIENTS; i++) {
							if(n.clients[i].is_active && i != cur_client) {
								SDLNet_TCP_Send(n.clients[i].sock, ndata, len+1 + 7);
							}
						}

						// print out the received message
						m.print(msg::MDEBUG, "world_server.cpp", "received message from client %s (%u): %s",
							clients[cur_client].name, cur_client, message);
						delete message;

						break;
					}
					case DT_MOVE: {
						if(SDL_GetTicks() - clients[cur_client].walk_time >= min_walk_time) {
                            const dReal* clvel = dBodyGetLinearVel(ode_players[cur_client]->get_body());
							float xrot = clients[cur_client].rotation * piover180;
							float zrot = clients[cur_client].rotation * piover180;

							// data[2] = move type
							switch((data[3] & 0xFF)) {
								case MV_FORWARD: {
									xrot = sinf(xrot) * -max_force;
									zrot = cosf(zrot) * -max_force;
									dBodySetLinearVel(ode_players[cur_client]->get_body(), xrot, clvel[1], zrot);
									clients[cur_client].walk_time = SDL_GetTicks();
									break;
								}
								case MV_BACKWARD: {
									xrot = sinf(xrot) * max_force;
									zrot = cosf(zrot) * max_force;
									dBodySetLinearVel(ode_players[cur_client]->get_body(), xrot, clvel[1], zrot);
									clients[cur_client].walk_time = SDL_GetTicks();
									break;
								}
								default:
									break;
							}
						}
						break;
					}
					case DT_UPDATE: {
						cout << "update routine ..." << endl;
						// handle update data
						float rot = 0.0f;
						memcpy(&rot, &data[2], 4);
						clients[cur_client].rotation = rot;

						// make new data
						ndata[0] = net::DAT;
						ndata[1] = DT_UPDATE;
						ndata[2] = cur_client;
						memcpy(&ndata[3], &clients[cur_client].position->x, 4);
						memcpy(&ndata[3+4], &clients[cur_client].position->y, 4);
						memcpy(&ndata[3+8], &clients[cur_client].position->z, 4);
						memcpy(&ndata[3+12], &clients[cur_client].rotation, 4);

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
		if(clients[i].walk_time >= min_walk_time && clients[i].status != 0) {
			const dReal* clvel = dBodyGetLinearVel(ode_players[i]->get_body());
			dBodySetLinearVel(ode_players[i]->get_body(), 0.0f, clvel[1], 0.0f);
		}
	}

	// update player positions
	for(unsigned int i = 0; i < cplayers; ++i) {
		if(clients[i].status != 0) {
			const dReal* clpos = dBodyGetPosition(ode_players[i]->get_body());
			clients[i].position->x = clpos[0];
			clients[i].position->y = clpos[1];
			clients[i].position->z = clpos[2];
		}
	}
}

void check_events() {
	for(unsigned int i = 0; i < MAX_CLIENTS; i++) {
		if(SDLNet_SocketReady(n.clients[i].sock)) {
			/*unsigned int cnum = 0;
			for(unsigned int j = 0; j < MAX_CLIENTS; j++) {
				if(strcmp(n.clients[i].name, clients[j].name) == 0) {
					cnum = j;
					j = MAX_CLIENTS;
				}
			}
			cout << cnum << "-" << n.clients[i].name << "-" << clients[cnum].name << endl;*/
			handle_client(i);
		}
	}
}

a2emodel* add_model(char* name, vertex3* pos, vertex3* scale, bool fixed, ode_object::OTYPE type, float radius) {
	objects[cobjects] = new a2emodel();
	objects[cobjects]->load_model(name);
	objects[cobjects]->set_position(pos->x, pos->y, pos->z);
	objects[cobjects]->set_scale(scale->x, scale->y, scale->z);
	if(type == ode_object::SPHERE) {
		objects[cobjects]->set_radius(radius);
	}

	ode_objects[cobjects] = o.add_object(objects[cobjects], fixed, type);

	cobjects++;

	return objects[cobjects];
}

a2emodel* add_player(vertex3* pos) {
	players[cplayers] = new a2emodel();
	players[cplayers]->load_model("../data/player_sphere.a2m");
	players[cplayers]->set_position(pos->x, pos->y, pos->z);
	players[cplayers]->set_radius(2.0f);
	players[cplayers]->set_visible(true);

	ode_players[cplayers] = o.add_object(players[cplayers], false, ode_object::SPHERE);

	cplayers++;

	return players[cplayers];
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
	for(unsigned int i = num; i < (cplayers-1); i++) {
		ode_players[i] = ode_players[i+1];
	}
	ode_players[(cplayers-1)] = NULL;

	// reset client stuff
	clients[num].id = 0;
	sprintf(clients[num].name, "unknown");
	clients[num].status = 0;
	clients[num].position->x = start_pos->x;
	clients[num].position->y = start_pos->y + 2.0f;
	clients[num].position->z = start_pos->z;
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
		sprintf(clients[i].name, "unknown");
		clients[i].status = 0;
		clients[i].position = new vertex3();
		clients[i].rotation = 0.0f;
		clients[i].walk_time = 0;
	}

	// set start position for all players
	start_pos = new vertex3();
	start_pos->x = 0.0f;
	start_pos->y = 10.0f;
	start_pos->z = 0.0f;

	// init ode
	o.init();

	// add plane
	vertex3* pos = new vertex3();
	vertex3* scale = new vertex3();
	pos->x = 0.0f;
	pos->y = 0.0f;
	pos->z = 0.0f;
	scale->x = 0.5f;
	scale->y = 0.5f;
	scale->z = 0.5f;
	add_model("../data/move_level.a2m", pos, scale, true, ode_object::TRIMESH);
	delete pos;
	delete scale;

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
			/*if(cplayers != 0) {
				cout << "player 0 pos: " << players[0]->get_position()->x << ", "
					<< players[0]->get_position()->y << ", " << players[0]->get_position()->z
					<< " - rot: " << clients[0].rotation << endl;
			}*/
		}
	}

	n.exit();
	o.close();

	delete start_pos;

	return 0;
}
