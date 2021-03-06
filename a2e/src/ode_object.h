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

#ifndef __ODE_OBJECT_H__
#define __ODE_OBJECT_H__

#include <iostream>
#include <SDL/SDL.h>
#include <ode/ode.h>
#include <cmath>
#include "msg.h"
#include "core.h"
#include "engine.h"
#include "a2emodel.h"
#include "quaternion.h"
using namespace std;

#include "win_dll_export.h"

/*! @class ode_object
 *  @brief class for ode objects (bodies) and their functions
 *  @author flo
 *  @todo more functions
 *  
 *  ode object (body) functions
 */

class A2E_API ode_object
{
public:
	//! the objects type specifier
	enum OTYPE {
		BOX,		//!< enum box geometry
		SPHERE,		//!< enum sphere geometry
		CYLINDER,	//!< enum cylinder geometry
		TRIMESH		//!< enum trimesh geometry
	};

	ode_object(engine* e);
	~ode_object();

	void init(dWorldID* world, dSpaceID* space, a2emodel* model, bool fixed, ode_object::OTYPE type, bool collision_map);

	void set_geom(a2emodel* model, ode_object::OTYPE type, bool collision_map);
	dGeomID get_geom();

	void set_body(ode_object::OTYPE type);
	dBodyID get_body();

	void set_model(a2emodel* model);
	a2emodel* get_model();

	void add_force(float x, float y, float z);
	void set_velocity(float x, float y, float z);
	void set_max_force(float force);

	void set_body_rotation(vertex3* rot);
	dReal* get_body_rotation();

	void reset();

	// for debugging purposes
	void set_mass(float mass);

protected:
	msg* m;
	core* c;
	engine* e;

	dWorldID* world;
	dSpaceID* space;
	dGeomID geom;
	dBodyID body;
	dMass* mass;
	dTriMeshDataID trimesh;
	dVector3* dvertices;
	unsigned int* dindices;
	unsigned int vertex_count;
	unsigned int index_count;

	a2emodel* model;
	bool fixed;

	static float density;

	float max_force;

};

#endif
