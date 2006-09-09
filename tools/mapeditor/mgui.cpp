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

#include "mgui.h"

string mgui::sys_call = "";
SDL_semaphore* mgui::mtleditor_se = NULL;

engine* mgui::e = NULL;
file_io* mgui::f = NULL;
const char* mgui::mdl_fname = NULL;
const char* mgui::mtl_fname = NULL;
const char* mgui::ani_fname = NULL;

mgui::mgui(engine* e, gui* agui, mapeditor* me, scene* sce) {
	mgui::e = e;
	mgui::agui = agui;
	mgui::gs = agui->get_gui_style();
	mgui::m = e->get_msg();
	mgui::f = e->get_file_io();
	mgui::me = me;
	mgui::sce = sce;

	menu = NULL;
	mopen_map = NULL;
	msave_map = NULL;
	mclose_map = NULL;

	prop_wnd = NULL;
	ao_wnd = NULL;
	od_wnd = NULL;

	prop_wnd_opened = false;

	font_size = 11;

	is_thread = false;

	mtleditor_se = SDL_CreateSemaphore(0);

	menu_id = 0xFFFFFFFF; // there is no gui object with the id 0xFFFFFFFF
	prop_wnd_id = 0xFFFFFFFF;
	ao_wnd_id = 0xFFFFFFFF;
	nm_wnd_id = 0xFFFFFFFF;
}

mgui::~mgui() {
	SDL_DestroySemaphore(mtleditor_se);
}

