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
#include <SDL.h>
#include "msg.h"
//#include "core.h"
#include "engine.h"
#include "event.h"
#include <math.h>
using namespace std;

#include "win_dll_export.h"

/*! @class camera
 *  @brief a2e camera functions
 *  @author flo
 *  @version 0.1.2
 *  @date 2004/09/12
 *  @todo more functions
 *  
 *  the camera class
 */

class A2E_API camera
{
public:
	camera();
	~camera();

	void init(engine &iengine, event &ievent);
	void run();

	void set_position(float x, float y, float z);
	core::vertex3* get_position();

	//! returns a 3d line (ray) from the screen position (ray)
	void get_ray_from_cord(gfx::pnt* pos, core::line* ray);

protected:
	msg m;
	//core c;

	event* event_handler;
	engine* engine_handler;

	core::vertex3* position;
	float rotation;
	float up_down;

	float piover180;
};

#endif
