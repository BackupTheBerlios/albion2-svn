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

// dll main for windows dll export
#ifdef WIN32
BOOL APIENTRY DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
	switch (ul_reason_for_call) {
		case DLL_PROCESS_ATTACH:
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			break;
	}
    return TRUE;
}

//! pseudo main function for properly linking with sdl ...
int main(int argc, char *argv[]) {
	return 0;
}
#endif // WIN32

/*! there is no function currently
 */
engine::engine(const char* datapath) {
	engine::datapath = datapath;
	cursor_visible = true;
	fps_limit = 10;
	flags = 0;
	position = NULL;
	rotation = NULL;
	screen = NULL;
	key_repeat = 200;
	width = 640;
	height = 480;
	depth = 16;
	zbuffer = 16;
	stencil = 8;
	fullscreen = false;
	shadow_type = 0;
	filtering = 0;
	hdr = false;
	hdr_rgba16 = true;

	m = new msg();
	f = new file_io(m);
	c = new core(m, f);
	x = new xml(m);
	e = new event(datapath, m, x);
	g = new gfx();
	t = new texman(m, f);
	l = new lua(m);
	gf = new gui_font(m);

	server = new server_data();
	client = new client_data();

	// load config
	if(!x->open(data_path("config.xml"))) {
		m->print(msg::MERROR, "engine.cpp", "engine(): couldn't open config file - using standard settings!");
	}
	else {
		while(x->process()) {
			if(strcmp(x->get_node_name(), "screen") == 0) {
				if(x->get_attribute((char*)(char*)"width") != NULL) {
					width = (unsigned int)atoi(x->get_attribute((char*)"width"));
				}

				if(x->get_attribute((char*)"height") != NULL) {
					height = (unsigned int)atoi(x->get_attribute((char*)"height"));
				}

				if(x->get_attribute((char*)"depth") != NULL) {
					depth = (unsigned int)atoi(x->get_attribute((char*)"depth"));
				}

				if(x->get_attribute((char*)"zbuffer") != NULL) {
					zbuffer = (unsigned int)atoi(x->get_attribute((char*)"zbuffer"));
				}

				if(x->get_attribute((char*)"stencil") != NULL) {
					stencil = (unsigned int)atoi(x->get_attribute((char*)"stencil"));
				}

				if(x->get_attribute((char*)"fullscreen") != NULL) {
					if(atoi(x->get_attribute((char*)"fullscreen")) != 0) {
						engine::fullscreen = true;
					}
				}
			}
			else if(strcmp(x->get_node_name(), "gui") == 0) {
				if(x->get_attribute((char*)"color_scheme") != NULL) {
					color_scheme = x->get_attribute((char*)"color_scheme");
				}

				if(x->get_attribute((char*)"shadow_type") != NULL) {
					shadow_type = (unsigned int)atoi(x->get_attribute((char*)"shadow_type"));
				}
			}
			else if(strcmp(x->get_node_name(), "key_repeat") == 0) {
				if(x->get_attribute((char*)"time") != NULL) {
					key_repeat = (unsigned int)atoi(x->get_attribute((char*)"time"));
				}
			}
			else if(strcmp(x->get_node_name(), "sleep") == 0) {
				if(x->get_attribute((char*)"time") != NULL) {
					fps_limit = (unsigned int)atoi(x->get_attribute((char*)"time"));
				}
			}
			else if(strcmp(x->get_node_name(), "thread") == 0) {
				if(x->get_attribute((char*)"count") != NULL) {
					thread_count = (unsigned int)atoi(x->get_attribute((char*)"count"));
					if(thread_count == 0) thread_count = omp_get_max_threads();
				}
			}
			else if(strcmp(x->get_node_name(), "server") == 0) {
				if(x->get_attribute((char*)"port") != NULL) {
					server->port = (unsigned short int)atoi(x->get_attribute((char*)"port"));
				}

				if(x->get_attribute((char*)"max_clients") != NULL) {
					server->max_clients = (unsigned int)atoi(x->get_attribute((char*)"max_clients"));
				}
			}
			else if(strcmp(x->get_node_name(), "client") == 0) {
				if(x->get_attribute((char*)"server") != NULL) {
					client->server_name = x->get_attribute((char*)"server");
				}

				if(x->get_attribute((char*)"port") != NULL) {
					client->port = (unsigned short int)atoi(x->get_attribute((char*)"port"));
				}

				if(x->get_attribute((char*)"lis_port") != NULL) {
					client->lis_port = (unsigned short int)atoi(x->get_attribute((char*)"lis_port"));
				}

				if(x->get_attribute((char*)"name") != NULL) {
					client->client_name = x->get_attribute((char*)"name");
				}
			}
			else if(strcmp(x->get_node_name(), "keyset") == 0) {
				if(x->get_attribute((char*)"set") != NULL) {
					keyset = x->get_attribute((char*)"set");
				}
			}
			else if(strcmp(x->get_node_name(), "graphic") == 0) {
				if(x->get_attribute((char*)"filtering") != NULL) {
					filtering = (unsigned int)atoi(x->get_attribute((char*)"filtering"));
				}

				if(x->get_attribute((char*)"hdr") != NULL) {
					hdr = atoi(x->get_attribute((char*)"hdr")) ? true : false;
				}

				if(x->get_attribute((char*)"hdr_rgba16") != NULL) {
					hdr_rgba16 = atoi(x->get_attribute((char*)"hdr_rgba16")) ? true : false;
				}
			}
		}

		x->close();
	}

	// load keyset
	e->load_keyset((char*)keyset.c_str());
}

