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

snet::snet(engine* e, net* n, userman* um, web* w) {
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

	buffer->str("");
	buffer->clear();
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
			tmp_str = "";
			// check if user exists
			if(!um->check_user(n->get_clients()->at(cnum).name.c_str())) {
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
			if(!um->check_pw(n->get_clients()->at(cnum).name.c_str(), tmp_str.c_str())) {
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
				if(i != cnum) {
					send_packet(i, cnum, snet::PT_NEW_CLIENT);
				}
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
		}
		break;
		case snet::PT_QUIT_CLIENT: {
			n->get_clients()->at(cnum).quit = true;

			m->print(msg::MDEBUG, "main.cpp", "process_packet(): quitting client (#%u, %s)!",
				n->get_clients()->at(cnum).id, n->get_clients()->at(cnum).name.c_str());

			unsigned int i = 0;
			for(vector<net::client>::iterator cl_iter = n->get_clients()->begin(); cl_iter != n->get_clients()->end(); cl_iter++) {
				if(i != cnum) {
					send_packet(i, cnum, snet::PT_QUIT_CLIENT);
				}
				i++;
			}
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
		default: {
			m->print(msg::MERROR, "main.cpp", "process_packet(): unknown packet type (%u)!", packet_type);
		}
		break;
	}

	used = 4 + 4 + packet_len;
	return used;
}

void snet::send_packet(unsigned int cnum, unsigned int inum, snet::PACKET_TYPE type) {
	buffer->str("");
	buffer->clear();
	switch(type) {
		// new client connection, send information to other clients
		case snet::PT_NEW_CLIENT: {
			if(n->get_clients()->at(cnum).sock == NULL && n->get_clients()->at(cnum).status == net::CS_ONLINE) break;

			c->put_uint(buffer, snet::PT_NEW_CLIENT);
			c->put_uint(buffer, n->get_clients()->at(inum).id);
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
