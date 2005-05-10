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

#define MAX_OBJS 32

#include <iostream>
#include <fstream>
#include <SDL.h>
#include <SDL_image.h>
#include <cmath>
#include <GL/gl.h>
#include <GL/glu.h>
#include "msg.h"
#include "core.h"
#include "vertex3.h"
#include "quaternion.h"
#include "file_io.h"
#include "a2ematerial.h"
using namespace std;

#include "win_dll_export.h"

/*! @class a2eanim
 *  @brief class for loading and displaying an a2e skeletal animated model
 *  @author flo
 *  @version 0.1
 *  @date 2005/05/07
 *  @todo more functions
 *  
 *  the a2e skeletal animation class
 *  
 *  thx to bozo for .md5 format specification and julien rebetez
 *  for that very useful .md5 tutorial (and code)
 */

class A2E_API a2eanim
{
protected:
	struct joint;

public:
	a2eanim();
	~a2eanim();

	void draw();
	void draw_joints();

	void load_model(char* filename);
	void add_animation(char* filename);

	void build_bone(unsigned int frame, joint* jnt, vertex3* parent_translation, quaternion* parent_oriantation);
	void skin_mesh();

	void set_material(a2ematerial* material);

	void set_draw_joints(bool state);
	bool get_draw_joints();


protected:
	msg m;
	file_io file;
	core c;

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
		core::coord* tex_coords;
		vertex3* vertices;

		unsigned int triangle_count;
		core::index* indices;

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

	char file_type[9];

	unsigned int joint_count;
	unsigned int base_joint_count;
	unsigned int mesh_count;

	joint* joints;
	joint** base_joints;
	mesh* meshes;

	unsigned int canimations;
	animation** animations;
	unsigned int current_animation;

	unsigned int current_frame;
	unsigned int timer;

	bool is_material;
	a2ematerial* material;

	bool is_draw_joints;

};

#endif
