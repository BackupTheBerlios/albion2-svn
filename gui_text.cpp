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

#include "gui_text.h"
#include <FTGLTextureFont.h>
#include "gfx.h"
#include "msg.h"
#include "core.h"
#include "engine.h"

/*! there is no function currently
 */
gui_text::gui_text() {
	is_notext = false;

	// 1024 chars
	gui_text::text = (char*)malloc(1024);
}

/*! there is no function currently
 */
gui_text::~gui_text() {
	free(text);
}

/*! creates a new FTFont element and sets it as the currently used font
 *  @param font_name the name of the .ttf file
 *  @param font_size the font size in pixel
 */
void gui_text::new_text(char* font_name, unsigned int font_size) {
	font = new FTGLTextureFont(font_name);
	font->FaceSize(font_size);
}

/*! draws the text
 */
void gui_text::draw_text() {
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glPushMatrix();

	// i dunno why, but we have to substract 10 from our y coord (maybe the title bar ...)
	glTranslatef((GLfloat)(point->x), (GLfloat)(engine_handler->get_screen()->h - point->y - 10.0f), 0.0f);
	glColor3f((GLfloat)(color.r / 255), (GLfloat)(color.g / 255), (GLfloat)(color.b / 255));
	font->Render(text);

	glPopMatrix();
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
}

/*! creates a engine_handler -> a pointer to the engine class
 *  @param iengine the engine we want to handle
 */
void gui_text::set_engine_handler(engine* iengine) {
	gui_text::engine_handler = iengine;
}

//! returns the text id
unsigned int gui_text::get_id() {
	return gui_text::id;
}

//! returns the text starting point
gfx::pnt* gui_text::get_point() {
	return gui_text::point;
}

//! returns the text
char* gui_text::get_text() {
	return gui_text::text;
}

//! returns the text color
SDL_Color gui_text::get_color() {
	return gui_text::color;
}

//! returns the text
char* gui_text::get_font_name() {
	return gui_text::font_name;
}

//! returns the text
unsigned int gui_text::get_font_size() {
	return gui_text::font_size;
}

/*! sets the text id
 *  @param id the id we want to set
 */
void gui_text::set_id(unsigned int id) {
	gui_text::id = id;
}

/*! sets the text starting point
 *  @param point the starting point we want to set
 */
void gui_text::set_point(gfx::pnt* point) {
	gui_text::point = point;
}

/*! sets the text
 *  @param text the text we want to set
 */
void gui_text::set_text(char* text) {
	is_notext = false;
	gui_text::text = text;
}

/*! sets the text color
 *  @param color the text color we want to set
 */
void gui_text::set_color(SDL_Color color) {
	gui_text::color = color;
}

/*! sets the font name
 *  @param font_name the font name we want to set
 */
void gui_text::set_font_name(char* font_name) {
	gui_text::font_name = font_name;
	delete font;
	font = new FTGLTextureFont(font_name);
}

/*! sets the font size
 *  @param font_size the font size we want to set
 */
void gui_text::set_font_size(unsigned int font_size) {
	gui_text::font_size = font_size;
	font->FaceSize(font_size);
}

/*! sets the init state
 *  @param font_size the font size we want to set
 */
void gui_text::set_init(bool state) {
	gui_text::is_init = state;
}

/*! sets the notext bool to true and the text to ""
 */
void gui_text::set_notext() {
	gui_text::is_notext = true;
	gui_text::text = "";
}

/*! returns the pointer to the currently used FTFont element 
 */
FTFont* gui_text::get_font() {
	return gui_text::font;
}

/*! returns the text's width
 */
unsigned int gui_text::get_text_width() {
	float wide = font->Advance(text);
	return (unsigned int)wide;
}

/*! returns the text's height 
 */
unsigned int gui_text::get_text_height() {
	float x, y, z, ux, uy, uz;
	font->BBox(text, x, y, z, ux, uy, uz);
	return (unsigned int)(uy - y);
}
