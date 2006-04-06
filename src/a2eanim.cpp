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

#include "a2eanim.h"

core* a2eanim::c = NULL;
msg* a2eanim::m = NULL;
ext* a2eanim::exts = NULL;
vertex3** a2eanim::mt_normals = NULL;
vertex3** a2eanim::mt_binormals = NULL;
vertex3** a2eanim::mt_tangents = NULL;
unsigned int a2eanim::mt_cur_mesh = 0;
unsigned int a2eanim::mt_cur_frame = 0;
unsigned int* a2eanim::mt_start_num = NULL;
unsigned int* a2eanim::mt_end_num = NULL;
a2eanim::mesh* a2eanim::mt_mesh = NULL;
a2eanim::nlist* a2eanim::mt_normal_list = NULL;
bool* a2eanim::mt_thread_done = NULL;
bool* a2eanim::mt_thread_done2 = NULL;
unsigned int a2eanim::mt_cur_tn = 0;

/*! there is no function currently
 */
a2eanim::a2eanim(engine* e, shader* s) {
	a2eanim::position = new vertex3();
	a2eanim::position->x = 0.0f;
	a2eanim::position->y = 0.0f;
	a2eanim::position->z = 0.0f;

	a2eanim::scale = new vertex3();
	a2eanim::scale->x = 1.0f;
	a2eanim::scale->y = 1.0f;
	a2eanim::scale->z = 1.0f;

	a2eanim::rotation = new vertex3();
	a2eanim::rotation->x = 0.0f;
	a2eanim::rotation->y = 0.0f;
	a2eanim::rotation->z = 0.0f;

	joint_count = 0;
	base_joint_count = 0;
	mesh_count = 0;
	canimations = 0;
	current_animation = 0;
	current_frame = 0;
	last_frame = 0;
	timer = SDL_GetTicks();

	joints = NULL;
	meshes = NULL;
	animations = NULL;
	base_joints = NULL;

	is_material = false;
	material = NULL;

	is_draw_joints = false;

	start_frame = 0;
	end_frame = 0;

	is_visible = true;

	angle0 = 0.0f;

	normal_list = NULL;

	bbox = new core::aabbox();

	init = false;
	vbo = false;

	// get classes
	a2eanim::e = e;
	a2eanim::s = s;
	a2eanim::c = e->get_core();
	a2eanim::m = e->get_msg();
	a2eanim::file = e->get_file_io();
	a2eanim::exts = e->get_ext();


	// thread count - 2 additional threads atm
	thread_count = e->get_thread_count();
}

/*! there is no function currently
 */
a2eanim::~a2eanim() {
	m->print(msg::MDEBUG, "a2eanim.cpp", "freeing a2eanim stuff");

	if(a2eanim::joints != NULL) {
		delete [] a2eanim::joints;
	}

	if(a2eanim::meshes != NULL) {
		delete [] a2eanim::meshes;
	}

	if(a2eanim::base_joints != NULL) {
		delete [] a2eanim::base_joints;
	}

	if(a2eanim::animations != NULL) {
		delete [] a2eanim::animations;
	}

	delete a2eanim::position;
	delete a2eanim::scale;
	delete a2eanim::rotation;

	if(a2eanim::normal_list != NULL) {
		delete [] a2eanim::normal_list;
	}

	delete bbox;

	// delete vbos
	if(vbo) {
		for(unsigned int i = 0; i < a2eanim::mesh_count; i++) {
			if(exts->glIsBufferARB(a2eanim::meshes[i].vbo_vertices_id)) exts->glDeleteBuffersARB(1, &a2eanim::meshes[i].vbo_vertices_id);
			if(exts->glIsBufferARB(a2eanim::meshes[i].vbo_tex_coords_id)) exts->glDeleteBuffersARB(1, &a2eanim::meshes[i].vbo_tex_coords_id);
			if(exts->glIsBufferARB(a2eanim::meshes[i].vbo_normals_id)) exts->glDeleteBuffersARB(1, &a2eanim::meshes[i].vbo_normals_id);
			if(exts->glIsBufferARB(a2eanim::meshes[i].vbo_binormals_id)) exts->glDeleteBuffersARB(1, &a2eanim::meshes[i].vbo_binormals_id);
			if(exts->glIsBufferARB(a2eanim::meshes[i].vbo_tangents_id)) exts->glDeleteBuffersARB(1, &a2eanim::meshes[i].vbo_tangents_id);
			if(exts->glIsBufferARB(a2eanim::meshes[i].vbo_indices_ids)) exts->glDeleteBuffersARB(1, &a2eanim::meshes[i].vbo_indices_ids);
		}
	}

	m->print(msg::MDEBUG, "a2eanim.cpp", "a2eanim stuff freed");
}

/*! draws the model
 */
