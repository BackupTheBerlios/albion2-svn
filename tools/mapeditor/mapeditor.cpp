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

#include "mapeditor.h"

mapeditor::mapeditor(engine* e, scene* sce) {
	mapeditor::e = e;
	mapeditor::sce = sce;
	mapeditor::m = e->get_msg();

	memap = new map(e);

	cur_bbox = new core::aabbox();
	cur_bbox->vmin.x = 0.0f;
	cur_bbox->vmin.y = 0.0f;
	cur_bbox->vmin.z = 0.0f;
	cur_bbox->vmax.x = 0.0f;
	cur_bbox->vmax.y = 0.0f;
	cur_bbox->vmax.z = 0.0f;

	new_sel = false;
	sel = false;

	map_opened = false;
}

mapeditor::~mapeditor() {
	delete cur_bbox;
	delete memap;

	for(list<a2emodel>::iterator mdl_iter = models.begin(); mdl_iter != models.end(); mdl_iter++) {
		sce->delete_model(&*mdl_iter);
	}
	for(list<a2eanim>::iterator amdl_iter = amodels.begin(); amdl_iter != amodels.end(); amdl_iter++) {
		sce->delete_model(&*amdl_iter);
	}
	models.clear();
	amodels.clear();
	materials.clear();
	objects.clear();
}

void mapeditor::run() {
	// draw bounding box of selected model
	vertex3* tmpv = new vertex3(cur_bbox->vmax.x, cur_bbox->vmin.y, cur_bbox->vmin.z);
	vertex3* tmpv2 = new vertex3();
	e->get_gfx()->draw_3d_line(&cur_bbox->vmin, tmpv, 0xFFFFFF);
	tmpv->set(cur_bbox->vmin.x, cur_bbox->vmax.y, cur_bbox->vmin.z);
	e->get_gfx()->draw_3d_line(&cur_bbox->vmin, tmpv, 0xFFFFFF);
    tmpv->set(cur_bbox->vmin.x, cur_bbox->vmin.y, cur_bbox->vmax.z);
	e->get_gfx()->draw_3d_line(&cur_bbox->vmin, tmpv, 0xFFFFFF);

    tmpv->set(cur_bbox->vmax.x, cur_bbox->vmax.y, cur_bbox->vmin.z);
    tmpv2->set(cur_bbox->vmin.x, cur_bbox->vmax.y, cur_bbox->vmin.z);
	e->get_gfx()->draw_3d_line(tmpv2, tmpv, 0xFFFFFF);
    tmpv->set(cur_bbox->vmin.x, cur_bbox->vmax.y, cur_bbox->vmax.z);
    tmpv2->set(cur_bbox->vmin.x, cur_bbox->vmax.y, cur_bbox->vmin.z);
	e->get_gfx()->draw_3d_line(tmpv2, tmpv, 0xFFFFFF);

    tmpv->set(cur_bbox->vmax.x, cur_bbox->vmax.y, cur_bbox->vmin.z);
    tmpv2->set(cur_bbox->vmax.x, cur_bbox->vmin.y, cur_bbox->vmin.z);
	e->get_gfx()->draw_3d_line(tmpv2, tmpv, 0xFFFFFF);
    tmpv->set(cur_bbox->vmin.x, cur_bbox->vmax.y, cur_bbox->vmax.z);
    tmpv2->set(cur_bbox->vmin.x, cur_bbox->vmin.y, cur_bbox->vmax.z);
	e->get_gfx()->draw_3d_line(tmpv2, tmpv, 0xFFFFFF);

    tmpv->set(cur_bbox->vmin.x, cur_bbox->vmax.y, cur_bbox->vmax.z);
	e->get_gfx()->draw_3d_line(&cur_bbox->vmax, tmpv, 0xFFFFFF);
    tmpv->set(cur_bbox->vmax.x, cur_bbox->vmin.y, cur_bbox->vmax.z);
	e->get_gfx()->draw_3d_line(&cur_bbox->vmax, tmpv, 0xFFFFFF);
    tmpv->set(cur_bbox->vmax.x, cur_bbox->vmax.y, cur_bbox->vmin.z);
	e->get_gfx()->draw_3d_line(&cur_bbox->vmax, tmpv, 0xFFFFFF);

    tmpv->set(cur_bbox->vmin.x, cur_bbox->vmin.y, cur_bbox->vmax.z);
    tmpv2->set(cur_bbox->vmax.x, cur_bbox->vmin.y, cur_bbox->vmax.z);
	e->get_gfx()->draw_3d_line(tmpv2, tmpv, 0xFFFFFF);
    tmpv->set(cur_bbox->vmax.x, cur_bbox->vmin.y, cur_bbox->vmin.z);
    tmpv2->set(cur_bbox->vmax.x, cur_bbox->vmin.y, cur_bbox->vmax.z);
	e->get_gfx()->draw_3d_line(tmpv2, tmpv, 0xFFFFFF);

    delete tmpv;
    delete tmpv2;
}