void mgui::run() {
	while(e->get_event()->is_gui_event()) {
		switch(e->get_event()->get_gui_event().type) {
			case event::BUTTON_PRESSED:
				switch(e->get_event()->get_gui_event().id) {
					// open dialog "cancel" button
					case 1: {
						od_wnd->set_deleted(true);
					}
					break;
					case 101: {
						if(!me->is_map_opened()) {
							new_map_dialog();
						}
						else {
							agui->add_msgbox_ok("Close current Map", "Please close the current Map first!");
						}
					}
					break;
					case 102: {
						if(!me->is_map_opened()) {
							open_map_dialog();
						}
						else {
							agui->add_msgbox_ok("Close current Map", "Please close the current Map first!");
						}
					}
					break;
					case 103: {
						if(me->is_map_opened()) {
							save_map();
						}
					}
					break;
					case 104: {
						if(me->is_map_opened()) {
							close_map();
						}
					}
					break;
					case 105: {
						open_property_wnd();
					}
					break;
					case 106: {
						if(me->is_map_opened()) {
							add_obj_dialog();
						}
					}
					break;
					case 107: {
						if(me->is_map_opened()) {
							me->delete_obj();
						}
					}
					break;
					case 202: {
						me->open_map(e->data_path(agui->get_open_diaolg_list()->get_selected_item()->text.c_str()));
						od_wnd->set_deleted(true);
					}
					break;
					case 332: {
						apply_changes();
					}
					break;
					case 333: {
						if(is_thread) {
							agui->add_msgbox_ok("Material Editor already opened!", "The Material Editor is already opened! Please close it first before you try to open another instance!");
							break;
						}

						sys_call = "mtleditor";

						#ifdef WIN32
						sys_call += ".exe";
						#endif

						// check if model file exists
						if(!e->get_file_io()->is_file(e->data_path((char*)pemod_fname->get_text()->c_str()))) {
							agui->add_msgbox_ok("Failed opening Material Editor", "Model doesn't exist!");
							break;
						}
						sys_call += " ";
						sys_call += pemod_fname->get_text()->c_str();

						// check if animation file exists (if it's an animated model)
						if(e->get_core()->is_a2eanim(e->data_path((char*)pemod_fname->get_text()->c_str()))) {
							if(!e->get_file_io()->is_file(e->data_path((char*)peani_fname->get_text()->c_str()))) {
								agui->add_msgbox_ok("Failed opening Material Editor", "Animation doesn't exist!");
								break;
							}
							sys_call += " ";
							sys_call += peani_fname->get_text()->c_str();
						}

						// check if material file exists
						if(!e->get_file_io()->is_file(e->data_path((char*)pemat_fname->get_text()->c_str()))) {
							agui->add_msgbox_ok("Failed opening Material Editor", "Material doesn't exist!");
							break;
						}
						sys_call += " ";
						sys_call += pemat_fname->get_text()->c_str();
						mat_name = pemat_fname->get_text()->c_str();

						mtleditor_th = SDL_CreateThread(mtleditor_thread, NULL);
						is_thread = true;
					}
					break;
					case 407: {
						if(me->is_map_opened()) {
							mtl_fname = ao_e_mat->get_text()->c_str();
							ani_fname = ao_e_ani->get_text()->c_str();
							od_wnd = agui->get_object<gui_window>(agui->add_open_dialog(420, "Add Model File", e->data_path(NULL), "a2m", 50, 50, &ffilter_mdl));
						}
					}
					break;
					case 408: {
						if(me->is_map_opened()) {
							mdl_fname = ao_e_model->get_text()->c_str();
							od_wnd = agui->get_object<gui_window>(agui->add_open_dialog(430, "Add Animation File", e->data_path(NULL), "a2a", 50, 50, &ffilter_ani));
						}
					}
					break;
					case 409: {
						if(me->is_map_opened()) {
							mdl_fname = ao_e_model->get_text()->c_str();
							od_wnd = agui->get_object<gui_window>(agui->add_open_dialog(440, "Add Material File", e->data_path(NULL), "a2mtl", 50, 50, &ffilter_mtl));
						}
					}
					break;
					case 410: {
						if(strlen(ao_e_model->get_text()->c_str()) == 0) {
							m->print(msg::MERROR, "mgui.cpp", "add_object(): please specify a model filename!");
							break;
						}
						if(strlen(ao_e_mat->get_text()->c_str()) == 0) {
							m->print(msg::MERROR, "mgui.cpp", "add_object(): please specify a material filename!");
							break;
						}

						bool ani = e->get_core()->is_a2eanim(e->data_path(ao_e_model->get_text()->c_str()));

						if(ani && strlen(ao_e_ani->get_text()->c_str()) == 0) {
							m->print(msg::MERROR, "mgui.cpp", "add_object(): please specify a animation filename!");
							break;
						}

						if(!ani) {
							me->add_object((char*)ao_e_model->get_text()->c_str(), (char*)ao_e_mat->get_text()->c_str());
						}
						else {
							me->add_object((char*)ao_e_model->get_text()->c_str(), (char*)ao_e_ani->get_text()->c_str(), (char*)ao_e_mat->get_text()->c_str());
						}

						ao_wnd->set_deleted(true);
					}
					break;
					case 422: {
						ao_e_model->set_text((char*)agui->get_open_diaolg_list()->get_selected_item()->text.c_str());
						od_wnd->set_deleted(true);
					}
					break;
					case 432: {
						ao_e_ani->set_text((char*)agui->get_open_diaolg_list()->get_selected_item()->text.c_str());
						od_wnd->set_deleted(true);
					}
					break;
					case 442: {
						ao_e_mat->set_text((char*)agui->get_open_diaolg_list()->get_selected_item()->text.c_str());
						od_wnd->set_deleted(true);
					}
					break;
					case 503: {
						me->new_map(e->data_path(nm_e_mapfname->get_text()->c_str()));
						me->open_map(e->data_path(nm_e_mapfname->get_text()->c_str()));
						nm_wnd->set_deleted(true);
					}
					break;
					default:
						break;
				}
				break;
			default:
			break;
		}
	}

	// update property window
	if(agui->exist(prop_wnd_id) && me->get_new_sel()) {
		if(!me->is_sel()) {
			// reset property window
			pemod_name->set_text("");
			pemod_fname->set_text("");
			peani_fname->set_text("");
			pemat_fname->set_text("");
			peposx->set_text("");
			peposy->set_text("");
			peposz->set_text("");
			peorientx->set_text("");
			peorienty->set_text("");
			peorientz->set_text("");
			pescalex->set_text("");
			pescaley->set_text("");
			pescalez->set_text("");
			plphys_prop->set_selected_id(0xFFFFFFFF);
		}
		else {
			list<mapeditor::map_object>::iterator smo = me->get_sel_object();
			pemod_name->set_text(smo->model_name);
			pemod_fname->set_text(smo->model_filename);
			peani_fname->set_text(smo->ani_filename);
			pemat_fname->set_text(smo->mat_filename);
			plphys_prop->set_selected_id(smo->phys_type);

			if(!smo->type) {
				char* tmp = new char[16];
				e->get_core()->ftoa(smo->model->get_position()->x, tmp);
				peposx->set_text(tmp);
				e->get_core()->ftoa(smo->model->get_position()->y, tmp);
				peposy->set_text(tmp);
				e->get_core()->ftoa(smo->model->get_position()->z, tmp);
				peposz->set_text(tmp);
				e->get_core()->ftoa(smo->model->get_rotation()->x, tmp);
				peorientx->set_text(tmp);
				e->get_core()->ftoa(smo->model->get_rotation()->y, tmp);
				peorienty->set_text(tmp);
				e->get_core()->ftoa(smo->model->get_rotation()->z, tmp);
				peorientz->set_text(tmp);
				e->get_core()->ftoa(smo->model->get_scale()->x, tmp);
				pescalex->set_text(tmp);
				e->get_core()->ftoa(smo->model->get_scale()->y, tmp);
				pescaley->set_text(tmp);
				e->get_core()->ftoa(smo->model->get_scale()->z, tmp);
				pescalez->set_text(tmp);
				delete tmp;
			}
			else {
				char* tmp = new char[16];
				e->get_core()->ftoa(smo->amodel->get_position()->x, tmp);
				peposx->set_text(tmp);
				e->get_core()->ftoa(smo->amodel->get_position()->y, tmp);
				peposy->set_text(tmp);
				e->get_core()->ftoa(smo->amodel->get_position()->z, tmp);
				peposz->set_text(tmp);
				e->get_core()->ftoa(smo->amodel->get_rotation()->x, tmp);
				peorientx->set_text(tmp);
				e->get_core()->ftoa(smo->amodel->get_rotation()->y, tmp);
				peorienty->set_text(tmp);
				e->get_core()->ftoa(smo->amodel->get_rotation()->z, tmp);
				peorientz->set_text(tmp);
				e->get_core()->ftoa(smo->amodel->get_scale()->x, tmp);
				pescalex->set_text(tmp);
				e->get_core()->ftoa(smo->amodel->get_scale()->y, tmp);
				pescaley->set_text(tmp);
				e->get_core()->ftoa(smo->amodel->get_scale()->z, tmp);
				pescalez->set_text(tmp);
				delete tmp;
			}
		}
		me->set_new_sel(false);
	}

	// check if the mtleditor thread semaphore has been increased (to 1)
	int sret = SDL_SemTryWait(mtleditor_se);
	if(sret == 0) {
		// if so, exit/wait for mtleditor thread ...
		SDL_WaitThread(mtleditor_th, NULL);

		// ... and update all materials with that material filename
		update_materials(mat_name.c_str());

		is_thread = false;
	}
	else if(sret == -1) {
		m->print(msg::MERROR, "mgui.cpp", "run(): error try waiting for semaphore!");
	}
}

