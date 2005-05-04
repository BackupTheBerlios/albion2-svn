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
#include "gfx.h"
#include "msg.h"

/*! there is no function currently
 */
image::image(engine* iengine) {
	image::engine_handler = iengine;

	image::position = new core::pnt();
	image::position->x = 0;
	image::position->y = 0;

	image::width = 0;
	image::heigth = 0;
}

/*! there is no function currently
 */
image::~image() {
	m.print(msg::MDEBUG, "image.cpp", "freeing image stuff");

	delete image::position;

	m.print(msg::MDEBUG, "image.cpp", "image stuff freed");
}

/*! draws the image
 */
void image::draw(unsigned int scale_x, unsigned int scale_y) {
	image::engine_handler->start_2d_draw();
	unsigned int screen_heigth = image::engine_handler->get_screen()->h;

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, image::texture);
	glTranslatef(0.0f, 0.0f, 0.0f);

	glColor3f(1.0f, 1.0f, 1.0f);
	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f);
		glVertex2i(image::position->x, screen_heigth - image::position->y);
		glTexCoord2f(0.0f, 1.0f);
		glVertex2i(image::position->x, screen_heigth - (image::position->y + scale_y));
		glTexCoord2f(1.0f, 1.0f);
		glVertex2i(image::position->x + scale_x, screen_heigth - (image::position->y + scale_y));
		glTexCoord2f(1.0f, 0.0f);
		glVertex2i(image::position->x + scale_x, screen_heigth - image::position->y);
	glEnd();

	// if we want to draw 3d stuff later on, we have to clear
	// the depth buffer, otherwise nothing will be seen
	glClear(GL_DEPTH_BUFFER_BIT);

	image::engine_handler->stop_2d_draw();
}

/*! draws the image
 */
void image::draw() {
	image::draw(image::width, image::heigth);
}

/*! opens an image file
 *  @param filename the image files name
 */
void image::open_image(char* filename) {
	if(image::texture) { glDeleteTextures(1, &(image::texture)); }

	SDL_Surface* img_srf = IMG_LoadPNG_RW(SDL_RWFromFile(filename, "rb"));
	if(!img_srf) {
		m.print(msg::MERROR, "image.cpp", "error loading image file \"%s\"!", filename);
		return;
	}

	image::width = img_srf->w;
	image::heigth = img_srf->h;

	glGenTextures(1, &(image::texture));
	glBindTexture(GL_TEXTURE_2D, image::texture);	

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);

	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, img_srf->w, img_srf->h,
		GL_RGB, GL_UNSIGNED_BYTE, img_srf->pixels);

	SDL_FreeSurface(img_srf);
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

/*! creates a engine_handler -> a pointer to the engine class
 *  @param iengine the engine we want to handle
 */
void image::set_engine_handler(engine* iengine) {
	image::engine_handler = iengine;
}
