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

mgui::mgui(engine* e, gui* agui, mapeditor* me, scene* sce) {
	mgui::e = e;
	mgui::agui = agui;
	mgui::m = e->get_msg();
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

	menu_id = 0xFFFFFFFF; // there is no gui object with the id 0xFFFFFFFF
	prop_wnd_id = 0xFFFFFFFF;
	ao_wnd_id = 0xFFFFFFFF;
	od_wnd_id = 0xFFFFFFFF;
}

mgui::~mgui() {
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
							open_map_dialog();
						}
					}
					break;
					case 102: {
						if(me->is_map_opened()) {
							save_map();
						}
					}
					break;
					case 103: {
						if(me->is_map_opened()) {
							close_map();
						}
					}
					break;
					case 104: {
						open_property_wnd();
					}
					break;
					case 105: {
						if(me->is_map_opened()) {
							add_obj_dialog();
						}
					}
					break;
					case 106: {
						if(me->is_map_opened()) {
							me->delete_obj();
						}
					}
					break;
					case 201: {
						me->open_map((char*)od_dirlist->get_selected_item()->text.c_str());
						od_wnd->set_deleted(true);
					}
					break;
					case 332: {
						apply_changes();
					}
					break;
					case 407: {
						if(me->is_map_opened()) {
							create_open_dialog(420, "Add Model File", "models/", "a2m", 50, 50);
						}
					}
					break;
					case 408: {
						if(me->is_map_opened()) {
							create_open_dialog(430, "Add Animation File", "models/", "a2a", 50, 50);
						}
					}
					break;
					case 409: {
						if(me->is_map_opened()) {
							create_open_dialog(440, "Add Material File", "models/", "a2mtl", 50, 50);
						}
					}
					break;
					case 410: {
						if(strlen(ao_e_model->get_text()) == 0) {
							m->print(msg::MERROR, "mgui.cpp", "add_object(): please specify a model filename!");
							break;
						}
						if(strlen(ao_e_mat->get_text()) == 0) {
							m->print(msg::MERROR, "mgui.cpp", "add_object(): please specify a material filename!");
							break;
						}
						e->get_file_io()->open_file(ao_e_model->get_text(), file_io::OT_READ_BINARY);
						e->get_file_io()->seek(8);
						char model_type = e->get_file_io()->get_char();
						e->get_file_io()->close_file();

						if(model_type == 0x01 && strlen(ao_e_ani->get_text()) == 0) {
							m->print(msg::MERROR, "mgui.cpp", "add_object(): please specify a animation filename!");
							break;
						}

						if(model_type == 0x00) {
							me->add_object(ao_e_model->get_text(), ao_e_mat->get_text());
						}
						else {
							me->add_object(ao_e_model->get_text(), ao_e_ani->get_text(), ao_e_mat->get_text());
						}

						ao_wnd->set_deleted(true);
					}
					break;
					case 421: {
						ao_e_model->set_text((char*)od_dirlist->get_selected_item()->text.c_str());
						od_wnd->set_deleted(true);
					}
					break;
					case 431: {
						ao_e_ani->set_text((char*)od_dirlist->get_selected_item()->text.c_str());
						od_wnd->set_deleted(true);
					}
					break;
					case 441: {
						ao_e_mat->set_text((char*)od_dirlist->get_selected_item()->text.c_str());
						od_wnd->set_deleted(true);
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
			pemod_name->set_notext();
			pemod_fname->set_notext();
			peani_fname->set_notext();
			pemat_fname->set_notext();
			peposx->set_notext();
			peposy->set_notext();
			peposz->set_notext();
			peorientx->set_notext();
			peorienty->set_notext();
			peorientz->set_notext();
			pescalex->set_notext();
			pescaley->set_notext();
			pescalez->set_notext();
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
}

void mgui::save_map() {
	me->save_map();
}

void mgui::open_map_dialog() {
	create_open_dialog(200, "Open Map File", "./", "a2map");
}

