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

/*! a2eanim constructor
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

	object_names = NULL;

	phys_scale = new vertex3(1.0f, 1.0f, 1.0f);

	draw_wireframe = false;

	max_vertex_connections = 0;

	// get classes
	a2eanim::e = e;
	a2eanim::s = s;
	a2eanim::c = e->get_core();
	a2eanim::m = e->get_msg();
	a2eanim::file = e->get_file_io();
	a2eanim::exts = e->get_ext();
}

/*! a2eanim destructor
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
	delete a2eanim::phys_scale;

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

	if(object_names != NULL) { delete [] object_names; }

	m->print(msg::MDEBUG, "a2eanim.cpp", "a2eanim stuff freed");
}

/*! draws the model
 *  @param use_shader flag that specifies if shaders should be used for rendering
 */
void a2eanim::draw(bool use_shader) {
	if(a2eanim::is_visible && e->get_init_mode() == engine::GRAPHICAL) {
		// frame stuff
		bool new_frame = !init;
		if(a2eanim::current_frame < a2eanim::end_frame &&
			SDL_GetTicks() - a2eanim::timer > a2eanim::current_frame * a2eanim::animations[current_animation]->frame_time) {
			a2eanim::last_frame = a2eanim::current_frame;
			a2eanim::current_frame = (unsigned int)((float)(SDL_GetTicks() - a2eanim::timer) /
				a2eanim::animations[current_animation]->frame_time);
			new_frame = true;
		}

		if(a2eanim::current_frame >= a2eanim::end_frame) {
			a2eanim::current_frame = a2eanim::start_frame;
			a2eanim::timer = SDL_GetTicks();
		}

		if(new_frame) {
			// build the bones
			for(unsigned int i = 0; i < a2eanim::base_joint_count; i++) {
				a2eanim::build_bone(a2eanim::current_frame, base_joints[i], &base_joints[i]->position, &base_joints[i]->orientation);
			}
			// skin the mesh
			a2eanim::skin_mesh();
		}

		// ... and here we go finally - render the model :)
		glPushMatrix();
		glTranslatef(a2eanim::position->x, a2eanim::position->y, a2eanim::position->z);

		// rotate the model (use local inverse model view matrix that we already calculated)
		glMultMatrixf(*mview_mat.m);

		// scale the model
		glMultMatrixf(*scale_mat.m);

		// if the wireframe flag is set, draw the model in wireframe mode
		if(a2eanim::draw_wireframe) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}

		if(!init) {
			// generate the normals, binormals and tangents
			unsigned int t1 = SDL_GetTicks();
			generate_normals();
			unsigned int t2 = SDL_GetTicks() - t1;
			m->print(msg::MDEBUG, "a2eanim.cpp", "n/nbt computing time: %u ms", t2);
		}

		if((new_frame || !init) && vbo) {
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

			// reset buffer
			exts->glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
			exts->glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);
		}

		if(!init) init = true;

		for(unsigned int i = 0; i < a2eanim::mesh_count; i++) {
			char mat_type = a2eanim::material->get_material_type(i);
			if(exts->is_shader_support() && mat_type == a2ematerial::PARALLAX && use_shader) {
				if(!e->get_hdr()) s->use_shader(shader::PARALLAX);
				else s->use_shader(shader::PARALLAX_HDR);

				// we need to add (resp. subtract) the objects position ... kinda "retranslating"
				s->set_uniform3f(0, -e->get_position()->x - a2eanim::position->x,
									-e->get_position()->y - a2eanim::position->y,
									-e->get_position()->z - a2eanim::position->z);
				s->set_uniform3f(1, light_position->x - a2eanim::position->x,
									light_position->y - a2eanim::position->y,
									light_position->z - a2eanim::position->z);

				s->set_uniform_matrix4fv(2, *mview_mat.m);

				s->set_uniform1i(3, 0);
				s->set_uniform1i(4, 1);
				s->set_uniform1i(5, 2);
				s->set_uniform1i(6, 3);

				exts->glActiveTextureARB(GL_TEXTURE0_ARB);
				glBindTexture(GL_TEXTURE_2D, a2eanim::material->get_texture(i, 0));
				glEnable(GL_TEXTURE_2D);
				exts->glActiveTextureARB(GL_TEXTURE1_ARB);
				glBindTexture(GL_TEXTURE_2D, a2eanim::material->get_texture(i, 1));
				glEnable(GL_TEXTURE_2D);
				exts->glActiveTextureARB(GL_TEXTURE2_ARB);
				glBindTexture(GL_TEXTURE_2D, a2eanim::material->get_texture(i, 2));
				glEnable(GL_TEXTURE_2D);
				exts->glActiveTextureARB(GL_TEXTURE3_ARB);
				glBindTexture(GL_TEXTURE_2D, a2eanim::material->get_texture(i, 3));
				glEnable(GL_TEXTURE_2D);

				if(a2eanim::material->get_color_type(i, 0) == 0x01) { glEnable(GL_BLEND); }

				glColor3f(1.0f, 1.0f, 1.0f);

				if(vbo) {
					exts->glBindBufferARB(GL_ARRAY_BUFFER_ARB, a2eanim::meshes[i].vbo_vertices_id);
					glVertexPointer(3, GL_FLOAT, 0, NULL);

					// note: PARALLAX and PARALLAX_HDR need the same vertex attribute structure
					exts->glBindBufferARB(GL_ARRAY_BUFFER_ARB, a2eanim::meshes[i].vbo_normals_id);
					exts->glEnableVertexAttribArrayARB(s->get_shader_object(shader::PARALLAX)->attributes[0]);
					exts->glVertexAttribPointerARB(s->get_shader_object(shader::PARALLAX)->attributes[0], 3, GL_FLOAT, GL_FALSE, 0, NULL);

					exts->glBindBufferARB(GL_ARRAY_BUFFER_ARB, a2eanim::meshes[i].vbo_binormals_id);
					exts->glEnableVertexAttribArrayARB(s->get_shader_object(shader::PARALLAX)->attributes[1]);
					exts->glVertexAttribPointerARB(s->get_shader_object(shader::PARALLAX)->attributes[1], 3, GL_FLOAT, GL_FALSE, 0, NULL);
					
					exts->glBindBufferARB(GL_ARRAY_BUFFER_ARB, a2eanim::meshes[i].vbo_tangents_id);
					exts->glEnableVertexAttribArrayARB(s->get_shader_object(shader::PARALLAX)->attributes[2]);
					exts->glVertexAttribPointerARB(s->get_shader_object(shader::PARALLAX)->attributes[2], 3, GL_FLOAT, GL_FALSE, 0, NULL);

					exts->glBindBufferARB(GL_ARRAY_BUFFER_ARB, a2eanim::meshes[i].vbo_tex_coords_id);
					exts->glEnableVertexAttribArrayARB(s->get_shader_object(shader::PARALLAX)->attributes[3]);
					exts->glVertexAttribPointerARB(s->get_shader_object(shader::PARALLAX)->attributes[3], 2, GL_FLOAT, GL_FALSE, 0, NULL);

					glEnableClientState(GL_VERTEX_ARRAY);

					exts->glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, a2eanim::meshes[i].vbo_indices_ids);
					glDrawElements(GL_TRIANGLES, a2eanim::meshes[i].triangle_count * 3, GL_UNSIGNED_INT, NULL);

					glDisableClientState(GL_VERTEX_ARRAY);

					exts->glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
					exts->glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);
				}
				else {
					glEnableClientState(GL_VERTEX_ARRAY);
					glVertexPointer(3, GL_FLOAT, 0, a2eanim::meshes[i].vertices);

					exts->glEnableVertexAttribArrayARB(s->get_shader_object(shader::PARALLAX)->attributes[0]);
					exts->glVertexAttribPointerARB(s->get_shader_object(shader::PARALLAX)->attributes[0], 3, GL_FLOAT, GL_FALSE, 0, a2eanim::meshes[i].normal[a2eanim::current_frame]);
					exts->glEnableVertexAttribArrayARB(s->get_shader_object(shader::PARALLAX)->attributes[1]);
					exts->glVertexAttribPointerARB(s->get_shader_object(shader::PARALLAX)->attributes[1], 3, GL_FLOAT, GL_FALSE, 0, a2eanim::meshes[i].binormal[a2eanim::current_frame]);
					exts->glEnableVertexAttribArrayARB(s->get_shader_object(shader::PARALLAX)->attributes[2]);
					exts->glVertexAttribPointerARB(s->get_shader_object(shader::PARALLAX)->attributes[2], 3, GL_FLOAT, GL_FALSE, 0, a2eanim::meshes[i].tangent[a2eanim::current_frame]);
					exts->glEnableVertexAttribArrayARB(s->get_shader_object(shader::PARALLAX)->attributes[3]);
					exts->glVertexAttribPointerARB(s->get_shader_object(shader::PARALLAX)->attributes[3], 2, GL_FLOAT, GL_FALSE, 0, a2eanim::meshes[i].tex_coords);
					
					glEnableClientState(GL_VERTEX_ARRAY);

					glDrawElements(GL_TRIANGLES, a2eanim::meshes[i].triangle_count * 3, GL_UNSIGNED_INT, a2eanim::meshes[i].indices);

					glDisableClientState(GL_VERTEX_ARRAY);
				}

				if(a2eanim::material->get_color_type(i, 0) == 0x01) { glDisable(GL_BLEND); }

				exts->glActiveTextureARB(GL_TEXTURE3_ARB);
				glDisable(GL_TEXTURE_2D);
				exts->glActiveTextureARB(GL_TEXTURE2_ARB);
				glDisable(GL_TEXTURE_2D);
				exts->glActiveTextureARB(GL_TEXTURE1_ARB);
				glDisable(GL_TEXTURE_2D);
				exts->glActiveTextureARB(GL_TEXTURE0_ARB);
				glDisable(GL_TEXTURE_2D);
				s->use_shader(shader::NONE);
			}
			// phong lighting (w/ diffuse mapping)
			else if(exts->is_shader_support() && use_shader && mat_type == a2ematerial::DIFFUSE) {
				if(!e->get_hdr()) s->use_shader(shader::PHONG);
				else s->use_shader(shader::PHONG_HDR);

				s->set_uniform1i(0, 0);
				s->set_uniform1i(1, 1);

				material->enable_texture(i);

				//if(a2eanim::material->get_color_type(i, 0) == 0x01) { glEnable(GL_BLEND); }

				glColor3f(1.0f, 1.0f, 1.0f);

				if(vbo) {
					exts->glBindBufferARB(GL_ARRAY_BUFFER_ARB, a2eanim::meshes[i].vbo_vertices_id);
					glVertexPointer(3, GL_FLOAT, 0, NULL);
					glEnableClientState(GL_VERTEX_ARRAY);

					exts->glBindBufferARB(GL_ARRAY_BUFFER_ARB, a2eanim::meshes[i].vbo_normals_id);
					glNormalPointer(GL_FLOAT, 0, NULL);
					glEnableClientState(GL_NORMAL_ARRAY);

					exts->glBindBufferARB(GL_ARRAY_BUFFER_ARB, a2eanim::meshes[i].vbo_tex_coords_id);
					glTexCoordPointer(2, GL_FLOAT, 0, NULL);
					glEnableClientState(GL_TEXTURE_COORD_ARRAY);


					exts->glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, a2eanim::meshes[i].vbo_indices_ids);
					glDrawElements(GL_TRIANGLES, a2eanim::meshes[i].triangle_count * 3, GL_UNSIGNED_INT, NULL);
					

					glDisableClientState(GL_VERTEX_ARRAY);
					glDisableClientState(GL_NORMAL_ARRAY);
					glDisableClientState(GL_TEXTURE_COORD_ARRAY);

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

					glDrawElements(GL_TRIANGLES, a2eanim::meshes[i].triangle_count * 3, GL_UNSIGNED_INT, a2eanim::meshes[i].indices);

					glDisableClientState(GL_VERTEX_ARRAY);
					glDisableClientState(GL_NORMAL_ARRAY);
					glDisableClientState(GL_TEXTURE_COORD_ARRAY);
				}

				//if(a2eanim::material->get_color_type(i, 0) == 0x01) { glDisable(GL_BLEND); }

				material->disable_texture(i);
				s->use_shader(shader::NONE);
			}
			else { // !use_shader
				if(a2eanim::is_material) {
					glBindTexture(GL_TEXTURE_2D, a2eanim::material->get_texture(i, 0));
				}

				//glColor3f(1.0f, 1.0f, 1.0f);
				//glEnable(GL_TEXTURE_2D);
				//if(a2eanim::material->get_color_type(i, 0) == 0x01) { glEnable(GL_BLEND); }
				a2eanim::material->enable_texture(i);

				glColor3f(1.0f, 1.0f, 1.0f);

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
					glDrawElements(GL_TRIANGLES, a2eanim::meshes[i].triangle_count * 3, GL_UNSIGNED_INT, NULL);

					glDisableClientState(GL_VERTEX_ARRAY);
					glDisableClientState(GL_NORMAL_ARRAY);
					glDisableClientState(GL_TEXTURE_COORD_ARRAY);

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

					glDrawElements(GL_TRIANGLES, a2eanim::meshes[i].triangle_count * 3, GL_UNSIGNED_INT, a2eanim::meshes[i].indices);

					glDisableClientState(GL_VERTEX_ARRAY);
					glDisableClientState(GL_NORMAL_ARRAY);
					glDisableClientState(GL_TEXTURE_COORD_ARRAY);
				}

				a2eanim::material->disable_texture(i);
				//if(a2eanim::material->get_color_type(i, 0) == 0x01) { glDisable(GL_BLEND); }
				//glDisable(GL_TEXTURE_2D);
			}
		}

		// reset to filled mode
		if(a2eanim::draw_wireframe) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
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

	// get object_names
	object_names = new string[a2eanim::mesh_count];
	char ch;
	for(unsigned int i = 0; i < a2eanim::mesh_count; i++) {
		while((ch = file->get_char()) != char(0xFF)) {
			object_names[i] += ch;
		}
	}

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


			// stream buffers:

			// vertices vbo
			exts->glGenBuffersARB(1, &a2eanim::meshes[i].vbo_vertices_id);
			exts->glBindBufferARB(GL_ARRAY_BUFFER_ARB, a2eanim::meshes[i].vbo_vertices_id);
			exts->glBufferDataARB(GL_ARRAY_BUFFER_ARB, a2eanim::meshes[i].vertex_count * 3 * sizeof(float), NULL, GL_DYNAMIC_DRAW_ARB);

			// normal vbo
			exts->glGenBuffersARB(1, &a2eanim::meshes[i].vbo_normals_id);
			exts->glBindBufferARB(GL_ARRAY_BUFFER_ARB, a2eanim::meshes[i].vbo_normals_id);
			exts->glBufferDataARB(GL_ARRAY_BUFFER_ARB, a2eanim::meshes[i].vertex_count * 3 * sizeof(float), NULL, GL_DYNAMIC_DRAW_ARB);

			if(exts->is_shader_support()) {
				// binormal vbo
				exts->glGenBuffersARB(1, &a2eanim::meshes[i].vbo_binormals_id);
				exts->glBindBufferARB(GL_ARRAY_BUFFER_ARB, a2eanim::meshes[i].vbo_binormals_id);
				exts->glBufferDataARB(GL_ARRAY_BUFFER_ARB, a2eanim::meshes[i].vertex_count * 3 * sizeof(float), NULL, GL_DYNAMIC_DRAW_ARB);

				// tangent vbo
				exts->glGenBuffersARB(1, &a2eanim::meshes[i].vbo_tangents_id);
				exts->glBindBufferARB(GL_ARRAY_BUFFER_ARB, a2eanim::meshes[i].vbo_tangents_id);
				exts->glBufferDataARB(GL_ARRAY_BUFFER_ARB, a2eanim::meshes[i].vertex_count * 3 * sizeof(float), NULL, GL_DYNAMIC_DRAW_ARB);
			}
		}

		// reset buffer
		exts->glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
		exts->glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);
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
/*void a2eanim::skin_mesh() {
	vertex3 position;
	for(unsigned int i = 0; i < a2eanim::mesh_count; i++) {
		for(unsigned int j = 0; j < a2eanim::meshes[i].vertex_count; j++) {
			a2eanim::meshes[i].vertices[j].set(0.0f, 0.0f, 0.0f);

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
}*/

