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

#define MAX_OBJS 32

#include <iostream>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <math.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include "msg.h"
#include "core.h"
#include "vertex3.h"
#include "file_io.h"
#include "engine.h"
#include "a2ematerial.h"
#include "shader.h"
using namespace std;

#include "win_dll_export.h"

/*! @class a2emodel
 *  @brief class for loading and displaying an a2e model
 *  @author flo
 *  @version 0.2
 *  @todo more functions
 *  
 *  the a2emodel class
 */

class A2E_API a2emodel
{
public:
	a2emodel(engine* e, shader* s);
	~a2emodel();

	void load_model(char* filename);
	void draw();
	void set_position(float x, float y, float z);
	vertex3* get_position();
	void set_scale(float x, float y, float z);
	void set_hard_scale(float x, float y, float z);
	vertex3* get_scale();
	void set_rotation(float x, float y, float z);
	vertex3* get_rotation();

	vertex3* get_vertices();
	core::index* get_indices();
	core::index* get_tex_indices();
	unsigned int get_vertex_count();
	unsigned int get_index_count();

	void build_bounding_box();
	core::aabbox* get_bounding_box();

	void set_draw_wireframe(bool state);
	bool get_draw_wireframe();

	void set_visible(bool state);
	bool get_visible();

	void set_material(a2ematerial* material);

	// stuff for collision detection
	void set_radius(float radius);
	void set_length(float length);
	float get_radius();
	float get_length();

	// used for parallax mapping
	void generate_normal_list();
	void generate_normals();

	void set_light_color(float* lcol);
	void set_light_position(vertex3* lpos);

protected:
	msg* m;
	file_io* file;
	core* c;
	engine* e;
	texman* t;
	shader* s;
	ext* exts;

	char model_type[9];
	char model_name[9];
	unsigned int vertex_count;
	vertex3* vertices;
	unsigned int texture_count;
	char* tex_names[MAX_OBJS];
	unsigned int object_count;
	char* obj_names[MAX_OBJS];
	unsigned int* index_count;
	unsigned int* tex_value;
	unsigned int tex_vertex_count;
	core::coord* tex_coords;
	core::index* indices[MAX_OBJS];
	core::index* tex_indices[MAX_OBJS];
	vertex3* normals;
	vertex3* binormals;
	vertex3* tangents;

	GLuint textures[MAX_OBJS];

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

	// normal stuff
	struct nlist {
		unsigned int* num;
		unsigned int count;
	};
    nlist* normal_list;

	float* light_color;
	vertex3* light_position;
};

#endif
