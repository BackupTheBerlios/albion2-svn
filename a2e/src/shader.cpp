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

/*! create and initialize the shader class
 */
shader::shader(engine* e) {
	// get classes
	shader::e = e;
	shader::m = e->get_msg();
	shader::c = e->get_core();
	shader::f = e->get_file_io();
	shader::exts = e->get_ext();

	cshaders = 1;
	shaders = new shader_object[cshaders];
	shaders[0].attributes = 0;
	shaders[0].cattributes = 0;
	shaders[0].cuniforms = 0;
	shaders[0].fragment_shader = 0;
	shaders[0].program = 0;
	shaders[0].uniforms = 0;
	shaders[0].vertex_shader = 0;

	shader_support = exts->is_shader_support();

	cur_shader = 0;

	// create parallax mapping shader
	const char* pl_uniforms[] = { "cam_pos", "light_pos", "local_mview", "diffuse_texture", "normal_texture", "height_texture", "specular_texture" };
	const char* pl_attributes[] = { "normal", "binormal", "tangent", "texture_coord" };
	string vs_path = e->data_path("parallax.vs");
	string fs_path = e->data_path("parallax.fs");
	shader::add_shader((char*)vs_path.c_str(), (char*)fs_path.c_str(), 7, (char**)pl_uniforms, 4, (char**)pl_attributes);
	fs_path = e->data_path("parallax_hdr.fs");
	shader::add_shader((char*)vs_path.c_str(), (char*)fs_path.c_str(), 7, (char**)pl_uniforms, 4, (char**)pl_attributes);

	const char* phl_uniforms[] = { "diffuse_texture", "specular_texture" };
	vs_path = e->data_path("phong.vs");
	fs_path = e->data_path("phong.fs");
	shader::add_shader((char*)vs_path.c_str(), (char*)fs_path.c_str(), 2, (char**)phl_uniforms, 0, NULL);
	fs_path = e->data_path("phong_hdr.fs");
	shader::add_shader((char*)vs_path.c_str(), (char*)fs_path.c_str(), 2, (char**)phl_uniforms, 0, NULL);

	const char* blur_uniforms[] = { "tcs", "texture" };
	const char* blur_attributes[] = { "texture_coord" };
	vs_path = e->data_path("blur.vs");
	fs_path = e->data_path("blur3x3.fs");
	shader::add_shader((char*)vs_path.c_str(), (char*)fs_path.c_str(), 2, (char**)blur_uniforms, 1, (char**)blur_attributes);
	fs_path = e->data_path("blur5x5.fs");
	shader::add_shader((char*)vs_path.c_str(), (char*)fs_path.c_str(), 2, (char**)blur_uniforms, 1, (char**)blur_attributes);
	fs_path = e->data_path("blurline3.fs");
	shader::add_shader((char*)vs_path.c_str(), (char*)fs_path.c_str(), 2, (char**)blur_uniforms, 1, (char**)blur_attributes);
	fs_path = e->data_path("blurline5.fs");
	shader::add_shader((char*)vs_path.c_str(), (char*)fs_path.c_str(), 2, (char**)blur_uniforms, 1, (char**)blur_attributes);

	const char* sb_uniforms[] = { "skybox_texture", "max_value" };
	vs_path = e->data_path("skybox.vs");
	fs_path = e->data_path("skybox.fs");
	shader::add_shader((char*)vs_path.c_str(), (char*)fs_path.c_str(), 2, (char**)sb_uniforms, 0, NULL);

	const char* cv_uniforms[] = { "half_pixel", "RT" };
	vs_path = e->data_path("convert.vs");
	fs_path = e->data_path("convert.fs");
	shader::add_shader((char*)vs_path.c_str(), (char*)fs_path.c_str(), 2, (char**)cv_uniforms, 0, NULL);


	vs_path = e->data_path("avexphdr_scale.vs");

	const char* av_uniforms[] = { "RT" };
	fs_path = e->data_path("average.fs");
	shader::add_shader((char*)vs_path.c_str(), (char*)fs_path.c_str(), 1, (char**)av_uniforms, 0, NULL);

	const char* exp_uniforms[] = { "average_texture", "exposure_texture", "frame_time" };
	fs_path = e->data_path("exposure.fs");
	shader::add_shader((char*)vs_path.c_str(), (char*)fs_path.c_str(), 3, (char**)exp_uniforms, 0, NULL);

	const char* hdr_uniforms[] = { "scene_texture", "blur_texture", "exposure_texture" };
	fs_path = e->data_path("hdr.fs");
	shader::add_shader((char*)vs_path.c_str(), (char*)fs_path.c_str(), 3, (char**)hdr_uniforms, 0, NULL);
}