void mgui::add_obj_dialog() {
	if(agui->exist(ao_wnd_id)) {
		if(!ao_wnd->get_deleted()) return;
	}

	ao_wnd_id = agui->add_window(e->get_gfx()->pnt_to_rect(30, 30, 410, 150), 400, "Add Object", true);
	ao_wnd = agui->get_window(ao_wnd_id);
	ao_model_fname = agui->get_text(agui->add_text("vera.ttf", font_size, "Model Filename", 0xFFFFFF, e->get_gfx()->cord_to_pnt(10, 14), 401, 400));
	ao_ani_fname = agui->get_text(agui->add_text("vera.ttf", font_size, "Animation Filename", 0xFFFFFF, e->get_gfx()->cord_to_pnt(10, 34), 402, 400));
	ao_mat_fname = agui->get_text(agui->add_text("vera.ttf", font_size, "Material Filename", 0xFFFFFF, e->get_gfx()->cord_to_pnt(10, 54), 403, 400));
	ao_e_model = agui->get_input(agui->add_input_box(e->get_gfx()->pnt_to_rect(115, 10, 295, 28), 404, "", 400));
	ao_e_ani = agui->get_input(agui->add_input_box(e->get_gfx()->pnt_to_rect(115, 30, 295, 48), 405, "", 400));
	ao_e_mat = agui->get_input(agui->add_input_box(e->get_gfx()->pnt_to_rect(115, 50, 295, 68), 406, "", 400));
	ao_model_browse = agui->get_button(agui->add_button(e->get_gfx()->pnt_to_rect(302, 10, 365, 28), 407, "Browse", 400));
	ao_ani_browse = agui->get_button(agui->add_button(e->get_gfx()->pnt_to_rect(302, 30, 365, 48), 408, "Browse", 400));
	ao_mat_browse = agui->get_button(agui->add_button(e->get_gfx()->pnt_to_rect(302, 50, 365, 68), 409, "Browse", 400));
	ao_add = agui->get_button(agui->add_button(e->get_gfx()->pnt_to_rect(10, 72, 365, 90), 410, "Add", 400));
}

