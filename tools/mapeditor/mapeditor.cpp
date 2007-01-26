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
	mapeditor::g = e->get_gfx();
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
	ctrl_obj_selected = new bool[3];
	for(unsigned int i = 0; i < 3; i++) {
		arrow[i] = sce->create_a2emodel();
		arrow[i]->load_model(e->data_path("arrow.a2m"), true);
		circle[i] = sce->create_a2emodel();
		circle[i]->load_model(e->data_path("circle.a2m"), true);
		arrow_cube[i] = sce->create_a2emodel();
		arrow_cube[i]->load_model(e->data_path("arrow_cube.a2m"), true);

		arrow_mat[i] = new a2ematerial(e);
		circle_mat[i] = new a2ematerial(e);

		ctrl_obj_bbox[i] = new core::aabbox(*arrow[i]->get_bounding_box());
		ctrl_obj_bbox_tmp[i] = new core::aabbox();
		ctrl_obj_selected[i] = false;
	}

	sphere = sce->create_a2emodel();
	sphere->load_model(e->data_path("rotate_sphere.a2m"), true);
	sphere->set_hard_scale(100.0f, 100.0f, 100.0f);
	sphere_mat = new a2ematerial(e);
	sphere_mat->load_material(e->data_path("rotate_sphere.a2mtl"));
	sphere->set_material(sphere_mat);

	arrow_mat[0]->load_material(e->data_path("arrow_x.a2mtl"));
	arrow_mat[1]->load_material(e->data_path("arrow_y.a2mtl"));
	arrow_mat[2]->load_material(e->data_path("arrow_z.a2mtl"));
	circle_mat[0]->load_material(e->data_path("circle_x.a2mtl"));
	circle_mat[1]->load_material(e->data_path("circle_y.a2mtl"));
	circle_mat[2]->load_material(e->data_path("circle_z.a2mtl"));

	for(unsigned int i = 0; i < 3; i++) {
		arrow[i]->set_material(arrow_mat[i]);
		circle[i]->set_material(circle_mat[i]);
		arrow_cube[i]->set_material(arrow_mat[i]);
	}

	sel_line = new line3();
	edit_line = new line3();

	intersection_point = new vertex3();

	draw_mode = false;

	objects_visibility = true;
	physical_map = false;

	for(unsigned int i = 0; i < 4; i++) {
		move_toggle[i] = false;
		rotate_toggle[i] = false;
		scale_toggle[i] = false;
		phys_scale_toggle[i] = false;
	}

	init_pos = new vertex3(0.0f, 0.0f, 0.0f);
	init_rot = new vertex3(0.0f, 0.0f, 0.0f);
	init_scl = new vertex3(1.0f, 1.0f, 1.0f);
	init_phys_scl = new vertex3(1.0f, 1.0f, 1.0f);
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
		delete circle[i];
		delete arrow_cube[i];
		delete arrow_mat[i];
		delete circle_mat[i];
		delete ctrl_obj_bbox[i];
		delete ctrl_obj_bbox_tmp[i];
	}
	delete [] ctrl_obj_selected;
	delete sphere;
	delete sphere_mat;

	delete sel_line;
	delete edit_line;

	delete intersection_point;

	delete init_pos;
	delete init_rot;
	delete init_scl;
	delete init_phys_scl;
}

