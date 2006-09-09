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

#include "snet.h"

snet::snet(engine* e, net* n, userman* um, web* w, smap* sm) {
	max_packet_size = 4096;

	buffer = new stringstream(stringstream::in | stringstream::out | stringstream::binary);
	data = new stringstream(stringstream::in | stringstream::out | stringstream::binary);
	packet_data = new char[max_packet_size];
	line = new char[65536];

	snet::e = e;
	snet::c = e->get_core();
	snet::m = e->get_msg();
	snet::n = n;
	snet::um = um;
	snet::w = w;
	snet::sm = sm;

	piover180 = (float)PI / 180.0f;
}

snet::~snet() {
	delete buffer;
	delete data;
	delete [] packet_data;
	delete [] line;
}

int snet::process_packet(unsigned int cnum, char* pdata, unsigned int max_len) {
	int used = 1;
	if(!n->check_header(pdata, net::HT_A2EN)) {
		return used;
	}

	c->reset(buffer);
	c->put_block(buffer, pdata, max_len);
	buffer->seekg(4, stringstream::beg);

	int packet_len = c->get_int(buffer);
	unsigned int packet_type = c->get_uint(buffer);
	string tmp_str;

	switch(packet_type) {
		case snet::PT_TEST: {
			// test package ...
		}
		break;
		case snet::PT_NEW_CLIENT: {
			unsigned int name_len = c->get_uint(buffer);
			c->get_block(buffer, &tmp_str, name_len);
			n->get_clients()->at(cnum).name = tmp_str;
			const char* client_name = n->get_clients()->at(cnum).name.c_str();
			userman::user_data* user = um->get_user(client_name);
			tmp_str = "";
			// check if user exists
			if(!um->check_user(client_name)) {
				// user doesn't exist, quit client
				n->get_clients()->at(cnum).quit = true;

				// send "wrong user name" flag
				c->reset(data);
				c->put_uint(data, F_WRONG_UNAME);
				send_packet(cnum, 0, snet::PT_FLAG);
				break;
			}

			tmp_str = "";
			unsigned int pw_len = c->get_uint(buffer);
			c->get_block(buffer, &tmp_str, pw_len);
			// check if user entered the right password
			if(!um->check_pw(client_name, tmp_str.c_str())) {
				// wrong pw, quit client
				n->get_clients()->at(cnum).quit = true;

				// send "wrong pw" flag
				c->reset(data);
				c->put_uint(data, F_WRONG_PW);
				send_packet(cnum, 0, snet::PT_FLAG);
				break;
			}

			n->get_clients()->at(cnum).status = net::CS_ONLINE;
			n->get_clients()->at(cnum).last_time = (unsigned int)time(NULL);
			m->print(msg::MDEBUG, "main.cpp", "process_packet(): activated client (#%u, %s)!",
				n->get_clients()->at(cnum).id, n->get_clients()->at(cnum).name.c_str());

			// send new client data to all clients
			unsigned int i = 0;
			for(vector<net::client>::iterator cl_iter = n->get_clients()->begin(); cl_iter != n->get_clients()->end(); cl_iter++) {
				//if(i != cnum) {
				// send data to client itself too ...
					send_packet(i, cnum, snet::PT_NEW_CLIENT);
				//}
				i++;
			}

			// send client data of all clients to the new client
			i = 0;
			for(vector<net::client>::iterator cl_iter = n->get_clients()->begin(); cl_iter != n->get_clients()->end(); cl_iter++) {
				if(i != cnum) {
					send_packet(cnum, i, snet::PT_NEW_CLIENT);
				}
				i++;
			}

			// send "success login" flag
			c->reset(data);
			c->put_uint(data, F_SUCCESS_LOGIN);
			send_packet(cnum, 0, snet::PT_FLAG);

			// set net id
			user->nid = cnum;

			// set user active
			user->active = true;

			// load model file (the physical object of the player)
			user->obj = new a2emodel(e, NULL);
			user->obj->load_model(e->data_path("player_sphere.a2m"), false);
			user->obj->set_radius(2.0f);
			user->obj->get_position()->set(&user->position);
			user->obj->get_rotation()->set(&user->rotation);

			sm->add_user_phys_obj(client_name);
		}
		break;
		case snet::PT_QUIT_CLIENT: {
			n->get_clients()->at(cnum).quit = true;
			const char* client_name = n->get_clients()->at(cnum).name.c_str();

			m->print(msg::MDEBUG, "main.cpp", "process_packet(): quitting client (#%u, %s)!",
				n->get_clients()->at(cnum).id, client_name);

			unsigned int i = 0;
			for(vector<net::client>::iterator cl_iter = n->get_clients()->begin(); cl_iter != n->get_clients()->end(); cl_iter++) {
				if(i != cnum) {
					send_packet(i, cnum, snet::PT_QUIT_CLIENT);
				}
				i++;
			}

			// reset net id
			um->get_user(client_name)->nid = -1;

			// set user inactive
			um->get_user(client_name)->active = false;

			delete um->get_user(client_name)->obj;
			sm->del_user_phys_obj(client_name);
		}
		break;
		case snet::PT_CHAT_MSG: {
			snet::CHAT_TYPE type = (snet::CHAT_TYPE)c->get_uint(buffer);
			unsigned int msg_len = c->get_uint(buffer);
			char* chat_msg = new char[msg_len+1];
			chat_msg[msg_len] = 0;
			c->get_block(buffer, chat_msg, msg_len);

			c->reset(data);
			c->put_uint(data, snet::CT_WORLD);
			c->put_uint(data, n->get_clients()->at(cnum).id);
			c->put_uint(data, msg_len);
			c->put_block(data, chat_msg, msg_len);
			unsigned int i = 0;
			for(vector<net::client>::iterator cl_iter = n->get_clients()->begin(); cl_iter != n->get_clients()->end(); cl_iter++) {
				if(i != cnum) {
					send_packet(i, cnum, snet::PT_CHAT_MSG);
				}
				i++;
			}

			m->print(msg::MDEBUG, "main.cpp", "process_packet(): received chat msg (#%u, %s): %s!",
				n->get_clients()->at(cnum).id, n->get_clients()->at(cnum).name.c_str(), chat_msg);

			delete [] chat_msg;
		}
		break;
		case snet::PT_FLAG: {
			unsigned int flag = c->get_uint(buffer);
			unsigned int client = c->get_uint(buffer);
			userman::user_data* user = um->get_user(client);

			switch(flag) {
				case snet::F_GET_MAP:
					c->reset(data);
					c->put_uint(data, snet::F_POST_MAP);
					c->put_uint(data, client);
					c->put_uint(data, user->map);
					send_packet(cnum, 0, snet::PT_FLAG);
					break;
				case snet::F_GET_POS:
					c->reset(data);
					c->put_uint(data, snet::F_POST_POS);
					c->put_uint(data, client);
					c->put_block(data, (const char*)&user->position, sizeof(vertex3));
					send_packet(cnum, 0, snet::PT_FLAG);
					break;
				case snet::F_GET_ROT:
					c->reset(data);
					c->put_uint(data, snet::F_POST_ROT);
					c->put_uint(data, client);
					c->put_block(data, (const char*)&user->rotation, sizeof(vertex3));
					send_packet(cnum, 0, snet::PT_FLAG);
					break;
				case snet::F_POST_ROT:
					c->get_block(buffer, (char*)&(user->rotation), sizeof(vertex3));

					c->reset(data);
					c->put_uint(data, snet::F_GET_ROT);
					send_packet(cnum, 0, snet::PT_FLAG);
					break;
				case snet::F_MOVE_FORWARD: {
					float x = -sinf(user->rotation.y * piover180);
					float y = sinf(user->rotation.x * piover180);
					float z = -cosf(user->rotation.y * piover180);
					//cout << x << " - " << y << " - " << z << endl;
					user->phys_obj->add_force(x, y, z);
				}
				break;
				case snet::F_MOVE_BACK: {
					float x = sinf(user->rotation.y * piover180);
					float y = -sinf(user->rotation.x * piover180);
					float z = cosf(user->rotation.y * piover180);
					//cout << x << " - " << y << " - " << z << endl;
					user->phys_obj->add_force(x, y, z);
				}
				break;
				default:
					break;
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

void snet::send_packet(unsigned int cnum, unsigned int inum, snet::PACKET_TYPE type) {
	c->reset(buffer);
	switch(type) {
		// new client connection, send information to other clients
		case snet::PT_NEW_CLIENT: {
			if(n->get_clients()->at(cnum).sock == NULL && n->get_clients()->at(cnum).status == net::CS_ONLINE) break;

			c->put_uint(buffer, snet::PT_NEW_CLIENT);
			c->put_uint(buffer, um->get_user(n->get_clients()->at(inum).name.c_str())->id);
			c->put_uint(buffer, (unsigned int)n->get_clients()->at(inum).name.size());
			c->put_block(buffer, n->get_clients()->at(inum).name.c_str(), (unsigned int)n->get_clients()->at(inum).name.size());
		}
		break;
		case snet::PT_QUIT_CLIENT: {
			if(n->get_clients()->at(cnum).sock == NULL && n->get_clients()->at(cnum).status == net::CS_ONLINE) break;

			c->put_uint(buffer, snet::PT_QUIT_CLIENT);
			c->put_uint(buffer, n->get_clients()->at(inum).id);
		}
		break;
		case snet::PT_CHAT_MSG: {
			if(n->get_clients()->at(cnum).sock == NULL && n->get_clients()->at(cnum).status == net::CS_ONLINE) break;

			c->put_uint(buffer, snet::PT_CHAT_MSG);
			c->put_block(buffer, data->str().c_str(), (unsigned int)data->str().size());
		}
		break;
		case snet::PT_FLAG: {
			if(n->get_clients()->at(cnum).sock == NULL && n->get_clients()->at(cnum).status == net::CS_ONLINE) break;

			c->put_uint(buffer, snet::PT_FLAG);
			c->put_block(buffer, data->str().c_str(), (unsigned int)data->str().size());
		}
		break;
		case snet::PT_VU_LIST: {
			if(n->get_clients()->at(cnum).sock == NULL && n->get_clients()->at(cnum).status == net::CS_ONLINE) break;

			c->put_uint(buffer, snet::PT_VU_LIST);
			c->put_block(buffer, data->str().c_str(), (unsigned int)data->str().size());
		}
		break;
		default:
			m->print(msg::MERROR, "main.cpp", "send_packet(): unknown packet type (%u)!", type);
			return;
			break;
	}

	// send packet ...
	n->send_packet(&n->get_clients()->at(cnum).sock, buffer->str().c_str(), (unsigned int)buffer->str().size(), cnum);
}

void snet::handle_client(unsigned int cnum) {
	int pos, len, used;

	// checks if client is still connected to the server and receive packet
	// TODO: if packet len = max_packet_len, don't process the last packet, instead put the last packet
	//       to the beginning of the next packet (b/c the last packet could only be "half available"
	len = n->recv_packet(&n->get_clients()->at(cnum).sock, packet_data, max_packet_size, cnum);
	if(len < 0) {
		n->close_socket(n->get_clients()->at(cnum).sock);
	}
	else if(len != 0) {
		// fill packet_data with 0's (if we later fill a stringstream with
		//  that data, it will act strange w/o doing this)
		for(int i = len; i < (int)max_packet_size; i++) {
			packet_data[i] = 0;
		}

		pos = 0;
		while(len > 0) {
			used = process_packet(cnum, &packet_data[pos], (unsigned int)len);
			if(used == 1 && n->check_header(&packet_data[pos], net::HT_HTTP)) {
				used = process_http_packet(cnum, &packet_data[pos], (unsigned int)len);
			}

			pos += used;
			len -= used;
			if(used == 0) {
				// lost data ...
				m->print(msg::MDEBUG, "main.cpp", "lost data - server down or disconnect? (handle server routine)");
				len = 0;
			}
		}
	}
}

void snet::manage_clients() {
	vector<net::client> clients = *(n->get_clients());
	for(unsigned int i = 0; i < (unsigned int)clients.size(); i++) {
		if(SDLNet_SocketReady(clients[i].sock)) {
			handle_client(i);
		}
	}
}

int snet::process_http_packet(unsigned int cnum, char* pdata, unsigned int max_len) {
	c->reset(buffer);
	c->reset(data);
	*buffer << pdata;
	bool post_data = false;
	while(!buffer->eof()) {
		buffer->getline(line, 65536);
		// if post_data flag is set (-> next line contains the post data)
		if(post_data) {
			post_data = false;
			// put line into our data buffer so we can use it later in "/register_post"
			for(unsigned int i = 0; i < strlen(line); i++) {
				// replace &'s and ='s with a space, so we can easily get each chunk via '<<'
				if(line[i] == '&' || line[i] == '=') line[i] = ' ';
			}
			*data << line;
		}
		// if line is an empty line (first char is 0x0D), set post_data flag
		if(line[0] == 0x0D) post_data = true;
	}

	// get request url
	c->reset(buffer);
	*buffer << pdata;
	*buffer >> tmp >> tmp;

	c->reset(buffer);
	if(tmp == "/" || tmp == "/status") {
		write_http_header(buffer, "200 OK");
		w->load_file("body.html");

		c->reset(data);
		*data << "<b>online clients:</b><br />" << endl;
		unsigned int i = 0;
		for(vector<net::client>::iterator cl_iter = n->get_clients()->begin(); cl_iter != n->get_clients()->end(); cl_iter++) {
			if(cl_iter->status == net::CS_ONLINE) {
				userman::user_data* ud = um->get_user(cl_iter->name.c_str());
				*data << "#" << i << ": " << cl_iter->name << " (ID: " << ud->id << ", IP: " <<
					((cl_iter->ip.host & 0xFF) & 0xFF) << "." << ((cl_iter->ip.host >> 8) & 0xFF) << "." <<
					((cl_iter->ip.host >> 16) & 0xFF) << "." << ((cl_iter->ip.host >> 24) & 0xFF) << ")<br />" << endl;
			}
			i++;
		}

		w->replace("$body", data->str().c_str());
		*buffer << *(w->get_data());
	}
	else if(tmp == "/register") {
		write_http_header(buffer, "200 OK");
		w->load_file("register.html");
		*buffer << *(w->get_data());
	}
	else if(tmp == "/register_post") {
		// arg string is sth. like that: "name User pw 123 Submit register"
		map<string, string> args;
		string n, v;
		while(!data->eof()) {
			*data >> n;
			*data >> v;
			args[n] = v;
		}

		// check if our arg string contains name and pw and if so,
		// check if those have a permitted value (no name=pw or pw=Submit)
		if(args.count("name") && args.count("pw") && args["name"] != "pw" && args["pw"] != "Submit") {
			bool user_exist = um->check_user(args["name"].c_str());

			if(user_exist) {
				m->print(msg::MERROR, "snet.cpp", "process_http_packet(): register_post: a user with such a name (\"%s\") already exists!", args["name"].c_str());

				write_http_header(buffer, "200 OK");
				w->load_file("body.html");
				w->replace("$body", "<b>a user with such a name already exists!</b>");
				*buffer << *(w->get_data());
			}
			else {
				um->add_user(args["name"].c_str(), args["pw"].c_str());

				write_http_header(buffer, "200 OK");
				w->load_file("body.html");
				w->replace("$body", "<b>you were successfully registered!</b>");
				*buffer << *(w->get_data());
			}
		}
		else {
			// no name or pw specified
			m->print(msg::MERROR, "snet.cpp", "process_http_packet(): register_post: no name or pw specified");

			write_http_header(buffer, "200 OK");
			w->load_file("body.html");
			w->replace("$body", "<b>no name or password specified!</b>");
			*buffer << *(w->get_data());
		}
	}
	else {
		write_http_header(buffer, "404 Not Found");
		w->load_file("body.html");
		w->replace("$body", "<b><font size=\"4\">ERROR #404 - Page not found</font></b>");
		*buffer << *(w->get_data());
	}

	n->send_packet(&n->get_clients()->at(cnum).sock, buffer->str().c_str(), (unsigned int)buffer->str().size(), cnum, false);

	n->get_clients()->at(cnum).quit = true;
	return max_len-1;
}

void snet::write_http_header(stringstream* stream, const char* status) {
	*buffer << "HTTP/1.1 " << status << endl;
	*buffer << "Server: A2 Server" << endl;
	*buffer << "Accept-Ranges: bytes" << endl;
	*buffer << "Connection: close" << endl;
	*buffer << "Content-Type: text/html; charset=UTF-8" << endl;
	*buffer << "" << endl;
}

stringstream* snet::get_data() {
	return data;
}

void snet::run() {
	map<string, userman::user_data>* ul = um->get_user_list();
	vector<net::client> clients = *(n->get_clients());

	// TODO: make this a really vis_user list ...
	// put user id of every visible player into the vis_user list of each user
	for(map<string, userman::user_data>::iterator iter = ul->begin(); iter != ul->end(); iter++) {
		// update list every 4 seconds ...
		if(iter->second.nid != -1 && SDL_GetTicks() - iter->second.vis_user_timer > 4000) {
			iter->second.vis_user.clear();
			for(map<string, userman::user_data>::iterator uiter = ul->begin(); uiter != ul->end(); uiter++) {
				iter->second.vis_user.push_back(uiter->second.id);
			}
			iter->second.vis_user_timer = SDL_GetTicks();

			// send new vis_user list to client
			c->reset(data);
			c->put_uint(data, (unsigned int)iter->second.vis_user.size());
			for(vector<unsigned int>::iterator id_iter = iter->second.vis_user.begin(); id_iter != iter->second.vis_user.end(); id_iter++) {
				c->put_uint(data, *id_iter);
			}
			send_packet(iter->second.nid, 0, snet::PT_VU_LIST);
		}
	}
}