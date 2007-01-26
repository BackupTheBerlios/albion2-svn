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

#ifndef __MATRIX4_H__
#define __MATRIX4_H__

#include <iostream>
#include <cmath>
using namespace std;

#include "win_dll_export.h"

/*! @class matrix4
 *  @brief matrix4 routines
 *  @author flo
 *  @todo nothing atm
 *
 *  the matrix4 class
 */

class A2E_API matrix4
{
public:
	float m[4][4];

	matrix4();
	matrix4(float m0, float m1, float m2, float m3,
			float m4, float m5, float m6, float m7,
			float m8, float m9, float m10, float m11,
			float m12, float m13, float m14, float m15);
	matrix4(matrix4* mat);
	~matrix4();

	matrix4 operator*(matrix4& mat);
	matrix4& operator*=(const matrix4& mat);
	A2E_API friend ostream& operator<<(ostream& o, matrix4& v);

	void rotate(float x, float y);
	void rotate(float x, float y, float z);
	void rotate_x(float x);
	void rotate_y(float y);
	void rotate_z(float z);
	void translate(float x, float y, float z);
	void scale(float x, float y, float z);
	void invert();
	void identity();

};

#endif
