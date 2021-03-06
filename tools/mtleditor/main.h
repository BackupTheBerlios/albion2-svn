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
 
#ifndef __MAIN_H__
#define __MAIN_H__

#ifdef WIN32
#include <windows.h>
#endif

#include <iostream>
#include <cmath>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <engine.h>
#include <msg.h>
#include <core.h>
#include <net.h>
#include <gfx.h>
#include <gui.h>
#include <event.h>
#include <camera.h>
#include <a2emodel.h>
#include <scene.h>
#include <ode.h>
#include <ode_object.h>
#include <light.h>
#include <shader.h>
#include <lua.h>
#include <xml.h>
#include "mgui.h"
#include "mdl.h"
using namespace std;

msg* m;
net* n;
core* c;
engine* e;
gfx* agfx;
event* aevent;
camera* cam;
file_io* fio;
gui* agui;
shader* s;
lua* l;
xml* x;
scene* sce;
mgui* mg;
mdl* model;

SDL_Surface* sf;

bool done = false;

SDL_Event ievent;

#endif
