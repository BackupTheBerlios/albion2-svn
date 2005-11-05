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

#include "shader.h"

#ifdef WIN32
#define glGetProcAddress(x) wglGetProcAddress(x)
#define ProcType LPCSTR
#else
#define glGetProcAddress(x) glXGetProcAddress(x)
#define ProcType GLubyte*
#endif

/*! create and initialize the shader class
 */
shader::shader(engine* e) {
	cshaders = 1;
	shaders = new shader_object[cshaders];
	shaders[0].attributes = 0;
	shaders[0].cattributes = 0;
	shaders[0].cuniforms = 0;
	shaders[0].fragment_shader = 0;
	shaders[0].program = 0;
	shaders[0].uniforms = 0;
	shaders[0].vertex_shader = 0;

	shader_support = false;

	cur_shader = 0;

	// get classes
	shader::e = e;
	shader::m = e->get_msg();
	shader::c = e->get_core();
	shader::f = e->get_file_io();
	shader::exts = e->get_ext();

	// now initialize the extensions
	init_extensions();

	// create parallax mapping shader
	char* pl_uniforms[] = { "cam_pos", "light_pos", "diffuse_map", "normal_map", "height_map", "light_color" };
	char* pl_attributes[] = { "normal", "binormal", "tangent", "texture_coord" };
	shader::add_shader("../data/parallax.vs", "../data/parallax.fs", 6, pl_uniforms, 4, pl_attributes);
}

/*! delete everything
 */
shader::~shader() {
	m->print(msg::MDEBUG, "shader.cpp", "freeing shader stuff");

	m->print(msg::MDEBUG, "shader.cpp", "shader stuff freed");
}

/*! initializes the extensions
 */
void shader::init_extensions() {
	if(exts->is_ext_supported("GL_ARB_fragment_shader") &&
		exts->is_ext_supported("GL_ARB_vertex_shader") &&
		exts->is_ext_supported("GL_ARB_shader_objects") &&
		exts->is_ext_supported("GL_ARB_shading_language_100")) {
		shader::shader_support = true;
		//shader::shader_support = false;
		m->print(msg::MDEBUG, "shader.cpp", "glsl is supported!");
	}
	else {
		m->print(msg::MDEBUG, "shader.cpp", "glsl is not supported!");
	}

	if(shader_support) {
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
	}
}

/*! adds a shader object
 *  @param vname the vertex shaders filename
 *  @param fname the fragment shaders filename
 *  @param cuniforms the amount of uniforms
 *  @param uniforms the uniform names
 *  @param cattributes the amount of attributes
 *  @param attributes the attribute names
 */
