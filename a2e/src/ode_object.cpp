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

#include "ode_object.h"

// initialize static variables
float ode_object::density = 5.0f;

/*! there is no function currently
 */
ode_object::ode_object(engine* e) {
	// get classes
	ode_object::e = e;
	ode_object::c = e->get_core();
	ode_object::m = e->get_msg();
}

/*! there is no function currently
 */
ode_object::~ode_object() {
	m->print(msg::MDEBUG, "ode_object.cpp", "freeing ode_object stuff");

	// destroy the geom
	if(ode_object::geom) { dGeomDestroy(ode_object::geom); }

	// destroy the body
	if(ode_object::body) { dBodyDestroy(ode_object::body); }

	// delete the mass
	if(ode_object::mass) { delete ode_object::mass; }

	// destroy the trimesh
	if(ode_object::trimesh) { dGeomTriMeshDataDestroy(ode_object::trimesh); }

	if(ode_object::dvertices != NULL) { delete [] ode_object::dvertices; }
	if(ode_object::dindices != NULL) { delete [] ode_object::dindices; }

	m->print(msg::MDEBUG, "ode_object.cpp", "ode_object stuff freed");
}

void ode_object::init(dWorldID* world, dSpaceID* space, a2emodel* model, bool fixed, ode_object::OTYPE type) {
	ode_object::world = world;
	ode_object::space = space;
	ode_object::dvertices = NULL;
	ode_object::dindices = NULL;
	ode_object::set_model(model);
	ode_object::set_geom(ode_object::get_model(), type);
	ode_object::body = 0;

	if(!fixed && type != ode_object::TRIMESH) {
		ode_object::mass = new dMass();
		ode_object::set_body(type);
	}
	else {
		fixed = true;
		ode_object::body = NULL;
		ode_object::mass = NULL;
	}

	if(type != ode_object::TRIMESH) {
		ode_object::trimesh = NULL;
	}

	max_force = 10.0f;
}

/*! sets the geometry of the object
 *  @param model a pointer to the model object
 *  @param type the type of the ode object
 */
