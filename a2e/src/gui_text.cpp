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
	// reserve 32 bytes ...
	gui_text::text.reserve(32);

	gui_text::point = new core::pnt();

	gui_text::redraw = false;

	gui_text::background_color = 0xFF000000;

	gui_text::advance = 0;
	gui_text::last_advance = 0;

	gui_text::type = "text";

	gui_text::tab = 0;

	gui_text::font_height = 0;

	// get classes
	gui_text::e = e;
	gui_text::c = e->get_core();
	gui_text::m = e->get_msg();
	gui_text::g = e->get_gfx();
	gui_text::exts = e->get_ext();
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
void gui_text::draw(unsigned int x, unsigned int y, bool draw_bg) {
	gui_text::draw(gui_text::text.c_str(), x, y, draw_bg);
}

/*! draws the text, that is specified by the parameter text
 *  @param text the text that should be drawn
 *  @param x specifies how much the element is moved on the x axis
 *  @param y specifies how much the element is moved on the y axis
 */
void gui_text::draw(const char* text, unsigned int x, unsigned int y, bool draw_bg) {
	if(draw_bg) {
		glPushMatrix();
		glTranslatef(0.0f, 0.0f, 0.0f);
		glColor4f(((GLfloat)((background_color>>16) & 0xFF)) / 0xFF,
			((GLfloat)((background_color>>8) & 0xFF)) / 0xFF,
			((GLfloat)(background_color & 0xFF)) / 0xFF,
			1.0f - (((GLfloat)((background_color>>24) & 0xFF)) / 0xFF));
		advance = (unsigned int)gui_text::font->ttf_font->Advance(text);
		height = get_text_height()+2;
		glBegin(GL_QUADS);
			if(advance < last_advance || height < last_height) {
				glVertex2i(gui_text::point->x + x, gui_text::point->y + y - 1);
				glVertex2i(gui_text::point->x + x + last_advance, gui_text::point->y + y - 1);
				glVertex2i(gui_text::point->x + x + last_advance, gui_text::point->y + y + last_height - 1);
				glVertex2i(gui_text::point->x + x, gui_text::point->y + y + last_height - 1);
			}
			else {
				glVertex2i(gui_text::point->x + x, gui_text::point->y + y - 1);
				glVertex2i(gui_text::point->x + x + advance, gui_text::point->y + y - 1);
				glVertex2i(gui_text::point->x + x + advance, gui_text::point->y + y + height - 1);
				glVertex2i(gui_text::point->x + x, gui_text::point->y + y + height - 1);
			}
		glEnd();
		glPopMatrix();
		last_advance = advance;
		last_height = height;
	}

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	// really nice blend function, hehe :> (create a fallback for < opengl 1.4?)
	if(exts->is_blend_func_separate_support()) exts->glBlendFuncSeparateEXT(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE_MINUS_DST_ALPHA, GL_ONE);
	glPushMatrix();

	// i dunno why, but we have to substract 10 from our y coord (maybe the title bar ...)
	glTranslatef((GLfloat)(gui_text::point->x + x),
		(GLfloat)(gui_text::point->y + y + gui_text::font->font_size + gui_text::font->ttf_font->Descender()),
		0.0f);
	glColor4f((GLfloat)((color & 0xFF0000) >> 16) / 255,
		(GLfloat)((color & 0xFF00) >> 8) / 255,
		(GLfloat)(color & 0xFF) / 255, 1.0f);
	gui_text::font->ttf_font->Render(text);

	glPopMatrix();
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
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
	if(strcmp(gui_text::text.c_str(), text) != 0) {
		gui_text::text = text;
		gui_text::redraw = true;
	}
}

/*! sets the init state
 *  @param state the state of the init bool
 */
void gui_text::set_init(bool state) {
	gui_text::is_init = state;
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
	/*float x, y, z, ux, uy, uz;
	font->ttf_font->BBox(text.c_str(), x, y, z, ux, uy, uz);
	return (unsigned int)(uy - y);*/
	return font_height;
}

/*! sets the font object of our text
 *  @param font the font object
 */
void gui_text::set_font(gui_font::font* font) {
	gui_text::font = font;
	font_height = gui_text::font->font_size;
}

/*! returns the font object
 */
gui_font::font* gui_text::get_font() {
	return gui_text::font;
}

/*! returns the fonts color
 */
unsigned int gui_text::get_color() {
	return gui_text::color;
}

/*! sets the fonts color
 */
void gui_text::set_color(unsigned int color) {
	gui_text::color = color;
}

/*! sets the redraw flag of the text (so the belonging window content will be rendered again at next gui draw)
 *  @param state the state of the flag
 */
void gui_text::set_redraw(bool state) {
	gui_text::redraw = state;
}

//! returns true if the text/window has to be redrawn
bool gui_text::get_redraw() {
	return gui_text::redraw;
}

/*! sets the texts background color
 *  @param color the background color to set
 */
void gui_text::set_background_color(unsigned int color) {
	gui_text::background_color = color;
}

void gui_text::set_type(const char* type) {
	gui_text::type = type;
}

string* gui_text::get_type() {
	return &(gui_text::type);
}

/*! sets the texts tab
 *  @param id the tab id we want to set
 */
void gui_text::set_tab(unsigned int id) {
	if(gui_text::tab != id) gui_text::redraw = true;
	gui_text::tab = id;
}

//! returns the objects tab
unsigned int gui_text::get_tab() {
	return gui_text::tab;
}