/*! skins the mesh (generates the vertices for the current frame)
 */
void a2eanim::skin_mesh() {
	vertex3 position;

	int i, j, k;
	a2eanim::mesh* msh = meshes;
	a2eanim::joint* jnt = joints;
	int base_index, bone_index;
	float tmp;

	#pragma omp parallel for \
	shared(msh, jnt) \
	private(i, j, k, position, base_index, bone_index, tmp) \
	default(shared) \
	schedule(static)
	for(i = 0; i < (int)a2eanim::mesh_count; i++) {
		for(j = 0; j < (int)msh[i].vertex_count; j++) {
			msh[i].vertices[j].set(0.0f, 0.0f, 0.0f);

			for(k = 0; k < (int)msh[i].weight_counts[j]; k++) {
				base_index = msh[i].weight_indices[j];
				bone_index = msh[i].bone_indices[base_index + k];

				position = jnt[bone_index].orientation.rotate(msh[i].weights[base_index + k]);
				msh[i].vertices[j] += (position + joints[bone_index].position) * msh[i].weight_strenghts[base_index + k];
			}
			// we have to switch the z and negative y coordinate here, b/c md5
			// models are rotated -90 along the x axis
			tmp = -msh[i].vertices[j].y;
			msh[i].vertices[j].y = msh[i].vertices[j].z;
			msh[i].vertices[j].z = tmp;
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
 *  @param num the number of the animation we want to set
 */
void a2eanim::set_current_animation(unsigned int num) {
	a2eanim::current_animation = num;
	a2eanim::current_frame = 0;
	a2eanim::timer = SDL_GetTicks();
}

/*! sets the range of frames that are "played back"
 *  @param start the start frame
 *  @param end the end frame
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
 *  @param num the number of the frame we want to set
 */
void a2eanim::set_current_frame(unsigned int num) {
	if(num > a2eanim::animations[current_animation]->frame_count || num < 0) {
		m->print(msg::MERROR, "a2eanim.cpp", "set_current_frame(): frame #%u doesn't exist!", num);
	}
	else {
		if(num > a2eanim::end_frame) {
			m->print(msg::MERROR, "a2eanim.cpp", "set_current_frame(): frame #%u is behind current play back range!", num);
		}
		else if(num < a2eanim::start_frame) {
			m->print(msg::MERROR, "a2eanim.cpp", "set_current_frame(): frame #%u is before current play back range!", num);
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
	if(!(x != a2eanim::scale->x || y != a2eanim::scale->y || z != a2eanim::scale->z)) return;

	a2eanim::scale->x = x;
	a2eanim::scale->y = y;
	a2eanim::scale->z = z;
	
	update_scale_matrix();
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
	if(!(x != a2eanim::rotation->x || y != a2eanim::rotation->y || z != a2eanim::rotation->z)) return;

	a2eanim::rotation->x = x;
	a2eanim::rotation->y = y;
	a2eanim::rotation->z = z;

	update_mview_matrix();
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
			if(normal_list[i][j].count > max_vertex_connections) max_vertex_connections = normal_list[i][j].count;
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

/*! generates the models normals, binormals and tangents
 */
void a2eanim::generate_normals() {
	unsigned int thread_count = e->get_thread_count();
	vertex3** normals = new vertex3*[thread_count];
	vertex3** binormals = new vertex3*[thread_count];
	vertex3** tangents = new vertex3*[thread_count];
	for(unsigned int i = 0; i < thread_count; i++) {
		normals[i] = new vertex3[max_vertex_connections];
		binormals[i] = new vertex3[max_vertex_connections];
		tangents[i] = new vertex3[max_vertex_connections];
	}

	for(unsigned int i = 0; i < a2eanim::mesh_count; i++) {
		if(!a2eanim::meshes[i].nbt_computed) {
			for(unsigned int j = 0; j < a2eanim::animations[a2eanim::current_animation]->frame_count; j++) {
				// build the bones for this frame ...
				for(unsigned int k = 0; k < a2eanim::base_joint_count; k++) {
					a2eanim::build_bone(j, base_joints[k], &base_joints[k]->position, &base_joints[k]->orientation);
				}
				// ... and skin the mesh
				a2eanim::skin_mesh();

				int k, l, tn, vertex_count;
				nlist* normal_listi = normal_list[i];
				a2eanim::mesh* meshesi = &meshes[i];
				vertex_count = meshesi->vertex_count;

				#pragma omp parallel for \
				shared(meshesi, normal_listi, vertex_count, normals, binormals, tangents) \
				private(k, l, tn) \
				default(shared) \
				schedule(guided)
				for(k = 0; k < vertex_count; k++) {
					tn = omp_get_thread_num();
					// check if vertex is part of a triangle
					if(normal_listi[k].count != 0) {
						// compute the normals, binormals and tangents for all triangles the vertex is part of
						for(l = 0; l < (int)normal_listi[k].count; l++) {
							// if there is no shader support, we don't need
							// to compute the binormal and tangent vectors
							if(exts->is_shader_support()) {
								c->compute_normal_tangent_binormal(
									&meshesi->vertices[meshesi->indices[normal_listi[k].num[l]].i1],
									&meshesi->vertices[meshesi->indices[normal_listi[k].num[l]].i2],
									&meshesi->vertices[meshesi->indices[normal_listi[k].num[l]].i3],
									normals[tn][l], binormals[tn][l], tangents[tn][l],
									&meshesi->tex_coords[meshesi->indices[normal_listi[k].num[l]].i1],
									&meshesi->tex_coords[meshesi->indices[normal_listi[k].num[l]].i2],
									&meshesi->tex_coords[meshesi->indices[normal_listi[k].num[l]].i3]);
							}
							else {
								c->compute_normal(
									&meshesi->vertices[meshesi->indices[normal_listi[k].num[l]].i1],
									&meshesi->vertices[meshesi->indices[normal_listi[k].num[l]].i2],
									&meshesi->vertices[meshesi->indices[normal_listi[k].num[l]].i3],
									normals[tn][l]);
							}
						}
						// add all normals, binormals and tangents and divide them by the amount of them
						for(l = 0; l < (int)normal_listi[k].count; l++) {
							meshesi->normal[j][k] += normals[tn][l];
							// if there is no shader support, we don't need
							// to compute the binormal and tangent vectors
							if(exts->is_shader_support()) {
								meshesi->binormal[j][k] += binormals[tn][l];
								meshesi->tangent[j][k] += tangents[tn][l];
							}
						}
						meshesi->normal[j][k] /= (float)normal_listi[k].count;
						// if there is no shader support, we don't need
						// to compute the binormal and tangent vectors
						if(exts->is_shader_support()) {
							meshesi->binormal[j][k] /= (float)normal_listi[k].count;
							meshesi->tangent[j][k] /= (float)normal_listi[k].count;
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

/*! returns the models sub-object count
 */
unsigned int a2eanim::get_object_count() {
	return a2eanim::mesh_count;
}

/*! returns a string pointer to the models sub-object names
 */
string* a2eanim::get_object_names() {
	return a2eanim::object_names;
}

/*! returns a pointer to the vertices of the specified mesh
 *  @param mesh the number of the mesh
 */
vertex3* a2eanim::get_vertices(unsigned int mesh) {
	return meshes[mesh].vertices;
}

/*! returns a pointer to the indices of the specified mesh
 *  @param mesh the number of the mesh
 */
core::index* a2eanim::get_indices(unsigned int mesh) {
	return meshes[mesh].indices;
}

/*! returns the index count of the specified mesh
 *  @param mesh the number of the mesh
 */
unsigned int a2eanim::get_index_count(unsigned int mesh) {
	return meshes[mesh].triangle_count;
}

/*! returns the index count of the model (all sub-objects)
 */
unsigned int a2eanim::get_index_count() {
	unsigned int total_index_count = 0;
	for(unsigned int i = 0; i < a2eanim::mesh_count; i++) {
		total_index_count += meshes[i].triangle_count;
	}
	return total_index_count;
}

/*! returns the vertex count of the specified mesh
 *  @param mesh the number of the mesh
 */
unsigned int a2eanim::get_vertex_count(unsigned int mesh) {
	return meshes[mesh].vertex_count;
}

/*! returns the vertex count of the model (all sub-objects)
 */
unsigned int a2eanim::get_vertex_count() {
	unsigned int total_vertex_count = 0;
	for(unsigned int i = 0; i < a2eanim::mesh_count; i++) {
		total_vertex_count += meshes[i].vertex_count;
	}
	return total_vertex_count;
}

/*! sets the models physical scale
 *  @param x x scale
 *  @param y y scale
 *  @param z z scale
 */
void a2eanim::set_phys_scale(float x, float y, float z) {
	phys_scale->set(x, y, z);
}

/*! returns the physical scale
 */
vertex3* a2eanim::get_phys_scale() {
	return phys_scale;
}

/*! sets the bool if the model is drawn as a wireframe
 *  @param state the new state
 */
void a2eanim::set_draw_wireframe(bool state) {
	a2eanim::draw_wireframe = state;
}

/*! returns a true if the model is drawn as a wireframe
 */
bool a2eanim::get_draw_wireframe() {
	return a2eanim::draw_wireframe;
}

/*! updates the local modelview matrix
 */
void a2eanim::update_mview_matrix() {
	a2eanim::mview_mat.rotate(c->deg_to_rad(360.0f - rotation->x), c->deg_to_rad(rotation->y), c->deg_to_rad(360.0f - rotation->z));
	a2eanim::mview_mat.invert();
}

/*! updates the local scale matrix
 */
void a2eanim::update_scale_matrix() {
	a2eanim::scale_mat.identity();
	a2eanim::scale_mat.scale(scale->x, scale->y, scale->z);
}