void mapeditor::run(EDIT_STATE edit_state) {
	mapeditor::edit_state = edit_state;
	// draw bounding box of selected model
	e->get_gfx()->draw_bbox(cur_bbox, 0xFFFFFF);

	if(sel && (edit_state == mapeditor::MOVE || edit_state == mapeditor::ROTATE || edit_state == mapeditor::SCALE || edit_state == mapeditor::PHYS_SCALE)) {
		// draw x, y and z axis arrows/circles
		a2emodel** ctrl_obj_models = NULL;
		if(edit_state == mapeditor::MOVE) {
			ctrl_obj_models = arrow;
			ctrl_obj_models[0]->set_rotation(0.0f, 0.0f, 90.0f);
			ctrl_obj_models[1]->set_rotation(0.0f, 0.0f, 0.0f);
			ctrl_obj_models[2]->set_rotation(-90.0f, 0.0f, 0.0f);
		}
		else if(edit_state == mapeditor::ROTATE) {
			ctrl_obj_models = circle;
			ctrl_obj_models[0]->set_rotation(0.0f, 0.0f, 0.0f);
			ctrl_obj_models[1]->set_rotation(0.0f, 0.0f, 90.0f);
			ctrl_obj_models[2]->set_rotation(0.0f, 90.0f, 0.0f);
		}
		else if(edit_state == mapeditor::SCALE || edit_state == mapeditor::PHYS_SCALE) {
			ctrl_obj_models = arrow_cube;
			ctrl_obj_models[0]->set_rotation(0.0f, 0.0f, 90.0f);
			ctrl_obj_models[1]->set_rotation(0.0f, 0.0f, 0.0f);
			ctrl_obj_models[2]->set_rotation(-90.0f, 0.0f, 0.0f);
		}

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


		line3 l(pos->x, pos->y, pos->z, -cam->get_position()->x, -cam->get_position()->y, -cam->get_position()->z);
		//float scale = l.get_length() / 600.0f; // a length of 6 (arrow size(100) * length(6) = 1000) is the "base scale" of 1.0f
		float scale = l.get_length() / 450.0f;
		for(unsigned int i = 0; i < 3; i++) { ctrl_obj_models[i]->set_scale(scale, scale, scale); }

		for(unsigned int i = 0; i < 3; i++) { ctrl_obj_models[i]->get_position()->set(pos); }

		// copy bbox data to tmp bboxes, scale, rotate and reposition tmp bboxes
		for(unsigned int i = 0; i < 3; i++) {
			memcpy(ctrl_obj_bbox[i], ctrl_obj_models[i]->get_bounding_box(), sizeof(core::aabbox));
			memcpy(ctrl_obj_bbox_tmp[i], ctrl_obj_bbox[i], sizeof(core::aabbox));
			ctrl_obj_bbox_tmp[i]->vmin *= scale;
			ctrl_obj_bbox_tmp[i]->vmax *= scale;
		}

		float rsin, rcos;
		vertex3* tmp_pos;

		if(edit_state == mapeditor::MOVE || edit_state == mapeditor::SCALE || edit_state == mapeditor::PHYS_SCALE) {
			rsin = sinf(90.0f*(float)PIOVER180);
			rcos = cosf(90.0f*(float)PIOVER180);
			tmp_pos = &ctrl_obj_bbox_tmp[0]->vmax;
			ctrl_obj_bbox_tmp[0]->vmax.set(tmp_pos->x * rcos + tmp_pos->y * rsin, tmp_pos->y * rcos - tmp_pos->x * rsin, tmp_pos->z);
			tmp_pos = &ctrl_obj_bbox_tmp[0]->vmin;
			ctrl_obj_bbox_tmp[0]->vmin.set(tmp_pos->x * rcos + tmp_pos->y * rsin, tmp_pos->y * rcos - tmp_pos->x * rsin, tmp_pos->z);

			rsin = sinf(-90.0f*(float)PIOVER180);
			rcos = cosf(-90.0f*(float)PIOVER180);
			tmp_pos = &ctrl_obj_bbox_tmp[2]->vmax;
			ctrl_obj_bbox_tmp[2]->vmax.set(tmp_pos->x, tmp_pos->y * rcos + tmp_pos->z * rsin, tmp_pos->z * rcos - tmp_pos->y * rsin);
			tmp_pos = &ctrl_obj_bbox_tmp[2]->vmin;
			ctrl_obj_bbox_tmp[2]->vmin.set(tmp_pos->x, tmp_pos->y * rcos + tmp_pos->z * rsin, tmp_pos->z * rcos - tmp_pos->y * rsin);

			for(unsigned int i = 0; i < 3; i++) {
				ctrl_obj_bbox_tmp[i]->vmin += pos;
				ctrl_obj_bbox_tmp[i]->vmax += pos;
			}
		}
		else if(edit_state == mapeditor::ROTATE) {
			vertex3* n = &l.get_direction();
			vertex3* cam_pos = cam->get_position();
			vertex3* f = new vertex3();
			// calculate distance: d = |P • n| / |n|
			double distance = fabs(pos->x*n->x + pos->y*n->y + pos->z*n->z) / sqrt(n->x*n->x + n->y*n->y + n->z*n->z);
			// intersection point (f) on the plane
			float t = n->x*n->x + n->y*n->y + n->z*n->z;
			float r = -(pos->x*n->x + pos->y*n->y + pos->z*n->z);
			r /= t;
			f->set(pos->x+n->x*r, pos->y+n->y*r, pos->z+n->z*r);
			// check if intersection point or model position is closer to the camera
			float distance_f_cam = sqrtf(powf(f->x + cam_pos->x, 2) + powf(f->y + cam_pos->y, 2) + powf(f->z + cam_pos->z, 2));
			if(distance_f_cam > l.get_length()) {
				// if so, "invert" the distance
				distance *= -1.0;
			}
			delete f;
			double plane[4] = {n->x, n->y, n->z, distance};
			glClipPlane(GL_CLIP_PLANE0, plane);
			glEnable(GL_CLIP_PLANE0);

			sphere->get_position()->set(pos);
			sphere->set_scale(scale, scale, scale);
			sphere->draw(false);

			radius = 50.0f * scale;

			glEnable(GL_POLYGON_OFFSET_FILL); // use polygon offset to "disable" z-fighting
			glPolygonOffset(-1.0f, 0.0f);

			rsin = sinf(90.0f*(float)PIOVER180);
			rcos = cosf(90.0f*(float)PIOVER180);
			tmp_pos = &ctrl_obj_bbox_tmp[1]->vmax;
			ctrl_obj_bbox_tmp[1]->vmax.set(tmp_pos->x * rcos + tmp_pos->y * rsin, tmp_pos->y * rcos - tmp_pos->x * rsin, tmp_pos->z);
			tmp_pos = &ctrl_obj_bbox_tmp[1]->vmin;
			ctrl_obj_bbox_tmp[1]->vmin.set(tmp_pos->x * rcos + tmp_pos->y * rsin, tmp_pos->y * rcos - tmp_pos->x * rsin, tmp_pos->z);

			rsin = sinf(90.0f*(float)PIOVER180);
			rcos = cosf(90.0f*(float)PIOVER180);
			tmp_pos = &ctrl_obj_bbox_tmp[2]->vmax;
			ctrl_obj_bbox_tmp[2]->vmax.set(tmp_pos->x * rcos + tmp_pos->z * rsin, tmp_pos->y, tmp_pos->z * rcos - tmp_pos->x * rsin);
			tmp_pos = &ctrl_obj_bbox_tmp[2]->vmin;
			ctrl_obj_bbox_tmp[2]->vmin.set(tmp_pos->x * rcos + tmp_pos->z * rsin, tmp_pos->y, tmp_pos->z * rcos - tmp_pos->x * rsin);

			// scale bounding boxes (for more input possibilities)
			float bbox_scale = 10.0f;
			ctrl_obj_bbox_tmp[0]->vmin.x *= bbox_scale;
			ctrl_obj_bbox_tmp[0]->vmax.x *= bbox_scale;
			ctrl_obj_bbox_tmp[1]->vmin.y *= bbox_scale;
			ctrl_obj_bbox_tmp[1]->vmax.y *= bbox_scale;
			ctrl_obj_bbox_tmp[2]->vmin.z *= bbox_scale;
			ctrl_obj_bbox_tmp[2]->vmax.z *= bbox_scale;

			for(unsigned int i = 0; i < 3; i++) {
				ctrl_obj_bbox_tmp[i]->vmin += pos;
				ctrl_obj_bbox_tmp[i]->vmax += pos;
			}
		}

		ctrl_obj_models[0]->draw(false);
		ctrl_obj_models[1]->draw(false);
		ctrl_obj_models[2]->draw(false);

		if(edit_state == mapeditor::ROTATE) {
			glDisable(GL_POLYGON_OFFSET_FILL);
			glDisable(GL_CLIP_PLANE0);
		}


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
		memap->map_objects[i].ani_filename = obj_iter->ani_filename;
		memap->map_objects[i].mat_filename = obj_iter->mat_filename;
		memap->map_objects[i].model_filename = obj_iter->model_filename;
		memap->map_objects[i].model_name = obj_iter->model_name;
		memap->map_objects[i].model_type = obj_iter->type;
		memap->map_objects[i].phys_type = obj_iter->phys_type;
		memap->map_objects[i].gravity = obj_iter->gravity;
		memap->map_objects[i].collision_model = obj_iter->collision_model;
		memap->map_objects[i].auto_mass = obj_iter->auto_mass;
		memap->map_objects[i].mass = obj_iter->mass;
		if(!obj_iter->type) {
			memap->map_objects[i].position = obj_iter->model->get_position();
			memap->map_objects[i].orientation = obj_iter->model->get_rotation();
			memap->map_objects[i].scale = obj_iter->model->get_scale();
			memap->map_objects[i].phys_scale = obj_iter->model->get_phys_scale();
		}
		else {
			memap->map_objects[i].position = obj_iter->amodel->get_position();
			memap->map_objects[i].orientation = obj_iter->amodel->get_rotation();
			memap->map_objects[i].scale = obj_iter->amodel->get_scale();
			memap->map_objects[i].phys_scale = obj_iter->amodel->get_phys_scale();
		}
		obj_iter->position = &memap->map_objects[i].position;
		obj_iter->orientation = &memap->map_objects[i].orientation;
		obj_iter->scale = &memap->map_objects[i].scale;
		obj_iter->phys_scale = &memap->map_objects[i].phys_scale;
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
		objects.back().model_name = memap->map_objects[i].model_name;
		objects.back().model_filename = memap->map_objects[i].model_filename;
		objects.back().ani_filename = memap->map_objects[i].ani_filename;
		objects.back().mat_filename = memap->map_objects[i].mat_filename;

		objects.back().position = &memap->map_objects[i].position;
		objects.back().orientation = &memap->map_objects[i].orientation;
		objects.back().scale = &memap->map_objects[i].scale;
		objects.back().phys_scale = &memap->map_objects[i].phys_scale;

		if(!memap->map_objects[i].model_type) {
			models.push_back(*sce->create_a2emodel());
			sce->add_model(&models.back());

			models.back().load_model(e->data_path(objects.back().model_filename.c_str()), true);
			models.back().set_position(memap->map_objects[i].position.x,
											memap->map_objects[i].position.y,
											memap->map_objects[i].position.z);
			models.back().set_rotation(memap->map_objects[i].orientation.x,
											memap->map_objects[i].orientation.y,
											memap->map_objects[i].orientation.z);
			models.back().set_scale(memap->map_objects[i].scale.x,
											memap->map_objects[i].scale.y,
											memap->map_objects[i].scale.z);
			models.back().set_phys_scale(memap->map_objects[i].phys_scale.x,
											memap->map_objects[i].phys_scale.y,
											memap->map_objects[i].phys_scale.z);

			models.back().set_draw_wireframe(draw_mode);

			objects.back().model = --models.end();
			objects.back().type = false;
		}
		else {
			amodels.push_back(*sce->create_a2eanim());
			sce->add_model(&amodels.back());

			amodels.back().load_model(e->data_path(objects.back().model_filename.c_str()), true);
			amodels.back().set_position(memap->map_objects[i].position.x,
											memap->map_objects[i].position.y,
											memap->map_objects[i].position.z);
			amodels.back().set_rotation(memap->map_objects[i].orientation.x,
											memap->map_objects[i].orientation.y,
											memap->map_objects[i].orientation.z);
			amodels.back().set_scale(memap->map_objects[i].scale.x,
											memap->map_objects[i].scale.y,
											memap->map_objects[i].scale.z);
			amodels.back().set_phys_scale(memap->map_objects[i].phys_scale.x,
											memap->map_objects[i].phys_scale.y,
											memap->map_objects[i].phys_scale.z);

			amodels.back().set_draw_wireframe(draw_mode);

			amodels.back().add_animation(e->data_path(objects.back().ani_filename.c_str()));
			amodels.back().play_frames(0, 0); // don't play frames

			objects.back().amodel = --amodels.end();
			objects.back().type = true;
		}

		materials.push_back(*new a2ematerial(e));
		materials.back().load_material(e->data_path(objects.back().mat_filename.c_str()));
		objects.back().type ? amodels.back().set_material(&materials.back()) : models.back().set_material(&materials.back());
		objects.back().mat = --materials.end();

		objects.back().phys_type = memap->map_objects[i].phys_type;
		objects.back().gravity = memap->map_objects[i].gravity;
		objects.back().collision_model = memap->map_objects[i].collision_model;
		objects.back().auto_mass = memap->map_objects[i].auto_mass;
		objects.back().mass = memap->map_objects[i].mass;
	}

	map_opened = true;
}

