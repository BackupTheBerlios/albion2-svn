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
 
#ifndef __GFX_H__
#define __GFX_H__

#ifdef WIN32
#include <windows.h>
#endif

#include <iostream>
#include <SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include "msg.h"
#include "core.h"
#include "net.h"
using namespace std;

#include "win_dll_export.h"

/*! @class gfx
 *  @brief graphical functions
 *  @author flo
 *  @version 0.3.1
 *  @date 2005/02/15
 *  @todo more functions
 *  
 *  the gfx class
 */

class A2E_API gfx
{
public:
	gfx();
	~gfx();

	struct rect {
		unsigned int x1;
		unsigned int y1;
		unsigned int x2;
		unsigned int y2;
	};

	struct pnt {
		unsigned int x;
		unsigned int y;
	};

	void cord_to_pnt(gfx::pnt* point, unsigned int x, unsigned int y);
	void pnt_to_rect(gfx::rect* rectangle, unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2);
	gfx::pnt* cord_to_pnt(unsigned int x, unsigned int y);
	gfx::rect* pnt_to_rect(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2);

	void draw_point(SDL_Surface* surface, gfx::pnt* point, unsigned int color);

	void draw_line(SDL_Surface* surface, gfx::pnt* point1, gfx::pnt* point2, unsigned int color);
	void draw_3d_line(core::vertex3* v1, core::vertex3* v2, unsigned int color);

	void draw_rectangle(SDL_Surface* surface, gfx::rect* rectangle, unsigned int color);
	void draw_2colored_rectangle(SDL_Surface* surface, gfx::rect* rectangle, unsigned int color1, unsigned int color2);
	void draw_filled_rectangle(SDL_Surface* surface, gfx::rect* rectangle, unsigned int color);
	bool is_pnt_in_rectangle(gfx::rect* rectangle, gfx::pnt* point);

	unsigned int get_color(SDL_Surface* surface, unsigned int red, unsigned int green, unsigned int blue);
	unsigned int get_color(SDL_Surface* surface, unsigned int rgb);
protected:
	msg m;
	core c;

	SDL_Surface* screen;
};

#endif
