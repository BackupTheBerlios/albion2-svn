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

#ifndef __EXTENSIONS_H__
#define __EXTENSIONS_H__

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
#include "msg.h"

#ifdef WIN32
#include <GL/wglext.h>
#else
#define GLX_GLXEXT_LEGACY
#include <GL/glx.h>
#endif

#include "win_dll_export.h"
using namespace std;

/*! @class ext
 *  @brief opengl extensions
 *  @author flo
 *
 *  here you can find all opengl extension definitions
 */

class A2E_API ext
{
public:
	ext(unsigned int imode, msg* m);
	~ext();

	bool is_ext_supported(char* ext_name);
	bool is_gl_version(unsigned int major, unsigned int minor);
	bool is_multitexture_support();
	bool is_texenv_combine_support();
	bool is_shader_support();
	bool is_vbo_support();
	bool is_fbo_support();
	bool is_blend_func_separate_support();

	PFNGLACTIVETEXTUREARBPROC glActiveTextureARB;
	PFNGLCLIENTACTIVETEXTUREARBPROC glClientActiveTextureARB;
	PFNGLMULTITEXCOORD2FARBPROC glMultiTexCoord2fARB;
	PFNGLMULTITEXCOORD3FARBPROC glMultiTexCoord3fARB;

	PFNGLENABLEVERTEXATTRIBARRAYARBPROC glEnableVertexAttribArrayARB;
	PFNGLVERTEXATTRIBPOINTERARBPROC glVertexAttribPointerARB;
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
	PFNGLUNIFORMMATRIX2FVARBPROC glUniformMatrix2fv;
	PFNGLUNIFORMMATRIX3FVARBPROC glUniformMatrix3fv;
	PFNGLUNIFORMMATRIX4FVARBPROC glUniformMatrix4fv;
	PFNGLVERTEXATTRIB4IVARBPROC glVertexAttrib4ivARB;
	PFNGLVERTEXATTRIB1FARBPROC glVertexAttrib1fARB;
	PFNGLVERTEXATTRIB2FARBPROC glVertexAttrib2fARB;
	PFNGLVERTEXATTRIB3FARBPROC glVertexAttrib3fARB;
	PFNGLVERTEXATTRIB4FARBPROC glVertexAttrib4fARB;
	PFNGLVERTEXATTRIB1FVARBPROC glVertexAttrib1fvARB;
	PFNGLVERTEXATTRIB2FVARBPROC glVertexAttrib2fvARB;
	PFNGLVERTEXATTRIB3FVARBPROC glVertexAttrib3fvARB;
	PFNGLVERTEXATTRIB4FVARBPROC glVertexAttrib4fvARB;

	PFNGLBINDBUFFERARBPROC glBindBufferARB;
	PFNGLBUFFERDATAARBPROC glBufferDataARB;
	PFNGLBUFFERSUBDATAARBPROC glBufferSubDataARB;
	PFNGLDELETEBUFFERSARBPROC glDeleteBuffersARB;
	PFNGLGENBUFFERSARBPROC glGenBuffersARB;
	PFNGLMAPBUFFERARBPROC glMapBufferARB;
	PFNGLUNMAPBUFFERARBPROC glUnmapBufferARB;
	PFNGLISBUFFERARBPROC glIsBufferARB;

	PFNGLISRENDERBUFFEREXTPROC glIsRenderbufferEXT;
	PFNGLBINDRENDERBUFFEREXTPROC glBindRenderbufferEXT;
	PFNGLDELETERENDERBUFFERSEXTPROC glDeleteRenderbuffersEXT;
	PFNGLGENRENDERBUFFERSEXTPROC glGenRenderbuffersEXT;
	PFNGLRENDERBUFFERSTORAGEEXTPROC glRenderbufferStorageEXT;
	PFNGLGETRENDERBUFFERPARAMETERIVEXTPROC glGetRenderbufferParameterivEXT;
	PFNGLISFRAMEBUFFEREXTPROC glIsFramebufferEXT;
	PFNGLBINDFRAMEBUFFEREXTPROC glBindFramebufferEXT;
	PFNGLDELETEFRAMEBUFFERSEXTPROC glDeleteFramebuffersEXT;
	PFNGLGENFRAMEBUFFERSEXTPROC glGenFramebuffersEXT;
	PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC glCheckFramebufferStatusEXT;
	PFNGLFRAMEBUFFERTEXTURE1DEXTPROC glFramebufferTexture1DEXT;
	PFNGLFRAMEBUFFERTEXTURE2DEXTPROC glFramebufferTexture2DEXT;
	PFNGLFRAMEBUFFERTEXTURE3DEXTPROC glFramebufferTexture3DEXT;
	PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC glFramebufferRenderbufferEXT;
	PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVEXTPROC glGetFramebufferAttachmentParameterivEXT;
	PFNGLGENERATEMIPMAPEXTPROC glGenerateMipmapEXT;

	PFNGLBLENDFUNCSEPARATEEXTPROC glBlendFuncSeparateEXT;


	// - taken from engine.h -
    // the initialization mode is used to determine if we should load
    // or compute graphical stuff like textures or shaders
	enum INIT_MODE {
	    GRAPHICAL,
	    CONSOLE
	};

protected:
	unsigned int mode;
	msg* m;

	bool multitexture_support;
	bool texenv_combine_support;
	bool shader_support;
	bool vbo_support;
	bool fbo_support;
	bool blend_func_separate_support;

};

#endif
