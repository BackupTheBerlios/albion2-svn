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

#include "a2emodel.h"

/*! a2emodel constructor
 */
a2emodel::a2emodel(engine* e, shader* s) {
	a2emodel::position = new vertex3();
	a2emodel::position->x = 0.0f;
	a2emodel::position->y = 0.0f;
	a2emodel::position->z = 0.0f;

	a2emodel::scale = new vertex3();
	a2emodel::scale->x = 1.0f;
	a2emodel::scale->y = 1.0f;
	a2emodel::scale->z = 1.0f;

	a2emodel::rotation = new vertex3();
	a2emodel::rotation->x = 0.0f;
	a2emodel::rotation->y = 0.0f;
	a2emodel::rotation->z = 0.0f;

	a2emodel::object_count = 0;
	a2emodel::vertex_count = 0;
	a2emodel::normals = NULL;
	a2emodel::binormals = NULL;
	a2emodel::tangents = NULL;
	a2emodel::bbox = NULL;
	a2emodel::indices = NULL;
	a2emodel::index_count = NULL;

	a2emodel::object_names = NULL;

	a2emodel::collision_model = false;
	a2emodel::col_vertex_count = 0;
	a2emodel::col_index_count = 0;
	a2emodel::col_vertices = NULL;
	a2emodel::col_indices = NULL;

	draw_wireframe = false;

	is_visible = true;

	normal_list = NULL;

	vbo = false;

	phys_scale = new vertex3(1.0f, 1.0f, 1.0f);

	is_draw_phys_obj = false;

	max_vertex_connections = 0;

	// get classes
	a2emodel::e = e;
	a2emodel::s = s;
	a2emodel::c = e->get_core();
	a2emodel::m = e->get_msg();
	a2emodel::file = e->get_file_io();
	a2emodel::t = e->get_texman();
	a2emodel::exts = e->get_ext();
}

/*! a2emodel destructor
 */
a2emodel::~a2emodel() {
	m->print(msg::MDEBUG, "a2emodel.cpp", "freeing a2emodel stuff");

	delete a2emodel::position;
	delete a2emodel::scale;
	delete a2emodel::rotation;
	if(a2emodel::bbox != NULL) { delete a2emodel::bbox; }
	if(a2emodel::vertices != NULL) { delete [] a2emodel::vertices; }
	if(a2emodel::tex_coords != NULL) { delete [] a2emodel::tex_coords; }
	if(a2emodel::indices != NULL) {
	    for(unsigned int i = 0; i < a2emodel::object_count; i++) {
	        delete [] a2emodel::indices[i];
	    }
	    delete [] a2emodel::indices;
    }
	if(a2emodel::index_count != NULL) { delete [] a2emodel::index_count; }

	if(a2emodel::normals != NULL) { delete [] a2emodel::normals; }
	if(a2emodel::binormals != NULL) { delete [] a2emodel::binormals; }
	if(a2emodel::tangents != NULL) { delete [] a2emodel::tangents; }

	if(a2emodel::normal_list != NULL) {
	    for(unsigned int i = 0; i < a2emodel::vertex_count; i++) {
	        delete [] a2emodel::normal_list[i].num;
	    }
	    delete [] a2emodel::normal_list;
    }

	if(a2emodel::object_names != NULL) { delete [] object_names; }

	// delete vbos
	if(vbo) {
		if(exts->glIsBufferARB(vbo_vertices_id)) { exts->glDeleteBuffersARB(1, &vbo_vertices_id); }
		if(exts->glIsBufferARB(vbo_tex_coords_id)) { exts->glDeleteBuffersARB(1, &vbo_tex_coords_id); }
		if(exts->glIsBufferARB(vbo_normals_id)) { exts->glDeleteBuffersARB(1, &vbo_normals_id); }
		if(exts->glIsBufferARB(vbo_binormals_id)) { exts->glDeleteBuffersARB(1, &vbo_binormals_id); }
		if(exts->glIsBufferARB(vbo_tangents_id)) { exts->glDeleteBuffersARB(1, &vbo_tangents_id); }
		if(exts->glIsBufferARB(vbo_indices_ids[0])) { exts->glDeleteBuffersARB(object_count, vbo_indices_ids); }
		if(a2emodel::vbo_indices_ids != NULL) { delete [] vbo_indices_ids; }
	}

	delete phys_scale;

	m->print(msg::MDEBUG, "a2emodel.cpp", "a2emodel stuff freed");
}

/*! draws the model
 *  @param use_shader flag that specifies if shaders should be used for rendering
 */