void mgui::save_map() {
	me->save_map();
}

void mgui::open_map_dialog() {
	od_wnd = agui->get_object<gui_window>(agui->add_open_dialog(200, "Open Map File", e->data_path(NULL), "a2map"));
}

void mgui::new_map_dialog() {
	if(agui->exist(nm_wnd_id)) {
		if(!nm_wnd->get_deleted()) return;
	}

	nm_wnd_id = agui->add_window(e->get_gfx()->pnt_to_rect(30, 30, 413, 119), 500, "New Map", true);
	nm_wnd = agui->get_object<gui_window>(nm_wnd_id);
	nm_mapfname = agui->get_object<gui_text>(agui->add_text("STANDARD", font_size, "Map Filename:", gs->get_color("FONT"), e->get_gfx()->cord_to_pnt(12, 14), 501, 500));
	nm_e_mapfname = agui->get_object<gui_input>(agui->add_input_box(e->get_gfx()->pnt_to_rect(100, 9, 369, 27), 502, ".a2map", 500));
	nm_open = agui->get_object<gui_button>(agui->add_button(e->get_gfx()->pnt_to_rect(12, 35, 368, 56), 503, "open", 0, 500));

	nm_e_mapfname->set_text_position(0);
}

void mgui::add_obj_dialog() {
	if(agui->exist(ao_wnd_id)) {
		if(!ao_wnd->get_deleted()) return;
	}

	ao_wnd_id = agui->add_window(e->get_gfx()->pnt_to_rect(30, 30, 410, 150), 400, "Add Object", true);
	ao_wnd = agui->get_object<gui_window>(ao_wnd_id);
	ao_model_fname = agui->get_object<gui_text>(agui->add_text("STANDARD", font_size, "Model Filename", gs->get_color("FONT"), e->get_gfx()->cord_to_pnt(10, 14), 401, 400));
	ao_ani_fname = agui->get_object<gui_text>(agui->add_text("STANDARD", font_size, "Animation Filename", gs->get_color("FONT"), e->get_gfx()->cord_to_pnt(10, 34), 402, 400));
	ao_mat_fname = agui->get_object<gui_text>(agui->add_text("STANDARD", font_size, "Material Filename", gs->get_color("FONT"), e->get_gfx()->cord_to_pnt(10, 54), 403, 400));
	ao_e_model = agui->get_object<gui_input>(agui->add_input_box(e->get_gfx()->pnt_to_rect(115, 10, 295, 28), 404, "", 400));
	ao_e_ani = agui->get_object<gui_input>(agui->add_input_box(e->get_gfx()->pnt_to_rect(115, 30, 295, 48), 405, "", 400));
	ao_e_mat = agui->get_object<gui_input>(agui->add_input_box(e->get_gfx()->pnt_to_rect(115, 50, 295, 68), 406, "", 400));
	ao_model_browse = agui->get_object<gui_button>(agui->add_button(e->get_gfx()->pnt_to_rect(302, 10, 365, 28), 407, "Browse", 0, 400));
	ao_ani_browse = agui->get_object<gui_button>(agui->add_button(e->get_gfx()->pnt_to_rect(302, 30, 365, 48), 408, "Browse", 0, 400));
	ao_mat_browse = agui->get_object<gui_button>(agui->add_button(e->get_gfx()->pnt_to_rect(302, 50, 365, 68), 409, "Browse", 0, 400));
	ao_add = agui->get_object<gui_button>(agui->add_button(e->get_gfx()->pnt_to_rect(10, 72, 365, 90), 410, "Add", 0, 400));
}

