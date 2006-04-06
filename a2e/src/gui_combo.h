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
 
#ifndef __GUI_COMBO_H__
#define __GUI_COMBO_H__

#include <iostream>
#include <string>
#include <vector>
#include <SDL/SDL.h>
#include "msg.h"
#include "core.h"
#include "gfx.h"
#include "event.h"
#include "engine.h"
#include "gui_text.h"
#include "gui_button.h"
#include "gui_font.h"
using namespace std;

#include "win_dll_export.h"

/*! @class gui_combo
 *  @brief gui combo box element functions
 *  @author flo
 *  @todo more functions
 *  
 *  the gui_combo class
 */

class A2E_API gui_combo
{
public:
	gui_combo(engine* e);
	~gui_combo();
	
	void draw(unsigned int x, unsigned int y);

	// gui combo box element variables functions

	unsigned int get_id();
	gfx::rect* get_rectangle();

	void set_id(unsigned int id);
	void set_rectangle(gfx::rect* rectangle);

	void set_list_visible(bool visible);
	bool is_list_visible();

	void set_item_text(gui_text* text);
	void set_list_button(gui_button* button);

	gui_text* get_item_text();
	gui_button* get_list_button();

	void add_item(char* text, unsigned int id);
	void set_selected_item(unsigned int item);
	void set_selected_id(unsigned int id);
	unsigned int get_selected_id();

	unsigned int get_item_count();

	void select_pos(core::pnt* pos);
	void click_pos(core::pnt* pos);

protected:
	msg* m;
	core* c;
	engine* e;
	gfx* g;
	gui_font* gf;


	// gui combo box element variables

	//! combo box id
	unsigned int id;
	//! combo box rectangle
	gfx::rect* rectangle;

	unsigned int selected_id;
	unsigned int selected_item;
	unsigned int marked_item;

	gui_text* item_text;
	gui_button* list_button;
	bool list_button_pressed;

	struct item {
		string text;
		unsigned int id;
	};
	vector<item> items;

	// event handle stuff
	bool list_visible;
};

#endif
