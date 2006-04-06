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
 
#ifndef __LIGHT_H__
#define __LIGHT_H__

#include <iostream>
#include <cmath>
#include <SDL/SDL.h>
#include "msg.h"
#include "core.h"
#include "engine.h"
using namespace std;

#include "win_dll_export.h"

/*! @class light
 *  @brief lighting routines
 *  @author flo
 *  @todo nothing atm
 *  
 *  the lighting class
 */

class A2E_API light
{
public:
	light(engine* e, float x, float y, float z);
	light(engine* e, vertex3* pos);
	~light();

	void set_position(float x, float y, float z);
	void set_lambient(float* lambient);
	void set_ldiffuse(float* ldiffuse);
	void set_lspecular(float* lspecular);
	void set_spot_direction(float* direction);
	void set_spot_cutoff(float angle);
	void set_enabled(bool state);
	void set_constant_attenuation(float constant);
	void set_linear_attenuation(float linear);
	void set_quadratic_attenuation(float quadratic);

	vertex3* get_position();
	float* get_lambient();
	float* get_ldiffuse();
	float* get_lspecular();
	float* get_spot_direction();
	float get_spot_cutoff();
	bool is_enabled();
	float get_constant_attenuation();
	float get_linear_attenuation();
	float get_quadratic_attenuation();

protected:
	msg* m;
	core* c;
	engine* e;

	vertex3* position;

	float* lambient;
	float* ldiffuse;
	float* lspecular;
	float* spot_dir;
	float cutoff;

	float constant_attenuation;
	float linear_attenuation;
	float quadratic_attenuation;

	bool enabled;

};

#endif