void mapeditor::save_map(char* filename) {
	// if filename == NULL, we will just overwrite the current map file (save ...)
	// if not, we will save it as a new map called filename (save as ...)
	if(filename == NULL) {
		filename = (char*)cur_map_name.c_str();
	}

	// feed the map class with the new map data and delete old data
	delete [] memap->map_objects;
	memap->object_count = 0;
	memap->map_objects = new map::map_object[objects.size()];
	memap->object_count = (unsigned int)objects.size();
	unsigned int i = 0;
	for(list<map_object>::iterator obj_iter = objects.begin(); obj_iter != objects.end(); obj_iter++) {
		memcpy(memap->map_objects[i].ani_filename, obj_iter->ani_filename, 32);
		memcpy(memap->map_objects[i].mat_filename, obj_iter->mat_filename, 32);
		memcpy(memap->map_objects[i].model_filename, obj_iter->model_filename, 32);
		memcpy(memap->map_objects[i].model_name, obj_iter->model_name, 32);
		memap->map_objects[i].model_type = obj_iter->type;
		memap->map_objects[i].phys_type = obj_iter->phys_type;
		if(!obj_iter->type) {
			memap->map_objects[i].orientation = obj_iter->model->get_rotation();
			memap->map_objects[i].position = obj_iter->model->get_position();
			memap->map_objects[i].scale = obj_iter->model->get_scale();
		}
		else {
			memap->map_objects[i].orientation = obj_iter->amodel->get_rotation();
			memap->map_objects[i].position = obj_iter->amodel->get_position();
			memap->map_objects[i].scale = obj_iter->amodel->get_scale();
		}
		i++;
	}

	// ... and save it finally
	memap->save_map(filename);
}

void mapeditor::open_map(char* filename) {
	cur_map_name = filename;
	if(!memap->open_map(filename)) {
		m->print(msg::MERROR, "mapeditor.cpp", "open_map(): error while loading map file %s!", filename);
		return;
	}

	// first of all get the amount of static and animated models
	unsigned int asmodels = 0;
	unsigned int aamodels = 0;
	for(unsigned int i = 0; i < memap->object_count; i++) {
		memap->map_objects[i].model_type ? aamodels++ : asmodels++;
	}

	// reserver memory for the models and materials
	models.clear();
	amodels.clear();
	materials.clear();
	objects.clear();

	// now, when everything is okay, "load" each object "into the scene"
	for(unsigned int i = 0; i < memap->object_count; i++) {
		objects.push_back(*new map_object());
		memcpy(objects.back().model_name, memap->map_objects[i].model_name, 64);
		memcpy(objects.back().model_filename, memap->map_objects[i].model_filename, 64);
		memcpy(objects.back().ani_filename, memap->map_objects[i].ani_filename, 64);
		memcpy(objects.back().mat_filename, memap->map_objects[i].mat_filename, 64);

		if(!memap->map_objects[i].model_type) {
			models.push_back(*sce->create_a2emodel());
			sce->add_model(&models.back());

			models.back().load_model((char*)string(e->get_data_path() + objects.back().model_filename).c_str());
			models.back().set_position(memap->map_objects[i].position.x,
											memap->map_objects[i].position.y,
											memap->map_objects[i].position.z);
			models.back().set_rotation(memap->map_objects[i].orientation.x,
											memap->map_objects[i].orientation.y,
											memap->map_objects[i].orientation.z);
			models.back().set_scale(memap->map_objects[i].scale.x,
											memap->map_objects[i].scale.y,
											memap->map_objects[i].scale.z);

			materials.push_back(*new a2ematerial(e));
			materials.back().load_material((char*)string(e->get_data_path() + objects.back().mat_filename).c_str());
			models.back().set_material(&materials.back());

			objects.back().model = --models.end();
			objects.back().mat = --materials.end();
			objects.back().type = false;
			objects.back().phys_type = memap->map_objects[i].phys_type;
		}
		else {
			amodels.push_back(*sce->create_a2eanim());
			sce->add_model(&amodels.back());

			amodels.back().load_model((char*)string(e->get_data_path() + objects.back().model_filename).c_str());
			amodels.back().set_position(memap->map_objects[i].position.x,
											memap->map_objects[i].position.y,
											memap->map_objects[i].position.z);
			amodels.back().set_rotation(memap->map_objects[i].orientation.x,
											memap->map_objects[i].orientation.y,
											memap->map_objects[i].orientation.z);
			amodels.back().set_scale(memap->map_objects[i].scale.x,
											memap->map_objects[i].scale.y,
											memap->map_objects[i].scale.z);
			amodels.back().add_animation((char*)string(e->get_data_path() + objects.back().ani_filename).c_str());
			amodels.back().play_frames(0, 0); // don't play frames

			materials.push_back(*new a2ematerial(e));
			materials.back().load_material((char*)string(e->get_data_path() + objects.back().mat_filename).c_str());
			amodels.back().set_material(&materials.back());

			objects.back().amodel = --amodels.end();
			objects.back().mat = --materials.end();
			objects.back().type = true;
			objects.back().phys_type = memap->map_objects[i].phys_type;
		}
	}

	map_opened = true;
}