/*! there is no function currently
 */
engine::~engine() {
	m->print(msg::MDEBUG, "engine.cpp", "freeing engine stuff");

	if(position != NULL) {
		delete position;
	}
	if(rotation != NULL) {
		delete rotation;
	}

	delete c;
	delete f;
	delete e;
	delete g;
	delete t;
	delete exts;
	delete l;
	delete x;
	delete gf;

	delete server;
	delete client;

	m->print(msg::MDEBUG, "engine.cpp", "engine stuff freed");

	delete m;

	SDL_Quit();
}

/*! initializes the engine in console + graphical or console only mode
 *  @param console the initialization mode (false = gfx/console, true = console only)
 *  @param width the window width
 *  @param height the window height
 *  @param depth the color depth of the window (16, 24 or 32)
 *  @param fullscreen bool if the window is drawn in fullscreen mode
 */
void engine::init(bool console, unsigned int width, unsigned int height, unsigned int depth, unsigned int zbuffer, unsigned int stencil, bool fullscreen, const char* ico) {
	if(console == true) {
	    engine::mode = engine::CONSOLE;
		// create extension class object
		exts = new ext(engine::mode, m);
		m->print(msg::MDEBUG, "engine.cpp", "initializing albion 2 engine in console only mode");
	}
	else {
		engine::width = width;
		engine::height = height;
		engine::depth = depth;
		engine::zbuffer = zbuffer;
		engine::stencil = stencil;
		engine::fullscreen = fullscreen;

		engine::init(ico);
	}
}

/*! initializes the engine in console + graphical mode
 */
