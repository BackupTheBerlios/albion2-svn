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
	flip = false;

	unsigned int max_point_count = 2048; // increase max point count if we have to render a line that is longer than 2048 pixel
	points = new core::pnt[max_point_count];
}

/*! there is no function currently
 */
gfx::~gfx() {
	delete [] points;
}

/*! draws a point
 *  @param point the position of the point
 *  @param color the color of the point
 */
void gfx::draw_point(core::pnt* point, unsigned int color) {
	glTranslatef(0.0f, 0.0f, 0.0f);
	glEnable(GL_BLEND);
	glColor4f(((GLfloat)((color>>16) & 0xFF)) / 0xFF, ((GLfloat)((color>>8) & 0xFF)) / 0xFF, ((GLfloat)(color & 0xFF)) / 0xFF, 1.0f - (((GLfloat)((color>>24) & 0xFF)) / 0xFF));
	glBegin(GL_POINTS);
		glVertex2i(point->x, point->y+1);
	glEnd();
	glDisable(GL_BLEND);
}

/*! draws a line
 *  @param point1 the position of the first point
 *  @param point2 the position of the second point
 *  @param color the color of the line
 */
void gfx::draw_line(core::pnt* point1, core::pnt* point2, unsigned int color) {
	draw_line(point1->x, point1->y, point2->x, point2->y, color);
}

