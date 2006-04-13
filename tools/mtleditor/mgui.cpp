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

mgui::mgui(engine* e, gui* agui, mdl* model, scene* sce) {
	mgui::e = e;
	mgui::agui = agui;
	mgui::m = e->get_msg();
	mgui::f = e->get_file_io();
	mgui::model = model;
	mgui::sce = sce;

	menu = NULL;
	obj_wnd = NULL;
	mat_wnd = NULL;

	font_size = 11;

	cur_obj = 0;
	cur_mat = NULL;

	cur_tex = 0;
	cur_rgb_arg = 0;
	cur_alpha_arg = 0;

	rot_axis = 0;
	rot[0] = 45.0f;
	rot[1] = 45.0f;
	rot[2] = 0.0f;

	mnone_tex = e->get_texman()->add_texture(e->data_path("none.png"));
	mitex_large = new image(e);
	mitex_large->set_texture(mnone_tex);

	menu_id = 0xFFFFFFFF; // there is no gui object with the id 0xFFFFFFFF
	obj_id = 0xFFFFFFFF;
	mat_id = 0xFFFFFFFF;
	tex_id = 0xFFFFFFFF;
	om_id = 0xFFFFFFFF;
	sm_id = 0xFFFFFFFF;
	omat_id = 0xFFFFFFFF;
}

mgui::~mgui() {
}

