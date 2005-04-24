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

#ifndef __ODE_H__
#define __ODE_H__

#include <iostream>
#include <SDL.h>
#include <ode/ode.h>
#include <cmath>
#include <list>
#include "msg.h"
#include "core.h"
#include "engine.h"
#include "a2emodel.h"
#include "ode_object.h"
using namespace std;

#define MAX_CONTACTS 128
#define MAX_OBJECTS 4096

#include "win_dll_export.h"

/*! @class ode
 *  @brief open dynamic engine bindings/functions
 *  @author flo
 *  @version 0.1
 *  @date 2005/01/10
 *  @todo more functions
 *  
 *  bindings and functions for using the open dynamic engine
 */

class A2E_API ode
{
public:
	ode();
	~ode();

	void init();
	void close();
	void run(unsigned int last_frame);

	static void collision_callback(void* data, dGeomID o1, dGeomID o2);

	void add_object(a2emodel* model, bool fixed, ode_object::OTYPE type);
	void update_objects();

	ode_object* get_ode_object(unsigned int num);

protected:
	msg m;
	core c;

	unsigned int object_count;
	ode_object* ode_objects[MAX_OBJECTS];

	static dWorldID world;
	static dSpaceID space;
	static dJointGroupID joint_group;
	static float gravity;
	static float cfm;
	static float erp;

};

#endif
