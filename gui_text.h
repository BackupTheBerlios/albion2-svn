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
 
#ifndef __GUI_TEXT_H__
#define __GUI_TEXT_H__

#include <iostream>
#include <SDL.h>
#include <SDL_ttf.h>
#include "msg.h"
#include "core.h"
#include "gfx.h"
#include "event.h"
#include "engine.h"
using namespace std;

#include "win_dll_export.h"

/*! @class gui_text
 *  @brief gui text element functions
 *  @author flo
 *  @version 0.1
 *  @date 2004/08/13
 *  @todo more functions
 *  
 *  the gui_text class
 */

class A2E_API gui_text
{
public:
	gui_text();
	~gui_text();

	TTF_Font* open_font(char* font_name, unsigned int font_size);
	void close_font(TTF_Font* font);

	void draw_text();
	void set_engine_handler(engine* iengine);


	// gui text element variables functions

	unsigned int get_id();
	SDL_Surface* get_surface();
	gfx::pnt* get_point();

	void set_id(unsigned int id);
	void set_surface(SDL_Surface* surface);
	void set_point(gfx::pnt* point);

protected:
	msg m;
	core c;
	gfx g;

	engine* engine_handler;

	// gui text element variables

	//! text id
	unsigned int id;
	//! text surface (no need to be changed or accessed)
	SDL_Surface* surface;
	//! text starting point (x,y)
	gfx::pnt* point;

};

#endif
