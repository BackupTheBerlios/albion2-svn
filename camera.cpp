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

#include "camera.h"
#include "msg.h"
#include <math.h>

/*! there is no function currently
 */
camera::camera() {
	camera::position = (core::vertex3*)malloc(sizeof(core::vertex3));

	// thx to nehe for that piover stuff ;)
	piover180 = 0.0174532925f;

	up_down = 0.0f;
}

/*! there is no function currently
 */
camera::~camera() {
	m.print(msg::MDEBUG, "camera.cpp", "freeing camera stuff");

	free(camera::position);

	m.print(msg::MDEBUG, "camera.cpp", "camera stuff freed");
}

/*! initializes the camera
 *  @param iengine pointer to the engine class for creating an engine handler
 *  @param ievent pointer to the event class for creating an event handler
 */
void camera::init(engine &iengine, event &ievent) {
	camera::event_handler = &ievent;
	camera::engine_handler = &iengine;

	camera::position->x = 0.0f;
	camera::position->y = 0.0f;
	camera::position->z = 0.0f;
	
	camera::rotation = 0.0f;
}

/*! runs the camera (expected to be called every draw)
 */
void camera::run() {
	if(event_handler->is_key_right()) {
		camera::rotation -= 1.5f;
	}
	if(event_handler->is_key_left()) {
		camera::rotation += 1.5f;
	}

	if(event_handler->is_key_up()) {
		camera::position->x += (float)sin(camera::rotation * camera::piover180) * 0.75f;
		camera::position->z += (float)cos(camera::rotation * camera::piover180) * 0.75f;
	}
	if(event_handler->is_key_down()) {
		camera::position->x -= (float)sin(camera::rotation * camera::piover180) * 0.75f;
		camera::position->z -= (float)cos(camera::rotation * camera::piover180) * 0.75f;
	}

    glRotatef(up_down, 1.0f, 0.0f , 0.0f);
    glRotatef(360.0f - camera::rotation, 0.0f, 1.0f , 0.0f);

	engine_handler->set_position(camera::position->x, -10.0f, camera::position->z);
}

/*! sets the position of the camera
 *  @param x x coordinate
 *  @param y y coordinate
 *  @param z z coordinate
 */
void camera::set_position(float x, float y, float z) {
	camera::position->x = x;
	camera::position->y = y;
	camera::position->z = z;
}

/*! returns the position of the camera
 */
core::vertex3* camera::get_position() {
	return camera::position;
}

/*! returns a 3d line (ray) from the screen position (ray)
 *  @param pos the x and y coordinate of the screen
 *  @param cam pointer to camera
 *  @param ray pointer to a ray object
 */
void camera::get_ray_from_cord(gfx::pnt* pos, core::line* ray) {
	ray->v1 = *camera::position;
	//ray->v2 = ;
}