void mapeditor::select_object(vertex3* look_at) {
	line3* sel_line = new line3(&(*e->get_position() * -1.0f), look_at);
	core::aabbox* tmp_bbox = new core::aabbox();

	// method: we go through all models and check wether they are
	// hit be the sel_line. those which do so are checked for their
	// cam_pos <-> bbox pos distance and the one with the smallest
	// distance will be selected. -- actually this can't be the best
	// solution, but it works quite well ;)

	float min_distance = 1500.0f;
	bool obj_found = false;
	list<map_object>::iterator min_object;

	for(list<map_object>::iterator obj_iter = objects.begin(); obj_iter != objects.end(); obj_iter++) {
		if(!obj_iter->type) {
			memcpy(tmp_bbox, obj_iter->model->get_bounding_box(), 24);
			tmp_bbox->vmin.adjust(obj_iter->model->get_scale());
			tmp_bbox->vmax.adjust(obj_iter->model->get_scale());
			tmp_bbox->vmin += obj_iter->model->get_position();
			tmp_bbox->vmax += obj_iter->model->get_position();
			if(e->get_core()->is_line_in_bbox(tmp_bbox, sel_line)) {
				float distance = e->get_core()->get_distance(e->get_position(),
								&((obj_iter->model->get_bounding_box()->vmin + obj_iter->model->get_bounding_box()->vmax) * 0.5f));
				if(distance < min_distance) {
					min_distance = distance;
					min_object = obj_iter;
					obj_found = true;
				}
			}
		}
		else {
			memcpy(tmp_bbox, obj_iter->amodel->get_bounding_box(), 24);
			tmp_bbox->vmin.adjust(obj_iter->amodel->get_scale());
			tmp_bbox->vmax.adjust(obj_iter->amodel->get_scale());
			tmp_bbox->vmin += obj_iter->amodel->get_position();
			tmp_bbox->vmax += obj_iter->amodel->get_position();
			if(e->get_core()->is_line_in_bbox(tmp_bbox, sel_line)) {
				float distance = e->get_core()->get_distance(e->get_position(),
								&((obj_iter->amodel->get_bounding_box()->vmin + obj_iter->amodel->get_bounding_box()->vmax) * 0.5f));
				if(distance < min_distance) {
					min_distance = distance;
					min_object = obj_iter;
					obj_found = true;
				}
			}
		}
	}

	if(obj_found) {
		if(!min_object->type) {
			memcpy(cur_bbox, min_object->model->get_bounding_box(), 24);
			cur_bbox->vmin.adjust(min_object->model->get_scale());
			cur_bbox->vmax.adjust(min_object->model->get_scale());
			cur_bbox->vmin += min_object->model->get_position();
			cur_bbox->vmax += min_object->model->get_position();
		}
		else {
			memcpy(cur_bbox, min_object->amodel->get_bounding_box(), 24);
			cur_bbox->vmin.adjust(min_object->amodel->get_scale());
			cur_bbox->vmax.adjust(min_object->amodel->get_scale());
			cur_bbox->vmin += min_object->amodel->get_position();
			cur_bbox->vmax += min_object->amodel->get_position();
		}

		sel_mobject = min_object;
		sel = true;
	}
	else {
		cur_bbox->vmin.x = 0.0f;
		cur_bbox->vmin.y = 0.0f;
		cur_bbox->vmin.z = 0.0f;
		cur_bbox->vmax.x = 0.0f;
		cur_bbox->vmax.y = 0.0f;
		cur_bbox->vmax.z = 0.0f;

		sel = false;
	}

	new_sel = true;

	delete tmp_bbox;
	delete sel_line;
}

list<mapeditor::map_object>::iterator mapeditor::get_sel_object() {
	return sel_mobject;
}

bool mapeditor::get_new_sel() {
	return mapeditor::new_sel;
}

void mapeditor::set_new_sel(bool state) {
	mapeditor::new_sel = state;
}