void a2emodel::draw(bool use_shader) {
	if(a2emodel::is_visible && e->get_init_mode() == engine::GRAPHICAL) {
		glPushMatrix();
		glTranslatef(a2emodel::position->x, a2emodel::position->y, a2emodel::position->z);

		// rotate the model (use local inverse model view matrix that we already calculated)
		glMultMatrixf(*mview_mat.m);

		// scale the model
		glMultMatrixf(*scale_mat.m);

		for(unsigned int i = 0; i < object_count; i++) {
			char mat_type = a2emodel::material->get_material_type(i);
			// parallax mapping
			if(exts->is_shader_support() && mat_type == a2ematerial::PARALLAX && use_shader) {
				if(!e->get_hdr()) s->use_shader(shader::PARALLAX);
				else s->use_shader(shader::PARALLAX_HDR);

				// we need to add (resp. subtract) the objects position ... kinda "retranslating"
				s->set_uniform3f(0, -e->get_position()->x - a2emodel::position->x,
									-e->get_position()->y - a2emodel::position->y,
									-e->get_position()->z - a2emodel::position->z);

				s->set_uniform3f(1, (light_position->x - a2emodel::position->x),
									(light_position->y - a2emodel::position->y),
									(light_position->z - a2emodel::position->z));

				s->set_uniform_matrix4fv(2, *mview_mat.m);

				s->set_uniform1i(3, 0);
				s->set_uniform1i(4, 1);
				s->set_uniform1i(5, 2);
				s->set_uniform1i(6, 3);

				exts->glActiveTextureARB(GL_TEXTURE0_ARB);
				glBindTexture(GL_TEXTURE_2D, a2emodel::material->get_texture(i, 0));
				glEnable(GL_TEXTURE_2D);
				exts->glActiveTextureARB(GL_TEXTURE1_ARB);
				glBindTexture(GL_TEXTURE_2D, a2emodel::material->get_texture(i, 1));
				glEnable(GL_TEXTURE_2D);
				exts->glActiveTextureARB(GL_TEXTURE2_ARB);
				glBindTexture(GL_TEXTURE_2D, a2emodel::material->get_texture(i, 2));
				glEnable(GL_TEXTURE_2D);
				exts->glActiveTextureARB(GL_TEXTURE3_ARB);
				glBindTexture(GL_TEXTURE_2D, a2emodel::material->get_texture(i, 3));
				glEnable(GL_TEXTURE_2D);

				// if the wireframe flag is set, draw the model in wireframe mode
				if(a2emodel::draw_wireframe) {
					glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				}
				if(a2emodel::material->get_color_type(i, 0) == 0x01) { glEnable(GL_BLEND); }

				glColor3f(1.0f, 1.0f, 1.0f);

				// do we use a vertex buffer object?
				if(vbo) {
					exts->glBindBufferARB(GL_ARRAY_BUFFER_ARB, vbo_vertices_id);
					glVertexPointer(3, GL_FLOAT, 0, NULL);

					exts->glBindBufferARB(GL_ARRAY_BUFFER_ARB, vbo_normals_id);
					exts->glEnableVertexAttribArrayARB(s->get_shader_object(shader::PARALLAX)->attributes[0]);
					exts->glVertexAttribPointerARB(s->get_shader_object(shader::PARALLAX)->attributes[0], 3, GL_FLOAT, GL_FALSE, 0, NULL);

					exts->glBindBufferARB(GL_ARRAY_BUFFER_ARB, vbo_binormals_id);
					exts->glEnableVertexAttribArrayARB(s->get_shader_object(shader::PARALLAX)->attributes[1]);
					exts->glVertexAttribPointerARB(s->get_shader_object(shader::PARALLAX)->attributes[1], 3, GL_FLOAT, GL_FALSE, 0, NULL);

					exts->glBindBufferARB(GL_ARRAY_BUFFER_ARB, vbo_tangents_id);
					exts->glEnableVertexAttribArrayARB(s->get_shader_object(shader::PARALLAX)->attributes[2]);
					exts->glVertexAttribPointerARB(s->get_shader_object(shader::PARALLAX)->attributes[2], 3, GL_FLOAT, GL_FALSE, 0, NULL);

					exts->glBindBufferARB(GL_ARRAY_BUFFER_ARB, vbo_tex_coords_id);
					exts->glEnableVertexAttribArrayARB(s->get_shader_object(shader::PARALLAX)->attributes[3]);
					exts->glVertexAttribPointerARB(s->get_shader_object(shader::PARALLAX)->attributes[3], 2, GL_FLOAT, GL_FALSE, 0, NULL);

					glEnableClientState(GL_VERTEX_ARRAY);

					exts->glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, vbo_indices_ids[i]);
					glDrawElements(GL_TRIANGLES, index_count[i] * 3, GL_UNSIGNED_INT, NULL);

					glDisableClientState(GL_VERTEX_ARRAY);

					exts->glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
					exts->glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);
				}
				// if not (or the extension isn't supported, use vertex arrays)
				else {
					glEnableClientState(GL_VERTEX_ARRAY);
					glVertexPointer(3, GL_FLOAT, 0, vertices);

					exts->glEnableVertexAttribArrayARB(s->get_shader_object(shader::PARALLAX)->attributes[0]);
					exts->glVertexAttribPointerARB(s->get_shader_object(shader::PARALLAX)->attributes[0], 3, GL_FLOAT, GL_FALSE, 0, normals);
					exts->glEnableVertexAttribArrayARB(s->get_shader_object(shader::PARALLAX)->attributes[1]);
					exts->glVertexAttribPointerARB(s->get_shader_object(shader::PARALLAX)->attributes[1], 3, GL_FLOAT, GL_FALSE, 0, binormals);
					exts->glEnableVertexAttribArrayARB(s->get_shader_object(shader::PARALLAX)->attributes[2]);
					exts->glVertexAttribPointerARB(s->get_shader_object(shader::PARALLAX)->attributes[2], 3, GL_FLOAT, GL_FALSE, 0, tangents);
					exts->glEnableVertexAttribArrayARB(s->get_shader_object(shader::PARALLAX)->attributes[3]);
					exts->glVertexAttribPointerARB(s->get_shader_object(shader::PARALLAX)->attributes[3], 2, GL_FLOAT, GL_FALSE, 0, tex_coords);

					glDrawElements(GL_TRIANGLES, index_count[i] * 3, GL_UNSIGNED_INT, indices[i]);

					glDisableClientState(GL_VERTEX_ARRAY);
				}


				if(a2emodel::material->get_color_type(i, 0) == 0x01) { glDisable(GL_BLEND); }
				// reset to filled mode
				if(a2emodel::draw_wireframe) {
					glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				}

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
				// if the wireframe flag is set, draw the model in wireframe mode
				if(a2emodel::draw_wireframe) {
					glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				}

				if(!e->get_hdr()) s->use_shader(shader::PHONG);
				else s->use_shader(shader::PHONG_HDR);

				s->set_uniform1i(0, 0);
				s->set_uniform1i(1, 1);

				a2emodel::material->enable_texture(i);

				glColor3f(1.0f, 1.0f, 1.0f);

				// do we use a vertex buffer object?
				if(vbo) {
					exts->glBindBufferARB(GL_ARRAY_BUFFER_ARB, vbo_vertices_id);
					glVertexPointer(3, GL_FLOAT, 0, NULL);
					glEnableClientState(GL_VERTEX_ARRAY);

					exts->glBindBufferARB(GL_ARRAY_BUFFER_ARB, vbo_normals_id);
					glNormalPointer(GL_FLOAT, 0, NULL);
					glEnableClientState(GL_NORMAL_ARRAY);

					for(unsigned int j = 0; j < a2emodel::material->get_texture_count(i); j++) {
						exts->glActiveTextureARB(GL_TEXTURE0_ARB+j);
						exts->glClientActiveTextureARB(GL_TEXTURE0_ARB+j);
						exts->glBindBufferARB(GL_ARRAY_BUFFER_ARB, vbo_tex_coords_id);
						glTexCoordPointer(2, GL_FLOAT, 0, NULL);
						glEnableClientState(GL_TEXTURE_COORD_ARRAY);
					}

					exts->glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, vbo_indices_ids[i]);
					glDrawElements(GL_TRIANGLES, index_count[i] * 3, GL_UNSIGNED_INT, NULL);

					glDisableClientState(GL_VERTEX_ARRAY);
					glDisableClientState(GL_NORMAL_ARRAY);
					for(int j = a2emodel::material->get_texture_count(i); j >= 0; j--) {
						exts->glActiveTextureARB(GL_TEXTURE0_ARB+j);
						exts->glClientActiveTextureARB(GL_TEXTURE0_ARB+j);
						glDisableClientState(GL_TEXTURE_COORD_ARRAY);
					}

					exts->glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
					exts->glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);
				}
				// if not (or the extension isn't supported, use vertex arrays)
				else {
					glVertexPointer(3, GL_FLOAT, 0, vertices);
					glEnableClientState(GL_VERTEX_ARRAY);
					glNormalPointer(GL_FLOAT, 0, normals);
					glEnableClientState(GL_NORMAL_ARRAY);
					
					for(unsigned int j = 0; j < a2emodel::material->get_texture_count(i); j++) {
						exts->glActiveTextureARB(GL_TEXTURE0_ARB+j);
						exts->glClientActiveTextureARB(GL_TEXTURE0_ARB+j);
						glTexCoordPointer(2, GL_FLOAT, 0, tex_coords);
						glEnableClientState(GL_TEXTURE_COORD_ARRAY);
					}

					glDrawElements(GL_TRIANGLES, index_count[i] * 3, GL_UNSIGNED_INT, indices[i]);

					glDisableClientState(GL_VERTEX_ARRAY);
					glDisableClientState(GL_NORMAL_ARRAY);
					for(int j = a2emodel::material->get_texture_count(i); j >= 0; j--) {
						exts->glActiveTextureARB(GL_TEXTURE0_ARB+j);
						exts->glClientActiveTextureARB(GL_TEXTURE0_ARB+j);
						glDisableClientState(GL_TEXTURE_COORD_ARRAY);
					}
				}

				a2emodel::material->disable_texture(i);

				s->use_shader(shader::NONE);

				// reset to filled mode
				if(a2emodel::draw_wireframe) {
					glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				}
			}
			// "normal" diffuse/mutli-texture mapping, no shader usage
			else { // !use_shader
				// if the wireframe flag is set, draw the model in wireframe mode
				if(a2emodel::draw_wireframe) {
					glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				}

				a2emodel::material->enable_texture(i, true);
				
				glColor3f(1.0f, 1.0f, 1.0f);

				unsigned int texture_count = a2emodel::material->get_texture_count(i);
				if(a2emodel::material->get_material_type(i) != a2ematerial::MULTITEXTURE) texture_count--;

				// do we use a vertex buffer object?
				if(vbo) {
					exts->glBindBufferARB(GL_ARRAY_BUFFER_ARB, vbo_vertices_id);
					glVertexPointer(3, GL_FLOAT, 0, NULL);
					glEnableClientState(GL_VERTEX_ARRAY);

					exts->glBindBufferARB(GL_ARRAY_BUFFER_ARB, vbo_normals_id);
					glNormalPointer(GL_FLOAT, 0, NULL);
					glEnableClientState(GL_NORMAL_ARRAY);


					for(unsigned int j = 0; j < texture_count; j++) {
						exts->glActiveTextureARB(GL_TEXTURE0_ARB+j);
						exts->glClientActiveTextureARB(GL_TEXTURE0_ARB+j);
						exts->glBindBufferARB(GL_ARRAY_BUFFER_ARB, vbo_tex_coords_id);
						glTexCoordPointer(2, GL_FLOAT, 0, NULL);
						glEnableClientState(GL_TEXTURE_COORD_ARRAY);
					}

					exts->glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, vbo_indices_ids[i]);
					glDrawElements(GL_TRIANGLES, index_count[i] * 3, GL_UNSIGNED_INT, NULL);

					glDisableClientState(GL_VERTEX_ARRAY);
					glDisableClientState(GL_NORMAL_ARRAY);
					for(int j = texture_count; j >= 0; j--) {
						exts->glActiveTextureARB(GL_TEXTURE0_ARB+j);
						exts->glClientActiveTextureARB(GL_TEXTURE0_ARB+j);
						glDisableClientState(GL_TEXTURE_COORD_ARRAY);
					}

					exts->glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
					exts->glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);
				}
				// if not (or the extension isn't supported, use vertex arrays)
				else {
					glVertexPointer(3, GL_FLOAT, 0, vertices);
					glEnableClientState(GL_VERTEX_ARRAY);
					glNormalPointer(GL_FLOAT, 0, normals);
					glEnableClientState(GL_NORMAL_ARRAY);
					
					for(unsigned int j = 0; j < texture_count; j++) {
						exts->glActiveTextureARB(GL_TEXTURE0_ARB+j);
						exts->glClientActiveTextureARB(GL_TEXTURE0_ARB+j);
						glTexCoordPointer(2, GL_FLOAT, 0, tex_coords);
						glEnableClientState(GL_TEXTURE_COORD_ARRAY);
					}

					glDrawElements(GL_TRIANGLES, index_count[i] * 3, GL_UNSIGNED_INT, indices[i]);

					glDisableClientState(GL_VERTEX_ARRAY);
					glDisableClientState(GL_NORMAL_ARRAY);
					for(int j = texture_count; j >= 0; j--) {
						exts->glActiveTextureARB(GL_TEXTURE0_ARB+j);
						exts->glClientActiveTextureARB(GL_TEXTURE0_ARB+j);
						glDisableClientState(GL_TEXTURE_COORD_ARRAY);
					}
				}

				a2emodel::material->disable_texture(i, true);

				// reset to filled mode
				if(a2emodel::draw_wireframe) {
					glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				}
			}
		}

		glPopMatrix();
	}
}

