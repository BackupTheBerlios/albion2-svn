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
#include "gfx.h"
#include "msg.h"
#include "core.h"
#include "engine.h"

/*! there is no function currently
 */
gui_text::gui_text() {
}

/*! there is no function currently
 */
gui_text::~gui_text() {
}

/*! opens a font and returns a pointer to the font
 *  @param font_name the fonts name
 *  @param font_size the size of the font (in px?)
 */
TTF_Font* gui_text::open_font(char* font_name, unsigned int font_size) {
	TTF_Font *font;
	font = TTF_OpenFont(font_name, font_size);
	if(!font) {
		m.print(msg::MERROR, "gui.cpp", "TTF_OpenFont: %s", TTF_GetError());
	}

	TTF_SetFontStyle(font, TTF_STYLE_NORMAL);
	return font;
}

/*! closes a font
 */
void gui_text::close_font(TTF_Font* font) {
	TTF_CloseFont(font);
	font = NULL;
}

/*! draws the text
 */
void gui_text::draw_text() {
	SDL_Rect* srcrect = (SDL_Rect*)malloc(sizeof(SDL_Rect));
	SDL_Rect* dstrect = (SDL_Rect*)malloc(sizeof(SDL_Rect));

	srcrect->x = 0;
	srcrect->y = 0;
	srcrect->w = gui_text::surface->w;
	srcrect->h = gui_text::surface->h;

	dstrect->x = gui_text::point->x;
	dstrect->y = gui_text::point->y;
	dstrect->w = gui_text::surface->w;
	dstrect->h = gui_text::surface->h;

	SDL_BlitSurface(gui_text::surface, srcrect, gui_text::engine_handler->get_screen(), dstrect);
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

//! returns the text surface
SDL_Surface* gui_text::get_surface() {
	return gui_text::surface;
}

//! returns the text starting point
gfx::pnt* gui_text::get_point() {
	return gui_text::point;
}

/*! sets the text id
 *  @param id the id we want to set
 */
void gui_text::set_id(unsigned int id) {
	gui_text::id = id;
}

/*! sets the text surface
 *  @param surface the text surface we want to set
 */
void gui_text::set_surface(SDL_Surface* surface) {
	gui_text::surface = surface;
}

/*! sets the text starting point
 *  @param point the starting point we want to set
 */
void gui_text::set_point(gfx::pnt* point) {
	gui_text::point = point;
}