void gfx::draw_line(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, unsigned int color) {
	glTranslatef(0.0f, 0.0f, 0.0f);
	glColor4f(((GLfloat)((color>>16) & 0xFF)) / 0xFF, ((GLfloat)((color>>8) & 0xFF)) / 0xFF, ((GLfloat)(color & 0xFF)) / 0xFF, 1.0f - (((GLfloat)((color>>24) & 0xFF)) / 0xFF));

	/*glBegin(GL_LINES);
		glVertex2i(point1->x, point1->y);
		glVertex2i(point2->x, point2->y);
	glEnd();*/
	// since GL_LINES is totally inaccurate in 2d mode, i decided to use a self implemented function
	// again to draw lines (it's the same version as the old sdl function, but it uses opengl to draw points)
	unsigned int dx = x1;
	unsigned int dy = y1;

	int x = 0, y = 0, sign_x, sign_y, cpx = 1, cpy = 1;

	int dist_x1_x2 = x2 - x1;
	int dist_y1_y2 = y2 - y1;

	if(dist_x1_x2 < 0) { dist_x1_x2 = dist_x1_x2 * -1 + 1; sign_x = -1; }
	else { dist_x1_x2++; sign_x = 1; }

	if(dist_y1_y2 < 0) { dist_y1_y2 = dist_y1_y2 * -1 + 1; sign_y = -1; }
	else { dist_y1_y2++; sign_y = 1; }

	cpx *= sign_x;
	cpy *= sign_y;

	// old non vertex array draw code
	/*glBegin(GL_POINTS);
	if(dist_x1_x2 < dist_y1_y2) {
	    for(; x < dist_y1_y2; x++, dy += cpy) {
			glVertex2i(dx, dy+1);
			y += dist_x1_x2;
			if(y >= dist_y1_y2) {
				y -= dist_y1_y2;
				dx += cpx;
			}
		}
	}
	else {
		for(; x < dist_x1_x2; x++, dx += cpx) {
			glVertex2i(dx, dy+1);
			y += dist_y1_y2;
			if(y >= dist_x1_x2) {
				y -= dist_x1_x2;
				dy += cpy;
			}
		}
	}
	glEnd();*/

	// use a vertex array to draw the points (is faster, measured performance increase with 1000 lines: ~ +33%)
	unsigned int point_count = dist_x1_x2 > dist_y1_y2 ? dist_x1_x2 : dist_y1_y2;
	unsigned int i = 0;
	if(dist_x1_x2 < dist_y1_y2) {
	    for(; x < dist_y1_y2; x++, dy += cpy) {
			points[i].x = dx;
			points[i].y = dy+1;
			i++;
			y += dist_x1_x2;
			if(y >= dist_y1_y2) {
				y -= dist_y1_y2;
				dx += cpx;
			}
		}
	}
	else {
		for(; x < dist_x1_x2; x++, dx += cpx) {
			points[i].x = dx;
			points[i].y = dy+1;
			i++;
			y += dist_y1_y2;
			if(y >= dist_x1_x2) {
				y -= dist_x1_x2;
				dy += cpy;
			}
		}
	}

	glEnable(GL_BLEND);
	glVertexPointer(2, GL_INT, 0, points);
	glEnableClientState(GL_VERTEX_ARRAY);
	glDrawArrays(GL_POINTS, 0, point_count);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisable(GL_BLEND);
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
	/*glTranslatef(0.0f, 0.0f, 0.0f);
	glColor4f(((GLfloat)((color>>16) & 0xFF)) / 0xFF, ((GLfloat)((color>>8) & 0xFF)) / 0xFF, ((GLfloat)(color & 0xFF)) / 0xFF, 1.0f);
	glBegin(GL_LINES);
		glVertex2i(rectangle->x1, rectangle->y1);
		glVertex2i(rectangle->x2, rectangle->y1 - 1);

		glVertex2i(rectangle->x1, rectangle->y2);
		glVertex2i(rectangle->x2, rectangle->y2 - 1);

		glVertex2i(rectangle->x1, rectangle->y1);
		glVertex2i(rectangle->x1, rectangle->y2 - 1);

		glVertex2i(rectangle->x2, rectangle->y1);
		glVertex2i(rectangle->x2, rectangle->y2 - 1);
	glEnd();*/
	draw_line(rectangle->x1, rectangle->y1, rectangle->x2, rectangle->y1, color);
	draw_line(rectangle->x1, rectangle->y2, rectangle->x2, rectangle->y2, color);
	draw_line(rectangle->x1, rectangle->y1, rectangle->x1, rectangle->y2, color);
	draw_line(rectangle->x2, rectangle->y1, rectangle->x2, rectangle->y2, color);
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
	/*glTranslatef(0.0f, 0.0f, 0.0f);
	glBegin(GL_LINES);
		glColor4f(((GLfloat)((color1>>16) & 0xFF)) / 0xFF, ((GLfloat)((color1>>8) & 0xFF)) / 0xFF, ((GLfloat)(color1 & 0xFF)) / 0xFF, 1.0f);
		glVertex2i(rectangle->x1, rectangle->y1);
		glVertex2i(rectangle->x2, rectangle->y1 - 1);

		glVertex2i(rectangle->x1, rectangle->y1);
		glVertex2i(rectangle->x1, rectangle->y2 - 1);

		glColor4f(((GLfloat)((color2>>16) & 0xFF)) / 0xFF, ((GLfloat)((color2>>8) & 0xFF)) / 0xFF, ((GLfloat)(color2 & 0xFF)) / 0xFF, 1.0f);
		glVertex2i(rectangle->x1, rectangle->y2);
		glVertex2i(rectangle->x2, rectangle->y2 - 1);

		glVertex2i(rectangle->x2, rectangle->y1);
		glVertex2i(rectangle->x2, rectangle->y2 - 1);
	glEnd();*/
	draw_line(rectangle->x1, rectangle->y1, rectangle->x2, rectangle->y1, color1);
	draw_line(rectangle->x1, rectangle->y1, rectangle->x1, rectangle->y2, color1);
	draw_line(rectangle->x1, rectangle->y2, rectangle->x2, rectangle->y2, color2);
	draw_line(rectangle->x2, rectangle->y1, rectangle->x2, rectangle->y2, color2);
}

/*! draws a filled rectangle
 *  @param rectangle the rectangle itself
 *  @param color the color of the filled rectangle
 */
void gfx::draw_filled_rectangle(gfx::rect* rectangle, unsigned int color) {
	glTranslatef(0.0f, 0.0f, 0.0f);
	glColor4f(((GLfloat)((color>>16) & 0xFF)) / 0xFF, ((GLfloat)((color>>8) & 0xFF)) / 0xFF, ((GLfloat)(color & 0xFF)) / 0xFF, 1.0f - (((GLfloat)((color>>24) & 0xFF)) / 0xFF));
	glBegin(GL_QUADS);
		if(!flip) {
			glVertex2i(rectangle->x1, rectangle->y2+1);
			glVertex2i(rectangle->x2 + 1, rectangle->y2+1);
			glVertex2i(rectangle->x2 + 1, rectangle->y1);
			glVertex2i(rectangle->x1, rectangle->y1);
		}
		else {
			glVertex2i(rectangle->x1, rectangle->y1);
			glVertex2i(rectangle->x2 + 1, rectangle->y1);
			glVertex2i(rectangle->x2 + 1, rectangle->y2+1);
			glVertex2i(rectangle->x1, rectangle->y2+1);
		}
	glEnd();
}