void engine::init(const char* ico) {
	engine::mode = engine::GRAPHICAL;
	m->print(msg::MDEBUG, "engine.cpp", "initializing albion 2 engine in console + graphical mode");

	// initialize sdl
	atexit(SDL_Quit);
	if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_NOPARACHUTE) == -1) {
		m->print(msg::MERROR, "engine.cpp", "can't init SDL: %s", SDL_GetError());
		exit(1);
	}
	else {
		m->print(msg::MDEBUG, "engine.cpp", "sdl initialized");
	}

	// load opengl library
	SDL_GL_LoadLibrary(NULL);

	// get video info
	video_info = SDL_GetVideoInfo();
	if(!video_info) {
		m->print(msg::MDEBUG, "engine.cpp", "video query failed: %s", SDL_GetError());
		exit(1);
	}

	// not working any more?
	//m->print(msg::MDEBUG, "engine.cpp", "amount of available video memory: %u kb", video_info->video_mem);

	// set some flags
	engine::flags |= SDL_HWPALETTE;
	engine::flags |= SDL_OPENGL;
	engine::flags |= SDL_DOUBLEBUF;
	/*if(video_info->hw_available) {
		//engine::flags |= SDL_HWSURFACE;
		m->print(msg::MDEBUG, "engine.cpp", "using hardware surface");
	}
	else {
		//engine::flags |= SDL_SWSURFACE;
		m->print(msg::MDEBUG, "engine.cpp", "using software surface");
	}*/
	engine::flags |= SDL_HWSURFACE;

	if(fullscreen) {
		engine::flags |= SDL_FULLSCREEN;
		m->print(msg::MDEBUG, "engine.cpp", "fullscreen enabled");
	}
	else {
		m->print(msg::MDEBUG, "engine.cpp", "fullscreen disabled");
	}
	engine::flags |= SDL_HWACCEL;

	// gl attributes
	switch(depth) {
		case 16:
			SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
			SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 6);
			SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);
			break;
		case 24:
			SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
			SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
			SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
			break;
		case 32:
			SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
			SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
			SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
			break;
		default:
			if(depth < 16) {
				m->print(msg::MERROR, "engine.cpp", "init(): to low depth, please use at least a depth of 16bit!");
				return;
			}
			else { // depth > 16
				m->print(msg::MERROR, "engine.cpp", "init(): unknown depth of %ubit! engine will run in 16bit mode!", depth);
				depth = 16;
				SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
				SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 6);
				SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);
			}
			break;
	}

	SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, depth);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, zbuffer);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, stencil);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);

	// ... load icon before SDL_SetVideoMode
	if(ico != NULL) load_ico(ico);

	// create screen
	engine::height = height;
	engine::width = width;
	engine::depth = depth;
	screen = SDL_SetVideoMode(width, height, depth, flags);
	if(screen == NULL) {
		m->print(msg::MERROR, "engine.cpp", "can't set video mode: %s", SDL_GetError());
		exit(1);
	}
	else {
		m->print(msg::MDEBUG, "engine.cpp", "video mode set: w%u h%u d%u", width,
			height, depth);
	}

	int tmp = 0;
	SDL_GL_GetAttribute(SDL_GL_BUFFER_SIZE, &tmp);
	m->print(msg::MDEBUG, "engine.cpp", "color depth set to %d bits", tmp);
	SDL_GL_GetAttribute(SDL_GL_DEPTH_SIZE, &tmp);
	m->print(msg::MDEBUG, "engine.cpp", "z buffer set to %d bits", tmp);
	SDL_GL_GetAttribute(SDL_GL_STENCIL_SIZE, &tmp);
	m->print(msg::MDEBUG, "engine.cpp", "stencil buffer set to %d bits", tmp);
	SDL_GL_GetAttribute(SDL_GL_DOUBLEBUFFER, &tmp);
	m->print(msg::MDEBUG, "engine.cpp", "double buffering %s", tmp == 1 ? "enabled" : "disabled");

	g->set_surface(screen);

	// print out some opengl informations
	m->print(msg::MDEBUG, "engine.cpp", "vendor: %s", glGetString(GL_VENDOR));
	m->print(msg::MDEBUG, "engine.cpp", "renderer: %s", glGetString(GL_RENDERER));
	m->print(msg::MDEBUG, "engine.cpp", "version: %s", glGetString(GL_VERSION));

	// enable key repeat
	if((SDL_EnableKeyRepeat(key_repeat, SDL_DEFAULT_REPEAT_INTERVAL))) {
		m->print(msg::MDEBUG, "engine.cpp", "setting keyboard repeat failed: %s",
				SDL_GetError());
		exit(1);
	}
	else {
		m->print(msg::MDEBUG, "engine.cpp", "keyboard repeat set");
	}

	// initialize ogl
	init_gl();
	m->print(msg::MDEBUG, "engine.cpp", "opengl initialized");

	// resize stuff
	resize_window();

	// reserve memory for position and rotation ...
	engine::position = new vertex3();
	engine::rotation = new vertex3();

	// create extension class object
	exts = new ext(engine::mode, m);

	// set flip state of core class to true if fbos are supported
	if(exts->is_fbo_support()) {
		c->set_flip(true);
	}

	// create render to texture object
	r = new rtt(m, c, g, exts, screen->w, screen->h);

	// print out informations about additional threads
	omp_set_num_threads(thread_count);
	thread_count = omp_get_max_threads();
	thread_count == 1 ? m->print(msg::MDEBUG, "engine.cpp", "using one single thread!") :
		m->print(msg::MDEBUG, "engine.cpp", "using %u threads!", thread_count);

	// if GL_RENDERER is that damn m$ gdi driver, exit a2e ...
	// no official support for this crappy piece of software ...
	if(strcmp((const char*)glGetString(GL_RENDERER), "GDI Generic") == 0) {
		m->print(msg::MERROR, "engine.cpp", "A2E doesn't support the MS GDI Generic driver!\nGo and install one of these (that match your grapic card):\nhttp://www.ati.com  http://www.nvidia.com  http://www.matrox.com  http://www.3dlabs.com http://www.intel.com");
		SDL_Delay(10000);
		exit(1);
	}

	// set standard texture filtering
	t->set_filtering(engine::filtering);

	// check if hdrr is technically possible
	if(hdr && !exts->is_shader_support() && !exts->is_fbo_support()) {
		hdr = false;
	}
	m->print(msg::MDEBUG, "engine.cpp", "HDR-Rendering is %s!", (hdr ? "enabled" : "disabled"));
}

