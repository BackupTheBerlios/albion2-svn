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

#ifdef WIN32
#include <io.h>
#include <direct.h>
#else
#include <dirent.h>
#endif

#include <iostream>
#include <vector>
#include <list>
#include <SDL/SDL.h>
#include "msg.h"
#include "core.h"
#include "engine.h"
#include "gfx.h"
#include "event.h"
#include "rtt.h"
#include "shader.h"
#include "gui_text.h"
#include "gui_button.h"
#include "gui_input.h"
#include "gui_list.h"
#include "gui_vbar.h"
#include "gui_check.h"
#include "gui_window.h"
#include "gui_font.h"
#include "gui_combo.h"
#include "gui_style.h"
#include "gui_mltext.h"
#include "gui_image.h"
#include "gui_tab.h"
#include "gui_toggle.h"
#include "gui_object.h"
using namespace std;
typedef unsigned int GUI_OBJ;

union gui_object_list_pointer {
	list<gui_object*>* o;
	
	list<gui_button*>* b;
	list<gui_input*>* in;
	list<gui_list*>* l;
	list<gui_vbar*>* v;
	list<gui_check*>* ch;
	list<gui_window*>* w;
	list<gui_combo*>* co;
	list<gui_mltext*>* m;
	list<gui_image*>* im;
	list<gui_tab*>* ta;
	list<gui_text*>* te;
	list<gui_toggle*>* to;
};

#include "win_dll_export.h"

/*! @class gui
 *  @brief graphical user interface functions
 *  @author flo
 *  @todo needs some more restructuring
 *  
 *  the gui class
 */

class A2E_API gui
{
public:
	gui(engine* e, shader* s);
	~gui();

	//! gui element types
	enum GTYPE {
		BUTTON,		//!< enum button type
		INPUT,		//!< enum input box type
		TEXT,		//!< enum static text type
		EMPTY,		//!< enum empty type
		LIST,		//!< enum list box type
		VBAR,		//!< enum vertical bar type
		CHECK,		//!< enum check box type
		COMBO,		//!< enum combo box type
		WINDOW,		//!< enum window type
		MLTEXT,		//!< enum multi line text type
		IMAGE,		//!< enum image type
		TAB,		//!< enum tab type
		TOGGLE,		//!< enum toggle button type
		OPENDIALOG,	//!< enum open file dialog type
		MSGBOX_OK	//!< enum message box type
	};

	typedef bool file_filter(const char*);

	struct gui_element {
		unsigned int id;
		unsigned int type;
		unsigned int num;
		unsigned int wid;
		unsigned int tid;
		bool is_drawn;
	};

	struct msg_ok_wnd {
		GUI_OBJ id;
		gui_window* wnd;
		gui_text* text;
		gui_button* ok;
	};

	void init();
	void draw();
	void draw_element(core::pnt* wp, list<gui::gui_element>::iterator ge_iter, list<gui_window>::reference wnd_iter);
	bool handle_element(core::pnt* wp, list<gui::gui_element>::iterator ge_iter, list<gui_window>::reference wnd_iter);

	bool delete_element(unsigned int id);

	GUI_OBJ add_button(gfx::rect* rectangle, unsigned int id, char* text, GLuint image_texture, unsigned int wid = 0, unsigned int tid = 0);
	GUI_OBJ add_text(char* font_name, unsigned int font_size, char* text,
				   unsigned int color, core::pnt* point, unsigned int id, unsigned int wid = 0, unsigned int tid = 0);	
	GUI_OBJ add_input_box(gfx::rect* rectangle, unsigned int id, char* text, unsigned int wid = 0, unsigned int tid = 0);
	GUI_OBJ add_list_box(gfx::rect* rectangle, unsigned int id, unsigned int wid = 0, unsigned int tid = 0);
	GUI_OBJ add_vbar(gfx::rect* rectangle, unsigned int id, unsigned int wid = 0, unsigned int tid = 0);
	GUI_OBJ add_check_box(gfx::rect* rectangle, unsigned int id, char* text, unsigned int wid = 0, unsigned int tid = 0);
	GUI_OBJ add_combo_box(gfx::rect* rectangle, unsigned int id, unsigned int wid = 0, unsigned int tid = 0);
	GUI_OBJ add_window(gfx::rect* rectangle, unsigned int id, char* caption, bool border = true, bool bg = true);
	GUI_OBJ add_mltext(gfx::rect* rectangle, unsigned int id, char* text, unsigned int wid = 0, unsigned int tid = 0);
	GUI_OBJ add_image(gfx::rect* rectangle, unsigned int id, const char* image_filename, image* image_obj, unsigned int wid = 0, unsigned int tid = 0);
	GUI_OBJ add_tab(gfx::rect* rectangle, unsigned int id, unsigned int wid = 0);
	GUI_OBJ add_toggle(gfx::rect* rectangle, unsigned int id, char* text, GLuint image_texture, unsigned int wid = 0, unsigned int tid = 0);

	gui_object* get_object(unsigned int id);

	void handle_input(list<gui_input>::reference input_box);

	SDL_Surface* get_gui_surface();

	list<gui_element>::iterator get_active_element();
	void set_active_element(list<gui_element>::iterator active_element);
	void set_active_element(unsigned int id);

	gui_style* get_gui_style();

