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

#include "image.h"

/*! there is no function currently
 *  @param e pointer to the engine class
 */
image::image(engine* e) {
	image::position = new core::pnt();
	image::position->x = 0;
	image::position->y = 0;

	image::width = 0;
	image::heigth = 0;

	alpha = false;

	scale = true;

	gui_img = false;

	color = 0xFFFFFF;

	// get classes
	image::e = e;
	image::m = e->get_msg();
	image::t = e->get_texman();
}

/*! there is no function currently
 */
image::~image() {
	m->print(msg::MDEBUG, "image.cpp", "freeing image stuff");

	delete image::position;

	m->print(msg::MDEBUG, "image.cpp", "image stuff freed");
}

/*! draws the image
 */
void image::draw(unsigned int scale_x, unsigned int scale_y) {
	if(!gui_img) image::e->start_2d_draw();

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, image::texture);
	glTranslatef(0.0f, 0.0f, 0.0f);

	glColor3f(((GLfloat)((color>>16) & 0xFF)) / 0xFF, ((GLfloat)((color>>8) & 0xFF)) / 0xFF, ((GLfloat)(color & 0xFF)) / 0xFF);
	if(alpha) { glEnable(GL_BLEND); }

	if(!gui_img) {
		if(scale) {
			glBegin(GL_QUADS);
				glTexCoord2f(0.0f, 0.0f);
				glVertex2i(image::position->x, image::position->y);
				glTexCoord2f(0.0f, 1.0f);
				glVertex2i(image::position->x, image::position->y + scale_y);
				glTexCoord2f(1.0f, 1.0f);
				glVertex2i(image::position->x + scale_x, image::position->y + scale_y);
				glTexCoord2f(1.0f, 0.0f);
				glVertex2i(image::position->x + scale_x, image::position->y);
			glEnd();
		}
		else {
			glBegin(GL_QUADS);
				glTexCoord2f(0.0f, 0.0f);
				glVertex2i(image::position->x, image::position->y);
				glTexCoord2f(0.0f, 1.0f);
				glVertex2i(image::position->x, image::position->y + image::heigth);
				glTexCoord2f(1.0f, 1.0f);
				glVertex2i(image::position->x + image::width, image::position->y + image::heigth);
				glTexCoord2f(1.0f, 0.0f);
				glVertex2i(image::position->x + image::width, image::position->y);
			glEnd();
		}
	}
	else {
		if(scale) {
			glBegin(GL_QUADS);
				glTexCoord2f(0.0f, 0.0f);
				glVertex2i(image::position->x, image::position->y);
				glTexCoord2f(1.0f, 0.0f);
				glVertex2i(image::position->x + scale_x, image::position->y);
				glTexCoord2f(1.0f, 1.0f);
				glVertex2i(image::position->x + scale_x, image::position->y + scale_y);
				glTexCoord2f(0.0f, 1.0f);
				glVertex2i(image::position->x, image::position->y + scale_y);
			glEnd();
		}
		else {
			glBegin(GL_QUADS);
				glTexCoord2f(0.0f, 0.0f);
				glVertex2i(image::position->x, image::position->y);
				glTexCoord2f(1.0f, 0.0f);
				glVertex2i(image::position->x + image::width, image::position->y);
				glTexCoord2f(1.0f, 1.0f);
				glVertex2i(image::position->x + image::width, image::position->y + image::heigth);
				glTexCoord2f(0.0f, 1.0f);
				glVertex2i(image::position->x, image::position->y + image::heigth);
			glEnd();
		}
	}

	if(alpha) { glDisable(GL_BLEND); }
	glDisable(GL_TEXTURE_2D);

	// if we want to draw 3d stuff later on, we have to clear
	// the depth buffer, otherwise nothing will be seen
	glClear(GL_DEPTH_BUFFER_BIT);

	if(!gui_img) image::e->stop_2d_draw();
}

/*! draws the image
 */
void image::draw() {
	image::draw(image::width, image::heigth);
}

/*! opens an image file
 *  @param filename the image files name
 *  @param components how many color components should be used (in bytes)
 *  @param format the images format (GL_RGB, GL_RGBA, etc. ...)
 */
void image::open_image(char* filename, GLint components, GLenum format) {
	if(components == 4) { alpha = true; }
	texture = t->add_texture(filename, components, format);
	image::width = t->get_texture(texture)->width;
	image::heigth = t->get_texture(texture)->height;
}

/*! sets the position (2 * unsigned int) of the image
 *  @param x the (new) x position of the image
 *  @param y the (new) y position of the image
 */
void image::set_position(unsigned int x, unsigned int y) {
	image::position->x = x;
	image::position->y = y;
}

/*! sets the position (pnt) of the image
 *  @param position the (new) position of the image
 */
void image::set_position(core::pnt* position) {
	image::set_position(position->x, position->y);
}

/*! returns the position (pnt) of the image
 */
core::pnt* image::get_position() {
	return image::position;
}

/*! sets the images texture
 *  @param tex the texture we want to set
 */
void image::set_texture(GLuint tex) {
	texture = tex;
	image::width = t->get_texture(texture)->width;
	image::heigth = t->get_texture(texture)->height;
	t->get_texture(texture)->components == 4 ? alpha = true : alpha = false;
}

void image::set_texture(GLuint tex, unsigned int width, unsigned int height, bool alpha) {
	texture = tex;
	image::width = width;
	image::heigth = height;
	image::alpha = alpha;
}

//! returns the images texture
GLuint image::get_texture() {
	return image::texture;
}

/*! sets image scaling to state
 *  @param state the scaling state
 */
void image::set_scaling(bool state) {
	image::scale = state;
}

//! returns the image scale flag
bool image::get_scaling() {
	return image::scale;
}

unsigned int image::get_width() {
	return image::width;
}

unsigned int image::get_height() {
	return image::heigth;
}

void image::set_color(unsigned int color) {
	image::color = color;
}

void image::set_gui_img(bool state) {
	gui_img = state;
}