/*! delete everything
 */
shader::~shader() {
	m->print(msg::MDEBUG, "shader.cpp", "freeing shader stuff");

    for(unsigned int i = 0; i < cshaders; i++) {
        delete [] shaders[i].uniforms;
        delete [] shaders[i].attributes;
    }
	delete [] shaders;

	m->print(msg::MDEBUG, "shader.cpp", "shader stuff freed");
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
		f->open_file(vname, file_io::OT_READ_BINARY);
		unsigned int vs_fsize = f->get_filesize();
		shader_text = new char[vs_fsize+1];
		f->get_block(shader_text, vs_fsize);
		shader_text[vs_fsize] = 0;
		f->close_file();

		// create the vertex shader object
		shaders[cshaders].vertex_shader = exts->glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
		pshader_text = (GLcharARB**)&shader_text;
		exts->glShaderSourceARB(shaders[cshaders].vertex_shader, 1, (const char**)pshader_text, NULL);
		delete [] shader_text;
		exts->glCompileShaderARB(shaders[cshaders].vertex_shader);
		exts->glGetObjectParameterivARB(shaders[cshaders].vertex_shader, GL_OBJECT_COMPILE_STATUS_ARB, &success);
		if(!success) {
			GLbyte info_log[1024];
			exts->glGetInfoLogARB(shaders[cshaders].vertex_shader, 1024, NULL, (GLcharARB*)info_log);
			m->print(msg::MERROR, "shader.cpp", "Error in high-level vertex shader #%d compilation!\nInfo log: %s", cshaders, info_log);
			return 0;
		}

		// load the fragment shader
		f->open_file(fname, file_io::OT_READ_BINARY);
		unsigned int fs_fsize = f->get_filesize();
		shader_text = new char[fs_fsize+1];
		f->get_block(shader_text, fs_fsize);
		shader_text[fs_fsize] = 0;
		f->close_file();

		// create the fragment shader object
		shaders[cshaders].fragment_shader = exts->glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);
		pshader_text = (GLcharARB**)&shader_text;
		exts->glShaderSourceARB(shaders[cshaders].fragment_shader, 1, (const char**)pshader_text, NULL);
		delete [] shader_text;
		exts->glCompileShaderARB(shaders[cshaders].fragment_shader);
		exts->glGetObjectParameterivARB(shaders[cshaders].fragment_shader, GL_OBJECT_COMPILE_STATUS_ARB, &success);
		if(!success) {
			GLbyte info_log[1024];
			exts->glGetInfoLogARB(shaders[cshaders].fragment_shader, 1024, NULL, (GLcharARB*)info_log);
			m->print(msg::MERROR, "shader.cpp", "Error in high-level fragment shader #%d compilation!\nInfo log: %s", cshaders, info_log);
			return 0;
		}

		// create the program object
		shaders[cshaders].program = exts->glCreateProgramObjectARB();
		// attach the vertex and fragment shader progam to it
		exts->glAttachObjectARB(shaders[cshaders].program, shaders[cshaders].vertex_shader);
		exts->glAttachObjectARB(shaders[cshaders].program, shaders[cshaders].fragment_shader);

		// now link the program object
		exts->glLinkProgramARB(shaders[cshaders].program);
		exts->glGetObjectParameterivARB(shaders[cshaders].program, GL_OBJECT_LINK_STATUS_ARB, &success);
		if(!success) {
			GLbyte info_log[1024];
			exts->glGetInfoLogARB(shaders[cshaders].program, 1024, NULL, (GLcharARB*)info_log);
			m->print(msg::MERROR, "shader.cpp", "Error in high-level program #%d linkage!\nInfo log: %s", cshaders, info_log);
			return 0;
		}

		// look up uniform locations
		for(unsigned int i = 0; i < cuniforms; i++) {
			shaders[cshaders].uniforms[i] = exts->glGetUniformLocationARB(shaders[cshaders].program, uniforms[i]);
			if(shaders[cshaders].uniforms[i] == -1) {
				m->print(msg::MERROR, "shader.cpp", "Warning: could not get location for uniform \"%s\"!", uniforms[i]);
			}
		}

		// look up attribute locations
		for(unsigned int i = 0; i < cattributes; i++) {
			shaders[cshaders].attributes[i] = exts->glGetAttribLocationARB(shaders[cshaders].program, attributes[i]);
			if(shaders[cshaders].attributes[i] == -1) {
				m->print(msg::MERROR, "shader.cpp", "Warning: could not get location for attribute \"%s\"!", attributes[i]);
			}
		}

		// validate the program object
		exts->glUseProgramObjectARB(shaders[cshaders].program);
		exts->glValidateProgramARB(shaders[cshaders].program);
		exts->glGetObjectParameterivARB(shaders[cshaders].program, GL_OBJECT_VALIDATE_STATUS_ARB, &success);
		if(!success) {
			GLbyte info_log[1024];
			exts->glGetInfoLogARB(shaders[cshaders].program, 1024, NULL, (GLcharARB*)info_log);
			m->print(msg::MERROR, "shader.cpp", "Error in high-level program #%d validation!\nInfo log: %s", cshaders, info_log);
			return 0;
		}
		exts->glUseProgramObjectARB(0);

		// increment shader count
		cshaders++;
	}

	return cshaders-1;
}

