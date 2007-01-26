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
 
#ifndef __MDL_H__
#define __MDL_H__

#ifdef WIN32
#include <windows.h>
#endif

#include <iostream>
#include <list>
#include <cmath>
#include <string>
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
using namespace std;

// the main class
class mdl {
public:
	mdl(engine* e, scene* sce, camera* cam);
	~mdl();

	void load_model(char* filename, char* ani_filename = NULL);
	void close_model();
	void set_material(char* filename);
	void save_material(const char* filename = "temp.dat");
	bool is_model();

	void update_selection(unsigned int obj_num);
	void draw_selection(bool wireframe);

	a2ematerial* get_material();

	a2emodel* model;
	a2eanim* amodel;
	a2ematerial* mat;
	bool ani;
	string mat_fname;
	string mdl_fname;

protected:
	engine* e;
	msg* m;
	scene* sce;
	file_io* f;
	camera* cam;
	gfx* agfx;

	bool mdl_ld;

	vertex3* vertices;
	core::index* indices;
	unsigned int index_count;
	bool updated;
	unsigned int ud_timer;
	unsigned int ud_timer2;
	unsigned int ud_timer_out;
	unsigned int ud_timer2_out;
	float ud_alpha;
	float ud_alpha_start;

};

#endif
