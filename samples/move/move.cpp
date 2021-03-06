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

#include "move.h"

/*!
 * \mainpage
 *
 * \author flo
 *
 * \date April 2005 - August 2005
 *
 * Albion 2 Engine Sample - Move Sample
 */

void update_cam(cam_type ctype) {
	float xpos = sphere_obj->get_model()->get_position()->x;
	float ypos = sphere_obj->get_model()->get_position()->y;
	float zpos = sphere_obj->get_model()->get_position()->z;

	float xrot = cam->get_rotation()->y * piover180;
	float zrot = cam->get_rotation()->y * piover180;

	cam->set_position(-xpos - sinf(xrot) * 15.0f, ypos + 10.0f, -zpos - cosf(zrot) * 15.0f);
	player->set_position(xpos, ypos - 2.0f, zpos);
	player->set_rotation(0.0f, 360.0f - cam->get_rotation()->y, 0.0f);

	if(SDL_GetTicks() - walk_time >= min_walk_time) {
		const dReal* clvel = dBodyGetLinearVel(sphere_obj->get_body());
		//cout << clvel[0] << " | " << clvel[1] << " | " << clvel[2] << endl;

		const dReal* cforce = dBodyGetForce(sphere_obj->get_body());
		//cout << cforce[0] << " | " << cforce[1] << " | " << cforce[2] << endl;

		switch(ctype) {
			case UP: {
				if(moved) {
                    //dBodySetLinearVel(sphere_obj->get_body(), clvel[0] - lastxforce, 0.0f, clvel[2] - lastzforce);
				}

				xrot = sinf(xrot) * -max_force;
				zrot = cosf(zrot) * -max_force;

				lastxforce = xrot;
				lastzforce = zrot;

				dBodySetLinearVel(sphere_obj->get_body(), xrot, clvel[1], zrot);

				walk_time = SDL_GetTicks();
				moved = true;
				break;
			}
			case DOWN: {
				if(moved) {
                    //dBodySetLinearVel(sphere_obj->get_body(), clvel[0] - lastxforce, 0.0f, clvel[2] - lastzforce);
				}

				xrot = sinf(xrot) * max_force;
				zrot = cosf(zrot) * max_force;

				lastxforce = xrot;
				lastzforce = zrot;

				dBodySetLinearVel(sphere_obj->get_body(), xrot, clvel[1], zrot);

				walk_time = SDL_GetTicks();
				moved = true;
				break;
			}
			default:
				dBodySetLinearVel(sphere_obj->get_body(), 0.0f, clvel[1], 0.0f);
				break;
		}
	}
}

