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
#include "a2emodel.h"
#include "msg.h"
#include <math.h>

#define MAX_OBJS 32

/*! there is no function currently
 */
a2emodel::a2emodel() {
	a2emodel::position = (core::vertex3*)malloc(sizeof(core::vertex3));
	a2emodel::position->x = 0.0f;
	a2emodel::position->y = 0.0f;
	a2emodel::position->z = 0.0f;

	a2emodel::scale = (core::vertex3*)malloc(sizeof(core::vertex3));
	a2emodel::scale->x = 0.0f;
	a2emodel::scale->y = 0.0f;
	a2emodel::scale->z = 0.0f;

	a2emodel::bbox = NULL;
	a2emodel::vertices = NULL;
	a2emodel::index_count = NULL;
	a2emodel::tex_value = NULL;
	for(unsigned int i = 0; i < MAX_OBJS; i++) {
        a2emodel::tex_names[i] = NULL;
		a2emodel::indices[i] = NULL;
		a2emodel::tex_cords[i] = NULL;
	}

	draw_wireframe = false;
}

/*! there is no function currently
 */
a2emodel::~a2emodel() {
	m.print(msg::MDEBUG, "a2emodel.cpp", "freeing a2emodel stuff");

	free(a2emodel::position);
	free(a2emodel::scale);
	if(a2emodel::bbox) { free(a2emodel::bbox); }
	if(a2emodel::vertices) { free(a2emodel::vertices); }
	if(a2emodel::index_count) { free(a2emodel::index_count); }
	if(a2emodel::tex_value) { free(a2emodel::tex_value); }

	for(unsigned int i = 0; i < MAX_OBJS; i++) {
		if(a2emodel::tex_names[i]) { free(a2emodel::tex_names[i]); }
		if(a2emodel::indices[i]) { free(a2emodel::indices[i]); }
		if(a2emodel::tex_cords[i]) { free(a2emodel::tex_cords[i]); }
	}

	m.print(msg::MDEBUG, "a2emodel.cpp", "a2emodel stuff freed");
}

/*! draws the model
 */
void a2emodel::draw_model() {
	for(unsigned int i = 0; i < object_count; i++) {
		glBindTexture(GL_TEXTURE_2D, textures[tex_value[i]]);

		if(!a2emodel::draw_wireframe) {
			glBegin(GL_TRIANGLES);
			for(unsigned int j = 0; j < index_count[i]; j++) {
				glTexCoord2f(tex_cords[i][j].v1.x, 1.0f - tex_cords[i][j].v1.y);
				glVertex3f(a2emodel::position->x + vertices[indices[i][j].i1].x,
					a2emodel::position->y + vertices[indices[i][j].i1].y,
					a2emodel::position->z + vertices[indices[i][j].i1].z);
				glTexCoord2f(tex_cords[i][j].v2.x, 1.0f - tex_cords[i][j].v2.y);
				glVertex3f(a2emodel::position->x + vertices[indices[i][j].i2].x,
					a2emodel::position->y + vertices[indices[i][j].i2].y,
					a2emodel::position->z + vertices[indices[i][j].i2].z);
				glTexCoord2f(tex_cords[i][j].v3.x, 1.0f - tex_cords[i][j].v3.y);
				glVertex3f(a2emodel::position->x + vertices[indices[i][j].i3].x,
					a2emodel::position->y + vertices[indices[i][j].i3].y,
					a2emodel::position->z + vertices[indices[i][j].i3].z);
			}
			glEnd();
		}
		else {
			glBegin(GL_LINES);
			for(unsigned int j = 0; j < index_count[i]; j++) {
				glTexCoord2f(tex_cords[i][j].v1.x, 1.0f - tex_cords[i][j].v1.y);
				glVertex3f(a2emodel::position->x + vertices[indices[i][j].i1].x,
					a2emodel::position->y + vertices[indices[i][j].i1].y,
					a2emodel::position->z + vertices[indices[i][j].i1].z);
				glTexCoord2f(tex_cords[i][j].v2.x, 1.0f - tex_cords[i][j].v2.y);
				glVertex3f(a2emodel::position->x + vertices[indices[i][j].i2].x,
					a2emodel::position->y + vertices[indices[i][j].i2].y,
					a2emodel::position->z + vertices[indices[i][j].i2].z);
				glTexCoord2f(tex_cords[i][j].v2.x, 1.0f - tex_cords[i][j].v2.y);
				glVertex3f(a2emodel::position->x + vertices[indices[i][j].i2].x,
					a2emodel::position->y + vertices[indices[i][j].i2].y,
					a2emodel::position->z + vertices[indices[i][j].i2].z);
				glTexCoord2f(tex_cords[i][j].v3.x, 1.0f - tex_cords[i][j].v3.y);
				glVertex3f(a2emodel::position->x + vertices[indices[i][j].i3].x,
					a2emodel::position->y + vertices[indices[i][j].i3].y,
					a2emodel::position->z + vertices[indices[i][j].i3].z);
				glTexCoord2f(tex_cords[i][j].v1.x, 1.0f - tex_cords[i][j].v1.y);
				glVertex3f(a2emodel::position->x + vertices[indices[i][j].i1].x,
					a2emodel::position->y + vertices[indices[i][j].i1].y,
					a2emodel::position->z + vertices[indices[i][j].i1].z);
				glTexCoord2f(tex_cords[i][j].v3.x, 1.0f - tex_cords[i][j].v3.y);
				glVertex3f(a2emodel::position->x + vertices[indices[i][j].i3].x,
					a2emodel::position->y + vertices[indices[i][j].i3].y,
					a2emodel::position->z + vertices[indices[i][j].i3].z);
			}
			glEnd();
		}
	}
}

