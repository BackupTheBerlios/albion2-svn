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

#include "cnet.h"

cnet::cnet(engine* e, csystem* cs) {
	max_packet_size = 4096;

	buffer = new stringstream(stringstream::in | stringstream::out | stringstream::binary);
	data = new stringstream(stringstream::in | stringstream::out | stringstream::binary);

	cnet::e = e;
	cnet::c = e->get_core();
	cnet::m = e->get_msg();
	cnet::cs = cs;
	cnet::n = cs->n;
}

cnet::~cnet() {
	delete buffer;
	delete data;
}

int cnet::process_packet(char* data, unsigned int max_len) {
	int used = 1;
	if(!n->check_header(data, net::HT_A2EN)) {
		return used;
	}

	buffer->str("");
	buffer->clear();
	c->put_block(buffer, data, max_len);
	buffer->seekg(4, stringstream::beg);

	int packet_len = c->get_int(buffer);
	unsigned int packet_type = c->get_uint(buffer);
	string tmp_str;

	switch(packet_type) {
		case cnet::PT_NEW_CLIENT: {
			unsigned int id = c->get_uint(buffer);
			unsigned int name_len = c->get_uint(buffer);
			c->get_block(buffer, &tmp_str, name_len);

			clients.push_back(*new client());
			clients.back().name = tmp_str;
			clients.back().id = id;
			clients.back().status = net::CS_ONLINE;
			m->print(msg::MDEBUG, "main.cpp", "process_packet(): added new client (#%u, %s)!", clients.back().id, clients.back().name.c_str());
		}
		break;
		case cnet::PT_QUIT_CLIENT: {
			unsigned int id = c->get_uint(buffer);

			int i = -1;
			int del = 0;
			for(vector<client>::iterator cl_iter = clients.begin(); cl_iter != clients.end(); cl_iter++) {
				i++;
				if(id == cl_iter->id) {
					del = i;
				}
			}
			if(i != -1) {
				m->print(msg::MDEBUG, "main.cpp", "process_packet(): client quit (#%u, %s)!", clients[del].id, clients[del].name.c_str());
				clients.erase(clients.begin()+del);
			}
		}
		break;
		case cnet::PT_CHAT_MSG: {
			cnet::CHAT_TYPE type = (cnet::CHAT_TYPE)c->get_uint(buffer);

			unsigned int id = c->get_uint(buffer);
			string name = "unknown";
			for(vector<client>::iterator cl_iter = clients.begin(); cl_iter != clients.end(); cl_iter++) {
				if(id == cl_iter->id) {
					name = cl_iter->name;
				}
			}

			unsigned int msg_len = c->get_uint(buffer);
			char* chat_msg = new char[msg_len+1];
			chat_msg[msg_len] = 0;
			c->get_block(buffer, chat_msg, msg_len);

			cs->add_chat_msg(type, (char*)name.c_str(), chat_msg);
			delete [] chat_msg;
		}
		break;
		case cnet::PT_FLAG: {
			unsigned int flag = c->get_uint(buffer);
			cs->flags.push_back(*new unsigned int(flag));
		}
		break;
		default: {
			m->print(msg::MERROR, "main.cpp", "process_packet(): unknown packet type (%u)!", packet_type);
		}
		break;
	}


	used = 4 + 4 + packet_len;
	return used;
}

void cnet::send_packet(cnet::PACKET_TYPE type) {
	buffer->str("");
	buffer->clear();

	switch(type) {
		case cnet::PT_TEST: {
			if(n->tcp_server_sock == NULL) break;

			c->put_uint(buffer, cnet::PT_TEST);
		}
		break;
		case cnet::PT_NEW_CLIENT: {
			if(n->tcp_server_sock == NULL) break;

			c->put_uint(buffer, cnet::PT_NEW_CLIENT);
			c->put_uint(buffer, (unsigned int)cs->client_name.size());
			c->put_block(buffer, cs->client_name.c_str(), (unsigned int)cs->client_name.size());
			c->put_uint(buffer, (unsigned int)cs->client_pw.size());
			c->put_block(buffer, cs->client_pw.c_str(), (unsigned int)cs->client_pw.size());
		}
		break;
		case cnet::PT_QUIT_CLIENT: {
			if(n->tcp_server_sock == NULL) break;

			c->put_uint(buffer, cnet::PT_QUIT_CLIENT);
		}
		break;
		case cnet::PT_CHAT_MSG: {
			if(n->tcp_server_sock == NULL) break;

			c->put_uint(buffer, cnet::PT_CHAT_MSG);
			c->put_block(buffer, data->str().c_str(), (unsigned int)data->str().size());
		}
		break;
		default:
			m->print(msg::MERROR, "main.cpp", "send_packet(): unknown packet type (%u)!", type);
			return;
			break;
	}

	// send packet
	n->send_packet(&n->tcp_server_sock, buffer->str().c_str(), (unsigned int)buffer->str().size(), 0xFFFFFFFF);
}

void cnet::handle_server() {
	char* data = new char[max_packet_size];
	int pos, len, used;

	// checks if client is still connected to the server
	len = n->recv_packet(&n->tcp_server_sock, data, max_packet_size, 0xFFFFFFFF);
	if(len <= 0) {
		n->close_socket(n->tcp_server_sock);
	}
	else {
		pos = 0;
		while(len > 0) {
			used = process_packet(&data[pos], len);

			pos += used;
			len -= used;
			if(used == 0) {
				// lost data ...
				m->print(msg::MDEBUG, "main.cpp", "lost data - server down or disconnect? (handle server routine)");
				len = 0;
			}
		}
	}
	delete [] data;
}

stringstream* cnet::get_data() {
	return data;
}

void cnet::clear_data() {
	data->str("");
	data->clear();
}

void cnet::close() {
	if(n->tcp_server_sock != NULL) n->close_socket(n->tcp_server_sock);
	if(n->tcp_client_sock != NULL) n->close_socket(n->tcp_client_sock);
}
