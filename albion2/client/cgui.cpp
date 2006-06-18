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

#include "cgui.h"

cgui::cgui(engine* e, gui* agui, csystem* cs, cnet* cn) {
	cgui::e = e;
	cgui::agui = agui;
	cgui::m = e->get_msg();
	cgui::c = e->get_core();
	cgui::f = e->get_file_io();
	cgui::cs = cs;
	cgui::cn = cn;

	txt = agui->get_text(agui->add_text("STANDARD", 12, "", 0x000000, e->get_gfx()->cord_to_pnt(0, 0), 40, 0));

	mmenu = NULL;

	gui_state = cgui::GS_MAIN;

	font_size = 11;

	bg_img = new image(e);
	bg_img->open_image(e->data_path("menu_bg.png"), 3, GL_RGB);

	buffer = new stringstream(stringstream::in | stringstream::out);

	mmenu_id = 0xFFFFFFFF; // there is no gui object with the id 0xFFFFFFFF
	mlogin_id = 0xFFFFFFFF;
	moptions_id = 0xFFFFFFFF;
	mcredits_id = 0xFFFFFFFF;
	gchat_id = 0xFFFFFFFF;
}

cgui::~cgui() {
	delete bg_img;

	delete buffer;
}

void cgui::init() {
	load_main_gui();
	load_login_wnd();
	load_options_wnd();
	load_credits_wnd();
	agui->set_visibility(200, false);
	agui->set_visibility(300, false);
	agui->set_visibility(400, false);
}

