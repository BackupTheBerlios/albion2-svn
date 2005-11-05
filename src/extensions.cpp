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
 
#include "extensions.h"

#ifdef WIN32
#define glGetProcAddress(x) wglGetProcAddress(x)
#define ProcType LPCSTR
#else
#define glGetProcAddress(x) glXGetProcAddress(x)
#define ProcType GLubyte*
#endif

// mac os x: #define glGetProcAddress(x) aglGetProcAddress(x) (?)

using namespace std;

/*! create and initialize the extension class
 */
ext::ext(unsigned int imode) {
	ext::mode = imode;

	glActiveTextureARB = NULL;
	glMultiTexCoord3fARB = NULL;

	if(is_ext_supported("GL_ARB_multitexture")) {
		glActiveTextureARB = (PFNGLACTIVETEXTUREARBPROC)glGetProcAddress((ProcType)"glActiveTextureARB");
		glMultiTexCoord3fARB = (PFNGLMULTITEXCOORD3FARBPROC)glGetProcAddress((ProcType)"glMultiTexCoord3fARB");
	}

	if(is_ext_supported("GL_ARB_fragment_shader") &&
		is_ext_supported("GL_ARB_vertex_shader") &&
		is_ext_supported("GL_ARB_shader_objects") &&
		is_ext_supported("GL_ARB_shading_language_100")) {
		glEnableVertexAttribArrayARB = (PFNGLENABLEVERTEXATTRIBARRAYARBPROC)glGetProcAddress((ProcType)"glEnableVertexAttribArrayARB");
		glVertexAttribPointerARB = (PFNGLVERTEXATTRIBPOINTERARBPROC)glGetProcAddress((ProcType)"glVertexAttribPointerARB");
	}
}

/*! delete everything
 */
ext::~ext() {
}

/*! returns true if the extension (ext_name) is supported by the graphics adapter
 *  @param ext_name the extensions name we want to look for if it's supported
 */
bool ext::is_ext_supported(char* ext_name) {
	if(ext::mode == ext::GRAPHICAL) {
		char* exts = (char*)glGetString(GL_EXTENSIONS);
		if(strstr(exts, ext_name) != NULL) {
			return true;
		}
		else {
			return false;
		}
	}
	else {
		return false;
	}
}
