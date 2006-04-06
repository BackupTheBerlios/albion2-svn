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

/*! draws a point
 *  @param point the position of the point
 *  @param color the color of the point
 */
void gfx::draw_point(core::pnt* point, unsigned int color) {
	glTranslatef(0.0f, 0.0f, 0.0f);
	glColor4f(((GLfloat)((color>>16) & 0xFF)) / 0xFF, ((GLfloat)((color>>8) & 0xFF)) / 0xFF, ((GLfloat)(color & 0xFF)) / 0xFF, 1.0f);
	glBegin(GL_POINTS);
		glVertex2d(point->x, screen->h - point->y - 1);
	glEnd();
}

/*! draws a line
 *  @param point1 the position of the first point
 *  @param point2 the position of the second point
 *  @param color the color of the line
 */
void gfx::draw_line(core::pnt* point1,
					core::pnt* point2, unsigned int color) {
	glTranslatef(0.0f, 0.0f, 0.0f);
	glColor4f(((GLfloat)((color>>16) & 0xFF)) / 0xFF, ((GLfloat)((color>>8) & 0xFF)) / 0xFF, ((GLfloat)(color & 0xFF)) / 0xFF, 1.0f);
	glBegin(GL_LINES);
	glVertex2i(point1->x, screen->h - point1->y);
	glVertex2i(point2->x, screen->h - point2->y - 1);
	glEnd();
}

/*! draws a line into a 3d space
 *  @param v1 the position of the first vertex
 *  @param v2 the position of the second vertex
 *  @param color the color of the line
 */
void gfx::draw_3d_line(vertex3* v1, vertex3* v2, unsigned int color) {
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	glTranslatef(0.0f, 0.0f, 0.0f);
	glColor4f(((GLfloat)((color>>16) & 0xFF)) / 0xFF, ((GLfloat)((color>>8) & 0xFF)) / 0xFF, ((GLfloat)(color & 0xFF)) / 0xFF, 1.0f);
	glBegin(GL_LINES);
		glVertex3f(v1->x, v1->y, v1->z);
		glVertex3f(v2->x, v2->y, v2->z);
	glEnd();
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
}

/*! draws a rectangle
 *  @param rectangle the rectangle itself
 *  @param color the color of the rectangle
 */
void gfx::draw_rectangle(gfx::rect* rectangle, unsigned int color) {
	glTranslatef(0.0f, 0.0f, 0.0f);
	glColor4f(((GLfloat)((color>>16) & 0xFF)) / 0xFF, ((GLfloat)((color>>8) & 0xFF)) / 0xFF, ((GLfloat)(color & 0xFF)) / 0xFF, 1.0f);
	glBegin(GL_LINES);
		glVertex2i(rectangle->x1, screen->h - rectangle->y1);
		glVertex2i(rectangle->x2, screen->h - rectangle->y1 - 1);

		glVertex2i(rectangle->x1, screen->h - rectangle->y2);
		glVertex2i(rectangle->x2, screen->h - rectangle->y2 - 1);

		glVertex2i(rectangle->x1, screen->h - rectangle->y1);
		glVertex2i(rectangle->x1, screen->h - rectangle->y2 - 1);

		glVertex2i(rectangle->x2, screen->h - rectangle->y1);
		glVertex2i(rectangle->x2, screen->h - rectangle->y2 - 1);
	glEnd();
}

/*! draws a two colored rectangle
 *! left + top border = first color
 *! right + bottom border = second color
 *  @param rectangle the rectangle itself
 *  @param color1 the first color of the rectangle
 *  @param color2 the second color of the rectangle
 */
