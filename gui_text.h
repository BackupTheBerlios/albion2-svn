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
 
#ifndef __GUI_TEXT_H__
#define __GUI_TEXT_H__

#include <iostream>
#include <SDL.h>
#include <SDL_ttf.h>
#include <FTGLTextureFont.h>
#include "msg.h"
#include "core.h"
#include "gfx.h"
#include "event.h"
#include "engine.h"
using namespace std;

#include "win_dll_export.h"

/*! @class gui_text
 *  @brief gui text element functions
 *  @author flo
 *  @version 0.3
 *  @date 2004/12/13
 *  @todo more functions
 *  
 *  the gui_text class
 */

class A2E_API gui_text
{
public:
	gui_text();
	~gui_text();

	void draw_text();
	void draw_text(char* text);
	void set_engine_handler(engine* iengine);
	void new_text(char* font_name, unsigned int font_size);


	// gui text element variables functions

	void remake_text();

	unsigned int get_id();
	gfx::pnt* get_point();
	char* get_text();
	SDL_Color get_color();
	char* get_font_name();
	unsigned int get_font_size();
	FTFont* get_font();

	void set_id(unsigned int id);
	void set_point(gfx::pnt* point);
	void set_text(char* text);
	void set_color(SDL_Color color);
	void set_font_name(char* font_name);
	void set_font_size(unsigned int font_size);
	void set_init(bool state);
	void set_font(FTFont* font);

	void set_notext();
	unsigned int get_text_width();
	unsigned int get_text_height();

protected:
	msg m;
	core c;
	gfx g;

	engine* engine_handler;

	// gui text element variables

	//! text id
	unsigned int id;
	//! text starting point (x,y)
	gfx::pnt* point;
	//! the text itself
	char* text;
	//! the color if the text
	SDL_Color color;
	//! the font name
	char* font_name;
	//! the font size
	unsigned int font_size;
	//! bool if everything was initialized
	bool is_init;

	//! bool if there is text in the input box
	bool is_notext;

	GLuint texture;
	float texmaxx;
	float texmaxy;

	FTFont* font;

};

#endif