void a2eanim::draw() {
	if(a2eanim::is_visible && e->get_init_mode() == engine::GRAPHICAL) {
		// frame stuff
		if(a2eanim::current_frame < a2eanim::end_frame &&
			SDL_GetTicks() - a2eanim::timer > a2eanim::current_frame * a2eanim::animations[current_animation]->frame_time) {
			a2eanim::last_frame = a2eanim::current_frame;
			a2eanim::current_frame = (unsigned int)((float)(SDL_GetTicks() - a2eanim::timer) /
				a2eanim::animations[current_animation]->frame_time);
		}

		if(a2eanim::current_frame >= a2eanim::end_frame) {
			a2eanim::current_frame = a2eanim::start_frame;
			a2eanim::timer = SDL_GetTicks();
		}

		// build the bones
		for(unsigned int i = 0; i < a2eanim::base_joint_count; i++) {
			a2eanim::build_bone(a2eanim::current_frame, base_joints[i], &base_joints[i]->position, &base_joints[i]->orientation);
		}

		// skin the mesh
		a2eanim::skin_mesh();

		// ... and here we go finally - render the model :)
		glPushMatrix();
		glTranslatef(a2eanim::position->x, a2eanim::position->y, a2eanim::position->z);

		// rotate the model
		glRotatef(360.0f - a2eanim::rotation->x, 1.0f, 0.0f , 0.0f);
		glRotatef(360.0f - a2eanim::rotation->y, 0.0f, 1.0f , 0.0f);
		glRotatef(360.0f - a2eanim::rotation->z, 0.0f, 0.0f , 1.0f);

		// scale the model
		glScalef(a2eanim::scale->x, a2eanim::scale->y, a2eanim::scale->z);

		if(!init) {
			// generate the normals, binormals and tangents
			unsigned int t1 = SDL_GetTicks();
			thread_count == 0 ? generate_normals_nt() : generate_normals();
			unsigned int t2 = SDL_GetTicks() - t1;
			m->print(msg::MDEBUG, "a2eanim.cpp", "n/nbt computing time: %u ms", t2);

			// create buffers
			if(vbo) {
				for(unsigned int i = 0; i < a2eanim::mesh_count; i++) {
					// vertices vbo
					exts->glGenBuffersARB(1, &a2eanim::meshes[i].vbo_vertices_id);
					exts->glBindBufferARB(GL_ARRAY_BUFFER_ARB, a2eanim::meshes[i].vbo_vertices_id);
					exts->glBufferDataARB(GL_ARRAY_BUFFER_ARB, a2eanim::meshes[i].vertex_count * 3 * sizeof(float),
						a2eanim::meshes[i].vertices, GL_DYNAMIC_DRAW_ARB);

					// normals vbo
					exts->glGenBuffersARB(1, &a2eanim::meshes[i].vbo_normals_id);
					exts->glBindBufferARB(GL_ARRAY_BUFFER_ARB, a2eanim::meshes[i].vbo_normals_id);
					exts->glBufferDataARB(GL_ARRAY_BUFFER_ARB, a2eanim::meshes[i].vertex_count * 3 * sizeof(float),
						a2eanim::meshes[i].normal[a2eanim::current_frame], GL_DYNAMIC_DRAW_ARB);

					if(exts->is_shader_support()) {
						// binormals vbo
						exts->glGenBuffersARB(1, &a2eanim::meshes[i].vbo_binormals_id);
						exts->glBindBufferARB(GL_ARRAY_BUFFER_ARB, a2eanim::meshes[i].vbo_binormals_id);
						exts->glBufferDataARB(GL_ARRAY_BUFFER_ARB, a2eanim::meshes[i].vertex_count * 3 * sizeof(float),
							a2eanim::meshes[i].binormal[a2eanim::current_frame], GL_DYNAMIC_DRAW_ARB);

						// tangents vbo
						exts->glGenBuffersARB(1, &a2eanim::meshes[i].vbo_tangents_id);
						exts->glBindBufferARB(GL_ARRAY_BUFFER_ARB, a2eanim::meshes[i].vbo_tangents_id);
						exts->glBufferDataARB(GL_ARRAY_BUFFER_ARB, a2eanim::meshes[i].vertex_count * 3 * sizeof(float),
							a2eanim::meshes[i].tangent[a2eanim::current_frame], GL_DYNAMIC_DRAW_ARB);
					}
				}
			}

			init = true;
		}

		if(last_frame != current_frame && vbo) {
			for(unsigned int i = 0; i < a2eanim::mesh_count; i++) {
				// vertices vbo
				exts->glBindBufferARB(GL_ARRAY_BUFFER_ARB, a2eanim::meshes[i].vbo_vertices_id);
				exts->glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, 0, a2eanim::meshes[i].vertex_count * 3 * sizeof(float),
					a2eanim::meshes[i].vertices);

				// normals vbo
				exts->glBindBufferARB(GL_ARRAY_BUFFER_ARB, a2eanim::meshes[i].vbo_normals_id);
				exts->glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, 0, a2eanim::meshes[i].vertex_count * 3 * sizeof(float),
					a2eanim::meshes[i].normal[a2eanim::current_frame]);

				if(exts->is_shader_support()) {
					// binormals vbo
					exts->glBindBufferARB(GL_ARRAY_BUFFER_ARB, a2eanim::meshes[i].vbo_binormals_id);
					exts->glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, 0, a2eanim::meshes[i].vertex_count * 3 * sizeof(float),
						a2eanim::meshes[i].binormal[a2eanim::current_frame]);

					// tangents vbo
					exts->glBindBufferARB(GL_ARRAY_BUFFER_ARB, a2eanim::meshes[i].vbo_tangents_id);
					exts->glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, 0, a2eanim::meshes[i].vertex_count * 3 * sizeof(float),
						a2eanim::meshes[i].tangent[a2eanim::current_frame]);
				}
			}
		}

		for(unsigned int i = 0; i < a2eanim::mesh_count; i++) {
			if(exts->is_shader_support() && a2eanim::material->get_material_type(i) == a2ematerial::PARALLAX) {
				s->use_shader(1);

				s->set_uniform3f(0, -e->get_position()->x, -e->get_position()->y, -e->get_position()->z);
				s->set_uniform3f(1, light_position->x, light_position->y, light_position->z);

				s->set_uniform1i(2, 0);
				s->set_uniform1i(3, 1);
				s->set_uniform1i(4, 2);
				s->set_uniform4f(5, light_color[0], light_color[1], light_color[2], light_color[3]);

				exts->glActiveTextureARB(GL_TEXTURE0_ARB);
				glBindTexture(GL_TEXTURE_2D, a2eanim::material->get_texture(i, 0));
				glEnable(GL_TEXTURE_2D);
				exts->glActiveTextureARB(GL_TEXTURE1_ARB);
				glBindTexture(GL_TEXTURE_2D, a2eanim::material->get_texture(i, 1));
				glEnable(GL_TEXTURE_2D);
				exts->glActiveTextureARB(GL_TEXTURE2_ARB);
				glBindTexture(GL_TEXTURE_2D, a2eanim::material->get_texture(i, 2));
				glEnable(GL_TEXTURE_2D);

				if(a2eanim::material->get_color_type(i, 0) == 0x01) { glEnable(GL_BLEND); }

				if(vbo) {
					exts->glBindBufferARB(GL_ARRAY_BUFFER_ARB, a2eanim::meshes[i].vbo_vertices_id);
					glVertexPointer(3, GL_FLOAT, 0, NULL);

					exts->glBindBufferARB(GL_ARRAY_BUFFER_ARB, a2eanim::meshes[i].vbo_normals_id);
					exts->glEnableVertexAttribArrayARB(s->get_shader_object(1)->attributes[0]);
					exts->glVertexAttribPointerARB(s->get_shader_object(1)->attributes[0], 3, GL_FLOAT, GL_FALSE, 0, NULL);

					exts->glBindBufferARB(GL_ARRAY_BUFFER_ARB, a2eanim::meshes[i].vbo_binormals_id);
					exts->glEnableVertexAttribArrayARB(s->get_shader_object(1)->attributes[1]);
					exts->glVertexAttribPointerARB(s->get_shader_object(1)->attributes[1], 3, GL_FLOAT, GL_FALSE, 0, NULL);
					
					exts->glBindBufferARB(GL_ARRAY_BUFFER_ARB, a2eanim::meshes[i].vbo_tangents_id);
					exts->glEnableVertexAttribArrayARB(s->get_shader_object(1)->attributes[2]);
					exts->glVertexAttribPointerARB(s->get_shader_object(1)->attributes[2], 3, GL_FLOAT, GL_FALSE, 0, NULL);

					exts->glBindBufferARB(GL_ARRAY_BUFFER_ARB, a2eanim::meshes[i].vbo_tex_coords_id);
					exts->glEnableVertexAttribArrayARB(s->get_shader_object(1)->attributes[3]);
					exts->glVertexAttribPointerARB(s->get_shader_object(1)->attributes[3], 2, GL_FLOAT, GL_FALSE, 0, NULL);

					glEnableClientState(GL_VERTEX_ARRAY);

					exts->glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, a2eanim::meshes[i].vbo_indices_ids);
					glFrontFace(GL_CW);
					glDrawElements(GL_TRIANGLES, a2eanim::meshes[i].triangle_count * 3, GL_UNSIGNED_INT, NULL);
					glFrontFace(GL_CCW);

					exts->glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
					exts->glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);
				}
				else {
					glEnableClientState(GL_VERTEX_ARRAY);
					glVertexPointer(3, GL_FLOAT, 0, a2eanim::meshes[i].vertices);

					exts->glEnableVertexAttribArrayARB(s->get_shader_object(1)->attributes[0]);
					exts->glVertexAttribPointerARB(s->get_shader_object(1)->attributes[0], 3, GL_FLOAT, GL_FALSE, 0, a2eanim::meshes[i].normal[a2eanim::current_frame]);
					exts->glEnableVertexAttribArrayARB(s->get_shader_object(1)->attributes[1]);
					exts->glVertexAttribPointerARB(s->get_shader_object(1)->attributes[1], 3, GL_FLOAT, GL_FALSE, 0, a2eanim::meshes[i].binormal[a2eanim::current_frame]);
					exts->glEnableVertexAttribArrayARB(s->get_shader_object(1)->attributes[2]);
					exts->glVertexAttribPointerARB(s->get_shader_object(1)->attributes[2], 3, GL_FLOAT, GL_FALSE, 0, a2eanim::meshes[i].tangent[a2eanim::current_frame]);
					exts->glEnableVertexAttribArrayARB(s->get_shader_object(1)->attributes[3]);
					exts->glVertexAttribPointerARB(s->get_shader_object(1)->attributes[3], 2, GL_FLOAT, GL_FALSE, 0, a2eanim::meshes[i].tex_coords);
					
					glEnableClientState(GL_VERTEX_ARRAY);

					glFrontFace(GL_CW);
					glDrawElements(GL_TRIANGLES, a2eanim::meshes[i].triangle_count * 3, GL_UNSIGNED_INT, a2eanim::meshes[i].indices);
					glFrontFace(GL_CCW);
				}

				if(a2eanim::material->get_color_type(i, 0) == 0x01) { glDisable(GL_BLEND); }

				exts->glActiveTextureARB(GL_TEXTURE2_ARB);
				glDisable(GL_TEXTURE_2D);
				exts->glActiveTextureARB(GL_TEXTURE1_ARB);
				glDisable(GL_TEXTURE_2D);
				exts->glActiveTextureARB(GL_TEXTURE0_ARB);
				glDisable(GL_TEXTURE_2D);
				s->use_shader(0);
			}
			else {
				if(a2eanim::is_material) {
					glBindTexture(GL_TEXTURE_2D, a2eanim::material->get_texture(i, 0));
				}

				glColor3f(1.0f, 1.0f, 1.0f);
				glEnable(GL_TEXTURE_2D);
				if(a2eanim::material->get_color_type(i, 0) == 0x01) { glEnable(GL_BLEND); }

				if(vbo) {
					exts->glBindBufferARB(GL_ARRAY_BUFFER_ARB, a2eanim::meshes[i].vbo_vertices_id);
					glVertexPointer(3, GL_FLOAT, 0, NULL);
					exts->glBindBufferARB(GL_ARRAY_BUFFER_ARB, a2eanim::meshes[i].vbo_normals_id);
					glNormalPointer(GL_FLOAT, 0, NULL);
					exts->glBindBufferARB(GL_ARRAY_BUFFER_ARB, a2eanim::meshes[i].vbo_tex_coords_id);
					glTexCoordPointer(2, GL_FLOAT, 0, NULL);

					glEnableClientState(GL_VERTEX_ARRAY);
					glEnableClientState(GL_NORMAL_ARRAY);
					glEnableClientState(GL_TEXTURE_COORD_ARRAY);

					exts->glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, a2eanim::meshes[i].vbo_indices_ids);
					glFrontFace(GL_CW);
					glDrawElements(GL_TRIANGLES, a2eanim::meshes[i].triangle_count * 3, GL_UNSIGNED_INT, NULL);
					glFrontFace(GL_CCW);

					exts->glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
					exts->glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);
				}
				else {
					glVertexPointer(3, GL_FLOAT, 0, a2eanim::meshes[i].vertices);
					glNormalPointer(GL_FLOAT, 0, a2eanim::meshes[i].normal[a2eanim::current_frame]);
					glTexCoordPointer(2, GL_FLOAT, 0, a2eanim::meshes[i].tex_coords);

					glEnableClientState(GL_VERTEX_ARRAY);
					glEnableClientState(GL_NORMAL_ARRAY);
					glEnableClientState(GL_TEXTURE_COORD_ARRAY);

					glFrontFace(GL_CW);
					glDrawElements(GL_TRIANGLES, a2eanim::meshes[i].triangle_count * 3, GL_UNSIGNED_INT, a2eanim::meshes[i].indices);
					glFrontFace(GL_CCW);
				}

				if(a2eanim::material->get_color_type(i, 0) == 0x01) { glDisable(GL_BLEND); }
				glDisable(GL_TEXTURE_2D);
			}
		}

		glPopMatrix();
	}

	// draw joints if flag is set
	if(a2eanim::is_draw_joints) {
		a2eanim::draw_joints();
	}
}

