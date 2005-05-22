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
 * \date April - May 2005
 *
 * Albion 2 Engine Sample - World Client Sample
 */

int handle_server_data(char *data) {
	int used = 0;
	switch ((data[0] & 0xFF)) {
		case net::CDAT: {
			if(!is_initialized) {
				max_clients = (data[1] & 0xFF);
				client_num = (data[2] & 0xFF);

				is_initialized = true;

				// init clients
				clients = new client[max_clients];
				for(unsigned int i = 0; i < max_clients; i++) {
					clients[i].id = 0;
					clients[i].name = new char[32];
					sprintf(clients[i].name, "unknown");
					clients[i].status = 0;
					clients[i].position = new vertex3();
					clients[i].position->x = 0.0f;
					clients[i].position->y = 0.0f;
					clients[i].position->z = 0.0f;
					clients[i].rotation = 0.0f;
					clients[i].host = 0;
					clients[i].port = 0;
					clients[i].text_point = new core::pnt();
					clients[i].text_point->x = 10;
					clients[i].text_point->y = 10;
					clients[i].text = agui.add_text("vera.ttf", 12, "-", 0x000000, clients[i].text_point, i, 0);
					// we don't want to show up the text already
					clients[i].text->set_notext();

					clients[i].model = NULL;
				}
				cout << "rcv cdat size: " << 3 << endl;
				used = 3;
			}
			else {
				m.print(msg::MDEBUG, "world_client.cpp", "network and clients are already initialized");
				used = 3;
			}
			break;
		}
		case net::DAT: {
			switch((data[1] & 0xFF)) {
				case DT_MSG: {
					// get data package length
					unsigned int plen = (unsigned int)((data[2] & 0xFF)*0xFF0000 + (data[3] & 0xFF)*0xFF00 + (data[4] & 0xFF)*0xFF + (data[5] & 0xFF));
					char* message = new char[plen+1];
					unsigned int j;
					for(j = 0; j < plen; j++) {
						message[j] = (data[j+7] & 0xFF);
					}
					message[j] = 0;

					// add message to list box
					add_msg(plen + 2 + (unsigned int)strlen(clients[(data[6] & 0xFF)].name), "%s: %s", clients[(data[6] & 0xFF)].name, message);
					cout << "message added - deleting message data!" << endl;
					delete message;

					cout << "rcv dt_msg size: " << 7 + plen + 1 << endl;
					used = 7 + plen + 1;
					break;
				}
				case DT_UPDATE: {
					unsigned int cnum = (data[2] & 0xFF);
					if(clients[cnum].status != 0) {
						memcpy(&clients[cnum].position->x, &data[3], 4);
						memcpy(&clients[cnum].position->y, &data[3+4], 4);
						memcpy(&clients[cnum].position->z, &data[3+8], 4);
						memcpy(&clients[cnum].rotation, &data[3+12], 4);
					}
					else {
						m.print(msg::MDEBUG, "world_client.cpp", "client doesn't exist anymore (update routine)");
					}

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
				m.print(msg::MDEBUG, "world_client.cpp", "client #%u doesn't exist or is already initalized! (add routine)", cur_client);
				break;
			}

			// save client data
			unsigned int len = (unsigned int)(data[8] & 0xFF);
			memcpy(clients[cur_client].name, &data[9], len);
			clients[cur_client].name[len+1] = 0;
			clients[cur_client].text->set_text(clients[cur_client].name);
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

			// create the players model
			clients[cur_client].model = new a2emodel();
			clients[cur_client].model->load_model("../data/player.a2m");
			clients[cur_client].model->set_visible(true);
			sce.add_model(clients[cur_client].model);

			cout << "rcv add size: " << 9 + len << endl;
			used = 9 + len;
			break;
		}
		case net::DEL: {
			// which client should be deleted?
			unsigned int cur_client = (data[1] & 0xFF);
			if((cur_client >= max_clients) || clients[cur_client].status != 1) {
				// client doesn't exist / all client "ports" are in use -> break
				m.print(msg::MDEBUG, "world_client.cpp", "client #%u doesn't exist or is already deleted! (delete routine)", cur_client);
				break;
			}

			// print out what happened
			m.print(msg::MDEBUG, "world_client.cpp", "lost client %d: %s", cur_client, clients[cur_client].name);

			// delete the client and resort the data
			delete_player(cur_client);
		}
		cout << "rcv delete size: " << 2 << endl;
		used = 2;
		break;
		case net::KICK: {
			// there are some errors with that, but i absolutely dunno y ...
			m.print(msg::MDEBUG, "world_client.cpp", "sorry, but the server is full!");
		}
		cout << "rcv kick size: " << 1 << endl;
		used = 1;
		break;
		default: {
			// unknown package type
			m.print(msg::MDEBUG, "world_client.cpp", "received a package with an unknown type (%u)! - skipping byte",
				(unsigned int)(data[0] & 0xFF));
		}
		cout << "rcv unknown size: " << 1 << endl;
		used = 1;
		break;
	}
	return used;
}

void handle_server(void) {
	char* data = new char[512];
	unsigned int pos, len, used;

	// checks if client is still connected to the server
	len = SDLNet_TCP_Recv(n.tcp_ssock, data, 512);
	if(len <= 0) {
		n.close_socket(n.tcp_ssock);
	}
	else {
		pos = 0;
		while(len > 0) {
			used = handle_server_data(&data[pos]);
			pos += used;
			len -= used;
			if(used == 0) {
				// lost data ...
				m.print(msg::MDEBUG, "world_client.cpp", "lost data - server down or disconnect? (handle server routine)");
				len = 0;
			}
		}
	}
	delete data;
}

void delete_player(unsigned int num) {
	// set new (own) client number
	if(client_num > num) {
		client_num--;
	}

	// reset all data
	clients[num].id = 0;
	clients[num].position->x = 0.0f;
	clients[num].position->y = 0.0f;
	clients[num].position->z = 0.0f;
	clients[num].rotation = 0.0f;
	clients[num].host = 0;
	clients[num].port = 0;
	clients[num].status = 0;
	clients[num].text->set_notext();
	sprintf(clients[num].name, "unknown");

	// delete the players objects
	sce.delete_model(clients[num].model);
	delete clients[num].model;
	clients[num].model = NULL;

	// resort the clients
	clients[(max_clients-1)] = clients[num];
	for(unsigned int i = num; i < (max_clients-1); i++) {
		clients[i] = clients[i+1];
	}

	// delete player from net class client list
	n.delete_client(num);

	// decrease player count
	cplayers--;
}

void send_msg() {
	// checks if msg input box isn't empty
	if(strlen(chat_msg_input->get_text()) != 0) {
		char* msg = chat_msg_input->get_text();
		unsigned int length = (unsigned int)strlen(msg);

		// send msg
		char data[512];
		data[0] = net::DAT;
		data[1] = DT_MSG;
		data[2] = length & 0xFF000000;
		data[3] = length & 0xFF0000;
		data[4] = length & 0xFF00;
		data[5] = length & 0xFF;
		data[6] = client_num;

		unsigned int i;
		for(i = 0; i < length; i++) {
			data[i+7] = msg[i];
		}
		data[i+7] = 0;
		cout << "dt_msg size: " << length+1 + 7 << endl;
		if(n.tcp_ssock != NULL) {
			SDLNet_TCP_Send(n.tcp_ssock, data, length+1 + 7);
		}

		// add msg to the list box
		add_msg(length + 2 + (unsigned int)strlen(client_name), "%s: %s", client_name, msg);

		// reset input box
		chat_msg_input->set_notext();
		chat_msg_input->set_text_position(0);
	}
}

void add_msg(unsigned int length, char* msg, ...) {
	// copy message
	char* new_msg = new char[length+1];
	va_list	argc;
	va_start(argc, msg);
		vsprintf(new_msg, msg, argc);
	va_end(argc);

	// word wrap ...
	gui_text* tmp_text = new gui_text();
	tmp_text->new_text("../data/vera.ttf", 12);
	tmp_text->set_text(new_msg);

	unsigned int max_width = 250 - 17;
	unsigned int width = tmp_text->get_text_width();
	if(width > max_width) {
		// get the number of words
		unsigned int cwords = 0;
		for(unsigned int i = 0; i < (unsigned int)strlen(new_msg); i++) {
			if(new_msg[i] == ' ') {
				cwords++;
			}
		}

		// make tokens
		unsigned int x = 0;
		char** tokens = new char*[cwords+2];
		tokens[x] = strtok(new_msg, " ");
		while(tokens[x] != NULL) {
			x++;
			tokens[x] = strtok(NULL, " ");
		}

		// allocate a temp string
		char* tmp = new char[512];
		strcpy(tmp, tokens[0]);
		unsigned int cur_width = 0;
		tmp_text->set_text(" ");
		unsigned int space_width = tmp_text->get_text_width();
		tmp_text->set_text(tmp);
		unsigned int last_width = tmp_text->get_text_width();
		// go through every word
		for(unsigned int i = 1; i <= cwords; i++) {
			tmp_text->set_text(tokens[i]);
			if(tmp_text->get_text_width() + space_width + cur_width <= max_width &&
				tmp_text->get_text_width() + space_width + last_width < max_width) {
				// add as many words to the line until the width of max_width px is reached
				last_width = tmp_text->get_text_width();
				sprintf(tmp, "%s %s", tmp, tokens[i]);
				tmp_text->set_text(tmp);
				cur_width = tmp_text->get_text_width();
			}
			else if(tmp_text->get_text_width() + space_width + cur_width > max_width &&
				tmp_text->get_text_width() + space_width + last_width < max_width) {
				// max_width px limit is reached -> add message to list box and increment lid
				chat_msg_list->add_item(tmp, lid);
				lid++;

				// start a new line ...
				strcpy(tmp, tokens[i]);
				tmp_text->set_text(tmp);
				cur_width = tmp_text->get_text_width();
				last_width = tmp_text->get_text_width();
			}
			// check if the new word oversteps the max_width px limit and split it if it does so
			else {
				sprintf(tmp, "%s %s", tmp, tokens[i]);
				tmp_text->set_text(tmp);
				cur_width = tmp_text->get_text_width();
				
				char* str = new char[(unsigned int)strlen(tmp)+1];
				strcpy(str, tmp);
				unsigned int k = 0;
				tmp[k] = str[0];
				tmp[k+1] = 0;
				k++;
				tmp_text->set_text(tmp);
				cur_width = tmp_text->get_text_width();
				char* tmp_str = new char[2];
				tmp_str[1] = 0;
				for(unsigned int j = 1; j < (unsigned int)strlen(str); j++) {
					tmp_str[0] = str[j];
					tmp_text->set_text(tmp_str);
					if(tmp_text->get_text_width() + cur_width <= max_width) {
						// add as many chars to the line until the width of max_width px is reached
						tmp[k] = str[j];
						tmp[k+1] = 0;
						tmp_text->set_text(tmp);
						cur_width = tmp_text->get_text_width();
						k++;
					}
					else {
						// max_width px limit is reached -> add message to list box and increment lid
						chat_msg_list->add_item(tmp, lid);
						lid++;

						// start a new line ...
						k = 0;
						tmp[k] = str[j];
						tmp[k+1] = 0;
						k++;
						tmp_text->set_text(tmp);
						cur_width = tmp_text->get_text_width();
					}
				}
				delete tmp_str;
				delete str;
			}
		}
		// end of the word "list" is reached -> add message to list box and increment lid
		chat_msg_list->add_item(tmp, lid);
		lid++;

		delete tmp;
	}
	else {
		// add message to list box
		chat_msg_list->add_item(new_msg, lid);
		// increment line id
		lid++;
	}

	delete tmp_text;

	// set list box (scroll bar) to the end
	chat_msg_list->set_position(lid-1);

	delete new_msg;
}

void update() {
	// create package
	char* data = new char[7];
	data[0] = net::DAT;
	data[1] = DT_UPDATE;
	memcpy(&data[2], &cam.get_rotation()->y, 4);

	// send data
	if(n.tcp_ssock != NULL) {
        SDLNet_TCP_Send(n.tcp_ssock, data, 6);
	}
	delete data;

	// update players data
	for(unsigned int i = 0; i < cplayers; i++) {
		if(clients[i].status != 0) {
			clients[i].model->set_position(clients[i].position->x, clients[i].position->y - 2.0f, clients[i].position->z);
			clients[i].model->set_rotation(0.0f, clients[i].rotation - 90.0f, 0.0f);
		}
	}

	// update the players cam
	cam.set_position(-clients[client_num].position->x - sinf(clients[client_num].rotation * piover180) * 15.0f,
		clients[client_num].position->y + 10.0f,
		-clients[client_num].position->z - cosf(clients[client_num].rotation * piover180) * 15.0f);

	// reset players light
	player_light->set_position(clients[client_num].position->x,
		clients[client_num].position->y + 5.0f,
		clients[client_num].position->z);
}

void move(MOVE_TYPE type) {
	// create package
	char* data = new char[4];

	data[0] = net::DAT;
	data[1] = DT_MOVE;
	//data[2] = client_num;
	data[2] = type;

	// send data
	if(n.tcp_ssock != NULL) {
        SDLNet_TCP_Send(n.tcp_ssock, data, 3);
	}
	delete data;
}

void init() {
	client_name = new char[32];
	server = new char[32];

	fio.open_file("settings.dat", false);
	char fline[256];
	bool end = false;

	while(!end) {
        fio.get_line(fline);

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
				// get user name
				if(strcmp(fline_tok[0], "name") == 0) {
					strcpy(client_name, fline_tok[1]);
				}
				// get server name/ip
				else if(strcmp(fline_tok[0], "server") == 0) {
					strcpy(server, fline_tok[1]);
				}
				// get server port
				else if(strcmp(fline_tok[0], "port") == 0) {
					port = atoi(fline_tok[1]);
				}
				// get color scheme
				else if(strcmp(fline_tok[0], "scheme") == 0) {
					if(strcmp(fline_tok[1], "windows") == 0) {
						scheme = gui_style::WINDOWS;
					}
					else if(strcmp(fline_tok[1], "blue") == 0) {
						scheme = gui_style::BLUE;
					}
					else if(strcmp(fline_tok[1], "blackwhite") == 0) {
						scheme = gui_style::BLACKWHITE;
					}
				}
				// get width
				else if(strcmp(fline_tok[0], "width") == 0) {
					width = atoi(fline_tok[1]);
				}
				// get height
				else if(strcmp(fline_tok[0], "height") == 0) {
					height = atoi(fline_tok[1]);
				}
				// get depth
				else if(strcmp(fline_tok[0], "depth") == 0) {
					depth = atoi(fline_tok[1]);
				}
			}
		}
	}
	fio.close_file();
}

