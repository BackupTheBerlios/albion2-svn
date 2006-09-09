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

#include "gui_sample.h"
#include <iostream>
#include <cstring>

/*!
 * \mainpage
 *
 * \author flo
 *
 * \date July 2004 - April 2006
 *
 * Albion 2 Engine Sample - GUI Sample
 */

void open_info_wnd() {
	unsigned int width = e->get_screen()->w;
	unsigned int height = e->get_screen()->h;

	info_wnd_id = agui->add_window(agfx->pnt_to_rect(width/2 - 140, height/2 - 80, width/2 + 140, height/2 + 80), 200, "Info Window");
	info_wnd = agui->get_object<gui_window>(info_wnd_id);

	info_wnd_tab = agui->get_object<gui_tab>(agui->add_tab(agfx->pnt_to_rect(0, 0, 276, 137), 201, 200));
	info_wnd_tab->add_tab("Combo-Box");
	info_wnd_tab->add_tab("Cursor-Position");
	info_wnd_tab->add_tab("Time");

	cinfo_type = agui->get_object<gui_combo>(agui->add_combo_box(agfx->pnt_to_rect(12, 12, 256, 33), 202, 200, 201));
	tinfo1 = agui->get_object<gui_text>(agui->add_text("STANDARD", 12, "", gs->get_color("FONT"), agfx->cord_to_pnt(12, 47), 203, 200, 201));
	tinfo2 = agui->get_object<gui_text>(agui->add_text("STANDARD", 12, "", gs->get_color("FONT"), agfx->cord_to_pnt(12, 64), 204, 200, 201));
	tinfo3 = agui->get_object<gui_text>(agui->add_text("STANDARD", 12, "", gs->get_color("FONT"), agfx->cord_to_pnt(12, 81), 205, 200, 201));

	cinfo_type->set_tab(0);
	tinfo1->set_tab(1);
	tinfo2->set_tab(1);
	tinfo3->set_tab(1);

	cinfo_type->add_item("Cursor-Position", 0);
	cinfo_type->add_item("Time", 1);

	cur_info_id = 0;
}

