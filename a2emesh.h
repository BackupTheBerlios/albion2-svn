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

#ifndef __A2EMESH_H__
#define __A2EMESH_H__

#ifdef WIN32
#include <windows.h>
#endif

#include <iostream>
#include <SDL.h>
#include <math.h>
#include <gl\gl.h>
#include <gl\glu.h>
#include "msg.h"
#include "core.h"
#include "file_io.h"
using namespace std;

#include "win_dll_export.h"

/*! @class a2emesh
 *  @brief class for loading and displaying an a2e mesh
 *  @author flo
 *  @version 0.1
 *  @date 2004/08/26
 *  @todo more functions
 *  
 *  the a2emesh class
 */

class A2E_API a2emesh
{
public:
	a2emesh();
	~a2emesh();

	void load_mesh(char* filename);
	void draw_mesh();

protected:
	msg m;
	file_io file;
	core c;

	char mesh_type[8];
	char mesh_name[9];
	unsigned int vertex_count;
	unsigned int index_count;

	core::vertex* vertices;
	core::triangle* indices;
};

#endif
