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
 
#ifndef __GUI_CHECK_H__
#define __GUI_CHECK_H__

#include <iostream>
#include <SDL.h>
#include "msg.h"
#include "core.h"
#include "gfx.h"
#include "event.h"
#include "engine.h"
#include "gui_text.h"
using namespace std;

#include "win_dll_export.h"

/*! @class gui_check
 *  @brief gui check box element functions
 *  @author flo
 *  @version 0.1
 *  @date 2005/02/07
 *  @todo more functions
 *  
 *  the gui_check class
 */

class A2E_API gui_check
{
public:
	gui_check();
	~gui_check();
	
	void draw();
    void set_engine_handler(engine* iengine);
	void set_text_handler(gui_text* itext);


	// gui button element variables functions

	unsigned int get_id();
	gfx::rect* get_rectangle();
	char* get_text();
	bool get_checked();

	void set_id(unsigned int id);
	void set_rectangle(gfx::rect* rectangle);
	void set_text(char* text);
	void set_checked(bool state);

protected:
	msg m;
	core c;
	gfx g;

	engine* engine_handler;

	gui_text* text_handler;

	// gui check box element variables

	//! check box id
	unsigned int id;
	//! check box rectangle
	gfx::rect* rectangle;
	//! check box text
	char* text;
	//! is check box set/checked?
	bool checked;

};

#endif
