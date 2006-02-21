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
#define glGetProcAddress(x) glXGetProcAddressARB(x)
#define ProcType GLubyte*
#endif

// mac os x: #define glGetProcAddress(x) aglGetProcAddress(x) (?)

using namespace std;

/*! create and initialize the extension class
 */
ext::ext(unsigned int imode, msg* m) {
	ext::mode = imode;

	glActiveTextureARB = NULL;
	glMultiTexCoord3fARB = NULL;

	glBindBufferARB = NULL;
	glBufferDataARB = NULL;
	glBufferSubDataARB = NULL;
	glDeleteBuffersARB = NULL;
	glGenBuffersARB = NULL;
	glMapBufferARB = NULL;
	glUnmapBufferARB = NULL;

	multitexture_support = false;
	shader_support = false;
	vbo_support = false;

	if(is_ext_supported("GL_ARB_multitexture")) {
		glActiveTextureARB = (PFNGLACTIVETEXTUREARBPROC)glGetProcAddress((ProcType)"glActiveTextureARB");
		glClientActiveTextureARB = (PFNGLCLIENTACTIVETEXTUREARBPROC)glGetProcAddress((ProcType)"glClientActiveTextureARB");
		glMultiTexCoord2fARB = (PFNGLMULTITEXCOORD2FARBPROC)glGetProcAddress((ProcType)"glMultiTexCoord2fARB");
		glMultiTexCoord3fARB = (PFNGLMULTITEXCOORD3FARBPROC)glGetProcAddress((ProcType)"glMultiTexCoord3fARB");
		multitexture_support = true;
	}
	else {
		m->print(msg::MERROR, "extensions.cpp", "ext(): your graphic device doesn't support 'GL_ARB_multitexture'!");
	}

	if(is_ext_supported("GL_ARB_texture_env_combine")) {
		texenv_combine_support = true;
	}
	else {
		m->print(msg::MERROR, "extensions.cpp", "ext(): your graphic device doesn't support 'GL_ARB_texture_env_combine'!");
	}

	if(is_ext_supported("GL_ARB_fragment_shader") &&
		is_ext_supported("GL_ARB_vertex_shader") &&
		is_ext_supported("GL_ARB_shader_objects") &&
		is_ext_supported("GL_ARB_shading_language_100")) {
		glEnableVertexAttribArrayARB = (PFNGLENABLEVERTEXATTRIBARRAYARBPROC)glGetProcAddress((ProcType)"glEnableVertexAttribArrayARB");
		glVertexAttribPointerARB = (PFNGLVERTEXATTRIBPOINTERARBPROC)glGetProcAddress((ProcType)"glVertexAttribPointerARB");
		shader_support = true;
	}
	else {
		m->print(msg::MERROR, "extensions.cpp", "ext(): your graphic device doesn't support 'GL_ARB_fragment_shader', 'GL_ARB_vertex_shader', 'GL_ARB_shader_objects' and/or 'GL_ARB_shading_language_100'!");
	}

	if(is_ext_supported("GL_ARB_vertex_buffer_object")) {
		glBindBufferARB = (PFNGLBINDBUFFERARBPROC)glGetProcAddress((ProcType)"glBindBufferARB");
		glBufferDataARB = (PFNGLBUFFERDATAARBPROC)glGetProcAddress((ProcType)"glBufferDataARB");
		glBufferSubDataARB = (PFNGLBUFFERSUBDATAARBPROC)glGetProcAddress((ProcType)"glBufferSubDataARB");
		glDeleteBuffersARB = (PFNGLDELETEBUFFERSARBPROC)glGetProcAddress((ProcType)"glDeleteBuffersARB");
		glGenBuffersARB = (PFNGLGENBUFFERSARBPROC)glGetProcAddress((ProcType)"glGenBuffersARB");
		glMapBufferARB = (PFNGLMAPBUFFERARBPROC)glGetProcAddress((ProcType)"glMapBufferARB");
		glUnmapBufferARB = (PFNGLUNMAPBUFFERARBPROC)glGetProcAddress((ProcType)"glUnmapBufferARB");
		glIsBufferARB = (PFNGLISBUFFERARBPROC)glGetProcAddress((ProcType)"glIsBufferARB");
		vbo_support = true;
	}
	else {
		m->print(msg::MERROR, "extensions.cpp", "ext(): your graphic device doesn't support 'GL_ARB_vertex_buffer_object'!");
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
	}
	return false;
}

/*! returns true if multi texturing is supported
 */
bool ext::is_multitexture_support() {
	return ext::multitexture_support;
}

/*! returns true if texture environment combine is supported
 */
bool ext::is_texenv_combine_support() {
	return ext::texenv_combine_support;
}

/*! returns true if glsl shaders are supported
 */
bool ext::is_shader_support() {
	return ext::shader_support;
}


/*! returns true if vertex buffer objects are supported
 */
bool ext::is_vbo_support() {
	return ext::vbo_support;
}