void gfx::draw_fade_rectangle(gfx::rect* rectangle, unsigned int color1, unsigned int color2, FADE_TYPE ft) {
	glTranslatef(0.0f, 0.0f, 0.0f);
	glEnable(GL_BLEND);
	glBegin(GL_QUADS);
	switch(ft) {
		case gfx::FT_HORIZONTAL:
			if(!flip) {
				glColor4f(((GLfloat)((color1>>16) & 0xFF)) / 0xFF, ((GLfloat)((color1>>8) & 0xFF)) / 0xFF, ((GLfloat)(color1 & 0xFF)) / 0xFF, 1.0f - (((GLfloat)((color1>>24) & 0xFF)) / 0xFF));
				glVertex2i(rectangle->x2 + 1, rectangle->y1);
				glVertex2i(rectangle->x1, rectangle->y1);
				glColor4f(((GLfloat)((color2>>16) & 0xFF)) / 0xFF, ((GLfloat)((color2>>8) & 0xFF)) / 0xFF, ((GLfloat)(color2 & 0xFF)) / 0xFF, 1.0f - (((GLfloat)((color2>>24) & 0xFF)) / 0xFF));
				glVertex2i(rectangle->x1, rectangle->y2+1);
				glVertex2i(rectangle->x2 + 1, rectangle->y2+1);
			}
			else {
				glColor4f(((GLfloat)((color2>>16) & 0xFF)) / 0xFF, ((GLfloat)((color2>>8) & 0xFF)) / 0xFF, ((GLfloat)(color2 & 0xFF)) / 0xFF, 1.0f - (((GLfloat)((color2>>24) & 0xFF)) / 0xFF));
				glVertex2i(rectangle->x2 + 1, rectangle->y2+1);
				glVertex2i(rectangle->x1, rectangle->y2+1);
				glColor4f(((GLfloat)((color1>>16) & 0xFF)) / 0xFF, ((GLfloat)((color1>>8) & 0xFF)) / 0xFF, ((GLfloat)(color1 & 0xFF)) / 0xFF, 1.0f - (((GLfloat)((color1>>24) & 0xFF)) / 0xFF));
				glVertex2i(rectangle->x1, rectangle->y1);
				glVertex2i(rectangle->x2 + 1, rectangle->y1);
			}
			break;
		case gfx::FT_VERTICAL:
			if(!flip) {
				glColor4f(((GLfloat)((color1>>16) & 0xFF)) / 0xFF, ((GLfloat)((color1>>8) & 0xFF)) / 0xFF, ((GLfloat)(color1 & 0xFF)) / 0xFF, 1.0f - (((GLfloat)((color1>>24) & 0xFF)) / 0xFF));
				glVertex2i(rectangle->x1, rectangle->y1);
				glVertex2i(rectangle->x1, rectangle->y2+1);
				glColor4f(((GLfloat)((color2>>16) & 0xFF)) / 0xFF, ((GLfloat)((color2>>8) & 0xFF)) / 0xFF, ((GLfloat)(color2 & 0xFF)) / 0xFF, 1.0f - (((GLfloat)((color2>>24) & 0xFF)) / 0xFF));
				glVertex2i(rectangle->x2 + 1, rectangle->y2+1);
				glVertex2i(rectangle->x2 + 1, rectangle->y1);
			}
			else {
				glColor4f(((GLfloat)((color2>>16) & 0xFF)) / 0xFF, ((GLfloat)((color2>>8) & 0xFF)) / 0xFF, ((GLfloat)(color2 & 0xFF)) / 0xFF, 1.0f - (((GLfloat)((color2>>24) & 0xFF)) / 0xFF));
				glVertex2i(rectangle->x2 + 1, rectangle->y1);
				glVertex2i(rectangle->x2 + 1, rectangle->y2+1);
				glColor4f(((GLfloat)((color1>>16) & 0xFF)) / 0xFF, ((GLfloat)((color1>>8) & 0xFF)) / 0xFF, ((GLfloat)(color1 & 0xFF)) / 0xFF, 1.0f - (((GLfloat)((color1>>24) & 0xFF)) / 0xFF));
				glVertex2i(rectangle->x1, rectangle->y2+1);
				glVertex2i(rectangle->x1, rectangle->y1);
			}
			break;
		case gfx::FT_DIAGONAL: {
			unsigned int avg = get_average_color(color1, color2);
			if(!flip) {
				glColor4f(((GLfloat)((avg>>16) & 0xFF)) / 0xFF, ((GLfloat)((avg>>8) & 0xFF)) / 0xFF, ((GLfloat)(avg & 0xFF)) / 0xFF, 1.0f - (((GLfloat)((avg>>24) & 0xFF)) / 0xFF));
				glVertex2i(rectangle->x2 + 1, rectangle->y1);
				glColor4f(((GLfloat)((color1>>16) & 0xFF)) / 0xFF, ((GLfloat)((color1>>8) & 0xFF)) / 0xFF, ((GLfloat)(color1 & 0xFF)) / 0xFF, 1.0f - (((GLfloat)((color1>>24) & 0xFF)) / 0xFF));
				glVertex2i(rectangle->x1, rectangle->y1);
				glColor4f(((GLfloat)((avg>>16) & 0xFF)) / 0xFF, ((GLfloat)((avg>>8) & 0xFF)) / 0xFF, ((GLfloat)(avg & 0xFF)) / 0xFF, 1.0f - (((GLfloat)((avg>>24) & 0xFF)) / 0xFF));
				glVertex2i(rectangle->x1, rectangle->y2+1);
				glColor4f(((GLfloat)((color2>>16) & 0xFF)) / 0xFF, ((GLfloat)((color2>>8) & 0xFF)) / 0xFF, ((GLfloat)(color2 & 0xFF)) / 0xFF, 1.0f - (((GLfloat)((color2>>24) & 0xFF)) / 0xFF));
				glVertex2i(rectangle->x2 + 1, rectangle->y2+1);
			}
			else {
				glColor4f(((GLfloat)((color2>>16) & 0xFF)) / 0xFF, ((GLfloat)((color2>>8) & 0xFF)) / 0xFF, ((GLfloat)(color2 & 0xFF)) / 0xFF, 1.0f - (((GLfloat)((color2>>24) & 0xFF)) / 0xFF));
				glVertex2i(rectangle->x2 + 1, rectangle->y2+1);
				glColor4f(((GLfloat)((avg>>16) & 0xFF)) / 0xFF, ((GLfloat)((avg>>8) & 0xFF)) / 0xFF, ((GLfloat)(avg & 0xFF)) / 0xFF, 1.0f - (((GLfloat)((avg>>24) & 0xFF)) / 0xFF));
				glVertex2i(rectangle->x1, rectangle->y2+1);
				glColor4f(((GLfloat)((color1>>16) & 0xFF)) / 0xFF, ((GLfloat)((color1>>8) & 0xFF)) / 0xFF, ((GLfloat)(color1 & 0xFF)) / 0xFF, 1.0f - (((GLfloat)((color1>>24) & 0xFF)) / 0xFF));
				glVertex2i(rectangle->x1, rectangle->y1);
				glColor4f(((GLfloat)((avg>>16) & 0xFF)) / 0xFF, ((GLfloat)((avg>>8) & 0xFF)) / 0xFF, ((GLfloat)(avg & 0xFF)) / 0xFF, 1.0f - (((GLfloat)((avg>>24) & 0xFF)) / 0xFF));
				glVertex2i(rectangle->x2 + 1, rectangle->y1);
			}
			}
			break;
		default:
			break;
	}
	glEnd();
	glDisable(GL_BLEND);
}

