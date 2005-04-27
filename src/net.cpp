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

#include <iostream>
using namespace std;
#include "net.h"
#include <math.h>

/*! there is no function currently
 */
net::net():
	max_clients(MAX_CLIENTS)
{
}

/*! there is no function currently
 */
net::~net() {
	m.print(msg::MDEBUG, "net.cpp", "freeing net stuff");


	m.print(msg::MDEBUG, "net.cpp", "net stuff freed");
}

/*! initialize the networking functions - returns true if successful, otherwise false
 */
bool net::init() {
	if(SDLNet_Init()==-1) {
		m.print(msg::MERROR, "net.cpp", "SDLNet_Init", "%s", SDLNet_GetError());
		return false;
	}

	return true;
}

/*! shutdown and cleanup all networking stuff
 */
void net::exit() {
	SDLNet_Quit();
	if(clients) {
		delete clients;
	}
}

/*! creates an server
 *  @param type the type of protocol that is used (currently just tcp support!)
 *  @param port the port that is used
 *  @param num_clients the maximal amount of people who can connect to the server
 */
bool net::create_server(unsigned int type, unsigned short int port, const unsigned int num_clients) {
	net::max_clients = num_clients;

	if(SDLNet_ResolveHost(&local_ip, NULL, port) == -1) {
		m.print(msg::MERROR, "net.cpp", "SDLNet_ResolveHost (local server): %s", SDLNet_GetError());
		return false;
	}

	// client stuff
	clients = new client[net::max_clients];
	for(unsigned int i = 0; i < net::max_clients; i++) {
		clients[i].is_active = false;
		clients[i].sock = NULL;
		sprintf(clients[i].name, "unknown");
	}

	// initialize socketset
	socketset = SDLNet_AllocSocketSet(net::max_clients);
	if(socketset == NULL) {
		m.print(msg::MERROR, "net.cpp", "Couldn't create socket set: %s", SDLNet_GetError());
		return false;
	}

	switch(type) {
		case net::TCP:
			// listening socket @localhost
			tcp_ssock = SDLNet_TCP_Open(&local_ip);
			if(!tcp_ssock) {
				m.print(msg::MERROR, "net.cpp", "SDLNet_TCP_Open (local server): %s", SDLNet_GetError());
				return false;
			}
			SDLNet_TCP_AddSocket(socketset, tcp_ssock);

			// everything is set up now - the clients are now able to connect to the server 

			break;
		case net::UDP:
			break;
	}

	return true;
}

/*! creates a client
 *  @param server the name/ip of the server
 *  @param type the type of protocol that is used (currently just tcp support!)
 *  @param port the port that is used
 *  @param client_name the clients' name
 */
bool net::create_client(char* server, unsigned int type, unsigned short int port, char* client_name) {
	net::max_clients = 256;
	if(SDLNet_ResolveHost(&server_ip, server, port)==-1) {
		m.print(msg::MERROR, "net.cpp", "SDLNet_ResolveHost (client->server): %s", SDLNet_GetError());
		return false;
	}

	if(SDLNet_ResolveHost(&local_ip, NULL, port)==-1) {
		m.print(msg::MERROR, "net.cpp", "SDLNet_ResolveHost (client->server): %s", SDLNet_GetError());
		return false;
	}

	// client stuff
	clients = new client[net::max_clients];
	for(unsigned int i = 0; i < net::max_clients; i++) {
		clients[i].is_active = false;
		sprintf(clients[i].name, "unknown");
		// aren't needed if it is a client
		clients[i].sock = NULL;
		clients[i].port = 0;
		clients[i].ip.port = 0;
		clients[i].ip.host = 0;
	}

	// initialize socketset
	socketset = SDLNet_AllocSocketSet(2);
	if(socketset == NULL) {
		m.print(msg::MERROR, "net.cpp", "Couldn't create socket set: %s", SDLNet_GetError());
		return false;
	}

	switch(type) {
		case net::TCP:
			// create server tcp socket
			tcp_ssock = SDLNet_TCP_Open(&server_ip);
			if(!tcp_ssock) {
				m.print(msg::MERROR, "net.cpp", "SDLNet_TCP_Open (server): %s", SDLNet_GetError());
				return false;
			}

			// connect to server
			tcp_csock = SDLNet_TCP_Open(&local_ip);
			if(!tcp_csock) {
				m.print(msg::MERROR, "net.cpp", "SDLNet_TCP_Open (client->server): %s", SDLNet_GetError());
				return false;
			}

			// connection created - data transfer is now possible
			m.print(msg::MDEBUG, "net.cpp", "SDLNet_TCP_Open (client->server): successfully connected to server!");

			SDLNet_TCP_AddSocket(socketset, tcp_ssock);
			SDLNet_TCP_AddSocket(socketset, tcp_csock);

			net::send_activation(client_name);

			break;
		case net::UDP:
			break;
	}

	return true;
}

