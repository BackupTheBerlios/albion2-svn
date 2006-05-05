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

#ifndef __CORE_H__
#define __CORE_H__

#ifdef WIN32
#include <windows.h>
#endif

#include <iostream>
#include <sstream>
#include <string>
#include <cmath>
#include <GL/gl.h>
#include <GL/glu.h>
#include "msg.h"
#include "vertex3.h"
#include "line3.h"
#include "file_io.h"
using namespace std;

#include "win_dll_export.h"

#define PI 3.1415926536

/*! @class core
 *  @brief core stuff
 *  @author flo
 *  @todo more functions, write extra classes for each of these crappy structs ...
 *  
 *  the core stuff
 */

class A2E_API core
{
public:
	core(msg* m, file_io* f);
	~core();

	struct pnt {
		unsigned int x;
		unsigned int y;
	};
	
	struct coord {
		float u;
		float v;
	};

	// remeber that the 'w-float' is the fourth element!
	struct vertex4 {
		float x, y, z, w;
	};

	struct index {
		unsigned int i1;
		unsigned int i2;
		unsigned int i3;
	};

	struct line {
		vertex3 v1;
		vertex3 v2;
	};

	struct triangle {
		vertex3 v1;
		vertex3 v2;
		vertex3 v3;
	};

	struct quad {
		vertex3 v1;
		vertex3 v2;
		vertex3 v3;
		vertex3 v4;
	};

	struct aabbox {
		vertex3 vmin;
		vertex3 vmax;
	};

	struct matrix4 {
		float m[16];
	};

	unsigned int get_bit(unsigned int value, unsigned int bit);
	unsigned int set_bit(unsigned int value, unsigned int bit, unsigned int num);

	float get_distance(vertex3* p1, vertex3* p2);

	bool is_vertex_in_line(line* l1, vertex3* p1, float precision = 1);
	bool is_vertex_in_triangle(triangle* t1, vertex3* p1, float precision = 1);
	bool is_vertex_in_quad(quad* q1, vertex3* p1, float precision = 1);

	/*bool is_line_in_triangle(triangle* t1, line* l1);
	bool is_line_in_quad(quad* t1, line* l1);*/
	bool is_line_in_bbox(aabbox* bbox, line3* l1);

	void vertex_to_line(vertex3 p1, vertex3 p2, line &rline);
	void vertex_to_triangle(vertex3 p1, vertex3 p2, vertex3 p3, triangle &rtriangle);
	void vertex_to_quad(vertex3 p1, vertex3 p2, vertex3 p3, vertex3 p4, quad &rquad);

	float rad_to_deg(float rad);

	void get_2d_from_3d(vertex3* v, pnt* p);
	void get_3d_from_2d(pnt* p, vertex3* v);

	void compute_normal(vertex3* v1, vertex3* v2, vertex3* v3, vertex3& normal);
	void compute_tangent_binormal(vertex3* v1, vertex3* v2, vertex3* v3, vertex3* normal,
		vertex3& binormal, vertex3& tangent,
		coord* t1, coord* t2, coord* t3);
	void compute_normal_tangent_binormal(vertex3* v1, vertex3* v2, vertex3* v3,
		vertex3& normal, vertex3& binormal, vertex3& tangent,
		coord* t1, coord* t2, coord* t3);

	unsigned int get_delimiter_count(char* string, char delim);
	char** tokenize(char* string, char delim);

	void ftoa(float f, char* str);

	bool is_a2eanim(char* filename);

	unsigned int swap_uint(unsigned int u);
	unsigned short int swap_suint(unsigned short int su);

	void put_int(stringstream* sstr, int i);
	void put_sint(stringstream* sstr, short int si);
	void put_uint(stringstream* sstr, unsigned int u);
	void put_suint(stringstream* sstr, unsigned short int su);
	void put_block(stringstream* sstr, const char* data, unsigned int size);

	char get_char(stringstream* sstr);
	int get_int(stringstream* sstr);
	short int get_sint(stringstream* sstr);
	unsigned int get_uint(stringstream* sstr);
	unsigned short int get_suint(stringstream* sstr);
	void get_block(stringstream* sstr, char* data, unsigned int size);
	void get_block(stringstream* sstr, string* data, unsigned int size);

	void reset(stringstream* sstr);

protected:
	unsigned int value, bit, num;
	msg* m;
	file_io* f;

};

#endif
