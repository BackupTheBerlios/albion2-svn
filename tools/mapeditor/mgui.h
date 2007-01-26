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
	void open_namelist_wnd();

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

	void set_edit_state(mapeditor::EDIT_STATE edit_state);
	mapeditor::EDIT_STATE get_edit_state();

	bool is_phys_obj_selected();
	list<mapeditor::map_object>::iterator get_phys_obj();
	void set_phys_obj(bool active);

protected:
	static engine* e;
	static file_io* f;
	gui* agui;
	gfx* g;
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

	mapeditor::EDIT_STATE edit_state;

	list<mapeditor::map_object>::iterator sel_phys_obj;
	bool phys_obj_selected;

	bool* move_toggle;
	bool* rotate_toggle;
	bool* scale_toggle;
	bool* phys_scale_toggle;

	/***********
	GUI ID Chart
	0 - system
	100 - main gui
	200 - open file dialog
	300 - property window
	400 - add object window
	500 - new map dialog
	600 - extended property window(s)
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
	gui_button* mwireframe_fill;
	gui_button* mphys_map;
	gui_button* mes_mouse;
	gui_button* mes_move;
	gui_button* mes_rotate;
	gui_button* mes_scale;
	gui_button* mes_phys_scale;
	GLuint mnew_tex;
	GLuint mopen_tex;
	GLuint msave_tex;
	GLuint mclose_tex;
	GLuint mprop_tex;
	GLuint madd_tex;
	GLuint mdel_tex;
	GLuint mwireframe_tex;
	GLuint mfill_tex;
	GLuint mphys_map_tex;
	GLuint mmouse_tex;
	GLuint mmove_tex;
	GLuint mrotate_tex;
	GLuint mscale_tex;
	GLuint mphys_scale_tex;
	GLuint mreset_tex;
	GLuint mresetx_tex;
	GLuint mresety_tex;
	GLuint mresetz_tex;

	// property window
	GUI_OBJ prop_wnd_id;
	gui_window* prop_wnd;
	gui_tab* prop_tab;
	gui_button* prop_apply;

	// tab files
	// labels
	gui_text* pf_type_label;
	gui_text* pf_mdl_name_label;
	gui_text* pf_mdl_fname_label;
	gui_text* pf_ani_fname_label;
	gui_text* pf_mat_fname_label;
	gui_text* pf_file_info_label;
	gui_text* pf_mdl_label;
	gui_text* pf_ani_label;
	gui_text* pf_mat_label;
	gui_text* pf_mdl_subobjs_label;
	gui_text* pf_mdl_vertices_label;
	gui_text* pf_mdl_texcoords_label;
	gui_text* pf_mdl_faces_label;
	gui_text* pf_ani_joints_label;
	gui_text* pf_ani_frames_label;
	gui_text* pf_mat_subobjs_label;
	// label info
	gui_text* pf_type;
	gui_text* pf_mdl_subobjs;
	gui_text* pf_mdl_vertices;
	gui_text* pf_mdl_texcoords;
	gui_text* pf_mdl_faces;
	gui_text* pf_ani_joints;
	gui_text* pf_ani_frames;
	gui_text* pf_mat_subobjs;
	// input boxes
	gui_input* pf_mdl_name;
	gui_input* pf_mdl_fname;
	gui_input* pf_ani_fname;
	gui_input* pf_mat_fname;
	// buttons
	gui_button* pf_mdl_name_brw;
	gui_button* pf_mdl_fname_brw;
	gui_button* pf_ani_fname_brw;
	gui_button* pf_mat_fname_brw;
	gui_button* pf_mdl_subobjs_namelist;
	gui_button* pf_mat_edit;

	// tab transformation
	// labels
	gui_text* pt_position_label;
	gui_text* pt_rotation_label;
	gui_text* pt_scale_label;
	gui_text* pt_pos_x_label;
	gui_text* pt_pos_y_label;
	gui_text* pt_pos_z_label;
	gui_text* pt_rot_x_label;
	gui_text* pt_rot_y_label;
	gui_text* pt_rot_z_label;
	gui_text* pt_scl_x_label;
	gui_text* pt_scl_y_label;
	gui_text* pt_scl_z_label;
	// input boxes
	gui_input* pt_pos_x;
	gui_input* pt_pos_y;
	gui_input* pt_pos_z;
	gui_input* pt_rot_x;
	gui_input* pt_rot_y;
	gui_input* pt_rot_z;
	gui_input* pt_scl_x;
	gui_input* pt_scl_y;
	gui_input* pt_scl_z;
	// buttons and toggle buttons
	gui_toggle* pt_pos_x_change;
	gui_toggle* pt_pos_y_change;
	gui_toggle* pt_pos_z_change;
	gui_toggle* pt_rot_x_change;
	gui_toggle* pt_rot_y_change;
	gui_toggle* pt_rot_z_change;
	gui_toggle* pt_scl_x_change;
	gui_toggle* pt_scl_y_change;
	gui_toggle* pt_scl_z_change;
	gui_button* pt_pos_reset;
	gui_button* pt_rot_reset;
	gui_button* pt_scl_reset;
	gui_button* pt_pos_x_reset;
	gui_button* pt_pos_y_reset;
	gui_button* pt_pos_z_reset;
	gui_button* pt_rot_x_reset;
	gui_button* pt_rot_y_reset;
	gui_button* pt_rot_z_reset;
	gui_button* pt_scl_x_reset;
	gui_button* pt_scl_y_reset;
	gui_button* pt_scl_z_reset;

	// tab phys properties
	// labels
	gui_text* pp_type_label;
	gui_text* pp_mass_label;
	gui_text* pp_scale_label;
	gui_text* pp_scl_x_label;
	gui_text* pp_scl_y_label;
	gui_text* pp_scl_z_label;
	// input boxes
	gui_input* pp_mass;
	gui_input* pp_scl_x;
	gui_input* pp_scl_y;
	gui_input* pp_scl_z;
	// further control objects
	gui_check* pp_gravity;
	gui_check* pp_collision_model;
	gui_check* pp_auto_mass;
	gui_combo* pp_type;
	gui_toggle* pp_scl_x_change;
	gui_toggle* pp_scl_y_change;
	gui_toggle* pp_scl_z_change;
	gui_button* pp_scl_reset;
	gui_button* pp_scl_x_reset;
	gui_button* pp_scl_y_reset;
	gui_button* pp_scl_z_reset;

	// tab misc
	// - none yet -

	// namelist wnd
	GUI_OBJ namelist_wnd_id;
	gui_window* namelist_wnd;
	gui_list* nl_list;
	gui_button* nl_close;
	gui_button* nl_reload;


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
