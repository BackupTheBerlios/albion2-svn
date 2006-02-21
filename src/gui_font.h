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

#ifndef __GUI_FONT_H__
#define __GUI_FONT_H__

#include <iostream>
#include <string>
#include <list>
#include <SDL/SDL.h>
#include <FTGL/FTGLTextureFont.h>
#include "msg.h"
using namespace std;

#include "win_dll_export.h"

/*! @class gui_font
 *  @brief gui font functions
 *  @author flo
 *  @todo more functions
 *
 *  the gui_font class
 */

class A2E_API gui_font
{
public:
	gui_font(msg* m);
	~gui_font();

	struct font {
		//! the font object
		FTFont* ttf_font;
		//! the fonts color
		unsigned int color;
		//! the font name
		string font_name;
		//! the font size
		unsigned int font_size;
	};

	font* add_font(char* filename, unsigned int size, unsigned int color);


protected:
	msg* m;

	list<font> fonts;


	GLuint texture;
	float texmaxx;
	float texmaxy;


};

#endif
