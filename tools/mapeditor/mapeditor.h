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

#define EPSILON 0.000001
#define FLOAT_EQ(x, v) (((v - EPSILON) < x) && (x < (v + EPSILON)))

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
#include <a2emodel.h>
#include "map.h"
using namespace std;

// the main class
class mapeditor {
public:
	mapeditor(engine* e, scene* sce, camera* cam);
	~mapeditor();

	struct map_object {
		list<a2emodel>::iterator model;
		list<a2eanim>::iterator amodel;
		list<a2ematerial>::iterator mat;
		bool type;
		string model_name;
		string model_filename;
		string ani_filename;
		string mat_filename;
		unsigned int phys_type;
		bool gravity;
		bool collision_model;
		bool auto_mass;
		float mass;
		vertex3* position;
		vertex3* orientation;
		vertex3* scale;
		vertex3* phys_scale;
	};

	enum EDIT_STATE {
		NONE,
		CAM_INPUT,
		MOVE,
		ROTATE,
		SCALE,
		PHYS_SCALE
	};

	void open_map(char* filename);
	void save_map(char* filename = NULL);
	void close_map();
	void new_map(char* filename);
	void run(EDIT_STATE edit_state);

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

	list<map_object>* get_objects();

	void select(vertex3* look_at);
	void edit_object(int x, int y, float scale = 2.0f);

	void set_draw_mode(bool wireframe); // true = wireframe; false = fill
	bool get_draw_mode();

	void set_objects_visibility(bool state);
	bool get_objects_visibility();

	void set_physical_map(bool state);
	bool get_physical_map();

	bool* get_move_toggle();
	bool* get_rotate_toggle();
	bool* get_scale_toggle();
	bool* get_phys_scale_toggle();

protected:
	engine* e;
	msg* m;
	core* c;
	gfx* g;
	scene* sce;
	camera* cam;

	cmap* memap;

	bool new_sel;
	bool sel;

	string cur_map_name;

	list<a2emodel> models;
	list<a2eanim> amodels;
	list<a2ematerial> materials;

	list<map_object> objects;

	core::aabbox* cur_bbox;

	list<map_object>::iterator sel_mobject;

	bool map_opened;

	a2emodel* arrow[3];
	a2emodel* circle[3];
	a2emodel* arrow_cube[3];
	a2ematerial* arrow_mat[3];
	a2ematerial* circle_mat[3];
	core::aabbox* ctrl_obj_bbox[3];
	core::aabbox* ctrl_obj_bbox_tmp[3];
	bool* ctrl_obj_selected;
	a2emodel* sphere;
	a2ematerial* sphere_mat;
	float radius;
	vertex3* intersection_point;

	line3* sel_line;
	line3* edit_line;

	EDIT_STATE edit_state;

	bool draw_mode;

	bool objects_visibility;
	bool physical_map;

	bool move_toggle[4]; // 0 = overall toggle flag; 1 = x; 2 = y; 3 = z
	bool rotate_toggle[4];
	bool scale_toggle[4];
	bool phys_scale_toggle[4];

	vertex3* init_pos;
	vertex3* init_rot;
	vertex3* init_scl;
	vertex3* init_phys_scl;

	line3 pick_line;

};

#endif
