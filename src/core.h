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
#include <math.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include "vertex3.h"
using namespace std;

#include "win_dll_export.h"

#define PI 3.1415926536

/*! @class core
 *  @brief core stuff
 *  @author flo
 *  @version 0.1.7
 *  @todo more functions, write extra classes for each of these crappy structs ...
 *  
 *  the core stuff
 */

class A2E_API core
{
public:
	core();
	~core();

	struct pnt {
		unsigned int x;
		unsigned int y;
	};
	
	struct coord {
		float u, v;
	};

	// remeber that the 'w-float' is the fourth element!
	struct vertex4 {
		float x, y, z, w;
	};

	struct index {
		unsigned int i1, i2, i3;
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

	float get_distance(vertex3 p1, vertex3 p2);

	bool is_vertex_in_line(line l1, vertex3 p1, float precision = 1);
	bool is_vertex_in_triangle(triangle t1, vertex3 p1, float precision = 1);
	bool is_vertex_in_quad(quad q1, vertex3 p1, float precision = 1);

	void vertex_to_line(vertex3 p1, vertex3 p2, line &rline);
	void vertex_to_triangle(vertex3 p1, vertex3 p2, vertex3 p3, triangle &rtriangle);
	void vertex_to_quad(vertex3 p1, vertex3 p2, vertex3 p3, vertex3 p4, quad &rquad);

	void mmatrix4_by_vertex4(matrix4 m, vertex4 v);

	float rad_to_deg(float rad);

	void get_2d_from_3d(vertex3* v, pnt* p);

protected:
	unsigned int value, bit, num;
};

#endif
