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

/*! there is no function currently
 */
camera::camera(engine* e) {
	camera::position = new vertex3();
	camera::rotation = new vertex3();
	camera::direction = new vertex3();

	up_down = 0.0f;

	camera::cam_input = true;
	camera::mouse_input = true;

	rotation_speed = 100.0f;
	cam_speed = 1.0f;

	camera::flip = e->get_ext()->is_fbo_support();

	// get classes
	camera::e = e;
	camera::c = e->get_core();
	camera::m = e->get_msg();
	camera::evt = e->get_event();
}

/*! there is no function currently
 */
camera::~camera() {
	m->print(msg::MDEBUG, "camera.cpp", "freeing camera stuff");

	delete camera::position;
	delete camera::rotation;
	delete camera::direction;

	m->print(msg::MDEBUG, "camera.cpp", "camera stuff freed");
}

/*! runs the camera (expected to be called every draw)
 */
void camera::run() {
	// if camera class input flag is set, then ...
	if(camera::cam_input) {
		// ... recalculate the cameras position
		if(evt->is_key_right()) {
			position->x += (float)sin((rotation->y - 90.0f) * PIOVER180) * cam_speed;
			position->z += (float)cos((rotation->y - 90.0f) * PIOVER180) * cam_speed;
		}

		if(evt->is_key_left()) {
			position->x -= (float)sin((rotation->y - 90.0f) * PIOVER180) * cam_speed;
			position->z -= (float)cos((rotation->y - 90.0f) * PIOVER180) * cam_speed;
		}

		if(evt->is_key_up()) {
			if(!flip) {
				position->x += (float)sin(rotation->y * PIOVER180) * cam_speed;
				position->y -= (float)sin(rotation->x * PIOVER180) * cam_speed;
				position->z += (float)cos(rotation->y * PIOVER180) * cam_speed;
			}
			else {
				position->x += (float)sin(rotation->y * PIOVER180) * cam_speed;
				position->y += (float)sin((360.0f - rotation->x) * PIOVER180) * cam_speed;
				position->z += (float)cos(rotation->y * PIOVER180) * cam_speed;
			}
		}

		if(evt->is_key_down()) {
			if(!flip) {
				position->x -= (float)sin(rotation->y * PIOVER180) * cam_speed;
				position->y += (float)sin(rotation->x * PIOVER180) * cam_speed;
				position->z -= (float)cos(rotation->y * PIOVER180) * cam_speed;
			}
			else {
				position->x -= (float)sin(rotation->y * PIOVER180) * cam_speed;
				position->y -= (float)sin((360.0f - rotation->x) * PIOVER180) * cam_speed;
				position->z -= (float)cos(rotation->y * PIOVER180) * cam_speed;
			}
		}
	}

	if(camera::mouse_input) {
		// calculate the rotation via the current mouse cursor position
		int cursor_pos_x = 0;
		int cursor_pos_y = 0;
		SDL_GetMouseState((int*)&cursor_pos_x, (int*)&cursor_pos_y);

		float xpos = (1.0f / (float)e->get_screen()->w) * (float)cursor_pos_x;
		float ypos = (1.0f / (float)e->get_screen()->h) * (float)cursor_pos_y;

		if(xpos != 0.5f || ypos != 0.5f) {
			rotation->x += (0.5f - ypos) * rotation_speed;
			rotation->y += (0.5f - xpos) * rotation_speed;
			SDL_WarpMouse(e->get_screen()->w/2, e->get_screen()->h/2);
		}
	}

	if(rotation->x > 360.0f) {
		rotation->x -= 360.0f;
	}
	else if(rotation->x < 0.0f) {
		rotation->x += 360.0f;
	}

	if(rotation->y > 360.0f) {
		rotation->y -= 360.0f;
	}
	else if(rotation->y < 0.0f) {
		rotation->y += 360.0f;
	}

	// rotate
	if(!flip) {
		glRotatef(360.0f - rotation->x, 1.0f, 0.0f, 0.0f);
		glRotatef(360.0f - rotation->y, 0.0f, 1.0f, 0.0f);
	}
	else {
		glRotatef(rotation->x, 1.0f, 0.0f, 0.0f);
		glRotatef(360.0f - rotation->y, 0.0f, 1.0f, 0.0f);
	}

	if(flip) position->y *= -1.0f;

	// reposition / "rotate"
	e->set_position(camera::position->x, camera::position->y, camera::position->z);
	e->set_rotation(camera::rotation->x, camera::rotation->y);

	if(flip) position->y *= -1.0f;
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

	camera::position->y *= -1.0f;
}

/*! sets the rotation of the camera
 *  @param x x rotation
 *  @param y y rotation
 *  @param z z rotation
 */
void camera::set_rotation(float x, float y, float z) {
	camera::rotation->x = x;
	camera::rotation->y = y;
	camera::rotation->z = z;
}

/*! returns the position of the camera
 */
vertex3* camera::get_position() {
	return camera::position;
}

/*! returns the rotation of the camera
 */
vertex3* camera::get_rotation() {
	return camera::rotation;
}

/*! if cam_input is set true then arrow key input and (auto-)reposition 
 *! stuff is done automatically in the camera class. otherwise you have
 *! to do it yourself.
 *  @param state the cam_input state
 */
void camera::set_cam_input(bool state) {
	camera::cam_input = state;
}

/*! if mouse_input is set true then the cameras rotation is controlled via
 *! the mouse - furthermore the mouse cursor is reset to (0.5, 0.5) every cycle.
 *! if it's set to false nothing (no rotation) happens.
 *  @param state the cam_input state
 */
void camera::set_mouse_input(bool state) {
	camera::mouse_input = state;
}

/*! returns the cam_input bool
 */
bool camera::get_cam_input() {
	return camera::cam_input;
}

/*! returns the mouse_input bool
 */
bool camera::get_mouse_input() {
	return camera::mouse_input;
}

/*! sets the cameras rotation speed to speed
 *  @param speed the new rotation speed
 */
void camera::set_rotation_speed(float speed) {
	camera::rotation_speed = speed;
}

/*! returns cameras rotation speed
 */
float camera::get_rotation_speed() {
	return camera::rotation_speed;
}

/*! sets the cameras speed to speed
 *  @param speed the new camera speed
 */
void camera::set_cam_speed(float speed) {
	camera::cam_speed = speed;
}

/*! returns cameras speed
 */
float camera::get_cam_speed() {
	return camera::cam_speed;
}

void camera::set_flip(bool state) {
	flip = state;
}

bool camera::get_flip() {
	return flip;
}

/*! returns the cameras direction
 */
vertex3* camera::get_direction() {
	direction->x = (float)sin(rotation->y * PIOVER180);
	direction->y = (float)sin(rotation->x * PIOVER180);
	direction->z = (float)cos(rotation->y * PIOVER180);
	return camera::direction;
}
