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
 * \date April - September 2005
 *
 * Albion 2 Engine Sample - World Server Sample
 */

void send_new_client(unsigned int snd_client, unsigned int rcv_client) {
	char* data = new char[512];

	unsigned int nm = (unsigned int)strlen((char*)n->clients[snd_client].name) + 1;
	data[0] = net::ADD;
	data[1] = snd_client;
	memcpy(&data[2], &n->clients[snd_client].ip.host, 4);
	memcpy(&data[6], &n->clients[snd_client].ip.port, 2);
	data[8] = nm;
	data[9+nm] = 0;
	memcpy(&data[9], n->clients[snd_client].name, nm);
	cout << "add size: " << 9+nm << endl;
	n->send_packet(&n->clients[rcv_client].sock, data, 9+nm, rcv_client);

	delete data;
}

void handle_client(unsigned int cur_client) {
	char data[512];
	char ndata[512];
	char cdata[32];

	// checks if connection has been closed
	if(n->recv_packet(&n->clients[cur_client].sock, data, 512, cur_client) <= 0) {
        m->print(msg::MDEBUG, "world_server.cpp", "closing %s socket (%d)",
			n->clients[cur_client].is_active ? "active" : "inactive", cur_client);
		// send delete data to all clients
		if(n->clients[cur_client].is_active) {
			n->clients[cur_client].is_active = false;
			data[0] = net::DEL;
			data[1] = cur_client;
			for(unsigned int i = 0; i < MAX_CLIENTS; i++) {
				if(n->clients[i].is_active) {
					cout << "delete size: " << 2 << endl;
					n->send_packet(&n->clients[i].sock, data, 2, i);
				}
			}

			delete_player(cur_client);
		}

		// close socket
		n->close_socket(n->clients[cur_client].sock);
		n->delete_client(cur_client);
	}
	else {
		switch(data[0]) {
			case net::NEW: {
				// active connection

				// send max client data (for the future maybe other stuff too)
				cdata[0] = net::CDAT;
				cdata[1] = MAX_CLIENTS;
				cdata[2] = (cplayers & 0xFF); // the clients number/id
				cout << "cdat size: " << 3 << endl;
				n->send_packet(&n->clients[cur_client].sock, cdata, 3, cur_client);

				// get client data
				memcpy(&n->clients[cur_client].ip.port, &data[1], 2);
				memcpy(&n->clients[cur_client].name, &data[4], 32);
				n->clients[cur_client].name[32] = 0;
				m->print(msg::MDEBUG, "world_server.cpp", "activating socket (%d: %s)",
					cur_client, n->clients[cur_client].name);
				// send data to all clients
				for(unsigned int i = 0; i < MAX_CLIENTS; i++) {
					if(n->clients[i].is_active) {
						send_new_client(cur_client, i);
					}
				}

				n->clients[cur_client].is_active = true;
				// send data of all clients to current client
				for(unsigned int i = 0; i < MAX_CLIENTS; i++) {
					if(n->clients[i].is_active) {
						send_new_client(i, cur_client);
					}
				}

				// actualize server data
				clients[cplayers].id = cplayers;
				memcpy(clients[cplayers].name, &n->clients[cur_client].name, 31);
				clients[cplayers].name[strlen(clients[cplayers].name)] = 0;
				clients[cplayers].position->x = start_pos->x;
				clients[cplayers].position->y = start_pos->y + 2.0f;
				clients[cplayers].position->z = start_pos->z;
				clients[cplayers].rotation = 0.0f;
				clients[cplayers].status = ST_ONLINE;

				// add player to physics engine object/player stack
				add_player(clients[cplayers].position);

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
							if(n->clients[i].is_active && i != cur_client) {
								n->send_packet(&n->clients[i].sock, ndata, len+1 + 7, i);
							}
						}

						// print out the received message
						m->print(msg::MDEBUG, "world_server.cpp", "received message from client %s (%u): %s",
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
							switch((data[2] & 0xFF)) {
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
					case DT_MOTION: {
						clients[cur_client].motion = (unsigned int)(data[2] & 0xFF);
						break;
					}
					case DT_UPDATE: {
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
						memcpy(&ndata[3+16], &clients[cur_client].motion, 4);

						// send data to all clients
						for(unsigned int i = 0; i < MAX_CLIENTS; i++) {
							if(n->clients[i].is_active) {
								n->send_packet(&n->clients[i].sock, ndata, 3 + 20, i);
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
				m->print(msg::MDEBUG, "world_server.cpp", "received a package with an unknown package type! type: %u", (unsigned int)(data[0] & 0xFF));
			}
			break;
		}
	}
}

void update_players() {
	// set velocity to 0 if the last time the player was walking
	// is at least _min_walk_time_ (100 ms) ago
	for(unsigned int i = 0; i < cplayers; i++) {
		if(clients[i].walk_time >= min_walk_time && clients[i].status != ST_OFFLINE) {
			const dReal* clvel = dBodyGetLinearVel(ode_players[i]->get_body());
			dBodySetLinearVel(ode_players[i]->get_body(), 0.0f, clvel[1], 0.0f);
		}
	}

	// update player positions
	for(unsigned int i = 0; i < cplayers; ++i) {
		if(clients[i].status != ST_OFFLINE) {
			const dReal* clpos = dBodyGetPosition(ode_players[i]->get_body());
			clients[i].position->x = clpos[0];
			clients[i].position->y = clpos[1];
			clients[i].position->z = clpos[2];
		}
	}
}

void check_events() {
	for(unsigned int i = 0; i < MAX_CLIENTS; i++) {
		if(SDLNet_SocketReady(n->clients[i].sock)) {
			handle_client(i);
		}
	}
}

a2emodel* add_model(char* name, vertex3* pos, vertex3* scale, bool fixed, ode_object::OTYPE type, float radius) {
	objects[cobjects] = new a2emodel(e, s);
	objects[cobjects]->load_model(name);
	objects[cobjects]->set_position(pos->x, pos->y, pos->z);
	objects[cobjects]->set_hard_scale(scale->x, scale->y, scale->z);
	if(type == ode_object::SPHERE) {
		objects[cobjects]->set_radius(radius);
	}

	ode_objects[cobjects] = o->add_object(objects[cobjects], fixed, type);

	cobjects++;

	return objects[cobjects];
}

a2emodel* add_player(vertex3* pos) {
	players[cplayers] = new a2emodel(e, s);
	players[cplayers]->load_model("../data/player_sphere.a2m");
	players[cplayers]->set_position(pos->x, pos->y, pos->z);
	players[cplayers]->set_radius(2.0f);
	players[cplayers]->set_visible(true);

	ode_players[cplayers] = o->add_object(players[cplayers], false, ode_object::SPHERE);

	cplayers++;

	return players[cplayers];
}

void delete_player(unsigned int num) {
	// delete player stuff
	delete players[num];
	for(unsigned int i = num; i < (MAX_CLIENTS-1); i++) {
		players[i] = players[i+1];
	}
	players[(MAX_CLIENTS-1)] = NULL;

	// delete ode player (object)
	unsigned int oo_num = 0;
	for(unsigned int i = 0; i < o->get_object_count(); i++) {
		if(o->get_ode_object(i) == ode_players[num]) {
			oo_num = i;
		}
	}
	o->delete_object(oo_num);
	for(unsigned int i = num; i < (MAX_CLIENTS-1); i++) {
		ode_players[i] = ode_players[i+1];
	}
	ode_players[(MAX_CLIENTS-1)] = NULL;

	// reset client stuff
	clients[num].id = 0;
	sprintf(clients[num].name, "unknown");
	clients[num].status = ST_OFFLINE;
	clients[num].position->x = start_pos->x;
	clients[num].position->y = start_pos->y + 2.0f;
	clients[num].position->z = start_pos->z;
	clients[num].rotation = 0.0f;
	clients[num].walk_time = 0;

	clients[(MAX_CLIENTS-1)] = clients[num];
	for(unsigned int i = num; i < (MAX_CLIENTS-1); i++) {
		clients[i] = clients[i+1];
	}

	// decrease player count
	cplayers--;
}

void handle_server() {
	TCPsocket tmpsock;
	unsigned int cur_client = 0;
	char* data = new char[1];

	tmpsock = SDLNet_TCP_Accept(n->tcp_ssock);
	if(tmpsock == NULL) {
		return;
	}

	// checks if there are any unconnected clients
	for(cur_client = 0; cur_client < MAX_CLIENTS; ++cur_client) {
		if(n->clients[cur_client].sock == NULL) {
			break;
		}
	}

	if(cur_client == MAX_CLIENTS) {
		// checks if there is any inactive client
		for(cur_client = 0; cur_client < MAX_CLIENTS; ++cur_client) {
			if(n->clients[cur_client].sock && !n->clients[cur_client].is_active) {
				// kick this client
				data[0] = net::KICK;
				n->send_packet(&n->clients[cur_client].sock, data, 1, cur_client);
				SDLNet_TCP_DelSocket(n->socketset, n->clients[cur_client].sock);
				SDLNet_TCP_Close(n->clients[cur_client].sock);
				m->print(msg::MDEBUG, "world_server.cpp", "inactive client (%d) was kicked", cur_client);
				break;
			}
		}
	}

	if(cur_client == MAX_CLIENTS) {
		// the server doesn't permit any further connections
		data[0] = net::KICK;
		n->send_packet(&tmpsock, data, 1, 0xFFFFFFFF);
		SDLNet_TCP_Close(tmpsock);
		m->print(msg::MDEBUG, "world_server.cpp", "connection refused: server doesn't permit any further connections");
	}
	else {
		// add socket to socketset and make it inactive
		n->clients[cur_client].sock = tmpsock;
		n->clients[cur_client].ip = *SDLNet_TCP_GetPeerAddress(tmpsock);
		SDLNet_TCP_AddSocket(n->socketset, n->clients[cur_client].sock);
		m->print(msg::MDEBUG, "world_server.cpp", "a new inactive socket (%d) was added", cur_client);
	}
}

void init() {
	f->open_file("server.dat", false);
	char fline[256];
	bool end = false;

	while(!end) {
		f->get_line(fline);

		// file end reached?
		if(strcmp(fline, "[EOF]") == 0) {
			end = true;
		}
		else {
			// otherwise we load data
			unsigned int x = 0;
			char* fline_tok[8];
			fline_tok[x] = strtok(fline, "=");
			while(fline_tok[x] != NULL) {
				x++;
				fline_tok[x] = strtok(NULL, "=");
			}

			if(fline_tok[0]) {
				// get server port
				if(strcmp(fline_tok[0], "port") == 0) {
					port = atoi(fline_tok[1]);
				}
				// get max players
				else if(strcmp(fline_tok[0], "max_players") == 0) {
					max_players = atoi(fline_tok[1]);
				}
				// get max objects
				else if(strcmp(fline_tok[0], "max_objects") == 0) {
					max_objects = atoi(fline_tok[1]);
				}
				// get netlog
				else if(strcmp(fline_tok[0], "netlog") == 0) {
					if(strcmp(fline_tok[1], "0") == 0) {
						netlog = false;
					}
					else {
						netlog = true;
					}
				}
			}
		}
	}
	f->close_file();
}

int main(int argc, char *argv[])
{
	// initialize the engine
	e = new engine();
	e->init(true);

	// init class pointers
	c = e->get_core();
	m = e->get_msg();
	f = e->get_file_io();
	s = new shader(e);
	o = new ode(e);
	n = new net(e);

	// initialize everything (and load the settings)
	init();

	// allocate memory for objects and players
	players = new a2emodel*[max_players];
	objects = new a2emodel*[max_objects];
	ode_players = new ode_object*[max_players];
	ode_objects = new ode_object*[max_objects];

	// set net logging
	n->set_netlog(netlog);

	// init clients
	clients = new client[max_players];
	for(unsigned int i = 0; i < max_players; i++) {
		clients[i].id = 0;
		clients[i].name = new char[32];
		sprintf(clients[i].name, "unknown");
		clients[i].status = ST_OFFLINE;
		clients[i].motion = MT_STANDING;
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
	o->init();

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
	if(n->init()) {
		cout << "net class initialized" << endl;
		if(n->create_server(net::TCP, port, max_players)) {
			cout << "server created" << endl;
			is_networking = true;
		}
		else {
			cout << "couldn't create server" << endl;
		}
	}
	else {
		cout << "couldn't initialize net class" << endl;
	}

	refresh_time = SDL_GetTicks();
	// main loop
	while(!done) {
		if(is_networking) {
			// server stuff
			SDLNet_CheckSockets(n->socketset, ~0);

			if(SDLNet_SocketReady(n->tcp_ssock)) {
				n->handle_server();
			}

			check_events();
		}
		else {
			done = true;
		}

		// refresh ode every 1000/40 milliseconds (~ 40 fps)
		if(SDL_GetTicks() - refresh_time >= 1000/40) {
			// run ode
			o->run(SDL_GetTicks() - refresh_time);
			refresh_time = SDL_GetTicks();

			update_players();
		}
	}

	n->exit();
	o->close();

	delete start_pos;
	delete n;
	delete o;
	delete s;
	delete [] players;
	delete [] objects;
	delete [] ode_players;
	delete [] ode_objects;
	delete [] clients;
	delete e;

	return 0;
}
