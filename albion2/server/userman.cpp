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

void userman::run() {
	for(map<string, user_data>::iterator iter = user_list.begin(); iter != user_list.end(); iter++) {
		if(iter->second.active) {
			memcpy(&(iter->second.position), iter->second.obj->get_position(), sizeof(vertex3));
			//memcpy(&(iter->second.rotation), iter->second.obj->get_rotation(), sizeof(vertex3)); // not needed
		}
	}
}

void userman::load_user_list(const char* file) {
	if(!x->open((char*)file)) {
		m->print(msg::MERROR, "userman.cpp", "load_user_list(): couldn't open user list file!");
		return;
	}

	user_list.clear();
	string name;
	while(x->process()) {
		if(strcmp(x->get_node_name(), "user") == 0) {
			// check if all attributes are available
			if(x->get_attribute("name") == NULL) {
				m->print(msg::MERROR, "userman.cpp", "load_user_list(): no user name specified!");
				break;
			}
			if(x->get_attribute("password") == NULL) {
				m->print(msg::MERROR, "userman.cpp", "load_user_list(): no user password specified!");
				break;
			}
			if(x->get_attribute("id") == NULL) {
				m->print(msg::MERROR, "userman.cpp", "load_user_list(): no user id specified!");
				break;
			}

			// get attributes
			name = x->get_attribute("name");
			user_list[name].name = name;
			user_list[name].password = x->get_attribute("password");
			user_list[name].id = (unsigned int)atoi(x->get_attribute("id"));
			user_list[name].vis_user_timer = SDL_GetTicks();
			user_list[name].nid = -1;
			user_list[name].active = false;
			if(user_list[name].id > max_id) max_id = user_list[name].id;
		}
	}
	x->close();

	m->print(msg::MDEBUG, "userman.cpp", "load_user_list(): successfully loaded user list (%u users)!", user_list.size());
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

userman::user_data* userman::get_user(unsigned int id) {
	for(map<string, user_data>::iterator iter = user_list.begin(); iter != user_list.end(); iter++) {
		if(iter->second.id == id) {
			return get_user(iter->second.name.c_str());
		}
	}
	m->print(msg::MERROR, "userman.cpp", "get_user(): no user with such an id (\"%u\") exists!", id);
	return NULL;
}

void userman::add_user(const char* name, const char* pw) {
	if(user_list.count(name)) {
		m->print(msg::MERROR, "userman.cpp", "add_user(): a user with such a name (\"%s\") already exists!", name);
		return;
	}

	user_list[name].name = name;
	user_list[name].password = pw;
	user_list[name].position.set(0.0f, 0.0f, 0.0f);
	user_list[name].id = max_id+1;
	user_list[name].vis_user_timer = SDL_GetTicks();
	user_list[name].nid = -1;
	user_list[name].active = false;
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

void userman::load_user_db(const char* file) {
	if(!x->open((char*)file)) {
		m->print(msg::MERROR, "userman.cpp", "load_user_db(): couldn't open user db file!");
		return;
	}

	string name;
	float f;
	while(x->process()) {
		if(strcmp(x->get_node_name(), "user") == 0) {
			// check if all attributes are available
			if(x->get_attribute("id") == NULL) {
				m->print(msg::MERROR, "userman.cpp", "load_user_db(): no user id specified!");
				break;
			}
			if(x->get_attribute("map") == NULL) {
				m->print(msg::MERROR, "userman.cpp", "load_user_db(): no user map specified!");
				break;
			}
			if(x->get_attribute("position") == NULL) {
				m->print(msg::MERROR, "userman.cpp", "load_user_db(): no user position specified!");
				break;
			}
			if(x->get_attribute("rotation") == NULL) {
				m->print(msg::MERROR, "userman.cpp", "load_user_db(): no user rotation specified!");
				break;
			}

			// get attributes
			user_data* user = get_user((unsigned int)atoi(x->get_attribute("id")));
			user->map = (unsigned int)atoi(x->get_attribute("map"));

			c->reset(buffer);
			*buffer << x->get_attribute("position") << endl;
			*buffer >> f;
			user->position.x = f;
			*buffer >> f;
			user->position.y = f;
			*buffer >> f;
			user->position.z = f;

			*buffer << x->get_attribute("rotation") << endl;
			*buffer >> f;
			user->rotation.x = f;
			*buffer >> f;
			user->rotation.y = f;
			*buffer >> f;
			user->rotation.z = f;
		}
	}
	x->close();

	m->print(msg::MDEBUG, "userman.cpp", "load_user_db(): successfully loaded user db!");
}

void userman::save_user_db() {
	x->open_write(e->data_path("user_db.xml"));
	x->start_element("users");

	for(map<string, user_data>::iterator iter = user_list.begin(); iter != user_list.end(); iter++) {
		x->start_element("user");

		c->reset(buffer);
		*buffer << iter->second.id;
		x->write_attribute("id", buffer->str().c_str());

		c->reset(buffer);
		*buffer << iter->second.map;
		x->write_attribute("map", buffer->str().c_str());

		c->reset(buffer);
		*buffer << iter->second.position.x << " " << iter->second.position.y << " " << iter->second.position.z;
		x->write_attribute("position", buffer->str().c_str());

		c->reset(buffer);
		*buffer << iter->second.rotation.x << " " << iter->second.rotation.y << " " << iter->second.rotation.z;
		x->write_attribute("rotation", buffer->str().c_str());

		x->end_element();
	}

	x->end_element();
	x->close_write();

	m->print(msg::MDEBUG, "userman.cpp", "save_user_db(): successfully saved user db!");
}

map<string, userman::user_data>* userman::get_user_list() {
	return &user_list;
}
