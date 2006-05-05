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

engine* e;
msg* m;
core* c;
gfx* agfx;
gui* agui;
event* aevent;
image* img;

bool done = false;

SDL_Event sevent;

stringstream xpos;
stringstream ypos;
stringstream caption;
stringstream tmp;

#endif
