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
#include "gui_style.h"

// dll main for windows dll export
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
	m.print(msg::MDEBUG, "engine.cpp", "freeing engine stuff");

	/*if(screen) {
        SDL_FreeSurface(screen);
	}*/

	m.print(msg::MDEBUG, "engine.cpp", "engine stuff freed");
}

/*! initializes the engine
 *  @param width the window width
 *  @param height the window height
 *  @param depth the depth of the window (8(?), 16, 24 or 32)
 *  @param fullscreen bool if the window is drawn in fullscreen mode
 */
void engine::init(unsigned int width, unsigned int height, unsigned int depth, bool fullscreen) {
	m.print(msg::MDEBUG, "engine.cpp", "initializing albion 2 engine");

	// initialize sdl
	if(SDL_Init(SDL_INIT_VIDEO) == -1) {
		m.print(m.MERROR, "engine.cpp", "Can't init SDL:  %s", SDL_GetError());
		exit(1);
	}
	else {
		m.print(msg::MDEBUG, "engine.cpp", "sdl initialized");
	}
	atexit(SDL_Quit);

	// get video info
	video_info = SDL_GetVideoInfo();
	if(!video_info) {
		m.print(msg::MDEBUG, "engine.cpp", "video query failed: %s",
			SDL_GetError());
		exit(1);
	}
	else {
		m.print(msg::MDEBUG, "engine.cpp", "successfully received video info");
	}

	m.print(msg::MDEBUG, "engine.cpp",
		"amount of available video memory: %u kb", video_info->video_mem);

	// gl attributes
	switch(depth) {
		case 16:
			SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
			SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 5);
			SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);
			SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 1);
			break;
		case 24:
			SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 6);
			SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 6);
			SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 6);
			SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 6);
			break;
		case 32:
			SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
			SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
			SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
			SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
			break;
	}

	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, depth);
	m.print(msg::MDEBUG, "engine.cpp", "depth set to %u bit", depth);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	m.print(msg::MDEBUG, "engine.cpp", "double buffer enabled");

	// create screen
	engine::flags |= SDL_HWPALETTE;
	engine::flags |= SDL_OPENGL;
	engine::flags |= SDL_GL_DOUBLEBUFFER;
	if(video_info->hw_available) { engine::flags |= SDL_HWSURFACE; }
	else { engine::flags |= SDL_SWSURFACE; }
	if(video_info->blit_hw) { engine::flags |= SDL_HWACCEL;	}
	if(fullscreen) {
		engine::flags |= SDL_FULLSCREEN;
		m.print(msg::MDEBUG, "engine.cpp", "fullscreen enabled");
	}
	else {
		m.print(msg::MDEBUG, "engine.cpp", "fullscreen disabled");
	}

	engine::height = height;
	engine::width = width;
	engine::depth = depth;
	screen = SDL_SetVideoMode(width, height, depth, flags);
	if(screen == NULL) {
		m.print(m.MERROR, "engine.cpp", "Can't set video mode: %s", SDL_GetError());
		exit(1);
	}
	else {
		m.print(msg::MDEBUG, "engine.cpp", "video mode set: w%u h%u d%u", width,
			height, depth);
	}

	// print out some opengl informations
	m.print(msg::MDEBUG, "engine.cpp", "vendor: %s", glGetString(GL_VENDOR));
	m.print(msg::MDEBUG, "engine.cpp", "renderer: %s", glGetString(GL_RENDERER));
	m.print(msg::MDEBUG, "engine.cpp", "version: %s", glGetString(GL_VERSION));

	// enable key repeat
	if((SDL_EnableKeyRepeat(200, SDL_DEFAULT_REPEAT_INTERVAL))) {
		m.print(msg::MDEBUG, "engine.cpp", "setting keyboard repeat failed: %s",
				SDL_GetError());
		exit(1);
	}
	else {
		m.print(msg::MDEBUG, "engine.cpp", "keyboard repeat set");
	}

	gstyle.init(screen);
	m.print(msg::MDEBUG, "engine.cpp", "gui style initialized");
	gstyle.set_color_scheme(gui_style::WINDOWS);
	m.print(msg::MDEBUG, "engine.cpp", "color scheme set to windows like");

	// initialize ogl
	init_gl();
	m.print(msg::MDEBUG, "engine.cpp", "opengl initialized");

	// resize stuff
	resize_window();
	m.print(msg::MDEBUG, "engine.cpp", "window resizing functions initialized");

	// reserve memory for position ...
	engine::position = new core::vertex3();
}

/*! sets the window width
 *  @param width the window width
 */
void engine::set_width(unsigned int width) {
	engine::width = width;
	engine::screen = SDL_SetVideoMode(engine::width, engine::height,
		engine::depth, engine::flags);
	if(engine::screen == NULL) {
		m.print(m.MERROR, "engine.cpp", "Can't set video mode: %s", SDL_GetError());
		exit(1);
	}
}

