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

/*! there is no function currently
 */
gfx::gfx() {
}

/*! there is no function currently
 */
gfx::~gfx() {
}

/*! draws a point on a surface - deprecated!
 *  @param surface the surface where we want to draw the point
 *  @param point the position of the point
 *  @param color the color of the point
 */
void gfx::draw_point(SDL_Surface* surface, core::pnt* point, unsigned int color) {
	// outdated - should be taken out soon or be reimplemented,
	// but remember that you need too much cpu power if you just
	// draw a single point in opengl
}

/*! draws a line on a surface
 *  @param surface the surface where we want to draw the line
 *  @param point1 the position of the first point
 *  @param point2 the position of the second point
 *  @param color the color of the line
 */
void gfx::draw_line(SDL_Surface* surface, core::pnt* point1,
					core::pnt* point2, unsigned int color) {
	glTranslatef(0.0f, 0.0f, 0.0f);
	glColor3f(((GLfloat)((color>>16) & 0xFF)) / 0xFF, ((GLfloat)((color>>8) & 0xFF)) / 0xFF, ((GLfloat)(color & 0xFF)) / 0xFF);
	glBegin(GL_LINES);
	glVertex2i(point1->x, surface->h - point1->y);
	glVertex2i(point2->x, surface->h - point2->y - 1);
	glEnd();
}

/*! draws a line into a 3d space
 *  @param v1 the position of the first vertex
 *  @param v2 the position of the second vertex
 *  @param color the color of the line
 */
void gfx::draw_3d_line(vertex3* v1, vertex3* v2, unsigned int color) {
	glTranslatef(0.0f, 0.0f, 0.0f);
	glColor3f(((GLfloat)((color>>16) & 0xFF)) / 0xFF, ((GLfloat)((color>>8) & 0xFF)) / 0xFF, ((GLfloat)(color & 0xFF)) / 0xFF);
	glBegin(GL_LINES);
		glVertex3f(v1->x, v1->y, v1->z);
		glVertex3f(v2->x, v2->y, v2->z);
	glEnd();
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

	if(dist_x1_x2 < 0) { dist_x1_x2 = (dist_x1_x2 * -1) + 1; }
	if(dist_y1_y2 < 0) { dist_y1_y2 = (dist_y1_y2 * -1) + 1; }

	if(dist_x1_x2 < dist_y1_y2) {
	    tmp = dist_x1_x2;
	    dist_x1_x2 = dist_y1_y2;
	    dist_y1_y2 = tmp;
	}

	core::pnt* p1 = new core::pnt();
	core::pnt* p2 = new core::pnt();
	gfx::cord_to_pnt(p1, rectangle->x1, rectangle->y1);
	gfx::cord_to_pnt(p2, rectangle->x1 + dist_x1_x2, rectangle->y1);
	gfx::draw_line(surface, p1, p2, color);

	gfx::cord_to_pnt(p1, rectangle->x1 + dist_x1_x2, rectangle->y1);
	gfx::cord_to_pnt(p2, rectangle->x1 + dist_x1_x2, rectangle->y1 + dist_y1_y2);
	gfx::draw_line(surface, p1, p2, color);

	gfx::cord_to_pnt(p1, rectangle->x1, rectangle->y1 + dist_y1_y2);
	gfx::cord_to_pnt(p2, rectangle->x1 + dist_x1_x2, rectangle->y1 + dist_y1_y2);
	gfx::draw_line(surface, p1, p2, color);

	gfx::cord_to_pnt(p1, rectangle->x1, rectangle->y1);
	gfx::cord_to_pnt(p2, rectangle->x1, rectangle->y1 + dist_y1_y2);
	gfx::draw_line(surface, p1, p2, color);

	delete p1;
	delete p2;
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

	if(dist_x1_x2 < 0) { dist_x1_x2 = (dist_x1_x2 * -1) + 1; }
	if(dist_y1_y2 < 0) { dist_y1_y2 = (dist_y1_y2 * -1) + 1; }

	core::pnt* p1 = new core::pnt();
	core::pnt* p2 = new core::pnt();
	gfx::cord_to_pnt(p1, rectangle->x1, rectangle->y1);
	gfx::cord_to_pnt(p2, rectangle->x1 + dist_x1_x2, rectangle->y1);
	gfx::draw_line(surface, p1, p2, color1);

	gfx::cord_to_pnt(p1, rectangle->x1, rectangle->y1 + dist_y1_y2);
	gfx::cord_to_pnt(p2, rectangle->x1 + dist_x1_x2, rectangle->y1 + dist_y1_y2);
	gfx::draw_line(surface, p1, p2, color2);

	gfx::cord_to_pnt(p1, rectangle->x1, rectangle->y1);
	gfx::cord_to_pnt(p2, rectangle->x1, rectangle->y1 + dist_y1_y2);
	gfx::draw_line(surface, p1, p2, color1);

	gfx::cord_to_pnt(p1, rectangle->x1 + dist_x1_x2, rectangle->y1);
	gfx::cord_to_pnt(p2, rectangle->x1 + dist_x1_x2, rectangle->y1 + dist_y1_y2);
	gfx::draw_line(surface, p1, p2, color2);

	delete p1;
	delete p2;
}

