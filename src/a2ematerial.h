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
 
#ifndef __A2EMATERIAL_H__
#define __A2EMATERIAL_H__

#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <SDL/SDL_image.h>
#include "msg.h"
#include "core.h"
#include "file_io.h"
#include "engine.h"
using namespace std;

#include "win_dll_export.h"

/*! @class a2ematerial
 *  @brief material routines
 *  @author flo
 *  @todo -
 *  
 *  the a2ematerial class
 */

class A2E_API a2ematerial
{
public:
	a2ematerial(engine* e);
	~a2ematerial();

	void load_material(char* filename);
	void load_textures();

	GLuint get_texture(unsigned int obj_num, unsigned int num);

	char get_material_type(unsigned int obj_num);
	char get_color_type(unsigned int obj_num, unsigned int texture);

	void enable_texture(unsigned int obj_num);
	void disable_texture(unsigned int obj_num);

	unsigned int get_texture_count(unsigned int obj_num);

	enum MAT_TYPES {
		NONE = 0x00,
		DIFFUSE = 0x01,
		BUMP = 0x02,
		PARALLAX = 0x03,
		MULTITEXTURE = 0x04
	};

protected:
	msg* m;
	core* c;
	file_io* file;
	engine* e;
	texman* t;

	unsigned int get_combine(char c);
	unsigned int get_rgb_source(char c);
	unsigned int get_rgb_operand(char c);
	unsigned int get_alpha_source(char c);
	unsigned int get_alpha_operand(char c);

	struct texture_elem {
		unsigned int obj_num; // needed?
		char mat_type;
		char* col_type;
		unsigned int tex_count;
		string* tex_names;
		GLuint* textures;

		// for multi texturing
		char* rgb_combine;
		char* alpha_combine;
		char** rgb_source;
		char** rgb_operand;
		char** alpha_source;
		char** alpha_operand;
	};

	vector<texture_elem> textures;

};

#endif
