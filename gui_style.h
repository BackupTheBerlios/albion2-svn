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
 
#ifndef __GUI_STYLE_H__
#define __GUI_STYLE_H__

#include <iostream>
#include <SDL.h>
#include <SDL_ttf.h>
#include "msg.h"
#include "gfx.h"
using namespace std;

#include "win_dll_export.h"

/*! @class gui_style
 *  @brief gui style stuff
 *  @author flo
 *  @version 0.3.1
 *  @date 2004/12/14
 *  @todo more functions
 *  
 *  the gui_style class
 */

class A2E_API gui_style
{
public:
	gui_style();
	~gui_style();

	//! the style color scheme
	enum COLOR_SCHEME {
		WINDOWS,	//!< @enum windows like colors
		BLUE,		//!< @enum blue colors
		BLACKWHITE	//!< @enum black/white colors
	};

	void init(SDL_Surface* screen);
	void set_color_scheme(COLOR_SCHEME scheme);

	//! gui style color - window bg color (used for i.g. start_draw() in the engine class)
	unsigned int STYLE_WINDOW_BG;
	//! gui style color - bar bg color (used for i.g. vertical bar)
	unsigned int STYLE_BARBG;
	//! gui style color - bg color (used for i.g. buttons)
	unsigned int STYLE_BG;
	//! gui style color - bg color 2 (used for i.g. input boxes)
	unsigned int STYLE_BG2;
	//! gui style color - light color
	unsigned int STYLE_LIGHT;
	//! gui style color - dark color
	unsigned int STYLE_DARK;
	//! gui style color - dark color 2
	unsigned int STYLE_DARK2;
	//! gui style color - inside dark color
	unsigned int STYLE_INDARK;
	//! gui style color - arrow color
	unsigned int STYLE_ARROW;
	//! gui style color - normal font color
	unsigned int STYLE_FONT;
	//! gui style color - normal font color 2
	unsigned int STYLE_FONT2;
	//! gui style color - selected color
	unsigned int STYLE_SELECTED;

protected:
    gfx g;
	SDL_Surface* screen;

};

#endif
