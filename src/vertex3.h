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
 
#ifndef __VERTEX3_H__
#define __VERTEX3_H__

#include <iostream>
#include <cmath>
using namespace std;

#include "win_dll_export.h"

/*! @class vertex3
 *  @brief vertex3 routines
 *  @author flo
 *  @version 0.1
 *  @date 2005/05/06
 *  @todo nothing atm
 *  
 *  the vertex3 class
 */

class A2E_API vertex3
{
public:
	float x;
	float y;
	float z;

	vertex3();
	vertex3(float x, float y, float z);
	vertex3(vertex3* v);
	~vertex3();

	// overloading routines ...
	vertex3 operator+(vertex3 &v);
	vertex3 operator-(vertex3 &v);
	vertex3 operator/(float f);
	vertex3 operator*(float f);
	vertex3& operator+=(vertex3 &v);
	vertex3& operator-=(vertex3 &v);

	vertex3 normalize();

};

#endif