/*! "uses" a shader program object
 *  @param num the shaders number we want to use
 */
void shader::use_shader(unsigned int num) {
	if(shader_support) {
		cur_shader = num;
		exts->glUseProgramObjectARB(shaders[cur_shader].program);
	}
}

/*! updates/sets an uniform value of the type 1 int
 *  @param num the uniforms number
 *  @param var1 the first value
 */
void shader::set_uniform1i(unsigned int num, int var1) {
	if(shader_support) {
		exts->glUniform1iARB(shaders[cur_shader].uniforms[num], var1);
	}
}

/*! updates/sets an uniform value of the type 2 int
 *  @param num the uniforms number
 *  @param var1 the first value
 *  @param var2 the second value
 */
void shader::set_uniform2i(unsigned int num, int var1, int var2) {
	if(shader_support) {
		exts->glUniform2iARB(shaders[cur_shader].uniforms[num], var1, var2);
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
		exts->glUniform3iARB(shaders[cur_shader].uniforms[num], var1, var2, var3);
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
		exts->glUniform4iARB(shaders[cur_shader].uniforms[num], var1, var2, var3, var4);
	}
}

/*! updates/sets an uniform value array of the type 1 int*
 *  @param num the uniforms number
 *  @param var the int array
 */
void shader::set_uniform1iv(unsigned int num, int* var, unsigned int count) {
	if(shader_support) {
		exts->glUniform1ivARB(shaders[cur_shader].uniforms[num], count, var);
	}
}

/*! updates/sets an uniform value array of the type 2 int*
 *  @param num the uniforms number
 *  @param var the int array
 */
void shader::set_uniform2iv(unsigned int num, int* var, unsigned int count) {
	if(shader_support) {
		exts->glUniform2ivARB(shaders[cur_shader].uniforms[num], count, var);
	}
}

/*! updates/sets an uniform value array of the type 3 int*
 *  @param num the uniforms number
 *  @param var the int array
 */
void shader::set_uniform3iv(unsigned int num, int* var, unsigned int count) {
	if(shader_support) {
		exts->glUniform3ivARB(shaders[cur_shader].uniforms[num], count, var);
	}
}

/*! updates/sets an uniform value array of the type 4 int*
 *  @param num the uniforms number
 *  @param var the int array
 */
void shader::set_uniform4iv(unsigned int num, int* var, unsigned int count) {
	if(shader_support) {
		exts->glUniform4ivARB(shaders[cur_shader].uniforms[num], count, var);
	}
}

/*! updates/sets an uniform value of the type 1 float
 *  @param num the uniforms number
 *  @param var1 the first value
 */
void shader::set_uniform1f(unsigned int num, float var1) {
	if(shader_support) {
		exts->glUniform1fARB(shaders[cur_shader].uniforms[num], var1);
	}
}

/*! updates/sets an uniform value of the type 2 float
 *  @param num the uniforms number
 *  @param var1 the first value
 *  @param var2 the second value
 */
void shader::set_uniform2f(unsigned int num, float var1, float var2) {
	if(shader_support) {
		exts->glUniform2fARB(shaders[cur_shader].uniforms[num], var1, var2);
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
		exts->glUniform3fARB(shaders[cur_shader].uniforms[num], var1, var2, var3);
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
		exts->glUniform4fARB(shaders[cur_shader].uniforms[num], var1, var2, var3, var4);
	}
}

/*! updates/sets an uniform value array of the type 1 float*
 *  @param num the uniforms number
 *  @param var the float array
 */
void shader::set_uniform1fv(unsigned int num, float* var, unsigned int count) {
	if(shader_support) {
		exts->glUniform1fvARB(shaders[cur_shader].uniforms[num], count, var);
	}
}

