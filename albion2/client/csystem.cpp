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

#include "csystem.h"

csystem::csystem(engine* e, camera* cam, cmap* cm) {
	csystem::e = e;
	csystem::c = e->get_core();
	csystem::m = e->get_msg();
	csystem::cam = cam;
	csystem::cm = cm;

	done = false;
	netinit = false;
	new_client = false;
	disconnected = false;
	logged_in = false;
	server = (char*)e->get_client_data()->server_name.c_str();
	port = e->get_client_data()->port;
	lis_port = e->get_client_data()->lis_port;
	client_name = e->get_client_data()->client_name;

	move_forward = false;
	move_back = false;
	move_timer = SDL_GetTicks();
}

csystem::~csystem() {
	chat_msgs.clear();
}

bool csystem::init_net() {
	bool ret = false;

	// net init
	if(n->init()) {
		m->print(msg::MDEBUG, "csystem.cpp", "init_net(): net class successfully initialized!");
		ret = true;
	}
	else {
		m->print(msg::MERROR, "csystem.cpp", "init_net(): couldn't initialize net class!");
		done = true;
	}

	return ret;
}

bool csystem::connect_client(const char* name, const char* pw) {
	bool ret = false;

	if(!netinit) {
		SDL_Delay(1000);
		if(n->create_client(server, port, lis_port)) { // TODO: put port number into config file
			m->print(msg::MDEBUG, "csystem.cpp", "connect_client(): successfully created client!");
			client_name = name;
			client_pw = pw;
			netinit = true;
			ret = true;
			new_client = true;
			disconnected = false;
		}
		else {
			m->print(msg::MERROR, "csystem.cpp", "connect_client(): couldn't create client!");
		}
	}

	return ret;
}

void csystem::add_chat_msg(unsigned int type, char* name, char* msg) {
	chat_msgs.push_back(*new csystem::chat_msg());
	chat_msgs.back().type = type;
	chat_msgs.back().name = name;
	chat_msgs.back().msg = msg;
}

void csystem::send_chat_msg(unsigned int type, char* msg) {
	send_msgs.push_back(*new csystem::chat_msg());
	send_msgs.back().type = type;
	send_msgs.back().msg = msg;
}

csystem::client* csystem::get_client(unsigned int id) {
	for(vector<client>::iterator cl_iter = clients.begin(); cl_iter != clients.end(); cl_iter++) {
		if(cl_iter->id == id) {
			return &*cl_iter;
		}
	}
	m->print(msg::MERROR, "csystem.cpp", "get_client(): no client with such an id (#%u) exists!", id);
	return NULL;
}

void csystem::add_flag(unsigned int flag) {
	flags.push_back(flag);
}

unsigned int csystem::get_flag() {
	if(flags.size() != 0) {
		unsigned int ret = flags.front();
		flags.erase(flags.begin());
		return ret;
	}

	return 0;
}

void csystem::run() {
	if(!logged_in) return;

	unsigned int flag = 0;
	while((flag = get_flag()) != 0) {
		switch(flag) {
			case CF_LOAD_MAP:
				cm->close_map();
				cm->load_map(get_client(client_id)->map);
				break;
			default:
				break;
		}
	}

	vertex3* pos = &(get_client(client_id)->position);
	cam->set_position(-pos->x, -pos->y - 0.75f, -pos->z);

	vertex3* rot = cam->get_rotation();
	get_client(client_id)->rotation.set(rot);
}