/*! draws the models physical model/object
 */
void a2emodel::draw_phys_obj() {
	if(e->get_init_mode() == engine::GRAPHICAL) {
		glPushMatrix();
		glTranslatef(a2emodel::position->x, a2emodel::position->y, a2emodel::position->z);

		// rotate the model (use local inverse model view matrix that we already calculated)
		glMultMatrixf(*mview_mat.m);

		// scale the model
		glScalef(a2emodel::phys_scale->x, a2emodel::phys_scale->y, a2emodel::phys_scale->z);

		glDisable(GL_TEXTURE_2D);
		glEnable(GL_BLEND);
		glDisable(GL_CULL_FACE);
		glDisable(GL_LIGHTING);
		glColor4f(1.0f, 0.0f, 0.0f, 0.5f);

		unsigned int draw_modes[] = { GL_FILL, GL_LINE };

		if(!collision_model) {
			for(unsigned int i = 0; i < 2; i++) {
				glPolygonMode(GL_FRONT_AND_BACK, draw_modes[i]);

				if(i) {
					glEnable(GL_POLYGON_OFFSET_LINE); // use polygon offset to "disable" z-fighting
					glPolygonOffset(-1.0f, 0.0f);
				}

				for(unsigned int j = 0; j < object_count; j++) {
					// do we use a vertex buffer object?
					if(vbo) {
						exts->glBindBufferARB(GL_ARRAY_BUFFER_ARB, vbo_vertices_id);
						glVertexPointer(3, GL_FLOAT, 0, NULL);
						glEnableClientState(GL_VERTEX_ARRAY);

						exts->glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, vbo_indices_ids[j]);

						glDrawElements(GL_TRIANGLES, index_count[j] * 3, GL_UNSIGNED_INT, NULL);

						glDisableClientState(GL_VERTEX_ARRAY);

						exts->glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
						exts->glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);
					}
					// if not (or the extension isn't supported, use vertex arrays)
					else {
						glVertexPointer(3, GL_FLOAT, 0, vertices);
						glEnableClientState(GL_VERTEX_ARRAY);

						glDrawElements(GL_TRIANGLES, index_count[j] * 3, GL_UNSIGNED_INT, indices[j]);

						glDisableClientState(GL_VERTEX_ARRAY);
					}
				}

				if(i) glDisable(GL_POLYGON_OFFSET_LINE);
			}
		}
		else {
			for(unsigned int i = 0; i < 2; i++) {
				glPolygonMode(GL_FRONT_AND_BACK, draw_modes[i]);

				if(i) {
					glEnable(GL_POLYGON_OFFSET_LINE); // use polygon offset to "disable" z-fighting
					glPolygonOffset(-1.0f, 0.0f);
				}

				glVertexPointer(3, GL_FLOAT, 0, col_vertices);
				glEnableClientState(GL_VERTEX_ARRAY);

				glDrawElements(GL_TRIANGLES, col_index_count * 3, GL_UNSIGNED_INT, col_indices);

				glDisableClientState(GL_VERTEX_ARRAY);

				if(i) glDisable(GL_POLYGON_OFFSET_LINE);
			}
		}

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		glEnable(GL_CULL_FACE);
		glDisable(GL_BLEND);

		glPopMatrix();
	}
}

