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
net::net(engine* e) {
	max_clients = MAX_CLIENTS;
	header = 0x41324557;

	netlog = false;
	logfile = new fstream();
	logfile->open("netlog.txt", fstream::out);

	// get classes
	net::e = e;
	net::m = e->get_msg();
}

/*! there is no function currently
 */
net::~net() {
	m->print(msg::MDEBUG, "net.cpp", "freeing net stuff");

	logfile->close();
	delete logfile;

	m->print(msg::MDEBUG, "net.cpp", "net stuff freed");
}

/*! initialize the networking functions - returns true if successful, otherwise false
 */
bool net::init() {
	if(SDLNet_Init() == -1) {
		m->print(msg::MERROR, "net.cpp", "SDLNet_Init: %s", SDLNet_GetError());
		return false;
	}

	return true;
}

/*! shutdown and cleanup all networking stuff
 */
void net::exit() {
	SDLNet_Quit();

	if(clients) {
		delete [] clients;
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
		m->print(msg::MERROR, "net.cpp", "SDLNet_ResolveHost (local server): %s", SDLNet_GetError());
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
		m->print(msg::MERROR, "net.cpp", "Couldn't create socket set: %s", SDLNet_GetError());
		return false;
	}

	switch(type) {
		case net::TCP:
			// listening socket @localhost
			tcp_ssock = SDLNet_TCP_Open(&local_ip);
			if(!tcp_ssock) {
				m->print(msg::MERROR, "net.cpp", "SDLNet_TCP_Open (local server): %s", SDLNet_GetError());
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
		m->print(msg::MERROR, "net.cpp", "SDLNet_ResolveHost (client->server): %s", SDLNet_GetError());
		return false;
	}

	if(SDLNet_ResolveHost(&local_ip, NULL, port)==-1) {
		m->print(msg::MERROR, "net.cpp", "SDLNet_ResolveHost (client->server): %s", SDLNet_GetError());
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
		m->print(msg::MERROR, "net.cpp", "Couldn't create socket set: %s", SDLNet_GetError());
		return false;
	}

	switch(type) {
		case net::TCP:
			// create server tcp socket
			tcp_ssock = SDLNet_TCP_Open(&server_ip);
			if(!tcp_ssock) {
				m->print(msg::MERROR, "net.cpp", "SDLNet_TCP_Open (server): %s", SDLNet_GetError());
				return false;
			}

			// connect to server
			tcp_csock = SDLNet_TCP_Open(&local_ip);
			if(!tcp_csock) {
				m->print(msg::MERROR, "net.cpp", "SDLNet_TCP_Open (client->server): %s", SDLNet_GetError());
				return false;
			}

			// connection created - data transfer is now possible
			m->print(msg::MDEBUG, "net.cpp", "SDLNet_TCP_Open (client->server): successfully connected to server!");

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
	char* data = new char[1];

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
				data[0] = net::KICK;
				send_packet(&clients[cur_client].sock, data, 1, cur_client);
				SDLNet_TCP_DelSocket(socketset, clients[cur_client].sock);
				SDLNet_TCP_Close(clients[cur_client].sock);
				m->print(msg::MDEBUG, "net.cpp", "inactive client (%d) was kicked", cur_client);
				break;
			}
		}
	}

	if(cur_client == net::max_clients) {
		// the server doesn't permit any further connections
		data[0] = net::KICK;
		send_packet(&tmpsock, data, 1, 0xFFFFFFFF);
		SDLNet_TCP_Close(tmpsock);
		m->print(msg::MDEBUG, "net.cpp", "connection refused: server doesn't permit any further connections");
	}
	else {
		// add socket to socketset and make it inactive
		clients[cur_client].sock = tmpsock;
		clients[cur_client].ip = *SDLNet_TCP_GetPeerAddress(tmpsock);
		SDLNet_TCP_AddSocket(socketset, clients[cur_client].sock);
		m->print(msg::MDEBUG, "net.cpp", "a new inactive socket (%d) was added", cur_client);
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
	if(recv_packet(&clients[cur_client].sock, data, 512, cur_client) <= 0) {
        m->print(msg::MDEBUG, "net.cpp", "closing %s socket (%d)",
			clients[cur_client].is_active ? "active" : "inactive", cur_client);
		// send delete data to all clients
		if(clients[cur_client].is_active) {
			clients[cur_client].is_active = false;
			data[0] = net::DEL;
			data[1] = cur_client;
			for(unsigned int i = 0; i < net::max_clients; ++i) {
				if(clients[i].is_active) {
					send_packet(&clients[i].sock, data, 2, i);
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
				cdata[2] = cur_client;
				send_packet(&clients[cur_client].sock, cdata, 3, cur_client);

				// get client data
				memcpy(&clients[cur_client].ip.port, &data[1], 2);
				memcpy(&clients[cur_client].name, &data[4], 32);
				clients[cur_client].name[32] = 0;
				m->print(msg::MDEBUG, "net.cpp", "activating socket (%d: %s)",
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
				m->print(msg::MMSG, "net.cpp", "data (%d bytes) send from %s(%d): %s",
					len, clients[cur_client].name, cur_client, print_data);
				delete print_data;

				// send package to all clients except the one who send the data
				for(unsigned int i = 0; i < net::max_clients; i++) {
					if(clients[i].is_active && i != cur_client) {
						send_packet(&clients[i].sock, ndata, len+1 + 6, i);
					}
				}
			}
			break;

			default: {
				// unknown packet type
				m->print(msg::MDEBUG, "net.cpp", "a package with an unknown package type was send! type: %u", data[0]);
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
	send_packet(&clients[rcv_client].sock, data, 9+n, rcv_client);
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
		//strncpy(&data[4], client_name, len);
		memcpy(&data[4], client_name, len);
		data[4 + len++] = 0;

		// send the package to the server
		send_packet(&tcp_ssock, data, 4 + len, 0xFFFFFFFF);
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

/*! deletes a client of the server
 *  @param num the clients num
 */
void net::delete_client(unsigned int num) {
	// reset everything
	sprintf(clients[num].name, "unknown");
	clients[num].sock = NULL;
	clients[num].ip.port = 0;
	clients[num].ip.host = 0;
	clients[num].port = 0;

	clients[(MAX_CLIENTS-1)] = clients[num];
	for(unsigned int i = num; i < (MAX_CLIENTS-1); i++) {
		clients[i] = clients[i+1];
	}
}

void net::send_packet(TCPsocket* sock, char* data, int len, unsigned int client_num) {
	char* packet = new char[len+4];

	// add the header to the packet
	memcpy(&packet[0], &header, 4);

	// reorder the packet data
	for(int i = 0; i < len; i++) {
		memcpy(&packet[i+4], &data[i], 1);
	}

	// send the packet
	SDLNet_TCP_Send(*sock, packet, len+4);

	if(net::netlog) {
		char* dbg = new char[(len-4)*3+20];
		dbg[0] = 0;
		for(int i = 0; i < (len-4); i++) {
			sprintf(dbg, "%s %X%c", dbg, (unsigned int)(data[i] & 0xFF), 0x00);
		}

		time_t rawtime;
		struct tm* tinfo;
		time(&rawtime);
		tinfo = localtime(&rawtime);

		*(net::logfile) << "<";

		if(tinfo->tm_mday < 10) {
			*(net::logfile) << "0" << tinfo->tm_mday << ".";
		}
		else {
			*(net::logfile) << tinfo->tm_mday << ".";
		}

		if((tinfo->tm_mon+1) < 10) {
			*(net::logfile) << "0" << (tinfo->tm_mon+1) << ".";
		}
		else {
			*(net::logfile) << (tinfo->tm_mon+1) << ".";
		}

		*(net::logfile) << (tinfo->tm_year + 1900) << " ";

		if(tinfo->tm_hour < 10) {
			*(net::logfile) << "0" << tinfo->tm_hour << ":";
		}
		else {
			*(net::logfile) << tinfo->tm_hour << ":";
		}

		if(tinfo->tm_min < 10) {
			*(net::logfile) << "0" << tinfo->tm_min << ":";
		}
		else {
			*(net::logfile) << tinfo->tm_min << ":";
		}

		if(tinfo->tm_sec < 10) {
			*(net::logfile) << "0" << tinfo->tm_sec;
		}
		else {
			*(net::logfile) << tinfo->tm_sec;
		}

		char* cnum = new char[64];
		if(client_num != 0xFFFFFFFF) {
			sprintf(cnum, "%s(%u)%c", clients[client_num].name, client_num, 0x00);
		}
		else {
			sprintf(cnum, "server%c", 0x00);
		}

		*(net::logfile) << "> send packet (len: " << (len-4) << " / client: " << cnum << "): " << dbg << endl;

		delete cnum;

		delete dbg;
	}

	delete packet;
}

int net::recv_packet(TCPsocket* sock, char* data, int maxlen, unsigned int client_num) {
	char* packet = new char[maxlen + 4];
	unsigned int head = 0;

	// receive the package
	int len = SDLNet_TCP_Recv(*sock, packet, maxlen+4);
	// received packet length is equal or less than zero -> return 0
	if(len <= 0) {
		return 0;
	}

	// check header, if it's false -> return 0
	memcpy(&head, &packet[0], 4);
	if(head != header) {
		return 0;
	}

	// everything is okay, save packet to data and return the
	// packet length subtracted by the header length
	for(int i = 0; i < (len-4); i++) {
		memcpy(&data[i], &packet[i+4], 1);
	}

	if(net::netlog) {
		char* dbg = new char[(len-4)*3+20];
		dbg[0] = 0;
		for(int i = 0; i < (len-4); i++) {
			sprintf(dbg, "%s %X%c", dbg, (unsigned int)(data[i] & 0xFF), 0x00);
		}

		time_t rawtime;
		struct tm* tinfo;
		time(&rawtime);
		tinfo = localtime(&rawtime);

		*(net::logfile) << "<";

		if(tinfo->tm_mday < 10) {
			*(net::logfile) << "0" << tinfo->tm_mday << ".";
		}
		else {
			*(net::logfile) << tinfo->tm_mday << ".";
		}

		if((tinfo->tm_mon+1) < 10) {
			*(net::logfile) << "0" << (tinfo->tm_mon+1) << ".";
		}
		else {
			*(net::logfile) << (tinfo->tm_mon+1) << ".";
		}

		*(net::logfile) << (tinfo->tm_year + 1900) << " ";

		if(tinfo->tm_hour < 10) {
			*(net::logfile) << "0" << tinfo->tm_hour << ":";
		}
		else {
			*(net::logfile) << tinfo->tm_hour << ":";
		}

		if(tinfo->tm_min < 10) {
			*(net::logfile) << "0" << tinfo->tm_min << ":";
		}
		else {
			*(net::logfile) << tinfo->tm_min << ":";
		}

		if(tinfo->tm_sec < 10) {
			*(net::logfile) << "0" << tinfo->tm_sec;
		}
		else {
			*(net::logfile) << tinfo->tm_sec;
		}

		char* cnum = new char[64];
		if(client_num != 0xFFFFFFFF) {
			sprintf(cnum, "%s(%u)%c", clients[client_num].name, client_num, 0x00);
		}
		else {
			sprintf(cnum, "server%c", 0x00);
		}

		*(net::logfile) << "> recv packet (len: " << (len-4) << " / client: " << cnum << "): " << dbg << endl;

		delete cnum;

		delete dbg;
	}

	delete packet;
	return (len-4);
}

/*! sets the netlog state
 *  @param state the state of logging (true = enabled, false = disabled)
 */
void net::set_netlog(bool state) {
	net::netlog = state;
}

/*! returns true if network logging is enabled and false if it is disabled
 */
bool net::get_netlog() {
	return net::netlog;
}
