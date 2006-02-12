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

#include "ode.h"

// initialize static variables
dWorldID ode::world = 0;
dSpaceID ode::space = 0;
dJointGroupID ode::joint_group = 0;
//float ode::gravity = -9.8f;
float ode::gravity = -20.0f;
float ode::cfm = 1e-5f;
float ode::erp = 0.2f;

/*! there is no function currently
 */
ode::ode(engine* e) {
	object_count = 0;

	// get classes
	ode::e = e;
	ode::c = e->get_core();
	ode::m = e->get_msg();
}

/*! there is no function currently
 */
ode::~ode() {
}

/*! initializes ode and creates a world
 */
void ode::init() {
	// create a world
	ode::world = dWorldCreate();

	// create a space
	ode::space = dHashSpaceCreate(0);

	// create a joint group
	ode::joint_group = dJointGroupCreate(0);

	// set the error reduction parameter to 0.2
	//dWorldSetERP(ode::world, ode::erp);

	// set the constraint force mixing to 10^-5
	dWorldSetCFM(ode::world, ode::cfm);

	// set the worlds gravity (
	dWorldSetGravity(ode::world, 0.0f, gravity, 0.0f);

	// - enable this if you notice jittering objects!
	//dWorldSetContactSurfaceLayer(ode::world, 0.001f);

	// set auto enable depth
	//dWorldSetAutoEnableDepthSF1(ode::world, 1);

	// set auto disable flag
	//dWorldSetAutoDisableFlag(ode::world, 1);

	// set the contacts max correction velocity
	dWorldSetContactMaxCorrectingVel(ode::world, 2.0f);

	// ?
	//dWorldSetQuickStepNumIterations(ode::world, 7);

	// initialize all ode objects
	for(unsigned int i = 0; i < MAX_OBJECTS; i++) {
		ode_objects[i] = 0;
	}
}

/*! destroys all ode relevant stuff
 */
void ode::close() {
	m->print(msg::MDEBUG, "ode.cpp", "freeing ode stuff");

	// destroy all ode objects
	for(unsigned int i = 0; i < ode::object_count; i++) {
		delete ode_objects[i];
	}

	// destroy the joint group
	dJointGroupDestroy(ode::joint_group);

	// destroy the space
	dSpaceDestroy(ode::space);

	// destroy the world
	dWorldDestroy(ode::world);

	// ... and finally close ode
	dCloseODE();

	m->print(msg::MDEBUG, "ode_object.cpp", "ode stuff freed");
}

/*! runs the open dynamics engine
 *  @param last_frame time that has elapsed since the last ode run
 */
void ode::run(unsigned int last_frame) {
	// execute a space collision
	dSpaceCollide(ode::space, 0, &ode::collision_callback);

	// execute a 0.05f world step
	dWorldStep(ode::world, 1.0f / last_frame);

	// clean the joint group for the next step
	dJointGroupEmpty(ode::joint_group);

	// update all objects
	ode::update_objects();
}

/*! runs the open dynamics engine
 */
