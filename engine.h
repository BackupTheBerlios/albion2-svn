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
 
#ifndef __ENGINE_H__
#define __ENGINE_H__

#include <iostream>
#include <SDL.h>
#include "msg.h"
#include "core.h"
#include "net.h"
#include "gui_style.h"
using namespace std;

#include "win_dll_export.h"

/*! @class engine
 *  @brief main engine
 *  @author laxity
 *  @author flo
 *  @version 0.1.2
 *  @date 2004/08/13
 *  @todo more functions
 *  
 *  the main engine
 */

class A2E_API engine
{
public:
	engine();
	~engine();
	void init(unsigned int width = 640, unsigned int height = 400, unsigned int depth = 16, bool fullscreen = false);
	void set_width(unsigned int new_width);
	void set_height(unsigned int new_height);
	void start_draw();
	void stop_draw();
	SDL_Surface* get_screen();

	void set_caption(char* caption);
	char* get_caption();

	gui_style get_gstyle();
	void set_color_scheme(gui_style::COLOR_SCHEME scheme);
protected:
	unsigned int width, height, depth, flags;
	SDL_Surface *screen;
	msg m;
	gui_style gstyle;
};

#endif
