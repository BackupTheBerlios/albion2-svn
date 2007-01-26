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

#ifndef __A2EMODEL_H__
#define __A2EMODEL_H__

#ifdef WIN32
#include <windows.h>
#endif

#include <iostream>
#include <cmath>
#include <vector>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include "msg.h"
#include "core.h"
#include "vertex3.h"
#include "file_io.h"
#include "engine.h"
#include "a2ematerial.h"
#include "shader.h"
#include "matrix4.h"
using namespace std;

#include "win_dll_export.h"

/*! @class a2emodel
 *  @brief class for loading and displaying an a2e static model
 *  @author flo
 *  @todo -
 *  
 *  the a2emodel class
 */

class A2E_API a2emodel
{
public:
	a2emodel(engine* e, shader* s);
	~a2emodel();

	void load_model(char* filename, bool vbo = false);
	void draw(bool use_shader = true);
	void set_position(float x, float y, float z);
	vertex3* get_position();
	void set_scale(float x, float y, float z);
	void set_hard_scale(float x, float y, float z);
	vertex3* get_scale();
	void set_rotation(float x, float y, float z);
	vertex3* get_rotation();

	void draw_phys_obj();
	void set_draw_phys_obj(bool state);
	bool get_draw_phys_obj();

	vertex3* get_vertices();
	core::index* get_total_indices();
	core::index* get_indices(unsigned int obj_num);
	unsigned int get_vertex_count();
	unsigned int get_index_count();
	unsigned int get_index_count(unsigned int obj_num);

	void scale_tex_coords(float su, float sv);

	void build_bounding_box();
	core::aabbox* get_bounding_box();

	void set_draw_wireframe(bool state);
	bool get_draw_wireframe();

	void set_visible(bool state);
	bool get_visible();

	void set_material(a2ematerial* material);

	string* get_object_names();

	// stuff for collision detection
	void set_radius(float radius);
	void set_length(float length);
	float get_radius();
	float get_length();
	void set_phys_scale(float x, float y, float z);
	vertex3* get_phys_scale();

	bool is_collision_model();
	vertex3* get_col_vertices();
	core::index* get_col_indices();
	unsigned int get_col_vertex_count();
	unsigned int get_col_index_count();

	void update_mview_matrix();
	void update_scale_matrix();

	// used for parallax mapping
	void generate_normal_list();
	void generate_normals();

	void set_light_position(vertex3* lpos);

	unsigned int get_object_count();

protected:
	msg* m;
	file_io* file;
	core* c;
	engine* e;
	texman* t;
	shader* s;
	ext* exts;

	vertex3* vertices;
	core::coord* tex_coords;
	core::index** indices;
	unsigned int object_count;
	unsigned int vertex_count;
	unsigned int* index_count;
	GLuint vbo_vertices_id;
	GLuint vbo_tex_coords_id;
	GLuint* vbo_indices_ids;
	GLuint vbo_normals_id;
	GLuint vbo_binormals_id;
	GLuint vbo_tangents_id;

	string* object_names;

	bool collision_model;
	unsigned int col_vertex_count;
	unsigned int col_index_count;
	vertex3* col_vertices;
	core::index* col_indices;

	vertex3* normals;
	vertex3* binormals;
	vertex3* tangents;

	vertex3* position;
	vertex3* scale;
	vertex3* rotation;

	core::aabbox* bbox;

	bool draw_wireframe;

	bool is_visible;

	bool is_material;
	a2ematerial* material;

	// some variables for collision detection
	float radius;
	float length;
	vertex3* phys_scale;

	// normal stuff
	struct nlist {
		unsigned int* num;
		unsigned int count;
	};
    nlist* normal_list;
	unsigned int max_vertex_connections;

	vertex3* light_position;

	matrix4 mview_mat;
	matrix4 scale_mat;

	bool is_draw_phys_obj;

	//! flag that specifies if this model is using a vertex buffer object
	bool vbo;
};

#endif
