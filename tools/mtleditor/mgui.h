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
 
#ifndef __MGUI_H__
#define __MGUI_H__

#ifdef WIN32
#include <windows.h>
#include <io.h>
#include <direct.h>
#else
#include <dirent.h>
#endif

#include <iostream>
#include <fstream>
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
#include <image.h>
#include "mdl.h"
using namespace std;

class mgui {
public:
	mgui(engine* e, gui* agui, mdl* model, scene* sce);
	~mgui();

	void run();
	void load_main_gui();
	void load_obj_wnd();
	void load_mat_wnd();
	void load_tex_wnd();
	void load_om_wnd();
	void load_sm_wnd();
	void load_omat_wnd();

	void update_obj_list();
	void update_mat(a2ematerial* mat, unsigned int obj, unsigned int tex);

	void new_mtl();
	void save_mtl();
	void close_mtl();
	void open_mtl();

	void open_mdl();

protected:
	engine* e;
	gui* agui;
	msg* m;
	scene* sce;
	mdl* model;
	file_io* f;

	unsigned int font_size;

	unsigned int cur_obj;
	unsigned int cur_tex;
	a2ematerial* cur_mat;

	unsigned int cur_rgb_arg;
	unsigned int cur_alpha_arg;

	unsigned int rot_axis;
	float rot[3];

	/***********
	GUI ID Chart
	0 - system
	50 - open file dialog
	60 - msg box stuff
	100 - main gui
	200 - obj window
	300 - mat window
	400 - texture window
	500 - open model window
	600 - save model window
	700 - open material window
	***********/

	gui_window* ofd_wnd;

	GUI_OBJ menu_id;
	gui_window* menu;
	gui_button* mnew_mtl;
	gui_button* mopen_mtl;
	gui_button* msave_mtl;
	gui_button* mclose_mtl;
	gui_button* mopen_model;
	gui_button* mrot_axis_x;
	gui_button* mrot_axis_y;
	gui_button* mrot_axis_z;
	gui_button* mrot_l;
	gui_button* mrot_r;
	GLuint mnew_tex;
	GLuint mopen_tex;
	GLuint msave_tex;
	GLuint mclose_tex;
	GLuint mopen_model_tex;

	GUI_OBJ obj_id;
	gui_window* obj_wnd;
	gui_list* oobj_list;
	gui_button* ochange;

	GUI_OBJ mat_id;
	gui_window* mat_wnd;
	gui_text* mtype;
	gui_text* mrgb;
	gui_text* malpha;
	gui_button* mapply;
	gui_button* mchange_tex;
	gui_button** mtex;
	gui_button** mrgb_arg;
	gui_button** malpha_arg;
	gui_combo* mcb_type;
	gui_combo* mcb_rgb_combine;
	gui_combo* mcb_alpha_combine;
	gui_combo* mcb_rgb_src;
	gui_combo* mcb_rgb_op;
	gui_combo* mcb_alpha_src;
	gui_combo* mcb_alpha_op;
	image* mitex_large;
	GLuint mnone_tex;

	GUI_OBJ tex_id;
	gui_window* tex_wnd;
	gui_text* ttex_fname;
	gui_text* ttex_type;
	gui_button* tbrowse;
	gui_button* tapply;
	gui_input* titex_fname;
	gui_combo* titex_type;

	GUI_OBJ om_id;
	gui_window* om_wnd;
	gui_text* om_mdl_fname;
	gui_text* om_ani_fname;
	gui_text* om_mat_fname;
	gui_text* om_mat_des;
	gui_button* om_browse_mdl;
	gui_button* om_browse_ani;
	gui_button* om_browse_mat;
	gui_button* om_open;
	gui_input* om_imdl_fname;
	gui_input* om_iani_fname;
	gui_input* om_imat_fname;

	GUI_OBJ sm_id;
	gui_window* sm_wnd;
	gui_text* sm_mat_fname;
	gui_button* sm_save;
	gui_input* sm_imat_fname;

	GUI_OBJ omat_id;
	gui_window* omat_wnd;
	gui_text* omat_mat_fname;
	gui_button* omat_open;
	gui_input* omat_imat_fname;
};

#endif
