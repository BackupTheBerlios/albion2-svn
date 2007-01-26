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

#include "main.h"

/*!
 * \mainpage
 *
 * \author flo
 *
 * \date December 2005 - June 2006
 *
 * Albion 2 Engine - Mapeditor
 */

int main(int argc, char *argv[])
{
	// initialize the engine
	e = new engine("../../data/");
	e->init();

	// init class pointers
	c = e->get_core();
	m = e->get_msg();
	aevent = e->get_event();
	agfx = e->get_gfx();
	s = new shader(e);
	sce = new scene(e, s);
	cam = new camera(e);
	agui = new gui(e, s);
	x = new xml(m);
	l = e->get_lua();
	sf = e->get_screen();
	me = new mapeditor(e, sce, cam);
	megui = new mgui(e, agui, me, sce);

	// initialize the a2e events
	aevent->init(ievent);

	// initialize gui and chat sutff
	agui->init();
	megui->load_main_gui();

	// initialize the camera
	cam->set_position(0.0f, 50.0f, 0.0f);
	cam->set_mouse_input(false);
	cam->set_rotation_speed(100.0f);
	cam->set_cam_speed(5.0f);

	// set the light
	light* l1 = new light(e, 0.0f, 80.0f, 0.0f);
	float lamb[] = { 0.12f, 0.12f, 0.12f, 1.0f};
	float ldif[] = { 0.9f, 0.9f, 0.9f, 1.0f};
	float lspc[] = { 1.0f, 1.0f, 1.0f, 1.0f};
	//light* l1 = new light(e, 0.0f, 40.0f, 0.0f);
	//light* l1 = new light(e, 50.0f, 50.0f, 50.0f);
	/*float lamb[] = { 1.0f, 1.0f, 1.0f, 1.0f};
	float ldif[] = { 1.0f, 1.0f, 1.0f, 1.0f};
	float lspc[] = { 1.0f, 1.0f, 1.0f, 1.0f};*/
	//light* l1 = new light(e, 0.0f, 0.0f, 0.0f);
	//float lamb[] = { 1.0f, 1.0f, 1.0f, 1.0f};
	//float ldif[] = { 0.0f, 0.0f, 0.0f, 1.0f};
	//float lspc[] = { 0.0f, 0.0f, 0.0f, 1.0f};
	l1->set_lambient(lamb);
	l1->set_ldiffuse(ldif);
	l1->set_lspecular(lspc);
	sce->add_light(l1);
	//sce->set_light(false);

	// needed for fps counting
	unsigned int fps = 0;
	unsigned int fps_time = 0;
	char* tmp = new char[512];
	sprintf(tmp, "A2E Mapeditor - v0.1.1");

	//me->open_map(e->data_path("omp_test.a2map"));
	//me->open_map(e->data_path("hdrr.a2map"));
	//me->open_map(e->data_path("mdl_loader.a2map"));

	SDLKey key;
	// main loop
	while(!done)
	{
		if(cam->get_mouse_input()) megui->set_edit_state(mapeditor::CAM_INPUT);

		while(aevent->is_event())
		{
			aevent->handle_events(aevent->get_event().type);
			switch(aevent->get_event().type) {
				case SDL_QUIT:
					done = true;
					break;
				case SDL_KEYDOWN:
					key = aevent->get_event().key.keysym.sym;
					switch(key) {
						case SDLK_ESCAPE:
							done = true;
							break;
						case SDLK_m:
						case SDLK_r:
						case SDLK_s:
						case SDLK_n:
						case SDLK_a:
						case SDLK_p:
						case SDLK_DELETE:
							if(aevent->get_active() == event::CAMERA) {
								switch(key) {
									case SDLK_m:
										megui->set_edit_state(mapeditor::MOVE);
										break;
									case SDLK_r:
										megui->set_edit_state(mapeditor::ROTATE);
										break;
									case SDLK_s:
										megui->set_edit_state(mapeditor::SCALE);
										break;
									case SDLK_n:
										megui->set_edit_state(mapeditor::NONE);
										break;
									case SDLK_a:
										if(me->is_map_opened()) megui->add_obj_dialog();
										break;
									case SDLK_p:
										megui->open_property_wnd();
										break;
									case SDLK_DELETE:
										if(me->is_map_opened()) {
											if(me->get_sel_object() == megui->get_phys_obj()) megui->set_phys_obj(false);
											me->delete_obj();
										}
										break;
									default:
									break;
								}
							}
							break;
						default:
						break;
					}
					break;
				case SDL_MOUSEMOTION: {
					unsigned int edit_state = megui->get_edit_state();
					if(button_left_pressed && (edit_state == mapeditor::MOVE || edit_state == mapeditor::ROTATE || edit_state == mapeditor::SCALE || edit_state == mapeditor::PHYS_SCALE)) {
						float scale = (SDL_GetModState() & KMOD_SHIFT) ? 0.1f : 2.0f;
						me->edit_object(aevent->get_event().motion.xrel, aevent->get_event().motion.yrel, scale);
					}
				}
				break;
				case SDL_MOUSEBUTTONUP:
					switch(aevent->get_event().button.button) {
						case SDL_BUTTON_LEFT: {
							button_left_pressed = false;
						}
						break;
						default:
							break;
					}
					break;
				case SDL_MOUSEBUTTONDOWN:
					switch(aevent->get_event().button.button) {
						case SDL_BUTTON_LEFT: {
							if(!cam->get_mouse_input()) button_left_pressed = true;
							unsigned int edit_state = megui->get_edit_state();
							if(edit_state == mapeditor::MOVE || edit_state == mapeditor::ROTATE || edit_state == mapeditor::SCALE || edit_state == mapeditor::PHYS_SCALE) {
								core::pnt* mp = new core::pnt();
								vertex3* proj_pos = new vertex3();
								aevent->get_mouse_pos(mp);
								c->get_3d_from_2d(mp, proj_pos);
								me->select(proj_pos);
								delete mp;
								delete proj_pos;
							}
						}
						break;
						case SDL_BUTTON_MIDDLE: {
							core::pnt* mp = new core::pnt();
							vertex3* proj_pos = new vertex3();
							aevent->get_mouse_pos(mp);
							c->get_3d_from_2d(mp, proj_pos);
							me->select_object(proj_pos);
							delete mp;
							delete proj_pos;
						}
						break;
						case SDL_BUTTON_RIGHT:
							cam->set_mouse_input(cam->get_mouse_input() ^ true);
							break;
						default:
						break;
					}
					break;
				default:
					break;
			}
		}

		// print out the fps count
		fps++;
		if(SDL_GetTicks() - fps_time > 1000) {
			sprintf(tmp, "A2E Mapeditor - v0.1.1 | FPS: %u | Pos: %f %f %f", fps,
				-cam->get_position()->x, -cam->get_position()->y, -cam->get_position()->z);
			fps = 0;
			fps_time = SDL_GetTicks();
		}
		e->set_caption(tmp);


		// start drawing the scene
		e->start_draw();

		cam->run();
		sce->start_draw();
		if(megui->is_phys_obj_selected()) { if(!megui->get_phys_obj()->type) megui->get_phys_obj()->model->draw_phys_obj(); }
		me->run(megui->get_edit_state());
		sce->stop_draw();
		megui->run();
		agui->draw();

		e->stop_draw();
	}

	delete [] tmp;

	delete me;
	delete megui;

	delete sce;
	delete cam;
	delete s;
	delete agui;
	delete e;

	//delete testmat;
	//delete testm;

	return 0;
}
