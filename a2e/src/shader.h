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
 
#ifndef __SHADER_H__
#define __SHADER_H__

#ifdef WIN32
#include <windows.h>
#include <winnt.h>
#endif

#include <iostream>
#include <SDL/SDL.h>
#include <cmath>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glext.h>

#ifdef WIN32
#include <GL/wglext.h>
#else
#include <GL/glx.h>
#endif

#include "engine.h"
#include "extensions.h"
using namespace std;

#include "win_dll_export.h"

/*! @class shader
 *  @brief shader class
 *  @author flo
 *  @todo more functions
 *  
 *  the glsl shader class
 */

class A2E_API shader
{
protected:
	struct shader_object;

public:

	shader(engine* e);
	~shader();

	unsigned int add_shader(char* vname, char* fname, unsigned int cuniforms, char** uniforms, unsigned int cattributes, char** attributes);
	void use_shader(unsigned int num);
	shader_object* get_shader_object(unsigned int num);

	void set_uniform1i(unsigned int num, int var1);
	void set_uniform2i(unsigned int num, int var1, int var2);
	void set_uniform3i(unsigned int num, int var1, int var2, int var3);
	void set_uniform4i(unsigned int num, int var1, int var2, int var3, int var4);
	void set_uniform1iv(unsigned int num, int* var, unsigned int count = 1);
	void set_uniform2iv(unsigned int num, int* var, unsigned int count = 1);
	void set_uniform3iv(unsigned int num, int* var, unsigned int count = 1);
	void set_uniform4iv(unsigned int num, int* var, unsigned int count = 1);

	void set_uniform1f(unsigned int num, float var1);
	void set_uniform2f(unsigned int num, float var1, float var2);
	void set_uniform3f(unsigned int num, float var1, float var2, float var3);
	void set_uniform4f(unsigned int num, float var1, float var2, float var3, float var4);
	void set_uniform1fv(unsigned int num, float* var, unsigned int count = 1);
	void set_uniform2fv(unsigned int num, float* var, unsigned int count = 1);
	void set_uniform3fv(unsigned int num, float* var, unsigned int count = 1);
	void set_uniform4fv(unsigned int num, float* var, unsigned int count = 1);

	void set_uniform_matrix2fv(unsigned int num, float* var, unsigned int count = 1, bool transpose = false);
	void set_uniform_matrix3fv(unsigned int num, float* var, unsigned int count = 1, bool transpose = false);
	void set_uniform_matrix4fv(unsigned int num, float* var, unsigned int count = 1, bool transpose = false);

	void set_attribute4iv(unsigned int num, int* var);

	void set_attribute1f(unsigned int num, float var1);
	void set_attribute2f(unsigned int num, float var1, float var2);
	void set_attribute3f(unsigned int num, float var1, float var2, float var3);
	void set_attribute4f(unsigned int num, float var1, float var2, float var3, float var4);
	void set_attribute1fv(unsigned int num, float* var);
	void set_attribute2fv(unsigned int num, float* var);
	void set_attribute3fv(unsigned int num, float* var);
	void set_attribute4fv(unsigned int num, float* var);

	enum INTERNAL_SHADERS {
		NONE,
		PARALLAX,
		PARALLAX_HDR,
		PHONG,
		PHONG_HDR,
		BLUR3X3,
		BLUR5X5,
		BLURLINE3,
		BLURLINE5,
		SKYBOX_HDR,
		CONVERT_HDR,
		AVERAGE_HDR,
		EXPOSURE_HDR,
		HDR
	};


protected:
	engine* e;
	core* c;
	msg* m;
	file_io* f;
	ext* exts;

	struct shader_object {
		GLhandleARB program;
		GLhandleARB vertex_shader;
		GLhandleARB fragment_shader;
		unsigned int cuniforms;
		unsigned int cattributes;
		int* uniforms;
		int* attributes;
	};

	unsigned int cshaders;
	shader_object* shaders;

	bool shader_support;

	unsigned int cur_shader;

};

#endif