int main(int argc, char *argv[])
{
	// initialize the engine
	e = new engine("../../data/");
	e->init();
	e->set_caption("A2E Sample - Move Sample");
	e->set_cursor_visible(false);

	// init class pointers
	c = e->get_core();
	m = e->get_msg();
	aevent = e->get_event();
	s = new shader(e);
	sce = new scene(e, s);
	cam = new camera(e);
	o = new ode(e);

	// initialize the a2e events
	aevent->init(ievent);

	// initialize the camera
	cam->set_position(-5.0f, 30.0f, -55.0f);
	cam->set_cam_input(false);
	cam->set_rotation_speed(50.0f);

	// load materials
	a2ematerial* level_mat = new a2ematerial(e);
	level_mat->load_material(e->data_path("level.a2mtl"));

	a2ematerial* logo_mat = new a2ematerial(e);
	logo_mat->load_material(e->data_path("logo.a2mtl"));
	
	a2ematerial* scale_mat = new a2ematerial(e);
	scale_mat->load_material(e->data_path("scale.a2mtl"));

	// load the models and set new positions
	level = sce->create_a2emodel();
	level->load_model(e->data_path("move_level.a2m"));
	level->set_position(0.0f, 0.0f, 0.0f);
	level->set_material(level_mat);

	sphere = sce->create_a2emodel();
	sphere->load_model(e->data_path("player_sphere.a2m"));
	sphere->set_position(-2.0f, 15.0f, -2.0f);
	sphere->set_visible(false);
	sphere->set_radius(2.0f);
	sphere->set_material(scale_mat);

	player = sce->create_a2emodel();
	player->load_model(e->data_path("player.a2m"));
	player->set_hard_scale(0.02f, 0.02f, 0.02f);
	player->set_position(sphere->get_position()->x, sphere->get_position()->y - 2.0f, sphere->get_position()->z);
	player->set_material(scale_mat);

	sce->add_model(level);
	sce->add_model(sphere);
	sce->add_model(player);

	a2emodel* test = sce->create_a2emodel();
	test->load_model(e->data_path("a2logo.a2m"));
	test->set_position(0.0f, 50.0f, 0.0f);
	test->set_material(logo_mat);
	sce->add_model(test);

	spheres = new a2emodel*[cspheres];
	for(unsigned int i = 0; i < (unsigned int)sqrt((float)cspheres); i++) {
		for(unsigned int j = 0; j < (unsigned int)sqrt((float)cspheres); j++) {
			spheres[i*(unsigned int)sqrt((float)cspheres) + j] = sce->create_a2emodel();
			spheres[i*(unsigned int)sqrt((float)cspheres) + j]->load_model(e->data_path("sphere.a2m"));
			spheres[i*(unsigned int)sqrt((float)cspheres) + j]->set_position((float)i*5, 20.0f, (float)j*5);
			spheres[i*(unsigned int)sqrt((float)cspheres) + j]->set_scale(0.5f, 0.5f, 0.5f);
			spheres[i*(unsigned int)sqrt((float)cspheres) + j]->set_radius(1.0f);
			spheres[i*(unsigned int)sqrt((float)cspheres) + j]->set_material(scale_mat);
			sce->add_model(spheres[i*(unsigned int)sqrt((float)cspheres) + j]);
		}
	}

	light* l1 = new light(e, -50.0f, 0.0f, -50.0f);
	float lamb[] = { 0.3f, 0.3f, 0.3f, 1.0f};
	float ldif[] = { 0.7f, 0.7f, 0.7f, 1.0f};
	float lspc[] = { 1.0f, 1.0f, 1.0f, 1.0f};
	l1->set_lambient(lamb);
	l1->set_ldiffuse(ldif);
	l1->set_lspecular(lspc);
	sce->add_light(l1);

	// initialize ode
	o->init();

	// pass the models to ode
	o->add_object(level, true, ode_object::TRIMESH);
	sphere_obj = o->add_object(sphere, false, ode_object::SPHERE);
	spheres_obj = new ode_object*[cspheres];
	for(unsigned int i = 0; i < cspheres; i++) {
		spheres_obj[i] = o->add_object(spheres[i], false, ode_object::SPHERE);
	}

	//sphere_obj = o->get_ode_object(1);
	sphere_obj->set_max_force(max_force);

	// needed for fps counting
	unsigned int fps = 0;
	unsigned int fps_time = 0;
	char* tmp = new char[512];
	sprintf(tmp, "A2E Sample - Move Sample");

	refresh_time = SDL_GetTicks();
	walk_time = min_walk_time;
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
						case SDLK_UP:
							update_cam(UP);
							break;
						case SDLK_DOWN:
							update_cam(DOWN);
							break;
						case SDLK_r:
							dBodySetLinearVel(sphere_obj->get_body(), 0.0f, 0.0f, 0.0f);
							break;
						case SDLK_KP_PLUS:
							max_force += 1.0f;
							break;
						case SDLK_KP_MINUS:
							max_force -= 1.0f;
							break;
						case SDLK_SPACE: {
							unsigned int side = (unsigned int)sqrtf((float)cspheres);
							for(unsigned int i = 0; i < side; i++) {
								for(unsigned int j = 0; j < side; j++) {
									spheres_obj[i*side+j]->reset();
									dBodySetPosition(spheres_obj[i*side+j]->get_body(), (float)i*5, 20.0f, (float)j*5);
								}
							}
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
			sprintf(tmp, "A2E Sample - Move Sample | FPS: %u | Walk Force: %f | Pos: %f %f %f", fps,
				max_force, cam->get_position()->x, cam->get_position()->y, cam->get_position()->z);
			fps = 0;
			fps_time = SDL_GetTicks();
		}
		e->set_caption(tmp);

		update_cam(NONE);

		e->start_draw();

		cam->run();
		sce->draw();
		o->run(SDL_GetTicks() - refresh_time);

		e->stop_draw();
		refresh_time = SDL_GetTicks();

		// let's rotate the a2 logo a lil bit ;)
		test->set_rotation(0.0f, yrot, 0.0f);
		yrot += 0.5f;
	}

	delete [] tmp;

	o->close();

	// delete everything
	delete o;
	delete sce;
	delete scale_mat;
	delete logo_mat;
	delete level_mat;
	delete l1;
	delete s;
	delete cam;
	delete e;

	return 0;
}
