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
 
#ifndef __FILE_IO_H__
#define __FILE_IO_H__

#include <iostream>
#include <fstream>
#include <SDL/SDL.h>
using namespace std;

#include "win_dll_export.h"

/*! @class file_io
 *  @brief file input/output
 *  @author flo
 *  @version 0.1.1
 *  @todo more functions
 *  
 *  the file input/output class
 */

class A2E_API file_io
{
public:
	file_io();
	~file_io();

	void open_file(char* filename, bool binary);
	void close_file();
	unsigned int get_filesize();
	void get_line(char* finput);
	void get_block(char* data, unsigned int size);
	char get_char();
	int get_int();
	unsigned int get_uint();
	float get_float();
	void seek(unsigned int offset);
	unsigned int get_current_offset();
	void write_block(char* data, unsigned int size);

protected:
	fstream filestream;

};

#endif
