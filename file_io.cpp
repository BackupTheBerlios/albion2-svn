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

#include "file_io.h"
#include "msg.h"

/*! there is no function currently
 */
file_io::file_io() {
}

/*! there is no function currently
 */
file_io::~file_io() {
}

void file_io::open_file(char* filename) {
	file_io::filestream.open(filename, fstream::in);
	if(!file_io::filestream.is_open())
	{
		m.print(msg::MDEBUG, "file_io.cpp", "error while loading file \"%s\"!", filename);
	}
}

void file_io::close_file() {
	file_io::filestream.close();
}

void file_io::get_line(char* finput) {
	file_io::filestream.getline(finput, 65536);
}