int main(int argc, char *argv[])
{
	// initialize the engine
	e = new engine("../../data/");
	e->init();
	e->set_caption("A2E Sample - GUI Sample");

	// init class pointers
	m = e->get_msg();
	c = e->get_core();
	aevent = e->get_event();
	agfx = e->get_gfx();
	s = new shader(e);
	agui = new gui(e, s);
	gs = agui->get_gui_style();

	// initialize the a2e events
	aevent->init(sevent);

	// initialize the gui
	agui->init();
	unsigned int width = e->get_screen()->w;
	unsigned int height = e->get_screen()->h;

	img = new image(e);
	img->open_image(e->data_path("engine_logo.png"));
	img->set_position(width - img->get_width(), height - img->get_height());

	bedit = agui->get_object<gui_button>(agui->add_button(agfx->pnt_to_rect(12, 152, 109, 173), 100, "edit", 0, 0));
	badd = agui->get_object<gui_button>(agui->add_button(agfx->pnt_to_rect(121, 152, 218, 173), 101, "add", 0, 0));
	bset = agui->get_object<gui_button>(agui->add_button(agfx->pnt_to_rect(169, 179, 218, 199), 102, "set", 0, 0));
	bopen_msg = agui->get_object<gui_button>(agui->add_button(agfx->pnt_to_rect(width - 112, 12, width - 12, 35), 103, "open msg box", 0, 0));
	bopen_fd = agui->get_object<gui_button>(agui->add_button(agfx->pnt_to_rect(width - 112, 41, width - 12, 64), 104, "open file dialog", 0, 0));
	bopen_wnd = agui->get_object<gui_button>(agui->add_button(agfx->pnt_to_rect(width - 112, 70, width - 12, 93), 105, "open window", 0, 0));
	llist = agui->get_object<gui_list>(agui->add_list_box(agfx->pnt_to_rect(12, 12, 218, 146), 106, 0));
	ilitem = agui->get_object<gui_input>(agui->add_input_box(agfx->pnt_to_rect(12, 179, 163, 199), 107, "", 0));
	tstatus = agui->get_object<gui_text>(agui->add_text("STANDARD", 12, "...", gs->get_color("FONT"), agfx->cord_to_pnt(9, height - 20), 108, 0));
	clogo = agui->get_object<gui_check>(agui->add_check_box(agfx->pnt_to_rect(width - 112, height - img->get_height() - 22, width - 12, height - img->get_height() - 2), 109, "logo visible?", 0));
	clogo->set_checked(img_visible);

	// fill list box with data
	llist->add_item("test item", 0);
	llist->add_item("another item", 1);
	llist->add_item("one more ...", 2);
	llist->add_item("blah", 3);

	// needed for fps counting
	unsigned int fps = 0;
	unsigned int fps_time = 0;

	core::pnt* mpos = new core::pnt();
	gfx::rect* r1 = new gfx::rect();
	r1->x1 = 0;
	r1->y1 = 0;
	r1->x2 = e->get_screen()->w;
	r1->y2 = e->get_screen()->h;
	while(!done)
	{
		while(aevent->is_event())
		{
			aevent->handle_events(aevent->get_event().type);
			switch(aevent->get_event().type) {
				case SDL_QUIT:
					done = true;
					break;
				case SDL_KEYDOWN:
					switch(aevent->get_event().key.keysym.sym) {
						case SDLK_ESCAPE:
							done = true;
							break;
						default:
						break;
					}
					break;
				default:
				break;
			}
		}

		while(aevent->is_gui_event()) {
			switch(aevent->get_gui_event().type) {
				case event::BUTTON_PRESSED:
					tmp << "you pressed a button with the id #" << aevent->get_gui_event().id << "!";
					tstatus->set_text((char*)tmp.str().c_str());
					c->reset(&tmp);

					switch(aevent->get_gui_event().id) {
						case 52:
							file_dialog->set_deleted(true);
							break;
						case 100:
							cur_ed_id = llist->get_selected_id();
							ilitem->set_text(llist->get_item(cur_ed_id)->text.c_str());
							ilitem->set_text_position((unsigned int)strlen(ilitem->get_text()->c_str()));
							break;
						case 101: {
							unsigned int nid = llist->get_citems();
							llist->add_item("", nid);
							llist->set_selected_id(nid);
							llist->set_position(nid);
							cur_ed_id = nid;
							ilitem->set_text(llist->get_item(cur_ed_id)->text.c_str());
							ilitem->set_text_position((unsigned int)strlen(ilitem->get_text()->c_str()));
							}
							break;
						case 102:
							if(cur_ed_id == -1) break;
							llist->get_item(cur_ed_id)->text = ilitem->get_text()->c_str();
							llist->set_selected_id(cur_ed_id);
							break;
						case 103:
							agui->add_msgbox_ok("test message box", "test message box text ...");
							break;
						case 104:
							file_dialog = agui->get_object<gui_window>(agui->add_open_dialog(file_dialog_id, "open file dialog ... (png files)", (char*)e->get_data_path().c_str(), "png"));
							break;
						case 105:
							if(agui->exist(info_wnd_id)) break;
							open_info_wnd();
							break;
						default:
							break;
					}
					break;
				case event::COMBO_ITEM_SELECTED:
					tmp << "you selected a combo box (id #" << aevent->get_gui_event().id << ") item with the id #"
						<< cinfo_type->get_selected_id() << "!";
					tstatus->set_text((char*)tmp.str().c_str());
					c->reset(&tmp);

					switch(aevent->get_gui_event().id) {
						case 202:
							cur_info_id = cinfo_type->get_selected_id();
							tinfo1->set_text("");
							tinfo2->set_text("");
							tinfo3->set_text("");
							info_wnd_tab->set_active_tab(cur_info_id+1);
							break;
						default:
							break;
					}
					break;
				case event::CHECKBOX_CHECKED:
					tmp << "you set the check box with the id #" << aevent->get_gui_event().id << " to "
						<< (clogo->get_checked() ? "checked" : "unchecked") << "!";
					tstatus->set_text((char*)tmp.str().c_str());
					c->reset(&tmp);

					switch(aevent->get_gui_event().id) {
						case 109: {
							img_visible = clogo->get_checked();
						  }
						  break;
						default:
							break;
					}
					break;
				case event::TAB_SELECTED:
					switch(aevent->get_gui_event().id) {
						case 201:
							tinfo1->set_text("");
							tinfo2->set_text("");
							tinfo3->set_text("");
							switch(info_wnd_tab->get_active_tab()) {
								case 0:
									cinfo_type->set_tab(0);
									break;
								case 1:
									cur_info_id = 0;
									cinfo_type->set_tab(1);
									tinfo1->set_tab(1);
									tinfo2->set_tab(1);
									tinfo3->set_tab(1);
									break;
								case 2:
									cur_info_id = 1;
									cinfo_type->set_tab(2);
									tinfo1->set_tab(2);
									tinfo2->set_tab(2);
									tinfo3->set_tab(2);
									break;
								default:
									break;
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

		if(agui->exist(info_wnd_id)) {
			switch(cur_info_id) {
				case 0:
					aevent->get_mouse_pos(mpos);

					tmp << "X: " << mpos->x;
					tinfo1->set_text((char*)tmp.str().c_str());
					c->reset(&tmp);

					tmp << "Y: " << mpos->y;
					tinfo2->set_text((char*)tmp.str().c_str());

					c->reset(&tmp);
					break;
				case 1:
					time(&rawtime);
					tinfo = localtime(&rawtime);

					tmp << (1900+tinfo->tm_year) << "/" << (tinfo->tm_mon+1) << "/" << tinfo->tm_mday << ", ";
					if(tinfo->tm_hour < 10) tmp << "0";
					tmp << tinfo->tm_hour;
					tmp << ":";
					if(tinfo->tm_min < 10) tmp << "0";
					tmp << tinfo->tm_min;
					tmp << ":";
					if(tinfo->tm_sec < 10) tmp << "0";
					tmp << tinfo->tm_sec;
					tinfo1->set_text((char*)tmp.str().c_str());
					c->reset(&tmp);

					tmp << "time since program start: ";
					tinfo2->set_text((char*)tmp.str().c_str());
					c->reset(&tmp);

					tmp << SDL_GetTicks() << " ms, " << (SDL_GetTicks() / 1000) << " sec";
					tinfo3->set_text((char*)tmp.str().c_str());
					c->reset(&tmp);
					break;
				default:
					break;
			}
		}

		// print out the fps count
		fps++;
		if(SDL_GetTicks() - fps_time > 1000) {
			caption << "A2E Sample - GUI Sample | FPS: " << fps;
			e->set_caption((char*)caption.str().c_str());
			c->reset(&caption);

			fps = 0;
			fps_time = SDL_GetTicks();
		}

		e->start_draw();

		e->start_2d_draw();
		agfx->draw_filled_rectangle(r1, gs->get_color("WINDOW_BG1"));
		e->stop_2d_draw();
		if(img_visible) img->draw();
		agui->draw();

		e->stop_draw();
	}
	delete r1;

	delete mpos;

	delete img;
	delete agui;
	delete e;

	return 0;
}
