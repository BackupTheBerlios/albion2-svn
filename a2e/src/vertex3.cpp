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

/*! = operator overload
 */
vertex3 vertex3::operator=(const vertex3& v) {
	vertex3::x = v.x;
	vertex3::y = v.y;
	vertex3::z = v.z;
	return *this;
}

/*! + operator overload
 */
vertex3 vertex3::operator+(const vertex3& v) {
	return vertex3(vertex3::x + v.x, vertex3::y + v.y, vertex3::z + v.z);
}

/*! - operator overload
 */
vertex3 vertex3::operator-(const vertex3& v) {
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
vertex3& vertex3::operator+=(const vertex3& v) {
	vertex3::x += v.x;
	vertex3::y += v.y;
	vertex3::z += v.z;
	return *this;
}

/*! -= operator overload
 */
vertex3& vertex3::operator-=(const vertex3& v) {
	vertex3::x -= v.x;
	vertex3::y -= v.y;
	vertex3::z -= v.z;
	return *this;
}

/*! *= operator overload
 */
vertex3& vertex3::operator*=(float f) {
	vertex3::x *= f;
	vertex3::y *= f;
	vertex3::z *= f;
	return *this;
}

/*! *= operator overload
 */
vertex3& vertex3::operator*=(const vertex3& v) {
	vertex3::x *= v.x;
	vertex3::y *= v.y;
	vertex3::z *= v.z;
	return *this;
}

vertex3& vertex3::operator*=(const matrix4& mat) {
	vertex3 v3;
	v3.x = mat.m[0][0] * vertex3::x + mat.m[0][1] * vertex3::y + mat.m[0][2] * vertex3::z + mat.m[0][3] * 1.0f;
	v3.y = mat.m[1][0] * vertex3::x + mat.m[1][1] * vertex3::y + mat.m[1][2] * vertex3::z + mat.m[1][3] * 1.0f;
	v3.z = mat.m[2][0] * vertex3::x + mat.m[2][1] * vertex3::y + mat.m[2][2] * vertex3::z + mat.m[2][3] * 1.0f;
	memcpy(this, &v3, sizeof(vertex3));
	return *this;
}

/*! /= operator overload
 */
vertex3& vertex3::operator/=(float f) {
	vertex3::x /= f;
	vertex3::y /= f;
	vertex3::z /= f;
	return *this;
}

/*! * operator overload / cross product (^=)
 */
vertex3& vertex3::operator^=(const vertex3& v) {
	float tmp1, tmp2;
	tmp1 = vertex3::y * v.z - vertex3::z * v.y;
	tmp2 = vertex3::z * v.x - vertex3::x * v.z;
	vertex3::z = vertex3::x * v.y - vertex3::y * v.x;
	vertex3::x = tmp1;
	vertex3::y = tmp2;
	return *this;
}

/*! == operator overload
 */
bool vertex3::operator==(const vertex3& v) {
	if(vertex3::x == v.x && vertex3::y == v.y && vertex3::z == v.z) {
		return true;
	}
	return false;
}

/*! != operator overload
 */
bool vertex3::operator!=(const vertex3& v) {
	if(vertex3::x == v.x && vertex3::y == v.y && vertex3::z == v.z) {
		return false;
	}
	return true;
}

/*! << operator overload
 */
A2E_API ostream& operator<<(ostream& o, vertex3& v) {
	o << "(" << v.x << ", " << v.y << ", " << v.z << ")";
	return o;
}

/*! returns the normalized vertex3 object
 */
vertex3 vertex3::normalize() {
	return *this / (float)sqrt(vertex3::x * vertex3::x + vertex3::y * vertex3::y + vertex3::z * vertex3::z);
}

/*! normalizes the vertex3 object
 */
void vertex3::norm() {
	*this = *this / (float)sqrt(vertex3::x * vertex3::x + vertex3::y * vertex3::y + vertex3::z * vertex3::z);
}

/*! * operator overload / cross product
 */
vertex3 vertex3::operator^(const vertex3& v) {
	return vertex3(vertex3::y * v.z - vertex3::z * v.y,
		vertex3::z * v.x - vertex3::x * v.z,
		vertex3::x * v.y - vertex3::y * v.x);
}

/*! * operator overload / dot product
 */
float vertex3::operator*(vertex3& v) {
	return (vertex3::x * v.x + vertex3::y * v.y + vertex3::z * v.z);
}

/*! adjusts the vertex3 object by another vertex3 object
 */
void vertex3::adjust(vertex3* v) {
	vertex3::x *= v->x;
	vertex3::y *= v->y;
	vertex3::z *= v->z;
}

/*! sets the x, y and z value of the vertex to the specified parameters
 *  @param x the x value
 *  @param y the y value
 *  @param z the z value
 */
void vertex3::set(float x, float y, float z) {
	vertex3::x = x;
	vertex3::y = y;
	vertex3::z = z;
}

/*! sets the x, y and z value of the vertex to the specified parameters
 *  @param v the vertex from which we want to overtake the x, y and z value
 */
void vertex3::set(vertex3* v) {
	vertex3::x = v->x;
	vertex3::y = v->y;
	vertex3::z = v->z;
}
