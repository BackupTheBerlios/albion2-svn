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

#include "lua.h"

using namespace std;

msg* lua::m = NULL;
gfx* lua::g = NULL;
core* lua::c = NULL;

vertex3* lua::v1 = NULL;
vertex3* lua::v2 = NULL;
gfx::rect* lua::r1 = NULL;
core::pnt* lua::p1 = NULL;
core::pnt* lua::p2 = NULL;

/*! create and initialize the lua class
 */
lua::lua(msg* m) {
	l = lua_open();

	luaopen_base(l);
	luaopen_table(l);
	luaopen_io(l);
	luaopen_string(l);
	luaopen_math(l);
	luaopen_debug(l);
	luaopen_loadlib(l);

    lua::v1 = new vertex3();
    lua::v2 = new vertex3();
    lua::r1 = new gfx::rect();
    lua::p1 = new core::pnt();
    lua::p2 = new core::pnt();

	lua::m = m;
	lua::g = NULL;
	lua::c = NULL;
}

/*! delete everything
 */
lua::~lua() {
    delete lua::r1;
    delete lua::p1;
    delete lua::p2;

    delete lua::v1;
    delete lua::v2;

	lua_close(l);
}

/*! runs the script specified by filename
 *  @param filename the scripts file name
 */
void lua::run_script(char* filename) {
	lua_dofile(l, filename);
}

/*! registers a c/c++ function in lua (so it can be called within a lua script)
 *  @param func_name the functions name (in lua)
 *  @param func a pointer to the function
 */
void lua::register_function(char* func_name, lua_CFunction func) {
	lua_register(l, func_name, func);
}

/*! returns the lua state
 */
lua_State* lua::get_lua_state() {
	return l;
}

/*! sets the reference pointer to the gfx class
 *  @param g a pointer to a gfx class object
 */
void lua::set_gfx(gfx* g) {
	lua::g = g;
}

/*! sets the reference pointer to the core class
 *  @param c a pointer to a core class object
 */
void lua::set_core(core* c) {
	lua::c = c;
}

/*! registers a2e gfx class functions in lua
 */
void lua::load_gfx() {
	if(lua::g != NULL) {
		register_function("draw_point", draw_point);
		register_function("draw_line", draw_line);
		register_function("draw_3d_line", draw_3d_line);
		register_function("draw_rectangle", draw_rectangle);
		register_function("draw_2colored_rectangle", draw_2colored_rectangle);
		register_function("draw_filled_rectangle", draw_filled_rectangle);
	}
	else {
		m->print(msg::MERROR, "lua.cpp", "load_gfx(): no gfx class specified yet!");
	}
}

/*! registers a2e core class functions in lua
 */
void lua::load_core() {
	if(lua::c != NULL) {
	}
	else {
		m->print(msg::MERROR, "lua.cpp", "load_core(): no core class specified yet!");
	}
}

/*! checks the overgiven arguments types
 *  @param l a pointer to the lua state
 *  @param carg amount of arguments
 *  @param arg_types the type of the arguments
 */
bool lua::check_args(lua_State* l, int carg, LARG_TYPE* arg_types, char* func_name) {
	// get number of arguments
	int n = lua_gettop(l);

	// if we don't have the sufficient amount of arguments, return false
	if(n < carg) {
		m->print(msg::MERROR, "lua.cpp", "check_args(): not sufficient amount of arguments!");
		m->print(msg::MERROR, "lua.cpp", "%s(): encountered argument problem!", func_name);
		return false;
	}

	for(int i = 0; i < carg; i++) {
		// i+1, b/c lua argument indexing starts at 1
		if(!check_type(l, (i+1), arg_types[i])) {
			m->print(msg::MERROR, "lua.cpp", "check_args(): encountered a wrong arg type - aborting function!");
			m->print(msg::MERROR, "lua.cpp", "%s(): encountered argument problem!", func_name);
			return false;
		}
	}

	// everything is ok, return true
	return true;
}

