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

/*! draws a point on a surface
 *  @param surface the surface where we want to draw the point
 *  @param point the position of the point
 *  @param color the color of the point
 */
void gfx::draw_point(SDL_Surface* surface, gfx::pnt* point, unsigned int color) {
	// there is no need for a draw point function any more - commented out

	//int depth = surface->format->BytesPerPixel;
    // p is the address to the pixel we want to draw
	//Uint8 *p = (Uint8*)malloc(depth);
	//GLuint *p = new GLuint[1*1*depth*sizeof(GLuint)];
	//free(p);

    /*switch(depth) {
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
    }*/

	/*glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0.0, 800, 0.0, 600, -1.0, 1.0);
	
	glMatrixMode(GL_MODELVIEW);
	
	glPushMatrix();
	glLoadIdentity();

	glDrawBuffer(GL_BACK);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glRasterPos2i(point->x, 599 - point->y);
	//glDrawPixels(1, 1, GL_BGRA_EXT, GL_UNSIGNED_BYTE, p);

	glFlush();
	
	glPopMatrix();
	
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();*/

	//free(p);
}

/*! draws a line on a surface
 *  @param surface the surface where we want to draw the line
 *  @param point1 the position of the first point
 *  @param point2 the position of the second point
 *  @param color the color of the line
 */
void gfx::draw_line(SDL_Surface* surface, gfx::pnt* point1,
					gfx::pnt* point2, unsigned int color) {
	unsigned int width = point2->x - point1->x;
	unsigned int height = point2->y - point1->y;

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0.0, surface->w, 0.0, surface->h, -1.0, 1.0);
	
	glMatrixMode(GL_MODELVIEW);
	
	glPushMatrix();
	glLoadIdentity();

	glDrawBuffer(GL_BACK);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glTranslatef(0.0f, 0.0f, 0.0f);
	glBegin(GL_LINES);
	glColor3ub((color>>16) & 255, (color>>8) & 255, color & 255);
	glVertex2i(point1->x, surface->h - point1->y);
	glVertex2i(point2->x, surface->h - point2->y);
	glEnd();

	glFlush();
	
	glPopMatrix();
	
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
}

/*! draws a rectangle on a surface
 *  @param surface the surface where we want to draw the rectangle
 *  @param rectangle the rectangle itself
 *  @param color the color of the rectangle
 */
void gfx::draw_rectangle(SDL_Surface* surface, gfx::rect* rectangle, unsigned int color) {
	int dist_x1_x2 = rectangle->x2 - rectangle->x1;
	int dist_y1_y2 = rectangle->y2 - rectangle->y1;
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

	gfx::pnt* p1 = (gfx::pnt*)malloc(sizeof(gfx::pnt));
	gfx::pnt* p2 = (gfx::pnt*)malloc(sizeof(gfx::pnt));
	gfx::cord_to_pnt(p1, rectangle->x1, rectangle->y1);
	gfx::cord_to_pnt(p2, rectangle->x1 + dist_x1_x2, rectangle->y1);
	gfx::draw_line(surface, p1, p2, color);

	gfx::cord_to_pnt(p1, rectangle->x1, rectangle->y1 + dist_y1_y2);
	gfx::cord_to_pnt(p2, rectangle->x1 + dist_x1_x2, rectangle->y1 + dist_y1_y2);
	gfx::draw_line(surface, p1, p2, color);

	gfx::cord_to_pnt(p1, rectangle->x1, rectangle->y1);
	gfx::cord_to_pnt(p2, rectangle->x1, rectangle->y1 + dist_y1_y2);
	gfx::draw_line(surface, p1, p2, color);

	gfx::cord_to_pnt(p1, rectangle->x1 + dist_x1_x2, rectangle->y1);
	gfx::cord_to_pnt(p2, rectangle->x1 + dist_x1_x2, rectangle->y1 + dist_y1_y2);
	gfx::draw_line(surface, p1, p2, color);

	free(p1);
	free(p2);
}

/*! draws a two colored rectangle on a surface
 *! left + top border = first color
 *! right + bottom border = second color
 *  @param surface the surface where we want to draw the rectangle
 *  @param rectangle the rectangle itself
 *  @param color1 the first color of the rectangle
 *  @param color2 the second color of the rectangle
 */
void gfx::draw_2colored_rectangle(SDL_Surface* surface, gfx::rect* rectangle,
					unsigned int color1, unsigned int color2) {
	int dist_x1_x2 = rectangle->x2 - rectangle->x1;
	int dist_y1_y2 = rectangle->y2 - rectangle->y1;
	//int tmp;

	if(dist_x1_x2 < 0) { dist_x1_x2 = dist_x1_x2 * -1 + 1; }
	else { dist_x1_x2++; }

	if(dist_y1_y2 < 0) { dist_y1_y2 = dist_y1_y2 * -1 + 1; }
	else { dist_y1_y2++; }

	/*if(dist_x1_x2 < dist_y1_y2) {
	    tmp = dist_x1_x2;
	    dist_x1_x2 = dist_y1_y2;
	    dist_y1_y2 = tmp;
	}*/

	gfx::pnt* p1 = (gfx::pnt*)malloc(sizeof(gfx::pnt));
	gfx::pnt* p2 = (gfx::pnt*)malloc(sizeof(gfx::pnt));
	gfx::cord_to_pnt(p1, rectangle->x1, rectangle->y1);
	gfx::cord_to_pnt(p2, rectangle->x1 + dist_x1_x2, rectangle->y1);
	gfx::draw_line(surface, p1, p2, color1);

	gfx::cord_to_pnt(p1, rectangle->x1, rectangle->y1);
	gfx::cord_to_pnt(p2, rectangle->x1, rectangle->y1 + dist_y1_y2);
	gfx::draw_line(surface, p1, p2, color1);

	gfx::cord_to_pnt(p1, rectangle->x1, rectangle->y1 + dist_y1_y2);
	gfx::cord_to_pnt(p2, rectangle->x1 + dist_x1_x2, rectangle->y1 + dist_y1_y2);
	gfx::draw_line(surface, p1, p2, color2);

	gfx::cord_to_pnt(p1, rectangle->x1 + dist_x1_x2, rectangle->y1);
	gfx::cord_to_pnt(p2, rectangle->x1 + dist_x1_x2, rectangle->y1 + dist_y1_y2);
	gfx::draw_line(surface, p1, p2, color2);

	free(p1);
	free(p2);
}

