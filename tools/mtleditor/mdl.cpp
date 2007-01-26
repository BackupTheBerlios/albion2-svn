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

#include "mdl.h"

mdl::mdl(engine* e, scene* sce, camera* cam) {
	mdl::e = e;
	mdl::sce = sce;
	mdl::m = e->get_msg();
	mdl::f = e->get_file_io();
	mdl::cam = cam;
	mdl::agfx = e->get_gfx();

	mdl::model = NULL;
	mdl::amodel = NULL;
	mdl::mat = NULL;
	mdl::ani = false;

	mdl::mdl_ld = false;

	mdl::updated = false;

	mdl::ud_alpha_start = 0.5f;
	mdl::ud_timer_out = 500;
	mdl::ud_timer2_out = 25;
}

mdl::~mdl() {
	if(mdl::model != NULL) {
		sce->delete_model(model);
		delete model;
	}
	if(mdl::amodel != NULL) {
		sce->delete_model(amodel);
		delete amodel;
	}
	if(mdl::mat != NULL) {
		delete mat;
	}
}

void mdl::close_model() {
	if(model != NULL) {
		sce->delete_model(model);
		delete model;
		model = NULL;
	}
	else if(amodel != NULL) {
		sce->delete_model(amodel);
		delete amodel;
		amodel = NULL;
	}

	if(mdl::mat != NULL) {
		delete mat;
	}

	mdl::mat = NULL;
	mdl::ani = false;
	mdl::mdl_ld = false;

	mat_fname = "";
	mdl_fname = "";
}

void mdl::load_model(char* filename, char* ani_filename) {
	if(model != NULL) {
		sce->delete_model(model);
		delete model;
		model = NULL;
	}
	else if(amodel != NULL) {
		sce->delete_model(amodel);
		delete amodel;
		amodel = NULL;
	}

	ani = e->get_core()->is_a2eanim(e->data_path(filename));

	if(!ani) {
		model = sce->create_a2emodel();
		model->load_model(e->data_path(filename), true);
		sce->add_model(model);
	}
	else {
		amodel = sce->create_a2eanim();
		amodel->load_model(e->data_path(filename), true);
		amodel->add_animation(e->data_path(ani_filename));
		amodel->play_frames(1, 1);
		sce->add_model(amodel);
	}

	mdl::mdl_ld = true;

	if(!ani) vertices = model->get_vertices();

	// reset cam
	core::aabbox* bbox = ani ? amodel->get_bounding_box() : model->get_bounding_box();
	vertex3* p = ani ? amodel->get_position() : model->get_position();
	vertex3* et = new vertex3(bbox->vmax - bbox->vmin);

	cam->set_position(-(p->x + et->x), (p->y + et->y), -(p->z + et->z));
	cam->set_rotation(0.0f, 45.0f, 0.0f);

	float max = et->x;
	if(max < et->y) max = et->y;
	if(max < et->z) max = et->z;
	cam->set_cam_speed(max / 100.0f);

	delete et;
	
	mdl_fname = filename;
}

void mdl::set_material(char* filename) {
	if(mat != NULL) {
		delete mat;
	}

	mat = new a2ematerial(e);
	mat->load_material(e->data_path(filename));

	if(!ani) {
		model->set_material(mat);
	}
	else {
		amodel->set_material(mat);
	}

	mat_fname = filename;
}

a2ematerial* mdl::get_material() {
	return mdl::mat;
}

