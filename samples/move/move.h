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
 
#ifndef __MOVE_H__
#define __MOVE_H__

#ifdef WIN32
#include <windows.h>
#endif

#include <iostream>
#include <math.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <engine.h>
#include <msg.h>
#include <core.h>
#include <gfx.h>
#include <event.h>
#include <camera.h>
#include <a2emodel.h>
#include <scene.h>
#include <ode.h>
#include <ode_object.h>
#include <light.h>
#include <shader.h>
using namespace std;

enum cam_type {
	NONE,
	UP,
	DOWN,
	LEFT,
	RIGHT
};

void update_cam(cam_type ctype);

msg* m;
core* c;
engine* e;
event* aevent;
camera* cam;
ode* o;
scene* sce;
shader* s;

a2emodel* level;
a2emodel* sphere;
a2emodel* player;
a2emodel** spheres;
unsigned int cspheres = 9; // should be a quadratic number
ode_object* sphere_obj;

bool done = false;

SDL_Event ievent;

unsigned int refresh_time;
unsigned int walk_time;
unsigned int min_walk_time = 100;
float max_force = 10.0f;

float piover180 = (float)PI / 180.0f;

bool moved = false;
float lastxforce;
float lastzforce;

bool player_init = false;

float yrot = 0.0f;

#endif
