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

#include "a2emap.h"

/*! there is no function currently
 */
a2emap::a2emap(engine* e, scene* sce, ode* o) {
	// get classes
	a2emap::e = e;
	a2emap::sce = sce;
	a2emap::o = o;
	a2emap::c = e->get_core();
	a2emap::m = e->get_msg();
	a2emap::f = e->get_file_io();
}

/*! there is no function currently
 */
a2emap::~a2emap() {
	m->print(msg::MDEBUG, "a2emap.cpp", "freeing a2emap stuff");

	m->print(msg::MDEBUG, "a2emap.cpp", "a2emap stuff freed");
}

/*! loads/opens an a2e map specified by filename and adds everything to the scene
 *  @param filename the maps filename
 */
bool a2emap::load_map(const char* filename, bool vbo) {
	f->open_file(filename, file_io::OT_READ_BINARY);

	char* map_type = new char[7];
	f->get_block(map_type, 6);
	map_type[6] = 0;
	if(strcmp(map_type, "A2EMAP") != 0) {
		delete [] map_type;
		m->print(msg::MERROR, "a2emap.cpp", "load_map(): wrong map type (%s)!", map_type);
		return false;
	}
	delete [] map_type;

	// get the amount of map objects
	object_count = f->get_uint();

	objects.reserve(object_count);
	char* tmp = new char[33];
	tmp[32] = 0;
	float x, y, z;
	for(unsigned int i = 0; i < object_count; i++) {
		objects.push_back(*new a2emap::map_object());

		f->get_block(tmp, 32); // model name
		objects[i].name = tmp;

		f->get_block(tmp, 32); // model filename
		objects[i].model_filename = tmp;

		f->get_block(tmp, 32); // model animation filename
		objects[i].ani_filename = tmp;

		f->get_char(); // model type, not needed any more
		f->get_block(tmp, 32); // model material filename
		objects[i].mat_filename = tmp;

		x = f->get_float(); // position
		y = f->get_float();
		z = f->get_float();
		objects[i].position.set(x, y, z);

		x = f->get_float(); // orientation/rotation
		y = f->get_float();
		z = f->get_float();
		objects[i].orientation.set(x, y, z);

		x = f->get_float(); // scale
		y = f->get_float();
		z = f->get_float();
		objects[i].scale.set(x, y, z);

		objects[i].phys_type = f->get_uint(); // physical property
	}
	delete [] tmp;

	f->close_file();

	// load models, animations and materials
	for(unsigned int i = 0; i < object_count; i++) {
		objects[i].model_type = e->get_core()->is_a2eanim(e->data_path((char*)objects[i].model_filename.c_str()));
		if(!objects[i].model_type) {
			objects[i].model = sce->create_a2emodel();
			objects[i].model->load_model(e->data_path(objects[i].model_filename.c_str()), vbo);
		}
		else {
			objects[i].amodel = sce->create_a2eanim();
			objects[i].amodel->load_model(e->data_path(objects[i].model_filename.c_str()), vbo);
		}

		if(objects[i].model_type) objects[i].amodel->add_animation(e->data_path(objects[i].ani_filename.c_str()));

		objects[i].mat = new a2ematerial(e);
		objects[i].mat->load_material(e->data_path(objects[i].mat_filename.c_str()));
		objects[i].model_type ? objects[i].amodel->set_material(objects[i].mat) : objects[i].model->set_material(objects[i].mat);

		objects[i].model_type ?
			objects[i].amodel->set_position(objects[i].position.x, objects[i].position.y, objects[i].position.z) :
			objects[i].model->set_position(objects[i].position.x, objects[i].position.y, objects[i].position.z);

		objects[i].model_type ?
			objects[i].amodel->set_rotation(objects[i].orientation.x, objects[i].orientation.y, objects[i].orientation.z) :
			objects[i].model->set_rotation(objects[i].orientation.x, objects[i].orientation.y, objects[i].orientation.z);

		objects[i].model_type ?
			objects[i].amodel->set_scale(objects[i].scale.x, objects[i].scale.y, objects[i].scale.z) :
			objects[i].model->set_scale(objects[i].scale.x, objects[i].scale.y, objects[i].scale.z);

		objects[i].model_type ? sce->add_model(objects[i].amodel) : sce->add_model(objects[i].model);

		if(!objects[i].model_type && o != NULL) {
			if(objects[i].phys_type == ode_object::SPHERE || objects[i].phys_type == ode_object::CYLINDER) {
				float r = 0.0f;
				float lx = abs(objects[i].model->get_bounding_box()->vmin.x - objects[i].model->get_bounding_box()->vmax.x);
				float lz = abs(objects[i].model->get_bounding_box()->vmin.z - objects[i].model->get_bounding_box()->vmax.z);
				lx > lz ? r = lx / 2.0f : r = lz / 2.0f;
				objects[i].model->set_radius(r);

				if(objects[i].phys_type == ode_object::CYLINDER) {
					float ly = abs(objects[i].model->get_bounding_box()->vmin.y - objects[i].model->get_bounding_box()->vmax.y);
					objects[i].model->set_length(ly);
				}
			}

			objects[i].ode_obj = o->add_object(objects[i].model, false, (ode_object::OTYPE)objects[i].phys_type);
		}
	}

	return true;
}

/*! closes the map
 */
void a2emap::close_map() {
	for(unsigned int i = 0; i < object_count; i++) {
		objects[i].model_type ? sce->delete_model(objects[i].amodel), delete objects[i].amodel :
								sce->delete_model(objects[i].model), delete objects[i].model;

		delete objects[i].mat;
	}
	objects.clear();
}

/*! returns a map object specified by the objects (model) name
 *  @param name the (model) name of the object
 */
a2emap::map_object* a2emap::get_object(const char* name) {
	for(unsigned int i = 0; i < object_count; i++) {
		if(strcmp(objects[i].name.c_str(), name) == 0) {
			return &objects[i];
		}
	}
	m->print(msg::MERROR, "a2emap.cpp", "get_object(): no object with the name \"%s\" could be found!", name);
	return NULL;
}
