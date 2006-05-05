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

#include "model_loader.h"

/*!
 * \mainpage
 *
 * \author flo
 *
 * \date August 2004 - April 2006
 *
 * Albion 2 Engine Sample - Model Loader Sample
 */

int main(int argc, char *argv[])
{
	// initialize the engine
	e = new engine("../../data/");
	e->init();
	e->set_caption("A2E Sample - Model Loader");

	// init class pointers
	c = e->get_core();
	m = e->get_msg();
	aevent = e->get_event();
	agfx = e->get_gfx();
	s = new shader(e);
	sce = new scene(e, s);
	cam = new camera(e);
	o = new ode(e);

	// initialize the a2e events
	aevent->init(sevent);
	aevent->load_keyset("DE");

	// initialize the camera
	cam->set_position(-5.0f, -30.0f, -55.0f);
	cam->set_rotation_speed(100.0f);

	// initialize ode
	o->init();

	if(!use_a2map) {
		// load materials
		ground_mat = new a2ematerial(e);
		ground_mat->load_material(e->data_path("ground.a2mtl"));

		cc_mat = new a2ematerial(e);
		cc_mat->load_material(e->data_path("celtic_house.a2mtl"));

		scale_mat = new a2ematerial(e);
		scale_mat->load_material(e->data_path("scale.a2mtl"));

		// load the model and set a new position
		plane = sce->create_a2emodel();
		plane->load_model(e->data_path("ground.a2m"), true);
		plane->set_position(0.0f, -13.0f, 0.0f);
		plane->set_draw_wireframe(wireframe);
		plane->set_material(ground_mat);

		cottage = sce->create_a2emodel();
		cottage->load_model(e->data_path("celtic_house.a2m"), true);
		cottage->set_position(0.0f, 0.0f, 0.0f);
		cottage->set_draw_wireframe(wireframe);
		cottage->set_material(cc_mat);

		sphere = sce->create_a2emodel();
		sphere->load_model(e->data_path("player_sphere.a2m"), true);
		sphere->set_position(45.0f, 20.0f, 20.0f);
		sphere->set_draw_wireframe(wireframe);
		sphere->set_radius(1.0f);
		sphere->set_material(scale_mat);

		sce->add_model(plane);
		sce->add_model(cottage);
		sce->add_model(sphere);

		// pass the models to ode
		o->add_object(plane, true, ode_object::TRIMESH);
		o->add_object(cottage, true, ode_object::TRIMESH);
		o->add_object(sphere, false, ode_object::SPHERE);

		osphere = o->get_ode_object(2);
	}
	else {
		mdl_map = new a2emap(e, sce, o);
		mdl_map->load_map(e->data_path("mdl_loader.a2map"), true);

		plane = mdl_map->get_object("ground")->model;
		cottage = mdl_map->get_object("house")->model;
		sphere = mdl_map->get_object("sphere")->model;
		osphere = mdl_map->get_object("sphere")->ode_obj;
	}

	// lights
	light* l1 = new light(e, 0.0f, 200.0f, 0.0f);
	light* l2 = new light(e, 0.0f, 15.0f, 0.0f);

	float lamb1[] = { 0.22f, 0.22f, 0.22f, 1.0f};
	float ldif1[] = { 0.9f, 0.9f, 0.9f, 1.0f};
	float lspc1[] = { 1.0f, 1.0f, 1.0f, 1.0f};
	float lamb2[] = { 0.0f, 0.1f, 0.3f, 0.5f};
	float ldif2[] = { 0.1f, 0.3f, 0.5f, 0.5f};
	float lspc2[] = { 0.0f, 0.2f, 0.6f, 0.5f};

	l1->set_lambient(lamb1);
	l1->set_ldiffuse(ldif1);
	l1->set_lspecular(lspc1);
	l2->set_lambient(lamb2);
	l2->set_ldiffuse(ldif2);
	l2->set_lspecular(lspc2);
	sce->add_light(l1);
	sce->add_light(l2);

	// needed for fps counting
	unsigned int fps = 0;
	unsigned int fps_time = 0;

	refresh_time = SDL_GetTicks();
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
						case SDLK_RETURN:
							break;
						case SDLK_r: {
							osphere->reset();
							dGeomSetPosition(osphere->get_geom(), 45.0f, 20.0f, 20.0f);
						}
						break;
						case SDLK_e: {
							wireframe = wireframe ^ true;
							plane->set_draw_wireframe(wireframe);
							cottage->set_draw_wireframe(wireframe);
							sphere->set_draw_wireframe(wireframe);
						}
						break;
						case SDLK_w: {
							dBodyAddForce(osphere->get_body(), 5.0f, 1.0f, 0.0f);
						}
						break;
						case SDLK_s: {
							dBodyAddForce(osphere->get_body(), -5.0f, 1.0f, 0.0f);
						}
						break;
						case SDLK_a: {
							dBodyAddForce(osphere->get_body(), 0.0f, 1.0f, 5.0f);
						}
						break;
						case SDLK_d: {
							dBodyAddForce(osphere->get_body(), 0.0f, 1.0f, -5.0f);
						}
						break;
						default:
						break;
					}
					break;
			}
		}

		// print out the fps count
		fps++;
		if(SDL_GetTicks() - fps_time > 1000) {
			caption << "A2E Sample - Model Loader | FPS: " << fps << " | Pos: " <<
				cam->get_position()->x << " " << cam->get_position()->y << " " << cam->get_position()->z;
			e->set_caption((char*)caption.str().c_str());
			c->reset(&caption);

			fps = 0;
			fps_time = SDL_GetTicks();
		}

		e->start_draw();

		cam->run();
		sce->draw();
		if(SDL_GetTicks() - refresh_time != 0) {
			o->run(SDL_GetTicks() - refresh_time);
			refresh_time = SDL_GetTicks();
		}

		e->stop_draw();
	}

	o->close();

	// delete everything
	delete o;
	delete sce;
	if(!use_a2map) {
		delete scale_mat;
		delete ground_mat;
		delete cc_mat;
	}
	else {
		mdl_map->close_map();
		delete mdl_map;
	}
	delete l1;
	delete l2;
	delete s;
	delete cam;
	delete e;

	return 0;
}
