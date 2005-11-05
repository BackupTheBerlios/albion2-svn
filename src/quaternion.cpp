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

#include "quaternion.h"

/*! creates a quaternion object
 */
quaternion::quaternion() {
	quaternion::r = 1.0f;
	quaternion::x = 0.0f;
	quaternion::y = 0.0f;
	quaternion::z = 0.0f;
}

/*! creates a quaternion object
 *  @param r the r value
 *  @param x the x value
 *  @param y the y value
 *  @param z the z value
 */
quaternion::quaternion(float r, float x, float y, float z) {
	quaternion::r = r;
	quaternion::x = x;
	quaternion::y = y;
	quaternion::z = z;
}

/*! creates a quaternion object
 *  @param q another quaternion that you want to copy
 */
quaternion::quaternion(const quaternion& q) {
	quaternion::r = q.r;
	quaternion::x = q.x;
	quaternion::y = q.y;
	quaternion::z = q.z;
}

/*! creates a quaternion object
 *  @param f resize float?
 *  @param v vertex of the rotation
 */
quaternion::quaternion(float f, vertex3 v) {
	quaternion::set_rotation(f, v);
}

/*! quaternion destructor
 */
quaternion::~quaternion() {
}

/*! = operator overload
 */
quaternion quaternion::operator=(const quaternion& q) {
	quaternion::r = q.r;
	quaternion::x = q.x;
	quaternion::y = q.y;
	quaternion::z = q.z;
	return (*this);
}

/*! + operator overload
 */
quaternion quaternion::operator+(const quaternion& q) {
	return quaternion(r + q.r, x + q.x, y + q.y, z + q.z);
}

/*! - operator overload
 */
quaternion quaternion::operator-(const quaternion& q) {
	return quaternion(quaternion::r - q.r, quaternion::x - q.x, quaternion::y - q.y, quaternion::z - q.z);
}

/*! * operator overload
 */
quaternion quaternion::operator*(const quaternion& q) {
	return quaternion(quaternion::r * q.r - quaternion::x * q.x - quaternion::y * q.y - quaternion::z * q.z, 
				quaternion::r * q.x + quaternion::x * q.r + quaternion::y * q.z - quaternion::z * q.y,
				quaternion::r * q.y + quaternion::y * q.r + quaternion::z * q.x - quaternion::x * q.z,
				quaternion::r * q.z + quaternion::z * q.r + quaternion::x * q.y - quaternion::y * q.x);
}

/*! * operator overload
 */
quaternion quaternion::operator*(float f) {
	return quaternion(quaternion::r * f, quaternion::x * f, quaternion::y * f, quaternion::z * f);
}

/*! / operator overload
 */
quaternion quaternion::operator/(quaternion& q) {
	return (*this) * q.inverse();
}

/*! += operator overload
 */
quaternion quaternion::operator+=(const quaternion& q) {
	quaternion::r += q.r;
	quaternion::x += q.x;
	quaternion::y += q.y;
	quaternion::z += q.z;
	return (*this);
}

/*! -= operator overload
 */
quaternion quaternion::operator-=(const quaternion& q) {
	quaternion::r -= q.r;
	quaternion::x -= q.x;
	quaternion::y -= q.y;
	quaternion::z -= q.z;
	return (*this);
}

/*! *= operator overload
 */
quaternion quaternion::operator*=(quaternion& q) {
	(*this) = q * (*this);
	return (*this);
}

/*! *= operator overload
 */
quaternion quaternion::operator*=(float f) {
	(*this) = (*this) * f;
	return (*this);
}

/*! /= operator overload
 */
quaternion quaternion::operator/=(quaternion& q) {
	(*this) = (*this) * q.inverse();
  	return (*this);
}

/*! /= operator overload
 */
quaternion quaternion::operator/=(float f) {
	return quaternion(quaternion::r / f, quaternion::x / f, quaternion::y / f, quaternion::z / f);
}

/*! resets the quaternion to r = 1, x = 0, y = 0 and z = 0
 */
void quaternion::reset() {
	quaternion::r = 1.0f;
	quaternion::x = 0.0f;
	quaternion::y = 0.0f;
	quaternion::z = 0.0f;
}

/*! normalizes the quaternion
 */
void quaternion::normalize() {
	(*this) /= this->magnitude();
}

/*! returns the quaternions magnitude
 */
float quaternion::magnitude() {
	return (float)sqrt(quaternion::r * quaternion::r + quaternion::x * quaternion::x + quaternion::y * quaternion::y + quaternion::z * quaternion::z);
}

/*! returns the inverted quaternion
 */
quaternion quaternion::inverse() {
	return conjugate() * (1.0f / (quaternion::r * quaternion::r + quaternion::x * quaternion::x + quaternion::y * quaternion::y + quaternion::z * quaternion::z));
}

/*! returns the conjugated quaternion
 */
quaternion quaternion::conjugate() {
	return quaternion(r, -x, -y, -z);
}

/*! returns the unit quaternion
 */
quaternion quaternion::unit() {
	return (*this) * (1.0f / (this->magnitude()));
}

/*! rotates the quaternion
 *  @param v rotation vector
 */
vertex3 quaternion::rotate(vertex3& v) {
	quaternion vec(0, v.x, v.y, v.z);
	quaternion q = *this;
	quaternion qinv = q.conjugate();

	quaternion vec2 = q * vec * qinv;

	return vertex3(vec2.x, vec2.y, vec2.z);
}

/*! sets the quaternions rotation
 *  @param a resize float?
 *  @param v rotation vector
 */
void quaternion::set_rotation(float a, vertex3 tv) {
	vertex3 tmp = tv.normalize();
	a *= (float)PI / 180.0f;
	float sin_a = sinf (a / 2);
	float cos_a = cosf (a / 2);
	quaternion::r = cos_a;
	quaternion::x = tmp.x * sin_a;
	quaternion::y = tmp.y * sin_a;
	quaternion::z = tmp.z * sin_a;
}

/*! sets the quaternions rotation
 *  @param a resize float?
 *  @param i rotation x-vector
 *  @param j rotation y-vector
 *  @param k rotation z-vector
 */
void quaternion::set_rotation(float a, float i, float j, float k) {
	quaternion::set_rotation(a, vertex3(i, j, k));
}

/*! computes the quaternions r value
 */
void quaternion::compute_r() {
	float t = 1.0f - (quaternion::x * quaternion::x) - (quaternion::y * quaternion::y) - (quaternion::z * quaternion::z);
	quaternion::r = (t < 0.0f) ? 0.0f : -(float)sqrt(t);
}
