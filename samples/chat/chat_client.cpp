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

#include "chat_client.h"
#include <iostream>
#include <cstdarg>

/*!
 * \mainpage
 *
 * \author flo
 *
 * \date July - February 2004
 *
 * Albion 2 Engine Sample - Chat Client Sample
 */

int HandleServerData(char *data)
{
	int used = 0;

	switch (data[0]) {
		case net::CDAT: {
			max_clients = data[1];

			// client stuff
			clients = (client*)malloc(sizeof(client)*max_clients);
			for(unsigned int i = 0; i < max_clients; i++) {
				clients[i].is_active = false;
				clients[i].id = 0;
				sprintf(clients[i].name, "unknown");
			}
		}
	    used = 2;
		break;
		case net::DAT: {
			// get data package length
			plen = (unsigned int)(data[1]*0xFF0000 + data[2]*0xFF00 + data[3]*0xFF + data[4]);
			char* message = (char*)malloc(plen);
			unsigned int j;
			for(j = 0; j < plen; j++) {
				message[j] = data[j+6];
			}
			message[j] = 0;
			add_msg("%s: %s", clients[data[5]].name, message);
			msg_list->set_position(lid-1);
			//free(message);
		}
		used = 6+plen;
		break;
		case net::ADD: {
			unsigned int cur_client;
			IPaddress cip;

			cur_client = data[1];
			if((cur_client >= max_clients) || clients[cur_client].is_active) {
				// invalid!
				break;
			}
			// get the client's ip
			cip.host=SDLNet_Read32(&data[2]);
			cip.port=SDLNet_Read16(&data[6]);

			// copy name into struct
			memcpy(clients[cur_client].name, &data[9], 32);
			clients[cur_client].name[32] = 0;
			clients[cur_client].is_active = true;

			// print out some data
			add_msg("Chat: new client on %d from %d.%d.%d.%d:%d (%s)",
				cur_client, (cip.host>>24)&0xFF, (cip.host>>16)&0xFF,
				(cip.host>>8)&0xFF, cip.host&0xFF,
				cip.port, clients[cur_client].name);

			// put the address back in network form
			cip.host = SDL_SwapBE32(cip.host);
			cip.port = SDL_SwapBE16(cip.port);

			client_num = cur_client;
			client_active = true;

			// add user to client list
			clients[cur_client].id = uid;
			add_user(clients[cur_client].name);
		}
		used = 9+data[8];
		break;
		case net::DEL: {
			unsigned int cur_client;

			// which client was deleted?
			cur_client = data[1];
			if ((cur_client >= max_clients) || !clients[cur_client].is_active) {
				// invalid!
				break;
			}
			clients[cur_client].is_active = false;

			// print out what happened
			add_msg("Chat: lost client %d: %s", cur_client, clients[cur_client].name);
			client_list->delete_item(clients[cur_client].id);

			// delete client item
			client_list->delete_item(clients[cur_client].id);
		}
		used = 2;
		break;
		case net::KICK: {
			// there are some errors with that, but i absolutely dunno y ...
			add_msg("Chat: sorry, but the chat server is full!");
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

void HandleServer(void)
{
	char data[512];
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
}

void get_msg()
{
	// checks if msg box isn't empty
	if(strlen(msg_box->get_text()) != 0) {
		send_msg(msg_box->get_text(), (unsigned int)strlen(msg_box->get_text()));
		// add msg to list
		// we have a max amount of chars per line, that is 55 chars
		char* text = msg_box->get_text();
		/*unsigned int text_len = (unsigned int)strlen(text);
		if(text_len > 55) {
			for(unsigned int i = 0; i <= (unsigned int)text_len/55; i++) {
				char tmp[56];
				for(unsigned int j = 0; j < 55; j++) {
					tmp[j] = text[j+i*55];
				}
				tmp[55] = 0;
				if(i == 0) {
					add_msg("%s: %s", client_name, tmp);
				}
				else {
					add_msg("%s", tmp);
				}
			}
		}
		else {
			add_msg("%s: %s", client_name, text);
		}*/

		add_msg("%s: %s", client_name, text);

		msg_list->set_position(lid-1);

		msg_box->set_notext();
		msg_box->set_text_position(0);
		// now we fake a click on the input box to activate it again ;)
		aevent.set_last_pressed(10, 560);
	}
}

void send_msg(char* message, unsigned int length) {
	char data[512];
	data[0] = net::DAT;
	data[1] = length & 0xFF000000;
	data[2] = length & 0xFF0000;
	data[3] = length & 0xFF00;
	data[4] = length & 0xFF;
	data[5] = client_num;
	unsigned int i;
	for(i = 0; i < length; i++) {
		data[i+6] = message[i];
	}
	data[i+6] = 0;
	if(n.tcp_ssock != NULL) {
        SDLNet_TCP_Send(n.tcp_ssock, data, length+1 + 6);
	}
}

void add_msg(char* msg, ...) {
	va_list	argc;
	char* omsg = (char*)malloc(256);

	va_start(argc, msg);
		vsprintf(omsg, msg, argc);
	va_end(argc);

	unsigned int text_len = (unsigned int)strlen(omsg);
	if(text_len > 55) {
		for(unsigned int i = 0; i <= (unsigned int)text_len/55; i++) {
			char tmp[56];
			for(unsigned int j = 0; j < 55; j++) {
				tmp[j] = omsg[j+i*55];
			}
			tmp[55] = 0;

			msg_list->add_item(tmp, lid);
			lid++;
		}
	}
	else {
		msg_list->add_item(omsg, lid);
		lid++;
	}

	free(omsg);
}

void add_user(char* name) {
	client_list->add_item(name, uid);
	uid++;
}

void load_settings() {
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
					else if(strcmp(fline_tok[1], "blachwhite") == 0) {
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
        
        port = 1337;
        width = 800;
        height = 600;
        depth = 24;
        scheme = gui_style::BLUE;
        sprintf(client_name, "Flo");
        client_name[3] = 0;
        sprintf(server, "localhost");
        client_name[9] = 0;   
}

int main(int argc, char *argv[])
{
	// init a2e
	load_settings();
	m.print(msg::MDEBUG, "chat_client.cpp", "settings loaded!");
	e.init(width, height, depth, false);
	m.print(msg::MDEBUG, "chat_client.cpp", "engine initialized!");
	aevent.init(ievent);
	m.print(msg::MDEBUG, "chat_client.cpp", "event class initialized!");
	sf = e.get_screen();
	agui.init(e, aevent);
	m.print(msg::MDEBUG, "chat_client.cpp", "gui initialized!");
	e.set_caption("A2E Sample - Chat Client");
	e.set_color_scheme(scheme);
	m.print(msg::MDEBUG, "chat_client.cpp", "color scheme set!");

	// init gui
	info_text = agui.add_text("vera.ttf", 14, "A2E Chat Sample - 0.01", e.get_gstyle().STYLE_FONT2, agfx.cord_to_pnt(10, 10), 100);
	send_button = agui.add_button(agfx.pnt_to_rect(620, 560, 790, 590), 101, "Send");
	msg_list = agui.add_list_box(agfx.pnt_to_rect(10, 45, 610, 550), 102, "msg list box");
	client_list = agui.add_list_box(agfx.pnt_to_rect(620, 45, 790, 550), 103, "clients list box");
	msg_box = agui.add_input_box(agfx.pnt_to_rect(10, 560, 610, 590), 104, "msg");
	msg_box->set_notext();
	msg_box->set_text_position(0);
	aevent.set_keyboard_layout(event::DE);
	m.print(msg::MDEBUG, "chat_client.cpp", "keyboard layout set!");

	// create client
	if(n.init()) {
		m.print(msg::MDEBUG, "chat_client.cpp", "net class initialized!");
		SDL_Delay(1000);
		// we just want a client program
		if(n.create_client(server, net::TCP, port, client_name)) {
			m.print(msg::MDEBUG, "chat_client.cpp", "client created!");
			add_msg("Chat: Welcome to the A2E Chat Client Sample! ;)");
			is_networking = true;
		}
		else {
			m.print(msg::MDEBUG, "chat_client.cpp", "client couldn't be created!");
		}
	}
	else {
		m.print(msg::MDEBUG, "chat_client.cpp", "net class couldn't be initialized!");
	}

	// now we fake a click on the input box to activate it ;)
	aevent.set_last_pressed(10, 560);

	// endless loop
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
							get_msg();
							break;
					}
					break;
			}
		}

		// is network running? do we have a connection?
		if(is_networking) {
			// client stuff
			SDLNet_CheckSockets(n.socketset, 0);
			if(SDLNet_SocketReady(n.tcp_ssock)) {
				HandleServer();
			}

			if(n.tcp_ssock == NULL) {
				done = true;
				add_msg("Chat: server isn't available - perhaps disconnect");
			}

			while(aevent.is_gui_event()) {
				switch(aevent.get_gui_event().type) {
					case event::BUTTON_PRESSED:
						switch(aevent.get_gui_event().id) {
							case 101: {
								get_msg();
							}
							break;
							default:
								break;
						}
						break;
				}
			}

			// refresh every 1000/60 milliseconds (~ 60 fps)
			if(SDL_GetTicks() - refresh_time >= 1000/60) {
				e.start_draw();
				agui.draw();
				e.stop_draw();
				refresh_time = SDL_GetTicks();
			}
		}
		else {
			done = true;
		}
	}

	n.exit();

	if(sf) {
        SDL_FreeSurface(sf);
	}
	if(clients) {
		free(clients);
	}

	return 0;
}