/*! loads the textures of the model - .png preferred!
 */
void a2emodel::load_textures() {
	SDL_Surface* tex_surface[MAX_OBJS];

	for(unsigned int i = 0; i < texture_count; i++) {
		tex_surface[i] = IMG_LoadPNG_RW(SDL_RWFromFile(tex_names[i], "rb"));
		if(!tex_surface) {
			m.print(msg::MERROR, "a2emodel.cpp", "error loading texture file \"%s\"!", tex_names[i]);
			return;
		}
	}

	glGenTextures(texture_count, &textures[0]);
	for(unsigned int i = 0; i < texture_count; i++) {
		glBindTexture(GL_TEXTURE_2D, textures[i]);	

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);

		gluBuild2DMipmaps(GL_TEXTURE_2D, 3, tex_surface[i]->w, tex_surface[i]->h,
			GL_RGB, GL_UNSIGNED_BYTE, tex_surface[i]->pixels);
	}

	for(unsigned int i = 0; i < texture_count; i++) {
		if(tex_surface[i]) {
			SDL_FreeSurface(tex_surface[i]);
		}
	}
}

/*! loads a .a2m model file
 *  @param filename the name of the .a2m model file
 */
void a2emodel::load_model(char* filename) {
	file.open_file(filename, true);

	// get type and name
	file.get_block(model_type, 8);
	model_type[9] = 0;
	file.get_block(model_name, 8);
	model_name[9] = 0;

	// get vertex3 count
	char* vc = (char*)malloc(5);
	file.get_block(vc, 4);
	vc[4] = 0;

	// there seems to be "a bug" with read char stuff, so we have to AND 0xFF
	vertex_count = 0;
	vertex_count += (unsigned int)((vc[0] & 0xFF)*0x1000000);
	vertex_count += (unsigned int)((vc[1] & 0xFF)*0x10000);
	vertex_count += (unsigned int)((vc[2] & 0xFF)*0x100);
	vertex_count += (unsigned int)(vc[3] & 0xFF);
	free(vc);

	// create vertices
	char vertex[4];
	vertices = (core::vertex3*)malloc(sizeof(core::vertex3)*vertex_count);
	for(unsigned int i = 0; i < vertex_count; i++) {
		file.get_block(vertex, 4);
		memcpy(&vertices[i].x, vertex, 4);
		file.get_block(vertex, 4);
		memcpy(&vertices[i].y, vertex, 4);
		file.get_block(vertex, 4);
		memcpy(&vertices[i].z, vertex, 4);
	}

	// get texture count
	char* tc = (char*)malloc(5);
	file.get_block(tc, 4);
	tc[4] = 0;

	// there seems to be "a bug" with read char stuff, so we have to AND 0xFF
	texture_count = 0;
	texture_count += (unsigned int)((tc[0] & 0xFF)*0x1000000);
	texture_count += (unsigned int)((tc[1] & 0xFF)*0x10000);
	texture_count += (unsigned int)((tc[2] & 0xFF)*0x100);
	texture_count += (unsigned int)(tc[3] & 0xFF);
	free(tc);

	// get texture names
	for(unsigned int i = 0; i < texture_count; i++) {
		tex_names[i] = (char*)malloc(33);
		for(unsigned int j = 0; j < 33; j++) {
			tex_names[i][j] = 0;
		}

		file.get_block(tex_names[i], 32);
		tex_names[33] = 0;
	}

	a2emodel::load_textures();

	// get object count
	char* oc = (char*)malloc(5);
	file.get_block(oc, 4);
	oc[4] = 0;

	// there seems to be "a bug" with read char stuff, so we have to AND 0xFF
	object_count = 0;
	object_count += (unsigned int)((oc[0] & 0xFF)*0x1000000);
	object_count += (unsigned int)((oc[1] & 0xFF)*0x10000);
	object_count += (unsigned int)((oc[2] & 0xFF)*0x100);
	object_count += (unsigned int)(oc[3] & 0xFF);
	free(oc);

	// init stuff
	index_count = (unsigned int*)malloc(sizeof(unsigned int)*object_count);
	for(unsigned int i = 0; i < object_count; i++) {
		index_count[i] = 0;
	}

	tex_value = (unsigned int*)malloc(sizeof(unsigned int)*object_count);
	for(unsigned int i = 0; i < object_count; i++) {
		tex_value[i] = 0;
	}

	// loop and load "sub"-objects
	for(unsigned int i = 0; i < object_count; i++) {
		// get object names
		obj_names[i] = (char*)malloc(9);
		for(unsigned int j = 0; j < 9; j++) {
			obj_names[i][j] = 0;
		}

		file.get_block(obj_names[i], 8);
		obj_names[9] = 0;

		// get index count
		char* ic = (char*)malloc(5);
		file.get_block(ic, 4);
		ic[4] = 0;

		// there seems to be "a bug" with read char stuff, so we have to AND 0xFF
		index_count[i] += (unsigned int)((ic[0] & 0xFF)*0x1000000);
		index_count[i] += (unsigned int)((ic[1] & 0xFF)*0x10000);
		index_count[i] += (unsigned int)((ic[2] & 0xFF)*0x100);
		index_count[i] += (unsigned int)(ic[3] & 0xFF);
		free(ic);

		// get texture value
		char* tv = (char*)malloc(5);
		file.get_block(tv, 4);
		tv[4] = 0;

		// there seems to be "a bug" with read char stuff, so we have to AND 0xFF
		tex_value[i] += (unsigned int)((tv[0] & 0xFF)*0x1000000);
		tex_value[i] += (unsigned int)((tv[1] & 0xFF)*0x10000);
		tex_value[i] += (unsigned int)((tv[2] & 0xFF)*0x100);
		tex_value[i] += (unsigned int)(tv[3] & 0xFF);
		free(tv);

		// create indices/triangles
		char index[4];
		unsigned int uindex;
		indices[i] = (core::index*)malloc(sizeof(core::index)*index_count[i]);
		for(unsigned int j = 0; j < index_count[i]; j++) {
			file.get_block(index, 4);
			uindex = (unsigned int)((index[0] & 0xFF)*0x1000000);
			uindex += (unsigned int)((index[1] & 0xFF)*0x10000);
			uindex += (unsigned int)((index[2] & 0xFF)*0x100);
			uindex += (unsigned int)(index[3] & 0xFF);
			indices[i][j].i1 = uindex;
			file.get_block(index, 4);
			uindex = (unsigned int)((index[0] & 0xFF)*0x1000000);
			uindex += (unsigned int)((index[1] & 0xFF)*0x10000);
			uindex += (unsigned int)((index[2] & 0xFF)*0x100);
			uindex += (unsigned int)(index[3] & 0xFF);
			indices[i][j].i2 = uindex;
			file.get_block(index, 4);
			uindex = (unsigned int)((index[0] & 0xFF)*0x1000000);
			uindex += (unsigned int)((index[1] & 0xFF)*0x10000);
			uindex += (unsigned int)((index[2] & 0xFF)*0x100);
			uindex += (unsigned int)(index[3] & 0xFF);
			indices[i][j].i3 = uindex;
		}

		// create texture coordinates
		//char vertex3[4];
		tex_cords[i] = (core::triangle*)malloc(sizeof(core::triangle)*index_count[i]);
		for(unsigned int j = 0; j < index_count[i]; j++) {
			file.get_block(vertex, 4);
			memcpy(&tex_cords[i][j].v1.x, vertex, 4);
			file.get_block(vertex, 4);
			memcpy(&tex_cords[i][j].v1.y, vertex, 4);
			file.get_block(vertex, 4);
			memcpy(&tex_cords[i][j].v1.z, vertex, 4);

			file.get_block(vertex, 4);
			memcpy(&tex_cords[i][j].v2.x, vertex, 4);
			file.get_block(vertex, 4);
			memcpy(&tex_cords[i][j].v2.y, vertex, 4);
			file.get_block(vertex, 4);
			memcpy(&tex_cords[i][j].v2.z, vertex, 4);

			file.get_block(vertex, 4);
			memcpy(&tex_cords[i][j].v3.x, vertex, 4);
			file.get_block(vertex, 4);
			memcpy(&tex_cords[i][j].v3.y, vertex, 4);
			file.get_block(vertex, 4);
			memcpy(&tex_cords[i][j].v3.z, vertex, 4);
		}
	}

	file.close_file();

	a2emodel::build_bounding_box();
}

