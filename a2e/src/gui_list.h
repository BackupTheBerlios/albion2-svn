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
#include <vector>
#include <SDL/SDL.h>
#include "msg.h"
#include "core.h"
#include "gfx.h"
#include "event.h"
#include "engine.h"
#include "gui_text.h"
#include "gui_button.h"
#include "gui_vbar.h"
#include "gui_font.h"
#include "gui_object.h"
#include "gui_style.h"
using namespace std;

#include "win_dll_export.h"

/*! @class gui_list
 *  @brief gui list box element functions
 *  @author flo
 *  @todo more functions
 *  
 *  the gui_list class
 */

class A2E_API gui_list : public gui_object
{
public:
	gui_list(engine* e, gui_style* gs);
	~gui_list();

	struct item {
		string text;
		unsigned int id;
	};
	
	void draw(unsigned int x, unsigned int y);

    void set_vbar_handler(gui_vbar* ivbar);
    gui_vbar* get_vbar_handler();

	// gui list box element variables functions

	gui_list::item* add_item(char* text, unsigned int id);
	void delete_item(unsigned int id);

	bool get_active();
	unsigned int get_position();
	unsigned int get_selected_id();
	gui_list::item* get_item(unsigned int id);
	gui_list::item* get_selected_item();

	void set_rectangle(gfx::rect* rectangle);
	void set_active(bool is_active);
	void set_position(unsigned int position);
	void set_selected_id(unsigned int sid);
	unsigned int get_citems();
	void select_pos(unsigned int x, unsigned int y);
	void clear();

protected:
	msg* m;
	core* c;
	engine* e;
	gfx* g;
	gui_font* gf;

	gui_vbar* vbar_handler;

	gfx::rect* r1;

	vector<item> items;

	unsigned int drawable_items;
	unsigned int top_item;

	// gui list box element variables
	//! list position
	unsigned int position;
	//! list selected id
	unsigned int sid;

	// event handle stuff

	bool is_active;
};

#endif
