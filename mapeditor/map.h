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
#include <fstream>
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
#include <lua.h>
#include <xml.h>
#include <file_io.h>
using namespace std;

class map {
public:
	map(engine* e);
	~map();

	bool open_map(char* filename);
	bool save_map(char* filename);
	bool close_map();

	struct map_object {
		char model_name[32];
		char model_filename[32];
		char ani_filename[32];
		bool model_type; // (false/0 = static, true/1 = dynamic/animated)
		char mat_filename[32];
		vertex3 position;
		vertex3 orientation;
		vertex3 scale;
		unsigned int phys_type; // (0 = box, 1 = sphere, 2 = cylinder, 3 = trimesh)
	};

	char map_type[7];
	unsigned int object_count;
	map_object* map_objects;

protected:
	engine* e;
	msg* m;
	file_io* f;

	bool map_opened;

};

#endif
