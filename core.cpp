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

#include <iostream>
using namespace std;
#include "core.h"
#include <math.h>

/*! there is no function currently
 */
core::core() {
}

/*! there is no function currently
 */
core::~core() {
}

/*! returns 1 if the bitth bit of value is 1 and 0 if it is 0
 *  @param value the value from which we want to get the bit
 *  @param bit the number of the bit we want to get the bit from (0-7)
 */
unsigned int core::get_bit(unsigned int value, unsigned int bit) {
	unsigned int rbit = (value & (unsigned int)pow((double)2, (double)bit)) >> bit;
	return rbit;
}

/*! sets the bitth bit of value to num
 *  @param value the value where we want to change the bit 
 *  @param bit the number of the bit (0-7)
 *  @param num change bit to 0 or 1?
 */
unsigned int core::set_bit(unsigned int value, unsigned int bit, unsigned int num) {
	unsigned int sbit = core::get_bit(value, bit);
	unsigned int rbit = value;
	if(num == 0) {
		if(sbit == 1) {
			rbit = value - (unsigned int)pow((double)2, (double)bit);
		}
	}
	else {
		if(sbit == 0) {
			rbit = value + (unsigned int)pow((double)2, (double)bit);
		}
	}

	return rbit;
}

/*! returns the distance between two points
 *  @param p1 point one (vector3)
 *  @param p2 point two (vector3)
 */
float core::get_distance(vector3 p1, vector3 p2) {
	// little explanation:
	// SQUARE ROOT(SQUARE ROOT((DIFF:X1-X2)^2 + (DIFF:Z1-Z2)^2) + (DIFF:Y1-Y2)^2)
	float dist = sqrtf(sqrtf(powf(p1.x - p2.x, 2) + powf(p1.z - p2.z, 2)) + powf(p1.y - p2.y, 2));
	return dist;
}

/*! returns true if p1 is in line l1
 *  @param l1 line one (line)
 *  @param p1 point one (vector3)
 *  @param precision the precision how much the value may differ
 *  1 = less precision / 0.1 = more precision (float)
 */
bool core::is_vector_in_line(line l1, vector3 p1, float precision) {
	float dist_l10_l11 = core::get_distance(l1.v1, l1.v2);
	float dist_l10_p1 = core::get_distance(l1.v1, p1);
	float dist_l11_p1 = core::get_distance(l1.v2, p1);

	if(dist_l11_p1 >= (dist_l10_l11 - dist_l10_p1 - precision)
		&& dist_l11_p1 <= (dist_l10_l11 - dist_l10_p1 + precision)) {
		return true;
	}
	else {
		return false;
	}

	// is needed?
	return false;
}

/*! returns true if p1 is in triangle t1
 *  @param t1 triangle one (triangle)
 *  @param p1 point one (vector3)
 *  @param precision the precision how much the value may differ
 *  1 = less precision / 0.1 = more precision (float)
 */