/*! updates/sets an uniform value array of the type 2 float*
 *  @param num the uniforms number
 *  @param var the float array
 */
void shader::set_uniform2fv(unsigned int num, float* var, unsigned int count) {
	if(shader_support) {
		exts->glUniform2fvARB(shaders[cur_shader].uniforms[num], count, var);
	}
}

/*! updates/sets an uniform value array of the type 3 float*
 *  @param num the uniforms number
 *  @param var the float array
 */
void shader::set_uniform3fv(unsigned int num, float* var, unsigned int count) {
	if(shader_support) {
		exts->glUniform3fvARB(shaders[cur_shader].uniforms[num], count, var);
	}
}

/*! updates/sets an uniform value array of the type 4 float*
 *  @param num the uniforms number
 *  @param var the float array
 */
void shader::set_uniform4fv(unsigned int num, float* var, unsigned int count) {
	if(shader_support) {
		exts->glUniform4fvARB(shaders[cur_shader].uniforms[num], count, var);
	}
}

/*! updates/sets an attribute value array of the type 4 int*
 *  @param num the attributes number
 *  @param var the int array
 */
void shader::set_attribute4iv(unsigned int num, int* var) {
	if(shader_support) {
		exts->glVertexAttrib4ivARB(shaders[cur_shader].attributes[num], var);
	}
}

/*! updates/sets an attribute value of the type 1 float
 *  @param num the attributes number
 *  @param var1 the first value
 */
void shader::set_attribute1f(unsigned int num, float var1) {
	if(shader_support) {
		exts->glVertexAttrib1fARB(shaders[cur_shader].attributes[num], var1);
	}
}

/*! updates/sets an attribute value of the type 2 float
 *  @param num the attributes number
 *  @param var1 the first value
 *  @param var2 the second value
 */
void shader::set_attribute2f(unsigned int num, float var1, float var2) {
	if(shader_support) {
		exts->glVertexAttrib2fARB(shaders[cur_shader].attributes[num], var1, var2);
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
		exts->glVertexAttrib3fARB(shaders[cur_shader].attributes[num], var1, var2, var3);
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
		exts->glVertexAttrib4fARB(shaders[cur_shader].attributes[num], var1, var2, var3, var4);
	}
}

/*! updates/sets an attribute value array of the type 1 float*
 *  @param num the attributes number
 *  @param var the float array
 */
void shader::set_attribute1fv(unsigned int num, float* var) {
	if(shader_support) {
		exts->glVertexAttrib1fvARB(shaders[cur_shader].attributes[num], var);
	}
}

/*! updates/sets an attribute value array of the type 2 float*
 *  @param num the attributes number
 *  @param var the float array
 */
void shader::set_attribute2fv(unsigned int num, float* var) {
	if(shader_support) {
		exts->glVertexAttrib2fvARB(shaders[cur_shader].attributes[num], var);
	}
}

/*! updates/sets an attribute value array of the type 3 float*
 *  @param num the attributes number
 *  @param var the float array
 */
void shader::set_attribute3fv(unsigned int num, float* var) {
	if(shader_support) {
		exts->glVertexAttrib3fvARB(shaders[cur_shader].attributes[num], var);
	}
}

/*! updates/sets an attribute value array of the type 4 float*
 *  @param num the attributes number
 *  @param var the float array
 */
void shader::set_attribute4fv(unsigned int num, float* var) {
	if(shader_support) {
		exts->glVertexAttrib4fvARB(shaders[cur_shader].attributes[num], var);
	}
}

/*! returns the 'num'th shader object
 *  @param num the number of the shader object we want to get/return
 */
shader::shader_object* shader::get_shader_object(unsigned int num) {
	return &shaders[num];
}


void shader::set_uniform_matrix2fv(unsigned int num, float* var, unsigned int count, bool transpose) {
	if(shader_support) {
		exts->glUniformMatrix2fv(shaders[cur_shader].uniforms[num], count, transpose, var);
	}
}

void shader::set_uniform_matrix3fv(unsigned int num, float* var, unsigned int count, bool transpose) {
	if(shader_support) {
		exts->glUniformMatrix3fv(shaders[cur_shader].uniforms[num], count, transpose, var);
	}
}

void shader::set_uniform_matrix4fv(unsigned int num, float* var, unsigned int count, bool transpose) {
	if(shader_support) {
		exts->glUniformMatrix4fv(shaders[cur_shader].uniforms[num], count, transpose, var);
	}
}
