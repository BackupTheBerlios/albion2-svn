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
 
#ifndef __GUI_LIST_H__
#define __GUI_LIST_H__

#include <iostream>
#include <SDL.h>
#include <SDL_ttf.h>
#include "msg.h"
#include "core.h"
#include "gfx.h"
#include "event.h"
#include "engine.h"
#include "gui_text.h"
#include "gui_button.h"
#include "gui_list_item.h"
#include "gui_vbar.h"
using namespace std;

#include "win_dll_export.h"

/*! @class gui_list
 *  @brief gui list box element functions
 *  @author flo
 *  @version 0.1.2
 *  @date 2004/08/21
 *  @todo more functions
 *  
 *  the gui_list class
 */

class A2E_API gui_list
{
public:
	gui_list();
	~gui_list();
	
	void draw_list();
    void set_engine_handler(engine* iengine);

    void set_vbar_handler(gui_vbar* ivbar);

	// gui list box element variables functions

	gui_list_item* add_item(char* text, unsigned int id);
	void delete_item(unsigned int id);

	unsigned int get_id();
	gfx::rect* get_rectangle();
	bool get_active();
	unsigned int get_position();

	void set_id(unsigned int id);
	void set_rectangle(gfx::rect* rectangle);
	void set_active(bool is_active);
	void set_position(unsigned int position);

	unsigned int get_citems();

protected:
	msg m;
	core c;
	gfx g;

	engine* engine_handler;

	gui_vbar* vbar_handler;

	gui_list_item* items[512];
	unsigned int citems;

	unsigned int drawable_items;

	// gui list box element variables

	//! list box id
	unsigned int id;
	//! list box rectangle
	gfx::rect* rectangle;
	//! list position
	unsigned int position;

	// event handle stuff

	bool is_active;
};

#endif