/*! loads a .a2m model file
 *  @param filename the name of the .a2m model file
 *  @param vbo flag that specifies if vertex buffer objects should be used
 */
void a2emodel::load_model(char* filename, bool vbo) {
	a2emodel::vbo = vbo;
	// if vertex buffer objects are not supported, set vbo to false
	if(!exts->is_vbo_support()) {
		a2emodel::vbo = false;
	}

	file->open_file(filename, file_io::OT_READ_BINARY);

	// get type and name
	char* file_type = new char[9];
	file->get_block(file_type, 8);
	file_type[8] = 0;

	if(strcmp(file_type, "A2EMODEL") != 0) {
		m->print(msg::MERROR, "a2emodel.cpp", "non supported file type: %s!", file_type);
		delete [] file_type;
		file->close_file();
		return;
	}
	delete [] file_type;

	// get model type and abort if it's not 0x00 or 0x02
	char mtype = file->get_char();
	if(mtype != 0x00 && mtype != 0x02) {
		m->print(msg::MERROR, "a2emodel.cpp", "non supported model type: %u!", (unsigned int)(mtype & 0xFF));
		file->close_file();
		return;
	}

	if(mtype == 0x02) collision_model = true;

	vertex_count = file->get_uint();
	a2emodel::normals = new vertex3[a2emodel::vertex_count];
	a2emodel::binormals = new vertex3[a2emodel::vertex_count];
	a2emodel::tangents = new vertex3[a2emodel::vertex_count];
	a2emodel::vertices = new vertex3[a2emodel::vertex_count];
	a2emodel::tex_coords = new core::coord[a2emodel::vertex_count];

	for(unsigned int i = 0; i < vertex_count; i++) {
		vertices[i].x = file->get_float();
		vertices[i].y = file->get_float();
		vertices[i].z = file->get_float();
	}
	for(unsigned int i = 0; i < vertex_count; i++) {
		tex_coords[i].u = file->get_float();
		tex_coords[i].v = 1.0f - file->get_float();
	}

	object_count = file->get_uint();

	object_names = new string[object_count];
	char ch;
	for(unsigned int i = 0; i < object_count; i++) {
		while((ch = file->get_char()) != char(0xFF)) {
			object_names[i] += ch;
		}
	}

	indices = new core::index*[object_count];
	index_count = new unsigned int[object_count];
	for(unsigned int i = 0; i < object_count; i++) {
		index_count[i] = file->get_uint();
		indices[i] = new core::index[index_count[i]];
		for(unsigned int j = 0; j < index_count[i]; j++) {
			indices[i][j].i1 = file->get_uint();
			indices[i][j].i2 = file->get_uint();
			indices[i][j].i3 = file->get_uint();
		}
	}

	if(collision_model) {
		col_vertex_count = file->get_uint();
		col_vertices = new vertex3[col_vertex_count];
		for(unsigned int i = 0; i < col_vertex_count; i++) {
			col_vertices[i].x = file->get_float();
			col_vertices[i].y = file->get_float();
			col_vertices[i].z = file->get_float();
		}

		col_index_count = file->get_uint();
		col_indices = new core::index[col_index_count];
		for(unsigned int i = 0; i < col_index_count; i++) {
			col_indices[i].i1 = file->get_uint();
			col_indices[i].i2 = file->get_uint();
			col_indices[i].i3 = file->get_uint();
		}
	}

	file->close_file();

	a2emodel::generate_normal_list();
	a2emodel::generate_normals();

	a2emodel::build_bounding_box();

	if(a2emodel::vbo) {
		// vertices vbo
		exts->glGenBuffersARB(1, &vbo_vertices_id);
		exts->glBindBufferARB(GL_ARRAY_BUFFER_ARB, vbo_vertices_id);
		exts->glBufferDataARB(GL_ARRAY_BUFFER_ARB, vertex_count * 3 * sizeof(float), vertices, GL_STATIC_DRAW_ARB);

		// tex_coords vbo
		exts->glGenBuffersARB(1, &vbo_tex_coords_id);
		exts->glBindBufferARB(GL_ARRAY_BUFFER_ARB, vbo_tex_coords_id);
		exts->glBufferDataARB(GL_ARRAY_BUFFER_ARB, vertex_count * 2 * sizeof(float), tex_coords, GL_STATIC_DRAW_ARB);

		// normals vbo
		exts->glGenBuffersARB(1, &vbo_normals_id);
		exts->glBindBufferARB(GL_ARRAY_BUFFER_ARB, vbo_normals_id);
		exts->glBufferDataARB(GL_ARRAY_BUFFER_ARB, vertex_count * 3 * sizeof(float), normals, GL_STATIC_DRAW_ARB);

		// binormals vbo
		exts->glGenBuffersARB(1, &vbo_binormals_id);
		exts->glBindBufferARB(GL_ARRAY_BUFFER_ARB, vbo_binormals_id);
		exts->glBufferDataARB(GL_ARRAY_BUFFER_ARB, vertex_count * 3 * sizeof(float), binormals, GL_STATIC_DRAW_ARB);

		// tangents vbo
		exts->glGenBuffersARB(1, &vbo_tangents_id);
		exts->glBindBufferARB(GL_ARRAY_BUFFER_ARB, vbo_tangents_id);
		exts->glBufferDataARB(GL_ARRAY_BUFFER_ARB, vertex_count * 3 * sizeof(float), tangents, GL_STATIC_DRAW_ARB);

		// indices vbos
		vbo_indices_ids = new GLuint[object_count];
		for(unsigned int i = 0; i < object_count; i++) {
			exts->glGenBuffersARB(1, &vbo_indices_ids[i]);
			exts->glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, vbo_indices_ids[i]);
			exts->glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB, index_count[i] * 3 * sizeof(unsigned int), indices[i], GL_STATIC_DRAW_ARB);
		}

		// reset buffer
		exts->glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
		exts->glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);

		// not needed any more ... delete them?
		/*delete [] normals;
		delete [] binormals;
		delete [] tangents;
		tex_coords.clear();*/
		// we can't delete the vertices and indices here, b/c we still need them for some operations (esp. physics stuff)
		/*vertices.clear();
		for(unsigned int i = 0; i < object_count; i++) {
			indices[i].clear();
		}
		delete [] indices;*/
	}
}

