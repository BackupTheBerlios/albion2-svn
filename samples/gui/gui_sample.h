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
 
#ifndef __GUI_SAMPLE_H__
#define __GUI_SAMPLE_H__

#ifdef WIN32
#include <windows.h>
#endif

#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include <ctime>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <engine.h>
#include <msg.h>
#include <core.h>
#include <gfx.h>
#include <gui.h>
#include <event.h>
#include <image.h>
using namespace std;

void open_info_wnd();

engine* e;
msg* m;
core* c;
gfx* agfx;
gui* agui;
event* aevent;
image* img;
gui_style* gs;

bool done = false;

SDL_Event sevent;

stringstream caption;
stringstream tmp;

gui_button* bedit;
gui_button* badd;
gui_button* bset;
gui_button* bopen_msg;
gui_button* bopen_fd;
gui_button* bopen_wnd;
gui_list* llist;
gui_input* ilitem;
gui_text* tstatus;
gui_check* clogo;

GUI_OBJ info_wnd_id = -1;
gui_window* info_wnd;
gui_combo* cinfo_type;
gui_text* tinfo1;
gui_text* tinfo2;
gui_text* tinfo3;

bool img_visible = true;
unsigned int cur_ed_id = -1;
gui_window* file_dialog;
GUI_OBJ file_dialog_id = 50;
unsigned int cur_info_id = 0;
time_t rawtime;
struct tm* tinfo;

#endif
