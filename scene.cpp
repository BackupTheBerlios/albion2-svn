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

#include "scene.h"
#include "msg.h"
#include <math.h>

/*! there is no function currently
 */
scene::scene() {
	scene::cmodels = 0;
	scene::position = (core::vertex*)malloc(sizeof(core::vertex));
	scene::position->x = 0.0f;
	scene::position->y = 0.0f;
	scene::position->z = 0.0f;
}

/*! there is no function currently
 */
scene::~scene() {
	free(scene::position);
}

/*! draws the scene
 */
void scene::draw() {
	for(unsigned int i = 0; i < cmodels; i++) {
		models[i]->draw_model();
	}
}

/*! adds a model to the scene
 *  @param model pointer to the model
 */
void scene::add_model(a2emodel* model) {
	models[cmodels] = model;
	cmodels++;
}

/*! sets the position of the scene
 *  @param xpos x coordinate
 *  @param ypos y coordinate
 *  @param zpos z coordinate
 */
void scene::set_position(float x, float y, float z) {
	scene::position->x = x;
	scene::position->y = y;
	scene::position->z = z;

	for(unsigned int i = 0; i < cmodels; i++) {
		models[i]->set_position(models[i]->get_position()->x + scene::position->x,
			models[i]->get_position()->y + scene::position->y,
			models[i]->get_position()->z + scene::position->z);
	}
}

/*! returns the position of the scene
 */
core::vertex* scene::get_position() {
	return scene::position;
}
