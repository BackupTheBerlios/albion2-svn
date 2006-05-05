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
#include <vector>
#include <map>
#include <string>
#include <sstream>
#include <SDL/SDL.h>
#include "engine.h"
#include "msg.h"
#include "gfx.h"
#include "file_io.h"
#include "gui_text.h"
#include "image.h"
using namespace std;

#include "win_dll_export.h"

/*! @class gui_style
 *  @brief gui style stuff
 *  @author flo
 *  @todo more functions
 *  
 *  the gui_style class
 */

class A2E_API gui_style
{
public:
	gui_style(engine* e);
	~gui_style();

	//! the style color scheme
	enum COLOR_SCHEME {
		WINDOWS,	//!< enum windows like colors
		BLUE,		//!< enum blue colors
		BLACKWHITE	//!< enum black/white colors
	};

	void load_gui_elements(const char* list);
	void render_gui_element(const char* name, const char* state_name, gfx::rect* rectangle, unsigned int x, unsigned int y);
	void set_gui_text(gui_text* text);
	void set_image(image* img);

	//! if you want set the color scheme from the outside, use the gui function
	bool set_color_scheme(const char* scheme);
	void load_color_schemes(const char* cs_file);
	unsigned int get_color(const char* name);

protected:
	engine* e;
	core* c;
	gfx* g;
	msg* m;
	xml* x;
	file_io* f;
	SDL_Surface* screen;

	map<string, map<string, unsigned int> > color_schemes;
	string cur_scheme;

	enum GFXTYPE {
		GT_POINT,
		GT_LINE,
		GT_RECT,
		GT_RECT_FILLED,
		GT_RECT_2COL,
		GT_TEXT,
		GT_IMG,
		GT_HFADE,
		GT_VFADE,
		GT_DFADE
	};

	struct gfx_type {
		GFXTYPE type;
		unsigned int bbox[4];
		int correct[4];
		string primary_color;
		string secondary_color;
	};

	struct state {
		string state_name;
	};

	struct gui_element {
		string filename;
		vector< vector<gfx_type> > gfx_types;
		vector<state> states;
	};

	map<string, gui_element> gui_elements;

	vector<string> state_types;

	stringstream* buffer;
	string tmp;

	gfx::rect* r1;
	core::pnt* p1;
	core::pnt* p2;

	gui_text* text;
	image* img;

};

#endif
