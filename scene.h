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
#include <SDL.h>
#include "msg.h"
#include "core.h"
#include "engine.h"
#include "a2emesh.h"
#include <math.h>
using namespace std;

#include "win_dll_export.h"

/*! @class scene
 *  @brief a2e scene manager
 *  @author flo
 *  @version 0.1
 *  @date 2004/09/12
 *  @todo more functions
 *  
 *  the scene manager class
 */

class A2E_API scene
{
public:
	scene();
	~scene();

	void draw();
	void add_model(a2emesh* model);

	void set_position(float x, float y, float z);
	core::vertex* get_position();

protected:
	msg m;
	core c;

	unsigned int cmodels;

	core::vertex* position;

	a2emesh* models[256];
};

#endif
