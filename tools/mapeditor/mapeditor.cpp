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

mapeditor::mapeditor(engine* e, scene* sce, camera* cam) {
	mapeditor::e = e;
	mapeditor::sce = sce;
	mapeditor::m = e->get_msg();
	mapeditor::c = e->get_core();
	mapeditor::cam = cam;

	memap = new cmap(e);

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

	// load arrow stuff
	arrow_selected = new bool[3];
	for(unsigned int i = 0; i < 3; i++) {
		arrow[i] = sce->create_a2emodel();
		arrow[i]->load_model(e->data_path("arrow.a2m"), false); // TODO: if i'd use a vbo for this object and do NOT render it, i get weird results (totally screwed up graphic output)

		arrow_mat[i] = new a2ematerial(e);

		arrow_bbox[i] = new core::aabbox(*arrow[i]->get_bounding_box());
		arrow_bbox_tmp[i] = new core::aabbox();
		arrow_selected[i] = false;
	}

	arrow_mat[0]->load_material(e->data_path("arrow_x.a2mtl"));
	arrow_mat[1]->load_material(e->data_path("arrow_y.a2mtl"));
	arrow_mat[2]->load_material(e->data_path("arrow_z.a2mtl"));

	for(unsigned int i = 0; i < 3; i++) {
		arrow[i]->set_material(arrow_mat[i]);
	}

	sel_line = new line3();
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

	for(unsigned int i = 0; i < 3; i++) {
		delete arrow[i];
		delete arrow_mat[i];
		delete arrow_bbox[i];
		delete arrow_bbox_tmp[i];
	}
	delete [] arrow_selected;

	delete sel_line;
}