/*! sets the windows width
 *  @param width the window width
 */
void engine::set_width(unsigned int width) {
	engine::width = width;
	engine::screen = SDL_SetVideoMode(engine::width, engine::height, engine::depth, engine::flags);
	if(engine::screen == NULL) {
		m->print(msg::MERROR, "engine.cpp", "can't set video mode: %s", SDL_GetError());
		exit(1);
	}
}

/*! sets the window height
 *  @param height the window height
 */
void engine::set_height(unsigned int height) {
	engine::height = height;
	engine::screen = SDL_SetVideoMode(engine::width, engine::height, engine::depth, engine::flags);
	if(engine::screen == NULL) {
		m->print(msg::MERROR, "engine.cpp", "can't set video mode: %s", SDL_GetError());
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

	GLenum error = glGetError();
	switch(error) {
		case GL_NO_ERROR:
			break;
		case GL_INVALID_ENUM:
			m->print(msg::MERROR, "engine.cpp", "OpenGL error: invalid enum!", error);
			break;
		case GL_INVALID_VALUE:
			m->print(msg::MERROR, "engine.cpp", "OpenGL error: invalid value!", error);
			break;
		case GL_INVALID_OPERATION:
			m->print(msg::MERROR, "engine.cpp", "OpenGL error: invalid operation!", error);
			break;
		case GL_STACK_OVERFLOW:
			m->print(msg::MERROR, "engine.cpp", "OpenGL error: invalid stack overflow!", error);
			break;
		case GL_STACK_UNDERFLOW:
			m->print(msg::MERROR, "engine.cpp", "OpenGL error: invalid stack underflow!", error);
			break;
		case GL_OUT_OF_MEMORY:
			m->print(msg::MERROR, "engine.cpp", "OpenGL error: out of memory!", error);
			break;
		case GL_INVALID_FRAMEBUFFER_OPERATION_EXT:
			m->print(msg::MERROR, "engine.cpp", "OpenGL error: invalid framebuffer operation!", error);
			break;
		default:
			m->print(msg::MERROR, "engine.cpp", "unknown OpenGL error: %u!", error);
			break;
	}

	// fps "limiter"
	if(engine::fps_limit != 0) SDL_Delay(engine::fps_limit);
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
	SDL_WM_SetCaption(caption, caption);
}

/*! returns the window caption
 */
char* engine::get_caption() {
	char* caption;
	SDL_WM_GetCaption(&caption, NULL);
	return caption;
}

/*! opengl initialization function
 */
bool engine::init_gl() {
	// enable texture mapping
	glEnable(GL_TEXTURE_2D);
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

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glAlphaFunc(GL_GREATER, 0.1f);

	return true;
}

/*! opengl drawing code
 */
bool engine::draw_gl_scene() {
	// clear the color and depth buffers.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	// reset model view matrix
	glLoadIdentity();

	return true;
}

/* function to reset our viewport after a window resize
 */
bool engine::resize_window() {
	// set the viewport
	glViewport(0, 0, (GLsizei)engine::width, (GLsizei)engine::height);

	// projection matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// set perspective with fov = 72 and far value = 1000.0f
	gluPerspective(72.0f, (float)engine::width / (float)engine::height, 0.1f, 1000.0f);

	glGetFloatv(GL_PROJECTION_MATRIX, *projection_matrix.m);

	// model view matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glGetFloatv(GL_MODELVIEW_MATRIX, *modelview_matrix.m);

	return true;
}

/*! sets the position of the user/viewer
 *  @param xpos x coordinate
 *  @param ypos y coordinate
 *  @param zpos z coordinate
 */
void engine::set_position(float xpos, float ypos, float zpos) {
	engine::position->x = xpos;
	engine::position->y = ypos;
	engine::position->z = zpos;

	glTranslatef(engine::position->x, engine::position->y, engine::position->z);

	if(exts->is_fbo_support()) engine::position->y *= -1.0f;

	glGetFloatv(GL_MODELVIEW_MATRIX, *modelview_matrix.m);
}

/*! sets the rotation of the user/viewer (this function doesn't affect the cam/users rotation - use the cam class's function instead)
 *  @param xrot x rotation
 *  @param yrot y rotation
 */
void engine::set_rotation(float xrot, float yrot) {
	engine::rotation->x = xrot;
	engine::rotation->y = yrot;
}

/*! returns the position of the user
 */
vertex3* engine::get_position() {
	return engine::position;
}

/*! returns the rotation of the user
 */
vertex3* engine::get_rotation() {
	return engine::rotation;
}

/*! starts drawing the 2d elements and initializes the opengl functions for that
 */
void engine::start_2d_draw() {
	glPushAttrib(GL_ENABLE_BIT);

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
	glDisable(GL_BLEND);

	glViewport(0, 0, screen->w, screen->h);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	// we need an orthogonal view (2d) for drawing 2d elements
	glOrtho(0, screen->w, screen->h, 0, -1.0, 1.0);

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

	glPopAttrib();
}

/*! sets the cursors visibility to state
 *  @param state the cursor visibility state
 */
void engine::set_cursor_visible(bool state) {
	engine::cursor_visible = state;
	SDL_ShowCursor(engine::cursor_visible);
}

/*! returns the cursor visibility state
 */
bool engine::get_cursor_visible() {
	return engine::cursor_visible;
}

/*! sets the fps limit (max. fps = 1000 / ms)
 *! note that a value of 0 increases the cpu usage to 100%
 *  @param ms how many milliseconds the engine should "sleep" after a frame is rendered
 */
void engine::set_fps_limit(unsigned int ms) {
	engine::fps_limit = ms;
}

/*! returns how many milliseconds the engine is "sleeping" after a frame is rendered
 */
unsigned int engine::get_fps_limit() {
	return engine::fps_limit;
}

/*! returns the type of the initialization (0 = GRAPHICAL, 1 = CONSOLE)
 */
unsigned int engine::get_init_mode() {
	return engine::mode;
}

/*! returns a pointer to the core class
 */
core* engine::get_core() {
	return engine::c;
}

/*! returns a pointer to the msg class
 */
msg* engine::get_msg() {
	return engine::m;
}

/*! returns a pointer to the file_io class
 */
file_io* engine::get_file_io() {
	return engine::f;
}

/*! returns a pointer to the file_io class
 */
event* engine::get_event() {
	return engine::e;
}

/*! returns the gfx class
 */
gfx* engine::get_gfx() {
	return engine::g;
}

/*! returns the texman class
 */
texman* engine::get_texman() {
	return engine::t;
}

/*! returns the extensions class
 */
ext* engine::get_ext() {
	return engine::exts;
}

/*! returns the lua class
 */
lua* engine::get_lua() {
	return engine::l;
}

/*! returns the gui_font class
 */
gui_font* engine::get_gui_font() {
	return engine::gf;
}

/*! returns the xml class
 */
xml* engine::get_xml() {
	return engine::x;
}

/*! returns the rtt class
 */
rtt* engine::get_rtt() {
	return engine::r;
}

/*! returns the color scheme
 */
string* engine::get_color_scheme() {
	return &(engine::color_scheme);
}

/*! returns the number/amount of additional threads (besides the main thread)
 */
unsigned int engine::get_thread_count() {
	return engine::thread_count;
}

/*! sets the data path
 *  @param data_path the data path
 */
void engine::set_data_path(const char* data_path) {
	engine::datapath = data_path;
}

/*! returns the data path
 */
string engine::get_data_path() {
	return engine::datapath;
}

/*! returns the data path + str
 *  @param str str we want to "add" to the data path
 */
char* engine::data_path(const char* str) {
	if(str == NULL || strcmp(str, "") == 0) tmp_str = engine::datapath;
	else tmp_str = engine::datapath + str;
	return (char*)tmp_str.c_str();
}

engine::server_data* engine::get_server_data() {
	return engine::server;
}

engine::client_data* engine::get_client_data() {
	return engine::client;
}

void engine::load_ico(const char* ico) {
	SDL_WM_SetIcon(IMG_Load(data_path(ico)), NULL);
}

unsigned int engine::get_shadow_type() {
	return engine::shadow_type;
}

unsigned int engine::get_filtering() {
	return engine::filtering;
}

bool engine::get_hdr() {
	return engine::hdr;
}

bool engine::get_hdr_rgba16() {
	return engine::hdr_rgba16;
}

matrix4* engine::get_projection_matrix() {
	return &(engine::projection_matrix);
}

matrix4* engine::get_modelview_matrix() {
	return &(engine::modelview_matrix);
}
