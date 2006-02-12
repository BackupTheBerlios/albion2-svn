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
#include <SDL/SDL.h>
#include <SDL/SDL_net.h>
#include <time.h>
#include <math.h>
#include "msg.h"
#include "engine.h"
using namespace std;

#define MAX_CLIENTS 32

#include "win_dll_export.h"

/*! @class net
 *  @brief networking class
 *  @author flo
 *  @author laxity
 *  @version 0.2.3
 *  @todo more functions
 *
 *  This is the network class
 */

class A2E_API net
{
public:
	net(engine* e);
	~net();

	bool init();
	void exit();

	bool create_server(unsigned int type, unsigned short int port, const unsigned int num_clients = 2);
	bool create_client(char* server, unsigned int type, unsigned short int port, char* client_name);
	void handle_server();
	void handle_client(unsigned int cur_client);
	void send_new_client(unsigned int snd_client, unsigned int rcv_client);
	void check_events();
	void send_activation(char* client_name);

	void close_socket(TCPsocket &sock);
	void close_socket(UDPsocket &sock);

	void delete_client(unsigned int num);

	void send_packet(TCPsocket* sock, char* data, int len, unsigned int client_num);
	int recv_packet(TCPsocket* sock, char* data, int maxlen, unsigned int client_num);

	// for debug purposes
	void set_netlog(bool state);
	bool get_netlog();

	//! protocol type
	enum PTYPE
	{
		TCP,	//!< enum tcp
		UDP		//!< enum udp - isn't supported atm
	};

	IPaddress server_ip;
	IPaddress local_ip;
	TCPsocket tcp_ssock;
	TCPsocket tcp_csock;
	UDPsocket udp_ssock;
	UDPsocket udp_csock;
	SDLNet_SocketSet socketset;

	//! server <-> client commands
	enum CMD {
		ADD,	//!< enum adds a client
		NEW,	//!< enum new client data
		DEL,	//!< enum delete client
		KICK,	//!< enum kicks a client
		DAT,	//!< enum data - can be used for "normal" data transfer
		CDAT	//!< enum client data
	};

	struct client {
		TCPsocket sock;
		unsigned short int port;
		IPaddress ip;
		char name[32];
		bool is_active;
	};

	client* clients;
	
protected:
	msg* m;
	engine* e;

	fstream* logfile;

	unsigned int max_clients;
	unsigned short int port;

	unsigned int header;

	bool netlog;
};

#endif
