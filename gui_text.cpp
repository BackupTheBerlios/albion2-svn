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
	is_notext = false;

	// 1024 chars
	gui_text::text = (char*)malloc(1024);
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
	glPushAttrib(GL_ENABLE_BIT);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glEnable(GL_TEXTURE_2D);

	// This allows alpha blending of 2D textures with the scene
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glViewport(0, 0, engine_handler->get_screen()->w, engine_handler->get_screen()->h);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	glOrtho(0.0, (GLdouble)engine_handler->get_screen()->w,
		(GLdouble)engine_handler->get_screen()->h, 0.0, 0.0, 1.0);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);




	glBindTexture(GL_TEXTURE_2D, gui_text::texture);
	glBegin(GL_TRIANGLE_STRIP);

	glTexCoord2f(0, 0);
	glVertex2i(gui_text::point->x,   gui_text::point->y  );
	glTexCoord2f(gui_text::texmaxx, 0);
	glVertex2i(gui_text::point->x + gui_text::surface->w, gui_text::point->y  );
	glTexCoord2f(0, gui_text::texmaxy);
	glVertex2i(gui_text::point->x,   gui_text::point->y + gui_text::surface->h);
	glTexCoord2f(gui_text::texmaxx, gui_text::texmaxy);
	glVertex2i(gui_text::point->x + gui_text::surface->w, gui_text::point->y + gui_text::surface->h);

	glEnd();




	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	glPopAttrib();
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

/*! sets the text surface
 *  @param surface the text surface we want to set
 */
void gui_text::set_surface(SDL_Surface* surface) {
	gui_text::surface = surface;
	// ogl stuff
	gui_text::texture = gui_text::SDL_GL_LoadTexture(gui_text::surface);
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
	gui_text::remake_text();
}

/*! sets the text color
 *  @param color the text color we want to set
 */
void gui_text::set_color(SDL_Color color) {
	gui_text::color = color;
	gui_text::remake_text();
}

/*! sets the font name
 *  @param font_name the font name we want to set
 */
void gui_text::set_font_name(char* font_name) {
	gui_text::font_name = font_name;
	gui_text::remake_text();
}

/*! sets the font size
 *  @param font_size the font size we want to set
 */
void gui_text::set_font_size(unsigned int font_size) {
	gui_text::font_size = font_size;
	gui_text::remake_text();
}

/*! sets the init state
 *  @param font_size the font size we want to set
 */
void gui_text::set_init(bool state) {
	gui_text::is_init = state;
}

/*! remakes the text surface with the new parameters
 */
void gui_text::remake_text() {
	// checks if everything was initialized
	if(is_init) {
		if(!is_notext) {
			TTF_Font* font = gui_text::open_font(gui_text::font_name, gui_text::font_size);
			SDL_FreeSurface(gui_text::get_surface());
			gui_text::set_surface(TTF_RenderText_Blended(font, gui_text::text, gui_text::color));
			gui_text::close_font(font);

			// ogl stuff
			gui_text::texture = gui_text::SDL_GL_LoadTexture(gui_text::get_surface());
		}
		else {
			TTF_Font* font = gui_text::open_font(gui_text::font_name, gui_text::font_size);
			SDL_FreeSurface(gui_text::get_surface());
			gui_text::set_surface(TTF_RenderText_Blended(font, "  ", gui_text::color));
			gui_text::close_font(font);

			// ogl stuff
			gui_text::texture = gui_text::SDL_GL_LoadTexture(gui_text::get_surface());
		}
	}
}

/*! sets the notext bool to true and the text to ""
 */
void gui_text::set_notext() {
	gui_text::is_notext = true;
	gui_text::text = "";
	gui_text::remake_text();
}

/*! quick utility function for texture creation taken from the SDL_ttf source
 *  @param input the input size
 */
int gui_text::power_of_two(int input) {
	int value = 1;

	while ( value < input ) {
		value <<= 1;
	}
	return value;
}

/*! function taken from the SDL_ttf source for creating an
 *! ogl texture out of a SDL Surface
 *  @param surface the surface from which we want to create an ogl texture
 */
GLuint gui_text::SDL_GL_LoadTexture(SDL_Surface *surface) {
	GLuint texture;
	int w, h;
	SDL_Surface *image;
	SDL_Rect area;
	Uint32 saved_flags;
	Uint8  saved_alpha;

	/* Use the surface width and height expanded to powers of 2 */
	w = power_of_two(surface->w);
	h = power_of_two(surface->h);
	//texcoord[0] = 0.0f;			/* Min X */
	//texcoord[1] = 0.0f;			/* Min Y */
	gui_text::texmaxx = (GLfloat)surface->w / w;	/* Max X */
	gui_text::texmaxy = (GLfloat)surface->h / h;	/* Max Y */

	image = SDL_CreateRGBSurface(
			SDL_SWSURFACE,
			w, h,
			32,
			0x00FF0000, 
			0x0000FF00, 
			0x000000FF,
			0xFF000000);
	if ( image == NULL ) {
		return 0;
	}

	/* Save the alpha blending attributes */
	saved_flags = surface->flags&(SDL_SRCALPHA|SDL_RLEACCELOK);
	saved_alpha = surface->format->alpha;
	if ( (saved_flags & SDL_SRCALPHA) == SDL_SRCALPHA ) {
		SDL_SetAlpha(surface, 0, 0);
	}

	/* Copy the surface into the GL texture image */
	area.x = 0;
	area.y = 0;
	area.w = surface->w;
	area.h = surface->h;
	SDL_BlitSurface(surface, &area, image, &area);

	/* Restore the alpha blending attributes */
	if ( (saved_flags & SDL_SRCALPHA) == SDL_SRCALPHA ) {
		SDL_SetAlpha(surface, saved_flags, saved_alpha);
	}

	/* Create an OpenGL texture for the image */
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D,
		     0,
		     GL_RGBA,
		     w, h,
		     0,
		     GL_RGBA,
		     GL_UNSIGNED_BYTE,
		     image->pixels);
	SDL_FreeSurface(image); /* No longer needed */

	return texture;
}