unsigned int shader::add_shader(char* vname, char* fname, unsigned int cuniforms, char** uniforms,
						unsigned int cattributes, char** attributes) {
	if(shader_support) {
		// temporary shader text variable (and pointer)
		char* shader_text;
		GLcharARB** pshader_text;
		// success flag (if it's 1 (true), we successfully created a shader object)
		int success;

		// create a new shader object
		shader_object* nshaders = new shader_object[cshaders+1];
		for(unsigned int i = 0; i < cshaders; i++) {
			nshaders[i] = shaders[i];
		}
		delete [] shaders;
		shaders = nshaders;

		// save shader informations
		shaders[cshaders].cuniforms = cuniforms;
		shaders[cshaders].cattributes = cattributes;

		// allocate memory for the uniforms and attributes
		shaders[cshaders].uniforms = new int[shaders[cshaders].cuniforms];
		shaders[cshaders].attributes = new int[shaders[cshaders].cattributes];

		// load the vertex shader
		f->open_file(vname, true);
		unsigned int vs_fsize = f->get_filesize();
		shader_text = new char[vs_fsize+1];
		f->get_block(shader_text, vs_fsize);
		shader_text[vs_fsize] = 0;
		f->close_file();

		// create the vertex shader object
		shaders[cshaders].vertex_shader = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
		pshader_text = (GLcharARB**)&shader_text;
		glShaderSourceARB(shaders[cshaders].vertex_shader, 1, (const char**)pshader_text, NULL);
		delete [] shader_text;
		glCompileShaderARB(shaders[cshaders].vertex_shader);
		glGetObjectParameterivARB(shaders[cshaders].vertex_shader, GL_OBJECT_COMPILE_STATUS_ARB, &success);
		if(!success) {
			GLbyte info_log[1024];
			glGetInfoLogARB(shaders[cshaders].vertex_shader, 1024, NULL, (GLcharARB*)info_log);
			m->print(msg::MERROR, "shader.cpp", "Error in high-level vertex shader #%d compilation!\nInfo log: %s", cshaders, info_log);
			return 0;
		}

		// load the fragment shader
		f->open_file(fname, true);
		unsigned int fs_fsize = f->get_filesize();
		shader_text = new char[fs_fsize+1];
		f->get_block(shader_text, fs_fsize);
		shader_text[fs_fsize] = 0;
		f->close_file();

		// create the fragment shader object
		shaders[cshaders].fragment_shader = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);
		pshader_text = (GLcharARB**)&shader_text;
		glShaderSourceARB(shaders[cshaders].fragment_shader, 1, (const char**)pshader_text, NULL);
		delete [] shader_text;
		glCompileShaderARB(shaders[cshaders].fragment_shader);
		glGetObjectParameterivARB(shaders[cshaders].fragment_shader, GL_OBJECT_COMPILE_STATUS_ARB, &success);
		if(!success) {
			GLbyte info_log[1024];
			glGetInfoLogARB(shaders[cshaders].fragment_shader, 1024, NULL, (GLcharARB*)info_log);
			m->print(msg::MERROR, "shader.cpp", "Error in high-level fragment shader #%d compilation!\nInfo log: %s", cshaders, info_log);
			return 0;
		}

		// create the program object
		shaders[cshaders].program = glCreateProgramObjectARB();
		// attach the vertex and fragment shader progam to it
		glAttachObjectARB(shaders[cshaders].program, shaders[cshaders].vertex_shader);
		glAttachObjectARB(shaders[cshaders].program, shaders[cshaders].fragment_shader);

		// now link the program object
		glLinkProgramARB(shaders[cshaders].program);
		glGetObjectParameterivARB(shaders[cshaders].program, GL_OBJECT_LINK_STATUS_ARB, &success);
		if(!success) {
			GLbyte info_log[1024];
			glGetInfoLogARB(shaders[cshaders].program, 1024, NULL, (GLcharARB*)info_log);
			m->print(msg::MERROR, "shader.cpp", "Error in high-level program #%d linkage!\nInfo log: %s", cshaders, info_log);
			return 0;
		}

		// look up uniform locations
		for(unsigned int i = 0; i < cuniforms; i++) {
			shaders[cshaders].uniforms[i] = glGetUniformLocationARB(shaders[cshaders].program, uniforms[i]);
			if(shaders[cshaders].uniforms[i] == -1) {
				m->print(msg::MERROR, "shader.cpp", "Warning: could not get location for uniform \"%s\"!", uniforms[i]);
			}
		}

		// look up attribute locations
		for(unsigned int i = 0; i < cattributes; i++) {
			shaders[cshaders].attributes[i] = glGetAttribLocationARB(shaders[cshaders].program, attributes[i]);
			if(shaders[cshaders].attributes[i] == -1) {
				m->print(msg::MERROR, "shader.cpp", "Warning: could not get location for attribute \"%s\"!", attributes[i]);
			}
		}

		// validate the program object
		glUseProgramObjectARB(shaders[cshaders].program);
		glValidateProgramARB(shaders[cshaders].program);
		glGetObjectParameterivARB(shaders[cshaders].program, GL_OBJECT_VALIDATE_STATUS_ARB, &success);
		if(!success) {
			GLbyte info_log[1024];
			glGetInfoLogARB(shaders[cshaders].program, 1024, NULL, (GLcharARB*)info_log);
			m->print(msg::MERROR, "shader.cpp", "Error in high-level program #%d validation!\nInfo log: %s", cshaders, info_log);
			return 0;
		}
		glUseProgramObjectARB(0);

		// increment shader count
		cshaders++;
	}

	return cshaders;
}

/*! returns true if glsl shaders are supported
 */
bool shader::is_shader_support() {
	return shader::shader_support;
}

/*! "uses" a shader program object
 *  @param num the shaders number we want to use
 */
void shader::use_shader(unsigned int num) {
	if(shader_support) {
		cur_shader = num;
		glUseProgramObjectARB(shaders[cur_shader].program);
	}
}

