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
#include <SDL/SDL_thread.h>
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
#include "mapeditor.h"
using namespace std;

class mgui {
public:
	mgui(engine* e, gui* agui, mapeditor* me, scene* sce);
	~mgui();

	void run();
	void load_main_gui();
	void open_map_dialog();
	void open_property_wnd();
	void add_obj_dialog();
	void new_map_dialog();

	void apply_changes();
	void save_map();
	void close_map();

	static int mtleditor_thread(void* data);
	void update_materials(const char* mat_fname);

	static bool ffilter_mdl(const char* filename);
	static bool ffilter_mtl(const char* filename);
	static bool ffilter_ani(const char* filename);
	static unsigned int get_mdl_obj_count(const char* mdl);
	static unsigned int get_mtl_obj_count(const char* mtl);
	static unsigned int get_mdl_joint_count(const char* mdl);
	static unsigned int get_ani_joint_count(const char* ani);

protected:
	static engine* e;
	static file_io* f;
	gui* agui;
	gui_style* gs;
	msg* m;
	mapeditor* me;
	scene* sce;

	bool prop_wnd_opened;

	unsigned int font_size;

	SDL_Thread* mtleditor_th;
	static SDL_semaphore* mtleditor_se;
	static string sys_call;
	string mat_name;
	bool is_thread;

	static const char* mdl_fname;
	static const char* mtl_fname;
	static const char* ani_fname;

	/***********
	GUI ID Chart
	0 - system
	100 - main gui
	200 - open file dialog
	300 - properties window
	400 - add object window
	500 - new map dialog
	***********/

	GUI_OBJ menu_id;
	gui_window* menu;
	gui_button* mnew_map;
	gui_button* mopen_map;
	gui_button* msave_map;
	gui_button* mclose_map;
	gui_button* mproperties;
	gui_button* madd_obj;
	gui_button* mdel_obj;
	GLuint mnew_tex;
	GLuint mopen_tex;
	GLuint msave_tex;
	GLuint mclose_tex;
	GLuint mprop_tex;
	GLuint madd_tex;
	GLuint mdel_tex;

	GUI_OBJ prop_wnd_id;
	gui_window* prop_wnd;
	gui_text* pmod_name;
	gui_text* pmod_fname;
	gui_text* pani_fname;
	gui_text* pmat_fname;
	gui_text* ppos;
	gui_text* pposx;
	gui_text* pposy;
	gui_text* pposz;
	gui_text* porient;
	gui_text* porientx;
	gui_text* porienty;
	gui_text* porientz;
	gui_text* pscale;
	gui_text* pscalex;
	gui_text* pscaley;
	gui_text* pscalez;
	gui_text* pphys_prop;
	gui_input* pemod_name;
	gui_input* pemod_fname;
	gui_input* peani_fname;
	gui_input* pemat_fname;
	gui_input* peposx;
	gui_input* peposy;
	gui_input* peposz;
	gui_input* peorientx;
	gui_input* peorienty;
	gui_input* peorientz;
	gui_input* pescalex;
	gui_input* pescaley;
	gui_input* pescalez;
	gui_list* plphys_prop;
	gui_button* papply;
	gui_button* pedit_mat;

	GUI_OBJ ao_wnd_id;
	gui_window* ao_wnd;
	gui_text* ao_model_fname;
	gui_text* ao_ani_fname;
	gui_text* ao_mat_fname;
	gui_input* ao_e_model;
	gui_input* ao_e_ani;
	gui_input* ao_e_mat;
	gui_button* ao_model_browse;
	gui_button* ao_ani_browse;
	gui_button* ao_mat_browse;
	gui_button* ao_add;

	gui_window* od_wnd;

	GUI_OBJ nm_wnd_id;
	gui_window* nm_wnd;
	gui_text* nm_mapfname;
	gui_input* nm_e_mapfname;
	gui_button* nm_open;

};

#endif
