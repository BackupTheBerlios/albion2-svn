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
 
#ifndef __MAPEDITOR_H__
#define __MAPEDITOR_H__

#ifdef WIN32
#include <windows.h>
#endif

#include <iostream>
#include <list>
#include <string>
#include <cmath>
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
#include <vertex3.h>
#include "map.h"
using namespace std;

// the main class
class mapeditor {
public:
	mapeditor(engine* e, scene* sce);
	~mapeditor();

	struct map_object {
		list<a2emodel>::iterator model;
		list<a2eanim>::iterator amodel;
		list<a2ematerial>::iterator mat;
		bool type;
		char model_name[64];
		char model_filename[64];
		char ani_filename[64];
		char mat_filename[64];
		unsigned int phys_type;
	};

	void open_map(char* filename);
	void save_map(char* filename = NULL);
	void close_map();
	void run();

	void select_object(vertex3* look_at);
	list<map_object>::iterator get_sel_object();
	bool is_sel();
	bool get_new_sel();
	void set_new_sel(bool state);

	void reload_cur_bbox();

	bool is_map_opened();

	void add_object(char* model_filename, char* mat_filename);
	void add_object(char* model_filename, char* ani_filename, char* mat_filename);

	void delete_obj();

	list<a2emodel>* get_models();
	list<a2eanim>* get_amodels();
	list<a2ematerial>* get_materials();

protected:
	engine* e;
	msg* m;
	scene* sce;

	cmap* memap;

	bool new_sel;
	bool sel;

	string cur_map_name;

	/*unsigned int csmodels;
	unsigned int camodels;
	a2emodel** models;
	a2eanim** amodels;
	a2ematerial** materials;*/
	list<a2emodel> models;
	list<a2eanim> amodels;
	list<a2ematerial> materials;

	//unsigned int cobjects;
	//map_object* objects;
	list<map_object> objects;

	core::aabbox* cur_bbox;

	list<map_object>::iterator sel_mobject;

	bool map_opened;

};

#endif
