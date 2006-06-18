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

#include "userman.h"

userman::userman(engine* e) {
	userman::e = e;
	userman::c = e->get_core();
	userman::m = e->get_msg();
	userman::x = e->get_xml();

	buffer = new stringstream(stringstream::in | stringstream::out | stringstream::binary);

	max_id = 0;
}

userman::~userman() {
	delete buffer;
}

void userman::load_list(const char* file) {
	if(!x->open((char*)file)) {
		m->print(msg::MERROR, "userman.cpp", "load_list(): couldn't open user list file!");
		return;
	}

	user_list.clear();
	string name;
	while(x->process()) {
		if(strcmp(x->get_node_name(), "user") == 0) {
			// check if all attributes are available
			if(x->get_attribute("name") == NULL) {
				m->print(msg::MERROR, "userman.cpp", "load_list(): no user name specified!");
				break;
			}
			if(x->get_attribute("password") == NULL) {
				m->print(msg::MERROR, "userman.cpp", "load_list(): no user password specified!");
				break;
			}
			if(x->get_attribute("id") == NULL) {
				m->print(msg::MERROR, "userman.cpp", "load_list(): no user id specified!");
				break;
			}

			// get attributes
			name = x->get_attribute("name");
			user_list[name].password = x->get_attribute("password");
			user_list[name].position.set(0.0f, 0.0f, 0.0f);
			user_list[name].id = (unsigned int)atoi(x->get_attribute("id"));
			if(user_list[name].id > max_id) max_id = user_list[name].id;
		}
	}
	x->close();

	m->print(msg::MDEBUG, "userman.cpp", "load_list(): successfully loaded user list (%u users)!", user_list.size());
}

bool userman::check_user(const char* name) {
	return user_list.count(name) ? true : false;
}

bool userman::check_pw(const char* name, const char* pw) {
	if(!user_list.count(name)) {
		m->print(msg::MERROR, "userman.cpp", "check_pw(): no user with the name \"%s\" does exist!", name);
		return false;
	}
	if(user_list[name].password != pw) {
		m->print(msg::MERROR, "userman.cpp", "check_pw(): user \"%s\" entered a wrong password!", name);
		return false;
	}
	return true;
}

userman::user_data* userman::get_user(const char* name) {
	return &user_list[name];
}

void userman::add_user(const char* name, const char* pw) {
	if(user_list.count(name)) {
		m->print(msg::MERROR, "userman.cpp", "add_user(): a user with such a name (\"%s\") already exists!", name);
		return;
	}

	user_list[name].password = pw;
	user_list[name].position.set(0.0f, 0.0f, 0.0f);
	user_list[name].id = max_id+1;
	max_id++;

	m->print(msg::MDEBUG, "userman.cpp", "add_user(): successfully added user \"%s\" (id: %u)!", name, user_list[name].id);
}

void userman::save_user_list() {
	x->open_write(e->data_path("users.xml"));
	x->start_element("users");

	for(map<string, user_data>::iterator iter = user_list.begin(); iter != user_list.end(); iter++) {
		c->reset(buffer);
		x->start_element("user");
		x->write_attribute("name", iter->first.c_str());
		x->write_attribute("password", iter->second.password.c_str());
		*buffer << iter->second.id;
		x->write_attribute("id", buffer->str().c_str());
		x->end_element();
	}

	x->end_element();
	x->close_write();

	m->print(msg::MDEBUG, "userman.cpp", "save_user_list(): successfully saved user list!");
}