void ode::collision_callback(void* data, dGeomID o1, dGeomID o2) {
	int i = 0;
	dBodyID b1 = dGeomGetBody(o1);
	dBodyID b2 = dGeomGetBody(o2);

	if(b1 && b2 && dAreConnectedExcluding(b1, b2, dJointTypeContact)) {
		return;
	}

	dContact contact[MAX_CONTACTS];
	for(i = 0; i < MAX_CONTACTS; i++) {
		/*contact[i].surface.mode = dContactBounce | dContactSoftCFM | dContactSlip1 | dContactSlip2 | dContactApprox1;
		//contact[i].surface.mode = dContactBounce | dContactSoftCFM;
		contact[i].surface.mu = dInfinity;
		contact[i].surface.bounce = 0.1f; // can be set to 0.1 too ;)
		contact[i].surface.bounce_vel = 0.1f; // can be set to 0.1 too ;)
		contact[i].surface.soft_cfm = 1e-5f;
		//contact[i].surface.soft_cfm = 0;
		//contact[i].surface.soft_erp = 1.0f;
		contact[i].surface.slip1 = 0.0f;
		contact[i].surface.slip2 = 0.0f;
		//contact[i].surface.*/

		contact[i].surface.mode = dContactBounce | dContactSoftERP | dContactSoftCFM;
		contact[i].surface.mu = dInfinity;
		contact[i].surface.slip1 = 0.1f; // friction
		contact[i].surface.slip2 = 0.1f;
		contact[i].surface.bounce= 0.0f;
		contact[i].surface.bounce_vel = 0.0f;
		contact[i].surface.soft_erp = 0.2f;
		contact[i].surface.soft_cfm = 1e-5f;

		/*contact[i].surface.mode = dContactBounce | dContactSoftCFM;
		contact[i].surface.mu = dInfinity;
		contact[i].surface.mu2 = 0.0f;
		contact[i].surface.bounce = 0.1f;
		contact[i].surface.bounce_vel = 0.1f;
		contact[i].surface.soft_cfm = 0.01f;*/

		/*contact[i].surface.mode = dContactSlip1 | dContactSlip2 | dContactSoftERP | dContactSoftCFM | dContactApprox1;
		contact[i].surface.mu = dInfinity;
		contact[i].surface.slip1 = 0.1f;
		contact[i].surface.slip2 = 0.1f;
		contact[i].surface.soft_erp = 0.5f;
		contact[i].surface.soft_cfm = 0.3f;*/
	}

	int numc = dCollide(o1, o2, MAX_CONTACTS, &contact[0].geom, sizeof(dContact));

	if(numc > 0) {
		for(i = 0; i < numc; i++){
			dJointID c = dJointCreateContact(ode::world, ode::joint_group, &contact[i]);
			dJointAttach(c, b1, b2);
		}
	}
}

/*! adds an object to the world
 *  @param model a pointer to the model object
 *  @param fixed bool, if the object can be moved or if its fixed
 *  @param type the type of the ode object
 */
ode_object* ode::add_object(a2emodel* model, bool fixed, ode_object::OTYPE type) {
	// create an new ode object and pass it to the list
	ode::ode_objects[ode::object_count] = new ode_object(e, &ode::world, &ode::space, model, fixed, type);

	// increment object count
	ode::object_count++;

	return ode::ode_objects[(ode::object_count-1)];
}

/*! deletes an object of the world
 *  @param num the objects number
 */
void ode::delete_object(unsigned int num) {
	delete ode::ode_objects[num];
	for(unsigned int i = num; i < (ode::object_count-1); i++) {
		ode::ode_objects[i] = ode::ode_objects[i+1];
	}
	ode::ode_objects[(ode::object_count-1)] = NULL;

	// decrease object count
	ode::object_count--;
}

/*! updates all ode objects
 */
void ode::update_objects() {
	for(unsigned int i = 0; i < object_count; i++) {
		if(ode::ode_objects[i]) {
			dGeomID geom = ode::ode_objects[i]->get_geom();
			dBodyID body = ode::ode_objects[i]->get_body();
			if(geom != 0) {
				dReal* pos = (dReal*)dGeomGetPosition(geom);
				ode::ode_objects[i]->get_model()->set_position((float)pos[0],
					(float)pos[1], (float)pos[2]);

				if(body != 0) {
					dReal* rotation = (dReal*)dBodyGetRotation(body);
					ode::ode_objects[i]->get_model()->set_rotation(c->rad_to_deg(rotation[0]),
						c->rad_to_deg(rotation[1]), c->rad_to_deg(rotation[2]));
				}
			}
		}
	}
}

/*! returns the ode object with the number 'num'
 *  @param num the index/number of the object you want to get
 */
ode_object* ode::get_ode_object(unsigned int num) {
	if(num < object_count) {
		return ode::ode_objects[num];
	}
	else {
		m->print(msg::MERROR, "ode.cpp", "object #%u does not exist!", num);
		return 0;
	}
}

/*! returns the ode object count
 */
unsigned int ode::get_object_count() {
	return ode::object_count;
}