void mapeditor::select_object(vertex3* look_at) {
	*sel_line->v1 = cam->get_position();
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
				float distance = c->get_distance(cam->get_position(),
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
				float distance = c->get_distance(cam->get_position(),
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
	objects.back().model_name = "Model";
	objects.back().model_filename = model_filename;
	objects.back().ani_filename = "\0";
	objects.back().mat_filename = mat_filename;

	models.push_back(*sce->create_a2emodel());
	sce->add_model(&models.back());

	models.back().load_model(e->data_path(objects.back().model_filename.c_str()), true);

	materials.push_back(*new a2ematerial(e));
	materials.back().load_material(e->data_path(objects.back().mat_filename.c_str()));
	models.back().set_material(&materials.back());

	models.back().set_draw_wireframe(draw_mode);

	objects.back().model = --models.end();
	objects.back().mat = --materials.end();
	objects.back().type = false;

	objects.back().phys_type = 0;
	objects.back().gravity = true;
	objects.back().auto_mass = true;
	objects.back().collision_model = false;
	objects.back().mass = 1.0f;

	objects.back().position = init_pos;
	objects.back().orientation = init_rot;
	objects.back().scale = init_scl;
	objects.back().phys_scale = init_phys_scl;
}

void mapeditor::add_object(char* model_filename, char* ani_filename, char* mat_filename) {
	objects.push_back(*new map_object());
	objects.back().model_name = "Model";
	objects.back().model_filename = model_filename;
	objects.back().ani_filename = ani_filename;
	objects.back().mat_filename = mat_filename;

	amodels.push_back(*sce->create_a2eanim());
	sce->add_model(&amodels.back());

	amodels.back().load_model(e->data_path(objects.back().model_filename.c_str()), true);
	amodels.back().add_animation(e->data_path(ani_filename));
	amodels.back().play_frames(0, 0); // don't play frames

	materials.push_back(*new a2ematerial(e));
	materials.back().load_material(e->data_path(objects.back().mat_filename.c_str()));
	amodels.back().set_material(&materials.back());

	amodels.back().set_draw_wireframe(draw_mode);

	objects.back().amodel = --amodels.end();
	objects.back().mat = --materials.end();
	objects.back().type = true;

	objects.back().phys_type = 0;
	objects.back().gravity = true;
	objects.back().auto_mass = true;
	objects.back().collision_model = false;
	objects.back().mass = 1.0f;

	objects.back().position = init_pos;
	objects.back().orientation = init_rot;
	objects.back().scale = init_scl;
	objects.back().phys_scale = init_phys_scl;
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

void mapeditor::select(vertex3* look_at) {
	// if no object is selected, return
	if(!sel) return;

	// create selection/intersection line
	*sel_line->v1 = cam->get_position();
	*sel_line->v1 *= -1.0f;
	*sel_line->v2 = look_at;

	if(edit_state == mapeditor::MOVE || edit_state == mapeditor::SCALE || edit_state == mapeditor::PHYS_SCALE) {
		ctrl_obj_selected[0] = c->is_line_in_bbox(ctrl_obj_bbox_tmp[0], sel_line);
		ctrl_obj_selected[1] = c->is_line_in_bbox(ctrl_obj_bbox_tmp[1], sel_line);
		ctrl_obj_selected[2] = c->is_line_in_bbox(ctrl_obj_bbox_tmp[2], sel_line);
	}
	else if(edit_state == mapeditor::ROTATE) {
		ctrl_obj_selected[0] = false;
		ctrl_obj_selected[1] = false;
		ctrl_obj_selected[2] = false;

		// now a lil complex algorithm
		// 1st: put sel_line into the "sphere equation" ((x - mx)^2 + (y - my)^2 + (z - mz)^2 = r^2)
		// 2nd: if t^2 != 0 -> there are 2 intersection points
		// 3rd: calculate x1 and x2
		// 4th: put x1 and x2 into sel_line and get the intersection points
		// 5th: check which intersection point is closer to the camera position (= ip)
		// 6th: check if ip lays in one of the three axis bounding boxes
		// 7th: if so, select this bounding box (if there is more than one, choose the x (if selected) or y (if selected) axis)
		vertex3* n = &sel_line->get_direction();
		vertex3* pos = sel_mobject->type ? sel_mobject->amodel->get_position() : sel_mobject->model->get_position();
		vertex3* cp = sel_line->v1;
		float sx, sy, sz;
		float x1, x2;
		vertex3 ip1, ip2;

		float tq = n->x*n->x + n->y*n->y + n->z*n->z; // this practically never equals zero
		if(FLOAT_EQ(tq, 0.0f)) return;

		float t = 2 * ((cp->x - pos->x)*n->x + (cp->y - pos->y)*n->y + (cp->z - pos->z)*n->z);
		sx = (cp->x - pos->x);
		sy = (cp->y - pos->y);
		sz = (cp->z - pos->z);
		float s = sx*sx + sy*sy + sz*sz - radius*radius;

		if(FLOAT_EQ(t, 0.0f)) return;

		t /= tq*2.0f;
		s /= tq;

		float sr = t*t - s;
		if(sr < 0.0f) return;
		sr = sqrtf(sr);
		x1 = -t + sr;
		x2 = -t - sr;

		ip1.set(cp->x + x1 * n->x, cp->y + x1 * n->y, cp->z + x1 * n->z);
		ip2.set(cp->x + x2 * n->x, cp->y + x2 * n->y, cp->z + x2 * n->z);

		float dist_ip1 = c->get_distance(&ip1, cp);
		float dist_ip2 = c->get_distance(&ip2, cp);

		if(dist_ip1 < dist_ip2) intersection_point->set(&ip1);
		else intersection_point->set(&ip2);

		ctrl_obj_selected[0] = c->is_vertex_in_bbox(ctrl_obj_bbox_tmp[0], intersection_point);
		if(ctrl_obj_selected[0]) return;

		ctrl_obj_selected[1] = c->is_vertex_in_bbox(ctrl_obj_bbox_tmp[1], intersection_point);
		if(ctrl_obj_selected[1]) return;

		ctrl_obj_selected[2] = c->is_vertex_in_bbox(ctrl_obj_bbox_tmp[2], intersection_point);
	}
}

void mapeditor::edit_object(int x, int y, float scale) {
	if(!ctrl_obj_selected[0] && !ctrl_obj_selected[1] && !ctrl_obj_selected[2]) return;

	vertex3* pos = sel_mobject->type ? sel_mobject->amodel->get_position() : sel_mobject->model->get_position();
	vertex3* rot = sel_mobject->type ? sel_mobject->amodel->get_rotation() : sel_mobject->model->get_rotation();
	vertex3* scl = sel_mobject->type ? sel_mobject->amodel->get_scale() : sel_mobject->model->get_scale();
	vertex3* phys_scl = sel_mobject->type ? sel_mobject->amodel->get_phys_scale() : sel_mobject->model->get_phys_scale();
	edit_line->v1->set(-cam->get_position()->x, -cam->get_position()->y, -cam->get_position()->z);
	edit_line->v2->set(pos->x, pos->y, pos->z);
	vertex3* dir = &edit_line->get_direction();

	float x_size = (edit_line->get_length() / e->get_screen()->w) * scale;
	float y_size = (edit_line->get_length() / e->get_screen()->h) * scale;
	float strengthx, strengthy, sign_x, sign_y, sign_z, dirx, diry, dirz;
	sign_x = dir->x < 0.0f ? -1.0f : 1.0f;
	sign_y = dir->y < 0.0f ? -1.0f : 1.0f;
	sign_z = dir->z < 0.0f ? -1.0f : 1.0f;
	if(edit_state == mapeditor::MOVE) {
		if(ctrl_obj_selected[0] && (!move_toggle[0] || move_toggle[1])) {
			strengthx = fabs(fabs(dir->x) - 1.0f)*-sign_z;
			strengthy = fabs(fabs(dir->z) - 1.0f)*-sign_x;
			pos->x += x_size*(float)x*strengthx;
			pos->x += y_size*(float)y*strengthy;
		}
		if(ctrl_obj_selected[1] && (!move_toggle[0] || move_toggle[2])) {
			pos->y += y_size*(float)y*-1.0f;
		}
		if(ctrl_obj_selected[2] && (!move_toggle[0] || move_toggle[3])) {
			strengthx = fabs(fabs(dir->z) - 1.0f)*sign_x;
			strengthy = fabs(fabs(dir->x) - 1.0f)*-sign_z;
			pos->z += x_size*(float)x*strengthx;
			pos->z += y_size*(float)y*strengthy;
		}
	}
	else if(edit_state == mapeditor::ROTATE) {
		line3 cam_ip_line(-cam->get_position()->x, -cam->get_position()->y, -cam->get_position()->z,
						  intersection_point->x, intersection_point->y, intersection_point->z);

		float a, b, angle, max_angle;
		vertex3* a_vec;
		vertex3* b_vec;
		float sign_view_y_90_270;
		float sign_view_y_0_180;
		if(ctrl_obj_selected[0] && (!rotate_toggle[0] || rotate_toggle[1])) {
			dirx = fabs(edit_line->get_direction().x);
			diry = fabs(sinf(c->deg_to_rad(cam->get_rotation()->y)));

			sign_view_y_90_270 = cam->get_rotation()->y > 90.0f && cam->get_rotation()->y < 270.0f ? -1.0f : 1.0f;
			sign_view_y_0_180 = cam->get_rotation()->y > 0.0f && cam->get_rotation()->y < 180.0f ? -1.0f : 1.0f;

			strengthx = ((((intersection_point->y - pos->y) / radius) * dirx) + (1.0f - dirx) * diry * -sign_y) * sign_view_y_0_180;
			strengthy = ((((radius - fabs(intersection_point->y - pos->y)) / radius) * dirx) + (1.0f - dirx) * (1.0f - diry)) * sign_view_y_90_270;

			strengthx = x_size*(float)x*-strengthx;
			strengthy = y_size*(float)y*-strengthy;

			rot->x += strengthx;
			rot->x += strengthy;
		}
		if(ctrl_obj_selected[1] && (!rotate_toggle[0] || rotate_toggle[2])) {
			diry = fabs(edit_line->get_direction().y);

			edit_line->v1->y = 0.0f;
			edit_line->v2->y = 0.0f;
			cam_ip_line.v1->y = 0.0f;
			cam_ip_line.v2->y = 0.0f;

			a = edit_line->get_length();
			b = cam_ip_line.get_length();

			a_vec = &edit_line->get_vector();
			b_vec = &cam_ip_line.get_vector();

			angle = c->rad_to_deg(acosf((a_vec->x * b_vec->x + a_vec->y * b_vec->y + a_vec->z * b_vec->z) / (a*b)));

			max_angle = c->rad_to_deg(asinf(radius / a));

			strengthx = ((max_angle - angle) / max_angle)*2.0f;
			strengthy = diry*(angle / max_angle)*2.0f;

			sign_y = sign_y*(fabs(cam_ip_line.get_direction().z) > fabs(edit_line->get_direction().z) ? -1.0f : 1.0f);
			rot->y += -x_size*(float)x*strengthx;
			rot->y += y_size*(float)y*strengthy*sign_x*sign_y*sign_z;
		}
		if(ctrl_obj_selected[2] && (!rotate_toggle[0] || rotate_toggle[3])) {
			dirz = fabs(edit_line->get_direction().z);
			diry = fabs(sinf(c->deg_to_rad(cam->get_rotation()->y)));

			sign_view_y_90_270 = cam->get_rotation()->y > 90.0f && cam->get_rotation()->y < 270.0f ? -1.0f : 1.0f;
			sign_view_y_0_180 = cam->get_rotation()->y > 0.0f && cam->get_rotation()->y < 180.0f ? -1.0f : 1.0f;

			// strength = [strength with dirz = 0° OR 180°] + [strength with dirz = 90° OR 270°]
			strengthx = ((((intersection_point->y - pos->y) / radius) * dirz) + (1.0f - dirz) * (1.0f - diry) * -sign_y) * -sign_view_y_90_270;
			strengthy = ((((radius - fabs(intersection_point->y - pos->y)) / radius) * dirz) + (1.0f - dirz) * diry) * sign_view_y_0_180;

			strengthx = x_size*(float)x*-strengthx;
			strengthy = y_size*(float)y*-strengthy;

			rot->z += strengthx;
			rot->z += strengthy;
		}

		if(rot->x > 360.0f) rot->x -= 360.f;
		if(rot->x < 0.0f) rot->x += 360.f;

		if(rot->y > 360.0f) rot->y -= 360.f;
		if(rot->y < 0.0f) rot->y += 360.f;

		if(rot->z > 360.0f) rot->z -= 360.f;
		if(rot->z < 0.0f) rot->z += 360.f;

		// update models local model view matrix
		sel_mobject->type ? sel_mobject->amodel->update_mview_matrix() : sel_mobject->model->update_mview_matrix();
	}
	else if(edit_state == mapeditor::SCALE) {
		float sclx, scly, sclz;
		sclx = scl->x;
		scly = scl->y;
		sclz = scl->z;
		if(ctrl_obj_selected[0] && (!scale_toggle[0] || scale_toggle[1])) {
			strengthx = fabs(fabs(dir->x) - 1.0f)*-sign_z;
			strengthy = fabs(fabs(dir->z) - 1.0f)*-sign_x;
			scl->x += x_size*(float)x*strengthx * 0.1f;
			scl->x += y_size*(float)y*strengthy * 0.1f;
		}
		if(ctrl_obj_selected[1] && (!scale_toggle[0] || scale_toggle[2])) {
			scl->y += y_size*(float)y*-1.0f * 0.1f;
		}
		if(ctrl_obj_selected[2] && (!scale_toggle[0] || scale_toggle[3])) {
			strengthx = fabs(fabs(dir->z) - 1.0f)*sign_x * 0.1f;
			strengthy = fabs(fabs(dir->x) - 1.0f)*-sign_z * 0.1f;
			scl->z += x_size*(float)x*strengthx;
			scl->z += y_size*(float)y*strengthy;
		}

		if(scl->x < 0.0f) scl->x = sclx;
		if(scl->y < 0.0f) scl->y = scly;
		if(scl->z < 0.0f) scl->z = sclz;

		// update models scale matrix
		sel_mobject->type ? sel_mobject->amodel->update_scale_matrix() : sel_mobject->model->update_scale_matrix();
	}
	else if(edit_state == mapeditor::PHYS_SCALE) {
		float sclx, scly, sclz;
		sclx = phys_scl->x;
		scly = phys_scl->y;
		sclz = phys_scl->z;
		if(ctrl_obj_selected[0] && (!phys_scale_toggle[0] || phys_scale_toggle[1])) {
			strengthx = fabs(fabs(dir->x) - 1.0f)*-sign_z;
			strengthy = fabs(fabs(dir->z) - 1.0f)*-sign_x;
			phys_scl->x += x_size*(float)x*strengthx * 0.1f;
			phys_scl->x += y_size*(float)y*strengthy * 0.1f;
		}
		if(ctrl_obj_selected[1] && (!phys_scale_toggle[0] || phys_scale_toggle[2])) {
			phys_scl->y += y_size*(float)y*-1.0f * 0.1f;
		}
		if(ctrl_obj_selected[2] && (!phys_scale_toggle[0] || phys_scale_toggle[3])) {
			strengthx = fabs(fabs(dir->z) - 1.0f)*sign_x * 0.1f;
			strengthy = fabs(fabs(dir->x) - 1.0f)*-sign_z * 0.1f;
			phys_scl->z += x_size*(float)x*strengthx;
			phys_scl->z += y_size*(float)y*strengthy;
		}

		if(phys_scl->x < 0.0f) phys_scl->x = sclx;
		if(phys_scl->y < 0.0f) phys_scl->y = scly;
		if(phys_scl->z < 0.0f) phys_scl->z = sclz;
	}

	reload_cur_bbox();
	new_sel = true;
}

void mapeditor::set_draw_mode(bool wireframe) {
	draw_mode = wireframe;
	for(list<map_object>::iterator obj_iter = objects.begin(); obj_iter != objects.end(); obj_iter++) {
		obj_iter->type ? obj_iter->amodel->set_draw_wireframe(wireframe) : obj_iter->model->set_draw_wireframe(wireframe);
	}
}

bool mapeditor::get_draw_mode() {
	return mapeditor::draw_mode;
}

void mapeditor::set_objects_visibility(bool state) {
	objects_visibility = state;
	for(list<map_object>::iterator obj_iter = objects.begin(); obj_iter != objects.end(); obj_iter++) {
		obj_iter->type ? obj_iter->amodel->set_visible(objects_visibility) : obj_iter->model->set_visible(objects_visibility);
	}
}

bool mapeditor::get_objects_visibility() {
	return mapeditor::objects_visibility;
}

void mapeditor::set_physical_map(bool state) {
	physical_map = state;
	for(list<map_object>::iterator obj_iter = objects.begin(); obj_iter != objects.end(); obj_iter++) {
		//obj_iter->type ? NULL : obj_iter->model->set_draw_phys_obj(physical_map);
		if(!obj_iter->type) obj_iter->model->set_draw_phys_obj(physical_map);
	}
}

bool mapeditor::get_physical_map() {
	return mapeditor::physical_map;
}

bool* mapeditor::get_move_toggle() {
	return mapeditor::move_toggle;
}

bool* mapeditor::get_rotate_toggle() {
	return mapeditor::rotate_toggle;
}

bool* mapeditor::get_scale_toggle() {
	return mapeditor::scale_toggle;
}

bool* mapeditor::get_phys_scale_toggle() {
	return mapeditor::phys_scale_toggle;
}
