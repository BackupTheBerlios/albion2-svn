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
//#include <GL/glut.h>
#include <GL/glext.h>

#ifdef WIN32
#include <GL/wglext.h>
#else
#include <GL/glx.h>
#endif

#include "engine.h"
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

	PFNGLPROGRAMSTRINGARBPROC glProgramStringARB;
	PFNGLBINDPROGRAMARBPROC glBindProgramARB;
	PFNGLDELETEPROGRAMSARBPROC glDeleteProgramsARB;
	PFNGLGENPROGRAMSARBPROC glGenProgramsARB;
	PFNGLPROGRAMLOCALPARAMETER4FARBPROC glProgramLocalParameter4fARB;
	PFNGLPROGRAMLOCALPARAMETER4FVARBPROC glProgramLocalParameter4fvARB;

	PFNGLDELETEOBJECTARBPROC glDeleteObjectARB;
	PFNGLGETHANDLEARBPROC glGetHandleARB;
	PFNGLDETACHOBJECTARBPROC glDetachObjectARB;
	PFNGLCREATESHADEROBJECTARBPROC glCreateShaderObjectARB;
	PFNGLSHADERSOURCEARBPROC glShaderSourceARB;
	PFNGLCOMPILESHADERARBPROC glCompileShaderARB;
	PFNGLCREATEPROGRAMOBJECTARBPROC glCreateProgramObjectARB;
	PFNGLATTACHOBJECTARBPROC glAttachObjectARB;
	PFNGLLINKPROGRAMARBPROC glLinkProgramARB;
	PFNGLVALIDATEPROGRAMARBPROC glValidateProgramARB;
	PFNGLUSEPROGRAMOBJECTARBPROC glUseProgramObjectARB;
	PFNGLGETOBJECTPARAMETERIVARBPROC glGetObjectParameterivARB;
	PFNGLGETINFOLOGARBPROC glGetInfoLogARB;

	PFNGLGETUNIFORMLOCATIONARBPROC glGetUniformLocationARB;
	PFNGLGETATTRIBLOCATIONARBPROC glGetAttribLocationARB;

	PFNGLUNIFORM1IARBPROC glUniform1iARB;
	PFNGLUNIFORM2IARBPROC glUniform2iARB;
	PFNGLUNIFORM3IARBPROC glUniform3iARB;
	PFNGLUNIFORM4IARBPROC glUniform4iARB;
	PFNGLUNIFORM1IVARBPROC glUniform1ivARB;
	PFNGLUNIFORM2IVARBPROC glUniform2ivARB;
	PFNGLUNIFORM3IVARBPROC glUniform3ivARB;
	PFNGLUNIFORM4IVARBPROC glUniform4ivARB;

	PFNGLUNIFORM1FARBPROC glUniform1fARB;
	PFNGLUNIFORM2FARBPROC glUniform2fARB;
	PFNGLUNIFORM3FARBPROC glUniform3fARB;
	PFNGLUNIFORM4FARBPROC glUniform4fARB;
	PFNGLUNIFORM1FVARBPROC glUniform1fvARB;
	PFNGLUNIFORM2FVARBPROC glUniform2fvARB;
	PFNGLUNIFORM3FVARBPROC glUniform3fvARB;
	PFNGLUNIFORM4FVARBPROC glUniform4fvARB;

	PFNGLVERTEXATTRIB4IVARBPROC glVertexAttrib4ivARB;

	PFNGLVERTEXATTRIB1FARBPROC glVertexAttrib1fARB;
	PFNGLVERTEXATTRIB2FARBPROC glVertexAttrib2fARB;
	PFNGLVERTEXATTRIB3FARBPROC glVertexAttrib3fARB;
	PFNGLVERTEXATTRIB4FARBPROC glVertexAttrib4fARB;
	PFNGLVERTEXATTRIB1FVARBPROC glVertexAttrib1fvARB;
	PFNGLVERTEXATTRIB2FVARBPROC glVertexAttrib2fvARB;
	PFNGLVERTEXATTRIB3FVARBPROC glVertexAttrib3fvARB;
	PFNGLVERTEXATTRIB4FVARBPROC glVertexAttrib4fvARB;

	shader(engine* e);
	~shader();

	void init_extensions();
	unsigned int add_shader(char* vname, char* fname, unsigned int cuniforms, char** uniforms, unsigned int cattributes, char** attributes);
	bool is_shader_support();
	void use_shader(unsigned int num);
	shader_object* get_shader_object(unsigned int num);

	void set_uniform1i(unsigned int num, int var1);
	void set_uniform2i(unsigned int num, int var1, int var2);
	void set_uniform3i(unsigned int num, int var1, int var2, int var3);
	void set_uniform4i(unsigned int num, int var1, int var2, int var3, int var4);
	void set_uniform1iv(unsigned int num, int* var);
	void set_uniform2iv(unsigned int num, int* var);
	void set_uniform3iv(unsigned int num, int* var);
	void set_uniform4iv(unsigned int num, int* var);

	void set_uniform1f(unsigned int num, float var1);
	void set_uniform2f(unsigned int num, float var1, float var2);
	void set_uniform3f(unsigned int num, float var1, float var2, float var3);
	void set_uniform4f(unsigned int num, float var1, float var2, float var3, float var4);
	void set_uniform1fv(unsigned int num, float* var);
	void set_uniform2fv(unsigned int num, float* var);
	void set_uniform3fv(unsigned int num, float* var);
	void set_uniform4fv(unsigned int num, float* var);

	void set_attribute4iv(unsigned int num, int* var);

	void set_attribute1f(unsigned int num, float var1);
	void set_attribute2f(unsigned int num, float var1, float var2);
	void set_attribute3f(unsigned int num, float var1, float var2, float var3);
	void set_attribute4f(unsigned int num, float var1, float var2, float var3, float var4);
	void set_attribute1fv(unsigned int num, float* var);
	void set_attribute2fv(unsigned int num, float* var);
	void set_attribute3fv(unsigned int num, float* var);
	void set_attribute4fv(unsigned int num, float* var);


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