/*! draws the models joints
 */
void a2eanim::draw_joints() {
	for(unsigned int i = 0; i < (unsigned int)a2eanim::joint_count; i++) {
		if(a2eanim::joints[i].parent != NULL) {
			glPushMatrix();
			glTranslatef(0.0f, 0.0f, 0.0f);
			glDisable(GL_TEXTURE_2D);
			glDisable(GL_DEPTH_TEST);
			glPushAttrib(GL_LIGHTING_BIT);
			glDisable(GL_LIGHTING);

			// we have to switch the z and negative y coordinate here, b/c md5
			// models are rotated -90 along the x axis
			glColor3f(0.0f, 1.0f, 0.0f);
			glBegin(GL_LINES);
				glVertex3f(a2eanim::joints[i].parent->position.x,
					a2eanim::joints[i].parent->position.z,
					-a2eanim::joints[i].parent->position.y);
				glVertex3f(a2eanim::joints[i].position.x,
					a2eanim::joints[i].position.z,
					-a2eanim::joints[i].position.y);
			glEnd();

			glPopAttrib();
			glEnable(GL_DEPTH_TEST);
			glEnable(GL_TEXTURE_2D);
			glPopMatrix();
		}
	}
}

/*! loads a .a2m model file
 *  @param filename the name of the .a2m model file
 *  @param vbo flag that specifies if vertex buffer objects should be used
 */
