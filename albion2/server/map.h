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

#ifndef __MAP_H__
#define __MAP_H__

#ifdef WIN32
#include <windows.h>
#endif

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <map>
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
#include "userman.h"
using namespace std;

class smap {
public:
	smap(engine* e, scene* sce, userman* um);
	~smap();

	void load();
	void run(unsigned int ode_time);

	void add_user_phys_obj(const char* name);
	void del_user_phys_obj(const char* name);
	void set_user_obj_map();

	struct map_object {
		a2emap* map;
		ode* o;
	};

protected:
	engine* e;
	core* c;
	msg* m;
	scene* sce;
	userman* um;

	map<string, map_object> maps;
	map<unsigned int, string> map_names;

	//a2emap* map;

};

#endif
