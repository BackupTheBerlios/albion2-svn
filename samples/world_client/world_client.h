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

#define MAX_CLIENTS 32
#define PORT 1337
#define OBJECTS 64

#include <iostream>
#include <math.h>
#include <SDL.h>
#include <SDL_image.h>
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
#include <shadow.h>
#include <light.h>
using namespace std;

enum DATA_TYPE {
	DT_MSG,
	DT_MOVE,
	DT_UPDATE
};

enum MOVE_TYPE {
	MV_FORWARD,
	MV_BACKWARD,
	MV_LEFT,
	MV_RIGHT
};

struct client {
	unsigned int id;
	char* name;
	unsigned int status;
	core::vertex3* position;
	float rotation;
	unsigned int host;
	unsigned int port;
};

int HandleServerData(char *data);
void HandleServer(void);
void update();
void move(MOVE_TYPE type);

msg m;
net n;
core c;
engine e;
gfx agfx;
event aevent;
camera cam;

scene sce;

a2emodel level;

SDL_Surface* sf;

bool done = false;

SDL_Event ievent;

client* clients;

unsigned int refresh_time;

//a2emodel* objects;
//unsigned int cobjects = 0;

a2emodel* players;
unsigned int cplayers = 0;

unsigned int max_clients = 0;
bool is_networking = false;

unsigned int client_num = 0;

float piover180 = (float)PI / 180.0f;

#endif
