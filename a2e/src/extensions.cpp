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
	texenv_combine_support = false;
	shader_support = false;
	vbo_support = false;
	fbo_support = false;
	blend_func_separate_support = false;

	if(is_ext_supported("GL_ARB_multitexture")) {
		glActiveTextureARB = (PFNGLACTIVETEXTUREARBPROC)glGetProcAddress((ProcType)"glActiveTextureARB");
		glClientActiveTextureARB = (PFNGLCLIENTACTIVETEXTUREARBPROC)glGetProcAddress((ProcType)"glClientActiveTextureARB");
		glMultiTexCoord2fARB = (PFNGLMULTITEXCOORD2FARBPROC)glGetProcAddress((ProcType)"glMultiTexCoord2fARB");
		glMultiTexCoord3fARB = (PFNGLMULTITEXCOORD3FARBPROC)glGetProcAddress((ProcType)"glMultiTexCoord3fARB");
		multitexture_support = true;
	}
	else {
		if(imode == 0) m->print(msg::MERROR, "extensions.cpp", "ext(): your graphic device doesn't support 'GL_ARB_multitexture'!");
	}

	if(is_ext_supported("GL_ARB_texture_env_combine")) {
		texenv_combine_support = true;
	}
	else {
		if(imode == 0) m->print(msg::MERROR, "extensions.cpp", "ext(): your graphic device doesn't support 'GL_ARB_texture_env_combine'!");
	}

	if(is_ext_supported("GL_ARB_fragment_shader") &&
		is_ext_supported("GL_ARB_vertex_shader") &&
		is_ext_supported("GL_ARB_shader_objects") &&
		is_ext_supported("GL_ARB_shading_language_100")) {
		glEnableVertexAttribArrayARB = (PFNGLENABLEVERTEXATTRIBARRAYARBPROC)glGetProcAddress((ProcType)"glEnableVertexAttribArrayARB");
		glVertexAttribPointerARB = (PFNGLVERTEXATTRIBPOINTERARBPROC)glGetProcAddress((ProcType)"glVertexAttribPointerARB");

		glProgramStringARB = (PFNGLPROGRAMSTRINGARBPROC)glGetProcAddress((ProcType)"glProgramStringARB");
		glBindProgramARB = (PFNGLBINDPROGRAMARBPROC)glGetProcAddress((ProcType)"glBindProgramARB");
		glDeleteProgramsARB = (PFNGLDELETEPROGRAMSARBPROC)glGetProcAddress((ProcType)"glDeleteProgramsARB");
		glGenProgramsARB = (PFNGLGENPROGRAMSARBPROC)glGetProcAddress((ProcType)"glGenProgramsARB");

		glCreateShaderObjectARB = (PFNGLCREATESHADEROBJECTARBPROC)glGetProcAddress((ProcType)"glCreateShaderObjectARB");
		glCreateProgramObjectARB = (PFNGLCREATEPROGRAMOBJECTARBPROC)glGetProcAddress((ProcType)"glCreateProgramObjectARB");
		glAttachObjectARB = (PFNGLATTACHOBJECTARBPROC)glGetProcAddress((ProcType)"glAttachObjectARB");
		glDetachObjectARB = (PFNGLDETACHOBJECTARBPROC)glGetProcAddress((ProcType)"glDetachObjectARB");
		glDeleteObjectARB = (PFNGLDELETEOBJECTARBPROC)glGetProcAddress((ProcType)"glDeleteObjectARB");
		glShaderSourceARB = (PFNGLSHADERSOURCEARBPROC)glGetProcAddress((ProcType)"glShaderSourceARB");
		glCompileShaderARB = (PFNGLCOMPILESHADERARBPROC)glGetProcAddress((ProcType)"glCompileShaderARB");
		glLinkProgramARB = (PFNGLLINKPROGRAMARBPROC)glGetProcAddress((ProcType)"glLinkProgramARB");
		glValidateProgramARB = (PFNGLVALIDATEPROGRAMARBPROC)glGetProcAddress((ProcType)"glValidateProgramARB");
		glUseProgramObjectARB = (PFNGLUSEPROGRAMOBJECTARBPROC)glGetProcAddress((ProcType)"glUseProgramObjectARB");
		glGetObjectParameterivARB = (PFNGLGETOBJECTPARAMETERIVARBPROC)glGetProcAddress((ProcType)"glGetObjectParameterivARB");
		glGetInfoLogARB = (PFNGLGETINFOLOGARBPROC)glGetProcAddress((ProcType)"glGetInfoLogARB");

		glGetUniformLocationARB = (PFNGLGETUNIFORMLOCATIONARBPROC)glGetProcAddress((ProcType)"glGetUniformLocationARB");
		glGetAttribLocationARB = (PFNGLGETATTRIBLOCATIONARBPROC)glGetProcAddress((ProcType)"glGetAttribLocationARB");

		glUniform1iARB = (PFNGLUNIFORM1IARBPROC)glGetProcAddress((ProcType)"glUniform1iARB");
		glUniform2iARB = (PFNGLUNIFORM2IARBPROC)glGetProcAddress((ProcType)"glUniform2iARB");
		glUniform3iARB = (PFNGLUNIFORM3IARBPROC)glGetProcAddress((ProcType)"glUniform3iARB");
		glUniform4iARB = (PFNGLUNIFORM4IARBPROC)glGetProcAddress((ProcType)"glUniform4iARB");
		glUniform1ivARB = (PFNGLUNIFORM1IVARBPROC)glGetProcAddress((ProcType)"glUniform1ivARB");
		glUniform2ivARB = (PFNGLUNIFORM2IVARBPROC)glGetProcAddress((ProcType)"glUniform2ivARB");
		glUniform3ivARB = (PFNGLUNIFORM3IVARBPROC)glGetProcAddress((ProcType)"glUniform3ivARB");
		glUniform4ivARB = (PFNGLUNIFORM4IVARBPROC)glGetProcAddress((ProcType)"glUniform4ivARB");

		glUniform1fARB = (PFNGLUNIFORM1FARBPROC)glGetProcAddress((ProcType)"glUniform1fARB");
		glUniform2fARB = (PFNGLUNIFORM2FARBPROC)glGetProcAddress((ProcType)"glUniform2fARB");
		glUniform3fARB = (PFNGLUNIFORM3FARBPROC)glGetProcAddress((ProcType)"glUniform3fARB");
		glUniform4fARB = (PFNGLUNIFORM4FARBPROC)glGetProcAddress((ProcType)"glUniform4fARB");
		glUniform1fvARB = (PFNGLUNIFORM1FVARBPROC)glGetProcAddress((ProcType)"glUniform1fvARB");
		glUniform2fvARB = (PFNGLUNIFORM2FVARBPROC)glGetProcAddress((ProcType)"glUniform2fvARB");
		glUniform3fvARB = (PFNGLUNIFORM3FVARBPROC)glGetProcAddress((ProcType)"glUniform3fvARB");
		glUniform4fvARB = (PFNGLUNIFORM4FVARBPROC)glGetProcAddress((ProcType)"glUniform4fvARB");

		glVertexAttrib4ivARB = (PFNGLVERTEXATTRIB4IVARBPROC)glGetProcAddress((ProcType)"glVertexAttrib4ivARB");

		glVertexAttrib1fARB = (PFNGLVERTEXATTRIB1FARBPROC)glGetProcAddress((ProcType)"glVertexAttrib1fARB");
		glVertexAttrib2fARB = (PFNGLVERTEXATTRIB2FARBPROC)glGetProcAddress((ProcType)"glVertexAttrib2fARB");
		glVertexAttrib3fARB = (PFNGLVERTEXATTRIB3FARBPROC)glGetProcAddress((ProcType)"glVertexAttrib3fARB");
		glVertexAttrib4fARB = (PFNGLVERTEXATTRIB4FARBPROC)glGetProcAddress((ProcType)"glVertexAttrib4fARB");
		glVertexAttrib1fvARB = (PFNGLVERTEXATTRIB1FVARBPROC)glGetProcAddress((ProcType)"glVertexAttrib1fvARB");
		glVertexAttrib2fvARB = (PFNGLVERTEXATTRIB2FVARBPROC)glGetProcAddress((ProcType)"glVertexAttrib2fvARB");
		glVertexAttrib3fvARB = (PFNGLVERTEXATTRIB3FVARBPROC)glGetProcAddress((ProcType)"glVertexAttrib3fvARB");
		glVertexAttrib4fvARB = (PFNGLVERTEXATTRIB4FVARBPROC)glGetProcAddress((ProcType)"glVertexAttrib4fvARB");

		shader_support = true;
	}
	else {
		if(imode == 0) m->print(msg::MERROR, "extensions.cpp", "ext(): your graphic device doesn't support 'GL_ARB_fragment_shader', 'GL_ARB_vertex_shader', 'GL_ARB_shader_objects' and/or 'GL_ARB_shading_language_100'!");
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
		if(imode == 0) m->print(msg::MERROR, "extensions.cpp", "ext(): your graphic device doesn't support 'GL_ARB_vertex_buffer_object'!");
	}

	if(is_ext_supported("GL_EXT_framebuffer_object")) {
		glIsRenderbufferEXT = (PFNGLISRENDERBUFFEREXTPROC)glGetProcAddress((ProcType)"glIsRenderbufferEXT");
		glBindRenderbufferEXT = (PFNGLBINDRENDERBUFFEREXTPROC)glGetProcAddress((ProcType)"glBindRenderbufferEXT");
		glDeleteRenderbuffersEXT = (PFNGLDELETERENDERBUFFERSEXTPROC)glGetProcAddress((ProcType)"glDeleteRenderbuffersEXT");
		glGenRenderbuffersEXT = (PFNGLGENRENDERBUFFERSEXTPROC)glGetProcAddress((ProcType)"glGenRenderbuffersEXT");
		glRenderbufferStorageEXT = (PFNGLRENDERBUFFERSTORAGEEXTPROC)glGetProcAddress((ProcType)"glRenderbufferStorageEXT");
		glGetRenderbufferParameterivEXT = (PFNGLGETRENDERBUFFERPARAMETERIVEXTPROC)glGetProcAddress((ProcType)"glGetRenderbufferParameterivEXT");
		glIsFramebufferEXT = (PFNGLISFRAMEBUFFEREXTPROC)glGetProcAddress((ProcType)"glIsFramebufferEXT");
		glBindFramebufferEXT = (PFNGLBINDFRAMEBUFFEREXTPROC)glGetProcAddress((ProcType)"glBindFramebufferEXT");
		glDeleteFramebuffersEXT = (PFNGLDELETEFRAMEBUFFERSEXTPROC)glGetProcAddress((ProcType)"glDeleteFramebuffersEXT");
		glGenFramebuffersEXT = (PFNGLGENFRAMEBUFFERSEXTPROC)glGetProcAddress((ProcType)"glGenFramebuffersEXT");
		glCheckFramebufferStatusEXT = (PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC)glGetProcAddress((ProcType)"glCheckFramebufferStatusEXT");
		glFramebufferTexture1DEXT = (PFNGLFRAMEBUFFERTEXTURE1DEXTPROC)glGetProcAddress((ProcType)"glFramebufferTexture1DEXT");
		glFramebufferTexture2DEXT = (PFNGLFRAMEBUFFERTEXTURE2DEXTPROC)glGetProcAddress((ProcType)"glFramebufferTexture2DEXT");
		glFramebufferTexture3DEXT = (PFNGLFRAMEBUFFERTEXTURE3DEXTPROC)glGetProcAddress((ProcType)"glFramebufferTexture3DEXT");
		glFramebufferRenderbufferEXT = (PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC)glGetProcAddress((ProcType)"glFramebufferRenderbufferEXT");
		glGetFramebufferAttachmentParameterivEXT = (PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVEXTPROC)glGetProcAddress((ProcType)"glGetFramebufferAttachmentParameterivEXT");
		glGenerateMipmapEXT = (PFNGLGENERATEMIPMAPEXTPROC)glGetProcAddress((ProcType)"glGenerateMipmapEXT");
		fbo_support = true;
	}
	else {
		if(imode == 0) m->print(msg::MERROR, "extensions.cpp", "ext(): your graphic device doesn't support 'GL_EXT_framebuffer_object'!");
	}

	if(is_ext_supported("EXT_blend_func_separate")) {
		glBlendFuncSeparateEXT = (PFNGLBLENDFUNCSEPARATEEXTPROC)glGetProcAddress((ProcType)"glBlendFuncSeparateEXT");
		blend_func_separate_support = true;
	}
	else {
		if(imode == 0) m->print(msg::MERROR, "extensions.cpp", "ext(): your graphic device doesn't support 'EXT_blend_func_separate'!");
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

/*! returns true if the opengl version specified by major and minor is available (opengl major.minor.xxxx)
 *  @param ext_name the extensions name we want to look for if it's supported
 */
bool ext::is_gl_version(unsigned int major, unsigned int minor) {
	char* version = (char*)glGetString(GL_VERSION);
	if((unsigned int)(version[0] & 0xFF) > major) return true;
	else if((unsigned int)(version[0] & 0xFF) == major && (unsigned int)(version[2] & 0xFF) >= minor) return true;
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

/*! returns true if framebuffer objects are supported
 */
bool ext::is_fbo_support() {
	return ext::fbo_support;
}


/*! returns true if blend func separate is supported
 */
bool ext::is_blend_func_separate_support() {
	return ext::blend_func_separate_support;
}
