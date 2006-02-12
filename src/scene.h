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
 
#ifndef __SCENE_H__
#define __SCENE_H__

#include <iostream>
#include <SDL/SDL.h>
#include "msg.h"
#include "core.h"
#include "engine.h"
#include "a2emodel.h"
#include "a2eanim.h"
#include "light.h"
#include "shader.h"
#include <cmath>
using namespace std;

#include "win_dll_export.h"

/*! @class scene
 *  @brief a2e scene manager
 *  @author flo
 *  @todo implement a method to render all models in sequencing from far to near to the cameras pos, depending on their position
 *  
 *  the scene manager class
 */

class A2E_API scene
{
public:
	scene(engine* e, shader* s);
	~scene();

	a2eanim* create_a2eanim();
	a2emodel* create_a2emodel();

	void draw();
	void add_model(a2emodel* model);
	void add_model(a2eanim* model);
	void delete_model(a2emodel* model);
	void delete_model(a2eanim* model);
	void add_light(light* light);

	void set_position(float x, float y, float z);
	void set_mambient(float* mambient);
	void set_mdiffuse(float* mdiffuse);
	void set_mspecular(float* mspecular);
	void set_mshininess(float* mshininess);
	void set_light(bool state);

	vertex3* get_position();
	float* get_mambient();
	float* get_mdiffuse();
	float* get_mspecular();
	float* get_mshininess();
	bool get_light();

protected:
	msg* m;
	core* c;
	engine* e;
	shader* s;

	vertex3* position;

	unsigned int cmodels;
	a2emodel* models[256];

	unsigned int camodels;
	a2eanim* amodels[256];

	unsigned int clights;
	light* lights[8];

	float* mambient;
	float* mdiffuse;
	float* mspecular;
	float* mshininess;

	//! specifies if lighting is enabled in this scene
	bool is_light;

};

#endif