void gfx::draw_2colored_rectangle(gfx::rect* rectangle,
					unsigned int color1, unsigned int color2) {
	glTranslatef(0.0f, 0.0f, 0.0f);
	glBegin(GL_LINES);
		glColor4f(((GLfloat)((color1>>16) & 0xFF)) / 0xFF, ((GLfloat)((color1>>8) & 0xFF)) / 0xFF, ((GLfloat)(color1 & 0xFF)) / 0xFF, 1.0f);
		glVertex2i(rectangle->x1, screen->h - rectangle->y1);
		glVertex2i(rectangle->x2, screen->h - rectangle->y1 - 1);

		glVertex2i(rectangle->x1, screen->h - rectangle->y1);
		glVertex2i(rectangle->x1, screen->h - rectangle->y2 - 1);

		glColor4f(((GLfloat)((color2>>16) & 0xFF)) / 0xFF, ((GLfloat)((color2>>8) & 0xFF)) / 0xFF, ((GLfloat)(color2 & 0xFF)) / 0xFF, 1.0f);
		glVertex2i(rectangle->x1, screen->h - rectangle->y2);
		glVertex2i(rectangle->x2, screen->h - rectangle->y2 - 1);

		glVertex2i(rectangle->x2, screen->h - rectangle->y1);
		glVertex2i(rectangle->x2, screen->h - rectangle->y2 - 1);
	glEnd();
}

/*! draws a filled rectangle
 *  @param rectangle the rectangle itself
 *  @param color the color of the filled rectangle
 */
void gfx::draw_filled_rectangle(gfx::rect* rectangle,
								unsigned int color) {
	glTranslatef(0.0f, 0.0f, 0.0f);
	glColor4f(((GLfloat)((color>>16) & 0xFF)) / 0xFF, ((GLfloat)((color>>8) & 0xFF)) / 0xFF, ((GLfloat)(color & 0xFF)) / 0xFF, 1.0f);
	glBegin(GL_QUADS);
	glVertex2i(rectangle->x1, screen->h - rectangle->y2 - 1);
	glVertex2i(rectangle->x2 + 1, screen->h - rectangle->y2 - 1);
	glVertex2i(rectangle->x2 + 1, screen->h - rectangle->y1);
	glVertex2i(rectangle->x1, screen->h - rectangle->y1);
	glEnd();
}

/*! returns the sdl_color that we get from the function arguments and the screen surface
 *  @param red how much red (0 - 255)
 *  @param green how much green (0 - 255)
 *  @param blue how much blue (0 - 255)
 */
unsigned int gfx::get_color(unsigned int red, unsigned int green, unsigned int blue) {
	return (unsigned int)SDL_MapRGBA(screen->format, (Uint8)red, (Uint8)green, (Uint8)blue, 255);
}

/*! returns the sdl_color that we get from the function arguments and the screen surface
 *  @param rgb red, green and blue in one value
 */
unsigned int gfx::get_color(unsigned int rgb) {
	unsigned int red = (rgb & 0xFF0000) >> 16;
	unsigned int green = (rgb & 0x00FF00) >> 8;
	unsigned int blue = rgb & 0x0000FF;
	return (unsigned int)gfx::get_color(red, green, blue);
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
	return false;
}

/*! sets the screen surface
 *  @param surface a pointer to a surface object
 */
void gfx::set_surface(SDL_Surface* surface) {
	gfx::screen = surface;
}

//! begins/enables the scissor
void gfx::begin_scissor() {
	glEnable(GL_SCISSOR_TEST);
}

/*! sets the scissor
 *  @param rectangle the scissor box
 */
void gfx::set_scissor(gfx::rect* rectangle) {
	gfx::set_scissor(rectangle->x1, rectangle->y1, rectangle->x2, rectangle->y2);
}

/*! sets the scissor
 *  @param x1 x1 value of the scissor box
 *  @param y1 y1 value of the scissor box
 *  @param x2 x2 value of the scissor box
 *  @param y2 y2 value of the scissor box
 */
void gfx::set_scissor(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2) {
	glScissor(x1, screen->h - y2, x2-x1, (y2 - y1));
}

//! ends/disables scissor test
void gfx::end_scissor() {
	glDisable(GL_SCISSOR_TEST);
}
