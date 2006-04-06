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

/*! there is no function currently
 */
gui_text::gui_text(engine* e) {
	is_notext = false;

	// reserve 32 bytes ...
	gui_text::text.reserve(32);

	gui_text::point = new core::pnt();

	// get classes
	gui_text::e = e;
	gui_text::c = e->get_core();
	gui_text::m = e->get_msg();
	gui_text::g = e->get_gfx();
}

/*! there is no function currently
 */
gui_text::~gui_text() {
	m->print(msg::MDEBUG, "gui_text.cpp", "freeing gui_text stuff");

	text.erase();

	delete gui_text::point;

	m->print(msg::MDEBUG, "gui_text.cpp", "gui_text stuff freed");
}

/*! draws the text
 *  @param x specifies how much the element is moved on the x axis
 *  @param y specifies how much the element is moved on the y axis
 */
void gui_text::draw(unsigned int x, unsigned int y) {
	gui_text::draw(gui_text::text.c_str(), x, y);
}

/*! draws the text, that is specified by the parameter text
 *  @param text the text that should be drawn
 *  @param x specifies how much the element is moved on the x axis
 *  @param y specifies how much the element is moved on the y axis
 */
void gui_text::draw(const char* text, unsigned int x, unsigned int y) {
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glPushMatrix();

	// i dunno why, but we have to substract 10 from our y coord (maybe the title bar ...)
	glTranslatef((GLfloat)(gui_text::point->x + x),
		(GLfloat)(gui_text::e->get_screen()->h - gui_text::point->y - 10.0f - y),
		0.0f);
	glColor3f((GLfloat)((gui_text::font->color & 0xFF0000) >> 16) / 255,
		(GLfloat)((gui_text::font->color & 0xFF00) >> 8) / 255,
		(GLfloat)(gui_text::font->color & 0xFF) / 255);
	gui_text::font->ttf_font->Render(text);

	glPopMatrix();
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
}

//! returns the text id
unsigned int gui_text::get_id() {
	return gui_text::id;
}

//! returns the text starting point
core::pnt* gui_text::get_point() {
	return gui_text::point;
}

//! returns the text
const char* gui_text::get_text() {
	return gui_text::text.c_str();
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
void gui_text::set_point(core::pnt* point) {
	memcpy(gui_text::point, point, 8);
}

/*! sets the text
 *  @param text the text we want to set
 */
void gui_text::set_text(char* text) {
	is_notext = false;
	gui_text::text = text;
}

/*! sets the init state
 *  @param state the state of the init bool
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

/*! returns the text's width
 */
unsigned int gui_text::get_text_width() {
	float width = font->ttf_font->Advance(text.c_str());
	return (unsigned int)width;
}

/*! returns the text's height
 */
unsigned int gui_text::get_text_height() {
	float x, y, z, ux, uy, uz;
	font->ttf_font->BBox(text.c_str(), x, y, z, ux, uy, uz);
	return (unsigned int)(uy - y);
}

/*! sets the font object of our text
 *  @param font the font object
 */
void gui_text::set_font(gui_font::font* font) {
	gui_text::font = font;
}

/*! returns the font object
 */
gui_font::font* gui_text::get_font() {
	return gui_text::font;
}
