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

#define MAX_OBJS 32

#ifdef WIN32
#include <windows.h>
#endif

#include <iostream>
#include <fstream>
using namespace std;

void load_materials(char* filename);

char model_name[9];
char* obj_names[MAX_OBJS];
unsigned int object_count = 0;
unsigned int vertex_count = 0;
unsigned int tex_vertex_count = 0;
unsigned int normal_vertex_count = 0;
unsigned int total_index_count = 0;
unsigned int* index_count;
unsigned int* vertices1;
unsigned int* vertices2;
unsigned int* vertices3;
unsigned int* indices1[MAX_OBJS];
unsigned int* indices2[MAX_OBJS];
unsigned int* indices3[MAX_OBJS];
unsigned int* texindices1[MAX_OBJS];
unsigned int* texindices2[MAX_OBJS];
unsigned int* texindices3[MAX_OBJS];
unsigned int* tex_number;
unsigned int* texcords1;
unsigned int* texcords2;

unsigned int material_count = 0;
unsigned int texture_count = 0;
char* mat_names[32];
char* tex_names[32];

#endif
