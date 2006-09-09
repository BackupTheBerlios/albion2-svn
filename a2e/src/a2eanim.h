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

#ifndef __A2EANIM_H__
#define __A2EANIM_H__

#ifdef WIN32
#include <windows.h>
#endif

#include <iostream>
#include <fstream>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_thread.h>
#include <cmath>
#include <GL/gl.h>
#include <GL/glu.h>
#include "msg.h"
#include "core.h"
#include "vertex3.h"
#include "quaternion.h"
#include "file_io.h"
#include "a2ematerial.h"
#include "engine.h"
#include "shader.h"
#include "extensions.h"
using namespace std;

#include "win_dll_export.h"

/*! @class a2eanim
 *  @brief class for loading and displaying an a2e skeletal animated model
 *  @author flo
 *  @todo -
 *  
 *  the a2e skeletal animation class
 *  
 *  thx to bozo for the .md5 format specification and
 *  julien rebetez for that very useful .md5 tutorial (and code)
 */

class A2E_API a2eanim
{
protected:
	struct joint;
	struct animation;

public:
	a2eanim(engine* e, shader* s);
	~a2eanim();

	void draw(bool use_shader = true);
	void draw_joints();

	void load_model(char* filename, bool vbo = false);
	void add_animation(char* filename);
	void delete_animations();

	void build_bone(unsigned int frame, joint* jnt, vertex3* parent_translation, quaternion* parent_oriantation);
	void skin_mesh();

	void set_material(a2ematerial* material);

	void set_draw_joints(bool state);
	bool get_draw_joints();

	unsigned int get_animation_count();
	animation* get_animation(unsigned int num);
	unsigned int get_current_animation_number();
	animation* get_current_animation();

	unsigned int get_current_frame();
	void set_current_frame(unsigned int num);

	void set_current_animation(unsigned int num);
	void play_frames(unsigned start, unsigned int end);

	void set_position(float x, float y, float z);
	vertex3* get_position();
	void set_scale(float x, float y, float z);
	vertex3* get_scale();
	void set_rotation(float x, float y, float z);
	vertex3* get_rotation();

	void set_visible(bool state);
	bool get_visible();

	core::aabbox* get_bounding_box();
	void build_bounding_box();

	string* get_object_names();

	vertex3* get_vertices(unsigned int mesh);
	core::index* get_indices(unsigned int mesh);
	unsigned int get_index_count(unsigned int mesh);

	// used for parallax mapping
	void generate_normal_list();
	void generate_normals(); // with threading
	void generate_normals_nt(); // w/o / no threading

	void set_light_color(float* lcol);
	void set_light_position(vertex3* lpos);

	unsigned int get_object_count();

protected:
	static msg* m;
	file_io* file;
	static core* c;
	engine* e;
	shader* s;
	static ext* exts;

	struct joint {
		int num;
		int parent_num;
		vertex3 position;
		quaternion orientation;

		joint* parent;
		unsigned int children_count;
		joint** childrens;
	};

	struct mesh {
		unsigned int vertex_count;
		unsigned int* weight_counts;
		unsigned int* weight_indices;
		vertex3* vertices;
		core::coord* tex_coords;
		GLuint vbo_vertices_id;
		GLuint vbo_tex_coords_id;

		// boolean that determines if we already computed
		// the normal, binormal and tangent vector
		bool nbt_computed;
		vertex3** normal;
		vertex3** binormal;
		vertex3** tangent;
		GLuint vbo_normals_id;
		GLuint vbo_binormals_id;
		GLuint vbo_tangents_id;

		unsigned int triangle_count;
		core::index* indices;
		GLuint vbo_indices_ids;

		unsigned int weight_count;
		int* bone_indices;
		float* weight_strenghts;
		vertex3* weights;
	};

	struct animation {
		unsigned int joint_count;
		unsigned int frame_count;
		unsigned int animated_components_count;
		float frame_time;

		int* joint_parents;
		unsigned int* joint_flags;
		unsigned int* joint_start_indices;
		vertex3* joint_base_translation;
		quaternion* joint_base_orientation;

		float** frames;
	};

	struct nlist {
		unsigned int* num;
		unsigned int count;
	};

	unsigned int joint_count;
	unsigned int base_joint_count;
	unsigned int mesh_count;

	joint* joints;
	joint** base_joints;
	mesh* meshes;
	string* object_names;

	unsigned int canimations;
	animation** animations;
	unsigned int current_animation;

	unsigned int current_frame;
	unsigned int last_frame;
	unsigned int timer;

	bool is_material;
	a2ematerial* material;

	bool is_draw_joints;

	unsigned int start_frame;
	unsigned int end_frame;

	vertex3* position;
	vertex3* scale;
	vertex3* rotation;

	bool is_visible;

	float angle0;

    nlist** normal_list;
	float* light_color;
	vertex3* light_position;

	core::aabbox* bbox;

	bool init;

	//! flag that specifies if this model is using a vertex buffer object
	bool vbo;

	// for multi-threading ...
	void mt_compute_nbt();
	static int mt_nbt(void* data);

	unsigned int thread_count;
	static vertex3** mt_normals;
	static vertex3** mt_binormals;
	static vertex3** mt_tangents;
	static unsigned int mt_cur_mesh;
	static unsigned int mt_cur_frame;
	static unsigned int* mt_start_num;
	static unsigned int* mt_end_num;
	static mesh* mt_mesh;
	static nlist* mt_normal_list;
	static bool* mt_thread_done;
	static bool* mt_thread_done2;
	static unsigned int mt_cur_tn;
};

#endif