/*! sets the position of the model
 *  @param x the x coordinate
 *  @param y the y coordinate
 *  @param z the z coordinate
 */
void a2emodel::set_position(float x, float y, float z) {
	a2emodel::position->x = x;
	a2emodel::position->y = y;
	a2emodel::position->z = z;
}

/*! returns the position of the model
 */
core::vertex3* a2emodel::get_position() {
	return a2emodel::position;
}

/*! sets the scale of the model
 *  @param x the x scale
 *  @param y the y scale
 *  @param z the z scale
 */
void a2emodel::set_scale(float x, float y, float z) {
	a2emodel::scale->x = x;
	a2emodel::scale->y = y;
	a2emodel::scale->z = z;

	for(unsigned int i = 0; i < a2emodel::vertex_count; i++) {
		a2emodel::vertices[i].x *= a2emodel::scale->x;
		a2emodel::vertices[i].y *= a2emodel::scale->y;
		a2emodel::vertices[i].z *= a2emodel::scale->z;
	}
}

/*! returns the scale of the model
 */
core::vertex3* a2emodel::get_scale() {
	return a2emodel::scale;
}

/*! sets a texture of the model to a new one
 *  @param texture the texture data
 *  @param num the number of the texture that you want to replace
 */
void a2emodel::set_texture(GLuint texture, unsigned int num) {
	a2emodel::textures[num] = texture;
}

