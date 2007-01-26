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

/*! a2emap constructor
 *  @param e a pointer to the engine class
 *  @param sce a pointer to the scene class
 *  @param o a pointer to the ode class (not necessarily needed)
 */
a2emap::a2emap(engine* e, scene* sce, ode* o) {
	// get classes
	a2emap::e = e;
	a2emap::sce = sce;
	a2emap::o = o;
	a2emap::c = e->get_core();
	a2emap::m = e->get_msg();
	a2emap::f = e->get_file_io();

	map_opened = false;
}

/*! a2emap destructor
 */
a2emap::~a2emap() {
	m->print(msg::MDEBUG, "a2emap.cpp", "freeing a2emap stuff");

	// close map if map is still loaded ...
	if(map_opened) {
		close_map();
	}

	m->print(msg::MDEBUG, "a2emap.cpp", "a2emap stuff freed");
}

/*! loads/opens an a2e map specified by filename and adds everything to the scene
 *  @param filename the maps filename
 *  @param vbo flag that specifies if vertex buffer objects should be used
 */
bool a2emap::load_map(const char* filename, bool vbo) {
	if(map_opened) {
		m->print(msg::MERROR, "a2emap.cpp", "load_map(): a map is already loaded!");
		return false;
	}

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
	float x, y, z;
	for(unsigned int i = 0; i < object_count; i++) {
		objects.push_back(*new a2emap::map_object());

		// model name
		f->get_terminated_block(&objects[i].name, (char)0xFF);

		// model filename
		f->get_terminated_block(&objects[i].model_filename, (char)0xFF);

		// model animation filename
		f->get_terminated_block(&objects[i].ani_filename, (char)0xFF);

		f->get_char(); // model type, not needed any more
		// model material filename
		f->get_terminated_block(&objects[i].mat_filename, (char)0xFF);

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

		// gravity flag
		objects[i].gravity = f->get_char() == 0x00 ? false : true;

		// collision model flag
		objects[i].collision_model = f->get_char() == 0x00 ? false : true;

		// auto mass flag
		objects[i].auto_mass = f->get_char() == 0x00 ? false : true;

		// mass
		objects[i].mass = f->get_float();

		// physical object scale
		objects[i].phys_scale.x = f->get_float();
		objects[i].phys_scale.y = f->get_float();
		objects[i].phys_scale.z = f->get_float();
	}

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

		// TODO: make a hard_scale at the amodel too ...
		/*objects[i].model_type ?
			objects[i].amodel->set_scale(objects[i].scale.x, objects[i].scale.y, objects[i].scale.z) :
			(objects[i].model->set_hard_scale(objects[i].scale.x, objects[i].scale.y, objects[i].scale.z),
			objects[i].model->set_scale(objects[i].scale.x, objects[i].scale.y, objects[i].scale.z));*/
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

			objects[i].model->set_phys_scale(objects[i].phys_scale.x, objects[i].phys_scale.y, objects[i].phys_scale.z);

			objects[i].ode_obj = o->add_object(objects[i].model, !objects[i].gravity, (ode_object::OTYPE)objects[i].phys_type, objects[i].collision_model);
			if(!objects[i].auto_mass) objects[i].ode_obj->set_mass(objects[i].mass);
		}
	}

	map_opened = true;

	return true;
}

/*! closes the map
 */
void a2emap::close_map() {
	if(!map_opened) {
		m->print(msg::MERROR, "a2emap.cpp", "close_map(): no map is currently loaded!");
		return;
	}

	for(unsigned int i = 0; i < object_count; i++) {
		objects[i].model_type ? (sce->delete_model(objects[i].amodel), delete objects[i].amodel) :
								(sce->delete_model(objects[i].model), delete objects[i].model);

		delete objects[i].mat;
	}
	objects.clear();
	object_count = 0;
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
