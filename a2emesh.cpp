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

#include <iostream>
using namespace std;
#include "a2emesh.h"
#include "msg.h"
#include <math.h>

/*! there is no function currently
 */
a2emesh::a2emesh() {
}

/*! there is no function currently
 */
a2emesh::~a2emesh() {
}

void a2emesh::draw_mesh() {
	glBegin(GL_TRIANGLES);
	for(unsigned int i = 0; i < index_count; i++) {
		glColor4ub(10, 107, 35, 255);
		glVertex3f(indices[i].v1.x, indices[i].v1.y, indices[i].v1.z);
		glColor4ub(2, 121, 33, 255);
		glVertex3f(indices[i].v2.x, indices[i].v2.y, indices[i].v2.z);
		glColor4ub(2, 88, 25, 255);
		glVertex3f(indices[i].v3.x, indices[i].v3.y, indices[i].v3.z);
	}
	glEnd();
}

void a2emesh::load_mesh(char* filename) {
	file.open_file(filename, true);

	// get type and name
	file.get_block(mesh_type, 7);
	mesh_type[8] = 0;
	file.get_block(mesh_name, 8);
	mesh_name[9] = 0;

	// get vertex/index count
	char* vc = (char*)malloc(5);
	char* ic = (char*)malloc(5);
	file.get_block(vc, 4);
	vc[4] = 0;
	file.get_block(ic, 4);
	ic[4] = 0;
	// there seems to be "a bug" with read char stuff, so we have to AND 0xFF
	vertex_count += (unsigned int)((vc[0] & 0xFF)*0x1000000);
	vertex_count += (unsigned int)((vc[1] & 0xFF)*0x10000);
	vertex_count += (unsigned int)((vc[2] & 0xFF)*0x100);
	vertex_count += (unsigned int)(vc[3] & 0xFF);
	index_count += (unsigned int)((ic[0] & 0xFF)*0x1000000);
	index_count += (unsigned int)((ic[1] & 0xFF)*0x10000);
	index_count += (unsigned int)((ic[2] & 0xFF)*0x100);
	index_count += (unsigned int)(ic[3] & 0xFF);
	free(vc);
	free(ic);

	// create vertices
	char vertex[4];
	vertices = (core::vertex*)malloc(sizeof(core::vertex)*vertex_count);
	for(unsigned int i = 0; i < vertex_count; i++) {
		file.get_block(vertex, 4);
		memcpy(&vertices[i].x, vertex, 4);
		file.get_block(vertex, 4);
		memcpy(&vertices[i].y, vertex, 4);
		file.get_block(vertex, 4);
		memcpy(&vertices[i].z, vertex, 4);
	}

	// create indices/triangles
	char index[4];
	unsigned int uindex;
	indices = (core::triangle*)malloc(sizeof(core::triangle)*index_count);
	for(unsigned int i = 0; i < index_count; i++) {
		file.get_block(index, 4);
		uindex = (unsigned int)((index[0] & 0xFF)*0x1000000);
		uindex += (unsigned int)((index[1] & 0xFF)*0x10000);
		uindex += (unsigned int)((index[2] & 0xFF)*0x100);
		uindex += (unsigned int)(index[3] & 0xFF);
		indices[i].v1 = vertices[uindex-1];
		file.get_block(index, 4);
		uindex = (unsigned int)((index[0] & 0xFF)*0x1000000);
		uindex += (unsigned int)((index[1] & 0xFF)*0x10000);
		uindex += (unsigned int)((index[2] & 0xFF)*0x100);
		uindex += (unsigned int)(index[3] & 0xFF);
		indices[i].v2 = vertices[uindex-1];
		file.get_block(index, 4);
		uindex = (unsigned int)((index[0] & 0xFF)*0x1000000);
		uindex += (unsigned int)((index[1] & 0xFF)*0x10000);
		uindex += (unsigned int)((index[2] & 0xFF)*0x100);
		uindex += (unsigned int)(index[3] & 0xFF);
		indices[i].v3 = vertices[uindex-1];
	}

	file.close_file();
}
