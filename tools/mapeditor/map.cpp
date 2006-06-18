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

cmap::cmap(engine* e) {
	cmap::e = e;
	cmap::m = e->get_msg();
	cmap::f = e->get_file_io();

	map_opened = false;
}

cmap::~cmap() {
}

bool cmap::open_map(char* filename) {
	if(map_opened) {
		// TODO: close and save old map
	}
	map_opened = true;

	// open map
	f->open_file(filename, file_io::OT_READ_BINARY);
	f->get_block(map_type, 6);
	map_type[6] = 0;
	if(strcmp(map_type, "A2EMAP") != 0) {
		m->print(msg::MERROR, "map.cpp", "open_map(): wrong map type (%s)!", map_type);
		return false;
	}

	// get the amount of map objects
	object_count = f->get_uint();

	// reserve memory for the objects
	map_objects = new map_object[object_count];

	// load objects
	for(unsigned int i = 0; i < object_count; i++) {
		// model name
		f->get_block(map_objects[i].model_name, 32);
		// model filename
		f->get_block(map_objects[i].model_filename, 32);
		// animation filename
		f->get_block(map_objects[i].ani_filename, 32);
		// model type
		if(f->get_char() == 0x00) map_objects[i].model_type = false;
		else map_objects[i].model_type = true;
		// mat filename
		f->get_block(map_objects[i].mat_filename, 32);

		// position
		map_objects[i].position.x = f->get_float();
		map_objects[i].position.y = f->get_float();
		map_objects[i].position.z = f->get_float();

		// orientation
		map_objects[i].orientation.x = f->get_float();
		map_objects[i].orientation.y = f->get_float();
		map_objects[i].orientation.z = f->get_float();

		// scale
		map_objects[i].scale.x = f->get_float();
		map_objects[i].scale.y = f->get_float();
		map_objects[i].scale.z = f->get_float();

		// physical properties
		map_objects[i].phys_type = f->get_uint();
	}

	f->close_file();

	return true;
}

bool cmap::save_map(char* filename) {
	f->open_file(filename, file_io::OT_WRITE_BINARY);

	f->write_block("A2EMAP", 6);
	f->put_uint(object_count);

	for(unsigned int i = 0; i < object_count; i++) {
		f->write_block(map_objects[i].model_name, 32);
		f->write_block(map_objects[i].model_filename, 32);
		f->write_block(map_objects[i].ani_filename, 32);
		f->put_bool(map_objects[i].model_type);
		f->write_block(map_objects[i].mat_filename, 32);

		f->put_float(map_objects[i].position.x);
		f->put_float(map_objects[i].position.y);
		f->put_float(map_objects[i].position.z);
		
		f->put_float(map_objects[i].orientation.x);
		f->put_float(map_objects[i].orientation.y);
		f->put_float(map_objects[i].orientation.z);
		
		f->put_float(map_objects[i].scale.x);
		f->put_float(map_objects[i].scale.y);
		f->put_float(map_objects[i].scale.z);

		f->put_uint(map_objects[i].phys_type);
	}

	f->close_file();

	return true;
}

bool cmap::close_map() {
	map_opened = false;

	delete [] map_objects;
	object_count = 0;

	return true;
}

void cmap::new_map(char* filename) {
	f->open_file(filename, file_io::OT_WRITE_BINARY);

	f->write_block("A2EMAP", 6);
	f->put_uint(0);

	f->close_file();
}
