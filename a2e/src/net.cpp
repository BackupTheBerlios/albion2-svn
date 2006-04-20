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

#include "net.h"

/*! there is no function currently
 */
net::net(engine* e) {
	max_clients = 0;
	header = 0x4132454E;
	rheader = 0x4E453241;

	client_counter = 0; // used for assigning an id to new clients

	netlog = false;
	logfile = new fstream();
	logfile->open("netlog.txt", fstream::out);

	buffer = new stringstream(stringstream::in | stringstream::out | stringstream::binary);

	// get classes
	net::e = e;
	net::c = e->get_core();
	net::m = e->get_msg();
}

/*! there is no function currently
 */
net::~net() {
	m->print(msg::MDEBUG, "net.cpp", "freeing net stuff");

	logfile->close();
	delete logfile;

	delete buffer;

	m->print(msg::MDEBUG, "net.cpp", "net stuff freed");
}

/*! initialize the networking functions - returns true if successful, otherwise false
 */
bool net::init() {
	if(SDLNet_Init() == -1) {
		m->print(msg::MERROR, "net.cpp", "SDLNet_Init(): %s", SDLNet_GetError());
		return false;
	}

	return true;
}

/*! shutdown and cleanup all networking stuff
 */
void net::exit() {
	SDLNet_Quit();

	clients.clear();
}

/*! creates an server
 *  @param port the port that is being used
 *  @param max_clients the maximal amount of people who can connect to the server
 */
bool net::create_server(unsigned short int port, const unsigned int max_clients) {
	net::max_clients = max_clients;
	net::port = port;

	if(SDLNet_ResolveHost(&local_ip, NULL, port) == -1) {
		m->print(msg::MERROR, "net.cpp", "create_server(): SDLNet_ResolveHost(): %s", SDLNet_GetError());
		return false;
	}

	// client stuff
	clients.reserve(net::max_clients);

	// initialize socketset
	socketset = SDLNet_AllocSocketSet(net::max_clients);
	if(socketset == NULL) {
		m->print(msg::MERROR, "net.cpp", "create_server(): SDLNet_AllocSocketSet(): couldn't create socket set: %s", SDLNet_GetError());
		return false;
	}

	// listening socket @localhost
	tcp_server_sock = SDLNet_TCP_Open(&local_ip);
	if(!tcp_server_sock) {
		m->print(msg::MERROR, "net.cpp", "create_server(): SDLNet_TCP_Open(): %s", SDLNet_GetError());
		return false;
	}
	SDLNet_TCP_AddSocket(socketset, tcp_server_sock);

	// everything is set up now - the clients are now able to connect to the server

	return true;
}

/*! creates a client
 *  @param server_name the name/ip of the server
 *  @param port the server port
 *  @param lis_port the local (listening) port
 *  @param client_name the clients' name
 */
bool net::create_client(char* server_name, unsigned short int port, unsigned short int lis_port) {
	net::max_clients = 256;
	if(SDLNet_ResolveHost(&server_ip, server_name, port) == -1) {
		m->print(msg::MERROR, "net.cpp", "SDLNet_ResolveHost() (client->server): %s", SDLNet_GetError());
		return false;
	}

	if(SDLNet_ResolveHost(&local_ip, NULL, lis_port) == -1) {
		m->print(msg::MERROR, "net.cpp", "SDLNet_ResolveHost() (client->server): %s", SDLNet_GetError());
		return false;
	}

	// client stuff
	clients.reserve(net::max_clients);

	// initialize socketset
	socketset = SDLNet_AllocSocketSet(2);
	if(socketset == NULL) {
		m->print(msg::MERROR, "net.cpp", "create_client(): couldn't create socket set: %s", SDLNet_GetError());
		return false;
	}

	// create server tcp socket
	tcp_server_sock = SDLNet_TCP_Open(&server_ip);
	if(!tcp_server_sock) {
		m->print(msg::MERROR, "net.cpp", "create_client(): SDLNet_TCP_Open() (server): %s", SDLNet_GetError());
		return false;
	}

	// connect to server
	tcp_client_sock = SDLNet_TCP_Open(&local_ip);
	if(!tcp_client_sock) {
		m->print(msg::MERROR, "net.cpp", "create_client(): SDLNet_TCP_Open (client->server): %s", SDLNet_GetError());
		return false;
	}

	// connection created - data transfer is now possible
	m->print(msg::MDEBUG, "net.cpp", "create_client(): SDLNet_TCP_Open (client->server): successfully connected to server!");

	SDLNet_TCP_AddSocket(socketset, tcp_server_sock);
	SDLNet_TCP_AddSocket(socketset, tcp_client_sock);

	return true;
}

