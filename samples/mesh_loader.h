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
 
#ifndef __MESH_LOADER_H__
#define __MESH_LOADER_H__

#ifdef WIN32
#include <windows.h>
#endif

#include <iostream>
#include <SDL.h>
#include <gl\gl.h>
#include <gl\glu.h>
#include <engine.h>
#include <msg.h>
#include <core.h>
#include <net.h>
#include <gfx.h>
#include <gui.h>
#include <gui_button.h>
#include <gui_text.h>
#include <event.h>
#include <a2emesh.h>
using namespace std;

msg m;
net n;
core c;
engine e;
gfx agfx;
gui agui;
event aevent;

a2emesh mesh;

SDL_Surface* sf;

bool done = false;

SDL_Event ievent;

unsigned int refresh_time;

#endif
