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
#include <list>
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
	char get_color_type(unsigned int obj_num);

	enum MAT_TYPES {
		NONE = 0x00,
		DIFFUSE = 0x01,
		BUMP = 0x02,
		PARALLAX = 0x03
	};

protected:
	msg* m;
	core* c;
	file_io* file;
	engine* e;
	texman* t;

	struct texture_elem {
		unsigned int obj_num; // needed?
		char mat_type;
		char col_type;
		string* tex_names;
		GLuint* textures;
	};

	list<texture_elem> textures;

};

#endif
