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
 
#ifndef __EVENT_H__
#define __EVENT_H__

#include <iostream>
#include <SDL.h>
#include "msg.h"
#include "core.h"
#include "engine.h"
using namespace std;

#include "win_dll_export.h"

/*! @class event
 *  @brief (sdl) event handler
 *  @author flo
 *  @version 0.1
 *  @date 2004/08/13
 *  @todo more functions
 *  
 *  the event handling class
 */

class A2E_API event
{
public:
	event();
	~event();

	void init(SDL_Event ievent);
	int is_event();
	SDL_Event get_event();
	void handle_events(unsigned int event_type);

	unsigned int get_lm_pressed_x();
	unsigned int get_lm_pressed_y();
protected:
	msg m;
	SDL_Event event_handle;

	//! left mouse button pressed (x coordinate)
	unsigned int lm_pressed_x;
	//! left mouse button pressed (y coordinate)
	unsigned int lm_pressed_y;
};

#endif
