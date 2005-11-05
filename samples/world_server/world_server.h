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
 
#ifndef __WORLD_SERVER_H__
#define __WORLD_SERVER_H__

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
#include <net.h>
#include <gfx.h>
#include <event.h>
#include <camera.h>
#include <a2emodel.h>
#include <scene.h>
#include <ode.h>
#include <ode_object.h>
#include <light.h>
#include <file_io.h>
#include <shader.h>
using namespace std;

enum DATA_TYPE {
	DT_MSG,
	DT_MOVE,
	DT_UPDATE,
	DT_MOTION
};

enum CONTROL_STATES {
	// move states
	MV_FORWARD,
	MV_BACKWARD,
	MV_LEFT,
	MV_RIGHT,

	// status states
	ST_OFFLINE,
	ST_ONLINE,

	// motion states
	MT_STANDING,
	MT_SITTING,
	MT_MOVING,
	MT_JUMPING
};

struct client {
	unsigned int id;
	char* name;
	unsigned int status;
	unsigned int motion;
	vertex3* position;
	float rotation;
	unsigned int walk_time;
};

void init();
void send_new_client(unsigned int snd_client, unsigned int rcv_client);
void check_events();
void handle_client(unsigned int cur_client);
void handle_server();
a2emodel* add_model(char* name, vertex3* pos, vertex3* scale, bool fixed, ode_object::OTYPE type, float radius = 1.0f);
a2emodel* add_player(vertex3* pos);
void delete_player(unsigned int num);
void update_players();

msg* m;
net* n;
core* c;
engine* e;
ode* o;
file_io* f;
shader* s;

SDL_Surface* sf;

bool done = false;

SDL_Event ievent;

client* clients;

unsigned int refresh_time;
float piover180 = (float)PI / 180.0f;
float max_force = 10.0f;
unsigned int min_walk_time = 0;

a2emodel** players;
ode_object** ode_players;
unsigned int cplayers = 0;

a2emodel** objects;
ode_object** ode_objects;
unsigned int cobjects = 0;

vertex3* start_pos;

unsigned int port = 1337;
unsigned int max_players = 32;
unsigned int max_objects = 64;
bool netlog = false;

#endif
