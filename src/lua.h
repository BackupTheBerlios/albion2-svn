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

#ifndef __LUA_H__
#define __LUA_H__

#ifdef WIN32
#include <windows.h>
#include <winnt.h>
#endif

#include <iostream>
#include <cmath>
#include <SDL/SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>
extern "C" {
#include <lua/lua.h>
#include <lua/lualib.h>
#include <lua/lauxlib.h>
}
#include "msg.h"
#include "core.h"
#include "gfx.h"

#include "win_dll_export.h"
using namespace std;

/*! @class lua
 *  @brief lua "bindings"
 *  @author flo
 *
 *  all the lua stuff needed by the engine and any other application ...
 */

class A2E_API lua
{
public:
	lua(msg* m);
	~lua();

	void run_script(char* filename);
	void register_function(char* func_name, lua_CFunction func);

	lua_State* get_lua_state();

	// class referencing functions
	void set_gfx(gfx* g);
	void set_core(core* c);

	// functions for calling a2e class functions from lua
	void load_gfx();
	void load_core();

	// begin of Lua function "bindings"
	static int draw_point(lua_State* l);
	static int draw_line(lua_State* l);
	static int draw_3d_line(lua_State* l);
	static int draw_rectangle(lua_State* l);
	static int draw_2colored_rectangle(lua_State* l);
	static int draw_filled_rectangle(lua_State* l);

	// checking functions
	enum LARG_TYPE {
		NIL,
		BOOLEAN,
		INT_NUMBER,
		FLOAT_NUMBER,
		STRING
	};
	static bool check_args(lua_State* l, int carg, LARG_TYPE* arg_types);
	static bool check_type(lua_State* l, int num, LARG_TYPE type);
	//static bool get_args(lua_State* l, int carg, LARG_TYPE* arg_types, void* args);

protected:
	lua_State* l;

	static msg* m;
	static gfx* g;
	static core* c;

	// temp variables - needed for faster use of some functions
	// -> we don't need to create a temp variable of these types
	// each time we need one
	static vertex3* v1;
	static vertex3* v2;
	static gfx::rect* r1;
	static core::pnt* p1;
	static core::pnt* p2;


};

#endif
