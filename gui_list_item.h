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
 
#ifndef __GUI_LIST_ITEM_H__
#define __GUI_LIST_ITEM_H__

#include <iostream>
#include <SDL.h>
#include <SDL_ttf.h>
#include "msg.h"
#include "core.h"
#include "gfx.h"
#include "event.h"
#include "engine.h"
#include "gui_text.h"
#include "gui_style.h"
using namespace std;

#include "win_dll_export.h"

/*! @class gui_list_item
 *  @brief gui list box item element functions
 *  @author flo
 *  @version 0.1.1
 *  @date 2004/08/22
 *  @todo more functions
 *  
 *  the gui_list_item class
 */

class A2E_API gui_list_item
{
public:
	gui_list_item();
	~gui_list_item();
	
	void draw_list_item();
    void set_engine_handler(engine* iengine);
	void set_text_handler(gui_text* itext);
	gui_text* get_text_handler();
	void clear();


	// gui list box item element variables functions

	char* get_text();
	unsigned int get_id();
	gfx::pnt* get_point();

	void set_text(char* text);
	void set_id(unsigned int id);
	void set_point(gfx::pnt* point);

protected:
	msg m;
	gfx g;
	gui_style gstyle;

	engine* engine_handler;
	gui_text* text_handler;

	// gui list box element variables

	//! list box item text
	char* text;
	//! list box item id
	unsigned int id;
	//! text starting point (x,y)
	gfx::pnt* point;
};

#endif
