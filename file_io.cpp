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

/*! opens a input file stream
 *  @param filename the name of the file
 *  @param binary flag if we read the file content binary of ascii wise
 */
void file_io::open_file(char* filename, bool binary) {
	if(binary) { file_io::filestream.open(filename, fstream::in | fstream::binary); }
	else { file_io::filestream.open(filename, fstream::in); }
	if(!file_io::filestream.is_open())
	{
		m.print(msg::MDEBUG, "file_io.cpp", "error while loading file \"%s\"!", filename);
	}
}

/*! closes the input file stream
 */
void file_io::close_file() {
	file_io::filestream.close();
}

/*! reads a line from the current input stream (senseless if we have a binary file)
 *  @param finput a pointer to a char where the line is written to
 */
void file_io::get_line(char* finput) {
	file_io::filestream.getline(finput, 65536);
}

/*! reads a block of -size- bytes
 *  @param data a pointer to a char where the block is written to
 *  @param size the amount of bytes we want to get
 */
void file_io::get_block(char* data, unsigned int size) {
	file_io::filestream.read(data, size);
}

/*! reads a single char from the current file input stream and returns it
 */
char file_io::get_char() {
	char c;
	file_io::filestream.get(c);
	return c;
}
