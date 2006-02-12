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

#include "core.h"

/*! there is no function currently
 */
core::core(msg* m) {
	core::m = m;
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
float core::get_distance(vertex3* p1, vertex3* p2) {
	return sqrtf(powf(p1->x - p2->x, 2) + powf(p1->y - p2->y, 2) + powf(p1->z - p2->z, 2));
}

/*! returns true if p1 is in line l1
 *  @param l1 line one (line)
 *  @param p1 point one (vertex3)
 *  @param precision the precision how much the value may differ
 *  1 = less precision / 0.1 = more precision (float)
 */
bool core::is_vertex_in_line(line* l1, vertex3* p1, float precision) {
	float dist_l10_l11 = core::get_distance(&l1->v1, &l1->v2);
	float dist_l10_p1 = core::get_distance(&l1->v1, p1);
	float dist_l11_p1 = core::get_distance(&l1->v2, p1);

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
bool core::is_vertex_in_triangle(triangle* t1, vertex3* p1, float precision) {
	// obsolete ...
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

/*! converts (projects) a 2d screen position to a 3d vertex
 *  @param p the 2d screen position
 *  @param v the 3d vertex
 */
void core::get_3d_from_2d(pnt* p, vertex3* v) {
	GLdouble* modelview = new GLdouble[16];
	GLdouble* projection = new GLdouble[16];
	GLint* viewport = new GLint[4];
	GLdouble* output = new GLdouble[3];
	glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
	glGetDoublev(GL_PROJECTION_MATRIX, projection);
	glGetIntegerv(GL_VIEWPORT, viewport);
	gluUnProject(p->x, ((GLdouble)viewport[3] - p->y), 1.0f, modelview, projection,
		viewport, &output[0], &output[1], &output[2]);

	v->x = (float)output[0];
	v->y = (float)output[1];
	v->z = (float)output[2];

	delete output;
	delete modelview;
	delete projection;
	delete viewport;
}

/*! computes the normal of a triangle specified by v1, v2 and v3
 *  @param v1 the first vertex
 *  @param v2 the second vertex
 *  @param v3 the third vertex
 *  @param normal the "output" normal
 */
void core::compute_normal(vertex3* v1, vertex3* v2, vertex3* v3, vertex3& normal) {
	normal = ((*v1) - (*v2)) ^ ((*v3) - (*v1));
	normal.norm();
}

/*! computes the binormal and tangent of a triangle specified by
 *! v1, v2 and v3, their normal and their texture coordinates
 *  @param v1 the first vertex
 *  @param v2 the second vertex
 *  @param v3 the third vertex
 *  @param normal the triangles normal
 *  @param binormal the "output" binormal
 *  @param tangent the "output" tangent
 *  @param t1 the first texture coordinate
 *  @param t2 the second texture coordinate
 *  @param t3 the third texture coordinate
 */
void core::compute_tangent_binormal(vertex3* v1, vertex3* v2, vertex3* v3, vertex3* normal,
	vertex3& binormal, vertex3& tangent,
	coord* t1, coord* t2, coord* t3) {
	vertex3 edge1 = (*v1) - (*v2);
	vertex3 edge2 = (*v3) - (*v1);

	// binormal 
	float deltaX1 = t1->u - t2->u;
	float deltaX2 = t3->u - t1->u;
	binormal = (edge1 * deltaX2) - (edge2 * deltaX1);
	binormal.norm();

	// tangent
	float deltaY1 = t1->v - t2->v;
	float deltaY2 = t3->v - t1->v;
	tangent = (edge1 * deltaY2) - (edge2 * deltaY1);
	tangent.norm();

	// adjust
	vertex3 txb = tangent ^ binormal;
	if(*normal * txb < 0.0f) {
		tangent *= -1.0f;

		// our cam doesn't have an "upvector" of -1.0f, so it isn't needed (?)
		//binormal *= -1.0f;
	}
	else {
		binormal *= -1.0f;
	}
}

/*! computes the normal, binormal and tangent of a triangle
 *! specified by v1, v2 and v3 and their texture coordinates
 *  @param v1 the first vertex
 *  @param v2 the second vertex
 *  @param v3 the third vertex
 *  @param normal the "output" normal
 *  @param binormal the "output" binormal
 *  @param tangent the "output" tangent
 *  @param t1 the first texture coordinate
 *  @param t2 the second texture coordinate
 *  @param t3 the third texture coordinate
 */
void core::compute_normal_tangent_binormal(vertex3* v1, vertex3* v2, vertex3* v3,
		vertex3& normal, vertex3& binormal, vertex3& tangent,
		coord* t1, coord* t2, coord* t3) {
	vertex3 edge1 = (*v1) - (*v2);
	vertex3 edge2 = (*v3) - (*v1);
	normal = edge2 ^ edge1;
	normal.norm();

	// binormal 
	float deltaX1 = t1->u - t2->u;
	float deltaX2 = t3->u - t1->u;
	binormal = (edge1 * deltaX2) - (edge2 * deltaX1);
	binormal.norm();

	// tangent
	float deltaY1 = t1->v - t2->v;
	float deltaY2 = t3->v - t1->v;
	tangent = (edge1 * deltaY2) - (edge2 * deltaY1);
	tangent.norm();

	// adjust
	vertex3 txb = tangent ^ binormal;
	if(normal * txb < 0.0f) {
		tangent *= -1.0f;

		// our cam doesn't have an "upvector" of -1.0f, so it isn't needed (?)
		//binormal *= -1.0f;
	}
	else {
		binormal *= -1.0f;
	}
}

/*! returns the amount of delimeters found in the specified string
 *  @param string the string we want to search in
 *  @param delim the delimiter char
 */
unsigned int core::get_delimiter_count(char* string, char delim) {
	if(strlen(string) == 0) {
		m->print(msg::MERROR, "core.cpp", "get_delimeter_count(): string length is 0!");
		return 0;
	}

	// get the amount of tokens
	unsigned int cdelim = 0;
	for(unsigned int i = 0; i < strlen(string); i++) {
		if(string[i] == delim) {
			cdelim++;
		}
	}

	return cdelim;
}

/*! tokenizes™ a string and returns the tokens
 *  @param string the string we want to tokenize
 *  @param delim the delimiter char
 */
char** core::tokenize(char* string, char delim) {
	if(strlen(string) == 0) {
		m->print(msg::MERROR, "core.cpp", "tokenize(): string length is 0!");
		return 0;
	}

	// get the amount of delimiters
	unsigned int cdelims = core::get_delimiter_count(string, delim);

	// reserve memory for our tokens
	char** tmp_tokens = new char*[cdelims+1];
	char** tokens = new char*[cdelims+1];

	// begin tokenizing
	unsigned int x = 0;

	char* sdelim = new char[2];
	sdelim[0] = delim;
	sdelim[1] = (char)0x00;

	tmp_tokens[x] = strtok(string, sdelim);
	tokens[x] = new char[strlen(tmp_tokens[x])+1];
	tokens[x][strlen(tmp_tokens[x])] = 0x00;
	memcpy(tokens[x], tmp_tokens[x], strlen(tmp_tokens[x]));
	while(x < cdelims) {
		x++;
		tmp_tokens[x] = strtok(NULL, sdelim);
		tokens[x] = new char[strlen(tmp_tokens[x])+1];
		tokens[x][strlen(tmp_tokens[x])] = 0x00;
		memcpy(tokens[x], tmp_tokens[x], strlen(tmp_tokens[x]));
	}

	delete [] sdelim;

	return tokens;
}

/*! returns true if line l1 is in bounding box bbox
 *  @param bbox bounding box (aabbox)
 *  @param l1 the line (line)
 */
bool core::is_line_in_bbox(aabbox* bbox, line3* l1) {
	// thx to miguel gomez for that routine (http://www.gamasutra.com/features/19991018/Gomez_6.htm)
	vertex3* l = new vertex3(l1->get_direction()); //line direction
	vertex3* mid = new vertex3(l1->get_middle()); //midpoint of the line
	float hl = l1->get_length() * 0.5f; //segment half-length
	vertex3* t = new vertex3(((bbox->vmin + bbox->vmax) * 0.5f) - mid); // bbox position subracted by the lines middle point
	vertex3* e = new vertex3((bbox->vmax - bbox->vmin) * 0.5f); // bounding box extent
	float r = 0.0f; // temp result
	bool ret = true;

	if(fabs(t->x) > e->x + hl*fabs(l->x) || fabs(t->y) > e->y + hl*fabs(l->y) || fabs(t->z) > e->z + hl*fabs(l->z)) {
		ret = false;
	}

	if(ret) {
		r = e->y*fabs(l->z) + e->z*fabs(l->y);
		if(fabs(t->y*l->z - t->z*l->y) > r) {
			ret = false;
		}
	}

	if(ret) {
		r = e->x*fabs(l->z) + e->z*fabs(l->x);
		if(fabs(t->z*l->x - t->x*l->z) > r) {
			ret = false;
		}
	}

	if(ret) {
		r = e->x*fabs(l->y) + e->y*fabs(l->x);
		if(fabs(t->x*l->y - t->y*l->x) > r) {
			ret = false;
		}
	}

	delete l;
	delete mid;
	delete t;
	delete e;
	return ret;
}


/*! converts a float to an char string - remember to delete the string afterwards
 *  @param f the float we want to convert
 *  @param str the string we want to "save" the float in
 */
void core::ftoa(float f, char* str) {
	sprintf(str, "%f%c", f, 0x00);
}