	list<gui_element>::iterator get_element_iter(unsigned int id);
	msg_ok_wnd* get_msgbox(unsigned int id);
	gui_element* get_element(unsigned int id);

	bool exist(unsigned int id);

	void set_visibility(unsigned int id, bool state, bool force = false);

	void set_color_scheme(const char* scheme);

	void set_focus(unsigned int id);

	unsigned int get_free_id();

	bool is_redraw(list<gui::gui_element>::iterator ge_iter);

	// dialogs
	GUI_OBJ add_open_dialog(unsigned int id, char* caption, char* dir, char* ext, unsigned int x = 30, unsigned int y = 30, file_filter* callback = NULL);
	gui_list* get_open_diaolg_list();
	GUI_OBJ add_msgbox_ok(char* caption, char* text);

protected:
	msg* m;
	core* c;
	event* evt;
	engine* e;
	gfx* g;
	gui_font* gf;
	gui_style* gs;
	rtt* r;
	shader* s;

	SDL_Surface* gui_surface;

	//! gui elements
	list<gui_element> gui_elements;

	//! current active gui element
	list<gui_element>::iterator active_element;

	//! gui buttons
	list<gui_button*> gui_buttons;

	//! gui texts
	list<gui_text*> gui_texts;

	//! gui input boxes
	list<gui_input*> gui_input_boxes;

	//! gui list boxes
	list<gui_list*> gui_list_boxes;

	//! gui vertical bars
	list<gui_vbar*> gui_vbars;

	//! gui check boxes
	list<gui_check*> gui_check_boxes;

	//! gui combo boxes
	list<gui_combo*> gui_combo_boxes;

	//! gui multi line texts
	list<gui_mltext*> gui_mltexts;

	//! gui images
	list<gui_image*> gui_images;

	//! gui windows
	list<gui_window*> gui_windows;

	//! gui tabs
	list<gui_tab*> gui_tabs;

	//! gui toggle buttons
	list<gui_toggle*> gui_toggle_buttons;

	//! window framebuffer objects
	struct window_buffer {
		unsigned int wid;
		rtt::fbo* buffer;
		rtt::fbo* shadow;
	};
	list<window_buffer*> window_buffers;

	//! list of all window ids (the id at the end is the id of the "most"
	//! front window and the one in the beginning the "most" far window)
	list<unsigned int> wnd_layers;

	core::pnt* p;
	core::pnt* p2;
	gfx::rect* r1;
	gfx::rect* r2;
	GUI_OBJ main_window;

	bool ae_reset;
	bool wnd_event;
	unsigned int current_id;

	GLuint icon_cross;
	GLuint icon_arrow_up;
	GLuint icon_arrow_down;
	GLuint icon_checked;

	//! used by get_free_id
	unsigned int start_id;

	unsigned int shadow_type;
	float* tcs_h;
	float* tcs_v;
	unsigned int xcorrect;
	unsigned int ycorrect;
	int xadd;
	int yadd;


	// dialog stuff ...
	GUI_OBJ ofd_wnd_id;
	gui_window* ofd_wnd;
	gui_button* ofd_open;
	gui_button* ofd_cancel;
	gui_list* ofd_dirlist;

	list<msg_ok_wnd*> msg_boxes;

public:
	// c++ is weird ..... at least in combination with gcc
	template<typename T> T* get_object(unsigned int id) {
		union T_list_pointer {
			list<T*>* t_pointer;
			
			list<gui_button*>* b;
			list<gui_input*>* in;
			list<gui_list*>* l;
			list<gui_vbar*>* v;
			list<gui_check*>* ch;
			list<gui_window*>* w;
			list<gui_combo*>* co;
			list<gui_mltext*>* m;
			list<gui_image*>* im;
			list<gui_tab*>* ta;
			list<gui_text*>* te;
			list<gui_toggle*>* to;
		};

		T_list_pointer objects;

		switch(get_element(id)->type) {
			case BUTTON:
				objects.b = &gui_buttons;
				break;
			case INPUT:
				objects.in = &gui_input_boxes;
				break;
			case TEXT:
				objects.te = &gui_texts;
				break;
			case LIST:
				objects.l = &gui_list_boxes;
				break;
			case VBAR:
				objects.v = &gui_vbars;
				break;
			case CHECK:
				objects.ch = &gui_check_boxes;
				break;
			case COMBO:
				objects.co = &gui_combo_boxes;
				break;
			case WINDOW:
				objects.w = &gui_windows;
				break;
			case MLTEXT:
				objects.m = &gui_mltexts;
				break;
			case IMAGE:
				objects.im = &gui_images;
				break;
			case TAB:
				objects.ta = &gui_tabs;
				break;
			case TOGGLE:
				objects.to = &gui_toggle_buttons;
				break;
			default:
				return NULL;
				break;
		}

		for(typename list<T*>::iterator iter = objects.t_pointer->begin(); iter != objects.t_pointer->end(); iter++) {
			if((*iter)->get_id() == id) {
				return *iter;
			}
		}

		m->print(msg::MERROR, "gui.cpp", "get_object(): no %s with such an id (%u) exists!", objects.t_pointer->size() > 0 ? objects.t_pointer->back()->get_type()->c_str() : "", id);

		return NULL;
	}

};

#endif
