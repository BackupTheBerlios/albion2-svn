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

#include <iostream>
#include <SDL.h>
#include <math.h>
#include "msg.h"
using namespace std;

#include "win_dll_export.h"

/*! @class core
 *  @brief core stuff
 *  @author flo
 *  @version 0.1.5
 *  @date 2004/09/08
 *  @todo more functions
 *  
 *  the core stuff
 */

class A2E_API core
{
public:
	core();
	~core();

	struct vertex {
		float x, y, z;
	};

	struct index {
		unsigned int i1, i2, i3;
	};

	struct line {
		vertex v1;
		vertex v2;
	};

	struct triangle {
		vertex v1;
		vertex v2;
		vertex v3;
	};

	struct quad {
		vertex v1;
		vertex v2;
		vertex v3;
		vertex v4;
	};

	struct aabbox {
		vertex vmin;
		vertex vmax;
	};

	unsigned int get_bit(unsigned int value, unsigned int bit);
	unsigned int set_bit(unsigned int value, unsigned int bit, unsigned int num);

	float get_distance(vertex p1, vertex p2);

	bool is_vertex_in_line(line l1, vertex p1, float precision = 1);
	bool is_vertex_in_triangle(triangle t1, vertex p1, float precision = 1);
	bool is_vertex_in_quad(quad q1, vertex p1, float precision = 1);

	void vertex_to_line(vertex p1, vertex p2, line &rline);
	void vertex_to_triangle(vertex p1, vertex p2, vertex p3, triangle &rtriangle);
	void vertex_to_quad(vertex p1, vertex p2, vertex p3, vertex p4, quad &rquad);
protected:
	unsigned int value, bit, num;
};

#endif
