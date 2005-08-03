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
	return ((value & (unsigned int)pow((double)2, (double)bit)) >> bit);
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
 *  @param p1 point one (vertex3)
 *  @param p2 point two (vertex3)
 */
float core::get_distance(vertex3 p1, vertex3 p2) {
	// little explanation:
	// SQUARE ROOT(SQUARE ROOT((DIFF:X1-X2)^2 + (DIFF:Z1-Z2)^2) + (DIFF:Y1-Y2)^2)
	float dist = sqrtf(sqrtf(powf(p1.x - p2.x, 2) + powf(p1.z - p2.z, 2)) + powf(p1.y - p2.y, 2));
	return dist;
}

/*! returns true if p1 is in line l1
 *  @param l1 line one (line)
 *  @param p1 point one (vertex3)
 *  @param precision the precision how much the value may differ
 *  1 = less precision / 0.1 = more precision (float)
 */
bool core::is_vertex_in_line(line l1, vertex3 p1, float precision) {
	float dist_l10_l11 = core::get_distance(l1.v1, l1.v2);
	float dist_l10_p1 = core::get_distance(l1.v1, p1);
	float dist_l11_p1 = core::get_distance(l1.v2, p1);

	if(dist_l11_p1 >= (dist_l10_l11 - dist_l10_p1 - precision)
		&& dist_l11_p1 <= (dist_l10_l11 - dist_l10_p1 + precision)) {
		return true;
	}

	return false;
}

/*! returns true if p1 is in triangle t1
 *  @param t1 triangle one (triangle)
 *  @param p1 point one (vertex3)
 *  @param precision the precision how much the value may differ
 *  1 = less precision / 0.1 = more precision (float)
 */
bool core::is_vertex_in_triangle(triangle t1, vertex3 p1, float precision) {
	// obsolete ... and probably totally fussily ...
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

	return false;
}

/*! makes a line out of 2 points (p1 and p2)
 *  @param p1 point one (vertex3)
 *  @param p2 point two (vertex3)
 *  @param rline the line (line)
 */
void core::vertex_to_line(vertex3 p1, vertex3 p2, line &rline) {
	rline.v1.x = p1.x;
	rline.v1.y = p1.y;
	rline.v1.z = p1.z;
	
	rline.v2.x = p2.x;
	rline.v2.y = p2.y;
	rline.v2.z = p2.z;
}

/*! makes a triangle out of 3 points (p1, p2 and p3)
 *  @param p1 point one (vertex3)
 *  @param p2 point two (vertex3)
 *  @param p3 point three (vertex3)
 *  @param rtriangle the triangle (triangle)
 */
void core::vertex_to_triangle(vertex3 p1, vertex3 p2, vertex3 p3, triangle &rtriangle) {
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
 *  @param p1 point one (vertex3)
 *  @param p2 point two (vertex3)
 *  @param p3 point three (vertex3)
 *  @param p4 point four (vertex3)
 *  @param rquad the quad (quad)
 */
void core::vertex_to_quad(vertex3 p1, vertex3 p2, vertex3 p3, vertex3 p4, quad &rquad) {
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

/*! multiplies a matrix4 object by a vertex4
 *  @param m the matrix4 (matrix4)
 *  @param v the vertex (vertex4)
 */
void core::mmatrix4_by_vertex4(matrix4 m, vertex4 v) {
	vertex4 result;

	result.x = m.m[0] * v.x + m.m[4] * v.y + m.m[8] * v.z + m.m[12] * v.w;
	result.y = m.m[1] * v.x + m.m[5] * v.y + m.m[9] * v.z + m.m[13] * v.w;
	result.z = m.m[2] * v.x + m.m[6] * v.y + m.m[10] * v.z + m.m[14] * v.w;
	result.w = m.m[3] * v.x + m.m[7] * v.y + m.m[11] * v.z + m.m[15] * v.w;

	v.x = result.x;
	v.y = result.y;
	v.z = result.z;
	v.w = result.w;
}

/*! converts a radiant value into degrees
 *  @param rad the radiant value
 */
float core::rad_to_deg(float rad) {
	return rad * (180.0f / (float)PI);
}

/*! converts (projects) a 3d vertex to a 2d screen position
 *  @param v the 3d vertex
 *  @param p the 2d screen position
 */
void core::get_2d_from_3d(vertex3* v, pnt* p) {
	GLdouble* modelview = new GLdouble[16];
	GLdouble* projection = new GLdouble[16];
	GLint* viewport = new GLint[4];
	GLdouble* output = new GLdouble[3];
	glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
	glGetDoublev(GL_PROJECTION_MATRIX, projection);
	glGetIntegerv(GL_VIEWPORT, viewport);
	gluProject(v->x, v->y, v->z, modelview, projection,
		viewport, &output[0], &output[1], &output[2]);

	p->x = (unsigned int)output[0];
	p->y = (unsigned int)((GLdouble)viewport[3] - output[1]);

	delete output;
	delete modelview;
	delete projection;
	delete viewport;
}
