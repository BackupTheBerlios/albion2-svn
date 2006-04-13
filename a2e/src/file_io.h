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
#include "msg.h"
#include <SDL/SDL.h>
using namespace std;

#include "win_dll_export.h"

/*! @class file_io
 *  @brief file input/output
 *  @author flo
 *  @todo more functions
 *  
 *  the file input/output class
 */

class A2E_API file_io
{
public:
	file_io(msg* m);
	~file_io();

	enum FIO_OPEN_TYPE {
		OT_READ,
		OT_READWRITE,
		OT_WRITE,
		OT_READ_BINARY,
		OT_READWRITE_BINARY,
		OT_WRITE_BINARY
	};

	bool open_file(const char* filename, FIO_OPEN_TYPE open_type);
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
	void put_uint(unsigned int uint);
	void put_swap_uint(unsigned int uint);
	void put_float(float flt);
	void put_bool(bool b);
	void put_char(char c);

	bool is_file(char* filename);

protected:
	msg* m;
	fstream filestream;

	bool check_open();

};

#endif