void a2eanim::load_model(char* filename, bool vbo) {
	a2eanim::vbo = vbo;
	// if vertex buffer objects are not supported, set vbo to false
	if(!exts->is_vbo_support()) {
		a2eanim::vbo = false;
	}

	if(!file->open_file(filename, file_io::OT_READ_BINARY)) {
		m->print(msg::MERROR, "a2eanim.cpp", "load_model(): error opening model file!");
		return;
	}

	// get file type
	char* file_type = new char[9];
	file->get_block(file_type, 8);
	file_type[8] = 0;
	if(strcmp(file_type, "A2EMODEL") != 0) {
		m->print(msg::MERROR, "a2eanim.cpp", "load_model(): unknown file type: %s!", file_type);
		file->close_file();
		return;
	}
	delete [] file_type;

	// get model type and abort if it's not 0x01
	char model_type = file->get_char();
	if(model_type != 0x01) {
		m->print(msg::MERROR, "a2eanim.cpp", "load_model(): non supported model type: %u!", (unsigned int)(model_type & 0xFF));
		file->close_file();
		return;
	}

	// joint count
	a2eanim::joint_count = file->get_uint();
	// base joint count
	a2eanim::base_joint_count = file->get_uint();

	// get joint data
	a2eanim::joints = new joint[a2eanim::joint_count];
	for(unsigned int i = 0; i < a2eanim::joint_count; i++) {
		// joint number
		a2eanim::joints[i].num = i;
		// joint parent
		a2eanim::joints[i].parent_num = file->get_uint();
		// joint children count
		a2eanim::joints[i].children_count = 0;
		// joint translation
		a2eanim::joints[i].position.x = file->get_float();
		a2eanim::joints[i].position.y = file->get_float();
		a2eanim::joints[i].position.z = file->get_float();
		// joint orientation
		a2eanim::joints[i].orientation.r = file->get_float();
		a2eanim::joints[i].orientation.x = file->get_float();
		a2eanim::joints[i].orientation.y = file->get_float();
		a2eanim::joints[i].orientation.z = file->get_float();
	}

	// object/mesh count
	a2eanim::mesh_count = file->get_uint();

	// get mesh data
	a2eanim::meshes = new mesh[a2eanim::mesh_count];
	for(unsigned int i = 0; i < a2eanim::mesh_count; i++) {
		// vertex count
		a2eanim::meshes[i].vertex_count = file->get_uint();

		a2eanim::meshes[i].nbt_computed = false;

		// vertex data
		a2eanim::meshes[i].tex_coords = new core::coord[a2eanim::meshes[i].vertex_count];
		a2eanim::meshes[i].weight_indices = new unsigned int[a2eanim::meshes[i].vertex_count];
		a2eanim::meshes[i].weight_counts = new unsigned int[a2eanim::meshes[i].vertex_count];
		a2eanim::meshes[i].vertices = new vertex3[a2eanim::meshes[i].vertex_count];
		for(unsigned int j = 0; j < a2eanim::meshes[i].vertex_count; j++) {
			// uv coordinates
			a2eanim::meshes[i].tex_coords[j].u = file->get_float();
			a2eanim::meshes[i].tex_coords[j].v = file->get_float();
			// weight index
			a2eanim::meshes[i].weight_indices[j] = file->get_uint();
			// weight count
			a2eanim::meshes[i].weight_counts[j] = file->get_uint();
			// vertices
			a2eanim::meshes[i].vertices[j].x = 0.0f;
			a2eanim::meshes[i].vertices[j].y = 0.0f;
			a2eanim::meshes[i].vertices[j].z = 0.0f;
		}

		// triangle count
		a2eanim::meshes[i].triangle_count = file->get_uint();

		// triangle data
		a2eanim::meshes[i].indices = new core::index[a2eanim::meshes[i].triangle_count];
		for(unsigned int j = 0; j < a2eanim::meshes[i].triangle_count; j++) {
			// indices
			a2eanim::meshes[i].indices[j].i1 = file->get_uint();
			a2eanim::meshes[i].indices[j].i2 = file->get_uint();
			a2eanim::meshes[i].indices[j].i3 = file->get_uint();
		}

		// weight count
		a2eanim::meshes[i].weight_count = file->get_uint();

		// weight data
		a2eanim::meshes[i].bone_indices = new int[a2eanim::meshes[i].weight_count];
		a2eanim::meshes[i].weight_strenghts = new float[a2eanim::meshes[i].weight_count];
		a2eanim::meshes[i].weights = new vertex3[a2eanim::meshes[i].weight_count];
		for(unsigned int j = 0; j < a2eanim::meshes[i].weight_count; j++) {
			// bone index
			a2eanim::meshes[i].bone_indices[j] = file->get_int();
			// weight strength
			a2eanim::meshes[i].weight_strenghts[j] = file->get_float();
			// weight itself
			a2eanim::meshes[i].weights[j].x = file->get_float();
			a2eanim::meshes[i].weights[j].y = file->get_float();
			a2eanim::meshes[i].weights[j].z = file->get_float();
		}
	}

	file->close_file();

	// compute parent<->children connections
	a2eanim::base_joints = new joint*[a2eanim::base_joint_count];

	// parent stuff ...
	unsigned int cbj = 0;
	for(unsigned int i = 0; i < a2eanim::joint_count; i++) {
		if(a2eanim::joints[i].parent_num < 0) {
			a2eanim::base_joints[cbj] = &a2eanim::joints[i];
			a2eanim::base_joints[cbj]->parent = NULL;
			cbj++;
		}
		else {
			a2eanim::joints[i].parent = &a2eanim::joints[a2eanim::joints[i].parent_num];
			a2eanim::joints[i].parent->children_count++;
		}
	}

	// children stuff ...
	unsigned int* count = new unsigned int[a2eanim::joint_count];
	for(unsigned int i = 0; i < a2eanim::joint_count; i++) {
		a2eanim::joints[i].childrens = new joint*[a2eanim::joints[i].children_count];
		count[i] = 0;
	}

	for(unsigned int i = 0; i < a2eanim::joint_count; i++) {
		if(a2eanim::joints[i].parent) {
			a2eanim::joints[i].parent->childrens[count[a2eanim::joints[i].parent_num]] = &a2eanim::joints[i];
			count[a2eanim::joints[i].parent_num]++;
		}
	}

	a2eanim::generate_normal_list();

	if(vbo) {
		for(unsigned int i = 0; i < a2eanim::mesh_count; i++) {
			a2eanim::meshes[i].vbo_binormals_id = 0;
			a2eanim::meshes[i].vbo_indices_ids = 0;
			a2eanim::meshes[i].vbo_normals_id = 0;
			a2eanim::meshes[i].vbo_tangents_id = 0;
			a2eanim::meshes[i].vbo_tex_coords_id = 0;
			a2eanim::meshes[i].vbo_vertices_id = 0;

			// tex_coords vbo
			exts->glGenBuffersARB(1, &a2eanim::meshes[i].vbo_tex_coords_id);
			exts->glBindBufferARB(GL_ARRAY_BUFFER_ARB, a2eanim::meshes[i].vbo_tex_coords_id);
			exts->glBufferDataARB(GL_ARRAY_BUFFER_ARB, a2eanim::meshes[i].vertex_count * 2 * sizeof(float),
				a2eanim::meshes[i].tex_coords, GL_STATIC_DRAW_ARB);

			// indices vbos
			exts->glGenBuffersARB(1, &a2eanim::meshes[i].vbo_indices_ids);
			exts->glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, a2eanim::meshes[i].vbo_indices_ids);
			exts->glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB, a2eanim::meshes[i].triangle_count * 3 * sizeof(unsigned int),
				a2eanim::meshes[i].indices, GL_STATIC_DRAW_ARB);
		}
	}
}

/*! loads an .a2a animation file
 *  @param filename the name of the .a2a animation file
 */
