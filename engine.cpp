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

/*! initializes the engine
 *  @param width the window width
 *  @param height the window height
 *  @param depth the depth of the window (8(?), 16, 24 or 32)
 *  @param fullscreen bool if the window is drawn in fullscreen mode
 */
void engine::init(unsigned int width, unsigned int height, unsigned int depth, bool fullscreen) {
	// initialize sdl
	if(SDL_Init(SDL_INIT_VIDEO) == -1) {
		m.print(m.MERROR, "engine.cpp", "Can't init SDL:  %s", SDL_GetError());
		exit(1);
	}
	atexit(SDL_Quit);

	// get video info
	video_info = SDL_GetVideoInfo();
	if(!video_info) {
		m.print(msg::MDEBUG, "engine.cpp", "video query failed: %s",
			SDL_GetError());
		exit(1);
	}

	// gl attributes
	switch(depth) {
		case 16:
			SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
			SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 6);
			SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);
			//SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 1);
			break;
		case 24:
			SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
			SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
			SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
			//SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 6);
			break;
		case 32:
			SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
			SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
			SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
			//SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
			break;
	}

	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, depth);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	// create screen
	engine::flags |= SDL_HWPALETTE;
	engine::flags |= SDL_OPENGL;
	engine::flags |= SDL_GL_DOUBLEBUFFER;
	if(video_info->hw_available) { engine::flags |= SDL_HWSURFACE; }
	else { engine::flags |= SDL_SWSURFACE; }
	if(video_info->blit_hw) { engine::flags |= SDL_HWACCEL;	}
	if(fullscreen) { engine::flags |= SDL_FULLSCREEN; }

	engine::height = height;
	engine::width = width;
	engine::depth = depth;
	screen = SDL_SetVideoMode(width, height, depth, flags);
	if(screen == NULL) {
		m.print(m.MERROR, "engine.cpp", "Can't set video mode: %s", SDL_GetError());
		exit(1);
	}

	// enable key repeat
	if((SDL_EnableKeyRepeat(100, SDL_DEFAULT_REPEAT_INTERVAL))) {
		m.print(msg::MDEBUG, "engine.cpp", "setting keyboard repeat failed: %s",
				SDL_GetError());
		exit(1);
	}

	gstyle.init(screen);
	gstyle.set_color_scheme(gui_style::WINDOWS);

	// initialize gl
	initGL();

	// resize stuff
	resizeWindow();

	// reserve memory for position ...
	engine::position = (core::vertex*)malloc(sizeof(core::vertex));
}

/*! sets the window width
 *  @param width the window width
 */
void engine::set_width(unsigned int new_width) {
	engine::width = new_width;
	screen = SDL_SetVideoMode(width, height, depth, flags);
	if(screen == NULL) {
		m.print(m.MERROR, "engine.cpp", "Can't set video mode: %s", SDL_GetError());
		exit(1);
	}
}

/*! sets the window height
 *  @param height the window height
 */
void engine::set_height(unsigned int new_height) {
	engine::height = new_height;
	screen = SDL_SetVideoMode(width, height, depth, flags);
	if(screen == NULL) {
		m.print(m.MERROR, "engine.cpp", "Can't set video mode: %s", SDL_GetError());
		exit(1);
	}
}

/*! starts drawing the window
 */
void engine::start_draw() {
	// draws ogl stuff
	drawGLScene();
}

/*! stops drawing the window
 */
void engine::stop_draw() {
	SDL_GL_SwapBuffers();
	//SDL_Flip(screen);
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
int engine::initGL(GLvoid) {
	// enable texture mapping
	glEnable(GL_TEXTURE_2D);

    /* Enable smooth shading */
    glShadeModel( GL_SMOOTH );

    /* Depth buffer setup */
    glClearDepth( 1.0f );

    /* Enables Depth Testing */
    glEnable( GL_DEPTH_TEST );

    /* The Type Of Depth Test To Do */
    glDepthFunc( GL_LEQUAL );

    /* Really Nice Perspective Calculations */
    glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );

	// disable backface culling
	glCullFace(GL_FRONT);

	return 1;
}

/*! opengl drawing code
 */
int engine::drawGLScene(GLvoid) {
	unsigned int bgcolor = gstyle.STYLE_WINDOW_BG;
	glClearColor((GLclampf)((float)((bgcolor&0xFF0000) >> 16) / 255),
		(GLclampf)((float)((bgcolor&0xFF00) >> 8) / 255),
		(GLclampf)((float)(bgcolor&0xFF) / 255), 0.0f);

    // Clear the color and depth buffers.
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    // We don't want to modify the projection matrix.
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity( );

    return 1;
}

/* function to reset our viewport after a window resize
 */
int engine::resizeWindow(GLvoid) {
    glViewport(0, 0, (GLsizei)engine::width, (GLsizei)engine::height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

	gluPerspective(45.0f, engine::width/engine::height, 0.1f, 500.0f);

    glMatrixMode(GL_MODELVIEW);

    glLoadIdentity();

    return 1;
}

void engine::set_position(float xpos, float ypos, float zpos) {
	engine::position->x = -xpos;
	engine::position->y = ypos;
	engine::position->z = zpos;

	glTranslatef(-engine::position->x, engine::position->y, engine::position->z);
}

core::vertex* engine::get_position() {
	return engine::position;
}
