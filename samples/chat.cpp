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

#include "chat.h"
#include <iostream>

/*!
 * \mainpage
 *
 * \author flo
 *
 * \date July 2004
 *
 * Albion 2 Engine Sample - Chat
 */

int HandleServerData(char *data)
{
	int used = 0;

	switch (data[0]) {
		case net::CMD::CDAT: {
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
		case net::CMD::DAT: {
			// get data package length
			unsigned int plen = (unsigned int)(data[1]*0xFF0000 + data[2]*0xFF00 + data[3]*0xFF + data[4]);
			char* message = (char*)malloc(plen);
			unsigned int j;
			for(j = 0; j < plen; j++) {
				message[j] = data[j+6];
			}
			message[j] = 0;
			m.print(msg::PRINT::MMSG, "Chat", "%s(%d): %s",
				clients[data[5]].name, data[5], message);

			used = 6+plen;
		}
		break;
		case net::CMD::ADD: {
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
			m.print(msg::PRINT::MMSG, "Chat", "new client on %d from %d.%d.%d.%d:%d (%s)",
				cur_client, (cip.host>>24)&0xFF, (cip.host>>16)&0xFF,
				(cip.host>>8)&0xFF, cip.host&0xFF,
				cip.port, clients[cur_client].name);

			// put the address back in network form
			cip.host = SDL_SwapBE32(cip.host);
			cip.port = SDL_SwapBE16(cip.port);

			client_num = cur_client;
			client_active = true;
		}
		used = 9+data[8];
		break;
		case net::CMD::DEL: {
			unsigned int cur_client;

			// which client was deleted?
			cur_client = data[1];
			if ((cur_client >= max_clients) || !clients[cur_client].is_active) {
				// invalid!
				break;
			}
			clients[cur_client].is_active = false;

			// print out what happened
			m.print(msg::PRINT::MMSG, "Chat", "lost client %d: %s", cur_client, clients[cur_client].name);
		}
		used = 2;
		break;
		case net::CMD::KICK: {
			m.print(msg::PRINT::MMSG, "Chat", "sorry, but the chat server is full!");
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
	char* msg_data = (char*)malloc(507); // 512 - 5, because we need 5 bytes for the package header
	cout << client_name << ": ";
	m.scan(507, msg_data);
	unsigned int msg_len;
	for(msg_len = 0; msg_data[msg_len] != 0; msg_len++) {} // perhaps that should be done in a different way ...
	send_msg(msg_data, msg_len);
}

void send_msg(char* message, unsigned int length) {
	char data[512];
	data[0] = net::CMD::DAT;
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

int main(int argc, char *argv[])
{
	SDL_Event event;
	bool is_networking = false;
	bool is_send = false;
	bool is_rec = false;
	bool is_server = false;

	if(argc > 1) {
		if(strcmp(argv[1], "server") == 0) {
			is_server = true;
		}
	}

	if(!is_server) {
		// sdl stuff ->
		// we have to create a sdl window, otherwise there won't be any sdl events :/
		if(SDL_Init(SDL_INIT_VIDEO) == -1)
		{
			printf("Can't init SDL:  %s\n", SDL_GetError());
			exit(1);
		}
		atexit(SDL_Quit);

		// create screen
		SDL_Surface *screen = SDL_SetVideoMode(80, 80, 16, SDL_HWSURFACE | SDL_DOUBLEBUF);
		if(screen == NULL)
		{
			printf("Can't set video mode: %s\n", SDL_GetError());
			exit(1);
		}
		// <- sdl stuff end
	}

	char* server;
	if(argc > 2) {
		server = argv[2];
	}
	else {
		server = "localhost"; // won't really work - just for testing or debugging purposes
	}

	if(argc > 3) {
		client_name = argv[3];
	}
	else {
		client_name = "unknown";
	}

	if(n.init()) {
		if(is_server) {
			if(n.create_server(net::PTYPE::TCP, 1337, 16)) {
				is_networking = true;
			}
		}
		else {
			if(n.create_client(server, net::PTYPE::TCP, 1337, client_name)) {
				is_networking = true;
			}
		}
	}

	while(!done)
	{
		while(SDL_PollEvent(&event))
		{
			switch(event.type)
			{
			case SDL_QUIT:
				done = true;
				break;
			}
		}

		if(is_networking) {
			if(is_server) {
				// server stuff
				SDLNet_CheckSockets(n.socketset, ~0);

				if(SDLNet_SocketReady(n.tcp_ssock)) {
					n.handle_server();
				}

				n.check_events();
			}
			else {
				// client stuff
				SDLNet_CheckSockets(n.socketset, 0);
				if(SDLNet_SocketReady(n.tcp_ssock)) {
					HandleServer();
				}

				keys = SDL_GetKeyState(NULL);
				if(keys[SDLK_s] == SDL_PRESSED) {
					get_msg();
				}

				if(n.tcp_ssock == NULL) {
					done = true;
					m.print(msg::PRINT::MERROR, "Chat:", "server isn't available - perhaps disconnect");
				}
			}
		}
	}

	n.exit();

	SDL_Delay(2000);

	return 0;
}
