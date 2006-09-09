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

#ifndef __CSYSTEM_H__
#define __CSYSTEM_H__

#ifdef WIN32
#include <windows.h>
#endif

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
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
#include <a2emap.h>
#include <image.h>
#include <net.h>
#include <gui.h>
#include "cmap.h"
using namespace std;

class csystem {
public:
	csystem(engine* e, camera* cam, cmap* cm);
	~csystem();

	bool init_net();
	bool connect_client(const char* name, const char* pw);

	struct chat_msg {
		unsigned int type;
		string name;
		string msg;
	};
	vector<chat_msg> chat_msgs;
	void add_chat_msg(unsigned int type, char* name, char* msg);
	chat_msg* get_msg();

	vector<chat_msg> send_msgs;
	void send_chat_msg(unsigned int type, char* msg);
	chat_msg* get_send_msg();


	engine* e;
	core* c;
	msg* m;
	net* n;
	gui* agui;
	camera* cam;
	cmap* cm;

	SDL_Surface* sf;

	bool done;
	bool netinit;
	bool new_client;
	bool disconnected;
	bool logged_in;
	char* server;
	unsigned short int port;
	unsigned short int lis_port;
	string client_name;
	string client_pw;
	unsigned int client_id;
	vector<unsigned int> vis_user;

	bool move_forward;
	bool move_back;
	unsigned int move_timer;

	struct client {
		string name;
		unsigned int status;
		unsigned int id;

		bool get_map;
		bool get_pos;
		bool get_rot;

		unsigned int map;
		vertex3 position;
		vertex3 rotation;
	};
	vector<client> clients;

	client* get_client(unsigned int id);

	enum CONTROL_FLAGS {
		CF_LOAD_MAP = 1
	};
	vector<unsigned int> flags;
	void add_flag(unsigned int flag);
	unsigned int get_flag();

	void run();

};

#endif
