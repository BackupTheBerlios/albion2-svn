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

#include "line3.h"

/*! creates a line3 object
 */
line3::line3() {
	line3::v1 = new vertex3();
	line3::v2 = new vertex3();
}

/*! creates a line3 object
 */
line3::line3(float x1, float y1, float z1, float x2, float y2, float z2) {
	line3::v1 = new vertex3();
	line3::v2 = new vertex3();

	line3::v1->x = x1;
	line3::v1->y = y1;
	line3::v1->z = z1;
	line3::v2->x = x2;
	line3::v2->y = y2;
	line3::v2->z = z2;
}

/*! creates a line3 object
 */
line3::line3(vertex3* v1, vertex3* v2) {
	line3::v1 = new vertex3();
	line3::v2 = new vertex3();

	line3::v1->x = v1->x;
	line3::v1->y = v1->y;
	line3::v1->z = v1->z;
	line3::v2->x = v2->x;
	line3::v2->y = v2->y;
	line3::v2->z = v2->z;
}

/*! line3 destructor
 */
line3::~line3() {
	delete line3::v1;
	delete line3::v2;
}

/*! returns the lines direction (as an unit vector)
 */
vertex3 line3::get_direction() {
	return ((*line3::v2) - (*line3::v1)).normalize();
}

/*! returns the lines middle point
 */
vertex3 line3::get_middle() {
	return (*line3::v1 + *line3::v2) * 0.5f;
}

/*! returns the lines length
 */
float line3::get_length() {
	return sqrtf(powf(line3::v1->x - line3::v2->x, 2) + powf(line3::v1->y - line3::v2->y, 2) + powf(line3::v1->z - line3::v2->z, 2));
}
