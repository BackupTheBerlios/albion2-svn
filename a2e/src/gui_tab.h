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
 
#ifndef __GUI_TAB_H__
#define __GUI_TAB_H__

#include <iostream>
#include <string>
#include <list>
#include <SDL/SDL.h>
#include "msg.h"
#include "core.h"
#include "gfx.h"
#include "engine.h"
#include "gui_text.h"
#include "gui_object.h"
#include "gui_style.h"
using namespace std;

#include "win_dll_export.h"

/*! @class gui_tab
 *  @brief gui tab element functions
 *  @author flo
 *  @todo more functions
 *  
 *  the gui_tab class
 */

class A2E_API gui_tab : public gui_object
{
public:
	gui_tab(engine* e, gui_style* gs);
	~gui_tab();

	struct tab {
		unsigned int id;
		string title;
	};

	void draw(unsigned int x, unsigned int y);

	unsigned int add_tab(const char* title);
	unsigned int get_tab_id(const char* title);

	void set_active_tab(unsigned int id);
	unsigned int get_active_tab();

	void select_pos(unsigned int x, unsigned int y);

protected:
	msg* m;
	core* c;
	engine* e;
	gfx* g;
	event* evt;

	gfx::rect* tab_rect;

	unsigned int active_tab;
	list<tab*> tabs;

};

#endif
