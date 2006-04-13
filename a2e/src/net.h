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
 
#ifndef __NET_H__
#define __NET_H__

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>
#include <SDL/SDL.h>
#include <SDL/SDL_net.h>
#include <time.h>
#include "msg.h"
#include "engine.h"
using namespace std;

#define MAX_CLIENTS 32

#include "win_dll_export.h"

/*! @class net
 *  @brief networking class
 *  @author flo
 *  @todo more functions
 *
 *  This is the network class
 */

class A2E_API net
{
public:
	net(engine* e);
	~net();

	struct client {
		TCPsocket sock;
		IPaddress ip;
		string name;
		unsigned int status;
		unsigned int login_time;
		unsigned int last_time;
		unsigned int id;
		bool quit;
	};

	enum CLIENT_STATUS {
		CS_OFFLINE,
		CS_ONLINE
	};

	enum HEADER_TYPE {
		HT_A2EN,
		HT_HTTP
	};


	bool init();
	void exit();

	bool create_server(unsigned short int port, const unsigned int max_clients = 16);
	bool create_client(char* server_name, unsigned short int port, unsigned short int lis_port);

	client* get_client(const char* name);
	vector<client>* get_clients();

	void handle_server();
	bool add_client();

	void close_socket(TCPsocket &sock);

	void send_packet(TCPsocket* sock, const char* data, int len, unsigned int client_num, bool a2en = true);
	int recv_packet(TCPsocket* sock, char* data, int maxlen, unsigned int client_num);
	bool check_header(char* data, HEADER_TYPE htype);

	IPaddress server_ip;
	IPaddress local_ip;
	TCPsocket tcp_server_sock;
	TCPsocket tcp_client_sock;
	SDLNet_SocketSet socketset;

	/*//! server <-> client commands
	enum CMD {
		ADD,	//!< enum adds a client
		NEW,	//!< enum new client data
		DEL,	//!< enum delete client
		KICK,	//!< enum kicks a client
		DAT,	//!< enum data - can be used for "normal" data transfer
		CDAT	//!< enum client data
	};*/

	// for debug purposes
	void set_netlog(bool state);
	bool get_netlog();
	
protected:
	msg* m;
	engine* e;
	core* c;
	fstream* logfile;
	unsigned int header;
	unsigned int rheader; // reverse header
	bool netlog;

	unsigned int max_clients;
	unsigned short int port;
	vector<client> clients;
	vector<vector<client>::iterator> del_cls;
	unsigned int client_counter;

	stringstream* buffer;


};

#endif
