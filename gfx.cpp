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

#include "gfx.h"
#include "engine.h"
#include "msg.h"
#include "core.h"

/*! there is no function currently
*/
gfx::gfx() {
}

/*! there is no function currently
*/
gfx::~gfx() {
}

void gfx::draw_point(SDL_Surface* surface, unsigned int x, unsigned int y, unsigned int color) {
	int depth = surface->format->BytesPerPixel;
    // p is the address to the pixel we want to draw
    Uint8 *p = (Uint8*)surface->pixels + y * surface->pitch + x * depth;

    switch(depth) {
		// 8bpp => 1 byte per pixel
		case 1:
			*p = color;
			break;

		// 16bpp => 2 byte per pixel
		case 2:
			*(Uint16*)p = color;
			break;

		// 24bpp => 3 byte per pixel
		case 3:
			if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
				p[0] = (color >> 16) & 0xff;
				p[1] = (color >> 8) & 0xff;
				p[2] = color & 0xff;
			}
			else {
				p[0] = color & 0xff;
				p[1] = (color >> 8) & 0xff;
				p[2] = (color >> 16) & 0xff;
			}
			break;

		// 32bpp => 4 byte per pixel
		case 4:
			*(Uint32*)p = color;
			break;
    }
}

void gfx::draw_line(SDL_Surface* surface, unsigned int x1, unsigned int y1,
					unsigned int x2, unsigned int y2, unsigned int color) {
	int cpx = surface->format->BytesPerPixel;
	int cpy = surface->pitch;
    // p is the address to the pixel we want to draw
    Uint8 *p = (Uint8*)surface->pixels + y1 * cpy + x1 * cpx;

	int x = 0, y = 0, tmp, sign_x, sign_y;

	int dist_x1_x2 = x2 - x1;
	int dist_y1_y2 = y2 - y1;

	if(dist_x1_x2 < 0) { dist_x1_x2 = dist_x1_x2 * -1 + 1; sign_x = -1; }
	else { dist_x1_x2++; sign_x = 1; }

	if(dist_y1_y2 < 0) { dist_y1_y2 = dist_y1_y2 * -1 + 1; sign_y = -1; }
	else { dist_y1_y2++; sign_y = 1; }

	cpx *= sign_x;
	cpy *= sign_y;

	if(dist_x1_x2 < dist_y1_y2) {
	    tmp = dist_x1_x2;
	    dist_x1_x2 = dist_y1_y2;
	    dist_y1_y2 = tmp;
	    tmp = cpx;
	    cpx = cpy;
	    cpy = tmp;
	}

    switch(surface->format->BytesPerPixel) {
		// 8bpp => 1 byte per pixel
		case 1:
			for(; x < dist_x1_x2; x++, p += cpx) {
				*p = color;
				y += dist_y1_y2;
				if(y >= dist_x1_x2) {
					y -= dist_x1_x2;
					p += cpy;
				}
			}
			break;

		// 16bpp => 2 byte per pixel
		case 2:
			for(; x < dist_x1_x2; x++, p += cpx) {
				*(Uint16*)p = color;
				y += dist_y1_y2;
				if(y >= dist_x1_x2) {
					y -= dist_x1_x2;
					p += cpy;
				}
			}
			break;

		// 24bpp => 3 byte per pixel
		case 3:
			for(; x < dist_x1_x2; x++, p += cpx) {
				if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
					p[0] = (color >> 16) & 0xff;
					p[1] = (color >> 8) & 0xff;
					p[2] = color & 0xff;
				}
				else {
					p[0] = color & 0xff;
					p[1] = (color >> 8) & 0xff;
					p[2] = (color >> 16) & 0xff;
				}
				y += dist_y1_y2;
				if(y >= dist_x1_x2) {
					y -= dist_x1_x2;
					p += cpy;
				}
			}
			break;

		// 32bpp => 4 byte per pixel
		case 4:
			for(; x < dist_x1_x2; x++, p += cpx) {
				*(Uint32*)p = color;
				y += dist_y1_y2;
				if(y >= dist_x1_x2) {
					y -= dist_x1_x2;
					p += cpy;
				}
			}
			break;
    }
}

void gfx::draw_rectangle(SDL_Surface* surface, unsigned int x1, unsigned int y1,
					unsigned int x2, unsigned int y2, unsigned int color) {
	int dist_x1_x2 = x2 - x1;
	int dist_y1_y2 = y2 - y1;
	int tmp;

	if(dist_x1_x2 < 0) { dist_x1_x2 = dist_x1_x2 * -1 + 1; }
	else { dist_x1_x2++; }

	if(dist_y1_y2 < 0) { dist_y1_y2 = dist_y1_y2 * -1 + 1; }
	else { dist_y1_y2++; }

	if(dist_x1_x2 < dist_y1_y2) {
	    tmp = dist_x1_x2;
	    dist_x1_x2 = dist_y1_y2;
	    dist_y1_y2 = tmp;
	}

	gfx::draw_line(surface, x1, y1, x1 + dist_x1_x2, y1, color);
	gfx::draw_line(surface, x1, y1 + dist_y1_y2, x1 + dist_x1_x2, y1 + dist_y1_y2, color);
	gfx::draw_line(surface, x1, y1, x1, y1 + dist_y1_y2, color);
	gfx::draw_line(surface, x1 + dist_x1_x2, y1, x1 + dist_x1_x2, y1 + dist_y1_y2, color);
}
