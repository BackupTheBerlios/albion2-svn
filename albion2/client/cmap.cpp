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

#include "cmap.h"

cmap::cmap(engine* e, scene* sce) {
	cmap::e = e;
	cmap::sce = sce;
	cmap::c = e->get_core();
	cmap::m = e->get_msg();

	map_open = false;
	amap = new a2emap(e, sce, NULL);

	map_names[0] = "klouta.a2map";
}

cmap::~cmap() {
	delete amap;
}

void cmap::load_map(unsigned int id) {
	if(map_open) {
		m->print(msg::MERROR, "cmap.cpp", "load_map(): a map is already opened!");
		return;
	}

	amap->load_map(e->data_path(map_names[id].c_str()), true);
	map_open = true;

	m->print(msg::MDEBUG, "cmap.cpp", "load_map(): successfully load map \"%s\"!", map_names[id].c_str());
}

void cmap::close_map() {
	if(!map_open) return;

	amap->close_map();
	map_open = false;
}