void ode_object::set_geom(a2emodel* model, ode_object::OTYPE type) {
	switch(type) {
		case ode_object::TRIMESH: {
			vertex3* vertices;
			core::index* indices;
			// initialize pointers to our a2emodel stuff
			if(!model->is_collision_model()) {
				// if we don't have a collision model, use the models vertices and indices
				vertices = model->get_vertices();
				indices = model->get_total_indices();
				ode_object::vertex_count = model->get_vertex_count();
				ode_object::index_count = model->get_index_count();
			}
			else {
				// if we have a collision model, use the collision models vertices and indices
				vertices = model->get_col_vertices();
				indices = model->get_col_indices();
				ode_object::vertex_count = model->get_col_vertex_count();
				ode_object::index_count = model->get_col_index_count();
			}

			// create trimesh data id
			ode_object::trimesh = dGeomTriMeshDataCreate();

			// because ode uses another vertex3/index store format,
			// we need to convert the a2emodel vertices/indices
			ode_object::dvertices = new dVector3[vertex_count];
			unsigned int x = 0;
			for(unsigned int i = 0; i < ode_object::vertex_count; i++) {
				ode_object::dvertices[x][0] = vertices[i].x;
				ode_object::dvertices[x][1] = vertices[i].y;
				ode_object::dvertices[x][2] = vertices[i].z;
				x++;
			}

			unsigned int j = 0;
			ode_object::dindices = new unsigned int[3*ode_object::index_count];
			for(unsigned int i = 0; i < ode_object::index_count; i++) {
				ode_object::dindices[j] = indices[i].i1;
				j++;
				ode_object::dindices[j] = indices[i].i2;
				j++;
				ode_object::dindices[j] = indices[i].i3;
				j++;
			}

			// and now build a simple trimesh out of the model vertices and indices
			dGeomTriMeshDataBuildSimple(ode_object::trimesh, (dReal*)ode_object::dvertices,
				ode_object::vertex_count, (const int*)ode_object::dindices,
				ode_object::index_count*3);

			// and finally create the tri mesh geom
			ode_object::geom = dCreateTriMesh(*ode_object::space, ode_object::trimesh, 0, 0, 0);

			// set the geoms data
			dGeomSetData(ode_object::geom, (void*)this);

			// set its body to 0
			dGeomSetBody(ode_object::geom, 0);

			// set the geoms position
			dGeomSetPosition(ode_object::geom, model->get_position()->x,
				model->get_position()->y, model->get_position()->z);

			if(!model->is_collision_model()) delete [] indices;

			// contact caching
			/*dGeomTriMeshEnableTC(ode_object::geom, 0, 1);
			dGeomTriMeshEnableTC(ode_object::geom, 1, 1);
			dGeomTriMeshEnableTC(ode_object::geom, 2, 1);*/
		}
		break;
		case ode_object::BOX: {
			ode_object::geom = dCreateBox(*(ode_object::space),
				(dReal)(ode_object::model->get_bounding_box()->vmax.x -
				ode_object::model->get_bounding_box()->vmin.x),
				(dReal)(ode_object::model->get_bounding_box()->vmax.y -
				ode_object::model->get_bounding_box()->vmin.y),
				(dReal)(ode_object::model->get_bounding_box()->vmax.z -
				ode_object::model->get_bounding_box()->vmin.z));
			dGeomSetData(ode_object::geom, (void*)this);
			dGeomSetPosition(ode_object::geom, model->get_position()->x,
				model->get_position()->y, model->get_position()->z);
		}
		break;
		case ode_object::SPHERE: {
			ode_object::geom = dCreateSphere(*(ode_object::space),
				(dReal)ode_object::model->get_radius());
			dGeomSetData(ode_object::geom, (void*)this);
			dGeomSetPosition(ode_object::geom, model->get_position()->x,
				model->get_position()->y, model->get_position()->z);
		}
		break;
		case ode_object::CYLINDER: {
			ode_object::geom = dCreateCCylinder(*(ode_object::space),
				(dReal)ode_object::model->get_radius(),
				(dReal)ode_object::model->get_length());
			dGeomSetData(ode_object::geom, (void*)this);
			dGeomSetPosition(ode_object::geom, model->get_position()->x,
				model->get_position()->y, model->get_position()->z);
		}
		break;
		default:
			m->print(msg::MERROR, "ode_object.cpp", "set_geom(): No object type specified!");
			break;
	}
}

/*! returns the geom
 */
dGeomID ode_object::get_geom() {
	return ode_object::geom;
}

/*! sets the objects model
 *  @param model a pointer to a model object
 */
void ode_object::set_model(a2emodel* model) {
	ode_object::model = model;
}

/*! returns the model
 */
a2emodel* ode_object::get_model() {
	return ode_object::model;
}

/*! sets the body of the object
 */
void ode_object::set_body(ode_object::OTYPE type) {
	// create the body
	ode_object::body = dBodyCreate(*ode_object::world);

	// why dMassAdjust? that confuses me, because
	// i already set the mass in setXYZ ... but the
	// calculated value is mostly much bigger than
	// the adjusted one ...
	// add 20050423: it seems like that a lower
	// mass increases the systems stability

	switch(type) {
		case ode_object::BOX: {
			// set the mass in the form of a box
			ode_object::mass->setBox(ode_object::density,
				(dReal)(ode_object::model->get_bounding_box()->vmax.x -
				ode_object::model->get_bounding_box()->vmin.x),
				(dReal)(ode_object::model->get_bounding_box()->vmax.y -
				ode_object::model->get_bounding_box()->vmin.y),
				(dReal)(ode_object::model->get_bounding_box()->vmax.z -
				ode_object::model->get_bounding_box()->vmin.z));
			dMassAdjust(ode_object::mass, 0.2f);
		}
		break;
		case ode_object::SPHERE: {
			// set the mass in the form of a sphere
			ode_object::mass->setSphere(ode_object::density,
				(dReal)ode_object::model->get_radius());
			dMassAdjust(ode_object::mass, 0.2f);

			dMassSetParameters(ode_object::mass, 10.0f, 0.0f, 0.0f, 0.0f,
				16.0f, 16.0f, 16.0f, 0.0f, 0.0f, 0.0f);
			/*cout << "-----" << endl;
			for(unsigned int i = 0; i < 12; i++) {
				cout << ode_object::mass->I[i] << endl;
			}
			cout << "-----" << endl;
			for(unsigned int i = 0; i < 4; i++) {
				cout << ode_object::mass->c[i] << endl;
			}*/
		}
		break;
		// need to be changed to capped cylinder some day ...
		case ode_object::CYLINDER: {
			// set the mass in the form of a cylinder
			// the 3rd parameter specifies the direction (x = 1, y = 2, z = 3)
			ode_object::mass->setCappedCylinder(ode_object::density, 2,
				(dReal)ode_object::model->get_radius(),
				(dReal)ode_object::model->get_length());
			dMassAdjust(ode_object::mass, 0.2f);
		}
		break;
		default:
			m->print(msg::MERROR, "ode_object.cpp", "set_body(): No object type specified!");
			break;
	}

	// set the bodys mass
	dBodySetMass(ode_object::body, ode_object::mass);

	// add the body to the geom
	dGeomSetBody(ode_object::geom, ode_object::body);

	// set the bodys position
	dBodySetPosition(ode_object::body, model->get_position()->x,
		model->get_position()->y, model->get_position()->z);

	// set the bodys data
	dBodySetData(ode_object::body, (void*)this);

	// set the velocity of the body to zero
	dBodySetAngularVel(ode_object::body, 0, 0, 0);
	dBodySetLinearVel(ode_object::body, 0, 0, 0);
}

