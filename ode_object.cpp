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
float ode_object::density = 1.0f;

/*! there is no function currently
 */
ode_object::ode_object(dWorldID* world, dSpaceID* space, a2emodel* model, bool fixed, ode_object::OTYPE type) {
	ode_object::world = world;
	ode_object::space = space;
	ode_object::set_model(model);
	ode_object::set_geom(ode_object::get_model(), type);

	if(!fixed) {
		ode_object::mass = new dMass;
		ode_object::set_body(type);
	}
}

/*! there is no function currently
 */
ode_object::~ode_object() {
	// destroy the geom
	if(ode_object::geom) { dGeomDestroy(ode_object::geom); }

	// destroy the body
	if(ode_object::body) { dBodyDestroy(ode_object::body); }

	// delete the mass
	if(ode_object::mass) { delete ode_object::mass; }

	// destroy the trimesh
	if(ode_object::trimesh) { dGeomTriMeshDataDestroy(ode_object::trimesh); }

	// delete the vertices
	if(ode_object::dvertices) { delete [] ode_object::dvertices; }

	// and free the indices
	if(ode_object::dindices) { free(ode_object::dindices); }
}

/*! sets the geometry of the object
 *  @param model a pointer to the model object
 */
void ode_object::set_geom(a2emodel* model, ode_object::OTYPE type) {
	switch(type) {
		case ode_object::TRIMESH: {
			// initialize pointers to our a2emodel stuff
			core::vertex* vertices = model->get_vertices();
			core::index* indices = model->get_indices();
			ode_object::vertex_count = model->get_vertex_count();
			ode_object::index_count = model->get_index_count();

			// create trimesh data id
			ode_object::trimesh = dGeomTriMeshDataCreate();

			// because ode uses another vertex/index store format,
			// we need to convert the a2emodel vertices/indices
			ode_object::dvertices = new dVector3[vertex_count];
			for(unsigned int i = 0; i < ode_object::vertex_count; i++) {
				ode_object::dvertices[i][0] = vertices[i].x;
				ode_object::dvertices[i][1] = vertices[i].y;
				ode_object::dvertices[i][2] = vertices[i].z;
			}

			ode_object::dindices = (unsigned int*)malloc(sizeof(unsigned int)*3*ode_object::index_count);
			for(unsigned int i = 0; i < ode_object::index_count; i++) {
				ode_object::dindices[i] = indices[i].i1;
				i++;
				ode_object::dindices[i] = indices[i].i2;
				i++;
				ode_object::dindices[i] = indices[i].i3;
			}

			// and now build a simple trimesh out of the model vertices and indices
			dGeomTriMeshDataBuildSimple(ode_object::trimesh, (dReal*)ode_object::dvertices,
				ode_object::vertex_count, (const int*)ode_object::dindices,
				ode_object::index_count);

			// and finally create the tri mesh geom
			ode_object::geom = dCreateTriMesh(*ode_object::space, ode_object::trimesh, 0, 0, 0);

			// set the geoms position
			dGeomSetPosition(ode_object::geom, model->get_position()->x,
				model->get_position()->y, model->get_position()->z);

			// set the geoms data
			dGeomSetData(ode_object::geom, (void*)this);

			// set its body to 0
			dGeomSetBody(ode_object::geom, 0);
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
			m.print(msg::MERROR, "ode_object.cpp", "set_geom(): No object type specified!");
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
			dMassAdjust(ode_object::mass, 0.25);
		}
		break;
		case ode_object::SPHERE: {
			// set the mass in the form of a sphere
			ode_object::mass->setSphere(ode_object::density,
				(dReal)ode_object::model->get_radius());
			dMassAdjust(ode_object::mass, 0.25);
		}
		break;
		// need to be changed to capped cylinder some day ...
		case ode_object::CYLINDER: {
			// set the mass in the form of a cylinder
			// the 3rd parameter specifies the direction (x = 1, y = 2, z = 3)
			ode_object::mass->setCappedCylinder(ode_object::density, 2,
				(dReal)ode_object::model->get_radius(),
				(dReal)ode_object::model->get_length());
			dMassAdjust(ode_object::mass, 0.25);
		}
		break;
		default:
			m.print(msg::MERROR, "ode_object.cpp", "set_body(): No object type specified!");
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