/*! checks if num is of the correct type
 *  @param l a pointer to the lua state
 *  @param num the number/argument/variable we want to check
 *  @param type the type of the argument
 */
bool lua::check_type(lua_State* l, int num, LARG_TYPE type) {
	switch(type) {
		case lua::NIL:
			if(!lua_isnil(l, num)) {
				m->print(msg::MERROR, "lua.cpp", "check_type(): arg #%i is of the wrong type (should be NIL)!", num);
				return false;
			}
			break;
		case lua::BOOLEAN:
			if(!lua_isboolean(l, num)) {
				m->print(msg::MERROR, "lua.cpp", "check_type(): arg #%i is of the wrong type (should be BOOLEAN)!", num);
				return false;
			}
			break;
		case lua::INT_NUMBER:
		case lua::FLOAT_NUMBER:
			if(!lua_isnumber(l, num)) {
				m->print(msg::MERROR, "lua.cpp", "check_type(): arg #%i is of the wrong type (should be NUMBER)!", num);
				return false;
			}
			break;
		case lua::STRING:
			if(!lua_isstring(l, num)) {
				m->print(msg::MERROR, "lua.cpp", "check_type(): arg #%i is of the wrong type (should be STRING)!", num);
				return false;
			}
			break;
		default:
			m->print(msg::MERROR, "lua.cpp", "check_type(): unknown type to check!");
			return false;
			break;
	}

	// everything is ok, return true
	return true;
}

/*bool lua::get_args(lua_State* l, int carg, LARG_TYPE* arg_types, void* args) {
	for(int i = 0; i < carg; i++) {
		switch(arg_types[i]) {
			case lua::BOOLEAN: {
				int tmp = lua_toboolean(l, i+1);
				//memcpy(&args[i], &tmp, 4);
				memmove(&args[i], &tmp, 4);
				// *args[i] = (bool)lua_toboolean(l, i+1);
			}
			break;
			default:
				break;
		}
	}

	// everything is ok, return true
	return true;
}*/


/*
------------------------------------------------------------------------------------
- begin of Lua function "bindings"                                                 -
------------------------------------------------------------------------------------
*/

/*int lua::template_func(lua_State* l) {
	LARG_TYPE types[] = { lua::ARG_TYPE, lua::ARG_TYPE };
	if(!check_args(l, arg_count, types)) {
		m->print(msg::MERROR, "lua.cpp", "template_func(): encountered argument problem!");
		return 0;
	}

	// get arguments
	const char* arg1 = lua_tostring(l, 1);
	const char* arg2 = lua_tostring(l, 2);

	// call the function
	func->blah(arg1, arg2);

	// return the number of results
	return 0;
}*/

int lua::draw_point(lua_State* l) {
	LARG_TYPE types[] = { lua::INT_NUMBER, lua::INT_NUMBER, lua::INT_NUMBER };
	if(!check_args(l, 3, types, "draw_point")) {
		return 0;
	}

	// get arguments
	p1->x = (unsigned int)lua_tonumber(l, 1);
	p1->y = (unsigned int)lua_tonumber(l, 2);
	unsigned int color = (unsigned int)lua_tonumber(l, 3);

	// call the function
	g->draw_point(p1, g->get_color(color));

	// return the number of results
	return 0;
}

int lua::draw_line(lua_State* l) {
	LARG_TYPE types[] = { lua::INT_NUMBER, lua::INT_NUMBER, lua::INT_NUMBER, lua::INT_NUMBER, lua::INT_NUMBER };
	if(!check_args(l, 5, types, "draw_line")) {
		return 0;
	}

	// get arguments
	p1->x = (unsigned int)lua_tonumber(l, 1);
	p1->y = (unsigned int)lua_tonumber(l, 2);
	p2->x = (unsigned int)lua_tonumber(l, 3);
	p2->y = (unsigned int)lua_tonumber(l, 4);
	unsigned color = (unsigned int)lua_tonumber(l, 5);

	// call the function
	g->draw_line(p1, p2, g->get_color(color));

	// return the number of results
	return 0;
}

