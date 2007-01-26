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
 
#ifndef __A2EMAP_H__
#define __A2EMAP_H__

#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include "msg.h"
#include "core.h"
#include "file_io.h"
#include "engine.h"
#include "a2emodel.h"
#include "a2eanim.h"
#include "a2ematerial.h"
#include "vertex3.h"
#include "scene.h"
#include "ode.h"
#include "ode_object.h"
using namespace std;

#include "win_dll_export.h"

/*! @class a2emap
 *  @brief a2e map routines
 *  @author flo
 *  @todo -
 *  
 *  the a2emap class
 */

class A2E_API a2emap
{
public:
	a2emap(engine* e, scene* sce, ode* o = NULL);
	~a2emap();

	struct map_object {
		bool model_type; // (false/0 = static, true/1 = dynamic/animated)
		string name;
		string model_filename;
		string ani_filename;
		string mat_filename;
		a2emodel* model;
		a2eanim* amodel;
		a2ematerial* mat;
		vertex3 position;
		vertex3 orientation;
		vertex3 scale;
		unsigned int phys_type; // (0 = box, 1 = sphere, 2 = cylinder, 3 = trimesh)
		ode_object* ode_obj;
		bool gravity;
		bool collision_model;
		bool auto_mass;
		float mass;
		vertex3 phys_scale;
	};

	bool load_map(const char* filename, bool vbo = false);
	void close_map();

	map_object* get_object(const char* name);

protected:
	engine* e;
	scene* sce;
	msg* m;
	core* c;
	file_io* f;
	ode* o;

	unsigned int object_count;
	vector<map_object> objects;
	bool map_opened;

};

#endif
