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
#include <string>
#include <list>
#include <SDL/SDL.h>
#include <FTGL/FTGLTextureFont.h>
#include "msg.h"
#include "core.h"
#include "gfx.h"
#include "engine.h"
#include "gui_font.h"
using namespace std;

#include "win_dll_export.h"

/*! @class gui_text
 *  @brief gui text element functions
 *  @author flo
 *  @todo more functions
 *
 *  the gui_text class
 */

class A2E_API gui_text
{
public:
	gui_text(engine* e);
	~gui_text();

	void draw(unsigned int x, unsigned int y);
	void draw(const char* text, unsigned int x, unsigned int y);
	void new_text(char* font_name, unsigned int font_size);


	// gui text element variables functions

	void remake_text();

	gui_font::font* get_font();
	unsigned int get_id();
	core::pnt* get_point();
	const char* get_text();

	void set_font(gui_font::font* font);
	void set_id(unsigned int id);
	void set_point(core::pnt* point);
	void set_text(char* text);
	void set_init(bool state);

	void set_notext();
	unsigned int get_text_width();
	unsigned int get_text_height();

protected:
	msg* m;
	core* c;
	engine* e;
	gfx* g;

	// gui text element variables

	//! pointer to a font
	gui_font::font* font;
	//! text id
	unsigned int id;
	//! text starting point (x,y)
	core::pnt* point;
	//! the text itself
	string text;
	//! bool if everything was initialized
	bool is_init;

	GLuint texture;
	float texmaxx;
	float texmaxy;


};

#endif