/*! updates/sets an uniform value of the type 1 int
 *  @param num the uniforms number
 *  @param var1 the first value
 */
void shader::set_uniform1i(unsigned int num, int var1) {
	if(shader_support) {
		glUniform1iARB(shaders[cur_shader].uniforms[num], var1);
	}
}

/*! updates/sets an uniform value of the type 2 int
 *  @param num the uniforms number
 *  @param var1 the first value
 *  @param var2 the second value
 */
void shader::set_uniform2i(unsigned int num, int var1, int var2) {
	if(shader_support) {
		glUniform2iARB(shaders[cur_shader].uniforms[num], var1, var2);
	}
}

/*! updates/sets an uniform value of the type 3 int
 *  @param num the uniforms number
 *  @param var1 the first value
 *  @param var2 the second value
 *  @param var3 the third value
 */
void shader::set_uniform3i(unsigned int num, int var1, int var2, int var3) {
	if(shader_support) {
		glUniform3iARB(shaders[cur_shader].uniforms[num], var1, var2, var3);
	}
}

/*! updates/sets an uniform value of the type 4 int
 *  @param num the uniforms number
 *  @param var1 the first value
 *  @param var2 the second value
 *  @param var3 the third value
 *  @param var4 the fourth value
 */
void shader::set_uniform4i(unsigned int num, int var1, int var2, int var3, int var4) {
	if(shader_support) {
		glUniform4iARB(shaders[cur_shader].uniforms[num], var1, var2, var3, var4);
	}
}

/*! updates/sets an uniform value array of the type 1 int*
 *  @param num the uniforms number
 *  @param var the int array
 */
void shader::set_uniform1iv(unsigned int num, int* var) {
	if(shader_support) {
		glUniform1ivARB(shaders[cur_shader].uniforms[num], 1, var);
	}
}

/*! updates/sets an uniform value array of the type 2 int*
 *  @param num the uniforms number
 *  @param var the int array
 */
void shader::set_uniform2iv(unsigned int num, int* var) {
	if(shader_support) {
		glUniform2ivARB(shaders[cur_shader].uniforms[num], 1, var);
	}
}

/*! updates/sets an uniform value array of the type 3 int*
 *  @param num the uniforms number
 *  @param var the int array
 */
void shader::set_uniform3iv(unsigned int num, int* var) {
	if(shader_support) {
		glUniform3ivARB(shaders[cur_shader].uniforms[num], 1, var);
	}
}

/*! updates/sets an uniform value array of the type 4 int*
 *  @param num the uniforms number
 *  @param var the int array
 */
void shader::set_uniform4iv(unsigned int num, int* var) {
	if(shader_support) {
		glUniform4ivARB(shaders[cur_shader].uniforms[num], 1, var);
	}
}

/*! updates/sets an uniform value of the type 1 float
 *  @param num the uniforms number
 *  @param var1 the first value
 */
void shader::set_uniform1f(unsigned int num, float var1) {
	if(shader_support) {
		glUniform1fARB(shaders[cur_shader].uniforms[num], var1);
	}
}

/*! updates/sets an uniform value of the type 2 float
 *  @param num the uniforms number
 *  @param var1 the first value
 *  @param var2 the second value
 */
void shader::set_uniform2f(unsigned int num, float var1, float var2) {
	if(shader_support) {
		glUniform2fARB(shaders[cur_shader].uniforms[num], var1, var2);
	}
}

/*! updates/sets an uniform value of the type 3 float
 *  @param num the uniforms number
 *  @param var1 the first value
 *  @param var2 the second value
 *  @param var3 the third value
 */
void shader::set_uniform3f(unsigned int num, float var1, float var2, float var3) {
	if(shader_support) {
		glUniform3fARB(shaders[cur_shader].uniforms[num], var1, var2, var3);
	}
}

/*! updates/sets an uniform value of the type 4 float
 *  @param num the uniforms number
 *  @param var1 the first value
 *  @param var2 the second value
 *  @param var3 the third value
 *  @param var4 the fourth value
 */
void shader::set_uniform4f(unsigned int num, float var1, float var2, float var3, float var4) {
	if(shader_support) {
		glUniform4fARB(shaders[cur_shader].uniforms[num], var1, var2, var3, var4);
	}
}

/*! updates/sets an uniform value array of the type 1 float*
 *  @param num the uniforms number
 *  @param var the float array
 */