void a2eanim::add_animation(char* filename) {
	// create new animation
	animation** anis = new animation*[a2eanim::canimations + 1];
	for(unsigned int i = 0; i < a2eanim::canimations; i++) {
		anis[i] = animations[i];
	}
	// delete old animation pointers
	if(animations) { delete [] animations; }


	// load animation file
	file->open_file(filename, file_io::OT_READ_BINARY);

	// create new animation
	animation* ani = new animation();

	// get file type
	char* file_type = new char[7];
	file->get_block(file_type, 7);
	delete [] file_type;

	// joint count
	ani->joint_count = file->get_uint();

	// get joint data
	ani->joint_parents = new int[ani->joint_count];
	ani->joint_flags = new unsigned int[ani->joint_count];
	ani->joint_start_indices = new unsigned int[ani->joint_count];
	ani->joint_base_translation = new vertex3[ani->joint_count];
	ani->joint_base_orientation = new quaternion[ani->joint_count];
	for(unsigned int i = 0; i < ani->joint_count; i++) {
		// joint parent
		ani->joint_parents[i] = file->get_int();
		// joint flags
		ani->joint_flags[i] = file->get_uint();
		// joint start index
		ani->joint_start_indices[i] = file->get_uint();
		// joint base translation
		ani->joint_base_translation[i].x = file->get_float();
		ani->joint_base_translation[i].y = file->get_float();
		ani->joint_base_translation[i].z = file->get_float();
		// joint base orientation
		ani->joint_base_orientation[i].r = file->get_float();
		ani->joint_base_orientation[i].x = file->get_float();
		ani->joint_base_orientation[i].y = file->get_float();
		ani->joint_base_orientation[i].z = file->get_float();
	}

	// frame count
	ani->frame_count = file->get_uint();
	// frame time
	ani->frame_time = file->get_float();
	// animated components count
	ani->animated_components_count = file->get_uint();

	// write frame data
	ani->frames = new float*[ani->frame_count];
	for(unsigned int i = 0; i < ani->frame_count; i++) {
		// animated components data
		ani->frames[i] = new float[ani->animated_components_count];
		for(unsigned int j = 0; j < ani->animated_components_count; j++) {
			ani->frames[i][j] = file->get_float();
		}
	}

	// allocate normal, binormal and tangent vertex3 pointer
	// and vertex3 memory for each frame and vertex
	for(unsigned int i = 0; i < a2eanim::mesh_count; i++) {
		a2eanim::meshes[i].normal = new vertex3*[ani->frame_count];
		// if there is no shader support, we don't even need to allocate memory for binormals and tangents
		if(exts->is_shader_support()) {
			a2eanim::meshes[i].binormal = new vertex3*[ani->frame_count];
			a2eanim::meshes[i].tangent = new vertex3*[ani->frame_count];
		}

		for(unsigned int j = 0; j < ani->frame_count; j++) {
			a2eanim::meshes[i].normal[j] = new vertex3[a2eanim::meshes[i].vertex_count];
			// if there is no shader support, we don't even need to allocate memory for binormals and tangents
			if(exts->is_shader_support()) {
				a2eanim::meshes[i].binormal[j] = new vertex3[a2eanim::meshes[i].vertex_count];
				a2eanim::meshes[i].tangent[j] = new vertex3[a2eanim::meshes[i].vertex_count];
			}
		}
	}

	// close animation file
	file->close_file();

	// set end_frame if this is the first animation
	if(a2eanim::canimations == 0) {
		end_frame = ani->frame_count;
	}

	// assign new animation
	anis[a2eanim::canimations] = ani;

	// increment animation count
	a2eanim::canimations++;

	// reassign animation pointers
	animations = anis;

	// (re-)build bounding box
	a2eanim::build_bounding_box();
}


/*! deletes all animations
 */
void a2eanim::delete_animations() {
	delete [] a2eanim::animations;
	a2eanim::canimations = 0;
}

/*! skins the mesh
 */
void a2eanim::skin_mesh() {
	vertex3 position;
	for(unsigned int i = 0; i < a2eanim::mesh_count; i++) {
		for(unsigned int j = 0; j < a2eanim::meshes[i].vertex_count; j++) {
			a2eanim::meshes[i].vertices[j].x = 0.0f;
			a2eanim::meshes[i].vertices[j].y = 0.0f;
			a2eanim::meshes[i].vertices[j].z = 0.0f;

			for(unsigned int k = 0; k < a2eanim::meshes[i].weight_counts[j]; k++) {
				int base_index = a2eanim::meshes[i].weight_indices[j];
				int bone_index = a2eanim::meshes[i].bone_indices[base_index + k];

				position = a2eanim::joints[bone_index].orientation.rotate(a2eanim::meshes[i].weights[base_index + k]);
				a2eanim::meshes[i].vertices[j] += (position + joints[bone_index].position) * a2eanim::meshes[i].weight_strenghts[base_index + k];
			}
			// we have to switch the z and negative y coordinate here, b/c md5
			// models are rotated -90 along the x axis
			float tmp = -a2eanim::meshes[i].vertices[j].y;
			a2eanim::meshes[i].vertices[j].y = a2eanim::meshes[i].vertices[j].z;
			a2eanim::meshes[i].vertices[j].z = tmp;
		}
	}
}

/*! builds a specific bone specified by the parameters/arguments
 *  @param frame use the bone information of the specified frame
 *  @param jnt the joint from which we want to create the bone
 *  @param parent_translation the parents joint/bone position/translation
 *  @param parent_oriantation the parents joint/bone rotation/orientation
 */
void a2eanim::build_bone(unsigned int frame, joint* jnt, vertex3* parent_translation, quaternion* parent_oriantation) {
	vertex3 translation = animations[current_animation]->joint_base_translation[jnt->num];
	quaternion orientation = animations[current_animation]->joint_base_orientation[jnt->num];

	unsigned int flags = animations[current_animation]->joint_flags[jnt->num];
	unsigned int start_index = animations[current_animation]->joint_start_indices[jnt->num];
	unsigned int n = 0;

	// get animated translation and orientation stuff (if specific flag is set)
	if(flags & 1) { translation.x = animations[current_animation]->frames[frame][start_index + n]; n++; };
	if(flags & 2) { translation.y = animations[current_animation]->frames[frame][start_index + n]; n++; };
	if(flags & 4) { translation.z = animations[current_animation]->frames[frame][start_index + n]; n++; };
	if(flags & 8) { orientation.x = animations[current_animation]->frames[frame][start_index + n]; n++; };
	if(flags & 16) { orientation.y = animations[current_animation]->frames[frame][start_index + n]; n++; };
	if(flags & 32) { orientation.z = animations[current_animation]->frames[frame][start_index + n]; n++; };

	orientation.compute_r();

	// now assign the new computed translation and orientation values
	if(jnt->parent_num < 0) {
		jnt->position = translation;
		jnt->orientation = orientation;
	}
	else {
		jnt->position = parent_oriantation->rotate(translation);
		jnt->position += *parent_translation;
		jnt->orientation = *parent_oriantation * orientation;
	}

	// build the bones of all childs
	for(unsigned int i = 0; i < jnt->children_count; i++) {
		a2eanim::build_bone(frame, jnt->childrens[i], &jnt->position, &jnt->orientation);
	}
}

