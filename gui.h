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
 
#ifndef __GUI_H__
#define __GUI_H__

#include <iostream>
#include <SDL.h>
#include <SDL_ttf.h>
#include "msg.h"
#include "core.h"
#include "net.h"
#include "engine.h"
#include "gfx.h"
#include "event.h"
#include "gui_text.h"
#include "gui_button.h"
using namespace std;

#include "win_dll_export.h"

/*! @class gui
 *  @brief graphical user interface functions
 *  @author flo
 *  @version 0.1
 *  @date 2004/08/13
 *  @todo more functions
 *  
 *  the gui class
 */

class A2E_API gui
{
public:
	gui();
	~gui();

	void init(engine &iengine, event &ievent);
	void draw();

	gui_button* add_button(gfx::rect* rectangle, unsigned int id, char* text);
	gui_text* add_text(char* font_name, unsigned int font_size, char* text,
				   unsigned int color, gfx::pnt* point, unsigned int id);	

	SDL_Surface* get_gui_surface();

protected:
	msg m;
	core c;
	gfx g;

	event* event_handler;
	engine* engine_handler;
	
	SDL_Surface* gui_surface;

	struct gui_element {
		unsigned int id;
		unsigned int type;
		unsigned int num;
		bool is_drawn;
	};

	//! gui element types
	enum GTYPE {
		BUTTON,	//!< @enum button type
		INPUT,	//!< @enum input box type
		TEXT,	//!< @enum static text type
		FONT,	//!< @enum font type
	};

	//! gui elements
	gui_element* gui_elements;
	//! current amount of gui elements
	unsigned int celements;

	//! gui buttons
	gui_button* gui_buttons[128];
	//! current amount of gui button elements
	unsigned int cbuttons;

	//! gui texts
	gui_text* gui_texts[128];
	//! current amount of gui text elements
	unsigned int ctexts;
};

#endif