void shader::set_uniform1fv(unsigned int num, float* var) {
	if(shader_support) {
		glUniform1fvARB(shaders[cur_shader].uniforms[num], 1, var);
	}
}

/*! updates/sets an uniform value array of the type 2 float*
 *  @param num the uniforms number
 *  @param var the float array
 */
void shader::set_uniform2fv(unsigned int num, float* var) {
	if(shader_support) {
		glUniform2fvARB(shaders[cur_shader].uniforms[num], 1, var);
	}
}

/*! updates/sets an uniform value array of the type 3 float*
 *  @param num the uniforms number
 *  @param var the float array
 */
void shader::set_uniform3fv(unsigned int num, float* var) {
	if(shader_support) {
		glUniform3fvARB(shaders[cur_shader].uniforms[num], 1, var);
	}
}

/*! updates/sets an uniform value array of the type 4 float*
 *  @param num the uniforms number
 *  @param var the float array
 */
void shader::set_uniform4fv(unsigned int num, float* var) {
	if(shader_support) {
		glUniform4fvARB(shaders[cur_shader].uniforms[num], 1, var);
	}
}

/*! updates/sets an attribute value array of the type 4 int*
 *  @param num the attributes number
 *  @param var the int array
 */
void shader::set_attribute4iv(unsigned int num, int* var) {
	if(shader_support) {
		glVertexAttrib4ivARB(shaders[cur_shader].attributes[num], var);
	}
}

/*! updates/sets an attribute value of the type 1 float
 *  @param num the attributes number
 *  @param var1 the first value
 */
void shader::set_attribute1f(unsigned int num, float var1) {
	if(shader_support) {
		glVertexAttrib1fARB(shaders[cur_shader].attributes[num], var1);
	}
}

/*! updates/sets an attribute value of the type 2 float
 *  @param num the attributes number
 *  @param var1 the first value
 *  @param var2 the second value
 */
void shader::set_attribute2f(unsigned int num, float var1, float var2) {
	if(shader_support) {
		glVertexAttrib2fARB(shaders[cur_shader].attributes[num], var1, var2);
	}
}

/*! updates/sets an attribute value of the type 3 float
 *  @param num the attributes number
 *  @param var1 the first value
 *  @param var2 the second value
 *  @param var3 the third value
 */
void shader::set_attribute3f(unsigned int num, float var1, float var2, float var3) {
	if(shader_support) {
		glVertexAttrib3fARB(shaders[cur_shader].attributes[num], var1, var2, var3);
	}
}

/*! updates/sets an attribute value of the type 4 float
 *  @param num the attributes number
 *  @param var1 the first value
 *  @param var2 the second value
 *  @param var3 the third value
 *  @param var4 the fourth value
 */
void shader::set_attribute4f(unsigned int num, float var1, float var2, float var3, float var4) {
	if(shader_support) {
		glVertexAttrib4fARB(shaders[cur_shader].attributes[num], var1, var2, var3, var4);
	}
}

/*! updates/sets an attribute value array of the type 1 float*
 *  @param num the attributes number
 *  @param var the float array
 */
void shader::set_attribute1fv(unsigned int num, float* var) {
	if(shader_support) {
		glVertexAttrib1fvARB(shaders[cur_shader].attributes[num], var);
	}
}

/*! updates/sets an attribute value array of the type 2 float*
 *  @param num the attributes number
 *  @param var the float array
 */
void shader::set_attribute2fv(unsigned int num, float* var) {
	if(shader_support) {
		glVertexAttrib2fvARB(shaders[cur_shader].attributes[num], var);
	}
}

/*! updates/sets an attribute value array of the type 3 float*
 *  @param num the attributes number
 *  @param var the float array
 */
void shader::set_attribute3fv(unsigned int num, float* var) {
	if(shader_support) {
		glVertexAttrib3fvARB(shaders[cur_shader].attributes[num], var);
	}
}

/*! updates/sets an attribute value array of the type 4 float*
 *  @param num the attributes number
 *  @param var the float array
 */
void shader::set_attribute4fv(unsigned int num, float* var) {
	if(shader_support) {
		glVertexAttrib4fvARB(shaders[cur_shader].attributes[num], var);
	}
}

shader::shader_object* shader::get_shader_object(unsigned int num) {
	return &shaders[num];
}
