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

#ifndef __A2EMESH_H__
#define __A2EMESH_H__

#ifdef WIN32
#include <windows.h>
#endif

#define MAX_OBJS 32

#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <math.h>
#include <gl\gl.h>
#include <gl\glu.h>
#include "msg.h"
#include "core.h"
#include "file_io.h"
using namespace std;

#include "win_dll_export.h"

/*! @class a2emesh
 *  @brief class for loading and displaying an a2e mesh
 *  @author flo
 *  @version 0.1.1
 *  @date 2004/09/08
 *  @todo more functions
 *  
 *  the a2emesh class
 */

class A2E_API a2emesh
{
public:
	a2emesh();
	~a2emesh();

	void load_mesh(char* filename);
	void draw_mesh();
	void load_textures();
	void set_position(float x, float y, float z);
	core::vertex* get_position();

	// for debug purposes
	void set_texture(GLuint texture, unsigned int num);

protected:
	msg m;
	file_io file;
	core c;

	char mesh_type[8];
	char mesh_name[9];
	unsigned int vertex_count;
	core::vertex* vertices;
	unsigned int texture_count;
	char* tex_names[MAX_OBJS];
	unsigned int object_count;
	char* obj_names[MAX_OBJS];
	unsigned int* index_count;
	unsigned int* tex_value;
	core::triangle* indices[MAX_OBJS];
	core::triangle* tex_cords[MAX_OBJS];

	GLuint textures[MAX_OBJS];

	core::vertex* position;
};

#endif