/*! handles the server - check for client inactivity and kick it
 */
void net::handle_server() {
	// handle clients (delete on inactivity)
	del_cls.clear();
	for(vector<client>::iterator cl_iter = clients.begin(); cl_iter != clients.end(); cl_iter++) {
		// client got offline/inactive since 30 sec or quit
		if((cl_iter->status == net::CS_OFFLINE && time(NULL) - cl_iter->login_time > 30) ||
			time(NULL) - cl_iter->last_time > 30 || cl_iter->quit) {
			// add client to the client delete list/vector
			del_cls.push_back(*new vector<net::client>::iterator());
			del_cls.back() = cl_iter;
 		}
	}
	// long life the complexity of stl ...
	for(vector<vector<net::client>::iterator>::reverse_iterator del_iter = del_cls.rbegin(); del_iter != del_cls.rend(); del_iter++) {
		m->print(msg::MDEBUG, "net.cpp", "handle_server(): removed client #%u (IP: %u.%u.%u.%u)!", (*del_iter)->id,
			(unsigned int)((*del_iter)->ip.host & 0xFF), (unsigned int)(((*del_iter)->ip.host >> 8) & 0xFF),
			(unsigned int)(((*del_iter)->ip.host >> 16) & 0xFF), (unsigned int)(((*del_iter)->ip.host >> 24) & 0xFF));
		if((*del_iter)->sock != NULL) {
			close_socket((*del_iter)->sock);
		}
		clients.erase(*del_iter);
	}
}

/*! adds a client the server - if there is a new connection, check if the max user limit
 *! is reached (and try to disconnect inactive clients -> handle_server). if there is a free "slot", it
 *! adds the client to the "client list". otherwise it will kick the new client.
 */
bool net::add_client() {
	TCPsocket tmp_sock;
	char* data = new char[1];

	// handle new client
	tmp_sock = SDLNet_TCP_Accept(tcp_server_sock);
	if(tmp_sock == NULL) return false;

	if(clients.size() == net::max_clients) {
		// all slots are being used
		// TODO: send kick msg
		//data[0] = net::KICK;
		//send_packet(&tmp_sock, data, 1, 0xFFFFFFFF);
		SDLNet_TCP_Close(tmp_sock);
		m->print(msg::MDEBUG, "net.cpp", "handle_server(): no further connection possible, all slots are being used!");
		return false;
	}
	else {
		// add socket to socketset and make it inactive
		clients.push_back(*new net::client());
		clients.back().sock = tmp_sock;
		clients.back().ip = *SDLNet_TCP_GetPeerAddress(tmp_sock);
		clients.back().status = net::CS_OFFLINE;
		clients.back().name = "unknown";
		clients.back().login_time = (unsigned int)time(NULL);
		clients.back().last_time = (unsigned int)time(NULL);
		//clients.back().id = (unsigned int)clients.size()-1;
		clients.back().id = client_counter;
		client_counter++;
		clients.back().quit = false;
		SDLNet_TCP_AddSocket(socketset, clients.back().sock);
		m->print(msg::MDEBUG, "net.cpp", "handle_server(): added new inactive socket (#%d, IP: %u.%u.%u.%u)!",
			clients.back().id, (unsigned int)(clients.back().ip.host & 0xFF), (unsigned int)((clients.back().ip.host >> 8) & 0xFF),
			(unsigned int)((clients.back().ip.host >> 16) & 0xFF), (unsigned int)((clients.back().ip.host >> 24) & 0xFF));
	}

	return true;
}

