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
 * \date July - August 2004
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
				sprintf(clients[i].name, "unknown");
			}
		}
	    used = 1;
		break;
		case net::DAT: {
			// get data package length
			unsigned int plen = (unsigned int)(data[1]*0xFF0000 + data[2]*0xFF00 + data[3]*0xFF + data[4]);
			char* message = (char*)malloc(plen);
			unsigned int j;
			for(j = 0; j < plen; j++) {
				message[j] = data[j+6];
			}
			message[j] = 0;
			add_msg("%s(%d): %s", clients[data[5]].name, data[5], message);

			used = 6+plen;
		}
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
		}
		used = 2;
		break;
		case net::KICK: {
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
	send_msg(msg_box->get_text(), (unsigned int)strlen(msg_box->get_text()));
	add_msg("%s: %s", client_name, msg_box->get_text());
	msg_box->set_notext();
	msg_box->set_text_position(0);
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

	msg_list->add_item(omsg, lid);
	lid++;

	free(omsg);
}

void add_user(char* name) {
	client_list->add_item(name, uid);
	uid++;
}

int main(int argc, char *argv[])
{
	// init a2e
	e.init(800, 600, 32, false);
	aevent.init(ievent);
	sf = e.get_screen();
	agui.init(e, aevent);

	// init gui
	info_text = agui.add_text("vera.ttf", 14, "A2E Chat Sample - 0.01", 0x000000, agfx.cord_to_pnt(10, 10), 100);
	send_button = agui.add_button(agfx.pnt_to_rect(620, 560, 790, 590), 101, "Send");
	msg_list = agui.add_list_box(agfx.pnt_to_rect(10, 45, 610, 550), 102, "msg list box");
	client_list = agui.add_list_box(agfx.pnt_to_rect(620, 45, 790, 550), 103, "clients list box");
	msg_box = agui.add_input_box(agfx.pnt_to_rect(10, 560, 610, 590), 104, "msg");
	msg_box->set_notext();
	msg_box->set_text_position(0);
	aevent.set_keyboard_layout(event::DE);


	char* server;
	if(argc > 2) {
		server = argv[2];
	}
	else {
		server = "localhost"; // if you host a a2e chat server and want to run a client, you have to set the ip to localhost
	}

	if(argc > 3) {
		client_name = argv[3];
	}
	else {
		client_name = "unknown";
	}

	// create client
	if(n.init()) {
		// we just want a client program
		if(n.create_client(server, net::TCP, 1337, client_name)) {
			add_msg("Chat: Welcome to the A2E Chat Client Sample! ;)");
			is_networking = true;
		}
	}

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
		}
		else {
			done = true;
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
			e.draw();
			agui.draw();
			refresh_time = SDL_GetTicks();
		}
	}

	n.exit();

	return 0;
}