/*! sets the window height
 *  @param height the window height
 */
void engine::set_height(unsigned int height) {
	engine::height = height;
	engine::screen = SDL_SetVideoMode(engine::width, engine::height,
		engine::depth, engine::flags);
	if(engine::screen == NULL) {
		m.print(m.MERROR, "engine.cpp", "Can't set video mode: %s", SDL_GetError());
		exit(1);
	}
}

/*! starts drawing the window
 */
void engine::start_draw() {
	// draws ogl stuff
	draw_gl_scene();
}

/*! stops drawing the window
 */
void engine::stop_draw() {
	SDL_GL_SwapBuffers();
}

/*! returns the surface used by the engine
 */
SDL_Surface* engine::get_screen() {
	return screen;
}

/*! sets the window caption
 *  @param caption the window caption
 */
void engine::set_caption(char* caption) {
	SDL_WM_SetCaption(caption, NULL);
}

/*! returns the window caption
 */
char* engine::get_caption() {
	char* caption;
	SDL_WM_GetCaption(&caption, NULL);
	return caption;
}

/*! returns the gstyle
 */
gui_style engine::get_gstyle() {
	return gstyle;
}

/*! sets the window color scheme
 *  @param scheme the window color scheme we want to set
 */
void engine::set_color_scheme(gui_style::COLOR_SCHEME scheme) {
	gstyle.set_color_scheme(scheme);
}

/*! opengl initialization function
 */
bool engine::init_gl() {
	// enable texture mapping
	glEnable(GL_TEXTURE_2D);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	// enable smooth shading
	glShadeModel(GL_SMOOTH);
	// set clear color
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	// depth buffer setup
	glClearDepth(1.0f);
	// enable depth testing
	glEnable(GL_DEPTH_TEST);
	// lequal depth test
	glDepthFunc(GL_LEQUAL);
	// nice perspective calculations
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	// enable backface culling
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);

	return true;
}

/*! opengl drawing code
 */
bool engine::draw_gl_scene() {
	unsigned int bgcolor = gstyle.STYLE_WINDOW_BG;
	glClearColor((GLclampf)((float)((bgcolor & 0xFF0000) >> 16) / 255),
		(GLclampf)((float)((bgcolor & 0xFF00) >> 8) / 255),
		(GLclampf)((float)(bgcolor & 0xFF) / 255), 0.0f);

    // clear the color and depth buffers.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	float pinf[4][4];
	pinf[1][0] = pinf[2][0] = pinf[3][0] = pinf[0][1] = pinf[2][1] = pinf[3][1] =
		pinf[0][2] = pinf[1][2] = pinf[0][3] = pinf[1][3] = pinf[3][3] = 0.0f;
	pinf[0][0] = atanf(60.0f) / (engine::width / engine::height);
	pinf[1][1] = atanf(60.0f);
	pinf[3][2] = -2.0f * 0.1f;
	pinf[2][2] = pinf[2][3] = -1.0f;

	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(&pinf[0][0]);

    // we don't want to modify the projection matrix.
	glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

	// draw the shadows
	engine::shd.draw();

	return true;
}

/* function to reset our viewport after a window resize
 */
bool engine::resize_window() {
	// set the viewport
	glViewport(0, 0, (GLsizei)engine::width, (GLsizei)engine::height);
	m.print(msg::MDEBUG, "engine.cpp", "viewport set");

	// projection matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	m.print(msg::MDEBUG, "engine.cpp", "matrix mode (projection) set");

	// set perspective with fov = 60° and far value = 1500.0f
	gluPerspective(60.0f, engine::width/engine::height, 0.1f, 1500.0f);
	
	m.print(msg::MDEBUG, "engine.cpp", "glu perspective set");

	// model view matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	m.print(msg::MDEBUG, "engine.cpp", "matrix mode (modelview) set");

	return true;
}

/*! sets the position of the user
 *  @param xpos x coordinate
 *  @param ypos y coordinate
 *  @param zpos z coordinate
 */
void engine::set_position(float xpos, float ypos, float zpos) {
	engine::position->x = xpos;
	engine::position->y = ypos;
	engine::position->z = zpos;

	glTranslatef(engine::position->x, engine::position->y, engine::position->z);
}

/*! returns the position of the user
 */
core::vertex3* engine::get_position() {
	return engine::position;
}

/*! starts drawing the 2d elements and initializes the opengl functions for that
 */
void engine::start_2d_draw() {
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	// we need an orthogonal view (2d) for drawing 2d elements
	glOrtho(0.0, screen->w, 0.0, screen->h, -1.0, 1.0);
	
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
}

/*! stops drawing the 2d elements
 */
void engine::stop_2d_draw() {
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
}