int lua::draw_3d_line(lua_State* l) {
	LARG_TYPE types[] = { lua::FLOAT_NUMBER, lua::FLOAT_NUMBER, lua::FLOAT_NUMBER, lua::FLOAT_NUMBER,
							lua::FLOAT_NUMBER, lua::FLOAT_NUMBER, lua::FLOAT_NUMBER };
	if(!check_args(l, 7, types, "draw_3d_line")) {
		return 0;
	}

	// get arguments
	v1->x = (float)lua_tonumber(l, 1);
	v1->y = (float)lua_tonumber(l, 2);
	v1->z = (float)lua_tonumber(l, 3);
	v2->x = (float)lua_tonumber(l, 4);
	v2->y = (float)lua_tonumber(l, 5);
	v2->z = (float)lua_tonumber(l, 6);
	unsigned int color = (unsigned int)lua_tonumber(l, 7);

	// call the function
	g->draw_3d_line(v1, v2, g->get_color(color));

	// return the number of results
	return 0;
}

int lua::draw_rectangle(lua_State* l) {
	LARG_TYPE types[] = { lua::INT_NUMBER, lua::INT_NUMBER, lua::INT_NUMBER, lua::INT_NUMBER, lua::INT_NUMBER };
	if(!check_args(l, 5, types, "draw_rectangle")) {
		return 0;
	}

	// get arguments
	r1->x1 = (unsigned int)lua_tonumber(l, 1);
	r1->y1 = (unsigned int)lua_tonumber(l, 2);
	r1->x2 = (unsigned int)lua_tonumber(l, 3);
	r1->y2 = (unsigned int)lua_tonumber(l, 4);
	unsigned color = (unsigned int)lua_tonumber(l, 5);

	// call the function
	g->draw_rectangle(r1, g->get_color(color));

	// return the number of results
	return 0;
}

int lua::draw_filled_rectangle(lua_State* l) {
	LARG_TYPE types[] = { lua::INT_NUMBER, lua::INT_NUMBER, lua::INT_NUMBER, lua::INT_NUMBER, lua::INT_NUMBER };
	if(!check_args(l, 5, types, "draw_filled_rectangle")) {
		return 0;
	}

	// get arguments
	r1->x1 = (unsigned int)lua_tonumber(l, 1);
	r1->y1 = (unsigned int)lua_tonumber(l, 2);
	r1->x2 = (unsigned int)lua_tonumber(l, 3);
	r1->y2 = (unsigned int)lua_tonumber(l, 4);
	unsigned color = (unsigned int)lua_tonumber(l, 5);

	// call the function
	g->draw_filled_rectangle(r1, g->get_color(color));

	// return the number of results
	return 0;
}

int lua::draw_2colored_rectangle(lua_State* l) {
	LARG_TYPE types[] = { lua::INT_NUMBER, lua::INT_NUMBER, lua::INT_NUMBER,
							lua::INT_NUMBER, lua::INT_NUMBER, lua::INT_NUMBER };
	if(!check_args(l, 6, types, "draw_2colored_rectangle")) {
		return 0;
	}

	// get arguments
	r1->x1 = (unsigned int)lua_tonumber(l, 1);
	r1->y1 = (unsigned int)lua_tonumber(l, 2);
	r1->x2 = (unsigned int)lua_tonumber(l, 3);
	r1->y2 = (unsigned int)lua_tonumber(l, 4);
	unsigned color1 = (unsigned int)lua_tonumber(l, 5);
	unsigned color2 = (unsigned int)lua_tonumber(l, 6);

	// call the function
	g->draw_2colored_rectangle(r1, g->get_color(color1), g->get_color(color2));

	// return the number of results
	return 0;
}
