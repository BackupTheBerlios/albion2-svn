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
 
#ifndef __CAMERA_H__
#define __CAMERA_H__

#include <iostream>
#include <cmath>
#include <SDL/SDL.h>
#include "msg.h"
#include "core.h"
#include "engine.h"
#include "event.h"
using namespace std;

#include "win_dll_export.h"

#define PIOVER180 0.01745329252

/*! @class camera
 *  @brief a2e camera functions
 *  @author flo
 *  @todo more functions
 *  
 *  the camera class
 */

class A2E_API camera
{
public:
	camera(engine* e);
	~camera();

	void run();

	void set_position(float x, float y, float z);
	void set_rotation(float x, float y, float z);
	vertex3* get_position();
	vertex3* get_rotation();

	void set_rotation_speed(float speed);
	float get_rotation_speed();
	void set_cam_speed(float speed);
	float get_cam_speed();

	void set_cam_input(bool state);
	void set_mouse_input(bool state);
	bool get_cam_input();
	bool get_mouse_input();

protected:
	msg* m;
	core* c;
	event* evt;
	engine* e;

	vertex3* position;
	vertex3* rotation;
	float up_down;
	float rotation_speed;
	float cam_speed;

	bool cam_input;
	bool mouse_input;
};

#endif