/*! sets the models material
 *  @param material the material object that we want tu use
 */
void a2eanim::set_material(a2ematerial* material) {
	a2eanim::material = material;
	a2eanim::is_material = true;
}

/*! sets the bool if the models joints are drawn
 *  @param state the new state
 */
void a2eanim::set_draw_joints(bool state) {
	a2eanim::is_draw_joints = state;
}

/*! returns a true if the models joints are drawn
 */
bool a2eanim::get_draw_joints() {
	return a2eanim::is_draw_joints;
}

/*! returns the animation count
 */
unsigned int a2eanim::get_animation_count() {
	return a2eanim::canimations;
}

/*! returns the num(th) animation
 */
a2eanim::animation* a2eanim::get_animation(unsigned int num) {
	return a2eanim::animations[num];
}

/*! returns the current animation number
 */
unsigned int a2eanim::get_current_animation_number() {
	return a2eanim::current_animation;
}

/*! returns the current animation
 */
a2eanim::animation* a2eanim::get_current_animation() {
	return a2eanim::animations[a2eanim::current_animation];
}

/*! sets the current animation
 */
void a2eanim::set_current_animation(unsigned int num) {
	a2eanim::current_animation = num;
	a2eanim::current_frame = 0;
	a2eanim::timer = SDL_GetTicks();
}

/*! sets the range of frames that are "played back"
 */
void a2eanim::play_frames(unsigned start, unsigned int end) {
	if(start < 0 || start >= a2eanim::animations[current_animation]->frame_count) {
		m->print(msg::MDEBUG, "a2eanim.cpp", "play_frames(): start frame #%u doesn't exist!", start);
		a2eanim::start_frame = 0;
	}
	else {
		a2eanim::start_frame = start;
	}

	if(end < 0 || end > a2eanim::animations[current_animation]->frame_count) {
		m->print(msg::MDEBUG, "a2eanim.cpp", "play_frames(): end frame #%u doesn't exist!", end);
		a2eanim::end_frame = a2eanim::animations[current_animation]->frame_count;
	}
	else {
		a2eanim::end_frame = end;
	}
}

/*! returns the current frame
 */
unsigned int a2eanim::get_current_frame() {
	return a2eanim::current_frame;
}

/*! sets the current frame
 */
void a2eanim::set_current_frame(unsigned int num) {
	if(num > a2eanim::animations[current_animation]->frame_count || num < 0) {
		m->print(msg::MDEBUG, "a2eanim.cpp", "set_current_frame(): frame #%u doesn't exist!", num);
	}
	else {
		if(num > a2eanim::end_frame) {
			m->print(msg::MDEBUG, "a2eanim.cpp", "set_current_frame(): frame #%u is behind current play back range!", num);
		}
		else if(num < a2eanim::start_frame) {
			m->print(msg::MDEBUG, "a2eanim.cpp", "set_current_frame(): frame #%u is before current play back range!", num);
		}
		else {
			a2eanim::current_frame = num;
		}
	}
}

/*! sets the position of the model
 *  @param x the x coordinate
 *  @param y the y coordinate
 *  @param z the z coordinate
 */
void a2eanim::set_position(float x, float y, float z) {
	a2eanim::position->x = x;
	a2eanim::position->y = y;
	a2eanim::position->z = z;
}

/*! returns the position of the model
 */
vertex3* a2eanim::get_position() {
	return a2eanim::position;
}

/*! sets the scale of the model
 *  @param x the x scale
 *  @param y the y scale
 *  @param z the z scale
 */
void a2eanim::set_scale(float x, float y, float z) {
	a2eanim::scale->x = x;
	a2eanim::scale->y = y;
	a2eanim::scale->z = z;
}

/*! returns the scale of the model
 */
vertex3* a2eanim::get_scale() {
	return a2eanim::scale;
}

/*! sets the rotation of the model
 *  @param x the x rotation
 *  @param y the y rotation
 *  @param z the z rotation
 */
void a2eanim::set_rotation(float x, float y, float z) {
	a2eanim::rotation->x = x;
	a2eanim::rotation->y = y;
	a2eanim::rotation->z = z;
}

/*! returns the rotation of the model
 */
vertex3* a2eanim::get_rotation() {
	return a2eanim::rotation;
}

/*! sets the models visible flag
 *  @param state the new state
 */
void a2eanim::set_visible(bool state) {
	a2eanim::is_visible = state;
}

/*! returns a true if the model is visible
 */
bool a2eanim::get_visible() {
	return a2eanim::is_visible;
}

/*! generates the "normal list"
 */
void a2eanim::generate_normal_list() {
	// we need temp numbers to store the triangle numbers of each vertex
	// i assume there is no vertex that is part of more than 16 triangles ...
	unsigned int* tmp_num = new unsigned int[16];
	unsigned int x = 0;

	// reserve memory for the normal list
	normal_list = new nlist*[a2eanim::mesh_count];

	for(unsigned int i = 0; i < a2eanim::mesh_count; i++) {
		normal_list[i] = new nlist[a2eanim::meshes[i].vertex_count];
		for(unsigned int j = 0; j < a2eanim::meshes[i].vertex_count; j++) {
			normal_list[i][j].num = NULL;
			normal_list[i][j].count = 0;
			x = 0;
			for(unsigned int k = 0; k < a2eanim::meshes[i].triangle_count; k++) {
				if(j == a2eanim::meshes[i].indices[k].i1 ||
					j == a2eanim::meshes[i].indices[k].i2 ||
					j == a2eanim::meshes[i].indices[k].i3) {
					tmp_num[x] = k;
					x++;
				}
			}
			normal_list[i][j].count = x;
			if(x != 0) {
				normal_list[i][j].num = new unsigned int[normal_list[i][j].count];
				for(unsigned int l = 0; l < x; l++) {
					normal_list[i][j].num[l] = tmp_num[l];
				}
			}
		}
	}
	delete [] tmp_num;
}

/*! generates the normals, binormals and tangents of the current mesh - with threading
 */