void mgui::load_main_gui() {
	if(agui->exist(menu_id)) {
		if(!menu->get_deleted()) return;
	}

	mnew_tex = e->get_texman()->add_texture(e->data_path("icons/new.png"), 4, GL_RGBA);
	mopen_tex = e->get_texman()->add_texture(e->data_path("icons/open.png"), 4, GL_RGBA);
	msave_tex = e->get_texman()->add_texture(e->data_path("icons/save.png"), 4, GL_RGBA);
	mclose_tex = e->get_texman()->add_texture(e->data_path("icons/close.png"), 4, GL_RGBA);
	mprop_tex = e->get_texman()->add_texture(e->data_path("icons/prop.png"), 4, GL_RGBA);
	madd_tex = e->get_texman()->add_texture(e->data_path("icons/add.png"), 4, GL_RGBA);
	mdel_tex = e->get_texman()->add_texture(e->data_path("icons/del.png"), 4, GL_RGBA);

	menu_id = agui->add_window(e->get_gfx()->pnt_to_rect(0, 0, e->get_screen()->w, 24), 100, "main gui", false);
	menu = agui->get_object<gui_window>(menu_id);
	mnew_map = agui->get_object<gui_button>(agui->add_button(e->get_gfx()->pnt_to_rect(3, 3, 21, 21), 101, "", mnew_tex, 100));
	mopen_map = agui->get_object<gui_button>(agui->add_button(e->get_gfx()->pnt_to_rect(24, 3, 42, 21), 102, "", mopen_tex, 100));
	msave_map = agui->get_object<gui_button>(agui->add_button(e->get_gfx()->pnt_to_rect(45, 3, 63, 21), 103, "", msave_tex, 100));
	mclose_map = agui->get_object<gui_button>(agui->add_button(e->get_gfx()->pnt_to_rect(66, 3, 84, 21), 104, "", mclose_tex, 100));
	mproperties = agui->get_object<gui_button>(agui->add_button(e->get_gfx()->pnt_to_rect(87, 3, 105, 21), 105, "", mprop_tex, 100));
	madd_obj = agui->get_object<gui_button>(agui->add_button(e->get_gfx()->pnt_to_rect(108, 3, 126, 21), 106, "", madd_tex, 100));
	mdel_obj = agui->get_object<gui_button>(agui->add_button(e->get_gfx()->pnt_to_rect(129, 3, 147, 21), 107, "", mdel_tex, 100));

	mnew_map->set_image_scaling(false);
	mopen_map->set_image_scaling(false);
	msave_map->set_image_scaling(false);
	mclose_map->set_image_scaling(false);
	mproperties->set_image_scaling(false);
	madd_obj->set_image_scaling(false);
	mdel_obj->set_image_scaling(false);
}