void update_names() {
	for(unsigned int i = 0; i < cplayers; i++) {
		if(clients[i].status != 0) {
			// set new player text position
			c.get_2d_from_3d(clients[i].position, clients[i].text_point);
			clients[i].text_point->y -= 50;
		}
	}
}

void draw_names() {
	e.start_2d_draw();
	gfx::rect* r = new gfx::rect();
	for(unsigned int i = 0; i < cplayers; i++) {
		if(clients[i].status != 0) {
			core::pnt* p = clients[i].text_point;
			r->x1 = p->x - 4;
			r->y1 = p->y - 4;

			r->x2 = p->x + clients[i].text->get_text_width() + 4;
			r->y2 = p->y + clients[i].text->get_text_height() + 4;
			agfx.draw_filled_rectangle(sf, r, 0xFFFFFF);
			agfx.draw_rectangle(sf, r, 0x000000);
		}
	}
	delete r;
	e.stop_2d_draw();
}

int main(int argc, char *argv[])
{
	// itialize everything (and load the settings)
	init();

	// initialize the engine
	e.init(width, height, depth, false);
	e.set_caption("A2E Sample - World Client Sample");
	e.set_cursor_visible(false);

	// set a color scheme (blue)
	e.set_color_scheme(scheme);
	sf = e.get_screen();

	// initialize the a2e events
	aevent.init(ievent);
	aevent.set_keyboard_layout(event::DE);

	// initialize gui and chat sutff
	agui.init(e, aevent);

	chat_window = agui.add_window(agfx.pnt_to_rect(0, 0, 250 + 4, 275 + 21), 100, "World Chat", true);
	chat_msg_list = agui.add_list_box(agfx.pnt_to_rect(0, 0, 250, 250), 101, "Chat List Box", 100);
	chat_msg_input = agui.add_input_box(agfx.pnt_to_rect(0, 250, 200, 275), 102, "", 100);
	chat_msg_send = agui.add_button(agfx.pnt_to_rect(200, 250, 250, 275), 103, "Send", 100);

	// initialize the camera
	cam.init(e, aevent);
	cam.set_position(0.0f, 50.0f, 0.0f);
	cam.set_cam_input(false);
	cam.set_rotation_speed(50.0f);

	// load the models and set new positions
	level.load_model("../data/move_level.a2m"); 
	level.set_scale(0.5f, 0.5f, 0.5f);
	sce.add_model(&level);

	sphere.load_model("../data/player_sphere.a2m"); 
	sce.add_model(&sphere);

	player_light = new light(0.0f, 50.0f, 0.0f);
	float pamb[] = { 1.0f, 1.0f, 1.0f, 0.0f};
	float pdif[] = { 1.0f, 1.0f, 1.0f, 0.0f};
	float pspc[] = { 1.0f, 1.0f, 1.0f, 0.0f};
	player_light->set_lambient(pamb);
	player_light->set_ldiffuse(pdif);
	player_light->set_lspecular(pspc);
	player_light->set_constant_attenuation(0.0f);
	player_light->set_linear_attenuation(1.0f / range);
	player_light->set_quadratic_attenuation(0.0f);
	sce.add_light(player_light);

	l1 = new light(-50.0f, 100.0f, -50.0f);
	float lamb[] = { 0.1f, 0.1f, 0.1f, 0.0f};
	float ldif[] = { 0.2f, 0.2f, 0.2f, 0.0f};
	float lspc[] = { 0.0f, 0.0f, 0.0f, 0.0f};
	l1->set_lambient(lamb);
	l1->set_ldiffuse(ldif);
	l1->set_lspecular(lspc);
	sce.add_light(l1);

	// init network stuff
	if(n.init()) {
		m.print(msg::MDEBUG, "world_client.cpp", "net class initialized!");
		SDL_Delay(1000);
		// we just want a client program
		if(n.create_client(server, net::TCP, port, client_name)) {
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
	name_time = SDL_GetTicks();
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
							if(control_state == 0) { move(MV_FORWARD); }
							break;
						case SDLK_DOWN:
							if(control_state == 0) { move(MV_BACKWARD); }
							break;
						case SDLK_RETURN:
							if(control_state == 1) { send_msg(); }
							break;
						default:
						break;
					}
					break;
				case SDL_MOUSEBUTTONDOWN:
					switch(aevent.get_event().button.button) {
						case SDL_BUTTON_RIGHT:
							// toggle control state
							control_state = !control_state;
							if(control_state == 0) {
								e.set_cursor_visible(false);
								cam.set_mouse_input(true);
							}
							else {
								e.set_cursor_visible(true);
								cam.set_mouse_input(false);
							}
							break;
						default:
						break;
					}
					break;
				default:
					break;
			}
		}

		while(aevent.is_gui_event()) {
			switch(aevent.get_gui_event().type) {
				case event::BUTTON_PRESSED:
					switch(aevent.get_gui_event().id) {
						case 103: {
							if(control_state == 1) { send_msg(); }
						}
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
				handle_server();
			}

			if(n.tcp_ssock == NULL) {
				done = true;
			}
		}
		else {
			done = true;
		}

		// print out the fps count
		fps++;
		if(SDL_GetTicks() - fps_time > 1000) {
			sprintf(tmp, "A2E Sample - World Client Sample | FPS: %u | Pos: %f %f %f", fps,
				-cam.get_position()->x, cam.get_position()->y, -cam.get_position()->z);
			fps = 0;
			fps_time = SDL_GetTicks();
		}
		e.set_caption(tmp);

		// send new client data to server
		if(is_initialized) { update(); }

		// start drawing the scene
		e.start_draw();

		cam.run();
		sce.draw();

		update_names();

		draw_names();
		agui.draw();

		e.stop_draw();

		// delay for 1000/75 ms to get an fps rate of 75 fps and
		// to reduce the cpu usage (so its no more 100%)
		SDL_Delay((unsigned int)1000/75);
	}

	delete tmp;

	n.exit();

	// remove player models from the scene list
	for(unsigned int i = 0; i < max_clients; i++) {
		if(clients[i].status != 0) {
            sce.delete_model(clients[i].model);
		}
	}

	delete clients;
	delete client_name;
	delete server;

	return 0;
}
