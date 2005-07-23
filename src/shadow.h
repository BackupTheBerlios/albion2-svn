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
 
#ifndef __SHADOW_H__
#define __SHADOW_H__

#include <iostream>
#include <SDL/SDL.h>
#include "msg.h"
#include "core.h"
//#include "engine.h"
#include "a2emodel.h"
#include <math.h>
using namespace std;

#include "win_dll_export.h"

/*! @class shadow
 *  @brief shadowing routines
 *  @author flo
 *  @version 0.1
 *  @date 2005/01/21
 *  @todo nothing atm
 *  
 *  the shadowing class
 */

class A2E_API shadow
{
public:
	shadow();
	~shadow();

	void draw();

protected:
	msg m;
	core c;

};

#endif