void mgui::run() {
	while(e->get_event()->is_gui_event()) {
		switch(e->get_event()->get_gui_event().type) {
			case event::BUTTON_PRESSED:
				switch(e->get_event()->get_gui_event().id) {
					// new material file
					case 101: {
						if(model->is_model()) {
							new_mtl();
						}
					}
					break;
					// open existing material file
					case 102: {
						if(model->is_model()) {
							load_omat_wnd();
						}
					}
					break;
					// save material file
					case 103: {
						if(model->is_model()) {
							load_sm_wnd();
						}
					}
					break;
					// close material file
					case 104: {
						if(model->is_model()) {
							close_mtl();
						}
					}
					break;
					// open model file
					case 105: {
						load_om_wnd();
					}
					break;
					// change rot axis to x
					case 106: {
						rot_axis = 0;
					}
					break;
					// change rot axis to y
					case 107: {
						rot_axis = 1;
					}
					break;
					// change rot axis to z
					case 108: {
						rot_axis = 2;
					}
					break;
					// subtract 10 from the rot
					case 109: {
						rot[rot_axis] -= 10.0f;
					}
					break;
					// add 10 to the rot
					case 110: {
						rot[rot_axis] += 10.0f;
					}
					break;
					// change object
					case 202: {
						update_mat(cur_mat, oobj_list->get_selected_item()->id, 0);
					}
					break;
					// change texture
					case 305:
					case 306:
					case 307:
					case 308:
					case 309:
					case 310:
					case 311:
					case 312: {
						update_mat(cur_mat, cur_obj, e->get_event()->get_gui_event().id - 305);
					}
					break;
					case 313:
					case 314:
					case 315: {
						cur_rgb_arg = e->get_event()->get_gui_event().id - 313;
						update_mat(cur_mat, cur_obj, cur_tex);
					}
					break;
					case 316:
					case 317:
					case 318: {
						cur_alpha_arg = e->get_event()->get_gui_event().id - 316;
						update_mat(cur_mat, cur_obj, cur_tex);
					}
					break;
					// opens the "change texture" window
					case 326: {
						load_tex_wnd();
						titex_fname->set_text((char*)cur_mat->get_tex_elem(cur_obj)->tex_names[cur_tex].c_str());
						titex_type->set_selected_id((cur_mat->get_tex_elem(cur_obj)->col_type[cur_tex] & 0xFF));
					}
					break;
					// open texture file dialog
					case 403: {
						ofd_wnd = agui->get_window(agui->add_open_dialog(450, "choose a texture file", e->data_path(NULL), "png"));
					}
					break;
					// apply button of the open texture file dialog
					case 404: {
						if(!e->get_file_io()->is_file(e->data_path(titex_fname->get_text()))) {
							m->print(msg::MERROR, "mgui.cpp", "cannot open file \"%s\"!", titex_fname->get_text());
							break;
						}
						cur_mat->get_tex_elem(cur_obj)->tex_names[cur_tex] = titex_fname->get_text();
						cur_mat->get_tex_elem(cur_obj)->col_type[cur_tex] = (char)titex_type->get_selected_id();
						if(cur_tex > (cur_mat->get_tex_elem(cur_obj)->tex_count-1)) { cur_mat->get_tex_elem(cur_obj)->tex_count++; }
						cur_mat->load_textures();
						update_mat(cur_mat, cur_obj, cur_tex);
						tex_wnd->set_deleted(true);
					}
					break;
					// set texture filename when the open button of the open texture file dialog is pressed
					case 451: {
						titex_fname->set_text((char*)agui->get_open_diaolg_list()->get_selected_item()->text.c_str());
						ofd_wnd->set_deleted(true);
					}
					break;
					// open model file dialog
					case 505: {
						ofd_wnd = agui->get_window(agui->add_open_dialog(520, "choose a model file", e->data_path(NULL), "a2m"));
					}
					break;
					// open animation file dialog
					case 506: {
						ofd_wnd = agui->get_window(agui->add_open_dialog(530, "choose an animation file", e->data_path(NULL), "a2a"));
					}
					break;
					// open material file dialog
					case 507: {
						ofd_wnd = agui->get_window(agui->add_open_dialog(540, "choose a material file", e->data_path(NULL), "a2mtl"));
					}
					break;
					// open model + ani + mat
					case 508: {
						// check if files exist
						if(!f->is_file(e->data_path(om_imdl_fname->get_text()))) {
							agui->add_msgbox_ok(60, "File does not exist!", "The model file does not exist!");
							m->print(msg::MERROR, "mgui.cpp", "The model file \"%s\" does not exist!", om_imdl_fname->get_text());
							break;
						}
						if(e->get_core()->is_a2eanim(e->data_path(om_imdl_fname->get_text())) && !f->is_file(e->data_path(om_iani_fname->get_text()))) {
							agui->add_msgbox_ok(60, "File does not exist!", "The animation file does not exist!");
							m->print(msg::MERROR, "mgui.cpp", "The animation file \"%s\" does not exist!", om_iani_fname->get_text());
							break;
						}
						if(strcmp(om_imat_fname->get_text(), "") != 0 && !f->is_file(e->data_path(om_imat_fname->get_text()))) {
							agui->add_msgbox_ok(60, "File does not exist!", "The material file does not exist!");
							m->print(msg::MERROR, "mgui.cpp", "The material file \"%s\" does not exist!", om_imat_fname->get_text());
							break;
						}

						open_mdl();
						om_wnd->set_deleted(true);
					}
					break;
					// set model filename when the open button of the open model file dialog is pressed
					case 521: {
						om_imdl_fname->set_text((char*)agui->get_open_diaolg_list()->get_selected_item()->text.c_str());
						ofd_wnd->set_deleted(true);
					}
					break;
					// set animation filename when the open button of the open animation file dialog is pressed
					case 531: {
						om_iani_fname->set_text((char*)agui->get_open_diaolg_list()->get_selected_item()->text.c_str());
						ofd_wnd->set_deleted(true);
					}
					break;
					// set material filename when the open button of the open material file dialog is pressed
					case 541: {
						om_imat_fname->set_text((char*)agui->get_open_diaolg_list()->get_selected_item()->text.c_str());
						ofd_wnd->set_deleted(true);
					}
					break;
					// save material ...
					case 602: {
						save_mtl();
						sm_wnd->set_deleted(true);
					}
					break;
					// open material ...
					case 702: {
						// check if files exist
						if(!f->is_file(e->data_path(omat_imat_fname->get_text()))) {
							agui->add_msgbox_ok(60, "File does not exist!", "The material file does not exist!");
							m->print(msg::MERROR, "mgui.cpp", "The material file \"%s\" does not exist!", omat_imat_fname->get_text());
							break;
						}

						open_mtl();
						omat_wnd->set_deleted(true);
					}
					break;
					default:
						break;
				}
				break;
			case event::COMBO_ITEM_SELECTED:
				switch(e->get_event()->get_gui_event().id) {
					case 304: {
						a2ematerial::texture_elem* te = cur_mat->get_tex_elem(cur_obj);
						unsigned int id = mcb_type->get_selected_id();
						unsigned int oid = (te->mat_type & 0xFF);
						if(id != oid) {
							switch(oid) {
								case a2ematerial::DIFFUSE:
									if(id == a2ematerial::PARALLAX) {
										te->tex_count = 3;
										for(unsigned int i = 1; i < 3; i++) {
											te->col_type[i] = 0;
											te->textures[i] = mnone_tex;
											te->tex_names[i] = "none.png";
										}
									}
									else if(id == a2ematerial::MULTITEXTURE) {
										te->tex_count = 1;
										for(unsigned int i = 1; i < 8; i++) {
											te->col_type[i] = 0;
											te->textures[i] = mnone_tex;
											te->tex_names[i] = "none.png";
										}
										for(unsigned int i = 0; i < 8; i++) {
											te->rgb_combine[i] = (char)0xFF;
											te->alpha_combine[i] = (char)0xFF;
											for(unsigned int j = 0; j < 3; j++) {
												te->rgb_source[i][j] = 0;
												te->rgb_operand[i][j] = 0;
												te->alpha_source[i][j] = 0;
												te->alpha_operand[i][j] = 0;
											}
										}
									}
									break;
								case a2ematerial::PARALLAX:
									if(id == a2ematerial::DIFFUSE) {
										te->tex_count = 1;
									}
									else if(id == a2ematerial::MULTITEXTURE) {
										te->tex_count = 3;
										for(unsigned int i = 3; i < 8; i++) {
											te->col_type[i] = 0;
											te->textures[i] = mnone_tex;
											te->tex_names[i] = "none.png";
										}
										for(unsigned int i = 0; i < 8; i++) {
											te->rgb_combine[i] = (char)0xFF;
											te->alpha_combine[i] = (char)0xFF;
											for(unsigned int j = 0; j < 3; j++) {
												te->rgb_source[i] = 0;
												te->rgb_operand[i] = 0;
												te->alpha_source[i] = 0;
												te->alpha_operand[i] = 0;
											}
										}
									}
									break;
								case a2ematerial::MULTITEXTURE:
									if(id == a2ematerial::DIFFUSE) {
										te->tex_count = 1;
									}
									else if(id == a2ematerial::PARALLAX) {
										for(unsigned int i = te->tex_count; i < 3; i++) {
											te->col_type[i] = 0;
											te->textures[i] = mnone_tex;
											te->tex_names[i] = "none.png";
										}
										te->tex_count = 3;
									}
									break;
								default:
									break;
							}
							te->mat_type = id;
							update_mat(cur_mat, cur_obj, cur_tex);
						}
					}
					break;
					case 319: {
						unsigned int sid = mcb_rgb_combine->get_selected_id();
						cur_mat->get_tex_elem(cur_obj)->rgb_combine[cur_tex] = sid;
						update_mat(cur_mat, cur_obj, cur_tex);
					}
					break;
					case 320: {
						unsigned int sid = mcb_alpha_combine->get_selected_id();
						cur_mat->get_tex_elem(cur_obj)->alpha_combine[cur_tex] = sid;
						update_mat(cur_mat, cur_obj, cur_tex);
					}
					break;
					case 321: {
						unsigned int sid = mcb_rgb_src->get_selected_id();
						cur_mat->get_tex_elem(cur_obj)->rgb_source[cur_tex][cur_rgb_arg] = sid;
						update_mat(cur_mat, cur_obj, cur_tex);
					}
					break;
					case 322: {
						unsigned int sid = mcb_rgb_op->get_selected_id();
						cur_mat->get_tex_elem(cur_obj)->rgb_operand[cur_tex][cur_rgb_arg] = sid;
						update_mat(cur_mat, cur_obj, cur_tex);
					}
					break;
					case 323: {
						unsigned int sid = mcb_alpha_src->get_selected_id();
						cur_mat->get_tex_elem(cur_obj)->alpha_source[cur_tex][cur_alpha_arg] = sid;
						update_mat(cur_mat, cur_obj, cur_tex);
					}
					break;
					case 324: {
						unsigned int sid = mcb_alpha_op->get_selected_id();
						cur_mat->get_tex_elem(cur_obj)->alpha_operand[cur_tex][cur_alpha_arg] = sid;
						update_mat(cur_mat, cur_obj, cur_tex);
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

	if(agui->exist(mat_id)) {
		if(!mat_wnd->get_deleted()) {
			mitex_large->set_position(mat_wnd->get_rectangle()->x1 + 151, mat_wnd->get_rectangle()->y1 + 28);
			mitex_large->draw(192, 192);
		}
	}

	// rotation is somehow bugged .....
	//if(model->is_model()) model->ani ? model->amodel->set_rotation(rot[0], rot[1], rot[2]) : model->model->set_rotation(rot[0], rot[1], rot[2]);
}

void mgui::load_main_gui() {
	if(agui->exist(menu_id)) {
		if(!menu->get_deleted()) return;
	}

	mnew_tex = e->get_texman()->add_texture(e->data_path("icons/new.png"), 4, GL_RGBA);
	mopen_tex = e->get_texman()->add_texture(e->data_path("icons/open.png"), 4, GL_RGBA);
	msave_tex = e->get_texman()->add_texture(e->data_path("icons/save.png"), 4, GL_RGBA);
	mclose_tex = e->get_texman()->add_texture(e->data_path("icons/close.png"), 4, GL_RGBA);
	mopen_model_tex = e->get_texman()->add_texture(e->data_path("icons/open_model.png"), 4, GL_RGBA);

	menu_id = agui->add_window(e->get_gfx()->pnt_to_rect(0, 0, e->get_screen()->w, 20), 100, "main gui", false);
	menu = agui->get_window(menu_id);
	mnew_mtl = agui->get_button(agui->add_button(e->get_gfx()->pnt_to_rect(1, 1, 19, 19), 0, 101, "", mnew_tex, 100));
	mopen_mtl = agui->get_button(agui->add_button(e->get_gfx()->pnt_to_rect(20, 1, 38, 19), 0, 102, "", mopen_tex, 100));
	msave_mtl = agui->get_button(agui->add_button(e->get_gfx()->pnt_to_rect(39, 1, 57, 19), 0, 103, "", msave_tex, 100));
	mclose_mtl = agui->get_button(agui->add_button(e->get_gfx()->pnt_to_rect(58, 1, 76, 19), 0, 104, "", mclose_tex, 100));

	mopen_model = agui->get_button(agui->add_button(e->get_gfx()->pnt_to_rect(77, 1, 117, 19), 0, 105, "", mopen_model_tex, 100));

	mnew_mtl->set_image_scaling(false);
	mopen_mtl->set_image_scaling(false);
	msave_mtl->set_image_scaling(false);
	mclose_mtl->set_image_scaling(false);
	mopen_model->set_image_scaling(false);

	// rotation is somehow bugged .....
	/*mrot_axis_x = agui->get_button(agui->add_button(e->get_gfx()->pnt_to_rect(150, 1, 168, 19), 0, 106, "X", 0, 100));
	mrot_axis_y = agui->get_button(agui->add_button(e->get_gfx()->pnt_to_rect(169, 1, 187, 19), 0, 107, "Y", 0, 100));
	mrot_axis_z = agui->get_button(agui->add_button(e->get_gfx()->pnt_to_rect(188, 1, 207, 19), 0, 108, "Z", 0, 100));
	mrot_l = agui->get_button(agui->add_button(e->get_gfx()->pnt_to_rect(208, 1, 226, 19), 0, 109, "<|", 0, 100));
	mrot_r = agui->get_button(agui->add_button(e->get_gfx()->pnt_to_rect(227, 1, 245, 19), 0, 110, "|>", 0, 100));*/
}

void mgui::load_obj_wnd() {
	if(agui->exist(obj_id)) {
		if(!obj_wnd->get_deleted()) return;
	}

	obj_id = agui->add_window(e->get_gfx()->pnt_to_rect(30, 30, 255, 375), 200, "sub-objects", true);
	obj_wnd = agui->get_window(obj_id);
	oobj_list = agui->get_list(agui->add_list_box(e->get_gfx()->pnt_to_rect(4, 4, 216, 296), 201, 200));
	ochange = agui->get_button(agui->add_button(e->get_gfx()->pnt_to_rect(4, 300, 216, 320), 0, 202, "change", 0, 200));
}

void mgui::load_mat_wnd() {
	if(agui->exist(mat_id)) {
		if(!mat_wnd->get_deleted()) return;
	}

	mat_id = agui->add_window(e->get_gfx()->pnt_to_rect(630, 30, 1000, 386), 300, "Material Properties", true);
	mat_wnd = agui->get_window(mat_id);

	mtype = agui->get_text(agui->add_text("STANDARD", font_size, "Type", 0xFFFFFF, e->get_gfx()->cord_to_pnt(15, 9), 301, 300));
	mrgb = agui->get_text(agui->add_text("STANDARD", font_size, "RGB Combine", 0xFFFFFF, e->get_gfx()->cord_to_pnt(151, 207), 302, 300));
	malpha = agui->get_text(agui->add_text("STANDARD", font_size, "Alpha Combine", 0xFFFFFF, e->get_gfx()->cord_to_pnt(252, 207), 303, 300));

	mcb_type = agui->get_combo(agui->add_combo_box(e->get_gfx()->pnt_to_rect(15, 22, 131, 42), 304, 300));
	mcb_type->add_item("NONE", 0);
	mcb_type->add_item("DIFFUSE", 1);
	mcb_type->add_item("BUMP (no support)", 2);
	mcb_type->add_item("PARALLAX", 3);
	mcb_type->add_item("MULTI-TEXTURE", 4);

	mtex = new gui_button*[8];
	mtex[0] = agui->get_button(agui->add_button(e->get_gfx()->pnt_to_rect(15, 47, 70, 102), 0, 305, "Tex #1", mnone_tex, 300));
	mtex[1] = agui->get_button(agui->add_button(e->get_gfx()->pnt_to_rect(76, 47, 131, 102), 0, 306, "Tex #2", mnone_tex, 300));
	mtex[2] = agui->get_button(agui->add_button(e->get_gfx()->pnt_to_rect(15, 105, 70, 160), 0, 307, "Tex #3", mnone_tex, 300));
	mtex[3] = agui->get_button(agui->add_button(e->get_gfx()->pnt_to_rect(76, 105, 131, 160), 0, 308, "Tex #4", mnone_tex, 300));
	mtex[4] = agui->get_button(agui->add_button(e->get_gfx()->pnt_to_rect(15, 163, 70, 218), 0, 309, "Tex #5", mnone_tex, 300));
	mtex[5] = agui->get_button(agui->add_button(e->get_gfx()->pnt_to_rect(76, 163, 131, 218), 0, 310, "Tex #6", mnone_tex, 300));
	mtex[6] = agui->get_button(agui->add_button(e->get_gfx()->pnt_to_rect(15, 221, 70, 276), 0, 311, "Tex #7", mnone_tex, 300));
	mtex[7] = agui->get_button(agui->add_button(e->get_gfx()->pnt_to_rect(76, 221, 131, 276), 0, 312, "Tex #8", mnone_tex, 300));
	//mapply = agui->get_button(agui->add_button(e->get_gfx()->pnt_to_rect(15, 278, 131, 297), 0, 325, "apply", 0, 300));
	mchange_tex = agui->get_button(agui->add_button(e->get_gfx()->pnt_to_rect(15, 298, 131, 317), 0, 326, "change texture", 0, 300));
	
	mrgb_arg = new gui_button*[3];
	malpha_arg = new gui_button*[3];
	mrgb_arg[0] = agui->get_button(agui->add_button(e->get_gfx()->pnt_to_rect(153, 251, 174, 271), 0, 313, "1", 0, 300));
	mrgb_arg[1] = agui->get_button(agui->add_button(e->get_gfx()->pnt_to_rect(175, 251, 196, 271), 0, 314, "2", 0, 300));
	mrgb_arg[2] = agui->get_button(agui->add_button(e->get_gfx()->pnt_to_rect(197, 251, 218, 271), 0, 315, "3", 0, 300));
	malpha_arg[0] = agui->get_button(agui->add_button(e->get_gfx()->pnt_to_rect(252, 251, 273, 271), 0, 316, "1", 0, 300));
	malpha_arg[1] = agui->get_button(agui->add_button(e->get_gfx()->pnt_to_rect(274, 251, 295, 271), 0, 317, "2", 0, 300));
	malpha_arg[2] = agui->get_button(agui->add_button(e->get_gfx()->pnt_to_rect(296, 251, 317, 271), 0, 318, "3", 0, 300));

	mcb_rgb_combine = agui->get_combo(agui->add_combo_box(e->get_gfx()->pnt_to_rect(151, 224, 242, 244), 319, 300));
	mcb_alpha_combine = agui->get_combo(agui->add_combo_box(e->get_gfx()->pnt_to_rect(251, 224, 342, 244), 320, 300));
	mcb_rgb_src = agui->get_combo(agui->add_combo_box(e->get_gfx()->pnt_to_rect(153, 274, 244, 294), 321, 300));
	mcb_rgb_op = agui->get_combo(agui->add_combo_box(e->get_gfx()->pnt_to_rect(153, 297, 244, 317), 322, 300));
	mcb_alpha_src = agui->get_combo(agui->add_combo_box(e->get_gfx()->pnt_to_rect(252, 274, 343, 294), 323, 300));
	mcb_alpha_op = agui->get_combo(agui->add_combo_box(e->get_gfx()->pnt_to_rect(252, 297, 343, 317), 324, 300));

	mcb_rgb_combine->add_item("REPLACE", 0);
	mcb_rgb_combine->add_item("MODULATE", 1);
	mcb_rgb_combine->add_item("ADD", 2);
	mcb_rgb_combine->add_item("ADD SIGNED", 3);
	mcb_rgb_combine->add_item("SUBSTRACT", 4);
	mcb_rgb_combine->add_item("INTERPOLATE", 5);
	mcb_rgb_combine->add_item("NONE", 255);
	mcb_rgb_combine->set_selected_id(255);

	mcb_alpha_combine->add_item("REPLACE", 0);
	mcb_alpha_combine->add_item("MODULATE", 1);
	mcb_alpha_combine->add_item("ADD", 2);
	mcb_alpha_combine->add_item("ADD SIGNED", 3);
	mcb_alpha_combine->add_item("SUBSTRACT", 4);
	mcb_alpha_combine->add_item("INTERPOLATE", 5);
	mcb_alpha_combine->add_item("NONE", 255);
	mcb_alpha_combine->set_selected_id(255);

	mcb_rgb_src->add_item("PRIM COLOR", 0);
	mcb_rgb_src->add_item("TEXTURE", 1);
	mcb_rgb_src->add_item("CONSTANT", 2);
	mcb_rgb_src->add_item("PREVIOUS", 3);

	mcb_rgb_op->add_item("SRC COLOR", 0);
	mcb_rgb_op->add_item("1 - SRC COLOR", 1);
	mcb_rgb_op->add_item("SRC_ALPHA", 2);
	mcb_rgb_op->add_item("1 - SRC ALPHA", 3);

	mcb_alpha_src->add_item("PRIM COLOR", 0);
	mcb_alpha_src->add_item("TEXTURE", 1);
	mcb_alpha_src->add_item("CONSTANT", 2);
	mcb_alpha_src->add_item("PREVIOUS", 3);

	mcb_alpha_op->add_item("SRC ALPHA", 0);
	mcb_alpha_op->add_item("1 - SRC ALPHA", 1);
}

void mgui::load_tex_wnd() {
	if(agui->exist(tex_id)) {
		if(!tex_wnd->get_deleted()) return;
	}

	tex_id = agui->add_window(e->get_gfx()->pnt_to_rect(270, 30, 656, 145), 400, "load texure ...", true);
	tex_wnd = agui->get_window(tex_id);

	ttex_fname = agui->get_text(agui->add_text("STANDARD", font_size, "texture filename:", 0xFFFFFF, e->get_gfx()->cord_to_pnt(12, 12), 401, 400));
	ttex_type = agui->get_text(agui->add_text("STANDARD", font_size, "texture type:", 0xFFFFFF, e->get_gfx()->cord_to_pnt(12, 35), 402, 400));

	tbrowse = agui->get_button(agui->add_button(e->get_gfx()->pnt_to_rect(339, 8, 368, 28), 0, 403, "...", 0, 400));
	tapply = agui->get_button(agui->add_button(e->get_gfx()->pnt_to_rect(15, 62, 368, 83), 0, 404, "apply", 0, 400));

	titex_fname = agui->get_input(agui->add_input_box(e->get_gfx()->pnt_to_rect(110, 8, 333, 28), 405, "", 400));

	titex_type = agui->get_combo(agui->add_combo_box(e->get_gfx()->pnt_to_rect(110, 35, 210, 55), 406, 400));
	titex_type->add_item("RGB", 0);
	titex_type->add_item("RGBA", 1);
	titex_type->set_selected_id(0);
}

void mgui::load_om_wnd() {
	if(agui->exist(om_id)) {
		if(!om_wnd->get_deleted()) return;
	}

	om_id = agui->add_window(e->get_gfx()->pnt_to_rect(270, 30, 652, 180), 500, "load model/animation/material ...", true);
	om_wnd = agui->get_window(om_id);

	om_mdl_fname = agui->get_text(agui->add_text("STANDARD", font_size, "model filename:", 0xFFFFFF, e->get_gfx()->cord_to_pnt(12, 9), 501, 500));
	om_ani_fname = agui->get_text(agui->add_text("STANDARD", font_size, "animation filename:", 0xFFFFFF, e->get_gfx()->cord_to_pnt(12, 32), 502, 500));
	om_mat_fname = agui->get_text(agui->add_text("STANDARD", font_size, "material filename:*", 0xFFFFFF, e->get_gfx()->cord_to_pnt(12, 55), 503, 500));
	om_mat_des = agui->get_text(agui->add_text("STANDARD", font_size, "*(if empty, a new one will be created)", 0xFFFFFF, e->get_gfx()->cord_to_pnt(160, 75), 504, 500));

	om_browse_mdl = agui->get_button(agui->add_button(e->get_gfx()->pnt_to_rect(339, 5, 368, 25), 0, 505, "...", 0, 500));
	om_browse_ani = agui->get_button(agui->add_button(e->get_gfx()->pnt_to_rect(339, 29, 368, 49), 0, 506, "...", 0, 500));
	om_browse_mat = agui->get_button(agui->add_button(e->get_gfx()->pnt_to_rect(339, 52, 368, 72), 0, 507, "...", 0, 500));
	om_open = agui->get_button(agui->add_button(e->get_gfx()->pnt_to_rect(12, 97, 368, 118), 0, 508, "open", 0, 500));

	om_imdl_fname = agui->get_input(agui->add_input_box(e->get_gfx()->pnt_to_rect(125, 5, 333, 25), 509, "", 500));
	om_iani_fname = agui->get_input(agui->add_input_box(e->get_gfx()->pnt_to_rect(125, 29, 333, 49), 510, "", 500));
	om_imat_fname = agui->get_input(agui->add_input_box(e->get_gfx()->pnt_to_rect(125, 52, 333, 72), 511, "", 500));
}

void mgui::load_sm_wnd() {
	if(agui->exist(sm_id)) {
		if(!sm_wnd->get_deleted()) return;
	}

	sm_id = agui->add_window(e->get_gfx()->pnt_to_rect(270, 30, 653, 119), 600, "save material ...", true);
	sm_wnd = agui->get_window(sm_id);

	sm_mat_fname = agui->get_text(agui->add_text("STANDARD", font_size, "material filename:", 0xFFFFFF, e->get_gfx()->cord_to_pnt(9, 12), 601, 600));

	sm_save = agui->get_button(agui->add_button(e->get_gfx()->pnt_to_rect(12, 35, 368, 56), 0, 602, "save", 0, 600));

	sm_imat_fname = agui->get_input(agui->add_input_box(e->get_gfx()->pnt_to_rect(120, 9, 369, 29), 603, (char*)model->mat_fname.c_str(), 600));
}

void mgui::load_omat_wnd() {
	if(agui->exist(omat_id)) {
		if(!omat_wnd->get_deleted()) return;
	}

	omat_id = agui->add_window(e->get_gfx()->pnt_to_rect(270, 30, 653, 119), 700, "open material ...", true);
	omat_wnd = agui->get_window(omat_id);

	omat_mat_fname = agui->get_text(agui->add_text("STANDARD", font_size, "material filename:", 0xFFFFFF, e->get_gfx()->cord_to_pnt(9, 12), 701, 700));

	omat_open = agui->get_button(agui->add_button(e->get_gfx()->pnt_to_rect(12, 35, 368, 56), 0, 702, "open", 0, 700));

	omat_imat_fname = agui->get_input(agui->add_input_box(e->get_gfx()->pnt_to_rect(120, 9, 369, 29), 703, (char*)model->mat_fname.c_str(), 700));
}

void mgui::update_obj_list() {
	oobj_list->clear();
	unsigned int obj_count = model->ani ? model->amodel->get_object_count() : model->model->get_object_count();
	char* tmp = new char[16];
	for(unsigned int i = 0; i < obj_count; i++) {
		sprintf(tmp, "object %u", i);
		oobj_list->add_item(tmp, i);
	}
	delete [] tmp;
}

void mgui::update_mat(a2ematerial* mat, unsigned int obj, unsigned int tex) {
	cur_mat = mat;
	cur_obj = obj;
	cur_tex = tex;

	// set material type
	mcb_type->set_selected_id((mat->get_material_type(obj) & 0xFF));

	// set multitexturing stuff
	unsigned int argc_rgb = 0;
	unsigned int argc_alpha = 0;
	if(mat->get_material_type(obj) == a2ematerial::MULTITEXTURE) {
		mcb_rgb_combine->set_selected_id((mat->get_tex_elem(obj)->rgb_combine[tex] & 0xFF));
		mcb_alpha_combine->set_selected_id((mat->get_tex_elem(obj)->alpha_combine[tex] & 0xFF));

		switch(mat->get_tex_elem(obj)->rgb_combine[tex] & 0xFF) {
			case 0x00:
				argc_rgb = 1;
				break;
			case 0x01:
			case 0x02:
			case 0x03:
			case 0x04:
				argc_rgb = 2;
				break;
			case 0x05:
				argc_rgb = 3;
				break;
			case 0xFF:
				argc_rgb = 0;
				break;
		}

		switch(mat->get_tex_elem(obj)->alpha_combine[tex] & 0xFF) {
			case 0x00:
				argc_alpha = 1;
				break;
			case 0x01:
			case 0x02:
			case 0x03:
			case 0x04:
				argc_alpha = 2;
				break;
			case 0x05:
				argc_alpha = 3;
				break;
			case 0xFF:
				argc_alpha = 0;
				break;
		}

		agui->set_visibility(mcb_rgb_combine->get_id(), true);
		agui->set_visibility(mcb_alpha_combine->get_id(), true);
		agui->set_visibility(mrgb->get_id(), true);
		agui->set_visibility(malpha->get_id(), true);

		if(tex < mat->get_texture_count(obj)) mitex_large->set_texture(mat->get_texture(obj, tex));
		else mitex_large->set_texture(mnone_tex);

		if(argc_rgb != 0) {
			mcb_rgb_src->set_selected_id((mat->get_tex_elem(obj)->rgb_source[tex][cur_rgb_arg] & 0xFF));
			mcb_rgb_op->set_selected_id((mat->get_tex_elem(obj)->rgb_operand[tex][cur_rgb_arg] & 0xFF));
		}

		if(argc_alpha != 0) {
			mcb_alpha_src->set_selected_id((mat->get_tex_elem(obj)->alpha_source[tex][cur_alpha_arg] & 0xFF));
			mcb_alpha_op->set_selected_id((mat->get_tex_elem(obj)->alpha_operand[tex][cur_alpha_arg] & 0xFF));
		}
	}
	else {
		agui->set_visibility(mcb_rgb_combine->get_id(), false);
		agui->set_visibility(mcb_alpha_combine->get_id(), false);
		agui->set_visibility(mrgb->get_id(), false);
		agui->set_visibility(malpha->get_id(), false);

		mitex_large->set_texture(mat->get_texture(obj, tex));
	}

	// set the visibility of the multi-texturing stuff
	for(unsigned int i = 0; i < (3-argc_rgb); i++) {
		agui->set_visibility(mrgb_arg[2-i]->get_id(), false);
	}
	for(unsigned int i = 0; i < (3-argc_alpha); i++) {
		agui->set_visibility(malpha_arg[2-i]->get_id(), false);
	}
	for(unsigned int i = 0; i < argc_rgb; i++) {
		agui->set_visibility(mrgb_arg[i]->get_id(), true);
	}
	for(unsigned int i = 0; i < argc_alpha; i++) {
		agui->set_visibility(malpha_arg[i]->get_id(), true);
	}

	if(argc_rgb == 0) {
		agui->set_visibility(mcb_rgb_src->get_id(), false);
		agui->set_visibility(mcb_rgb_op->get_id(), false);
	}
	else {
		agui->set_visibility(mcb_rgb_src->get_id(), true);
		agui->set_visibility(mcb_rgb_op->get_id(), true);
	}

	if(argc_alpha == 0) {
		agui->set_visibility(mcb_alpha_src->get_id(), false);
		agui->set_visibility(mcb_alpha_op->get_id(), false);
	}
	else {
		agui->set_visibility(mcb_alpha_src->get_id(), true);
		agui->set_visibility(mcb_alpha_op->get_id(), true);
	}

	// set the texture images
	for(unsigned int i = 0; i < mat->get_texture_count(obj); i++) {
		mtex[i]->get_image()->set_texture(mat->get_texture(obj, i));
	}
	for(unsigned int i = mat->get_texture_count(obj); i < 8; i++) {
		mtex[i]->get_image()->set_texture(mnone_tex);
	}

	// set the visibility of the texture buttons
	switch(mat->get_material_type(obj)) {
		case a2ematerial::NONE:
			for(unsigned int i = 0; i < 8; i++) {
				agui->set_visibility(mtex[i]->get_id(), false);
			}
			break;
		case a2ematerial::DIFFUSE:
			agui->set_visibility(mtex[0]->get_id(), true);
			for(unsigned int i = 1; i < 8; i++) {
				agui->set_visibility(mtex[i]->get_id(), false);
			}
			break;
		case a2ematerial::BUMP:
			agui->set_visibility(mtex[0]->get_id(), true);
			agui->set_visibility(mtex[1]->get_id(), true);
			for(unsigned int i = 2; i < 8; i++) {
				agui->set_visibility(mtex[i]->get_id(), false);
			}
			break;
		case a2ematerial::PARALLAX:
			agui->set_visibility(mtex[0]->get_id(), true);
			agui->set_visibility(mtex[1]->get_id(), true);
			agui->set_visibility(mtex[2]->get_id(), true);
			for(unsigned int i = 3; i < 8; i++) {
				agui->set_visibility(mtex[i]->get_id(), false);
			}
			break;
		case a2ematerial::MULTITEXTURE:
			// all textures can be used ...
			for(unsigned int i = 0; i < 8; i++) {
				agui->set_visibility(mtex[i]->get_id(), true);
			}
			break;
		default:
			break;
	}

	char* caption = new char[255];
	sprintf(caption, "Material Properties - Object #%u - Texture #%u", obj, tex);
	mat_wnd->set_caption(caption);
	delete [] caption;
}

void mgui::new_mtl() {
	// create new (empty) material ...
	unsigned int oc = model->ani ? model->amodel->get_object_count() : model->model->get_object_count();

	file_io* f = e->get_file_io();
	f->open_file(e->data_path("temp.a2mtl"), file_io::OT_WRITE_BINARY);

	f->write_block("A2EMATERIAL", 11);
	f->put_uint(oc);
	for(unsigned int i = 0; i < oc; i++) {
		f->put_char(0x01);
		f->put_char(0x00);
		f->write_block("scale.png", 9);
		f->put_char((char)0xFF);
	}

	f->close_file();

	model->set_material("temp.a2mtl");

	update_mat(model->get_material(), 0, 0);
}

void mgui::open_mdl() {
	load_obj_wnd();
	load_mat_wnd();

	model->load_model(om_imdl_fname->get_text(), om_iani_fname->get_text());
	if(strcmp(om_imat_fname->get_text(), "") != 0) {
		model->set_material(om_imat_fname->get_text());
	}
	else {
		new_mtl();
	}

	update_obj_list();
	update_mat(model->get_material(), 0, 0);
}

void mgui::save_mtl() {
	model->mat_fname = sm_imat_fname->get_text();
	model->save_material(model->mat_fname.c_str());
}

void mgui::close_mtl() {
	cur_mat = NULL;
	oobj_list->clear();
	model->close_model();

	obj_wnd->set_deleted(true);
	mat_wnd->set_deleted(true);
}

void mgui::open_mtl() {
	model->set_material(omat_imat_fname->get_text());
	update_mat(model->get_material(), 0, 0);
}
