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
#include <SDL.h>
#include "msg.h"
#include "core.h"
#include <math.h>
using namespace std;

#include "win_dll_export.h"

/*! @class light
 *  @brief lighting routines
 *  @author flo
 *  @version 0.1
 *  @date 2005/01/29
 *  @todo nothing atm
 *  
 *  the lighting class
 */

class A2E_API light
{
public:
	light(float x, float y, float z);
	light(core::vertex3* pos);
	~light();

	//void draw();

	void set_position(core::vertex3* position);
	void set_lambient(float* lambient);
	void set_ldiffuse(float* ldiffuse);
	void set_lspecular(float* lspecular);
	void set_enabled(bool state);

	core::vertex3* get_position();
	float* get_lambient();
	float* get_ldiffuse();
	float* get_lspecular();
	bool is_enabled();

protected:
	msg m;
	core c;

	core::vertex3* position;

	float* lambient;
	float* ldiffuse;
	float* lspecular;

	bool enabled;

};

#endif