/*! handles the server - if there is a new connection, it checks if the max user limit
 *! is reached and trys to disconnect inactive clients. if there is a free "slot", it
 *! adds the client to the "client list". otherwise it would kick the new client.
 */
void net::handle_server() {
	TCPsocket tmpsock;
	unsigned int cur_client;
	unsigned char data;

	tmpsock = SDLNet_TCP_Accept(tcp_ssock);
	if(tmpsock == NULL) {
		return;
	}

	// checks if there are any unconnected clients
	for(cur_client = 0; cur_client < net::max_clients; ++cur_client) {
		if(clients[cur_client].sock == NULL) {
			break;
		}
	}

	if(cur_client == net::max_clients) {
		// checks if there is any inactive client
		for(cur_client = 0; cur_client < net::max_clients; ++cur_client) {
			if(clients[cur_client].sock && !clients[cur_client].is_active) {
				// kick this client
				data = net::KICK;
				SDLNet_TCP_Send(clients[cur_client].sock, &data, 1);
				SDLNet_TCP_DelSocket(socketset, clients[cur_client].sock);
				SDLNet_TCP_Close(clients[cur_client].sock);
				m.print(msg::MDEBUG, "net.cpp", "inactive client (%d) was kicked", cur_client);
				break;
			}
		}
	}

	if(cur_client == net::max_clients) {
		// the server doesn't permit any further connections
		data = net::KICK;
		SDLNet_TCP_Send(tmpsock, &data, 1);
		SDLNet_TCP_Close(tmpsock);
		m.print(msg::MDEBUG, "net.cpp", "connection refused: server doesn't permit any further connections");
	}
	else {
		// add socket to socketset and make it inactive
		clients[cur_client].sock = tmpsock;
		clients[cur_client].ip = *SDLNet_TCP_GetPeerAddress(tmpsock);
		SDLNet_TCP_AddSocket(socketset, clients[cur_client].sock);
		m.print(msg::MDEBUG, "net.cpp", "a new inactive socket (%d) was added", cur_client);
	}
}

/*! handles the clients (on the server) - closes the socket, if the client is inactive
 *! and handles new clients (send new client data to the other clients and vice versa)
 *  @param cur_client the current (or new) client number
 */