/*! returns the body
 */
dBodyID ode_object::get_body() {
	return ode_object::body;
}

/*! for debugging purposes - reset the bodys position, rotation and velocity
 */
void ode_object::reset() {
	dBodySetAngularVel(ode_object::body, 0, 0, 0);
	dBodySetLinearVel(ode_object::body, 0, 0, 0);
	dBodySetPosition(ode_object::body, 0, 0, 0);
	//dBodySetRotation(ode_object::body);
	dBodyEnable(ode_object::body);
}

/*! for debugging purposes - sets the objects mass
 */
void ode_object::set_mass(float mass) {
	dMassAdjust(ode_object::mass, mass);
	dBodySetMass(ode_object::body, ode_object::mass);
}


/*! adds a force to the object specified by x, y and z
 *  @param x add force on the x axis
 *  @param y add force on the y axis
 *  @param z add force on the z axis
 */
void ode_object::add_force(float x, float y, float z) {
	const dReal* cur_force = dBodyGetForce(ode_object::body);

	// look if final force is bigger than max allowed force
	float total_cur_force = fabsf(cur_force[0]) + fabsf(cur_force[1]) + fabsf(cur_force[2]);
	float total_add_force = fabsf(x) + fabsf(y) + fabsf(z);
	if(total_cur_force + total_add_force > max_force) {
		// if so, scale force (so it is the max force)
		float diff = max_force - total_cur_force;
		float scale = diff / total_add_force;
		x *= scale;
		y *= scale;
		z *= scale;
	}

	dBodyAddForce(ode_object::body, x, 0.0f, 0.0f);
	dBodyAddForce(ode_object::body, 0.0f, y, 0.0f);
	dBodyAddForce(ode_object::body, 0.0f, 0.0f, z);
}

/*! sets a velocity to the object specified by x, y and z
 *  @param x set velocity on the x axis
 *  @param y set velocity on the y axis
 *  @param z set velocity on the z axis
 */
void ode_object::set_velocity(float x, float y, float z) {
	dBodySetLinearVel(ode_object::body, x, 0.0f, 0.0f);
	dBodySetLinearVel(ode_object::body, 0.0f, y, 0.0f);
	dBodySetLinearVel(ode_object::body, 0.0f, 0.0f, z);
}

/*! sets the maximum force that the object can encounter
 *  @param force the maximal force
 */
void ode_object::set_max_force(float force) {
	ode_object::max_force = force;
}

void ode_object::set_body_rotation(vertex3* rot) {
	dMatrix3 R;
	dRFromAxisAndAngle(R, 1.0f, 0.0f, 0.0f, rot->x);
	dRFromAxisAndAngle(R, 0.0f, 1.0f, 0.0f, rot->y);
	dRFromAxisAndAngle(R, 0.0f, 0.0f, 1.0f, rot->z);
	dBodySetRotation(ode_object::body, R);
}

dReal* ode_object::get_body_rotation() {
	return (dReal*)dBodyGetRotation(body);
}
