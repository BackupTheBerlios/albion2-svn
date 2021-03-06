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
#include <SDL/SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include "core.h"
using namespace std;

#include "win_dll_export.h"

/*! @class gfx
 *  @brief graphical functions
 *  @author flo
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

	enum FADE_TYPE {
		FT_HORIZONTAL,
		FT_VERTICAL,
		FT_DIAGONAL
	};

	void set_surface(SDL_Surface* surface);

	void cord_to_pnt(core::pnt* point, unsigned int x, unsigned int y);
	void pnt_to_rect(gfx::rect* rectangle, unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2);
	core::pnt* cord_to_pnt(unsigned int x, unsigned int y);
	gfx::rect* pnt_to_rect(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2);

	void draw_point(core::pnt* point, unsigned int color);
	void draw_line(core::pnt* point1, core::pnt* point2, unsigned int color);
	void draw_line(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, unsigned int color);
	void draw_3d_line(vertex3* v1, vertex3* v2, unsigned int color);
	void draw_rectangle(gfx::rect* rectangle, unsigned int color);
	void draw_2colored_rectangle(gfx::rect* rectangle, unsigned int color1, unsigned int color2);
	void draw_filled_rectangle(gfx::rect* rectangle, unsigned int color);
	void draw_fade_rectangle(gfx::rect* rectangle, unsigned int color1, unsigned int color2, FADE_TYPE ft);
	void draw_bbox(core::aabbox* bbox, unsigned int color);

	bool is_pnt_in_rectangle(gfx::rect* rectangle, core::pnt* point);

	unsigned int get_color(unsigned int red, unsigned int green, unsigned int blue);
	unsigned int get_color(unsigned int rgb);
	unsigned int get_average_color(unsigned int color1, unsigned int color2);

	void begin_scissor();
	void set_scissor(gfx::rect* rectangle);
	void set_scissor(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2);
	void end_scissor();

	void set_flip(bool state);
	bool get_flip();

protected:
	SDL_Surface* screen;
	bool flip;

	core::pnt* points;
};

#endif
