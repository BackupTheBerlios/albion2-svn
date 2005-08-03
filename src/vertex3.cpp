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

#include "vertex3.h"

/*! creates a vertex3 object
 */
vertex3::vertex3() {
	vertex3::x = 0.0f;
	vertex3::y = 0.0f;
	vertex3::z = 0.0f;
}

/*! creates a vertex3 object
 *  @param x the x value
 *  @param y the y value
 *  @param z the z value
 */
vertex3::vertex3(float x, float y, float z) {
	vertex3::x = x;
	vertex3::y = y;
	vertex3::z = z;
}

/*! creates a vertex3 object
 *  @param v pointer to another vertex3 object, whose values you want to overtake
 */
vertex3::vertex3(vertex3* v) {
	vertex3::x = v->x;
	vertex3::y = v->y;
	vertex3::z = v->z;
}

/*! vertex3 destructor
 */
vertex3::~vertex3() {
}

/*! + operator overload
 */
vertex3 vertex3::operator+(vertex3& v) {
	return vertex3(vertex3::x + v.x, vertex3::y + v.y, vertex3::z + v.z);
}

/*! - operator overload
 */
vertex3 vertex3::operator-(vertex3& v) {
	return vertex3(vertex3::x - v.x, vertex3::y - v.y, vertex3::z - v.z);
}

/*! / operator overload
 */
vertex3 vertex3::operator/(float f) {
	return vertex3(vertex3::x / f, vertex3::y / f, vertex3::z / f);
}

/*! * operator overload
 */
vertex3 vertex3::operator*(float f) {
	return vertex3(vertex3::x * f, vertex3::y * f, vertex3::z * f);
}

/*! += operator overload
 */
vertex3& vertex3::operator+=(vertex3& v) {
	vertex3::x += v.x;
	vertex3::y += v.y;
	vertex3::z += v.z;
	return *this;
}

/*! -= operator overload
 */
vertex3& vertex3::operator-=(vertex3& v) {
	vertex3::x -= v.x;
	vertex3::y -= v.y;
	vertex3::z -= v.z;
	return *this;
}

/*! returns the normalized vertex3 object
 */
vertex3 vertex3::normalize() {
	return *this / (float)sqrt(vertex3::x * vertex3::x + vertex3::y * vertex3::y + vertex3::z * vertex3::z);
}
