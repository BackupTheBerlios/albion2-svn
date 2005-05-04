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

#ifndef __A2EMODEL_H__
#define __A2EMODEL_H__

#ifdef WIN32
#include <windows.h>
#endif

#define MAX_OBJS 32

#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <math.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include "msg.h"
#include "core.h"
#include "file_io.h"
using namespace std;

#include "win_dll_export.h"

/*! @class a2emodel
 *  @brief class for loading and displaying an a2e model
 *  @author flo
 *  @version 0.2
 *  @date 2005/04/22
 *  @todo more functions
 *  
 *  the a2emodel class
 */

class A2E_API a2emodel
{
public:
	a2emodel();
	~a2emodel();

	void load_model(char* filename);
	void draw_model();
	void load_textures();
	void set_position(float x, float y, float z);
	core::vertex3* get_position();
	void set_scale(float x, float y, float z);
	core::vertex3* get_scale();
	void set_rotation(float x, float y, float z);
	core::vertex3* get_rotation();

	core::vertex3* get_vertices();
	core::index* get_indices();
	unsigned int get_vertex_count();
	unsigned int get_index_count();

	void build_bounding_box();
	core::aabbox* get_bounding_box();

	void set_draw_wireframe(bool state);
	bool get_draw_wireframe();

	void set_visible(bool state);
	bool get_visible();

	// stuff for collision detection
	void set_radius(float radius);
	void set_length(float length);
	float get_radius();
	float get_length();

	// for debug purposes
	void set_texture(GLuint texture, unsigned int num);

	void draw_normals();

protected:
	msg m;
	file_io file;
	core c;

	char model_type[9];
	char model_name[9];
	unsigned int vertex_count;
	core::vertex3* vertices;
	unsigned int texture_count;
	char* tex_names[MAX_OBJS];
	unsigned int normal_count;
	unsigned int object_count;
	char* obj_names[MAX_OBJS];
	unsigned int* index_count;
	unsigned int* tex_value;
	core::index* indices[MAX_OBJS];
	core::triangle* tex_cords[MAX_OBJS];
	core::triangle* normals[MAX_OBJS];

	GLuint textures[MAX_OBJS];

	core::vertex3* position;
	core::vertex3* scale;
	core::vertex3* rotation;

	core::aabbox* bbox;

	bool draw_wireframe;

	bool visible;

	// some variables for collision detection
	float radius;
	float length;
};

#endif