void mapeditor::run(bool cam_input) {
	// draw bounding box of selected model
	e->get_gfx()->draw_bbox(cur_bbox, 0xFFFFFF);

	if(sel && !cam_input) {
		// draw x, y and z axis arrows

		vertex3* pos = sel_mobject->type ? sel_mobject->amodel->get_position() : sel_mobject->model->get_position();
		core::pnt* p2d = new core::pnt();
		c->get_2d_from_3d(pos, p2d);

		glPushMatrix();
		glPushAttrib(GL_ENABLE_BIT);

		glDisable(GL_TEXTURE_2D);
		glDisable(GL_LIGHTING);
		glDisable(GL_BLEND);

		// clear depth buffer so the axis arrows will "overwrite" all other stuff
		glClear(GL_DEPTH_BUFFER_BIT);


		line3 l(pos->x, pos->y, pos->z, -e->get_position()->x, -e->get_position()->y, -e->get_position()->z);
		float scale = l.get_length() / 600.0f; // a length of 6 (arrow size(100) * length(6) = 1000) is the "base scale" of 1.0f
		for(unsigned int i = 0; i < 3; i++) { arrow[i]->set_scale(scale, scale, scale); }

		for(unsigned int i = 0; i < 3; i++) { arrow[i]->get_position()->set(pos); }
		arrow[0]->set_rotation(0.0f, 0.0f, 90.0f);
		arrow[1]->set_rotation(0.0f, 0.0f, 0.0f);
		arrow[2]->set_rotation(-90.0f, 0.0f, 0.0f);

		// copy bbox data to tmp bboxes, scale, rotate and reposition tmp bboxes
		for(unsigned int i = 0; i < 3; i++) {
			memcpy(arrow_bbox_tmp[i], arrow_bbox[i], sizeof(core::aabbox));
			arrow_bbox_tmp[i]->vmin *= scale;
			arrow_bbox_tmp[i]->vmax *= scale;
		}

		float rsin, rcos;
		vertex3* tmp_pos;

		rsin = sinf(90.0f*(float)PIOVER180);
		rcos = cosf(90.0f*(float)PIOVER180);
		tmp_pos = &arrow_bbox_tmp[0]->vmax;
		arrow_bbox_tmp[0]->vmax.set(tmp_pos->x * rcos + tmp_pos->y * rsin, tmp_pos->y * rcos - tmp_pos->x * rsin, tmp_pos->z);
		tmp_pos = &arrow_bbox_tmp[0]->vmin;
		arrow_bbox_tmp[0]->vmin.set(tmp_pos->x * rcos + tmp_pos->y * rsin, tmp_pos->y * rcos - tmp_pos->x * rsin, tmp_pos->z);

		rsin = sinf(-90.0f*(float)PIOVER180);
		rcos = cosf(-90.0f*(float)PIOVER180);
		tmp_pos = &arrow_bbox_tmp[2]->vmax;
		arrow_bbox_tmp[2]->vmax.set(tmp_pos->x, tmp_pos->y * rcos + tmp_pos->z * rsin, tmp_pos->z * rcos - tmp_pos->y * rsin);
		tmp_pos = &arrow_bbox_tmp[2]->vmin;
		arrow_bbox_tmp[2]->vmin.set(tmp_pos->x, tmp_pos->y * rcos + tmp_pos->z * rsin, tmp_pos->z * rcos - tmp_pos->y * rsin);

		for(unsigned int i = 0; i < 3; i++) {
			arrow_bbox_tmp[i]->vmin += pos;
			arrow_bbox_tmp[i]->vmax += pos;
		}

		arrow[0]->draw(false);
		//e->get_gfx()->draw_bbox(arrow_bbox_tmp[0], 0xFFFFFF);
		arrow[1]->draw(false);
		//e->get_gfx()->draw_bbox(arrow_bbox_tmp[1], 0xFFFFFF);
		arrow[2]->draw(false);
		//e->get_gfx()->draw_bbox(arrow_bbox_tmp[2], 0xFFFFFF);


		glPopAttrib();
		glPopMatrix();
		
		delete p2d;
	}
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
	memap->map_objects = new cmap::map_object[objects.size()];
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

			models.back().load_model(e->data_path(objects.back().model_filename));
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
			materials.back().load_material(e->data_path(objects.back().mat_filename));
			models.back().set_material(&materials.back());

			objects.back().model = --models.end();
			objects.back().mat = --materials.end();
			objects.back().type = false;
			objects.back().phys_type = memap->map_objects[i].phys_type;
		}
		else {
			amodels.push_back(*sce->create_a2eanim());
			sce->add_model(&amodels.back());

			amodels.back().load_model(e->data_path(objects.back().model_filename));
			amodels.back().set_position(memap->map_objects[i].position.x,
											memap->map_objects[i].position.y,
											memap->map_objects[i].position.z);
			amodels.back().set_rotation(memap->map_objects[i].orientation.x,
											memap->map_objects[i].orientation.y,
											memap->map_objects[i].orientation.z);
			amodels.back().set_scale(memap->map_objects[i].scale.x,
											memap->map_objects[i].scale.y,
											memap->map_objects[i].scale.z);
			amodels.back().add_animation(e->data_path(objects.back().ani_filename));
			amodels.back().play_frames(0, 0); // don't play frames

			materials.push_back(*new a2ematerial(e));
			materials.back().load_material(e->data_path(objects.back().mat_filename));
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
	*sel_line->v1 = e->get_position();
	*sel_line->v1 *= -1.0f;
	*sel_line->v2 = look_at;
	core::aabbox* tmp_bbox = new core::aabbox();

	// method: we go through all models and check wether they are
	// hit by the sel_line. those which do so are checked for their
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
			if(c->is_line_in_bbox(tmp_bbox, sel_line)) {
				float distance = c->get_distance(e->get_position(),
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
			if(c->is_line_in_bbox(tmp_bbox, sel_line)) {
				float distance = c->get_distance(e->get_position(),
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
	memcpy(objects.back().model_name, "Model", 64);
	memcpy(objects.back().model_filename, model_filename, 64);
	memcpy(objects.back().ani_filename, "\0", 64);
	memcpy(objects.back().mat_filename, mat_filename, 64);

	models.push_back(*sce->create_a2emodel());
	sce->add_model(&models.back());

	models.back().load_model(e->data_path(objects.back().model_filename));

	materials.push_back(*new a2ematerial(e));
	materials.back().load_material(e->data_path(objects.back().mat_filename));
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

	amodels.back().load_model(e->data_path(objects.back().model_filename));
	amodels.back().add_animation(e->data_path(ani_filename));
	amodels.back().play_frames(0, 0); // don't play frames

	materials.push_back(*new a2ematerial(e));
	materials.back().load_material(e->data_path(objects.back().mat_filename));
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

void mapeditor::new_map(char* filename) {
	memap->new_map(filename);
}

list<mapeditor::map_object>* mapeditor::get_objects() {
	return &objects;
}

void mapeditor::arrow_select(vertex3* look_at) {
	// if no object is selected, return
	if(!sel) return;

	// create selection/intersection line
	*sel_line->v1 = e->get_position();
	*sel_line->v1 *= -1.0f;
	*sel_line->v2 = look_at;

	arrow_selected[0] = c->is_line_in_bbox(arrow_bbox_tmp[0], sel_line);
	arrow_selected[1] = c->is_line_in_bbox(arrow_bbox_tmp[1], sel_line);
	arrow_selected[2] = c->is_line_in_bbox(arrow_bbox_tmp[2], sel_line);
}

void mapeditor::move_object(int x, int y, float scale) {
	if(!arrow_selected[0] && !arrow_selected[1] && !arrow_selected[2]) return;

	vertex3* pos = sel_mobject->type ? sel_mobject->amodel->get_position() : sel_mobject->model->get_position();
	line3 l(pos->x, pos->y, pos->z, -e->get_position()->x, -e->get_position()->y, -e->get_position()->z);
	vertex3* dir = &sel_line->get_direction();

	float x_size = (l.get_length() / e->get_screen()->w) * scale;
	float y_size = (l.get_length() / e->get_screen()->h) * scale;

	float strengthx, strengthy;
	if(arrow_selected[0]) {
		strengthx = fabs(fabs(dir->x) - 1.0f);
		strengthy = fabs(fabs(dir->y) - 1.0f);
		pos->x += x_size*(float)x*strengthx;
		pos->x += y_size*(float)y*-1.0f*strengthy;
	}
	if(arrow_selected[1]) {
		pos->y += y_size*(float)y*-1.0f;
	}
	if(arrow_selected[2]) {
		strengthx = fabs(fabs(dir->x) - 1.0f);
		strengthy = fabs(fabs(dir->y) - 1.0f);
		pos->z += x_size*(float)y*strengthx;
		pos->z += y_size*(float)x*strengthy;
	}

	reload_cur_bbox();
	new_sel = true;
}