/*! draws a filled rectangle on a surface
 *  @param surface the surface where we want to draw the filled rectangle
 *  @param rectangle the rectangle itself
 *  @param color the color of the filled rectangle
 */
void gfx::draw_filled_rectangle(SDL_Surface* surface, gfx::rect* rectangle,
								unsigned int color) {
	unsigned int width = rectangle->x2 - rectangle->x1;
	unsigned int height = rectangle->y2 - rectangle->y1;

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0.0, surface->w, 0.0, surface->h, -1.0, 1.0);
	
	glMatrixMode(GL_MODELVIEW);
	
	glPushMatrix();
	glLoadIdentity();

	glDrawBuffer(GL_BACK);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glTranslatef(0.0f, 0.0f, 0.0f);
	glBegin(GL_QUADS);
	glColor3ub((color>>16) & 255, (color>>8) & 255, color & 255);
	glVertex2i(rectangle->x1, surface->h - rectangle->y1);
	glVertex2i(rectangle->x2, surface->h - rectangle->y1);
	glVertex2i(rectangle->x2, surface->h - rectangle->y2);
	glVertex2i(rectangle->x1, surface->h - rectangle->y2);
	glEnd();

	glFlush();
	
	glPopMatrix();
	
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
}

/*! returns the sdl_color, we get from the function arguments and surface
 *  @param surface the surface (we need the surface to get the bpp), otherwise we'll get a false color
 *  @param red how much red (0 - 255)
 *  @param green how much green (0 - 255)
 *  @param blue how much blue (0 - 255)
 */
unsigned int gfx::get_color(SDL_Surface* surface, unsigned int red, unsigned int green, unsigned int blue) {
	return (unsigned int)SDL_MapRGBA(surface->format, red, green, blue, 255);
}

/*! returns the sdl_color, we get from the function arguments and surface
 *  @param surface the surface (we need the surface to get the bpp), otherwise we'll get a false color
 *  @param rgb red, green and blue in one value
 */
unsigned int gfx::get_color(SDL_Surface* surface, unsigned int rgb) {
	unsigned int red = (rgb & 0xFF0000) >> 16;
	unsigned int green = (rgb & 0x00FF00) >> 8;
	unsigned int blue = rgb & 0x0000FF;
	return (unsigned int)gfx::get_color(surface, red, green, blue);
}

/*! makes a rectangle out of 2 points
 *  @param x1 x cord point 1
 *  @param y1 y cord point 1
 *  @param x2 x cord point 2
 *  @param y2 y cord point 2
 */
void gfx::pnt_to_rect(gfx::rect* rectangle, unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2) {
	rectangle->x1 = x1;
	rectangle->y1 = y1;
	rectangle->x2 = x2;
	rectangle->y2 = y2;
}

/*! makes a rectangle out of 2 points and returns it
 *  @param x1 x cord point 1
 *  @param y1 y cord point 1
 *  @param x2 x cord point 2
 *  @param y2 y cord point 2
 */
gfx::rect* gfx::pnt_to_rect(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2) {
	gfx::rect* r = (gfx::rect*)malloc(sizeof(gfx::rect));
	r->x1 = x1;
	r->y1 = y1;
	r->x2 = x2;
	r->y2 = y2;
	return r;
}

/*! makes a point out of 2 cords
 *  @param x x cord
 *  @param y y cord
 */
void gfx::cord_to_pnt(gfx::pnt* point, unsigned int x, unsigned int y) {
	point->x = x;
	point->y = y;
}


/*! makes a point out of 2 cords and returns it
 *  @param x x cord
 *  @param y y cord
 */
gfx::pnt* gfx::cord_to_pnt(unsigned int x, unsigned int y) {
	gfx::pnt* p = (gfx::pnt*)malloc(sizeof(gfx::pnt));
	p->x = x;
	p->y = y;
	return p;
}

/*! returns true if point is in rectangle
 *  @param rectangle the rectangle
 *  @param point the point we want to test
 */
bool gfx::is_pnt_in_rectangle(gfx::rect* rectangle, gfx::pnt* point) {
	if(point->x >= rectangle->x1 && point->x <= rectangle->x2
		&& point->y >= rectangle->y1 && point->y <= rectangle->y2) {
		return true;
	}
	else {
		return false;
	}
}