net::client* net::get_client(const char* name) {
	for(vector<client>::iterator cl_iter = clients.begin(); cl_iter != clients.end(); cl_iter++) {
		if(strcmp(name, cl_iter->name.c_str()) == 0) {
			return &*cl_iter;
		}
	}
	m->print(msg::MERROR, "net.cpp", "get_client(): no client with such a name (%s) exists!", name);
	return NULL;
}

vector<net::client>* net::get_clients() {
	return &(net::clients);
}

/*! closes and deletes the tcp socket
 *  @param sock the tcp socket that should be closed
 */
void net::close_socket(TCPsocket &sock) {
	SDLNet_TCP_DelSocket(socketset, sock);
	SDLNet_TCP_Close(sock);
	sock = NULL;
}

void net::send_packet(TCPsocket* sock, const char* data, int len, unsigned int client_num, bool a2en) {
	if(a2en) {
		buffer->str("");
		buffer->clear();

		// add the header and packet length to the packet
		c->put_uint(buffer, header);
		c->put_int(buffer, len);

		c->put_block(buffer, &data[0], (unsigned int)len);

		// send the packet
		SDLNet_TCP_Send(*sock, (void*)buffer->str().c_str(), len+8);

		if(net::netlog) {
			char* dbg = new char[(len-8)*3+20];
			dbg[0] = 0;
			for(int i = 0; i < (len-8); i++) {
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

			delete [] cnum;

			delete [] dbg;
		}
	}
	else {
		// send the packet
		SDLNet_TCP_Send(*sock, (void*)data, len);
	}
}

int net::recv_packet(TCPsocket* sock, char* data, int maxlen, unsigned int client_num) {
	char* packet = new char[maxlen];

	// receive the package
	int len = SDLNet_TCP_Recv(*sock, packet, maxlen);
	// received packet length is equal or less than zero -> return 0
	if(len <= 0) {
		delete [] packet;
		return len;
	}

	if(client_num != 0xFFFFFFFF) {
		clients[client_num].last_time = (unsigned int)time(NULL);
	}

	/*// check header, if it's false -> return 0
	memcpy(&head, &packet[0], 4);
	if(head != header) {
		delete [] packet;
		return 0;
	}*/

	// everything is okay, save packet to data and return the
	// packet length subtracted by the header length
	/*for(int i = 0; i < (len-4); i++) {
		memcpy(&data[i], &packet[i+4], 1);
	}*/
	memcpy(&data[0], &packet[0], len);

	if(net::netlog) {
		char* dbg = new char[(len-0)*3+20];
		dbg[0] = 0;
		for(int i = 0; i < (len-0); i++) {
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

		delete [] dbg;
	}

	delete [] packet;
	return (len-0);
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

bool net::check_header(char* data, HEADER_TYPE htype) {
	switch(htype) {
		case net::HT_A2EN: {
			unsigned int head = 0;
			memcpy(&head, &data[0], 4);
			if(head != rheader) {
				return false;
			}
		}
		break;
		case net::HT_HTTP: {
			char* head = new char[5];
			//memcpy(&head, &data, 4);
			head[0] = data[0];
			head[1] = data[1];
			head[2] = data[2];
			head[3] = data[3];
			head[4] = 0;
			if(strcmp(head, "GET ") != 0 && strcmp(head, "POST") != 0) {
				delete [] head;
				return false;
			}
			delete [] head;
		}
		break;
		default:
			return false;
			break;
	}
	return true;
}