void mgui::open_property_wnd() {
	if(agui->exist(prop_wnd_id)) {
		if(!prop_wnd->get_deleted()) return;
	}

	prop_wnd_id = agui->add_window(e->get_gfx()->pnt_to_rect(30, 30, 285, 542), 300, "Properties", true);
	prop_wnd = agui->get_object<gui_window>(prop_wnd_id);

	pmod_name = agui->get_object<gui_text>(agui->add_text("STANDARD", font_size, "Model Name", gs->get_color("FONT"), e->get_gfx()->cord_to_pnt(10, 10), 301, 300));
	pmod_fname = agui->get_object<gui_text>(agui->add_text("STANDARD", font_size, "Model Filename", gs->get_color("FONT"), e->get_gfx()->cord_to_pnt(10, 30), 302, 300));
	pani_fname = agui->get_object<gui_text>(agui->add_text("STANDARD", font_size, "Animation Filenm.", gs->get_color("FONT"), e->get_gfx()->cord_to_pnt(10, 50), 303, 300));
	pmat_fname = agui->get_object<gui_text>(agui->add_text("STANDARD", font_size, "Material Filename", gs->get_color("FONT"), e->get_gfx()->cord_to_pnt(10, 70), 304, 300));
	pedit_mat = agui->get_object<gui_button>(agui->add_button(e->get_gfx()->pnt_to_rect(110, 86, 241, 106), 333, "edit material", 0, 300));
	ppos = agui->get_object<gui_text>(agui->add_text("STANDARD", font_size, "Position", gs->get_color("FONT"), e->get_gfx()->cord_to_pnt(10, 116), 305, 300));
	pposx = agui->get_object<gui_text>(agui->add_text("STANDARD", font_size, "X", gs->get_color("FONT"), e->get_gfx()->cord_to_pnt(10, 136), 306, 300));
	pposy = agui->get_object<gui_text>(agui->add_text("STANDARD", font_size, "Y", gs->get_color("FONT"), e->get_gfx()->cord_to_pnt(10, 156), 307, 300));
	pposz = agui->get_object<gui_text>(agui->add_text("STANDARD", font_size, "Z", gs->get_color("FONT"), e->get_gfx()->cord_to_pnt(10, 176), 308, 300));
	porient = agui->get_object<gui_text>(agui->add_text("STANDARD", font_size, "Orientation", gs->get_color("FONT"), e->get_gfx()->cord_to_pnt(10, 196), 309, 300));
	porientx = agui->get_object<gui_text>(agui->add_text("STANDARD", font_size, "X", gs->get_color("FONT"), e->get_gfx()->cord_to_pnt(10, 216), 310, 300));
	porienty = agui->get_object<gui_text>(agui->add_text("STANDARD", font_size, "Y", gs->get_color("FONT"), e->get_gfx()->cord_to_pnt(10, 236), 311, 300));
	porientz = agui->get_object<gui_text>(agui->add_text("STANDARD", font_size, "Z", gs->get_color("FONT"), e->get_gfx()->cord_to_pnt(10, 256), 312, 300));
	pscale = agui->get_object<gui_text>(agui->add_text("STANDARD", font_size, "Scale", gs->get_color("FONT"), e->get_gfx()->cord_to_pnt(10, 276), 313, 300));
	pscalex = agui->get_object<gui_text>(agui->add_text("STANDARD", font_size, "X", gs->get_color("FONT"), e->get_gfx()->cord_to_pnt(10, 296), 314, 300));
	pscaley = agui->get_object<gui_text>(agui->add_text("STANDARD", font_size, "Y", gs->get_color("FONT"), e->get_gfx()->cord_to_pnt(10, 316), 315, 300));
	pscalez = agui->get_object<gui_text>(agui->add_text("STANDARD", font_size, "Z", gs->get_color("FONT"), e->get_gfx()->cord_to_pnt(10, 336), 316, 300));
	pphys_prop = agui->get_object<gui_text>(agui->add_text("STANDARD", font_size, "Physical Type", gs->get_color("FONT"), e->get_gfx()->cord_to_pnt(10, 356), 317, 300));

	pemod_name = agui->get_object<gui_input>(agui->add_input_box(e->get_gfx()->pnt_to_rect(110, 6, 241, 24), 318, "", 300));
	pemod_fname = agui->get_object<gui_input>(agui->add_input_box(e->get_gfx()->pnt_to_rect(110, 26, 241, 44), 319, "", 300));
	peani_fname = agui->get_object<gui_input>(agui->add_input_box(e->get_gfx()->pnt_to_rect(110, 46, 241, 64), 320, "", 300));
	pemat_fname = agui->get_object<gui_input>(agui->add_input_box(e->get_gfx()->pnt_to_rect(110, 66, 241, 84), 321, "", 300));
	peposx = agui->get_object<gui_input>(agui->add_input_box(e->get_gfx()->pnt_to_rect(30, 132, 136, 150), 322, "", 300));
	peposy = agui->get_object<gui_input>(agui->add_input_box(e->get_gfx()->pnt_to_rect(30, 152, 136, 170), 323, "", 300));
	peposz = agui->get_object<gui_input>(agui->add_input_box(e->get_gfx()->pnt_to_rect(30, 172, 136, 190), 324, "", 300));
	peorientx = agui->get_object<gui_input>(agui->add_input_box(e->get_gfx()->pnt_to_rect(30, 212, 136, 230), 325, "", 300));
	peorienty = agui->get_object<gui_input>(agui->add_input_box(e->get_gfx()->pnt_to_rect(30, 232, 136, 250), 326, "", 300));
	peorientz = agui->get_object<gui_input>(agui->add_input_box(e->get_gfx()->pnt_to_rect(30, 252, 136, 270), 327, "", 300));
	pescalex = agui->get_object<gui_input>(agui->add_input_box(e->get_gfx()->pnt_to_rect(30, 292, 136, 310), 328, "", 300));
	pescaley = agui->get_object<gui_input>(agui->add_input_box(e->get_gfx()->pnt_to_rect(30, 312, 136, 330), 329, "", 300));
	pescalez = agui->get_object<gui_input>(agui->add_input_box(e->get_gfx()->pnt_to_rect(30, 332, 136, 350), 330, "", 300));

	plphys_prop = agui->get_object<gui_list>(agui->add_list_box(e->get_gfx()->pnt_to_rect(10, 372, 241, 452), 331, 300));
	plphys_prop->add_item("Box", 0);
	plphys_prop->add_item("Sphere", 1);
	plphys_prop->add_item("Cylinder", 2);
	plphys_prop->add_item("Trimesh", 3);
	plphys_prop->set_selected_id(0xFFFFFFFF);

	papply = agui->get_object<gui_button>(agui->add_button(e->get_gfx()->pnt_to_rect(10, 462, 241, 482), 332, "Apply", 0, 300));

	prop_wnd_opened = true;
}