void a2eanim::generate_normals() {
	mt_normals = new vertex3*[thread_count];
	mt_binormals = new vertex3*[thread_count];
	mt_tangents = new vertex3*[thread_count];

	SDL_mutex** mutexes = new SDL_mutex*[thread_count]; // mutexes, or mutices: that is the question ...
	SDL_Thread** threads = new SDL_Thread*[thread_count];
	mt_thread_done = new bool[thread_count];
	mt_thread_done2 = new bool[thread_count];
	mt_start_num = new unsigned int[thread_count];
	mt_end_num = new unsigned int[thread_count];

	mt_cur_tn = 0;
	for(unsigned int i = 0; i < thread_count; i++) {
		mt_normals[i] = new vertex3[16];
		mt_binormals[i] = new vertex3[16];
		mt_tangents[i] = new vertex3[16];

		mutexes[i] = SDL_CreateMutex();
		if(mutexes[i] == NULL) {
			m->print(msg::MERROR, "a2eanim.cpp", "generate_normals(): unable to create mutex (#%u): %s!", i, SDL_GetError());
		}

		mt_thread_done[i] = false;
		mt_thread_done2[i] = true;
		threads[i] = SDL_CreateThread(mt_nbt, mutexes[i]);
		if(threads[i] == NULL) {
			m->print(msg::MERROR, "a2eanim.cpp", "generate_normals(): unable to create thread (#%u): %s!", i, SDL_GetError());
		}
	}

	for(unsigned int i = 0; i < a2eanim::mesh_count; i++) {
		unsigned int std_range = a2eanim::meshes[i].vertex_count / thread_count;
		unsigned int remain = a2eanim::meshes[i].vertex_count - (std_range * thread_count);
		for(unsigned int j = 0; j < thread_count; j++) {
			mt_start_num[j] = 0;
			mt_end_num[j] = 0;

			if(j > 0)
				mt_start_num[j] += mt_end_num[j-1];

			mt_end_num[j] = mt_start_num[j] + std_range;
			if(j < remain)
				mt_end_num[j]++;

			//cout << "thread #" << j << " range: " << mt_start_num[j] << " - " << mt_end_num[j] << " : " << (mt_end_num[j] - mt_start_num[j]) << endl;
		}

		if(!a2eanim::meshes[i].nbt_computed) {
			for(unsigned int j = 0; j < a2eanim::animations[a2eanim::current_animation]->frame_count; j++) {
				//cout << "frame " << j << "/" << a2eanim::animations[a2eanim::current_animation]->frame_count << endl;
				// build the bones for this frame ...
				for(unsigned int k = 0; k < a2eanim::base_joint_count; k++) {
					a2eanim::build_bone(j, base_joints[k], &base_joints[k]->position, &base_joints[k]->orientation);
				}
				// ... and skin the mesh
				a2eanim::skin_mesh();


				mt_cur_mesh = i;
				mt_cur_frame = j;
				mt_mesh = &meshes[i];
				mt_normal_list = normal_list[i];


				for(unsigned int k = 0; k < thread_count; k++) {
					SDL_mutexP(mutexes[k]);
					mt_thread_done2[k] = false;
					SDL_mutexV(mutexes[k]);
				}

				bool end = false;
				for(unsigned int k = 0; k < thread_count; k++) {
					while(!end) {
						if(SDL_mutexP(mutexes[k]) != 0) cout << "error locking mutex ..." << endl;
						if(mt_thread_done2[k]) {
							end = true;
						}
						if(SDL_mutexV(mutexes[k]) != 0) cout << "error unlocking mutex ..." << endl;
					}
					end = false;
				}
			}
		}
		a2eanim::meshes[i].nbt_computed = true;
	}

	for(unsigned int i = 0; i < thread_count; i++) {
		SDL_mutexP(mutexes[i]);
		mt_thread_done[i] = true;
		SDL_mutexV(mutexes[i]);

		if(threads[i] != NULL) { SDL_WaitThread(threads[i], NULL); }
		SDL_DestroyMutex(mutexes[i]);
	}

	delete [] threads;
	delete [] mutexes;

	delete [] mt_thread_done;
	delete [] mt_thread_done2;

	delete [] mt_start_num;
	delete [] mt_end_num;

	delete [] mt_normals;
	delete [] mt_binormals;
	delete [] mt_tangents;
}

/*! generates the normals, binormals and tangents of the current mesh - without threading
 */
void a2eanim::generate_normals_nt() {
	vertex3* normals = new vertex3[16];
	vertex3* binormals = new vertex3[16];
	vertex3* tangents = new vertex3[16];

	for(unsigned int i = 0; i < a2eanim::mesh_count; i++) {
		if(!a2eanim::meshes[i].nbt_computed) {
			for(unsigned int j = 0; j < a2eanim::animations[a2eanim::current_animation]->frame_count; j++) {
				// build the bones for this frame ...
				for(unsigned int k = 0; k < a2eanim::base_joint_count; k++) {
					a2eanim::build_bone(j, base_joints[k], &base_joints[k]->position, &base_joints[k]->orientation);
				}
				// ... and skin the mesh
				a2eanim::skin_mesh();

				for(unsigned int k = 0; k < a2eanim::meshes[i].vertex_count; k++) {
					// check if vertex is part of a triangle
					if(a2eanim::normal_list[i][k].count != 0) {
						// first of all: clear all vectors
						a2eanim::meshes[i].normal[j][k].x = 0.0f;
						a2eanim::meshes[i].normal[j][k].y = 0.0f;
						a2eanim::meshes[i].normal[j][k].z = 0.0f;
						// if there is no shader support, we don't need
						// to initialize the binormal and tangent vectors
						if(exts->is_shader_support()) {
							a2eanim::meshes[i].binormal[j][k].x = 0.0f;
							a2eanim::meshes[i].binormal[j][k].y = 0.0f;
							a2eanim::meshes[i].binormal[j][k].z = 0.0f;
							a2eanim::meshes[i].tangent[j][k].x = 0.0f;
							a2eanim::meshes[i].tangent[j][k].y = 0.0f;
							a2eanim::meshes[i].tangent[j][k].z = 0.0f;
						}
						// compute the normals, binormals and tangents for all triangles the vertex is part of
						for(unsigned int l = 0; l < a2eanim::normal_list[i][k].count; l++) {
							// if there is no shader support, we don't need
							// to compute the binormal and tangent vectors
							if(exts->is_shader_support()) {
								c->compute_normal_tangent_binormal(
									&a2eanim::meshes[i].vertices[a2eanim::meshes[i].indices[a2eanim::normal_list[i][k].num[l]].i1],
									&a2eanim::meshes[i].vertices[a2eanim::meshes[i].indices[a2eanim::normal_list[i][k].num[l]].i2],
									&a2eanim::meshes[i].vertices[a2eanim::meshes[i].indices[a2eanim::normal_list[i][k].num[l]].i3],
									normals[l], binormals[l], tangents[l],
									&a2eanim::meshes[i].tex_coords[a2eanim::meshes[i].indices[a2eanim::normal_list[i][k].num[l]].i1],
									&a2eanim::meshes[i].tex_coords[a2eanim::meshes[i].indices[a2eanim::normal_list[i][k].num[l]].i2],
									&a2eanim::meshes[i].tex_coords[a2eanim::meshes[i].indices[a2eanim::normal_list[i][k].num[l]].i3]);
							}
							else {
								c->compute_normal(
									&a2eanim::meshes[i].vertices[a2eanim::meshes[i].indices[a2eanim::normal_list[i][k].num[l]].i1],
									&a2eanim::meshes[i].vertices[a2eanim::meshes[i].indices[a2eanim::normal_list[i][k].num[l]].i2],
									&a2eanim::meshes[i].vertices[a2eanim::meshes[i].indices[a2eanim::normal_list[i][k].num[l]].i3],
									normals[l]);
							}
						}
						// add all normals, binormals and tangents and divide them by the amount of them
						for(unsigned int l = 0; l < a2eanim::normal_list[i][k].count; l++) {
							a2eanim::meshes[i].normal[j][k] += normals[l];
							// if there is no shader support, we don't need
							// to compute the binormal and tangent vectors
							if(exts->is_shader_support()) {
								a2eanim::meshes[i].binormal[j][k] += binormals[l];
								a2eanim::meshes[i].tangent[j][k] += tangents[l];
							}
						}
						a2eanim::meshes[i].normal[j][k] /= (float)a2eanim::normal_list[i][k].count;
						// if there is no shader support, we don't need
						// to compute the binormal and tangent vectors
						if(exts->is_shader_support()) {
							a2eanim::meshes[i].normal[j][k] *= -1.0f;
							a2eanim::meshes[i].binormal[j][k] /= (float)a2eanim::normal_list[i][k].count;
							a2eanim::meshes[i].tangent[j][k] /= (float)a2eanim::normal_list[i][k].count;
						}
					}
				}
			}
		}
		a2eanim::meshes[i].nbt_computed = true;
	}

	delete [] normals;
	delete [] binormals;
	delete [] tangents;
}

