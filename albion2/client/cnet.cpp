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

cnet::cnet(engine* e, csystem* cs, cgui* cg) {
	max_packet_size = 4096;

	buffer = new stringstream(stringstream::in | stringstream::out | stringstream::binary);
	data = new stringstream(stringstream::in | stringstream::out | stringstream::binary);

	cnet::e = e;
	cnet::c = e->get_core();
	cnet::m = e->get_msg();
	cnet::cs = cs;
	cnet::cg = cg;
	cnet::n = cs->n;
	cnet::agui = cs->agui;
}

cnet::~cnet() {
	delete buffer;
	delete data;
}

int cnet::process_packet(char* pdata, unsigned int max_len) {
	int used = 1;
	if(!n->check_header(pdata, net::HT_A2EN)) {
		return used;
	}

	buffer->str("");
	buffer->clear();
	c->put_block(buffer, pdata, max_len);
	buffer->seekg(4, stringstream::beg);

	int packet_len = c->get_int(buffer);
	unsigned int packet_type = c->get_uint(buffer);
	string tmp_str;

	switch(packet_type) {
		case cnet::PT_NEW_CLIENT: {
			unsigned int id = c->get_uint(buffer);
			unsigned int name_len = c->get_uint(buffer);
			c->get_block(buffer, &tmp_str, name_len);

			cs->clients.push_back(*new csystem::client());
			cs->clients.back().name = tmp_str;
			cs->clients.back().id = id;
			cs->clients.back().status = net::CS_ONLINE;
			cs->clients.back().get_map = true;
			cs->clients.back().get_pos = true;
			cs->clients.back().get_rot = true;
			if(cs->clients.back().name == cs->client_name) {
				cs->client_id = cs->clients.back().id;

				c->reset(data);
				c->put_uint(data, cnet::F_GET_ROT);
				c->put_uint(data, cs->client_id);
				send_packet(cnet::PT_FLAG);
				cs->clients.back().get_rot = false;
			}
			m->print(msg::MDEBUG, "main.cpp", "process_packet(): added new client (#%u, %s)!", cs->clients.back().id, cs->clients.back().name.c_str());
		}
		break;
		case cnet::PT_QUIT_CLIENT: {
			unsigned int id = c->get_uint(buffer);

			int i = -1;
			int del = 0;
			for(vector<csystem::client>::iterator cl_iter = cs->clients.begin(); cl_iter != cs->clients.end(); cl_iter++) {
				i++;
				if(id == cl_iter->id) {
					del = i;
				}
			}
			if(i != -1) {
				m->print(msg::MDEBUG, "main.cpp", "process_packet(): client quit (#%u, %s)!", cs->clients[del].id, cs->clients[del].name.c_str());
				cs->clients.erase(cs->clients.begin()+del);
			}
		}
		break;
		case cnet::PT_CHAT_MSG: {
			cgui::CHAT_TYPE type = (cgui::CHAT_TYPE)c->get_uint(buffer);

			unsigned int id = c->get_uint(buffer);
			string name = "unknown";
			for(vector<csystem::client>::iterator cl_iter = cs->clients.begin(); cl_iter != cs->clients.end(); cl_iter++) {
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

			switch(flag) {
				case cnet::F_SUCCESS_LOGIN:
					// if needed?
					if(cg->get_gui_state() != cgui::GS_GAME) {
						cg->set_gui_state(cgui::GS_GAME);
						cg->load_game_gui();
					}
					cs->logged_in = true;
					break;
				case cnet::F_WRONG_UNAME:
					agui->add_msgbox_ok("system message", "you entered a user name that doesn't exist!");
					break;
				case cnet::F_WRONG_PW:
					agui->add_msgbox_ok("system message", "you entered a wrong password!");
					break;
				case cnet::F_POST_MAP: {
					if(!cs->logged_in) break;

					unsigned int cid = c->get_uint(buffer);
					csystem::client* client = cs->get_client(cid);
					client->map = c->get_uint(buffer);

					if(cid == cs->client_id) {
						cs->add_flag(csystem::CF_LOAD_MAP);
					}
				}
				break;
				case cnet::F_POST_POS: {
					if(!cs->logged_in) break;

					unsigned int cid = c->get_uint(buffer);
					csystem::client* client = cs->get_client(cid);
					c->get_block(buffer, (char*)&client->position, sizeof(vertex3));
					client->get_pos = true;
				}
				break;
				case cnet::F_POST_ROT: {
					if(!cs->logged_in) break;

					unsigned int cid = c->get_uint(buffer);
					csystem::client* client = cs->get_client(cid);
					c->get_block(buffer, (char*)&client->rotation, sizeof(vertex3));
					if(cid != cs->client_id) client->get_rot = true;
					else {
						cs->cam->get_rotation()->set(&client->rotation);

						c->reset(data);
						c->put_uint(data, cnet::F_POST_ROT);
						c->put_uint(data, cs->client_id);
						c->put_block(data, (const char*)&(cs->get_client(cs->client_id)->rotation), sizeof(vertex3));
						send_packet(cnet::PT_FLAG);
					}
				}
				break;
				case cnet::F_GET_ROT: {
					if(!cs->logged_in) break;

					c->reset(data);
					c->put_uint(data, cnet::F_POST_ROT);
					c->put_uint(data, cs->client_id);
					c->put_block(data, (const char*)&(cs->get_client(cs->client_id)->rotation), sizeof(vertex3));
					send_packet(cnet::PT_FLAG);
				}
				break;
				default:
					//m->print(msg::MERROR, "cnet.cpp", "process_packet(): unknown flag type %u!", flag);
					break;
			}
		}
		break;
		case cnet::PT_VU_LIST: {
			cs->vis_user.clear();

			unsigned int size = c->get_uint(buffer);
			for(unsigned int i = 0; i < size; i++) {
				cs->vis_user.push_back(c->get_uint(buffer));
			}
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
		case cnet::PT_FLAG: {
			if(n->tcp_server_sock == NULL) break;

			c->put_uint(buffer, cnet::PT_FLAG);
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
	char* pdata = new char[max_packet_size];
	int pos, len, used;

	// checks if client is still connected to the server
	len = n->recv_packet(&n->tcp_server_sock, pdata, max_packet_size, 0xFFFFFFFF);
	if(len <= 0) {
		n->close_socket(n->tcp_server_sock);
	}
	else {
		pos = 0;
		while(len > 0) {
			used = process_packet(&pdata[pos], len);

			pos += used;
			len -= used;
			if(used == 0) {
				// lost data ...
				m->print(msg::MDEBUG, "main.cpp", "lost data - server down or disconnect? (handle server routine)");
				len = 0;
			}
		}
	}
	delete [] pdata;
}

void cnet::run() {
	// check if there are any new messages to send ...
	if(cs->send_msgs.size() != 0) {
		// if so, send messages ...
		for(vector<csystem::chat_msg>::iterator cm_iter = cs->send_msgs.begin(); cm_iter != cs->send_msgs.end(); cm_iter++) {
			c->reset(data);
			c->put_uint(data, (cgui::CHAT_TYPE)cm_iter->type);
			c->put_uint(data, (unsigned int)cm_iter->msg.length());
			c->put_block(data, cm_iter->msg.c_str(), (unsigned int)cm_iter->msg.length());
			send_packet(cnet::PT_CHAT_MSG);
		}
		// all messages sent, clear msg buffer
		cs->send_msgs.clear();
	}

	// get positions and rotations of visible users
	for(vector<unsigned int>::iterator iter = cs->vis_user.begin(); iter != cs->vis_user.end(); iter++) {
		csystem::client* client = cs->get_client(*iter);
		if(client->get_pos) {
			c->reset(data);
			c->put_uint(data, cnet::F_GET_POS);
			c->put_uint(data, *iter);
			send_packet(cnet::PT_FLAG);

			client->get_pos = false;
		}
		if(client->get_rot) {
			c->reset(data);
			c->put_uint(data, cnet::F_GET_ROT);
			c->put_uint(data, *iter);
			send_packet(cnet::PT_FLAG);

			client->get_rot = false;
		}
		if(client->get_map) {
			c->reset(data);
			c->put_uint(data, cnet::F_GET_MAP);
			c->put_uint(data, *iter);
			send_packet(cnet::PT_FLAG);

			client->get_map = false;
		}
	}

	if(SDL_GetTicks() - cs->move_timer >= 200) {
		cs->move_timer = SDL_GetTicks();

		if(cs->move_forward) {
			c->reset(data);
			c->put_uint(data, cnet::F_MOVE_FORWARD);
			c->put_uint(data, cs->client_id);
			send_packet(cnet::PT_FLAG);
		}
		else if(cs->move_back) {
			c->reset(data);
			c->put_uint(data, cnet::F_MOVE_BACK);
			c->put_uint(data, cs->client_id);
			send_packet(cnet::PT_FLAG);
		}
	}
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