/*! sets the position of the model
 *  @param x the x coordinate
 *  @param y the y coordinate
 *  @param z the z coordinate
 */
void a2emodel::set_position(float x, float y, float z) {
	a2emodel::position->x = x;
	a2emodel::position->y = y;
	a2emodel::position->z = z;
}

/*! returns the position of the model
 */
vertex3* a2emodel::get_position() {
	return a2emodel::position;
}

/*! sets the render scale of the model (the rendered model is scaled)
 *  @param x the x scale
 *  @param y the y scale
 *  @param z the z scale
 */
void a2emodel::set_scale(float x, float y, float z) {
	if(!(x != a2emodel::scale->x || y != a2emodel::scale->y || z != a2emodel::scale->z)) return;
	a2emodel::scale->x = x;
	a2emodel::scale->y = y;
	a2emodel::scale->z = z;

	update_scale_matrix();

	// rebuild the bounding box - not needed in here / pointless ...
	//a2emodel::build_bounding_box();
}

/*! sets the "vertex scale" of the model (the model itself is scaled)
 *  @param x the x scale
 *  @param y the y scale
 *  @param z the z scale
 */
void a2emodel::set_hard_scale(float x, float y, float z) {
	for(unsigned int i = 0; i < a2emodel::vertex_count; i++) {
		vertices[i].x *= x;
		vertices[i].y *= y;
		vertices[i].z *= z;
	}

	// reupload vertices stuff to vbo
	if(vbo) {
		exts->glBindBufferARB(GL_ARRAY_BUFFER_ARB, vbo_vertices_id);
		exts->glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, 0, vertex_count * 3 * sizeof(float), vertices);
		exts->glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
	}

	if(collision_model) {
		for(unsigned int i = 0; i < a2emodel::col_vertex_count; i++) {
			col_vertices[i].x *= x;
			col_vertices[i].y *= y;
			col_vertices[i].z *= z;
		}
	}

	// rebuild the bounding box
	a2emodel::build_bounding_box();
}

