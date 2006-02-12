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
 *  @param e pointer to the engine class
 *  @param x the lights x position
 *  @param y the lights y position
 *  @param z the lights z position
 */
light::light(engine* e, float x, float y, float z) {
	light::position = new vertex3();
	light::position->x = x;
	light::position->y = y;
	light::position->z = z;

	light::lambient = new float[4];
	light::ldiffuse = new float[4];
	light::lspecular = new float[4];
	light::spot_dir = new float[4];
	for(unsigned int i = 0; i < 4; i++) {
		light::lambient[i] = 0.0f;
		light::ldiffuse[i] = 0.0f;
		light::lspecular[i] = 0.0f;
		light::spot_dir[i] = 0.0f;
	}
	light::cutoff = 180.0f;

	constant_attenuation = 1.0f;
	linear_attenuation = 0.0f;
	quadratic_attenuation = 0.0f;

	// get classes
	light::e = e;
	light::c = e->get_core();
	light::m = e->get_msg();
}

/*! creates a new light object
 */
light::light(engine* e, vertex3* pos) {
	light::light(e, pos->x, pos->y, pos->z);

	// get classes
	light::e = e;
	light::c = e->get_core();
	light::m = e->get_msg();
}

/*! destroys the light object
 */
light::~light() {
	m->print(msg::MDEBUG, "light.cpp", "freeing light stuff");

	delete light::position;
	delete [] light::lambient;
	delete [] light::ldiffuse;
	delete [] light::lspecular;
	delete [] light::spot_dir;

	m->print(msg::MDEBUG, "light.cpp", "light stuff freed");
}

/*! sets the lights position
 *  @param x the lights x position
 *  @param y the lights y position
 *  @param z the lights z position
 */
void light::set_position(float x, float y, float z) {
	light::position->x = x;
	light::position->y = y;
	light::position->z = z;
}

/*! sets the lights ambient color
 *  @param lambient the lights ambient color (float[3])
 */
void light::set_lambient(float* lambient) {
	light::lambient[0] = lambient[0];
	light::lambient[1] = lambient[1];
	light::lambient[2] = lambient[2];
	light::lambient[3] = lambient[3];
}

/*! sets the lights diffuse color
 *  @param ldiffuse the lights diffuse color (float[3])
 */
void light::set_ldiffuse(float* ldiffuse) {
	light::ldiffuse[0] = ldiffuse[0];
	light::ldiffuse[1] = ldiffuse[1];
	light::ldiffuse[2] = ldiffuse[2];
	light::ldiffuse[3] = ldiffuse[3];
}

/*! sets the lights specular color
 *  @param lspecular the lights specular color (float[3])
 */
void light::set_lspecular(float* lspecular) {
	light::lspecular[0] = lspecular[0];
	light::lspecular[1] = lspecular[1];
	light::lspecular[2] = lspecular[2];
	light::lspecular[3] = lspecular[3];
}

/*! sets the lights spot (specular?) light direction
 *  @param direction the spots direction (float[3])
 */
void light::set_spot_direction(float* direction) {
	light::spot_dir[0] = direction[0];
	light::spot_dir[1] = direction[1];
	light::spot_dir[2] = direction[2];
	light::spot_dir[3] = direction[3];
}

/*! sets the lights spot light cutoff angle
 *  @param angle the spots cutoff angle (float)
 */
void light::set_spot_cutoff(float angle) {
	light::cutoff = angle;
}

/*! sets the lights enabled flag
 *  @param state the flag if the light is enabled or not (bool)
 */
void light::set_enabled(bool state) {
	light::enabled = state;
}

/*! sets the lights constant attenuation value
 *  @param constant the constant attenuation value (float)
 */
void light::set_constant_attenuation(float constant) {
	light::constant_attenuation = constant;
}

/*! sets the lights linear attenuation value
 *  @param linear the linear attenuation value (float)
 */
void light::set_linear_attenuation(float linear) {
	light::linear_attenuation = linear;
}

/*! sets the lights quadratic attenuation value
 *  @param quadratic the quadratic attenuation value (float)
 */
void light::set_quadratic_attenuation(float quadratic) {
	light::quadratic_attenuation = quadratic;
}

/*! returns the lights position
 */
vertex3* light::get_position() {
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

/*! returns the lights spot direction
 */
float* light::get_spot_direction() {
	return light::spot_dir;
}

/*! returns the spot cutoff
 */
float light::get_spot_cutoff() {
	return light::cutoff;
}

/*! returns true if the light is enabled
 */
bool light::is_enabled() {
	return light::enabled;
}

/*! returns the constant attenuation value
 */
float light::get_constant_attenuation() {
	return light::constant_attenuation;
}

/*! returns the linear attenuation value
 */
float light::get_linear_attenuation() {
	return light::linear_attenuation;
}

/*! returns the quadratic attenuation value
 */
float light::get_quadratic_attenuation() {
	return light::quadratic_attenuation;
}