unsigned int gfx::get_average_color(unsigned int color1, unsigned int color2) {
	unsigned int tmp = 0, ret = 0;
	tmp = ((color2>>24) & 0xFF) > ((color1>>24) & 0xFF)?
		((color1>>24) & 0xFF) + ((((color2>>24) & 0xFF) - ((color1>>24) & 0xFF)) / 2) :
		((color2>>24) & 0xFF) + ((((color1>>24) & 0xFF) - ((color2>>24) & 0xFF)) / 2);
	ret += tmp << 24;
	tmp = ((color2>>16) & 0xFF) > ((color1>>16) & 0xFF)?
		((color1>>16) & 0xFF) + ((((color2>>16) & 0xFF) - ((color1>>16) & 0xFF)) / 2) :
		((color2>>16) & 0xFF) + ((((color1>>16) & 0xFF) - ((color2>>16) & 0xFF)) / 2);
	ret += tmp << 16;
	tmp = ((color2>>8) & 0xFF) > ((color1>>8) & 0xFF)?
		((color1>>8) & 0xFF) + ((((color2>>8) & 0xFF) - ((color1>>8) & 0xFF)) / 2) :
		((color2>>8) & 0xFF) + ((((color1>>8) & 0xFF) - ((color2>>8) & 0xFF)) / 2);
	ret += tmp << 8;
	tmp = (color2 & 0xFF) > (color1 & 0xFF)?
		(color1 & 0xFF) + (((color2 & 0xFF) - (color1 & 0xFF)) / 2) :
		(color2 & 0xFF) + (((color1 & 0xFF) - (color2 & 0xFF)) / 2);
	ret += tmp;
	return ret;
}