/*! returns the scale of the model
 */
vertex3* a2emodel::get_scale() {
	return a2emodel::scale;
}

/*! sets the rotation of the model
 *  @param x the x rotation
 *  @param y the y rotation
 *  @param z the z rotation
 */
void a2emodel::set_rotation(float x, float y, float z) {
	if(!(x != a2emodel::rotation->x || y != a2emodel::rotation->y || z != a2emodel::rotation->z)) return;

	a2emodel::rotation->x = x;
	a2emodel::rotation->y = y;
	a2emodel::rotation->z = z;

	update_mview_matrix();
}

/*! returns the rotation of the model
 */
vertex3* a2emodel::get_rotation() {
	return a2emodel::rotation;
}

/*! returns a pointer to the vertices
 */
vertex3* a2emodel::get_vertices() {
	return a2emodel::vertices;
}

/*! returns a pointer to all the indices (needs to be deleted manually!)
 */
core::index* a2emodel::get_total_indices() {
	unsigned int total_count = 0;
	for(unsigned int i = 0; i < object_count; i++) {
		total_count += index_count[i];
	}

	core::index* total_indices = new core::index[total_count];

	unsigned int cidx = 0;
	for(unsigned int i = 0; i < object_count; i++) {
		for(unsigned int j = 0; j < index_count[i]; j++) {
			total_indices[cidx].i1 = indices[i][j].i1;
			total_indices[cidx].i2 = indices[i][j].i2;
			total_indices[cidx].i3 = indices[i][j].i3;
			cidx++;
		}
	}

	return total_indices;
}

/*! returns a pointer to the specified (by obj_num) indices
 *  @param obj_num sub-object number we want the indices from
 */
core::index* a2emodel::get_indices(unsigned int obj_num) {
	return indices[obj_num];
}

/*! returns the vertex count
 */
unsigned int a2emodel::get_vertex_count() {
	return a2emodel::vertex_count;
}

/*! returns the index count
 */
unsigned int a2emodel::get_index_count() {
	unsigned int total_index_count = 0;
	for(unsigned int i = 0; i < a2emodel::object_count; i++) {
		total_index_count += index_count[i];
	}
	return total_index_count;
}

/*! returns the index count of the sub-object obj_num
 *  @param obj_num sub-object we want to get the index count from
 */