/*! sets the current light color (used for parallax mapping)
 *  @param lcol the light color
 */
void a2eanim::set_light_color(float* lcol) {
	a2eanim::light_color = lcol;
}

/*! sets the current light position (used for parallax mapping)
 *  @param lpos the light position
 */
void a2eanim::set_light_position(vertex3* lpos) {
	a2eanim::light_position = lpos;
}

/*! returns the models bounding box
 */
core::aabbox* a2eanim::get_bounding_box() {
	return a2eanim::bbox;
}

/*! builds the models bounding box
 */
void a2eanim::build_bounding_box() {
	// create frame 0 and build the bounding box
	for(unsigned int i = 0; i < a2eanim::base_joint_count; i++) {
		a2eanim::build_bone(0, base_joints[i], &base_joints[i]->position, &base_joints[i]->orientation);
	}
	a2eanim::skin_mesh();

	float minx, miny, minz, maxx, maxy, maxz;
	minx = a2eanim::meshes[0].vertices[0].x;
	miny = a2eanim::meshes[0].vertices[0].y;
	minz = a2eanim::meshes[0].vertices[0].z;
	maxx = a2eanim::meshes[0].vertices[0].x;
	maxy = a2eanim::meshes[0].vertices[0].y;
	maxz = a2eanim::meshes[0].vertices[0].z;
	for(unsigned int i = 1; i < a2eanim::meshes[0].vertex_count; i++) {
		if(a2eanim::meshes[0].vertices[i].x < minx) {
			minx = a2eanim::meshes[0].vertices[i].x;
		}
		if(a2eanim::meshes[0].vertices[i].y < miny) {
			miny = a2eanim::meshes[0].vertices[i].y;
		}
		if(a2eanim::meshes[0].vertices[i].z < minz) {
			minz = a2eanim::meshes[0].vertices[i].z;
		}

		if(a2eanim::meshes[0].vertices[i].x > maxx) {
			maxx = a2eanim::meshes[0].vertices[i].x;
		}
		if(a2eanim::meshes[0].vertices[i].y > maxy) {
			maxy = a2eanim::meshes[0].vertices[i].y;
		}
		if(a2eanim::meshes[0].vertices[i].z > maxz) {
			maxz = a2eanim::meshes[0].vertices[i].z;
		}
	}

	if(a2eanim::bbox) { delete a2eanim::bbox; }
	a2eanim::bbox = new core::aabbox();
	a2eanim::bbox->vmin.x = minx;
	a2eanim::bbox->vmin.y = miny;
	a2eanim::bbox->vmin.z = minz;
	a2eanim::bbox->vmax.x = maxx;
	a2eanim::bbox->vmax.y = maxy;
	a2eanim::bbox->vmax.z = maxz;
}

int a2eanim::mt_nbt(void* data) {
	SDL_mutex* lock = (SDL_mutex*)data;

	SDL_mutexP(lock);
	unsigned int tn = mt_cur_tn; // thread number
	mt_cur_tn++;
	SDL_mutexV(lock);

	while(!mt_thread_done[tn]) {
		if(SDL_mutexP(lock) != 0) cout << "error locking mutex ..." << endl;
		if(!mt_thread_done2[tn]) {
			for(unsigned int i = mt_start_num[tn]; i < mt_end_num[tn]; i++) {
				// check if vertex is part of a triangle
				if(a2eanim::mt_normal_list[i].count != 0) {
					// compute the normals, binormals and tangents for all triangles the vertex is part of
					for(unsigned int l = 0; l < mt_normal_list[i].count; l++) {
						// if there is no shader support, we don't need
						// to compute the binormal and tangent vectors
						if(exts->is_shader_support()) {
							c->compute_normal_tangent_binormal(
								&mt_mesh->vertices[mt_mesh->indices[mt_normal_list[i].num[l]].i1],
								&mt_mesh->vertices[mt_mesh->indices[mt_normal_list[i].num[l]].i2],
								&mt_mesh->vertices[mt_mesh->indices[mt_normal_list[i].num[l]].i3],
								mt_normals[tn][l], mt_binormals[tn][l], mt_tangents[tn][l],
								&mt_mesh->tex_coords[mt_mesh->indices[mt_normal_list[i].num[l]].i1],
								&mt_mesh->tex_coords[mt_mesh->indices[mt_normal_list[i].num[l]].i2],
								&mt_mesh->tex_coords[mt_mesh->indices[mt_normal_list[i].num[l]].i3]);
						}
						else {
							c->compute_normal(
								&mt_mesh->vertices[mt_mesh->indices[mt_normal_list[i].num[l]].i1],
								&mt_mesh->vertices[mt_mesh->indices[mt_normal_list[i].num[l]].i2],
								&mt_mesh->vertices[mt_mesh->indices[mt_normal_list[i].num[l]].i3],
								mt_normals[tn][l]);
						}
					}

					// add all normals, binormals and tangents and divide them by the amount of them
					for(unsigned int l = 0; l < a2eanim::mt_normal_list[i].count; l++) {
						a2eanim::mt_mesh->normal[mt_cur_frame][i] += mt_normals[tn][l];
						// if there is no shader support, we don't need
						// to compute the binormal and tangent vectors
						if(exts->is_shader_support()) {
							a2eanim::mt_mesh->binormal[mt_cur_frame][i] += mt_binormals[tn][l];
							a2eanim::mt_mesh->tangent[mt_cur_frame][i] += mt_tangents[tn][l];
						}
					}
					a2eanim::mt_mesh->normal[mt_cur_frame][i] /= (float)a2eanim::mt_normal_list[i].count;
					// if there is no shader support, we don't need
					// to compute the binormal and tangent vectors
					if(exts->is_shader_support()) {
						a2eanim::mt_mesh->normal[mt_cur_frame][i] *= -1.0f;
						a2eanim::mt_mesh->binormal[mt_cur_frame][i] /= (float)a2eanim::mt_normal_list[i].count;
						a2eanim::mt_mesh->tangent[mt_cur_frame][i] /= (float)a2eanim::mt_normal_list[i].count;
					}
				}
			}
			mt_thread_done2[tn] = true;
		}
		if(SDL_mutexV(lock) != 0) cout << "error unlocking mutex ..." << endl;
	}

	return 1;
}

unsigned int a2eanim::get_object_count() {
	return a2eanim::mesh_count;
}

