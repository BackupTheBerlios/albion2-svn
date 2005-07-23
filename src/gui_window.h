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
 
#ifndef __GUI_WINDOW_H__
#define __GUI_WINDOW_H__

#include <iostream>
#include <SDL/SDL.h>
#include "msg.h"
#include "core.h"
#include "gfx.h"
#include "event.h"
#include "engine.h"
#include "gui_text.h"
#include "gui_button.h"
using namespace std;

#include "win_dll_export.h"

/*! @class gui_window
 *  @brief gui window element functions
 *  @author flo
 *  @version 0.1
 *  @date 2005/02/16
 *  @todo more functions
 *  
 *  the gui_window class
 */

class A2E_API gui_window
{
public:
	gui_window();
	~gui_window();
	
	void draw();
    void set_engine_handler(engine* iengine);
	void set_text_handler(gui_text* itext);

	unsigned int get_id();
	gfx::rect* get_rectangle();
	unsigned int get_lid();
	char* get_caption();
	bool get_border();

	void set_id(unsigned int id);
	void set_rectangle(gfx::rect* rectangle);
	void set_lid(unsigned int id);
	void set_caption(char* caption);
	void set_border(bool state);

	void change_position(int x, int y);

	bool is_moving();
	void set_moving(bool state);

	void set_exit_button_handler(gui_button* ibutton);

	bool get_deleted();
	void set_deleted(bool state);

protected:
	msg m;
	core c;
	gfx g;

	engine* engine_handler;
	gui_text* text_handler;
	
	gui_button* exit_button_handler;

	//! window id
	unsigned int id;
	//! window rectangle
	gfx::rect* rectangle;
	//! window layer id
	unsigned int lid;
	//! window caption
	char* caption;
	//! window border draw flag
	bool border;

	bool moving;

	bool deleted;

};

#endif
