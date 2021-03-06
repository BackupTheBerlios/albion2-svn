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

#include "gui_object.h"

/*! there is no function currently
 */
gui_object::gui_object(engine* e, gui_style* gs) {
	// get classes
	gui_object::e = e;
	gui_object::c = e->get_core();
	gui_object::m = e->get_msg();
	gui_object::g = e->get_gfx();
	gui_object::gs = gs;
	gui_object::text_handler = NULL;
	gui_object::img = NULL;
	gui_object::redraw = false;
	gui_object::scissor = true;
	gui_object::tab = 0;

	gui_object::state = "NORMAL";
}

/*! there is no function currently
 */
gui_object::~gui_object() {
	m->print(msg::MDEBUG, "gui_object.cpp", "freeing gui_object stuff");

	if(gui_object::img != NULL) {
		delete gui_object::img;
	}
	
	delete gui_object::rectangle;
	
	text.clear();

	m->print(msg::MDEBUG, "gui_object.cpp", "gui_object stuff freed");
}

/*! draws the object - to be overwritten from other classes
 *  @param x specifies how much the element is moved on the x axis
 *  @param y specifies how much the element is moved on the y axis
 */
void gui_object::draw(int x, int y) {
	draw_object(x, y);
}

/*! draws the object
 *  @param x specifies how much the element is moved on the x axis
 *  @param y specifies how much the element is moved on the y axis
 */
void gui_object::draw_object(int x, int y) {
	gs->set_gui_text(text_handler);
	gs->set_image(img);
	if(scissor) {
		g->set_scissor(x + rectangle->x1, y + rectangle->y1, x + rectangle->x2+1, y + rectangle->y2+1);
		g->begin_scissor();
	}
	gs->render_gui_element(type.c_str(), state.c_str(), rectangle, x, y);
	if(scissor) g->end_scissor();
}

//! returns the objects id
unsigned int gui_object::get_id() {
	return gui_object::id;
}

//! returns the objects rectangle
gfx::rect* gui_object::get_rectangle() {
	return gui_object::rectangle;
}

//! returns the objects state
string gui_object::get_state() {
	return state;
}

/*! sets the objects id
 *  @param id the id we want to set
 */
void gui_object::set_id(unsigned int id) {
	gui_object::id = id;
}

/*! sets the objects rectangle
 *  @param rectangle the rectangle we want to set
 */
void gui_object::set_rectangle(gfx::rect* rectangle) {
	gui_object::rectangle = rectangle;
}

/*! sets the objects state
 *  @param state the current state of the object
 */
void gui_object::set_state(const char* state) {
	gui_object::state = state;
}

void gui_object::set_text_handler(gui_text* text) {
	gui_object::text_handler = text;
}

gui_text* gui_object::get_text_handler() {
	return gui_object::text_handler;
}

void gui_object::set_text(const char* text) {
	if(strcmp(gui_object::text.c_str(), text) != 0) redraw = true;
	gui_object::text = text;
	gui_object::text_handler->set_text((char*)text);
}

string* gui_object::get_text() {
	return &(gui_object::text);
}

void gui_object::set_image(image* img) {
	// delete old image if there is one
	if(gui_object::img != NULL) {
		delete gui_object::img;
	}
	gui_object::img = img;

	gui_object::redraw = true;
}

image* gui_object::get_image() {
	return gui_object::img;
}

void gui_object::set_image_texture(unsigned int tex) {
	if(gui_object::img != NULL) {
		gui_object::img->set_texture(tex);
		gui_object::redraw = true;
	}
	else {
		m->print(msg::MERROR, "gui_object.cpp", "set_image_texture(): unable to set the texture of an image that doesn't exist!");
	}
}

unsigned int gui_object::get_image_texture() {
	return gui_object::img->get_texture();
}

void gui_object::set_type(const char* type) {
	gui_object::type = type;
}

string* gui_object::get_type() {
	return &(gui_object::type);
}

/*! sets the redraw flag of the object (so the belonging window content will be rendered again at next gui draw)
 *  @param state the state of the flag
 */
void gui_object::set_redraw(bool state) {
	gui_object::redraw = state;
}

//! returns true if the object/window has to be redrawn
bool gui_object::get_redraw() {
	return gui_object::redraw;
}

void gui_object::set_scissor(bool state) {
	gui_object::scissor = state;
}

bool gui_object::get_scissor() {
	return gui_object::scissor;
}

/*! sets the objects tab
 *  @param id the tab id we want to set
 */
void gui_object::set_tab(unsigned int id) {
	if(gui_object::tab != id) gui_object::redraw = true;
	gui_object::tab = id;
}

//! returns the objects tab
unsigned int gui_object::get_tab() {
	return gui_object::tab;
}