void mgui::apply_changes() {
	list<mapeditor::map_object>::iterator smo = me->get_sel_object();
	if(!me->is_sel()) {
		return;
	}

	memcpy(smo->model_name,	pemod_name->get_text()->c_str(), 32);

	if(strcmp(smo->ani_filename, peani_fname->get_text()->c_str()) != 0) {
		memcpy(smo->ani_filename, peani_fname->get_text()->c_str(), 32);
		// animation filename has changed - delete old animation and load the new one
		if(smo->type) {
			smo->amodel->delete_animations();
			smo->amodel->add_animation(smo->ani_filename);
		}
	}

	if(strcmp(smo->model_filename, pemod_fname->get_text()->c_str()) != 0) {
		// model filename has been change - delete old model and load new model
		if(!smo->type) {
			sce->delete_model(&*smo->model);
			me->get_models()->erase(smo->model);
		}
		else {
			sce->delete_model(&*smo->amodel);
			me->get_amodels()->erase(smo->amodel);
		}

		e->get_file_io()->open_file(smo->model_filename, file_io::OT_READ_BINARY);
		e->get_file_io()->seek(8);
		char t = e->get_file_io()->get_char();
		e->get_file_io()->close_file();

		if(t == 0x00) {
			me->get_models()->push_back(*sce->create_a2emodel());
			smo->model = --me->get_models()->end();
			smo->model->load_model(smo->model_filename);
			smo->model->set_material(&*smo->mat);
			sce->add_model(&*smo->model);
		}
		else {
			me->get_amodels()->push_back(*sce->create_a2eanim());
			smo->amodel = --me->get_amodels()->end();
			smo->amodel->load_model(smo->model_filename);
			smo->amodel->add_animation(smo->ani_filename);
			smo->amodel->set_material(&*smo->mat);
			sce->add_model(&*smo->amodel);
		}
	}

	if(strcmp(smo->mat_filename, pemat_fname->get_text()->c_str()) != 0) {
		// material filename has changed - delete old material and load new one
		//delete smo->mat;
		me->get_materials()->erase(smo->mat);
		memcpy(smo->mat_filename, pemat_fname->get_text()->c_str(), 64);

		me->get_materials()->push_back(*new a2ematerial(e));
		smo->mat = --me->get_materials()->end();
		smo->mat->load_material(e->data_path(smo->mat_filename));
		if(!smo->type) smo->model->set_material(&*smo->mat);
		else smo->amodel->set_material(&*smo->mat);
	}

	if(!smo->type) {
		smo->model->set_position((float)atof(peposx->get_text()->c_str()), (float)atof(peposy->get_text()->c_str()), (float)atof(peposz->get_text()->c_str()));
		smo->model->set_rotation((float)atof(peorientx->get_text()->c_str()), (float)atof(peorienty->get_text()->c_str()), (float)atof(peorientz->get_text()->c_str()));
		smo->model->set_scale((float)atof(pescalex->get_text()->c_str()), (float)atof(pescaley->get_text()->c_str()), (float)atof(pescalez->get_text()->c_str()));
	}
	else {
		smo->amodel->set_position((float)atof(peposx->get_text()->c_str()), (float)atof(peposy->get_text()->c_str()), (float)atof(peposz->get_text()->c_str()));
		smo->amodel->set_rotation((float)atof(peorientx->get_text()->c_str()), (float)atof(peorienty->get_text()->c_str()), (float)atof(peorientz->get_text()->c_str()));
		smo->amodel->set_scale((float)atof(pescalex->get_text()->c_str()), (float)atof(pescaley->get_text()->c_str()), (float)atof(pescalez->get_text()->c_str()));
	}

	smo->phys_type = plphys_prop->get_selected_id();

	me->reload_cur_bbox();
}

