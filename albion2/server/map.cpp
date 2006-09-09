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

#include "map.h"

smap::smap(engine* e, scene* sce, userman* um) {
	smap::e = e;
	smap::sce = sce;
	smap::c = e->get_core();
	smap::m = e->get_msg();
	smap::um = um;

	map_names[0] = "klouta.a2map";
}

smap::~smap() {
	maps.clear();
}

void smap::load() {
	for(map<unsigned int, string>::iterator iter = map_names.begin(); iter != map_names.end(); iter++) {
		const char* map_name = iter->second.c_str();
		maps[map_name].o = new ode(e);
		maps[map_name].o->init();

		maps[map_name].map = new a2emap(e, sce, maps[map_name].o);
		maps[map_name].map->load_map(e->data_path(map_name), true);

		m->print(msg::MDEBUG, "map.cpp", "load(): successfully load map \"%s\"!", map_name);
	}
}

void smap::run(unsigned int ode_time) {
	for(map<string, map_object>::iterator iter = maps.begin(); iter != maps.end(); iter++) {
		iter->second.o->run(ode_time);
	}
}

void smap::add_user_phys_obj(const char* name) {
	um->get_user(name)->phys_obj = maps[map_names[um->get_user(name)->map]].o->add_object(um->get_user(name)->obj, false, ode_object::SPHERE);
}

void smap::del_user_phys_obj(const char* name) {
	maps[map_names[um->get_user(name)->map]].o->delete_object(um->get_user(name)->phys_obj);
}