/*! draws a filled rectangle on a surface
 *  @param surface the surface where we want to draw the filled rectangle
 *  @param rectangle the rectangle itself
 *  @param color the color of the filled rectangle
 */
void gfx::draw_filled_rectangle(SDL_Surface* surface, gfx::rect* rectangle,
								unsigned int color) {
	glTranslatef(0.0f, 0.0f, 0.0f);
	glColor3f(((GLfloat)((color>>16) & 0xFF)) / 0xFF, ((GLfloat)((color>>8) & 0xFF)) / 0xFF, ((GLfloat)(color & 0xFF)) / 0xFF);
	glBegin(GL_QUADS);
	glVertex2i(rectangle->x1, surface->h - rectangle->y2 - 1);
	glVertex2i(rectangle->x2 + 1, surface->h - rectangle->y2 - 1);
	glVertex2i(rectangle->x2 + 1, surface->h - rectangle->y1);
	glVertex2i(rectangle->x1, surface->h - rectangle->y1);
	/*glVertex2i(rectangle->x1, surface->h - rectangle->y2);
	glVertex2i(rectangle->x2, surface->h - rectangle->y2);
	glVertex2i(rectangle->x2, surface->h - rectangle->y1);
	glVertex2i(rectangle->x1, surface->h - rectangle->y1);*/
	glEnd();
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
 *  @param rectangle a pointer to a rectangle object
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
	gfx::rect* r = new gfx::rect();
	r->x1 = x1;
	r->y1 = y1;
	r->x2 = x2;
	r->y2 = y2;
	return r;
}

/*! makes a point out of 2 cords
 *  @param point a pointer to a pnt object
 *  @param x x coordinate
 *  @param y y coordinate
 */
void gfx::cord_to_pnt(core::pnt* point, unsigned int x, unsigned int y) {
	point->x = x;
	point->y = y;
}

/*! makes a point out of 2 cords and returns it
 *  @param x x cord
 *  @param y y cord
 */
core::pnt* gfx::cord_to_pnt(unsigned int x, unsigned int y) {
	core::pnt* p = new core::pnt();
	p->x = x;
	p->y = y;
	return p;
}

/*! returns true if point is in rectangle
 *  @param rectangle the rectangle
 *  @param point the point we want to test
 */
bool gfx::is_pnt_in_rectangle(gfx::rect* rectangle, core::pnt* point) {
	if(point->x >= rectangle->x1 && point->x <= rectangle->x2
		&& point->y >= rectangle->y1 && point->y <= rectangle->y2) {
		return true;
	}
	else {
		return false;
	}
}
