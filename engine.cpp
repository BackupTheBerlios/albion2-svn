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

#include "engine.h"
#include "msg.h"
#include "core.h"
#include "net.h"

#ifdef WIN32
BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
    return TRUE;
}
#endif // WIN32

/*! there is no function currently
*/
engine::engine() {
}

/*! there is no function currently
*/
engine::~engine() {
}

void engine::init(unsigned int width, unsigned int height, unsigned int depth, bool fullscreen) {
	// initialize sdl
	if(SDL_Init(SDL_INIT_VIDEO) == -1) {
		m.print(m.MERROR, "engine.cpp", "Can't init SDL:  %s", SDL_GetError());
		exit(1);
	}
	atexit(SDL_Quit);

	// create screen
	engine::flags = SDL_HWSURFACE | SDL_DOUBLEBUF;
	engine::height = height;
	engine::width = width;
	engine::depth = depth;
	if(fullscreen) { flags = flags | SDL_FULLSCREEN; }
	screen = SDL_SetVideoMode(width, height, depth, flags);
	if(screen == NULL) {
		m.print(m.MERROR, "engine.cpp", "Can't set video mode: %s", SDL_GetError());
		exit(1);
	}
}

void engine::setwidth(unsigned int new_width) {
	engine::width = new_width;
	screen = SDL_SetVideoMode(width, height, depth, flags);
	if(screen == NULL) {
		m.print(m.MERROR, "engine.cpp", "Can't set video mode: %s", SDL_GetError());
		exit(1);
	}
}

void engine::setheight(unsigned int new_height) {
	engine::height = new_height;
	screen = SDL_SetVideoMode(width, height, depth, flags);
	if(screen == NULL) {
		m.print(m.MERROR, "engine.cpp", "Can't set video mode: %s", SDL_GetError());
		exit(1);
	}
}

void engine::draw() {
	SDL_Flip(screen);
}

SDL_Surface* engine::get_screen() {
	return screen;
}
