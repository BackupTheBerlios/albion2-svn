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
 *  @version 0.1.3
 *  @date 2004/07/20
 *  @todo more functions
 *  
 *  the core stuff
 */

class A2E_API core
{
public:
	core();
	~core();

	struct vector3 {
		float x;
		float y;
		float z;
	};

	struct line {
		vector3 v1;
		vector3 v2;
	};

	struct triangle {
		vector3 v1;
		vector3 v2;
		vector3 v3;
	};

	struct quad {
		vector3 v1;
		vector3 v2;
		vector3 v3;
		vector3 v4;
	};

	unsigned int get_bit(unsigned int value, unsigned int bit);
	unsigned int set_bit(unsigned int value, unsigned int bit, unsigned int num);

	float get_distance(vector3 p1, vector3 p2);

	bool is_vector_in_line(line l1, vector3 p1, float precision = 1);
	bool is_vector_in_triangle(triangle t1, vector3 p1, float precision = 1);
	bool is_vector_in_quad(quad q1, vector3 p1, float precision = 1);

	void vec_to_line(vector3 p1, vector3 p2, line &rline);
	void vec_to_triangle(vector3 p1, vector3 p2, vector3 p3, triangle &rtriangle);
	void vec_to_quad(vector3 p1, vector3 p2, vector3 p3, vector3 p4, quad &rquad);
protected:
	unsigned int value, bit, num;
};

#endif
