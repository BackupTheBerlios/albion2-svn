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

engine* mgui::e = NULL;
file_io* mgui::f = NULL;
const char* mgui::mdl_fname = NULL;
const char* mgui::mtl_fname = NULL;
const char* mgui::ani_fname = NULL;

mgui::mgui(engine* e, gui* agui, mdl* model, scene* sce) {
	mgui::e = e;
	mgui::agui = agui;
	mgui::gs = agui->get_gui_style();
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

	mnone_tex = e->get_texman()->add_texture(e->data_path("none.png"));

	wireframe = false;

	menu_id = 0xFFFFFFFF; // there is no gui object with the id 0xFFFFFFFF
	obj_id = 0xFFFFFFFF;
	mat_id = 0xFFFFFFFF;
	tex_id = 0xFFFFFFFF;
	om_id = 0xFFFFFFFF;
	sm_id = 0xFFFFFFFF;
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
							mdl_fname = model->mdl_fname.c_str();
							ofd_wnd = agui->get_object<gui_window>(agui->add_open_dialog(700, (char*)"choose a material", e->data_path(NULL), (char*)"a2mtl", 30, 30, &ffilter_mtl));
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
					case 106: {
						if(model->is_model()) {
							load_obj_wnd();
							update_obj_list();
							oobj_list->set_selected_id(cur_obj);
						}
					}
					break;
					case 107: {
						if(model->is_model()) {
							load_mat_wnd();
							update_mat(model->get_material(), cur_obj, 0);
						}
					}
					break;
					case 108: {
						if(model->is_model()) {
							wireframe = wireframe ^ true;
						}
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
						ofd_wnd = agui->get_object<gui_window>(agui->add_open_dialog(450, (char*)"choose a texture file", e->data_path(NULL), (char*)"png"));
					}
					break;
					// apply button of the open texture file dialog
					case 404: {
						if(!e->get_file_io()->is_file(e->data_path(titex_fname->get_text()->c_str()))) {
							m->print(msg::MERROR, "mgui.cpp", "cannot open file \"%s\"!", titex_fname->get_text());
							break;
						}
						cur_mat->get_tex_elem(cur_obj)->tex_names[cur_tex] = titex_fname->get_text()->c_str();
						cur_mat->get_tex_elem(cur_obj)->col_type[cur_tex] = (char)titex_type->get_selected_id();
						if(cur_tex > (cur_mat->get_tex_elem(cur_obj)->tex_count-1)) { cur_mat->get_tex_elem(cur_obj)->tex_count++; }
						cur_mat->load_textures();
						update_mat(cur_mat, cur_obj, cur_tex);
						tex_wnd->set_deleted(true);

						// check if color type was possible
						GLint format = e->get_texman()->get_texture(cur_mat->get_tex_elem(cur_obj)->textures[cur_tex])->format;
						if((titex_type->get_selected_id() == 0 && format != GL_RGB) ||
							(titex_type->get_selected_id() == 1 && format != GL_RGBA) ||
							(titex_type->get_selected_id() == 2 && format != GL_LUMINANCE)) {
							switch(format) {
								case GL_RGB:
									cur_mat->get_tex_elem(cur_obj)->col_type[cur_tex] = 0;
									break;
								case GL_RGBA:
									cur_mat->get_tex_elem(cur_obj)->col_type[cur_tex] = 1;
									break;
								case GL_LUMINANCE:
									cur_mat->get_tex_elem(cur_obj)->col_type[cur_tex] = 2;
									break;
								default:
									cur_mat->get_tex_elem(cur_obj)->col_type[cur_tex] = 0;
									break;
							}
						}
					}
					break;
					// set texture filename when the open button of the open texture file dialog is pressed
					case 452: {
						titex_fname->set_text((char*)agui->get_open_diaolg_list()->get_selected_item()->text.c_str());
						ofd_wnd->set_deleted(true);
					}
					break;
					// open model file dialog
					case 505: {
						mtl_fname = om_imat_fname->get_text()->c_str();
						ani_fname = om_iani_fname->get_text()->c_str();
						unsigned int wid = agui->add_open_dialog(520, (char*)"choose a model file", e->data_path(NULL), (char*)"a2m", 30, 30, &ffilter_mdl);
						if(wid != 0) ofd_wnd = agui->get_object<gui_window>(wid);
					}
					break;
					// open animation file dialog
					case 506: {
						mdl_fname = om_imdl_fname->get_text()->c_str();
						unsigned int wid = agui->add_open_dialog(530, (char*)"choose an animation file", e->data_path(NULL), (char*)"a2a", 30, 30, &ffilter_ani);
						if(wid != 0) ofd_wnd = agui->get_object<gui_window>(wid);
					}
					break;
					// open material file dialog
					case 507: {
						mdl_fname = om_imdl_fname->get_text()->c_str();
						unsigned int wid = agui->add_open_dialog(540, (char*)"choose a material file", e->data_path(NULL), (char*)"a2mtl", 30, 30, &ffilter_mtl);
						if(wid != 0) ofd_wnd = agui->get_object<gui_window>(wid);
					}
					break;
					// open model + ani + mat
					case 508: {
						// check if files exist
						if(!f->is_file(e->data_path(om_imdl_fname->get_text()->c_str()))) {
							agui->add_msgbox_ok((char*)"File does not exist!", (char*)"The model file does not exist!");
							m->print(msg::MERROR, "mgui.cpp", "The model file \"%s\" does not exist!", om_imdl_fname->get_text()->c_str());
							break;
						}
						if(e->get_core()->is_a2eanim(e->data_path(om_imdl_fname->get_text()->c_str())) && !f->is_file(e->data_path(om_iani_fname->get_text()->c_str()))) {
							agui->add_msgbox_ok((char*)"File does not exist!", (char*)"The animation file does not exist!");
							m->print(msg::MERROR, "mgui.cpp", "The animation file \"%s\" does not exist!", om_iani_fname->get_text()->c_str());
							break;
						}
						if(strcmp(om_imat_fname->get_text()->c_str(), "") != 0 && !f->is_file(e->data_path(om_imat_fname->get_text()->c_str()))) {
							agui->add_msgbox_ok((char*)"File does not exist!", (char*)"The material file does not exist!");
							m->print(msg::MERROR, "mgui.cpp", "The material file \"%s\" does not exist!", om_imat_fname->get_text()->c_str());
							break;
						}

						open_mdl();
						om_wnd->set_deleted(true);
					}
					break;
					// set model filename when the open button of the open model file dialog is pressed
					case 522: {
						om_imdl_fname->set_text((char*)agui->get_open_diaolg_list()->get_selected_item()->text.c_str());
						ofd_wnd->set_deleted(true);
					}
					break;
					// set animation filename when the open button of the open animation file dialog is pressed
					case 532: {
						om_iani_fname->set_text((char*)agui->get_open_diaolg_list()->get_selected_item()->text.c_str());
						ofd_wnd->set_deleted(true);
					}
					break;
					// set material filename when the open button of the open material file dialog is pressed
					case 542: {
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
						ofd_wnd->set_deleted(true);
						model->set_material((char*)agui->get_open_diaolg_list()->get_selected_item()->text.c_str());
						update_mat(model->get_material(), 0, 0);
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
						unsigned int tex_count = cur_mat->get_mat_type_tex_count((a2ematerial::MAT_TYPE)id);
						unsigned int otex_count = cur_mat->get_mat_type_tex_count((a2ematerial::MAT_TYPE)oid);
						if(id != oid) {
							te->tex_count = tex_count;
							for(unsigned int i = otex_count; i < tex_count; i++) {
								te->col_type[i] = 0;
								te->textures[i] = mnone_tex;
								te->tex_names[i] = "none.png";
							}
							if(id == a2ematerial::MULTITEXTURE) {
								for(unsigned int i = 0; i < 8; i++) {
									if(te->textures[7-i] != mnone_tex) {
										te->tex_count = 8 - i;
										i = 8;
									}
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
			case event::LISTBOX_ITEM_SELECTED:
				switch(e->get_event()->get_gui_event().id) {
					// change object
					case 201: {
						if(agui->exist(mat_id)) update_mat(cur_mat, oobj_list->get_selected_item()->id, 0);
						else cur_obj = oobj_list->get_selected_item()->id;
						model->update_selection(cur_obj);
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
}

void mgui::load_main_gui() {
	if(agui->exist(menu_id)) {
		if(!menu->get_deleted()) return;
	}

	unsigned int width = e->get_screen()->w;

	mnew_tex = e->get_texman()->add_texture(e->data_path("icons/new.png"), 4, GL_RGBA);
	mopen_tex = e->get_texman()->add_texture(e->data_path("icons/open.png"), 4, GL_RGBA);
	msave_tex = e->get_texman()->add_texture(e->data_path("icons/save.png"), 4, GL_RGBA);
	mclose_tex = e->get_texman()->add_texture(e->data_path("icons/close.png"), 4, GL_RGBA);
	mopen_model_tex = e->get_texman()->add_texture(e->data_path("icons/open_model.png"), 4, GL_RGBA);
	msub_obj_tex = e->get_texman()->add_texture(e->data_path("icons/subobj.png"), 4, GL_RGBA);
	mmat_prop_tex = e->get_texman()->add_texture(e->data_path("icons/mat_prop.png"), 4, GL_RGBA);
	mwireframe_tex = e->get_texman()->add_texture(e->data_path("icons/wireframe.png"), 4, GL_RGBA);

	menu_id = agui->add_window(e->get_gfx()->pnt_to_rect(0, 0, e->get_screen()->w, 24), 100, (char*)"main gui", false);
	menu = agui->get_object<gui_window>(menu_id);
	mnew_mtl = agui->get_object<gui_button>(agui->add_button(e->get_gfx()->pnt_to_rect(3, 3, 21, 21), 101, (char*)"", mnew_tex, 100));
	mopen_mtl = agui->get_object<gui_button>(agui->add_button(e->get_gfx()->pnt_to_rect(24, 3, 42, 21), 102, (char*)"", mopen_tex, 100));
	msave_mtl = agui->get_object<gui_button>(agui->add_button(e->get_gfx()->pnt_to_rect(45, 3, 63, 21), 103, (char*)"", msave_tex, 100));
	mclose_mtl = agui->get_object<gui_button>(agui->add_button(e->get_gfx()->pnt_to_rect(66, 3, 84, 21), 104, (char*)"", mclose_tex, 100));
	mopen_model = agui->get_object<gui_button>(agui->add_button(e->get_gfx()->pnt_to_rect(87, 3, 127, 21), 105, (char*)"", mopen_model_tex, 100));
	msub_obj = agui->get_object<gui_button>(agui->add_button(e->get_gfx()->pnt_to_rect(width - 42, 3, width - 24, 21), 106, (char*)"", msub_obj_tex, 100));
	mmat_prop = agui->get_object<gui_button>(agui->add_button(e->get_gfx()->pnt_to_rect(width - 21, 3, width - 3, 21), 107, (char*)"", mmat_prop_tex, 100));
	mwireframe = agui->get_object<gui_button>(agui->add_button(e->get_gfx()->pnt_to_rect(width - 63, 3, width - 45, 21), 108, (char*)"", mwireframe_tex, 100));

	mnew_mtl->set_image_scaling(false);
	mopen_mtl->set_image_scaling(false);
	msave_mtl->set_image_scaling(false);
	mclose_mtl->set_image_scaling(false);
	mopen_model->set_image_scaling(false);
	msub_obj->set_image_scaling(false);
	mmat_prop->set_image_scaling(false);
	mwireframe->set_image_scaling(false);
}

void mgui::load_obj_wnd() {
	if(agui->exist(obj_id)) {
		if(!obj_wnd->get_deleted()) return;
	}

	obj_id = agui->add_window(e->get_gfx()->pnt_to_rect(30, 30, 255, 375), 200, (char*)"Sub-Objects", true);
	obj_wnd = agui->get_object<gui_window>(obj_id);
	oobj_list = agui->get_object<gui_list>(agui->add_list_box(e->get_gfx()->pnt_to_rect(4, 4, 216, 320), 201, 200));
}

void mgui::load_mat_wnd() {
	if(agui->exist(mat_id)) {
		if(!mat_wnd->get_deleted()) return;
	}

	mat_id = agui->add_window(e->get_gfx()->pnt_to_rect(630, 30, 1000, 386), 300, (char*)"Material Properties", true);
	mat_wnd = agui->get_object<gui_window>(mat_id);

	mtype = agui->get_object<gui_text>(agui->add_text((char*)"STANDARD", font_size, (char*)"Type", gs->get_color("FONT"), e->get_gfx()->cord_to_pnt(15, 9), 301, 300));
	mrgb = agui->get_object<gui_text>(agui->add_text((char*)"STANDARD", font_size, (char*)"RGB Combine", gs->get_color("FONT"), e->get_gfx()->cord_to_pnt(151, 207), 302, 300));
	malpha = agui->get_object<gui_text>(agui->add_text((char*)"STANDARD", font_size, (char*)"Alpha Combine", gs->get_color("FONT"), e->get_gfx()->cord_to_pnt(252, 207), 303, 300));
	mitex = agui->get_object<gui_image>(agui->add_image(e->get_gfx()->pnt_to_rect(151, 28, 343, 220), 325, NULL, NULL, 300));
	mitex->set_image_texture(mnone_tex);
	mitex->get_image()->set_gui_img(true);

	mcb_type = agui->get_object<gui_combo>(agui->add_combo_box(e->get_gfx()->pnt_to_rect(15, 22, 131, 42), 304, 300));
	mcb_type->add_item((char*)"NONE", 0);
	mcb_type->add_item((char*)"DIFFUSE", 1);
	mcb_type->add_item((char*)"BUMP (no support)", 2);
	mcb_type->add_item((char*)"PARALLAX", 3);
	mcb_type->add_item((char*)"MULTI-TEXTURE", 4);

	mtex = new gui_button*[8];
	mtex[0] = agui->get_object<gui_button>(agui->add_button(e->get_gfx()->pnt_to_rect(15, 47, 70, 102), 305, (char*)"Tex #1", mnone_tex, 300));
	mtex[1] = agui->get_object<gui_button>(agui->add_button(e->get_gfx()->pnt_to_rect(76, 47, 131, 102), 306, (char*)"Tex #2", mnone_tex, 300));
	mtex[2] = agui->get_object<gui_button>(agui->add_button(e->get_gfx()->pnt_to_rect(15, 105, 70, 160), 307, (char*)"Tex #3", mnone_tex, 300));
	mtex[3] = agui->get_object<gui_button>(agui->add_button(e->get_gfx()->pnt_to_rect(76, 105, 131, 160), 308, (char*)"Tex #4", mnone_tex, 300));
	mtex[4] = agui->get_object<gui_button>(agui->add_button(e->get_gfx()->pnt_to_rect(15, 163, 70, 218), 309, (char*)"Tex #5", mnone_tex, 300));
	mtex[5] = agui->get_object<gui_button>(agui->add_button(e->get_gfx()->pnt_to_rect(76, 163, 131, 218), 310, (char*)"Tex #6", mnone_tex, 300));
	mtex[6] = agui->get_object<gui_button>(agui->add_button(e->get_gfx()->pnt_to_rect(15, 221, 70, 276), 311, (char*)"Tex #7", mnone_tex, 300));
	mtex[7] = agui->get_object<gui_button>(agui->add_button(e->get_gfx()->pnt_to_rect(76, 221, 131, 276), 312, (char*)"Tex #8", mnone_tex, 300));
	mchange_tex = agui->get_object<gui_button>(agui->add_button(e->get_gfx()->pnt_to_rect(15, 298, 131, 317), 326, (char*)"change texture", 0, 300));
	
	mrgb_arg = new gui_button*[3];
	malpha_arg = new gui_button*[3];
	mrgb_arg[0] = agui->get_object<gui_button>(agui->add_button(e->get_gfx()->pnt_to_rect(153, 251, 174, 271), 313, (char*)"1", 0, 300));
	mrgb_arg[1] = agui->get_object<gui_button>(agui->add_button(e->get_gfx()->pnt_to_rect(175, 251, 196, 271), 314, (char*)"2", 0, 300));
	mrgb_arg[2] = agui->get_object<gui_button>(agui->add_button(e->get_gfx()->pnt_to_rect(197, 251, 218, 271), 315, (char*)"3", 0, 300));
	malpha_arg[0] = agui->get_object<gui_button>(agui->add_button(e->get_gfx()->pnt_to_rect(252, 251, 273, 271), 316, (char*)"1", 0, 300));
	malpha_arg[1] = agui->get_object<gui_button>(agui->add_button(e->get_gfx()->pnt_to_rect(274, 251, 295, 271), 317, (char*)"2", 0, 300));
	malpha_arg[2] = agui->get_object<gui_button>(agui->add_button(e->get_gfx()->pnt_to_rect(296, 251, 317, 271), 318, (char*)"3", 0, 300));

	mcb_rgb_combine = agui->get_object<gui_combo>(agui->add_combo_box(e->get_gfx()->pnt_to_rect(151, 224, 242, 244), 319, 300));
	mcb_alpha_combine = agui->get_object<gui_combo>(agui->add_combo_box(e->get_gfx()->pnt_to_rect(251, 224, 342, 244), 320, 300));
	mcb_rgb_src = agui->get_object<gui_combo>(agui->add_combo_box(e->get_gfx()->pnt_to_rect(153, 274, 244, 294), 321, 300));
	mcb_rgb_op = agui->get_object<gui_combo>(agui->add_combo_box(e->get_gfx()->pnt_to_rect(153, 297, 244, 317), 322, 300));
	mcb_alpha_src = agui->get_object<gui_combo>(agui->add_combo_box(e->get_gfx()->pnt_to_rect(252, 274, 343, 294), 323, 300));
	mcb_alpha_op = agui->get_object<gui_combo>(agui->add_combo_box(e->get_gfx()->pnt_to_rect(252, 297, 343, 317), 324, 300));

	mcb_rgb_combine->add_item((char*)"REPLACE", 0);
	mcb_rgb_combine->add_item((char*)"MODULATE", 1);
	mcb_rgb_combine->add_item((char*)"ADD", 2);
	mcb_rgb_combine->add_item((char*)"ADD SIGNED", 3);
	mcb_rgb_combine->add_item((char*)"SUBSTRACT", 4);
	mcb_rgb_combine->add_item((char*)"INTERPOLATE", 5);
	mcb_rgb_combine->add_item((char*)"NONE", 255);
	mcb_rgb_combine->set_selected_id(255);

	mcb_alpha_combine->add_item((char*)"REPLACE", 0);
	mcb_alpha_combine->add_item((char*)"MODULATE", 1);
	mcb_alpha_combine->add_item((char*)"ADD", 2);
	mcb_alpha_combine->add_item((char*)"ADD SIGNED", 3);
	mcb_alpha_combine->add_item((char*)"SUBSTRACT", 4);
	mcb_alpha_combine->add_item((char*)"INTERPOLATE", 5);
	mcb_alpha_combine->add_item((char*)"NONE", 255);
	mcb_alpha_combine->set_selected_id(255);

	mcb_rgb_src->add_item((char*)"PRIM COLOR", 0);
	mcb_rgb_src->add_item((char*)"TEXTURE", 1);
	mcb_rgb_src->add_item((char*)"CONSTANT", 2);
	mcb_rgb_src->add_item((char*)"PREVIOUS", 3);

	mcb_rgb_op->add_item((char*)"SRC COLOR", 0);
	mcb_rgb_op->add_item((char*)"1 - SRC COLOR", 1);
	mcb_rgb_op->add_item((char*)"SRC_ALPHA", 2);
	mcb_rgb_op->add_item((char*)"1 - SRC ALPHA", 3);

	mcb_alpha_src->add_item((char*)"PRIM COLOR", 0);
	mcb_alpha_src->add_item((char*)"TEXTURE", 1);
	mcb_alpha_src->add_item((char*)"CONSTANT", 2);
	mcb_alpha_src->add_item((char*)"PREVIOUS", 3);

	mcb_alpha_op->add_item((char*)"SRC ALPHA", 0);
	mcb_alpha_op->add_item((char*)"1 - SRC ALPHA", 1);
}

void mgui::load_tex_wnd() {
	if(agui->exist(tex_id)) {
		if(!tex_wnd->get_deleted()) return;
	}

	tex_id = agui->add_window(e->get_gfx()->pnt_to_rect(270, 30, 656, 145), 400, (char*)"load texure", true);
	tex_wnd = agui->get_object<gui_window>(tex_id);

	ttex_fname = agui->get_object<gui_text>(agui->add_text((char*)"STANDARD", font_size, (char*)"texture filename:", gs->get_color("FONT"), e->get_gfx()->cord_to_pnt(12, 12), 401, 400));
	ttex_type = agui->get_object<gui_text>(agui->add_text((char*)"STANDARD", font_size, (char*)"texture type:", gs->get_color("FONT"), e->get_gfx()->cord_to_pnt(12, 35), 402, 400));

	tbrowse = agui->get_object<gui_button>(agui->add_button(e->get_gfx()->pnt_to_rect(339, 8, 368, 28), 403, (char*)"...", 0, 400));
	tapply = agui->get_object<gui_button>(agui->add_button(e->get_gfx()->pnt_to_rect(15, 62, 368, 83), 404, (char*)"apply", 0, 400));

	titex_fname = agui->get_object<gui_input>(agui->add_input_box(e->get_gfx()->pnt_to_rect(110, 8, 333, 28), 405, (char*)"", 400));

	titex_type = agui->get_object<gui_combo>(agui->add_combo_box(e->get_gfx()->pnt_to_rect(110, 35, 210, 55), 406, 400));
	titex_type->add_item((char*)"RGB", 0);
	titex_type->add_item((char*)"RGBA", 1);
	titex_type->add_item((char*)"LUMINANCE", 2);
	titex_type->set_selected_id(0);
}

void mgui::load_om_wnd() {
	if(agui->exist(om_id)) {
		if(!om_wnd->get_deleted()) return;
	}

	om_id = agui->add_window(e->get_gfx()->pnt_to_rect(270, 30, 652, 180), 500, (char*)"load model/animation/material", true);
	om_wnd = agui->get_object<gui_window>(om_id);

	om_mdl_fname = agui->get_object<gui_text>(agui->add_text((char*)"STANDARD", font_size, (char*)"model filename:", gs->get_color("FONT"), e->get_gfx()->cord_to_pnt(12, 9), 501, 500));
	om_ani_fname = agui->get_object<gui_text>(agui->add_text((char*)"STANDARD", font_size, (char*)"animation filename:", gs->get_color("FONT"), e->get_gfx()->cord_to_pnt(12, 32), 502, 500));
	om_mat_fname = agui->get_object<gui_text>(agui->add_text((char*)"STANDARD", font_size, (char*)"material filename:*", gs->get_color("FONT"), e->get_gfx()->cord_to_pnt(12, 55), 503, 500));
	om_mat_des = agui->get_object<gui_text>(agui->add_text((char*)"STANDARD", font_size, (char*)"*(if empty, a new one will be created)", gs->get_color("FONT"), e->get_gfx()->cord_to_pnt(160, 75), 504, 500));

	om_browse_mdl = agui->get_object<gui_button>(agui->add_button(e->get_gfx()->pnt_to_rect(339, 5, 368, 25), 505, (char*)"...", 0, 500));
	om_browse_ani = agui->get_object<gui_button>(agui->add_button(e->get_gfx()->pnt_to_rect(339, 29, 368, 49), 506, (char*)"...", 0, 500));
	om_browse_mat = agui->get_object<gui_button>(agui->add_button(e->get_gfx()->pnt_to_rect(339, 52, 368, 72), 507, (char*)"...", 0, 500));
	om_open = agui->get_object<gui_button>(agui->add_button(e->get_gfx()->pnt_to_rect(12, 97, 368, 118), 508, (char*)"open", 0, 500));

	om_imdl_fname = agui->get_object<gui_input>(agui->add_input_box(e->get_gfx()->pnt_to_rect(125, 5, 333, 25), 509, (char*)"", 500));
	om_iani_fname = agui->get_object<gui_input>(agui->add_input_box(e->get_gfx()->pnt_to_rect(125, 29, 333, 49), 510, (char*)"", 500));
	om_imat_fname = agui->get_object<gui_input>(agui->add_input_box(e->get_gfx()->pnt_to_rect(125, 52, 333, 72), 511, (char*)"", 500));
}

void mgui::load_sm_wnd() {
	if(agui->exist(sm_id)) {
		if(!sm_wnd->get_deleted()) return;
	}

	sm_id = agui->add_window(e->get_gfx()->pnt_to_rect(270, 30, 653, 119), 600, (char*)"save material", true);
	sm_wnd = agui->get_object<gui_window>(sm_id);

	sm_mat_fname = agui->get_object<gui_text>(agui->add_text((char*)"STANDARD", font_size, (char*)"material filename:", gs->get_color("FONT"), e->get_gfx()->cord_to_pnt(9, 12), 601, 600));

	sm_save = agui->get_object<gui_button>(agui->add_button(e->get_gfx()->pnt_to_rect(12, 35, 368, 56), 602, (char*)"save", 0, 600));

	sm_imat_fname = agui->get_object<gui_input>(agui->add_input_box(e->get_gfx()->pnt_to_rect(120, 9, 369, 29), 603, (char*)model->mat_fname.c_str(), 600));
}

void mgui::update_obj_list() {
	oobj_list->clear();
	unsigned int obj_count = model->ani ? model->amodel->get_object_count() : model->model->get_object_count();
	string* obj_names = model->ani ? model->amodel->get_object_names() : model->model->get_object_names();
	for(unsigned int i = 0; i < obj_count; i++) {
		oobj_list->add_item((char*)obj_names[i].c_str(), i);
	}
}

void mgui::update_mat(a2ematerial* mat, unsigned int obj, unsigned int tex) {
	cur_mat = mat;
	cur_obj = obj;
	cur_tex = tex;

	// set material type
	mcb_type->set_selected_id(mat->get_material_type(obj) & 0xFF);

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

		if(tex < mat->get_texture_count(obj)) mitex->set_image_texture(mat->get_texture(obj, tex));
		else mitex->set_image_texture(mnone_tex);

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

		mitex->set_image_texture(mat->get_texture(obj, tex));
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
		mtex[i]->set_image_texture(mat->get_texture(obj, i));
	}
	for(unsigned int i = mat->get_texture_count(obj); i < 8; i++) {
		mtex[i]->set_image_texture(mnone_tex);
	}

	// set the visibility of the texture buttons
	for(unsigned int i = 0; i < mat->get_mat_type_tex_count((a2ematerial::MAT_TYPE)mat->get_material_type(obj)); i++) {
		agui->set_visibility(mtex[i]->get_id(), true);
	}
	for(unsigned int i = mat->get_mat_type_tex_count((a2ematerial::MAT_TYPE)mat->get_material_type(obj)); i < 8; i++) {
		agui->set_visibility(mtex[i]->get_id(), false);
	}

	/*switch(mat->get_material_type(obj)) {
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
	}*/

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
		f->put_char(0x01);
		f->write_block("scale.png", 9);
		f->put_char((char)0xFF);
		f->write_block("white.png", 9);
		f->put_char((char)0xFF);
	}

	f->close_file();

	model->set_material((char*)"temp.a2mtl");

	update_mat(model->get_material(), 0, 0);
}

void mgui::open_mdl() {
	load_obj_wnd();
	load_mat_wnd();

	model->load_model((char*)om_imdl_fname->get_text()->c_str(), (char*)om_iani_fname->get_text()->c_str());
	if(strcmp(om_imat_fname->get_text()->c_str(), "") != 0) {
		model->set_material((char*)om_imat_fname->get_text()->c_str());
	}
	else {
		new_mtl();
	}

	update_obj_list();
	update_mat(model->get_material(), 0, 0);
	model->update_selection(cur_obj);
}

void mgui::save_mtl() {
	model->mat_fname = sm_imat_fname->get_text()->c_str();
	model->save_material(model->mat_fname.c_str());
}

void mgui::close_mtl() {
	cur_mat = NULL;
	oobj_list->clear();
	model->close_model();

	obj_wnd->set_deleted(true);
	mat_wnd->set_deleted(true);
}

bool mgui::is_wireframe() {
	return wireframe;
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