void net::handle_client(unsigned int cur_client) {
	char data[512];
	char ndata[512];
	char cdata[32];

	// checks if connection has been closed
	if(SDLNet_TCP_Recv(clients[cur_client].sock, data, 512) <= 0) {
        m.print(msg::MDEBUG, "net.cpp", "closing %s socket (%d)",
			clients[cur_client].is_active ? "active" : "inactive", cur_client);
		// send delete data to all clients
		if(clients[cur_client].is_active) {
			clients[cur_client].is_active = false;
			data[0] = net::DEL;
			data[1] = cur_client;
			for(unsigned int i = 0; i < net::max_clients; ++i) {
				if(clients[i].is_active) {
					SDLNet_TCP_Send(clients[i].sock, data, 2);
				}
			}
		}
		// close socket
		net::close_socket(clients[cur_client].sock);
	}
	else {
		switch(data[0]) {
			case net::NEW:
				// active connection

				// send max client data (for the future maybe other stuff too)
				cdata[0] = net::CDAT;
				cdata[1] = net::max_clients;
				SDLNet_TCP_Send(clients[cur_client].sock, cdata, 2);

				// get client data
				memcpy(&clients[cur_client].ip.port, &data[1], 2);
				memcpy(&clients[cur_client].name, &data[4], 32);
				clients[cur_client].name[32] = 0;
				m.print(msg::MDEBUG, "net.cpp", "activating socket (%d: %s)",
					cur_client, clients[cur_client].name);
				// send data to all clients
				for(unsigned int i = 0; i < net::max_clients; i++) {
					if(clients[i].is_active) {
						send_new_client(cur_client, i);
					}
				}

				// send data of all clients to current client
				clients[cur_client].is_active = true;
				for(unsigned int i = 0; i < net::max_clients; i++) {
					if(clients[i].is_active) {
						send_new_client(i, cur_client);
					}
				}
                break;

			case net::DAT: {
				// set package type
				ndata[0] = net::DAT;
				// get data package length
				unsigned int len = (unsigned int)(data[1]*0xFF0000 + data[2]*0xFF00 + data[3]*0xFF + data[4]);
				ndata[1] = len & 0xFF000000;
				ndata[2] = len & 0xFF0000;
				ndata[3] = len & 0xFF00;
				ndata[4] = len & 0xFF;
				ndata[5] = cur_client;
				// put package data into new package (and into data that is printed out)
				char* print_data = new char[len];
				unsigned int i;
				for(i = 0; i < len; i++) {
					ndata[i+6] = data[i+6];
					print_data[i] = data[i+6];
				}
				ndata[i+6] = 0;
				print_data[i] = 0;

				// print out package data
				m.print(msg::MMSG, "net.cpp", "data (%d bytes) send from %s(%d): %s",
					len, clients[cur_client].name, cur_client, print_data);
				delete print_data;

				// send package to all clients except the one who send the data
				for(unsigned int i = 0; i < net::max_clients; i++) {
					if(clients[i].is_active && i != cur_client) {
						SDLNet_TCP_Send(clients[i].sock, ndata, len+1 + 6);
					}
				}
			}
			break;

			default: {
				// unknown packet type
				m.print(msg::MDEBUG, "net.cpp", "a package with an unknown package type was send! type: %u", data[0]);
			}
			break;
		}
	}
}

/*! sends client data (ip, port, name) to another client
 *  @param snd_client the client number from which the data is send
 *  @param rcv_client the client number which receives the data
 */
void net::send_new_client(unsigned int snd_client, unsigned int rcv_client) {
	char data[512];

	unsigned int n = (int)strlen((char*)clients[snd_client].name)+1;
	data[0] = net::ADD;
	data[1] = snd_client;
	memcpy(&data[2], &clients[snd_client].ip.host, 4);
	memcpy(&data[6], &clients[snd_client].ip.port, 2);
	data[8] = n;
	data[9+n] = 0;
	memcpy(&data[9], clients[snd_client].name, n);
	SDLNet_TCP_Send(clients[rcv_client].sock, data, 9+n);
}

/*! checks if a client has send an event
 */
void net::check_events() {
	for(unsigned int i = 0; i < net::max_clients; ++i) {
		if(SDLNet_SocketReady(clients[i].sock)) {
			handle_client(i);
		}
	}
}

/*! sends client data to the server and activates the client
 *  @param client_name the clients name
 */
void net::send_activation(char* client_name) {
	char data[1+1+32];
	int len;

	// set all clients to inactive at first
	for(unsigned int i = 0; i < net::max_clients; i++) { // constant value of MAX_CLIENTS clients ... has to be changed!
		clients[i].is_active = false;
	}
	if(!net::tcp_ssock == 0) {
		// make the package

		data[0] = net::NEW;
		// need to create a new IPaddress value, because getpeeraddress returns a
		// IPaddress* and not a IPaddress like the other sdl_net functions?
		IPaddress* lh_ip = SDLNet_TCP_GetPeerAddress(tcp_ssock);
		memcpy(&data[1], &lh_ip->port, 2);
		if(strlen(client_name) > 32) {
			len = 32;
		}
		else {
			len = (int)strlen(client_name);
		}
		data[3] = len;
		strncpy(&data[4], client_name, len);
		data[4 + len++] = 0;

		// send the package to the server
		SDLNet_TCP_Send(tcp_ssock, data, 4 + len);
	}

	return;
}

/*! closes and deletes the tcp socket
 *  @param sock the tcp socket that should be closed
 */
void net::close_socket(TCPsocket &sock) {
	SDLNet_TCP_DelSocket(socketset, sock);
	SDLNet_TCP_Close(sock);
	sock = NULL;
}

/*! closes and deletes the udp socket
 *  @param sock the udp socket that should be closed
 */
void net::close_socket(UDPsocket &sock) {
	SDLNet_UDP_DelSocket(socketset, sock);
	SDLNet_UDP_Close(sock);
	sock = NULL;
}