bool core::is_vector_in_triangle(triangle t1, vector3 p1, float precision) {
	float dist_t10_t11 = core::get_distance(t1.v1, t1.v2);
	float dist_t10_t12 = core::get_distance(t1.v1, t1.v3);
	float dist_t11_t12 = core::get_distance(t1.v2, t1.v3);

	float dist_t10_p1 = core::get_distance(t1.v1, p1);
	float dist_t11_p1 = core::get_distance(t1.v2, p1);
	float dist_t12_p1 = core::get_distance(t1.v3, p1);

	// triangle
	float alpha = acosf(((powf(dist_t10_t11, 2) + powf(dist_t11_t12, 2) - powf(dist_t10_t12, 2)) / (2 * powf(dist_t10_t11, 2) * powf(dist_t11_t12, 2))));
	float beta = acosf(((powf(dist_t10_t12, 2) + powf(dist_t11_t12, 2) - powf(dist_t10_t11, 2)) / (2 * powf(dist_t10_t12, 2) * powf(dist_t11_t12, 2))));
	float height = sinf(alpha) * dist_t10_t11;
	float kath1 = height / tanf(alpha);
	float kath2 = height / tanf(beta);
	float area_t = ((height * kath1) / 2) + ((height * kath2) / 2);

	// sub triangle 1
	float alpha1 = acosf(((powf(dist_t10_p1, 2) + powf(dist_t10_t11, 2) - powf(dist_t11_p1, 2)) / (2 * powf(dist_t10_p1, 2) * powf(dist_t10_t11, 2))));
	float beta1 = acosf(((powf(dist_t11_p1, 2) + powf(dist_t10_t11, 2) - powf(dist_t10_p1, 2)) / (2 * powf(dist_t11_p1, 2) * powf(dist_t10_t11, 2))));
	float height1 = sinf(alpha1) * dist_t10_p1;
	float kath1_1 = height1 / tanf(alpha1);
	float kath1_2 = height1 / tanf(beta1);
	float area1 = ((height1 * kath1_1) / 2) + ((height1 * kath1_2) / 2);

	// sub triangle 2
	float alpha2 = acosf(((powf(dist_t11_p1, 2) + powf(dist_t11_t12, 2) - powf(dist_t12_p1, 2)) / (2 * powf(dist_t11_p1, 2) * powf(dist_t11_t12, 2))));
	float beta2 = acosf(((powf(dist_t12_p1, 2) + powf(dist_t11_t12, 2) - powf(dist_t11_p1, 2)) / (2 * powf(dist_t12_p1, 2) * powf(dist_t11_t12, 2))));
	float height2 = sinf(alpha2) * dist_t11_p1;
	float kath2_1 = height2 / tanf(alpha2);
	float kath2_2 = height2 / tanf(beta2);
	float area2 = ((height2 * kath2_1) / 2) + ((height2 * kath2_2) / 2);

	// sub triangle 3
	float alpha3 = acosf(((powf(dist_t12_p1, 2) + powf(dist_t10_t12, 2) - powf(dist_t10_p1, 2)) / (2 * powf(dist_t12_p1, 2) * powf(dist_t10_t12, 2))));
	float beta3 = acosf(((powf(dist_t10_p1, 2) + powf(dist_t10_t12, 2) - powf(dist_t12_p1, 2)) / (2 * powf(dist_t10_p1, 2) * powf(dist_t10_t12, 2))));
	float height3 = sinf(alpha3) * dist_t12_p1;
	float kath3_1 = height3 / tanf(alpha3);
	float kath3_2 = height3 / tanf(beta3);
	float area3 = ((height3 * kath3_1) / 2) + ((height3 * kath3_2) / 2);

	// addition of all sub triangle areas 
	float area = area1 + area2 + area3;

	if(area >= (area_t - precision)
		&& area <= (area_t + precision)) {
		return true;
	}
	else {
		return false;
	}

	// is needed?
	return false;
}

/*! makes a line out of 2 points (p1 and p2)
 *  @param p1 point one (vector3)
 *  @param p2 point two (vector3)
 *  @param rline the line (line)
 */
void core::vec_to_line(vector3 p1, vector3 p2, line &rline) {
	rline.v1.x = p1.x;
	rline.v1.y = p1.y;
	rline.v1.z = p1.z;
	
	rline.v2.x = p2.x;
	rline.v2.y = p2.y;
	rline.v2.z = p2.z;
}

/*! makes a triangle out of 3 points (p1, p2 and p3)
 *  @param p1 point one (vector3)
 *  @param p2 point two (vector3)
 *  @param p3 point three (vector3)
 *  @param rtriangle the triangle (triangle)
 */
void core::vec_to_triangle(vector3 p1, vector3 p2, vector3 p3, triangle &rtriangle) {
	rtriangle.v1.x = p1.x;
	rtriangle.v1.y = p1.y;
	rtriangle.v1.z = p1.z;
	
	rtriangle.v2.x = p2.x;
	rtriangle.v2.y = p2.y;
	rtriangle.v2.z = p2.z;

	rtriangle.v3.x = p3.x;
	rtriangle.v3.y = p3.y;
	rtriangle.v3.z = p3.z;
}

/*! makes a quad out of 4 points (p1, p2, p3 and p4)
 *  @param p1 point one (vector3)
 *  @param p2 point two (vector3)
 *  @param p3 point three (vector3)
 *  @param p4 point four (vector3)
 *  @param rquad the quad (quad)
 */
void core::vec_to_quad(vector3 p1, vector3 p2, vector3 p3, vector3 p4, quad &rquad) {
	rquad.v1.x = p1.x;
	rquad.v1.y = p1.y;
	rquad.v1.z = p1.z;
	
	rquad.v2.x = p2.x;
	rquad.v2.y = p2.y;
	rquad.v2.z = p2.z;

	rquad.v3.x = p3.x;
	rquad.v3.y = p3.y;
	rquad.v3.z = p3.z;

	rquad.v4.x = p4.x;
	rquad.v4.y = p4.y;
	rquad.v4.z = p4.z;
}