unsigned int a2emodel::get_index_count(unsigned int obj_num) {
	return index_count[obj_num];
}

/*! builds the bounding box
 */
void a2emodel::build_bounding_box() {
	float minx, miny, minz, maxx, maxy, maxz;
	minx = vertices[0].x;
	miny = vertices[0].y;
	minz = vertices[0].z;
	maxx = vertices[0].x;
	maxy = vertices[0].y;
	maxz = vertices[0].z;
	for(unsigned int i = 0; i < vertex_count; i++) {
		if(vertices[i].x < minx) {
			minx = vertices[i].x;
		}
		if(vertices[i].y < miny) {
			miny = vertices[i].y;
		}
		if(vertices[i].z < minz) {
			minz = vertices[i].z;
		}

		if(vertices[i].x > maxx) {
			maxx = vertices[i].x;
		}
		if(vertices[i].y > maxy) {
			maxy = vertices[i].y;
		}
		if(vertices[i].z > maxz) {
			maxz = vertices[i].z;
		}
	}

	if(a2emodel::bbox) { delete a2emodel::bbox; }
	a2emodel::bbox = new core::aabbox();
	a2emodel::bbox->vmin.x = minx;
	a2emodel::bbox->vmin.y = miny;
	a2emodel::bbox->vmin.z = minz;
	a2emodel::bbox->vmax.x = maxx;
	a2emodel::bbox->vmax.y = maxy;
	a2emodel::bbox->vmax.z = maxz;
}

/*! returns the bounding box of the model
 */
core::aabbox* a2emodel::get_bounding_box() {
	return a2emodel::bbox;
}

/*! used for ode collision - sets the radius of an sphere/cylinder object
 *  @param radius the objects radius
 */
void a2emodel::set_radius(float radius) {
	a2emodel::radius = radius;
}

/*! used for ode collision - returns the radius of an sphere/cylinder object
 */
float a2emodel::get_radius() {
	return a2emodel::radius;
}

/*! used for ode collision - sets the length of an cylinder object
 *  @param length the objects length
 */
void a2emodel::set_length(float length) {
	a2emodel::length = length;
}

/*! used for ode collision - returns the length of an cylinder object
 */
float a2emodel::get_length() {
	return a2emodel::length;
}

/*! sets the bool if the model is drawn as a wireframe
 *  @param state the new state
 */
void a2emodel::set_draw_wireframe(bool state) {
	a2emodel::draw_wireframe = state;
}

/*! returns a true if the model is drawn as a wireframe
 */
bool a2emodel::get_draw_wireframe() {
	return a2emodel::draw_wireframe;
}

/*! sets the models visible flag
 *  @param state the new state
 */
void a2emodel::set_visible(bool state) {
	a2emodel::is_visible = state;
}

/*! returns a true if the model is visible
 */
bool a2emodel::get_visible() {
	return a2emodel::is_visible;
}

/*! sets the models material
 *  @param material the material object that we want tu use
 */
void a2emodel::set_material(a2ematerial* material) {
	a2emodel::material = material;
	a2emodel::is_material = true;
}

/*! generates the "normal list"
 */
void a2emodel::generate_normal_list() {
	// we need temp numbers to store the triangle numbers of each vertex
	// i assume there is no vertex that is part of more than 32 triangles ...
	unsigned int* tmp_num = new unsigned int[32];
	unsigned int x = 0;
	core::index* all_indices = a2emodel::get_total_indices();

	// reserve memory for the normal list
	normal_list = new nlist[a2emodel::vertex_count];

	for(unsigned int i = 0; i < a2emodel::vertex_count; i++) {
		normal_list[i].num = NULL;
		normal_list[i].count = 0;
		x = 0;
		for(unsigned int j = 0; j < a2emodel::get_index_count(); j++) {
			if(i == all_indices[j].i1 ||
				i == all_indices[j].i2 ||
				i == all_indices[j].i3) {
				tmp_num[x] = j;
				x++;
			}
		}
		normal_list[i].count = x;
		if(normal_list[i].count > max_vertex_connections) max_vertex_connections = normal_list[i].count;
		if(x != 0) {
			normal_list[i].num = new unsigned int[normal_list[i].count];
			for(unsigned int j = 0; j < x; j++) {
				normal_list[i].num[j] = tmp_num[j];
			}
		}
	}

	delete [] tmp_num;
	delete [] all_indices;
}

/*! generates the normals, binormals and tangents of the model
 */
