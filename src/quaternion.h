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
 
#ifndef __QUATERNION_H__
#define __QUATERNION_H__

#include <iostream>
#include "core.h"
#include "vertex3.h"
#include <cmath>
using namespace std;

#include "win_dll_export.h"

/*! @class quaternion
 *  @brief quaternion routines
 *  @author flo
 *  @version 0.1
 *  @todo nothing atm
 *  
 *  the quaternion class
 *  thx to julien rebetez for that quaternion stuff ;)
 */

class A2E_API quaternion
{
public:
	float r;
	float x;
	float y;
	float z;

 	quaternion();
	quaternion(float r, float x, float y, float z);
	quaternion(quaternion& q);
	quaternion(float f, vertex3 v);
 	~quaternion();

	quaternion operator=(quaternion& q);
	quaternion operator+(quaternion& q);
	quaternion operator-(quaternion& q);
	quaternion operator*(quaternion& q);
	quaternion operator*(float f);
	quaternion operator/(quaternion& q);
	quaternion operator+=(quaternion& q);
	quaternion operator-=(quaternion& q);
	quaternion operator*=(quaternion& q);
	quaternion operator*=(float f);
	quaternion operator/=(quaternion& q);
	quaternion operator/=(float f);

	void reset();
	float magnitude();
	void normalize();
	void compute_r();

	quaternion inverse();
	quaternion conjugate();
	quaternion unit();

	vertex3 rotate(vertex3& v);
	void set_rotation(float a, vertex3 tv);
	void set_rotation(float a, float i, float j, float k);
};

#endif