void mapeditor::reload_cur_bbox() {
	if(!is_sel())  {
		cur_bbox->vmin.x = 0.0f;
		cur_bbox->vmin.y = 0.0f;
		cur_bbox->vmin.z = 0.0f;
		cur_bbox->vmax.x = 0.0f;
		cur_bbox->vmax.y = 0.0f;
		cur_bbox->vmax.z = 0.0f;
	}
	else {
		if(!sel_mobject->type) {
			memcpy(cur_bbox, sel_mobject->model->get_bounding_box(), 24);
			cur_bbox->vmin.adjust(sel_mobject->model->get_scale());
			cur_bbox->vmax.adjust(sel_mobject->model->get_scale());
			cur_bbox->vmin += sel_mobject->model->get_position();
			cur_bbox->vmax += sel_mobject->model->get_position();
		}
		else {
			memcpy(cur_bbox, sel_mobject->amodel->get_bounding_box(), 24);
			cur_bbox->vmin.adjust(sel_mobject->amodel->get_scale());
			cur_bbox->vmax.adjust(sel_mobject->amodel->get_scale());
			cur_bbox->vmin += sel_mobject->amodel->get_position();
			cur_bbox->vmax += sel_mobject->amodel->get_position();
		}
	}
}

bool mapeditor::is_map_opened() {
	return mapeditor::map_opened;
}

void mapeditor::close_map() {
	for(list<map_object>::iterator obj_iter = objects.begin(); obj_iter != objects.end(); obj_iter++) {
		!obj_iter->type ? sce->delete_model(&*obj_iter->model) : sce->delete_model(&*obj_iter->amodel);
	}

	models.clear();
	amodels.clear();
	materials.clear();
	objects.clear();

	cur_map_name = "";

	new_sel = true;
	sel = false;
	reload_cur_bbox();

	memap->close_map();

	map_opened = false;
}

void mapeditor::add_object(char* model_filename, char* mat_filename) {
	objects.push_back(*new map_object());
	memcpy(objects.back().model_name, "Model", 32);
	memcpy(objects.back().model_filename, model_filename, 32);
	memcpy(objects.back().ani_filename, "\0", 32);
	memcpy(objects.back().mat_filename, mat_filename, 32);

	models.push_back(*sce->create_a2emodel());
	sce->add_model(&models.back());

	models.back().load_model((char*)string(e->get_data_path() + objects.back().model_filename).c_str());

	materials.push_back(*new a2ematerial(e));
	materials.back().load_material((char*)string(e->get_data_path() + objects.back().mat_filename).c_str());
	models.back().set_material(&materials.back());

	objects.back().model = --models.end();
	objects.back().mat = --materials.end();
	objects.back().type = false;
	objects.back().phys_type = 0;
}

void mapeditor::add_object(char* model_filename, char* ani_filename, char* mat_filename) {
	objects.push_back(*new map_object());
	memcpy(objects.back().model_name, "Model", 64);
	memcpy(objects.back().model_filename, model_filename, 64);
	memcpy(objects.back().ani_filename, ani_filename, 64);
	memcpy(objects.back().mat_filename, mat_filename, 64);

	amodels.push_back(*sce->create_a2eanim());
	sce->add_model(&amodels.back());

	amodels.back().load_model((char*)string(e->get_data_path() + objects.back().model_filename).c_str());
	amodels.back().add_animation((char*)string(e->get_data_path() + ani_filename).c_str());
	amodels.back().play_frames(0, 0); // don't play frames

	materials.push_back(*new a2ematerial(e));
	materials.back().load_material((char*)string(e->get_data_path() + objects.back().mat_filename).c_str());
	amodels.back().set_material(&materials.back());

	objects.back().amodel = --amodels.end();
	objects.back().mat = --materials.end();
	objects.back().type = true;
	objects.back().phys_type = 0;
}

void mapeditor::delete_obj() {
	if(!is_sel()) {
		return;
	}

	if(!sel_mobject->type) {
		sce->delete_model(&*sel_mobject->model);
		models.erase(sel_mobject->model);
	}
	else {
		sce->delete_model(&*sel_mobject->amodel);
		amodels.erase(sel_mobject->amodel);
	}
	materials.erase(sel_mobject->mat);
	objects.erase(sel_mobject);

	new_sel = true;
	sel = false;
	reload_cur_bbox();
}

list<a2emodel>* mapeditor::get_models() {
	return &models;
}

list<a2eanim>* mapeditor::get_amodels() {
	return &amodels;
}

list<a2ematerial>* mapeditor::get_materials() {
	return &materials;
}

bool mapeditor::is_sel() {
	return sel;
}