void a2emodel::generate_normals() {
	unsigned int thread_count = e->get_thread_count();
	vertex3** tnormals = new vertex3*[thread_count];
	vertex3** tbinormals = new vertex3*[thread_count];
	vertex3** ttangents = new vertex3*[thread_count];
	for(unsigned int i = 0; i < thread_count; i++) {
		tnormals[i] = new vertex3[max_vertex_connections];
		tbinormals[i] = new vertex3[max_vertex_connections];
		ttangents[i] = new vertex3[max_vertex_connections];
	}
	core::index* all_indices = a2emodel::get_total_indices();

	int i, j, tn;

	#pragma omp parallel for \
	shared(all_indices) \
	private(i, j, tn) \
	firstprivate(tnormals, tbinormals, ttangents) \
	default(shared) \
	schedule(guided)
	for(i = 0; i < (int)vertex_count; i++) {
		tn = omp_get_thread_num();
		// check if vertex is part of a triangle
		if(a2emodel::normal_list[i].count != 0) {
			// compute the normals, binormals and tangents for all triangles the vertex is part of
			for(j = 0; j < (int)a2emodel::normal_list[i].count; j++) {
				if(exts->is_shader_support()) {
					c->compute_normal_tangent_binormal(
						&a2emodel::vertices[all_indices[a2emodel::normal_list[i].num[j]].i1],
						&a2emodel::vertices[all_indices[a2emodel::normal_list[i].num[j]].i2],
						&a2emodel::vertices[all_indices[a2emodel::normal_list[i].num[j]].i3],
						tnormals[tn][j], tbinormals[tn][j], ttangents[tn][j],
						&a2emodel::tex_coords[all_indices[a2emodel::normal_list[i].num[j]].i1],
						&a2emodel::tex_coords[all_indices[a2emodel::normal_list[i].num[j]].i2],
						&a2emodel::tex_coords[all_indices[a2emodel::normal_list[i].num[j]].i3]);
				}
				else {
					c->compute_normal(
						&a2emodel::vertices[all_indices[a2emodel::normal_list[i].num[j]].i1],
						&a2emodel::vertices[all_indices[a2emodel::normal_list[i].num[j]].i2],
						&a2emodel::vertices[all_indices[a2emodel::normal_list[i].num[j]].i3],
						tnormals[tn][j]);
				}
			}
			// add all normals, binormals and tangents and divide them by the amount of them
			for(j = 0; j < (int)a2emodel::normal_list[i].count; j++) {
				a2emodel::normals[i] += tnormals[tn][j];
				if(exts->is_shader_support()) {
					a2emodel::binormals[i] += tbinormals[tn][j];
					a2emodel::tangents[i] += ttangents[tn][j];
				}
			}
			a2emodel::normals[i] /= (float)a2emodel::normal_list[i].count;
			if(exts->is_shader_support()) {
				a2emodel::binormals[i] /= (float)a2emodel::normal_list[i].count;
				a2emodel::tangents[i] /= (float)a2emodel::normal_list[i].count;
			}
		}
	}

	delete [] all_indices;
	delete [] tnormals;
	delete [] tbinormals;
	delete [] ttangents;
}

/*! sets the current light position (used for parallax mapping)
 *  @param lpos the light position
 */
void a2emodel::set_light_position(vertex3* lpos) {
	a2emodel::light_position = lpos;
}

/*! scales the texture coordinates by (su, sv) - note that this is a "hard scale" and the vbo is updated automatically
 *  @param su the su scale factor
 *  @param sv the sv scale factor
 */
void a2emodel::scale_tex_coords(float su, float sv) {
	for(unsigned int i = 0; i < vertex_count; i++) {
		tex_coords[i].u *= su;
		tex_coords[i].v *= sv;
	}

	if(a2emodel::vbo) {
		// delete old vertex coordinates
		if(exts->glIsBufferARB(vbo_tex_coords_id)) { exts->glDeleteBuffersARB(1, &vbo_tex_coords_id); }
		// create new buffer
		exts->glGenBuffersARB(1, &vbo_tex_coords_id);
		exts->glBindBufferARB(GL_ARRAY_BUFFER_ARB, vbo_tex_coords_id);
		exts->glBufferDataARB(GL_ARRAY_BUFFER_ARB, vertex_count * 2 * sizeof(float), tex_coords, GL_STATIC_DRAW_ARB);
	}
}

/*! returns the models sub-object count
 */
unsigned int a2emodel::get_object_count() {
	return a2emodel::object_count;
}

/*! returns true if the model has a collision model
 */
bool a2emodel::is_collision_model() {
	return a2emodel::collision_model;
}

/*! returns a pointer to the models collision model vertices
 */
vertex3* a2emodel::get_col_vertices() {
	return col_vertices;
}

/*! returns a pointer to the models collision model indices
 */
core::index* a2emodel::get_col_indices() {
	return col_indices;
}

/*! returns the models collision model vertex count
 */
unsigned int a2emodel::get_col_vertex_count() {
	return col_vertex_count;
}

/*! returns the models collision model index count
 */
unsigned int a2emodel::get_col_index_count() {
	return col_index_count;
}

/*! returns a string pointer to the models sub-object names
 */
string* a2emodel::get_object_names() {
	return a2emodel::object_names;
}

/*! sets the models physical scale
 *  @param x x scale
 *  @param y y scale
 *  @param z z scale
 */
void a2emodel::set_phys_scale(float x, float y, float z) {
	phys_scale->set(x, y, z);
}

/*! returns the physical scale
 */
vertex3* a2emodel::get_phys_scale() {
	return phys_scale;
}

/*! updates the local modelview matrix
 */
void a2emodel::update_mview_matrix() {
	a2emodel::mview_mat.rotate(c->deg_to_rad(360.0f - rotation->x), c->deg_to_rad(rotation->y), c->deg_to_rad(360.0f - rotation->z));
	a2emodel::mview_mat.invert();
}

/*! updates the local scale matrix
 */
void a2emodel::update_scale_matrix() {
	a2emodel::scale_mat.identity();
	a2emodel::scale_mat.scale(scale->x, scale->y, scale->z);
}

/*! sets the models draw physical object state
 *  @param state the new state
 */
void a2emodel::set_draw_phys_obj(bool state) {
	a2emodel::is_draw_phys_obj = state;
}

/*! returns the models draw physical object flag
 */
bool a2emodel::get_draw_phys_obj() {
	return a2emodel::is_draw_phys_obj;
}