void mdl::save_material(const char* filename) {
	mdl::f->open_file(e->data_path(filename), file_io::OT_WRITE_BINARY);

	mdl::f->write_block("A2EMATERIAL", 11);

	unsigned int object_count = ani ? amodel->get_object_count() : model->get_object_count();
	mdl::f->put_uint(object_count);
	for(unsigned int i = 0; i < object_count; i++) {
		char mtype = mat->get_material_type(i);
		mdl::f->put_char(mtype);

		a2ematerial::texture_elem* te = mat->get_tex_elem(i);
		unsigned int tex_count = mat->get_texture_count(i);

		if(mtype == 0x04) {
			mdl::f->put_uint(tex_count);

			for(unsigned int j = 0; j < tex_count; j++) {
				mdl::f->put_char(mat->get_color_type(i, j));
				mdl::f->put_char(te->rgb_combine[j]);
				mdl::f->put_char(te->alpha_combine[j]);

				unsigned int argc_rgb = 0;
				unsigned int argc_alpha = 0;
				switch(te->rgb_combine[j]) {
					case 0x00:
						argc_rgb = 1;
						break;
					case 0x01:
					case 0x02:
					case 0x03:
					case 0x04:
						argc_rgb = 2;
						break;
					case 0x05:
						argc_rgb = 3;
						break;
					case 0xFF:
						argc_rgb = 0;
						break;
				}

				switch(te->alpha_combine[j]) {
					case 0x00:
						argc_alpha = 1;
						break;
					case 0x01:
					case 0x02:
					case 0x03:
					case 0x04:
						argc_alpha = 2;
						break;
					case 0x05:
						argc_alpha = 3;
						break;
					case 0xFF:
						argc_alpha = 0;
						break;
				}

				for(unsigned int k = 0; k < argc_rgb; k++) {
					mdl::f->put_char(te->rgb_source[j][k]);
					mdl::f->put_char(te->rgb_operand[j][k]);
				}

				for(unsigned int k = 0; k < argc_alpha; k++) {
					mdl::f->put_char(te->alpha_source[j][k]);
					mdl::f->put_char(te->alpha_operand[j][k]);
				}
			}
		}
		else {
			for(unsigned int j = 0; j < tex_count; j++) {
				mdl::f->put_char(mat->get_color_type(i, j));
			}
		}
		for(unsigned int j = 0; j < tex_count; j++) {
			mdl::f->write_block((char*)te->tex_names[j].c_str(), (unsigned int)te->tex_names[j].size());
			mdl::f->put_char('ÿ'); // 255/0xFF ... C4309?
		}
	}

	mdl::f->close_file();
}

bool mdl::is_model() {
	return mdl::mdl_ld;
}

void mdl::update_selection(unsigned int obj_num) {
	if(!ani) {
		indices = model->get_indices(obj_num);
		index_count = model->get_index_count(obj_num);
	}
	else {
		vertices = amodel->get_vertices(obj_num);
		indices = amodel->get_indices(obj_num);
		index_count = amodel->get_index_count(obj_num);
	}

	updated = true;
	ud_alpha = ud_alpha_start;
	ud_timer = SDL_GetTicks();
	ud_timer2 = SDL_GetTicks();
}

void mdl::draw_selection(bool wireframe) {
	vertex3* pos = ani ? amodel->get_position() : model->get_position();

	glPushMatrix();
	glTranslatef(pos->x, pos->y, pos->z);

	if(wireframe) {
		glDisable(GL_LIGHTING);
		glDisable(GL_TEXTURE_2D);

		// draw wireframe
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		glEnable(GL_POLYGON_OFFSET_LINE); // use polygon offset to "disable" z-fighting
		glPolygonOffset(-1.0f, 0.0f);

		glVertexPointer(3, GL_FLOAT, 0, vertices);
		glEnableClientState(GL_VERTEX_ARRAY);

		glColor3f(1.0f, 1.0f, 1.0f);
		glDrawElements(GL_TRIANGLES, index_count * 3, GL_UNSIGNED_INT, indices);

		glDisableClientState(GL_VERTEX_ARRAY);

		glDisable(GL_POLYGON_OFFSET_LINE);

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	// draw white fade effect
	if(updated) {
		glEnable(GL_BLEND);

		glEnable(GL_POLYGON_OFFSET_FILL); // use polygon offset to "disable" z-fighting
		glPolygonOffset(-1.0f, 0.0f);

		glVertexPointer(3, GL_FLOAT, 0, vertices);
		glEnableClientState(GL_VERTEX_ARRAY);

		glColor4f(1.0f, 1.0f, 1.0f, ud_alpha);
		glDrawElements(GL_TRIANGLES, index_count * 3, GL_UNSIGNED_INT, indices);

		glDisableClientState(GL_VERTEX_ARRAY);

		glDisable(GL_POLYGON_OFFSET_FILL);
		glDisable(GL_BLEND);

		if(SDL_GetTicks() - ud_timer > ud_timer_out) {
			updated = false;
		}

		if(SDL_GetTicks() - ud_timer2 > ud_timer2_out) {
			ud_alpha -= ud_alpha_start / (float)(ud_timer_out / ud_timer2_out);
			ud_timer2 = SDL_GetTicks();
		}
	}

	glPopMatrix();
}