void mgui::close_map() {
	me->close_map();
}

void mgui::update_materials(const char* mat_fname) {
	for(list<mapeditor::map_object>::iterator iter = me->get_objects()->begin(); iter != me->get_objects()->end(); iter++) {
		if(strcmp(iter->mat_filename, mat_fname) == 0) {
			iter->mat->load_material(e->data_path(mat_fname));
		}
	}
}

int mgui::mtleditor_thread(void* data) {
	// open material editor with the current material
	system(sys_call.c_str());

	// increase semaphore (to 1), so it will be "catched" in the next run() call
	SDL_SemPost(mtleditor_se);

	return 1;
}

bool mgui::ffilter_mdl(const char* filename) {
	if(strcmp(mtl_fname, "") == 0 && strcmp(ani_fname, "") == 0) {
		return true;
	}
	else {
		bool ret = true;
		// if model object count equals material object count, set ret to true
		if(strcmp(mtl_fname, "") != 0) ret = (get_mdl_obj_count(filename) == get_mtl_obj_count(mtl_fname) ? true : false);

		// if ani_fname is set and ret is true
		if(strcmp(ani_fname, "") != 0 && ret) {
			// if model is no animated model, return false
			if(!e->get_core()->is_a2eanim(e->data_path(filename))) {
				return false;
			}
			// if model joint count equals animation joint count, set ret to true (if not, set it false)
			ret = (get_mdl_joint_count(filename) == get_ani_joint_count(ani_fname) ? true : false);
		}

		return ret;
	}
	return false;
}