/*! returns the sdl_color that we get from the function arguments and the screen surface
 *  @param red how much red (0 - 255)
 *  @param green how much green (0 - 255)
 *  @param blue how much blue (0 - 255)
 */
unsigned int gfx::get_color(unsigned int red, unsigned int green, unsigned int blue) {
	return (unsigned int)SDL_MapRGBA(screen->format, (Uint8)red, (Uint8)green, (Uint8)blue, 255);
}

/*! returns the sdl_color that we get from the function arguments and the screen surface - OBSOLETE?
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
	if(!flip) {
		glScissor(x1, screen->h - y2, x2 - x1, y2 - y1);
	}
	else {
		glScissor(x1, y1, x2 - x1, y2 - y1);
	}
}

//! ends/disables scissor test
void gfx::end_scissor() {
	glDisable(GL_SCISSOR_TEST);
}

void gfx::draw_bbox(core::aabbox* bbox, unsigned int color) {
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);

	glTranslatef(0.0f, 0.0f, 0.0f);
	glColor4f(((GLfloat)((color>>16) & 0xFF)) / 0xFF, ((GLfloat)((color>>8) & 0xFF)) / 0xFF, ((GLfloat)(color & 0xFF)) / 0xFF, 1.0f);
	glBegin(GL_LINE_STRIP);
		glVertex3f(bbox->vmax.x, bbox->vmax.y, bbox->vmax.z);
		glVertex3f(bbox->vmax.x, bbox->vmax.y, bbox->vmin.z);
		glVertex3f(bbox->vmax.x, bbox->vmin.y, bbox->vmin.z);
		glVertex3f(bbox->vmax.x, bbox->vmin.y, bbox->vmax.z);
		glVertex3f(bbox->vmax.x, bbox->vmax.y, bbox->vmax.z);
		glVertex3f(bbox->vmin.x, bbox->vmax.y, bbox->vmax.z);
		glVertex3f(bbox->vmin.x, bbox->vmax.y, bbox->vmin.z);
		glVertex3f(bbox->vmin.x, bbox->vmin.y, bbox->vmin.z);
		glVertex3f(bbox->vmin.x, bbox->vmin.y, bbox->vmax.z);
		glVertex3f(bbox->vmin.x, bbox->vmax.y, bbox->vmax.z);
	glEnd();
	glBegin(GL_LINES);
		glVertex3f(bbox->vmax.x, bbox->vmax.y, bbox->vmin.z);
		glVertex3f(bbox->vmin.x, bbox->vmax.y, bbox->vmin.z);

		glVertex3f(bbox->vmax.x, bbox->vmin.y, bbox->vmin.z);
		glVertex3f(bbox->vmin.x, bbox->vmin.y, bbox->vmin.z);

		glVertex3f(bbox->vmax.x, bbox->vmin.y, bbox->vmax.z);
		glVertex3f(bbox->vmin.x, bbox->vmin.y, bbox->vmax.z);
	glEnd();
}

void gfx::set_flip(bool state) {
	flip = state;
}

bool gfx::get_flip() {
	return flip;
}
