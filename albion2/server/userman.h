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

#ifndef __USERMAN_H__
#define __USERMAN_H__

#ifdef WIN32
#include <windows.h>
#endif

#include <iostream>
#include <string>
#include <sstream>
#include <map>
#include <SDL/SDL.h>
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
#include <net.h>
using namespace std;

class userman {
public:
	userman(engine* e);
	~userman();

	struct user_data {
		string password;
		vertex3 position;
		unsigned int id;
	};

	void load_list(const char* file);
	user_data* get_user(const char* name);
	void add_user(const char* name, const char* pw);
	bool check_user(const char* name);
	bool check_pw(const char* name, const char* pw);
	void save_user_list();

protected:
	engine* e;
	msg* m;
	core* c;
	xml* x;

	stringstream* buffer;

	map<string, user_data> user_list;
	unsigned int max_id;

};

#endif