bool mgui::ffilter_mtl(const char* filename) {
	if(strcmp(mdl_fname, "") == 0) {
		return true;
	}
	else {
		// if model object count equals material object count, return true
		return (get_mdl_obj_count(mdl_fname) == get_mtl_obj_count(filename) ? true : false);
	}
	return false;
}

bool mgui::ffilter_ani(const char* filename) {
	if(strcmp(mdl_fname, "") == 0) {
		return true;
	}
	else {
		// if model joint count equals animation joint count, return true
		return (get_mdl_joint_count(mdl_fname) == get_ani_joint_count(filename) ? true : false);
	}
	return false;
}

unsigned int mgui::get_mdl_obj_count(const char* mdl) {
	unsigned int mdl_obj_count;

	// get model object count
	f->open_file(e->data_path(mdl), file_io::OT_READ_BINARY);

	f->seek(8);
	unsigned int type = (unsigned int)(f->get_char() & 0xFF);

	if(type == 0 || type == 2) {
		unsigned int vertex_count = f->get_uint();
		f->seek(8 + 1 + 4 + (vertex_count * 3 * 4) + (vertex_count * 2 * 4));
		mdl_obj_count = f->get_uint();
	}
	else if(type == 1) {
		unsigned int joint_count = f->get_uint();
		f->seek(8 + 1 + 4 + 4 + joint_count * (4 + 12 + 16));
		mdl_obj_count = f->get_uint();
	}

	f->close_file();

	return mdl_obj_count;
}

unsigned int mgui::get_mtl_obj_count(const char* mtl) {
	unsigned int mtl_obj_count;

	// get material object count
	f->open_file(e->data_path(mtl), file_io::OT_READ_BINARY);

	f->seek(11);
	mtl_obj_count = f->get_uint();

	f->close_file();

	return mtl_obj_count;
}

unsigned int mgui::get_mdl_joint_count(const char* mdl) {
	unsigned int mdl_joint_count;

	// get model joint count
	f->open_file(e->data_path(mdl), file_io::OT_READ_BINARY);

	f->seek(9);
	mdl_joint_count = f->get_uint();

	f->close_file();

	return mdl_joint_count;
}

unsigned int mgui::get_ani_joint_count(const char* ani) {
	unsigned int ani_joint_count;

	// get model joint count
	f->open_file(e->data_path(ani), file_io::OT_READ_BINARY);

	f->seek(7);
	ani_joint_count = f->get_uint();

	f->close_file();

	return ani_joint_count;
}
