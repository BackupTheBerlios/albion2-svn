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
	bool is_multitexture_support();
	bool is_texenv_combine_support();
	bool is_shader_support();
	bool is_vbo_support();

	PFNGLACTIVETEXTUREARBPROC glActiveTextureARB;
	PFNGLCLIENTACTIVETEXTUREARBPROC glClientActiveTextureARB;
	PFNGLMULTITEXCOORD2FARBPROC glMultiTexCoord2fARB;
	PFNGLMULTITEXCOORD3FARBPROC glMultiTexCoord3fARB;

	PFNGLENABLEVERTEXATTRIBARRAYARBPROC glEnableVertexAttribArrayARB;
	PFNGLVERTEXATTRIBPOINTERARBPROC glVertexAttribPointerARB;

	PFNGLBINDBUFFERARBPROC glBindBufferARB;
	PFNGLBUFFERDATAARBPROC glBufferDataARB;
	PFNGLBUFFERSUBDATAARBPROC glBufferSubDataARB;
	PFNGLDELETEBUFFERSARBPROC glDeleteBuffersARB;
	PFNGLGENBUFFERSARBPROC glGenBuffersARB;
	PFNGLMAPBUFFERARBPROC glMapBufferARB;
	PFNGLUNMAPBUFFERARBPROC glUnmapBufferARB;
	PFNGLISBUFFERARBPROC glIsBufferARB;

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

};

#endif
