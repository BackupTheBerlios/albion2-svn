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

#include "light.h"
#include "msg.h"
#include <math.h>

/*! creates a new light object
 *  @param x the lights x position
 *  @param y the lights y position
 *  @param z the lights z position
 */
light::light(float x, float y, float z) {
	position = (core::vertex3*)malloc(sizeof(core::vertex3));
	position->x = x;
	position->y = y;
	position->z = z;

	float lambient[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	float ldiffuse[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	float lspecular[] = { 0.0f, 0.0f, 0.0f, 0.0f };
}

/*! creates a new light object
 */
light::light(core::vertex3* pos) {
	light::light(pos->x, pos->y, pos->z);
}

/*! destroys the light object
 */
light::~light() {
	m.print(msg::MDEBUG, "light.cpp", "freeing light stuff");

	free(position);
	/*free(lambient);
	free(ldiffuse);
	free(lspecular);*/

	m.print(msg::MDEBUG, "light.cpp", "light stuff freed");
}

/*! draws the lights
 */
//void light::draw() {
//}

/*! sets the lights position
 *  @param position the lights position (vertex3)
 */
void light::set_position(core::vertex3* position) {
	light::position = position;
}

/*! sets the lights ambient color
 *  @param lambient the lights ambient color (vertex4)
 */
void light::set_lambient(float* lambient) {
	light::lambient = lambient;
}

/*! sets the lights diffuse color
 *  @param ldiffuse the lights diffuse color (vertex4)
 */
void light::set_ldiffuse(float* ldiffuse) {
	light::ldiffuse = ldiffuse;
}

/*! sets the lights specular color
 *  @param lspecular the lights specular color (vertex4)
 */
void light::set_lspecular(float* lspecular) {
	light::lspecular = lspecular;
}

/*! sets the lights enabled flag
 *  @param state the flag if the light is enabled or not (bool)
 */
void light::set_enabled(bool state) {
	light::enabled = state;
}

/*! returns the lights position
 */
core::vertex3* light::get_position() {
	return light::position;
}

/*! returns the lights ambient color
 */
float* light::get_lambient() {
	return light::lambient;
}

/*! returns the lights diffuse color
 */
float* light::get_ldiffuse() {
	return light::ldiffuse;
}

/*! returns the lights specular color
 */
float* light::get_lspecular() {
	return light::lspecular;
}

/*! returns true if the light is enabled
 */
bool light::is_enabled() {
	return light::enabled;
}
