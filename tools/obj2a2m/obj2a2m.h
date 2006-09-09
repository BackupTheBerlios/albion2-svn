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

#ifndef __OBJ2A2M_H__
#define __OBJ2A2M_H__

#ifdef WIN32
#include <windows.h>
#endif

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <core.h>
#include <msg.h>
#include <file_io.h>
#include <vertex3.h>
using namespace std;

vector<vertex3> vertices;
vector<vertex3> ivertices;
vector<core::coord> tex_coords;
vector<vector<core::index> > indices;
vector<string> obj_names;
unsigned int object_count = 0;
unsigned int cur_object = 0;
bool ginit = false;
bool blender = false;

vector<vertex3> col_vertices;
vector<core::index> col_indices;
bool collision = false;
unsigned int col_object = -1;
unsigned int col_vnum = -1;
unsigned int col_vertex_count = 0;
unsigned int col_tex_count = 0;

#endif
