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
 
#ifndef __WORLD_CLIENT_H__
#define __WORLD_CLIENT_H__

#ifdef WIN32
#include <windows.h>
#endif

#define MAX_CLIENTS 32
#define OBJECTS 64

#include <iostream>
#include <math.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <engine.h>
#include <msg.h>
#include <core.h>
#include <net.h>
#include <gfx.h>
#include <gui.h>
#include <event.h>
#include <camera.h>
#include <a2emodel.h>
#include <scene.h>
#include <ode.h>
#include <ode_object.h>
#include <light.h>
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
	unsigned int host;
	unsigned int port;

	core::pnt* text_point;
	gui_text* text;

	a2eanim* model;
	a2ematerial* mat;
};

int handle_server_data(char *data);
void handle_server();
void send_msg();
void add_msg(unsigned int length, char* msg, ...);
void update();
void move(CONTROL_STATES type);
void set_motion(CONTROL_STATES type);
void init();
void update_names();
void draw_names();
void delete_player(unsigned int num);
void handle_cam();

msg* m;
net* n;
core* c;
engine* e;
gfx* agfx;
event* aevent;
camera* cam;
file_io* fio;
gui* agui;
shader* s;

scene* sce;

a2ematerial* level_mat;
a2ematerial* scale_mat;
a2ematerial* player_mat;
a2emodel* level;
a2emodel* sphere;

SDL_Surface* sf;

bool done = false;

SDL_Event ievent;

client* clients;

gui_style::COLOR_SCHEME scheme = gui_style::BLUE;

unsigned int refresh_time;
unsigned int name_time;

//a2emodel* objects;
//unsigned int cobjects = 0;

unsigned int cplayers = 0;

unsigned int max_clients = 0;
bool is_networking = false;
bool is_initialized = false;

unsigned int client_num = 0;

float piover180 = (float)PI / 180.0f;

char* client_name;
char* server;
unsigned int port = 0;
unsigned int width = 640;
unsigned int height = 480;
unsigned int depth = 24;
bool fullscreen = false;

light* l1;
light* player_light;

float range = 5.0f;

unsigned int control_state = 0;

gui_window* chat_window;
gui_list* chat_msg_list;
gui_input* chat_msg_input;
gui_button* chat_msg_send;
unsigned int lid = 0;

float direction = 0.0f;
float ydirection = 0.0f;

bool netlog = false;
unsigned int fpslim = 0;

#endif