void mgui::load_main_gui() {
	if(agui->exist(menu_id)) {
		if(!menu->get_deleted()) return;
	}

	mopen_tex = e->get_texman()->add_texture("icons/open.png", 4, GL_RGBA);
	msave_tex = e->get_texman()->add_texture("icons/save.png", 4, GL_RGBA);
	mclose_tex = e->get_texman()->add_texture("icons/close.png", 4, GL_RGBA);
	mprop_tex = e->get_texman()->add_texture("icons/prop.png", 4, GL_RGBA);
	madd_tex = e->get_texman()->add_texture("icons/add.png", 4, GL_RGBA);
	mdel_tex = e->get_texman()->add_texture("icons/del.png", 4, GL_RGBA);

	menu_id = agui->add_window(e->get_gfx()->pnt_to_rect(0, 0, e->get_screen()->w, 20), 100, "main gui", false);
	menu = agui->get_window(menu_id);
	mopen_map = agui->get_button(agui->add_button(e->get_gfx()->pnt_to_rect(1, 1, 19, 19), 0, 101, "", mopen_tex, 100));
	msave_map = agui->get_button(agui->add_button(e->get_gfx()->pnt_to_rect(20, 1, 38, 19), 0, 102, "", msave_tex, 100));
	mclose_map = agui->get_button(agui->add_button(e->get_gfx()->pnt_to_rect(39, 1, 57, 19), 0, 103, "", mclose_tex, 100));
	mproperties = agui->get_button(agui->add_button(e->get_gfx()->pnt_to_rect(58, 1, 76, 19), 0, 104, "", mprop_tex, 100));
	madd_obj = agui->get_button(agui->add_button(e->get_gfx()->pnt_to_rect(77, 1, 95, 19), 0, 105, "", madd_tex, 100));
	mdel_obj = agui->get_button(agui->add_button(e->get_gfx()->pnt_to_rect(96, 1, 114, 19), 0, 106, "", mdel_tex, 100));

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

	prop_wnd_id = agui->add_window(e->get_gfx()->pnt_to_rect(30, 30, 285, 516), 300, "Properties", true);
	prop_wnd = agui->get_window(prop_wnd_id);

	pmod_name = agui->get_text(agui->add_text("vera.ttf", font_size, "Model Name", 0xFFFFFF, e->get_gfx()->cord_to_pnt(10, 10), 301, 300));
	pmod_fname = agui->get_text(agui->add_text("vera.ttf", font_size, "Model Filename", 0xFFFFFF, e->get_gfx()->cord_to_pnt(10, 30), 302, 300));
	pani_fname = agui->get_text(agui->add_text("vera.ttf", font_size, "Animation Filename", 0xFFFFFF, e->get_gfx()->cord_to_pnt(10, 50), 303, 300));
	pmat_fname = agui->get_text(agui->add_text("vera.ttf", font_size, "Material Filename", 0xFFFFFF, e->get_gfx()->cord_to_pnt(10, 70), 304, 300));
	ppos = agui->get_text(agui->add_text("vera.ttf", font_size, "Position", 0xFFFFFF, e->get_gfx()->cord_to_pnt(10, 90), 305, 300));
	pposx = agui->get_text(agui->add_text("vera.ttf", font_size, "X", 0xFFFFFF, e->get_gfx()->cord_to_pnt(10, 110), 306, 300));
	pposy = agui->get_text(agui->add_text("vera.ttf", font_size, "Y", 0xFFFFFF, e->get_gfx()->cord_to_pnt(10, 130), 307, 300));
	pposz = agui->get_text(agui->add_text("vera.ttf", font_size, "Z", 0xFFFFFF, e->get_gfx()->cord_to_pnt(10, 150), 308, 300));
	porient = agui->get_text(agui->add_text("vera.ttf", font_size, "Orientation", 0xFFFFFF, e->get_gfx()->cord_to_pnt(10, 170), 309, 300));
	porientx = agui->get_text(agui->add_text("vera.ttf", font_size, "X", 0xFFFFFF, e->get_gfx()->cord_to_pnt(10, 190), 310, 300));
	porienty = agui->get_text(agui->add_text("vera.ttf", font_size, "Y", 0xFFFFFF, e->get_gfx()->cord_to_pnt(10, 210), 311, 300));
	porientz = agui->get_text(agui->add_text("vera.ttf", font_size, "Z", 0xFFFFFF, e->get_gfx()->cord_to_pnt(10, 230), 312, 300));
	pscale = agui->get_text(agui->add_text("vera.ttf", font_size, "Scale", 0xFFFFFF, e->get_gfx()->cord_to_pnt(10, 250), 313, 300));
	pscalex = agui->get_text(agui->add_text("vera.ttf", font_size, "X", 0xFFFFFF, e->get_gfx()->cord_to_pnt(10, 270), 314, 300));
	pscaley = agui->get_text(agui->add_text("vera.ttf", font_size, "Y", 0xFFFFFF, e->get_gfx()->cord_to_pnt(10, 290), 315, 300));
	pscalez = agui->get_text(agui->add_text("vera.ttf", font_size, "Z", 0xFFFFFF, e->get_gfx()->cord_to_pnt(10, 310), 316, 300));
	pphys_prop = agui->get_text(agui->add_text("vera.ttf", font_size, "Physical Type", 0xFFFFFF, e->get_gfx()->cord_to_pnt(10, 330), 317, 300));

	pemod_name = agui->get_input(agui->add_input_box(e->get_gfx()->pnt_to_rect(110, 6, 241, 24), 318, "", 300));
	pemod_fname = agui->get_input(agui->add_input_box(e->get_gfx()->pnt_to_rect(110, 26, 241, 44), 319, "", 300));
	peani_fname = agui->get_input(agui->add_input_box(e->get_gfx()->pnt_to_rect(110, 46, 241, 64), 320, "", 300));
	pemat_fname = agui->get_input(agui->add_input_box(e->get_gfx()->pnt_to_rect(110, 66, 241, 84), 321, "", 300));
	peposx = agui->get_input(agui->add_input_box(e->get_gfx()->pnt_to_rect(30, 106, 221, 124), 322, "", 300));
	peposy = agui->get_input(agui->add_input_box(e->get_gfx()->pnt_to_rect(30, 126, 221, 144), 323, "", 300));
	peposz = agui->get_input(agui->add_input_box(e->get_gfx()->pnt_to_rect(30, 146, 221, 164), 324, "", 300));
	peorientx = agui->get_input(agui->add_input_box(e->get_gfx()->pnt_to_rect(30, 186, 221, 204), 325, "", 300));
	peorienty = agui->get_input(agui->add_input_box(e->get_gfx()->pnt_to_rect(30, 206, 221, 224), 326, "", 300));
	peorientz = agui->get_input(agui->add_input_box(e->get_gfx()->pnt_to_rect(30, 226, 221, 244), 327, "", 300));
	pescalex = agui->get_input(agui->add_input_box(e->get_gfx()->pnt_to_rect(30, 266, 221, 284), 328, "", 300));
	pescaley = agui->get_input(agui->add_input_box(e->get_gfx()->pnt_to_rect(30, 286, 221, 304), 329, "", 300));
	pescalez = agui->get_input(agui->add_input_box(e->get_gfx()->pnt_to_rect(30, 306, 221, 324), 330, "", 300));

	plphys_prop = agui->get_list(agui->add_list_box(e->get_gfx()->pnt_to_rect(10, 346, 241, 426), 331, 300));
	plphys_prop->add_item("Box", 0);
	plphys_prop->add_item("Sphere", 1);
	plphys_prop->add_item("Cylinder", 2);
	plphys_prop->add_item("Trimesh", 3);
	plphys_prop->set_selected_id(0xFFFFFFFF);

	papply = agui->get_button(agui->add_button(e->get_gfx()->pnt_to_rect(10, 436, 241, 456), 0, 332, "Apply", 0, 300));

	prop_wnd_opened = true;
}