/*! returns a pointer to the vertices
 */
core::vertex3* a2emodel::get_vertices() {
	return a2emodel::vertices;
}

/*! returns a pointer to the indices
 */
core::index* a2emodel::get_indices() {
	unsigned int total_size = 0;
	for(unsigned int i = 0; i < object_count; i++) {
		total_size += index_count[i] * sizeof(core::index);
	}

	core::index* total_indices = (core::index*)malloc(total_size);

	unsigned int cidx = 0;
	for(unsigned int i = 0; i < object_count; i++) {
		for(unsigned int j = 0; j < index_count[i]; j++) {
			total_indices[cidx].i1 = indices[i][j].i1;
			total_indices[cidx].i2 = indices[i][j].i2;
			total_indices[cidx].i3 = indices[i][j].i3;
			cidx++;
		}
	}

	return total_indices;
}

/*! returns the vertex count
 */
unsigned int a2emodel::get_vertex_count() {
	return a2emodel::vertex_count;
}

/*! returns the index count
 */
unsigned int a2emodel::get_index_count() {
	return *a2emodel::index_count;
}

/*! builds the bounding box
 */
void a2emodel::build_bounding_box() {
	float minx, miny, minz, maxx, maxy, maxz;
	minx = vertices[0].x;
	miny = vertices[0].y;
	minz = vertices[0].z;
	maxx = vertices[0].x;
	maxy = vertices[0].y;
	maxz = vertices[0].z;
	for(unsigned int i = 1; i < vertex_count; i++) {
		if(vertices[i].x < minx) {
			minx = vertices[i].x;
		}
		if(vertices[i].y < miny) {
			miny = vertices[i].y;
		}
		if(vertices[i].z < minz) {
			minz = vertices[i].z;
		}

		if(vertices[i].x > maxx) {
			maxx = vertices[i].x;
		}
		if(vertices[i].y > maxy) {
			maxy = vertices[i].y;
		}
		if(vertices[i].z > maxz) {
			maxz = vertices[i].z;
		}
	}

	if(a2emodel::bbox) { free(a2emodel::bbox); }
	a2emodel::bbox = (core::aabbox*)malloc(sizeof(core::aabbox));
	a2emodel::bbox->vmin.x = minx;
	a2emodel::bbox->vmin.y = miny;
	a2emodel::bbox->vmin.z = minz;
	a2emodel::bbox->vmax.x = maxx;
	a2emodel::bbox->vmax.y = maxy;
	a2emodel::bbox->vmax.z = maxz;
}

/*! returns the bounding box of the model
 */
core::aabbox* a2emodel::get_bounding_box() {
	return a2emodel::bbox;
}

/*! used for ode collision - sets the radius of an sphere/cylinder object
 *  @param radius the objects radius
 */
void a2emodel::set_radius(float radius) {
	a2emodel::radius = radius;
}

/*! used for ode collision - returns the radius of an sphere/cylinder object
 */
float a2emodel::get_radius() {
	return a2emodel::radius;
}

/*! used for ode collision - sets the length of an cylinder object
 *  @param length the objects length
 */
void a2emodel::set_length(float length) {
	a2emodel::length = length;
}

/*! used for ode collision - returns the length of an cylinder object
 */
float a2emodel::get_length() {
	return a2emodel::length;
}

/*! sets the bool if the model is drawn as a wireframe
 *  @param state the new state
 */
void a2emodel::set_draw_wireframe(bool state) {
	a2emodel::draw_wireframe = state;
}

/*! returns a true if the model is drawn as a wireframe
 */
bool a2emodel::get_draw_wireframe() {
	return a2emodel::draw_wireframe;
}