void cgui::run() {
	while(e->get_event()->is_gui_event()) {
		switch(e->get_event()->get_gui_event().type) {
			case event::BUTTON_PRESSED:
				switch(e->get_event()->get_gui_event().id) {
					/////////// menu handling ///////////
					case 101: {
						agui->set_visibility(100, false);
						agui->set_visibility(200, true);
					}
					break;
					case 102: {
						agui->set_visibility(100, false);
						agui->set_visibility(300, true);
					}
					break;
					case 103: {
						agui->set_visibility(100, false);
						agui->set_visibility(400, true);
					}
					break;
					case 201: {
						agui->set_visibility(100, true);
						agui->set_visibility(200, false);
					}
					break;
					case 301: {
						agui->set_visibility(100, true);
						agui->set_visibility(300, false);
					}
					break;
					case 401: {
						agui->set_visibility(100, true);
						agui->set_visibility(400, false);
					}
					break;
					/////////////////////////////////////
					case 104: {
						cs->done = true;
					}
					break;
					case 202: {
						if(!cs->connect_client(ml_iname->get_text()->c_str(), ml_ipw->get_text()->c_str())) {
							agui->add_msgbox_ok("client login failed", "login failed - can't connect to server!");
							break;
						}
					}
					break;
					///////////// game stuff ////////////
					case 1101: {
						const char* chat_msg = gc_imsg->get_text()->c_str();
						unsigned int msg_len = (unsigned int)strlen(chat_msg);

						if(msg_len == 0) break;
						if(strcmp(chat_msg, "/who") == 0) {
							string who = "";
							for(vector<cnet::client>::iterator cl_iter = cn->clients.begin(); cl_iter != cn->clients.end(); cl_iter++) {
								who += cl_iter->name;
								if(cl_iter != cn->clients.end()-1) {
									who += ", ";
								}
							}
							if(who != "") cgui::add_chat_msg(cnet::CT_WORLD, "System", who.c_str());
							gc_imsg->set_text("");
							break;
						}

						// put chat stuff into data buffer and send packet
						cn->clear_data();
						c->put_uint(cn->get_data(), cnet::CT_WORLD);
						c->put_uint(cn->get_data(), msg_len);
						c->put_block(cn->get_data(), chat_msg, msg_len);
						cn->send_packet(cnet::PT_CHAT_MSG);

						// add msg to "local" client msg box
						cgui::add_chat_msg(cnet::CT_WORLD, cs->client_name.c_str(), chat_msg);

						// reset input box
						gc_imsg->set_text("");
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

	if(cs->disconnected) {
		//agui->add_msgbox_ok("disconnection", "lost connection to the server (server down or network disconnection)!");
		agui->add_msgbox_ok("disconnection", "you have been disconnected from the server!");

		gui_state = cgui::GS_MAIN;
		load_main_gui();

		cs->disconnected = false;
	}

	if(gui_state == cgui::GS_MAIN) {
		// draw background image
		bg_img->draw(cs->sf->w, cs->sf->h);
	}
	else {
		// check if client received a new msg
		if(cs->chat_msgs.size() != 0) {
			// if so, add msg to the chat msg box
			for(vector<csystem::chat_msg>::iterator cm_iter = cs->chat_msgs.begin(); cm_iter != cs->chat_msgs.end(); cm_iter++) {
				add_chat_msg((cnet::CHAT_TYPE)cm_iter->type, cm_iter->name.c_str(), cm_iter->msg.c_str());
			}
			// all messages added, clear msg buffer
			cs->chat_msgs.clear();
		}
	}

	// check if we received any new flags
	if(cs->flags.size() != 0) {
		unsigned int flag = cs->flags.front();

		switch(flag) {
			case cnet::F_SUCCESS_LOGIN:
				if(cgui::gui_state != cgui::GS_GAME) {
					gui_state = cgui::GS_GAME;
					load_game_gui();
				}
				break;
			case cnet::F_WRONG_UNAME:
				agui->add_msgbox_ok("system message", "you entered a user name that doesn't exist!");
				break;
			case cnet::F_WRONG_PW:
				agui->add_msgbox_ok("system message", "you entered a wrong password!");
				break;
			default:
				//m->print(msg::MERROR, "cgui.cpp", "run(): unknown flag type %u!", flag);
				break;
		}

		cs->flags.erase(cs->flags.begin());
	}
}

void cgui::load_main_gui() {
	if(agui->exist(mmenu_id)) agui->set_visibility(100, true);
	if(agui->exist(mlogin_id)) agui->set_visibility(200, false);
	if(agui->exist(moptions_id)) agui->set_visibility(300, false);
	if(agui->exist(mcredits_id)) agui->set_visibility(400, false);
	if(agui->exist(gchat_id)) agui->set_visibility(1100, false);

	if(agui->exist(mmenu_id)) {
		if(!mmenu->get_deleted()) return;
	}

	mmenu_id = agui->add_window(e->get_gfx()->pnt_to_rect(0, 0, e->get_screen()->w, e->get_screen()->h), 100, "main gui", false, false);
	mmenu = agui->get_window(mmenu_id);
	mm_login = agui->get_button(agui->add_button(e->get_gfx()->pnt_to_rect(190, 112, 372, 144), 101, "login", 0, 100));
	mm_options = agui->get_button(agui->add_button(e->get_gfx()->pnt_to_rect(70, 236, 252, 268), 102, "options", 0, 100));
	mm_credits = agui->get_button(agui->add_button(e->get_gfx()->pnt_to_rect(50, 380, 232, 412), 103, "credits", 0, 100));
	mm_exit = agui->get_button(agui->add_button(e->get_gfx()->pnt_to_rect(124, 522, 306, 554), 104, "exit", 0, 100));
}

void cgui::load_login_wnd() {
	if(agui->exist(mlogin_id)) {
		if(!mlogin->get_deleted()) return;
	}

	mlogin_id = agui->add_window(e->get_gfx()->pnt_to_rect(0, 0, e->get_screen()->w, e->get_screen()->h), 200, "login", false, false);
	mlogin = agui->get_window(mlogin_id);
	ml_back = agui->get_button(agui->add_button(e->get_gfx()->pnt_to_rect(190, 112, 372, 144), 201, "back", 0, 200));
	ml_login = agui->get_button(agui->add_button(e->get_gfx()->pnt_to_rect(780, 165, 962, 187), 202, "login", 0, 200));
	ml_iname = agui->get_input(agui->add_input_box(e->get_gfx()->pnt_to_rect(780, 112, 962, 132), 203, (char*)cs->client_name.c_str(), 200));
	ml_ipw = agui->get_input(agui->add_input_box(e->get_gfx()->pnt_to_rect(780, 139, 962, 159), 204, "", 200));
	ml_name = agui->get_text(agui->add_text("STANDARD", font_size, "username:", 0xFFFFFF, e->get_gfx()->cord_to_pnt(680, 115), 205, 200));
	ml_pw = agui->get_text(agui->add_text("STANDARD", font_size, "password:", 0xFFFFFF, e->get_gfx()->cord_to_pnt(680, 142), 206, 200));
}

void cgui::load_options_wnd() {
	if(agui->exist(moptions_id)) {
		if(!moptions->get_deleted()) return;
	}

	moptions_id = agui->add_window(e->get_gfx()->pnt_to_rect(0, 0, e->get_screen()->w, e->get_screen()->h), 300, "options", false, false);
	moptions = agui->get_window(moptions_id);
	mo_back = agui->get_button(agui->add_button(e->get_gfx()->pnt_to_rect(190, 112, 372, 144), 301, "back", 0, 300));
	mo_save = agui->get_button(agui->add_button(e->get_gfx()->pnt_to_rect(780, 330, 962, 362), 302, "save", 0, 300));
}

void cgui::load_credits_wnd() {
	if(agui->exist(mcredits_id)) {
		if(!mcredits->get_deleted()) return;
	}

	mcredits_id = agui->add_window(e->get_gfx()->pnt_to_rect(0, 0, e->get_screen()->w, e->get_screen()->h), 400, "credits", false, false);
	mcredits = agui->get_window(moptions_id);
	mc_back = agui->get_button(agui->add_button(e->get_gfx()->pnt_to_rect(190, 112, 372, 144), 401, "back", 0, 400));
}

////////////////// game gui //////////////////
void cgui::load_game_gui() {
	if(agui->exist(mmenu_id)) agui->set_visibility(100, false);
	if(agui->exist(mlogin_id)) agui->set_visibility(200, false);
	if(agui->exist(moptions_id)) agui->set_visibility(300, false);
	if(agui->exist(mcredits_id)) agui->set_visibility(400, false);
	if(agui->exist(gchat_id)) agui->set_visibility(1100, true);

	load_chat_wnd();
}

void cgui::load_chat_wnd() {
	if(agui->exist(gchat_id)) {
		if(!gchat->get_deleted()) return;
	}

	gchat_id = agui->add_window(e->get_gfx()->pnt_to_rect(0, 0, 384, 340), 1100, "chat", true);
	gchat = agui->get_window(gchat_id);
	gc_send = agui->get_button(agui->add_button(e->get_gfx()->pnt_to_rect(289, 282, 364, 302), 1101, "send", 0, 1100));
	gc_all = agui->get_button(agui->add_button(e->get_gfx()->pnt_to_rect(12, 12, 71, 32), 1102, "all", 0, 1100));
	gc_world = agui->get_button(agui->add_button(e->get_gfx()->pnt_to_rect(77, 12, 136, 32), 1103, "world", 0, 1100));
	gc_region = agui->get_button(agui->add_button(e->get_gfx()->pnt_to_rect(142, 12, 201, 32), 1104, "region", 0, 1100));
	gc_party = agui->get_button(agui->add_button(e->get_gfx()->pnt_to_rect(207, 12, 266, 32), 1105, "party", 0, 1100));
	gc_imsg = agui->get_input(agui->add_input_box(e->get_gfx()->pnt_to_rect(12, 282, 283, 302), 1106, "", 1100));
	gc_msg_box = agui->get_list(agui->add_list_box(e->get_gfx()->pnt_to_rect(12, 38, 364, 276), 1107, 1100));
}

void cgui::add_chat_msg(cnet::CHAT_TYPE type, const char* name, const char* msg) {
	buffer->clear();
	buffer->str("");
	*buffer << name << ": " << msg;
	string tmp = "";
	string last_text = "";
	unsigned int lb_width = gc_msg_box->get_rectangle()->x2 - gc_msg_box->get_rectangle()->x1 - 19;
	unsigned int buffer_len = (unsigned int)buffer->str().length();
	txt->set_text((char*)buffer->str().c_str());

	// word wrapper ... pretty tricky ;)
	if(txt->get_text_width() <= lb_width) {
		gc_msg_box->add_item((char*)buffer->str().c_str(), gc_msg_box->get_citems());
	}
	else {
		while((unsigned int)buffer->tellg() < buffer_len) {
			txt->set_text("");
			buffer->seekg((unsigned int)((unsigned int)buffer->tellg() - tmp.length()), ios::beg);
			while(txt->get_text_width() <= lb_width && (unsigned int)buffer->tellg() < buffer_len) {
				last_text = txt->get_text();
				*buffer >> tmp;
				txt->set_text((char*)string(last_text + tmp + string(" ")).c_str());
			}

			txt->set_text((char*)tmp.c_str());
			// if a single word is larger than the allowed width, split the word
			if(txt->get_text_width() > lb_width) {
				string tmp2 = "";
				for(unsigned int i = 0; i < tmp.length(); i++) {
					tmp2 += tmp[i];
					txt->set_text((char*)string(last_text + tmp2).c_str());
					if(txt->get_text_width() > lb_width) {
						tmp2.erase(tmp2.length()-1, 1);
						gc_msg_box->add_item((char*)string(last_text + tmp2).c_str(), gc_msg_box->get_citems());

						last_text = tmp[i];
						tmp2 = "";
					}
				}
				tmp = last_text + tmp2;
				if((unsigned int)buffer->tellg() == buffer_len) gc_msg_box->add_item((char*)txt->get_text(), gc_msg_box->get_citems());
			}
			else {
				if(last_text != "" && (unsigned int)buffer->tellg() != buffer_len) {
					gc_msg_box->add_item((char*)last_text.c_str(), gc_msg_box->get_citems());
				}
				else if(last_text != "" && (unsigned int)buffer->tellg() == buffer_len) {
					gc_msg_box->add_item((char*)string(last_text + tmp).c_str(), gc_msg_box->get_citems());
				}
				else if(last_text == "" && (unsigned int)buffer->tellg() == buffer_len) {
					gc_msg_box->add_item((char*)txt->get_text(), gc_msg_box->get_citems());
				}
			}
		}
	}

	gc_msg_box->set_selected_id(gc_msg_box->get_citems()-1);
	gc_msg_box->set_position(gc_msg_box->get_citems()-1);
	txt->set_text("");
}
