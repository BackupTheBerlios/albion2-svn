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
	mgui::g = e->get_gfx();
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

	phys_obj_selected = false;

	move_toggle = me->get_move_toggle();
	rotate_toggle = me->get_rotate_toggle();
	scale_toggle = me->get_scale_toggle();
	phys_scale_toggle = me->get_phys_scale_toggle();

	menu_id = 0xFFFFFFFF; // there is no gui object with the id 0xFFFFFFFF
	prop_wnd_id = 0xFFFFFFFF;
	ao_wnd_id = 0xFFFFFFFF;
	nm_wnd_id = 0xFFFFFFFF;
	namelist_wnd_id = 0xFFFFFFFF;
}

mgui::~mgui() {
	SDL_DestroySemaphore(mtleditor_se);
}

void mgui::run() {
	static unsigned int gui_event_id;
	while(e->get_event()->is_gui_event()) {
		gui_event_id = e->get_event()->get_gui_event().id;
		switch(e->get_event()->get_gui_event().type) {
			case event::BUTTON_PRESSED:
				switch(gui_event_id) {
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
							agui->add_msgbox_ok((char*)"Close current Map", (char*)"Please close the current Map first!");
						}
					}
					break;
					case 102: {
						if(!me->is_map_opened()) {
							open_map_dialog();
						}
						else {
							agui->add_msgbox_ok((char*)"Close current Map", (char*)"Please close the current Map first!");
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
							set_phys_obj(false);
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
							if(me->get_sel_object() == sel_phys_obj) set_phys_obj(false);
							me->delete_obj();
						}
					}
					break;
					case 108:
						me->set_draw_mode(me->get_draw_mode() ^ true);
						mwireframe_fill->set_image_texture(me->get_draw_mode() ? mfill_tex : mwireframe_tex);
						break;
					case 109:
						me->set_objects_visibility(me->get_objects_visibility() ^ true);
						me->set_physical_map(me->get_physical_map() ^ true);
						// if we currently have a phys obj selected than both, the phys obj and the
						// model itself, are visible after the command above so we have to make the
						// selected phys obj visible only
						if(phys_obj_selected) set_phys_obj(true);
						break;
					case 110:
						edit_state = mapeditor::NONE;
						break;
					case 111:
						edit_state = mapeditor::MOVE;
						break;
					case 112:
						edit_state = mapeditor::ROTATE;
						break;
					case 113:
						edit_state = mapeditor::SCALE;

						if(agui->exist(prop_wnd_id)) prop_tab->set_active_tab(1);
						else set_phys_obj(false);
						break;
					case 114:
						edit_state = mapeditor::PHYS_SCALE;

						if(agui->exist(prop_wnd_id)) prop_tab->set_active_tab(2);
						else if(me->is_sel()) set_phys_obj(true);
						break;
					case 202: {
						me->open_map(e->data_path(agui->get_open_diaolg_list()->get_selected_item()->text.c_str()));
						od_wnd->set_deleted(true);
					}
					break;
					case 328: {
						if(me->is_map_opened()) {
							mtl_fname = pf_mat_fname->get_text()->c_str();
							ani_fname = pf_ani_fname->get_text()->c_str();
							od_wnd = agui->get_object<gui_window>(agui->add_open_dialog(600, (char*)"Open Model File", e->data_path(NULL), (char*)"a2m", 50, 50, &ffilter_mdl));
						}
					}
					break;
					case 329: {
						if(me->is_map_opened()) {
							mdl_fname = pf_mdl_fname->get_text()->c_str();
							od_wnd = agui->get_object<gui_window>(agui->add_open_dialog(610, (char*)"Open Animation File", e->data_path(NULL), (char*)"a2a", 50, 50, &ffilter_ani));
						}
					}
					break;
					case 330: {
						if(me->is_map_opened()) {
							mdl_fname = pf_mdl_fname->get_text()->c_str();
							od_wnd = agui->get_object<gui_window>(agui->add_open_dialog(620, (char*)"Open Material File", e->data_path(NULL), (char*)"a2mtl", 50, 50, &ffilter_mtl));
						}
					}
					break;
					case 331: {
						if(me->is_map_opened() && me->is_sel()) {
							open_namelist_wnd();
						}
					}
					break;
					case 332: {
						if(is_thread) {
							agui->add_msgbox_ok((char*)"Material Editor already opened!", (char*)"The Material Editor is already opened! Please close it first before you try to open another instance!");
							break;
						}

						sys_call = "";
						#ifndef WIN32
						sys_call += "./";
						#endif

						sys_call += "mtleditor";

						#ifdef WIN32
						sys_call += ".exe";
						#endif

						// check if model file exists
						if(!e->get_file_io()->is_file(e->data_path((char*)pf_mdl_fname->get_text()->c_str()))) {
							agui->add_msgbox_ok((char*)"Failed opening Material Editor", (char*)"Model doesn't exist!");
							break;
						}
						sys_call += " ";
						sys_call += pf_mdl_fname->get_text()->c_str();

						// check if animation file exists (if it's an animated model)
						if(e->get_core()->is_a2eanim(e->data_path((char*)pf_mdl_fname->get_text()->c_str()))) {
							if(!e->get_file_io()->is_file(e->data_path((char*)pf_ani_fname->get_text()->c_str()))) {
								agui->add_msgbox_ok((char*)"Failed opening Material Editor", (char*)"Animation doesn't exist!");
								break;
							}
							sys_call += " ";
							sys_call += pf_ani_fname->get_text()->c_str();
						}

						// check if material file exists
						if(!e->get_file_io()->is_file(e->data_path((char*)pf_mat_fname->get_text()->c_str()))) {
							agui->add_msgbox_ok((char*)"Failed opening Material Editor", (char*)"Material doesn't exist!");
							break;
						}
						sys_call += " ";
						sys_call += pf_mat_fname->get_text()->c_str();
						mat_name = pf_mat_fname->get_text()->c_str();

						mtleditor_th = SDL_CreateThread(mtleditor_thread, NULL);
						is_thread = true;
					}
					break;

					case 363:
					case 364:
					case 365:
					case 366:
					case 367:
					case 368:
					case 369:
					case 370:
					case 371:
					case 372:
					case 373:
					case 374:
					case 389:
					case 393:
					case 394:
					case 395: {
						if(me->is_sel()) {
							list<mapeditor::map_object>::iterator sel_mobject = me->get_sel_object();

							switch(gui_event_id) {
								case 363:
									sel_mobject->type ? sel_mobject->amodel->get_position()->set(sel_mobject->position) :
														sel_mobject->model->get_position()->set(sel_mobject->position);
									break;
								case 364:
									sel_mobject->type ? sel_mobject->amodel->get_rotation()->set(sel_mobject->orientation) :
														sel_mobject->model->get_rotation()->set(sel_mobject->orientation);
									sel_mobject->type ? sel_mobject->amodel->update_mview_matrix() : sel_mobject->model->update_mview_matrix();
									break;
								case 365:
									sel_mobject->type ? sel_mobject->amodel->get_scale()->set(sel_mobject->scale) :
														sel_mobject->model->get_scale()->set(sel_mobject->scale);
									sel_mobject->type ? sel_mobject->amodel->update_scale_matrix() : sel_mobject->model->update_scale_matrix();
									break;

								case 366:
									sel_mobject->type ? sel_mobject->amodel->get_position()->x = sel_mobject->position->x :
														sel_mobject->model->get_position()->x = sel_mobject->position->x;
									break;
								case 367:
									sel_mobject->type ? sel_mobject->amodel->get_position()->y = sel_mobject->position->y :
														sel_mobject->model->get_position()->y = sel_mobject->position->y;
									break;
								case 368:
									sel_mobject->type ? sel_mobject->amodel->get_position()->z = sel_mobject->position->z :
														sel_mobject->model->get_position()->z = sel_mobject->position->z;
									break;

								case 369:
									sel_mobject->type ? sel_mobject->amodel->get_rotation()->x = sel_mobject->orientation->x :
														sel_mobject->model->get_rotation()->x = sel_mobject->orientation->x;
									sel_mobject->type ? sel_mobject->amodel->update_mview_matrix() : sel_mobject->model->update_mview_matrix();
									break;
								case 370:
									sel_mobject->type ? sel_mobject->amodel->get_rotation()->y = sel_mobject->orientation->y :
														sel_mobject->model->get_rotation()->y = sel_mobject->orientation->y;
									sel_mobject->type ? sel_mobject->amodel->update_mview_matrix() : sel_mobject->model->update_mview_matrix();
									break;
								case 371:
									sel_mobject->type ? sel_mobject->amodel->get_rotation()->z = sel_mobject->orientation->z :
														sel_mobject->model->get_rotation()->z = sel_mobject->orientation->z;
									sel_mobject->type ? sel_mobject->amodel->update_mview_matrix() : sel_mobject->model->update_mview_matrix();
									break;

								case 372:
									sel_mobject->type ? sel_mobject->amodel->get_scale()->x = sel_mobject->scale->x :
														sel_mobject->model->get_scale()->x = sel_mobject->scale->x;
									sel_mobject->type ? sel_mobject->amodel->update_scale_matrix() : sel_mobject->model->update_scale_matrix();
									break;
								case 373:
									sel_mobject->type ? sel_mobject->amodel->get_scale()->y = sel_mobject->scale->y :
														sel_mobject->model->get_scale()->y = sel_mobject->scale->y;
									sel_mobject->type ? sel_mobject->amodel->update_scale_matrix() : sel_mobject->model->update_scale_matrix();
									break;
								case 374:
									sel_mobject->type ? sel_mobject->amodel->get_scale()->z = sel_mobject->scale->z :
														sel_mobject->model->get_scale()->z = sel_mobject->scale->z;
									sel_mobject->type ? sel_mobject->amodel->update_scale_matrix() : sel_mobject->model->update_scale_matrix();
									break;

								case 389:
									sel_mobject->type ? sel_mobject->amodel->get_phys_scale()->set(sel_mobject->phys_scale) :
														sel_mobject->model->get_phys_scale()->set(sel_mobject->phys_scale);
									break;

								case 393:
									sel_mobject->type ? sel_mobject->amodel->get_phys_scale()->x = sel_mobject->phys_scale->x :
														sel_mobject->model->get_phys_scale()->x = sel_mobject->phys_scale->x;
									break;
								case 394:
									sel_mobject->type ? sel_mobject->amodel->get_phys_scale()->y = sel_mobject->phys_scale->y :
														sel_mobject->model->get_phys_scale()->y = sel_mobject->phys_scale->y;
									break;
								case 395:
									sel_mobject->type ? sel_mobject->amodel->get_phys_scale()->z = sel_mobject->phys_scale->z :
														sel_mobject->model->get_phys_scale()->z = sel_mobject->phys_scale->z;
									break;
								default:
									break;
							}

							// reload model info
							if(gui_event_id != 364 && (gui_event_id < 369 || gui_event_id > 371)) me->reload_cur_bbox();
							me->set_new_sel(true);
						}
					}
					break;
					case 399: {
						apply_changes();
					}
					break;
					case 407: {
						if(me->is_map_opened()) {
							mtl_fname = ao_e_mat->get_text()->c_str();
							ani_fname = ao_e_ani->get_text()->c_str();
							od_wnd = agui->get_object<gui_window>(agui->add_open_dialog(420, (char*)"Open Model File", e->data_path(NULL), (char*)"a2m", 50, 50, &ffilter_mdl));
						}
					}
					break;
					case 408: {
						if(me->is_map_opened()) {
							mdl_fname = ao_e_model->get_text()->c_str();
							od_wnd = agui->get_object<gui_window>(agui->add_open_dialog(430, (char*)"Open Animation File", e->data_path(NULL), (char*)"a2a", 50, 50, &ffilter_ani));
						}
					}
					break;
					case 409: {
						if(me->is_map_opened()) {
							mdl_fname = ao_e_model->get_text()->c_str();
							od_wnd = agui->get_object<gui_window>(agui->add_open_dialog(440, (char*)"Open Material File", e->data_path(NULL), (char*)"a2mtl", 50, 50, &ffilter_mtl));
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
					case 602: {
						pf_mdl_fname->set_text((char*)agui->get_open_diaolg_list()->get_selected_item()->text.c_str());
						od_wnd->set_deleted(true);
					}
					break;
					case 612: {
						pf_ani_fname->set_text((char*)agui->get_open_diaolg_list()->get_selected_item()->text.c_str());
						od_wnd->set_deleted(true);
					}
					break;
					case 622: {
						pf_mat_fname->set_text((char*)agui->get_open_diaolg_list()->get_selected_item()->text.c_str());
						od_wnd->set_deleted(true);
					}
					break;
					case 632: {
						nl_list->clear();
						if(me->is_map_opened() && me->is_sel()) {
							list<mapeditor::map_object>::iterator smo = me->get_sel_object();
							string* obj_names = (smo->type ? smo->amodel->get_object_names() : smo->model->get_object_names());
							unsigned int obj_count = (smo->type ? smo->amodel->get_object_count() : smo->model->get_object_count());
							for(unsigned int i = 0; i < obj_count; i++) {
								nl_list->add_item((char*)obj_names[i].c_str(), i);
							}
						}
					}
					break;
					case 633:
						namelist_wnd->set_deleted(true);
						break;
					default:
						break;
				}
				break;
			case event::TAB_SELECTED:
				switch(gui_event_id) {
					case 301:
						// if active tab == phys prop. tab
						if(prop_tab->get_active_tab() == 2 && me->is_sel()) {
							set_phys_obj(true);
						}
						else {
							set_phys_obj(false);
						}
						break;
					default:
						break;
				}
				break;
			case event::TOGGLE_BUTTON_PRESSED:
				switch(gui_event_id) {
					case 354:
					case 355:
					case 356:
						switch(gui_event_id) {
							case 354:
								move_toggle[1] = pt_pos_x_change->get_toggled();
								break;
							case 355:
								move_toggle[2] = pt_pos_y_change->get_toggled();
								break;
							case 356:
								move_toggle[3] = pt_pos_z_change->get_toggled();
								break;
							default:
							break;
						}
						// if all move toggle buttons aren't toggled/pressed, than set overall toggle flag to false too
						if(move_toggle[1] == false && move_toggle[2] == false && move_toggle[3] == false) move_toggle[0] = false;
						// if not, set it to true
						else move_toggle[0] = true;
						break;
					case 357:
					case 358:
					case 359:
						switch(gui_event_id) {
							case 357:
								rotate_toggle[1] = pt_rot_x_change->get_toggled();
								break;
							case 358:
								rotate_toggle[2] = pt_rot_y_change->get_toggled();
								break;
							case 359:
								rotate_toggle[3] = pt_rot_z_change->get_toggled();
								break;
							default:
							break;
						}
						// if all rotate toggle buttons aren't toggled/pressed, than set overall toggle flag to false too
						if(rotate_toggle[1] == false && rotate_toggle[2] == false && rotate_toggle[3] == false) rotate_toggle[0] = false;
						// if not, set it to true
						else rotate_toggle[0] = true;
						break;
					case 360:
					case 361:
					case 362:
						switch(gui_event_id) {
							case 360:
								scale_toggle[1] = pt_scl_x_change->get_toggled();
								break;
							case 361:
								scale_toggle[2] = pt_scl_y_change->get_toggled();
								break;
							case 362:
								scale_toggle[3] = pt_scl_z_change->get_toggled();
								break;
							default:
							break;
						}
						// if all scale toggle buttons aren't toggled/pressed, than set overall toggle flag to false too
						if(scale_toggle[1] == false && scale_toggle[2] == false && scale_toggle[3] == false) scale_toggle[0] = false;
						// if not, set it to true
						else scale_toggle[0] = true;
						break;
					case 390:
					case 391:
					case 392:
						switch(gui_event_id) {
							case 390:
								phys_scale_toggle[1] = pp_scl_x_change->get_toggled();
								break;
							case 391:
								phys_scale_toggle[2] = pp_scl_y_change->get_toggled();
								break;
							case 392:
								phys_scale_toggle[3] = pp_scl_z_change->get_toggled();
								break;
							default:
							break;
						}
						// if all phys scale toggle buttons aren't toggled/pressed, than set overall toggle flag to false too
						if(phys_scale_toggle[1] == false && phys_scale_toggle[2] == false && phys_scale_toggle[3] == false) phys_scale_toggle[0] = false;
						// if not, set it to true
						else phys_scale_toggle[0] = true;
						break;
					default:
						break;
				}
				break;
			default:
			break;
		}
	}

	if(me->get_new_sel()) {
		if(!me->is_sel()) {
			// reset phys obj
			set_phys_obj(false);
		}
		else {
			// "reset" old phys obj and set new one if current tab == "phys. properties"
			bool phys_obj = phys_obj_selected;
			set_phys_obj(false);
			sel_phys_obj = me->get_sel_object();
			if(!agui->exist(prop_wnd_id) && phys_obj) set_phys_obj(true);
		}

		if(!agui->exist(prop_wnd_id)) me->set_new_sel(false);
	}

	// update property window
	if(agui->exist(prop_wnd_id) && me->get_new_sel()) {
		if(!me->is_sel()) {
			// reset property window
			pf_mdl_name->set_text("");
			pf_mdl_fname->set_text("");
			pf_ani_fname->set_text("");
			pf_mat_fname->set_text("");
			pt_pos_x->set_text("");
			pt_pos_y->set_text("");
			pt_pos_z->set_text("");
			pt_rot_x->set_text("");
			pt_rot_y->set_text("");
			pt_rot_z->set_text("");
			pt_scl_x->set_text("");
			pt_scl_y->set_text("");
			pt_scl_z->set_text("");
			pp_type->set_selected_id(0);
			pf_mdl_subobjs->set_text((char*)"-");
			pf_mdl_vertices->set_text((char*)"-");
			pf_mdl_faces->set_text((char*)"-");
			pf_ani_joints->set_text((char*)"-");
			pf_ani_frames->set_text((char*)"-");
			pf_mat_subobjs->set_text((char*)"-");
			pf_type->set_text((char*)"-");
			pp_gravity->set_checked(false);
			pp_collision_model->set_checked(false);
			pp_auto_mass->set_checked(false);
			pp_mass->set_text("");
			pp_scl_x->set_text("");
			pp_scl_y->set_text("");
			pp_scl_z->set_text("");
		}
		else {
			list<mapeditor::map_object>::iterator smo = me->get_sel_object();
			pf_mdl_name->set_text(smo->model_name.c_str());
			pf_mdl_fname->set_text(smo->model_filename.c_str());
			pf_ani_fname->set_text(smo->ani_filename.c_str());
			pf_mat_fname->set_text(smo->mat_filename.c_str());
			pp_type->set_selected_id(smo->phys_type);

			vertex3* pos;
			vertex3* rot;
			vertex3* scl;
			vertex3* phys_scl;
			if(!smo->type) {
				pos = smo->model->get_position();
				rot = smo->model->get_rotation();
				scl = smo->model->get_scale();
				phys_scl = smo->model->get_phys_scale();
			}
			else {
				pos = smo->amodel->get_position();
				rot = smo->amodel->get_rotation();
				scl = smo->amodel->get_scale();
				phys_scl = smo->amodel->get_phys_scale();
			}

			char* tmp = new char[32];
			e->get_core()->ftoa(pos->x, tmp);
			pt_pos_x->set_text(tmp);
			e->get_core()->ftoa(pos->y, tmp);
			pt_pos_y->set_text(tmp);
			e->get_core()->ftoa(pos->z, tmp);
			pt_pos_z->set_text(tmp);
			e->get_core()->ftoa(rot->x, tmp);
			pt_rot_x->set_text(tmp);
			e->get_core()->ftoa(rot->y, tmp);
			pt_rot_y->set_text(tmp);
			e->get_core()->ftoa(rot->z, tmp);
			pt_rot_z->set_text(tmp);
			e->get_core()->ftoa(scl->x, tmp);
			pt_scl_x->set_text(tmp);
			e->get_core()->ftoa(scl->y, tmp);
			pt_scl_y->set_text(tmp);
			e->get_core()->ftoa(scl->z, tmp);
			pt_scl_z->set_text(tmp);

			// file information
			if(!smo->type) {
				snprintf(tmp, 32, "%u", smo->model->get_object_count());
				pf_mdl_subobjs->set_text(tmp);
				snprintf(tmp, 32, "%u", smo->model->get_vertex_count());
				pf_mdl_vertices->set_text(tmp);
				snprintf(tmp, 32, "%u", smo->model->get_index_count());
				pf_mdl_faces->set_text(tmp);

				pf_ani_joints->set_text((char*)"-");
				pf_ani_frames->set_text((char*)"-");
			}
			else {
				snprintf(tmp, 32, "%u", smo->amodel->get_object_count());
				pf_mdl_subobjs->set_text(tmp);
				snprintf(tmp, 32, "%u", smo->amodel->get_vertex_count());
				pf_mdl_vertices->set_text(tmp);
				snprintf(tmp, 32, "%u", smo->amodel->get_index_count());
				pf_mdl_faces->set_text(tmp);

				snprintf(tmp, 32, "%u", smo->amodel->get_current_animation()->joint_count);
				pf_ani_joints->set_text(tmp);
				snprintf(tmp, 32, "%u (%f)", smo->amodel->get_current_animation()->frame_count, smo->amodel->get_current_animation()->frame_time);
				pf_ani_frames->set_text(tmp);
			}

			snprintf(tmp, 32, "%u", smo->mat->get_object_count());
			pf_mat_subobjs->set_text(tmp);

			pp_gravity->set_checked(smo->gravity);
			pp_collision_model->set_checked(smo->collision_model);
			pp_auto_mass->set_checked(smo->auto_mass);
			e->get_core()->ftoa(smo->mass, tmp);
			pp_mass->set_text(tmp);
			e->get_core()->ftoa(phys_scl->x, tmp);
			pp_scl_x->set_text(tmp);
			e->get_core()->ftoa(phys_scl->y, tmp);
			pp_scl_y->set_text(tmp);
			e->get_core()->ftoa(phys_scl->z, tmp);
			pp_scl_z->set_text(tmp);
			
			// TODO: add "light" type ...
			if(!smo->type) {
				pf_type->set_text((char*)"static Model");
			}
			else {
				pf_type->set_text((char*)"animated Model");
			}

			delete tmp;

			// update phys obj
			if(prop_tab->get_active_tab() == 2) set_phys_obj(true);
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
	od_wnd = agui->get_object<gui_window>(agui->add_open_dialog(200, (char*)"Open Map File", e->data_path(NULL), (char*)"a2map"));
}

void mgui::new_map_dialog() {
	if(agui->exist(nm_wnd_id)) {
		if(!nm_wnd->get_deleted()) return;
	}

	nm_wnd_id = agui->add_window(g->pnt_to_rect(30, 30, 413, 119), 500, (char*)"New Map", true);
	nm_wnd = agui->get_object<gui_window>(nm_wnd_id);
	nm_mapfname = agui->get_object<gui_text>(agui->add_text((char*)"STANDARD", font_size, (char*)"Map Filename:", gs->get_color("FONT"), g->cord_to_pnt(12, 14), 501, 500));
	nm_e_mapfname = agui->get_object<gui_input>(agui->add_input_box(g->pnt_to_rect(100, 9, 369, 27), 502, (char*)".a2map", 500));
	nm_open = agui->get_object<gui_button>(agui->add_button(g->pnt_to_rect(12, 35, 368, 56), 503, (char*)"open", 0, 500));

	nm_e_mapfname->set_text_position(0);
}

void mgui::add_obj_dialog() {
	if(agui->exist(ao_wnd_id)) {
		if(!ao_wnd->get_deleted()) return;
	}

	ao_wnd_id = agui->add_window(g->pnt_to_rect(30, 30, 410, 150), 400, (char*)"Add Object", true);
	ao_wnd = agui->get_object<gui_window>(ao_wnd_id);
	ao_model_fname = agui->get_object<gui_text>(agui->add_text((char*)"STANDARD", font_size, (char*)"Model Filename", gs->get_color("FONT"), g->cord_to_pnt(10, 14), 401, 400));
	ao_ani_fname = agui->get_object<gui_text>(agui->add_text((char*)"STANDARD", font_size, (char*)"Animation Filename", gs->get_color("FONT"), g->cord_to_pnt(10, 34), 402, 400));
	ao_mat_fname = agui->get_object<gui_text>(agui->add_text((char*)"STANDARD", font_size, (char*)"Material Filename", gs->get_color("FONT"), g->cord_to_pnt(10, 54), 403, 400));
	ao_e_model = agui->get_object<gui_input>(agui->add_input_box(g->pnt_to_rect(125, 10, 334, 28), 404, (char*)"", 400));
	ao_e_ani = agui->get_object<gui_input>(agui->add_input_box(g->pnt_to_rect(125, 30, 334, 48), 405, (char*)"", 400));
	ao_e_mat = agui->get_object<gui_input>(agui->add_input_box(g->pnt_to_rect(125, 50, 334, 68), 406, (char*)"", 400));
	ao_model_browse = agui->get_object<gui_button>(agui->add_button(g->pnt_to_rect(341, 10, 365, 28), 407, (char*)"...", 0, 400));
	ao_ani_browse = agui->get_object<gui_button>(agui->add_button(g->pnt_to_rect(341, 30, 365, 48), 408, (char*)"...", 0, 400));
	ao_mat_browse = agui->get_object<gui_button>(agui->add_button(g->pnt_to_rect(341, 50, 365, 68), 409, (char*)"...", 0, 400));
	ao_add = agui->get_object<gui_button>(agui->add_button(g->pnt_to_rect(10, 72, 365, 90), 410, (char*)"Add", 0, 400));
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
	mmouse_tex = e->get_texman()->add_texture(e->data_path("icons/mouse.png"), 4, GL_RGBA);
	mmove_tex = e->get_texman()->add_texture(e->data_path("icons/move.png"), 4, GL_RGBA);
	mrotate_tex = e->get_texman()->add_texture(e->data_path("icons/rotate.png"), 4, GL_RGBA);
	mscale_tex = e->get_texman()->add_texture(e->data_path("icons/scale.png"), 4, GL_RGBA);
	mphys_scale_tex = e->get_texman()->add_texture(e->data_path("icons/phys_scale.png"), 4, GL_RGBA);
	mwireframe_tex = e->get_texman()->add_texture(e->data_path("icons/wireframe.png"), 4, GL_RGBA);
	mfill_tex = e->get_texman()->add_texture(e->data_path("icons/fill.png"), 4, GL_RGBA);
	mphys_map_tex = e->get_texman()->add_texture(e->data_path("icons/phys_map.png"), 4, GL_RGBA);
	mreset_tex = e->get_texman()->add_texture(e->data_path("icons/reset.png"), 4, GL_RGBA);
	mresetx_tex = e->get_texman()->add_texture(e->data_path("icons/resetx.png"), 4, GL_RGBA);
	mresety_tex = e->get_texman()->add_texture(e->data_path("icons/resety.png"), 4, GL_RGBA);
	mresetz_tex = e->get_texman()->add_texture(e->data_path("icons/resetz.png"), 4, GL_RGBA);

	menu_id = agui->add_window(g->pnt_to_rect(0, 0, e->get_screen()->w-1, 24), 100, (char*)"main gui", false);
	menu = agui->get_object<gui_window>(menu_id);
	mnew_map = agui->get_object<gui_button>(agui->add_button(g->pnt_to_rect(3, 3, 21, 21), 101, (char*)"", mnew_tex, 100));
	mopen_map = agui->get_object<gui_button>(agui->add_button(g->pnt_to_rect(24, 3, 42, 21), 102, (char*)"", mopen_tex, 100));
	msave_map = agui->get_object<gui_button>(agui->add_button(g->pnt_to_rect(45, 3, 63, 21), 103, (char*)"", msave_tex, 100));
	mclose_map = agui->get_object<gui_button>(agui->add_button(g->pnt_to_rect(66, 3, 84, 21), 104, (char*)"", mclose_tex, 100));
	mproperties = agui->get_object<gui_button>(agui->add_button(g->pnt_to_rect(87, 3, 105, 21), 105, (char*)"", mprop_tex, 100));
	madd_obj = agui->get_object<gui_button>(agui->add_button(g->pnt_to_rect(108, 3, 126, 21), 106, (char*)"", madd_tex, 100));
	mdel_obj = agui->get_object<gui_button>(agui->add_button(g->pnt_to_rect(129, 3, 147, 21), 107, (char*)"", mdel_tex, 100));

	mwireframe_fill = agui->get_object<gui_button>(agui->add_button(g->pnt_to_rect(154, 3, 172, 21), 108, (char*)"", mwireframe_tex, 100));
	mphys_map = agui->get_object<gui_button>(agui->add_button(g->pnt_to_rect(175, 3, 193, 21), 109, (char*)"", mphys_map_tex, 100));

	mes_mouse = agui->get_object<gui_button>(agui->add_button(g->pnt_to_rect(200, 3, 218, 21), 110, (char*)"", mmouse_tex, 100));
	mes_move = agui->get_object<gui_button>(agui->add_button(g->pnt_to_rect(221, 3, 239, 21), 111, (char*)"", mmove_tex, 100));
	mes_rotate = agui->get_object<gui_button>(agui->add_button(g->pnt_to_rect(242, 3, 260, 21), 112, (char*)"", mrotate_tex, 100));
	mes_scale = agui->get_object<gui_button>(agui->add_button(g->pnt_to_rect(263, 3, 281, 21), 113, (char*)"", mscale_tex, 100));
	mes_phys_scale = agui->get_object<gui_button>(agui->add_button(g->pnt_to_rect(284, 3, 302, 21), 114, (char*)"", mphys_scale_tex, 100));

	mnew_map->set_image_scaling(false);
	mopen_map->set_image_scaling(false);
	msave_map->set_image_scaling(false);
	mclose_map->set_image_scaling(false);
	mproperties->set_image_scaling(false);
	madd_obj->set_image_scaling(false);
	mdel_obj->set_image_scaling(false);

	mwireframe_fill->set_image_scaling(false);
	mphys_map->set_image_scaling(false);

	mes_mouse->set_image_scaling(false);
	mes_move->set_image_scaling(false);
	mes_rotate->set_image_scaling(false);
	mes_scale->set_image_scaling(false);
	mes_phys_scale->set_image_scaling(false);
}

void mgui::open_property_wnd() {
	if(agui->exist(prop_wnd_id)) {
		if(!prop_wnd->get_deleted()) return;
	}

	prop_wnd_id = agui->add_window(g->pnt_to_rect(30, 30, 356, 445), 300, (char*)"Properties", true);
	prop_wnd = agui->get_object<gui_window>(prop_wnd_id);

	prop_tab = agui->get_object<gui_tab>(agui->add_tab(g->pnt_to_rect(0, 0, 322, 371), 301, 300));
	prop_tab->add_tab("Files");
	prop_tab->add_tab("Transformation");
	prop_tab->add_tab("phys. Properties");
	prop_tab->add_tab("Misc");

	prop_apply = agui->get_object<gui_button>(agui->add_button(g->pnt_to_rect(0, 372, 322, 392), 399, (char*)"Apply", 0, 300)); 

	// tab files
	// labels
	pf_type_label = agui->get_object<gui_text>(agui->add_text((char*)"STANDARD", font_size, (char*)"Type:", gs->get_color("FONT"), g->cord_to_pnt(8, 9), 302, 300, 301));
	pf_mdl_name_label = agui->get_object<gui_text>(agui->add_text((char*)"STANDARD", font_size, (char*)"Model Name:", gs->get_color("FONT"), g->cord_to_pnt(8, 30), 303, 300, 301));
	pf_mdl_fname_label = agui->get_object<gui_text>(agui->add_text((char*)"STANDARD", font_size, (char*)"Model Filename:", gs->get_color("FONT"), g->cord_to_pnt(8, 56), 304, 300, 301));
	pf_ani_fname_label = agui->get_object<gui_text>(agui->add_text((char*)"STANDARD", font_size, (char*)"Animation Filename:", gs->get_color("FONT"), g->cord_to_pnt(8, 82), 305, 300, 301));
	pf_mat_fname_label = agui->get_object<gui_text>(agui->add_text((char*)"STANDARD", font_size, (char*)"Material Filename:", gs->get_color("FONT"), g->cord_to_pnt(8, 108), 306, 300, 301));
	pf_file_info_label = agui->get_object<gui_text>(agui->add_text((char*)"STANDARD", font_size, (char*)"File Information:", gs->get_color("FONT"), g->cord_to_pnt(8, 138), 307, 300, 301));
	pf_mdl_label = agui->get_object<gui_text>(agui->add_text((char*)"STANDARD", font_size, (char*)"Model:", gs->get_color("FONT"), g->cord_to_pnt(8, 160), 308, 300, 301));
	pf_ani_label = agui->get_object<gui_text>(agui->add_text((char*)"STANDARD", font_size, (char*)"Animation:", gs->get_color("FONT"), g->cord_to_pnt(8, 241), 309, 300, 301));
	pf_mat_label = agui->get_object<gui_text>(agui->add_text((char*)"STANDARD", font_size, (char*)"Material:", gs->get_color("FONT"), g->cord_to_pnt(8, 289), 310, 300, 301));
	pf_mdl_subobjs_label = agui->get_object<gui_text>(agui->add_text((char*)"STANDARD", font_size, (char*)"Sub-Objects:", gs->get_color("FONT"), g->cord_to_pnt(31, 173), 311, 300, 301));
	pf_mdl_vertices_label = agui->get_object<gui_text>(agui->add_text((char*)"STANDARD", font_size, (char*)"Vertices:", gs->get_color("FONT"), g->cord_to_pnt(31, 186), 312, 300, 301));
	pf_mdl_faces_label = agui->get_object<gui_text>(agui->add_text((char*)"STANDARD", font_size, (char*)"Faces:", gs->get_color("FONT"), g->cord_to_pnt(31, 199), 313, 300, 301));
	pf_ani_joints_label = agui->get_object<gui_text>(agui->add_text((char*)"STANDARD", font_size, (char*)"Joints:", gs->get_color("FONT"), g->cord_to_pnt(31, 254), 314, 300, 301));
	pf_ani_frames_label = agui->get_object<gui_text>(agui->add_text((char*)"STANDARD", font_size, (char*)"Frames (Speed):", gs->get_color("FONT"), g->cord_to_pnt(31, 267), 315, 300, 301));
	pf_mat_subobjs_label = agui->get_object<gui_text>(agui->add_text((char*)"STANDARD", font_size, (char*)"Sub-Objects:", gs->get_color("FONT"), g->cord_to_pnt(31, 302), 316, 300, 301));
	// label info
	pf_type = agui->get_object<gui_text>(agui->add_text((char*)"STANDARD", font_size, (char*)"-", gs->get_color("FONT"), g->cord_to_pnt(48, 9), 317, 300, 301));
	pf_mdl_subobjs = agui->get_object<gui_text>(agui->add_text((char*)"STANDARD", font_size, (char*)"-", gs->get_color("FONT"), g->cord_to_pnt(160, 173), 318, 300, 301));
	pf_mdl_vertices = agui->get_object<gui_text>(agui->add_text((char*)"STANDARD", font_size, (char*)"-", gs->get_color("FONT"), g->cord_to_pnt(160, 186), 319, 300, 301));
	pf_mdl_faces = agui->get_object<gui_text>(agui->add_text((char*)"STANDARD", font_size, (char*)"-", gs->get_color("FONT"), g->cord_to_pnt(160, 199), 320, 300, 301));
	pf_ani_joints = agui->get_object<gui_text>(agui->add_text((char*)"STANDARD", font_size, (char*)"-", gs->get_color("FONT"), g->cord_to_pnt(160, 254), 321, 300, 301));
	pf_ani_frames = agui->get_object<gui_text>(agui->add_text((char*)"STANDARD", font_size, (char*)"-", gs->get_color("FONT"), g->cord_to_pnt(160, 267), 322, 300, 301));
	pf_mat_subobjs = agui->get_object<gui_text>(agui->add_text((char*)"STANDARD", font_size, (char*)"-", gs->get_color("FONT"), g->cord_to_pnt(160, 302), 323, 300, 301));
	// input boxes
	pf_mdl_name = agui->get_object<gui_input>(agui->add_input_box(g->pnt_to_rect(126, 25, 306, 45), 324, (char*)"", 300, 301));
	pf_mdl_fname = agui->get_object<gui_input>(agui->add_input_box(g->pnt_to_rect(126, 51, 276, 71), 325, (char*)"", 300, 301));
	pf_ani_fname = agui->get_object<gui_input>(agui->add_input_box(g->pnt_to_rect(126, 77, 276, 97), 326, (char*)"", 300, 301));
	pf_mat_fname = agui->get_object<gui_input>(agui->add_input_box(g->pnt_to_rect(126, 103, 276, 123), 327, (char*)"", 300, 301));
	// buttons
	pf_mdl_fname_brw = agui->get_object<gui_button>(agui->add_button(g->pnt_to_rect(282, 51, 306, 71), 328, (char*)"...", 0, 300, 301));
	pf_ani_fname_brw = agui->get_object<gui_button>(agui->add_button(g->pnt_to_rect(282, 77, 306, 97), 329, (char*)"...", 0, 300, 301));
	pf_mat_fname_brw = agui->get_object<gui_button>(agui->add_button(g->pnt_to_rect(282, 103, 306, 123), 330, (char*)"...", 0, 300, 301));
	pf_mdl_subobjs_namelist = agui->get_object<gui_button>(agui->add_button(g->pnt_to_rect(31, 213, 173, 232), 331, (char*)"Sub-Objects Namelist", 0, 300, 301));
	pf_mat_edit = agui->get_object<gui_button>(agui->add_button(g->pnt_to_rect(31, 316, 173, 335), 332, (char*)"edit Material", 0, 300, 301));

	// tab transformation
	// labels
	pt_position_label = agui->get_object<gui_text>(agui->add_text((char*)"STANDARD", font_size, (char*)"Position", gs->get_color("FONT"), g->cord_to_pnt(6, 9), 333, 300, 301));
	pt_rotation_label = agui->get_object<gui_text>(agui->add_text((char*)"STANDARD", font_size, (char*)"Rotation", gs->get_color("FONT"), g->cord_to_pnt(6, 122), 334, 300, 301));
	pt_scale_label = agui->get_object<gui_text>(agui->add_text((char*)"STANDARD", font_size, (char*)"Scale", gs->get_color("FONT"), g->cord_to_pnt(6, 234), 335, 300, 301));
	pt_pos_x_label = agui->get_object<gui_text>(agui->add_text((char*)"STANDARD", font_size, (char*)"X", gs->get_color("FONT"), g->cord_to_pnt(11, 35), 336, 300, 301));
	pt_pos_y_label = agui->get_object<gui_text>(agui->add_text((char*)"STANDARD", font_size, (char*)"Y", gs->get_color("FONT"), g->cord_to_pnt(11, 61), 337, 300, 301));
	pt_pos_z_label = agui->get_object<gui_text>(agui->add_text((char*)"STANDARD", font_size, (char*)"Z", gs->get_color("FONT"), g->cord_to_pnt(11, 87), 338, 300, 301));
	pt_rot_x_label = agui->get_object<gui_text>(agui->add_text((char*)"STANDARD", font_size, (char*)"X", gs->get_color("FONT"), g->cord_to_pnt(11, 148), 339, 300, 301));
	pt_rot_y_label = agui->get_object<gui_text>(agui->add_text((char*)"STANDARD", font_size, (char*)"Y", gs->get_color("FONT"), g->cord_to_pnt(11, 174), 340, 300, 301));
	pt_rot_z_label = agui->get_object<gui_text>(agui->add_text((char*)"STANDARD", font_size, (char*)"Z", gs->get_color("FONT"), g->cord_to_pnt(11, 200), 341, 300, 301));
	pt_scl_x_label = agui->get_object<gui_text>(agui->add_text((char*)"STANDARD", font_size, (char*)"X", gs->get_color("FONT"), g->cord_to_pnt(11, 260), 342, 300, 301));
	pt_scl_y_label = agui->get_object<gui_text>(agui->add_text((char*)"STANDARD", font_size, (char*)"Y", gs->get_color("FONT"), g->cord_to_pnt(11, 286), 343, 300, 301));
	pt_scl_z_label = agui->get_object<gui_text>(agui->add_text((char*)"STANDARD", font_size, (char*)"Z", gs->get_color("FONT"), g->cord_to_pnt(11, 312), 344, 300, 301));
	// input boxes
	pt_pos_x = agui->get_object<gui_input>(agui->add_input_box(g->pnt_to_rect(31, 29, 161, 49), 345, (char*)"", 300, 301));
	pt_pos_y = agui->get_object<gui_input>(agui->add_input_box(g->pnt_to_rect(31, 55, 161, 75), 346, (char*)"", 300, 301));
	pt_pos_z = agui->get_object<gui_input>(agui->add_input_box(g->pnt_to_rect(31, 81, 161, 101), 347, (char*)"", 300, 301));
	pt_rot_x = agui->get_object<gui_input>(agui->add_input_box(g->pnt_to_rect(31, 142, 161, 162), 348, (char*)"", 300, 301));
	pt_rot_y = agui->get_object<gui_input>(agui->add_input_box(g->pnt_to_rect(31, 168, 161, 188), 349, (char*)"", 300, 301));
	pt_rot_z = agui->get_object<gui_input>(agui->add_input_box(g->pnt_to_rect(31, 194, 161, 214), 350, (char*)"", 300, 301));
	pt_scl_x = agui->get_object<gui_input>(agui->add_input_box(g->pnt_to_rect(31, 254, 161, 274), 351, (char*)"", 300, 301));
	pt_scl_y = agui->get_object<gui_input>(agui->add_input_box(g->pnt_to_rect(31, 280, 161, 300), 352, (char*)"", 300, 301));
	pt_scl_z = agui->get_object<gui_input>(agui->add_input_box(g->pnt_to_rect(31, 306, 161, 326), 353, (char*)"", 300, 301));
	// buttons
	pt_pos_x_change = agui->get_object<gui_toggle>(agui->add_toggle(g->pnt_to_rect(165, 29, 185, 49), 354, (char*)"X", 0, 300, 301));
	pt_pos_y_change = agui->get_object<gui_toggle>(agui->add_toggle(g->pnt_to_rect(165, 55, 185, 75), 355, (char*)"Y", 0, 300, 301));
	pt_pos_z_change = agui->get_object<gui_toggle>(agui->add_toggle(g->pnt_to_rect(165, 81, 185, 101), 356, (char*)"Z", 0, 300, 301));
	pt_rot_x_change = agui->get_object<gui_toggle>(agui->add_toggle(g->pnt_to_rect(165, 142, 185, 162), 357, (char*)"X", 0, 300, 301));
	pt_rot_y_change = agui->get_object<gui_toggle>(agui->add_toggle(g->pnt_to_rect(165, 168, 185, 186), 358, (char*)"Y", 0, 300, 301));
	pt_rot_z_change = agui->get_object<gui_toggle>(agui->add_toggle(g->pnt_to_rect(165, 194, 185, 214), 359, (char*)"Z", 0, 300, 301));
	pt_scl_x_change = agui->get_object<gui_toggle>(agui->add_toggle(g->pnt_to_rect(165, 254, 185, 274), 360, (char*)"X", 0, 300, 301));
	pt_scl_y_change = agui->get_object<gui_toggle>(agui->add_toggle(g->pnt_to_rect(165, 280, 185, 300), 361, (char*)"Y", 0, 300, 301));
	pt_scl_z_change = agui->get_object<gui_toggle>(agui->add_toggle(g->pnt_to_rect(165, 306, 185, 326), 362, (char*)"Z", 0, 300, 301));
	pt_pos_reset = agui->get_object<gui_button>(agui->add_button(g->pnt_to_rect(60, 3, 80, 23), 363, (char*)"", mreset_tex, 300, 301));
	pt_rot_reset = agui->get_object<gui_button>(agui->add_button(g->pnt_to_rect(60, 116, 80, 136), 364, (char*)"", mreset_tex, 300, 301));
	pt_scl_reset = agui->get_object<gui_button>(agui->add_button(g->pnt_to_rect(60, 228, 80, 248), 365, (char*)"", mreset_tex, 300, 301));
	pt_pos_x_reset = agui->get_object<gui_button>(agui->add_button(g->pnt_to_rect(188, 29, 208, 49), 366, (char*)"", mresetx_tex, 300, 301));
	pt_pos_y_reset = agui->get_object<gui_button>(agui->add_button(g->pnt_to_rect(188, 55, 208, 75), 367, (char*)"", mresety_tex, 300, 301));
	pt_pos_z_reset = agui->get_object<gui_button>(agui->add_button(g->pnt_to_rect(188, 81, 208, 101), 368, (char*)"", mresetz_tex, 300, 301));
	pt_rot_x_reset = agui->get_object<gui_button>(agui->add_button(g->pnt_to_rect(188, 142, 208, 162), 369, (char*)"", mresetx_tex, 300, 301));
	pt_rot_y_reset = agui->get_object<gui_button>(agui->add_button(g->pnt_to_rect(188, 168, 208, 186), 370, (char*)"", mresety_tex, 300, 301));
	pt_rot_z_reset = agui->get_object<gui_button>(agui->add_button(g->pnt_to_rect(188, 194, 208, 214), 371, (char*)"", mresetz_tex, 300, 301));
	pt_scl_x_reset = agui->get_object<gui_button>(agui->add_button(g->pnt_to_rect(188, 254, 208, 274), 372, (char*)"", mresetx_tex, 300, 301));
	pt_scl_y_reset = agui->get_object<gui_button>(agui->add_button(g->pnt_to_rect(188, 280, 208, 300), 373, (char*)"", mresety_tex, 300, 301));
	pt_scl_z_reset = agui->get_object<gui_button>(agui->add_button(g->pnt_to_rect(188, 306, 208, 326), 374, (char*)"", mresetz_tex, 300, 301));

	// set tab ids
	pt_position_label->set_tab(1);
	pt_rotation_label->set_tab(1);
	pt_scale_label->set_tab(1);
	pt_pos_x_label->set_tab(1);
	pt_pos_y_label->set_tab(1);
	pt_pos_z_label->set_tab(1);
	pt_rot_x_label->set_tab(1);
	pt_rot_y_label->set_tab(1);
	pt_rot_z_label->set_tab(1);
	pt_scl_x_label->set_tab(1);
	pt_scl_y_label->set_tab(1);
	pt_scl_z_label->set_tab(1);
	pt_pos_x->set_tab(1);
	pt_pos_y->set_tab(1);
	pt_pos_z->set_tab(1);
	pt_rot_x->set_tab(1);
	pt_rot_y->set_tab(1);
	pt_rot_z->set_tab(1);
	pt_scl_x->set_tab(1);
	pt_scl_y->set_tab(1);
	pt_scl_z->set_tab(1);
	pt_pos_x_change->set_tab(1);
	pt_pos_y_change->set_tab(1);
	pt_pos_z_change->set_tab(1);
	pt_rot_x_change->set_tab(1);
	pt_rot_y_change->set_tab(1);
	pt_rot_z_change->set_tab(1);
	pt_scl_x_change->set_tab(1);
	pt_scl_y_change->set_tab(1);
	pt_scl_z_change->set_tab(1);
	pt_pos_reset->set_tab(1);
	pt_rot_reset->set_tab(1);
	pt_scl_reset->set_tab(1);
	pt_pos_x_reset->set_tab(1);
	pt_pos_y_reset->set_tab(1);
	pt_pos_z_reset->set_tab(1);
	pt_rot_x_reset->set_tab(1);
	pt_rot_y_reset->set_tab(1);
	pt_rot_z_reset->set_tab(1);
	pt_scl_x_reset->set_tab(1);
	pt_scl_y_reset->set_tab(1);
	pt_scl_z_reset->set_tab(1);

	pt_pos_reset->set_image_scaling(false);
	pt_rot_reset->set_image_scaling(false);
	pt_scl_reset->set_image_scaling(false);
	pt_pos_x_reset->set_image_scaling(false);
	pt_pos_y_reset->set_image_scaling(false);
	pt_pos_z_reset->set_image_scaling(false);
	pt_rot_x_reset->set_image_scaling(false);
	pt_rot_y_reset->set_image_scaling(false);
	pt_rot_z_reset->set_image_scaling(false);
	pt_scl_x_reset->set_image_scaling(false);
	pt_scl_y_reset->set_image_scaling(false);
	pt_scl_z_reset->set_image_scaling(false);

	// tab phys properties
	// labels
	pp_type_label = agui->get_object<gui_text>(agui->add_text((char*)"STANDARD", font_size, (char*)"Type:", gs->get_color("FONT"), g->cord_to_pnt(8, 13), 375, 300, 301));
	pp_mass_label = agui->get_object<gui_text>(agui->add_text((char*)"STANDARD", font_size, (char*)"Mass:", gs->get_color("FONT"), g->cord_to_pnt(8, 64), 376, 300, 301));
	pp_scale_label = agui->get_object<gui_text>(agui->add_text((char*)"STANDARD", font_size, (char*)"Scale", gs->get_color("FONT"), g->cord_to_pnt(8, 112), 377, 300, 301));
	pp_scl_x_label = agui->get_object<gui_text>(agui->add_text((char*)"STANDARD", font_size, (char*)"X", gs->get_color("FONT"), g->cord_to_pnt(13, 138), 378, 300, 301));
	pp_scl_y_label = agui->get_object<gui_text>(agui->add_text((char*)"STANDARD", font_size, (char*)"Y", gs->get_color("FONT"), g->cord_to_pnt(13, 164), 379, 300, 301));
	pp_scl_z_label = agui->get_object<gui_text>(agui->add_text((char*)"STANDARD", font_size, (char*)"Z", gs->get_color("FONT"), g->cord_to_pnt(13, 190), 380, 300, 301));
	// input boxes
	pp_mass = agui->get_object<gui_input>(agui->add_input_box(g->pnt_to_rect(48, 59, 180, 79), 381, (char*)"", 300, 301));
	pp_scl_x = agui->get_object<gui_input>(agui->add_input_box(g->pnt_to_rect(30, 134, 156, 154), 382, (char*)"", 300, 301));
	pp_scl_y = agui->get_object<gui_input>(agui->add_input_box(g->pnt_to_rect(30, 160, 156, 180), 383, (char*)"", 300, 301));
	pp_scl_z = agui->get_object<gui_input>(agui->add_input_box(g->pnt_to_rect(30, 186, 156, 206), 384, (char*)"", 300, 301));
	// further control objects
	pp_gravity = agui->get_object<gui_check>(agui->add_check_box(g->pnt_to_rect(8, 39, 68, 56), 385, (char*)"Gravity", 300, 301));
	pp_collision_model = agui->get_object<gui_check>(agui->add_check_box(g->pnt_to_rect(84, 39, 192, 56), 386, (char*)"Collision Model", 300, 301));
	pp_auto_mass = agui->get_object<gui_check>(agui->add_check_box(g->pnt_to_rect(25, 82, 181, 100), 387, (char*)"compute automatically", 300, 301));
	pp_type = agui->get_object<gui_combo>(agui->add_combo_box(g->pnt_to_rect(48, 8, 180, 29), 388, 300, 301));
	pp_scl_reset = agui->get_object<gui_button>(agui->add_button(g->pnt_to_rect(48, 106, 68, 126), 389, (char*)"", mreset_tex, 300, 301));
	pp_scl_x_change = agui->get_object<gui_toggle>(agui->add_toggle(g->pnt_to_rect(160, 133, 180, 153), 390, (char*)"X", 0, 300, 301));
	pp_scl_y_change = agui->get_object<gui_toggle>(agui->add_toggle(g->pnt_to_rect(160, 159, 180, 179), 391, (char*)"Y", 0, 300, 301));
	pp_scl_z_change = agui->get_object<gui_toggle>(agui->add_toggle(g->pnt_to_rect(160, 185, 180, 205), 392, (char*)"Z", 0, 300, 301));
	pp_scl_x_reset = agui->get_object<gui_button>(agui->add_button(g->pnt_to_rect(183, 133, 203, 153), 393, (char*)"", mresetx_tex, 300, 301));
	pp_scl_y_reset = agui->get_object<gui_button>(agui->add_button(g->pnt_to_rect(183, 159, 203, 179), 394, (char*)"", mresety_tex, 300, 301));
	pp_scl_z_reset = agui->get_object<gui_button>(agui->add_button(g->pnt_to_rect(183, 185, 203, 205), 395, (char*)"", mresetz_tex, 300, 301));

	pp_type->add_item((char*)"Box", 0);
	pp_type->add_item((char*)"Sphere", 1);
	pp_type->add_item((char*)"Cylinder", 2);
	pp_type->add_item((char*)"Trimesh", 3);

	// set tab ids
	pp_type_label->set_tab(2);
	pp_mass_label->set_tab(2);
	pp_scale_label->set_tab(2);
	pp_scl_x_label->set_tab(2);
	pp_scl_y_label->set_tab(2);
	pp_scl_z_label->set_tab(2);
	pp_mass->set_tab(2);
	pp_scl_x->set_tab(2);
	pp_scl_y->set_tab(2);
	pp_scl_z->set_tab(2);
	pp_gravity->set_tab(2);
	pp_collision_model->set_tab(2);
	pp_auto_mass->set_tab(2);
	pp_type->set_tab(2);
	pp_scl_reset->set_tab(2);
	pp_scl_x_change->set_tab(2);
	pp_scl_y_change->set_tab(2);
	pp_scl_z_change->set_tab(2);
	pp_scl_x_reset->set_tab(2);
	pp_scl_y_reset->set_tab(2);
	pp_scl_z_reset->set_tab(2);

	pp_scl_reset->set_image_scaling(false);
	pp_scl_x_reset->set_image_scaling(false);
	pp_scl_y_reset->set_image_scaling(false);
	pp_scl_z_reset->set_image_scaling(false);

	prop_wnd_opened = true;

	// if a (phys) obj is being scaled set active tab to 1/2
	if(edit_state == mapeditor::SCALE) prop_tab->set_active_tab(1);
	else if(edit_state == mapeditor::PHYS_SCALE) prop_tab->set_active_tab(2);

	// update property window info
	me->set_new_sel(true);
}

void mgui::apply_changes() {
	list<mapeditor::map_object>::iterator smo = me->get_sel_object();
	if(!me->is_sel()) {
		return;
	}

	smo->model_name = pf_mdl_name->get_text()->c_str();

	if(smo->ani_filename != *pf_ani_fname->get_text()) {
		smo->ani_filename = pf_ani_fname->get_text()->c_str();
		// animation filename has changed - delete old animation and load the new one
		if(smo->type) {
			smo->amodel->delete_animations();
			smo->amodel->add_animation(e->data_path(smo->ani_filename.c_str()));
		}
	}

	if(smo->model_filename != *pf_mdl_fname->get_text()) {
		// model filename has been change - delete old model and load new model
		if(!smo->type) {
			sce->delete_model(&*smo->model);
			me->get_models()->erase(smo->model);
		}
		else {
			sce->delete_model(&*smo->amodel);
			me->get_amodels()->erase(smo->amodel);
		}

		smo->model_filename = pf_mdl_fname->get_text()->c_str();
		e->get_file_io()->open_file(e->data_path(smo->model_filename.c_str()), file_io::OT_READ_BINARY);
		e->get_file_io()->seek(8);
		char t = e->get_file_io()->get_char();
		e->get_file_io()->close_file();

		if(t == 0x00) {
			me->get_models()->push_back(*sce->create_a2emodel());
			smo->model = --me->get_models()->end();
			smo->model->load_model(e->data_path(smo->model_filename.c_str()), true);
			smo->model->set_material(&*smo->mat);
			sce->add_model(&*smo->model);
		}
		else {
			me->get_amodels()->push_back(*sce->create_a2eanim());
			smo->amodel = --me->get_amodels()->end();
			smo->amodel->load_model(e->data_path(smo->model_filename.c_str()), true);
			smo->amodel->add_animation(e->data_path(smo->ani_filename.c_str()));
			smo->amodel->set_material(&*smo->mat);
			sce->add_model(&*smo->amodel);
		}
	}

	if(smo->mat_filename != *pf_mat_fname->get_text()) {
		// material filename has changed - delete old material and load new one
		//delete smo->mat;
		me->get_materials()->erase(smo->mat);
		smo->mat_filename = pf_mat_fname->get_text()->c_str();

		me->get_materials()->push_back(*new a2ematerial(e));
		smo->mat = --me->get_materials()->end();
		smo->mat->load_material(e->data_path(smo->mat_filename.c_str()));
		if(!smo->type) smo->model->set_material(&*smo->mat);
		else smo->amodel->set_material(&*smo->mat);
	}

	if(!smo->type) {
		smo->model->set_position((float)atof(pt_pos_x->get_text()->c_str()), (float)atof(pt_pos_y->get_text()->c_str()), (float)atof(pt_pos_z->get_text()->c_str()));
		smo->model->set_rotation((float)atof(pt_rot_x->get_text()->c_str()), (float)atof(pt_rot_y->get_text()->c_str()), (float)atof(pt_rot_z->get_text()->c_str()));
		smo->model->set_scale((float)atof(pt_scl_x->get_text()->c_str()), (float)atof(pt_scl_y->get_text()->c_str()), (float)atof(pt_scl_z->get_text()->c_str()));
		smo->model->set_phys_scale((float)atof(pp_scl_x->get_text()->c_str()), (float)atof(pp_scl_y->get_text()->c_str()), (float)atof(pp_scl_z->get_text()->c_str()));
	}
	else {
		smo->amodel->set_position((float)atof(pt_pos_x->get_text()->c_str()), (float)atof(pt_pos_y->get_text()->c_str()), (float)atof(pt_pos_z->get_text()->c_str()));
		smo->amodel->set_rotation((float)atof(pt_rot_x->get_text()->c_str()), (float)atof(pt_rot_y->get_text()->c_str()), (float)atof(pt_rot_z->get_text()->c_str()));
		smo->amodel->set_scale((float)atof(pt_scl_x->get_text()->c_str()), (float)atof(pt_scl_y->get_text()->c_str()), (float)atof(pt_scl_z->get_text()->c_str()));
		smo->amodel->set_phys_scale((float)atof(pp_scl_x->get_text()->c_str()), (float)atof(pp_scl_y->get_text()->c_str()), (float)atof(pp_scl_z->get_text()->c_str()));
	}

	smo->phys_type = pp_type->get_selected_id();
	smo->gravity = pp_gravity->get_checked();
	smo->collision_model = pp_collision_model->get_checked();
	smo->auto_mass = pp_auto_mass->get_checked();
	smo->mass = ((float)atof(pp_mass->get_text()->c_str()));

	me->reload_cur_bbox();
}

void mgui::close_map() {
	me->close_map();
}

void mgui::update_materials(const char* mat_fname) {
	for(list<mapeditor::map_object>::iterator iter = me->get_objects()->begin(); iter != me->get_objects()->end(); iter++) {
		if(strcmp(iter->mat_filename.c_str(), mat_fname) == 0) {
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
	unsigned int mdl_obj_count = 0;

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

void mgui::open_namelist_wnd() {
	if(agui->exist(namelist_wnd_id)) {
		if(!namelist_wnd->get_deleted()) return;
	}

	namelist_wnd_id = agui->add_window(g->pnt_to_rect(50, 50, 304, 223), 630, (char*)"Sub-Objects Namelist");
	namelist_wnd = agui->get_object<gui_window>(namelist_wnd_id);

	nl_list = agui->get_object<gui_list>(agui->add_list_box(g->pnt_to_rect(0, 0, 250, 129), 631, 630));
	nl_reload = agui->get_object<gui_button>(agui->add_button(g->pnt_to_rect(0, 130, 125, 150), 632, (char*)"reload", 0, 630));
	nl_close = agui->get_object<gui_button>(agui->add_button(g->pnt_to_rect(126, 130, 250, 150), 633, (char*)"close", 0, 630));

	list<mapeditor::map_object>::iterator smo = me->get_sel_object();
	string* obj_names = (smo->type ? smo->amodel->get_object_names() : smo->model->get_object_names());
	unsigned int obj_count = (smo->type ? smo->amodel->get_object_count() : smo->model->get_object_count());
	for(unsigned int i = 0; i < obj_count; i++) {
		nl_list->add_item((char*)obj_names[i].c_str(), i);
	}
}

void mgui::set_edit_state(mapeditor::EDIT_STATE edit_state) {
	mgui::edit_state = edit_state;
}

mapeditor::EDIT_STATE mgui::get_edit_state() {
	return mgui::edit_state;
}

bool mgui::is_phys_obj_selected() {
	return mgui::phys_obj_selected;
}

list<mapeditor::map_object>::iterator mgui::get_phys_obj() {
	return mgui::sel_phys_obj;
}

void mgui::set_phys_obj(bool active) {
	if(phys_obj_selected || active) sel_phys_obj->type ? sel_phys_obj->amodel->set_visible(active ^ true) : sel_phys_obj->model->set_visible(active ^ true);
	phys_obj_selected = active;
}