void mgui::apply_changes() {
	list<mapeditor::map_object>::iterator smo = me->get_sel_object();
	if(!me->is_sel()) {
		return;
	}

	memcpy(smo->model_name,	pemod_name->get_text(), 32);

	if(strcmp(smo->ani_filename, peani_fname->get_text()) != 0) {
		memcpy(smo->ani_filename, peani_fname->get_text(), 32);
		// animation filename has changed - delete old animation and load the new one
		if(smo->type) {
			smo->amodel->delete_animations();
			smo->amodel->add_animation(smo->ani_filename);
		}
	}

	if(strcmp(smo->model_filename, pemod_fname->get_text()) != 0) {
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

	if(strcmp(smo->mat_filename, pemat_fname->get_text()) != 0) {
		// material filename has changed - delete old material and load new one
		//delete smo->mat;
		me->get_materials()->erase(smo->mat);
		memcpy(smo->mat_filename, pemat_fname->get_text(), 32);

		me->get_materials()->push_back(*new a2ematerial(e));
		smo->mat = --me->get_materials()->end();
		smo->mat->load_material(smo->mat_filename);
		if(!smo->type) smo->model->set_material(&*smo->mat);
		else smo->amodel->set_material(&*smo->mat);
	}

	if(!smo->type) {
		smo->model->set_position((float)atof(peposx->get_text()), (float)atof(peposy->get_text()), (float)atof(peposz->get_text()));
		smo->model->set_rotation((float)atof(peorientx->get_text()), (float)atof(peorienty->get_text()), (float)atof(peorientz->get_text()));
		smo->model->set_scale((float)atof(pescalex->get_text()), (float)atof(pescaley->get_text()), (float)atof(pescalez->get_text()));
	}
	else {
		smo->amodel->set_position((float)atof(peposx->get_text()), (float)atof(peposy->get_text()), (float)atof(peposz->get_text()));
		smo->amodel->set_rotation((float)atof(peorientx->get_text()), (float)atof(peorienty->get_text()), (float)atof(peorientz->get_text()));
		smo->amodel->set_scale((float)atof(pescalex->get_text()), (float)atof(pescaley->get_text()), (float)atof(pescalez->get_text()));
	}

	smo->phys_type = plphys_prop->get_selected_id();

	me->reload_cur_bbox();
}

void mgui::close_map() {
	me->close_map();
}

//! TODO: should be moved to gui class
void mgui::create_open_dialog(unsigned int id, char* caption, char* dir, char* ext, unsigned int x, unsigned int y) {
	if(agui->exist(od_wnd_id)) {
		if(!od_wnd->get_deleted()) return;
	}

	od_wnd_id = agui->add_window(e->get_gfx()->pnt_to_rect(x, y, 370+x, 220+y), id, caption, true);
	od_wnd = agui->get_window(od_wnd_id);
	od_open = agui->get_button(agui->add_button(e->get_gfx()->pnt_to_rect(302, 0, 365, 20), id+1, "Open", id));
	od_cancel = agui->get_button(agui->add_button(e->get_gfx()->pnt_to_rect(302, 20, 365, 40), 1, "Cancel", id));
	od_dirlist = agui->get_list(agui->add_list_box(e->get_gfx()->pnt_to_rect(0, 0, 300, 198), id+2, id));

#ifdef WIN32
	struct _finddata_t c_file;
	intptr_t hFile;
	unsigned int i = 0;

	char* fstring = new char[strlen(dir) + 2 + strlen(ext) + 1];
	strcpy(fstring, dir);
	strcat(fstring, "*.");
	strcat(fstring, ext);
	if((hFile = _findfirst(fstring, &c_file)) != -1L) {
		do {
			unsigned int fnlen = (unsigned int)strlen(c_file.name);
			if(c_file.size != 0 &&
				c_file.name[fnlen-3] == ext[strlen(ext)-3] &&
				c_file.name[fnlen-2] == ext[strlen(ext)-2] &&
				c_file.name[fnlen-1] == ext[strlen(ext)-1]) {
				od_dirlist->add_item(c_file.name, i);
				i++;
			}
		}
		while(_findnext(hFile, &c_file) == 0);

		_findclose(hFile);
	}
	delete [] fstring;
#else
	struct dirent **namelist;
	unsigned int i = 0;

	string fstring = dir;
	fstring += ".";
	int n = scandir(fstring.c_str(), &namelist, 0, alphasort);
	if (n >= 0) {
		while(n--) {
			unsigned int fnlen = (unsigned int)strlen(namelist[n]->d_name);
			if(namelist[n]->d_type != DT_DIR &&
				namelist[n]->d_name[fnlen-3] == ext[strlen(ext)-3] &&
				namelist[n]->d_name[fnlen-2] == ext[strlen(ext)-2] &&
				namelist[n]->d_name[fnlen-1] == ext[strlen(ext)-1]) {
				od_dirlist->add_item(namelist[n]->d_name, i);
				i++;
			}
			free(namelist[n]);
		}
		free(namelist);
	}
#endif
}
