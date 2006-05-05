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

/*! there is no function currently
 */
file_io::file_io(msg* m) {
	file_io::m = m;
}

/*! there is no function currently
 */
file_io::~file_io() {
}

/*! opens a input file stream
 *  @param filename the name of the file
 *  @param open_type enum that specifies how we want to open the file (like "r", "wb", etc. ...)
 */
bool file_io::open_file(const char* filename, FIO_OPEN_TYPE open_type) {
	if(check_open()) {
		m->print(msg::MERROR, "file_io.cpp", "open_file(): a file is already opened! can't open another file!");
		return false;
	}

	switch(open_type) {
		case file_io::OT_READ:
			file_io::filestream.open(filename, fstream::in);
			break;
		case file_io::OT_READWRITE:
			file_io::filestream.open(filename, fstream::in | fstream::out);
			break;
		case file_io::OT_WRITE:
			file_io::filestream.open(filename, fstream::out);
			break;
		case file_io::OT_READ_BINARY:
			file_io::filestream.open(filename, fstream::in | fstream::binary);
			break;
		case file_io::OT_READWRITE_BINARY:
			file_io::filestream.open(filename, fstream::in | fstream::out | fstream::binary);
			break;
		case file_io::OT_WRITE_BINARY:
			file_io::filestream.open(filename, fstream::out | fstream::binary);
			break;
		default:
			file_io::filestream.open(filename, fstream::in);
			break;
	}

	if(!file_io::filestream.is_open()) {
		m->print(msg::MDEBUG, "file_io.cpp", "open_file(): error while loading file %s!", filename);
		file_io::filestream.clear();
		return false;
	}
	return true;
}

/*! closes the input file stream
 */
void file_io::close_file() {
	file_io::filestream.close();
	file_io::filestream.clear();
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

/*! reads a single int from the current file input stream and returns it
 */
int file_io::get_int() {
	int i = 0;
	char* tmp = new char[4];
	file_io::filestream.read(tmp, 4);
	i = ((tmp[0] & 0xFF) << 24) + ((tmp[1] & 0xFF) << 16) + ((tmp[2] & 0xFF) << 8) + (tmp[3] & 0xFF);
	delete [] tmp;

	return i;
}

/*! reads a single unsigned int from the current file input stream and returns it
 */
unsigned int file_io::get_uint() {
	unsigned int u = 0;
	char* tmp = new char[4];
	file_io::filestream.read(tmp, 4);
	u = ((tmp[0] & 0xFF) << 24) + ((tmp[1] & 0xFF) << 16) + ((tmp[2] & 0xFF) << 8) + (tmp[3] & 0xFF);
	delete [] tmp;

	return u;
}

/*! reads a single float from the current file input stream and returns it
 */
float file_io::get_float() {
	float f = 0.0f;
	char* tmp = new char[4];
	file_io::filestream.read(tmp, 4);
	memcpy(&f, tmp, 4);
	delete [] tmp;

	return f;
}

/*! returns the filesize
 */
unsigned int file_io::get_filesize() {
	// get current get pointer position
	unsigned int cur_position = file_io::filestream.tellg();

	// get the file size
	file_io::filestream.seekg(0, ios::end);
	unsigned int size = file_io::filestream.tellg();
	file_io::filestream.seekg(0, ios::beg);

	// reset get pointer position
	file_io::filestream.seekg(cur_position, ios::beg);

	// return file size
	return size;
}

/*! seeks to offset
 *  @param offset the offset we you want to seek the file
 */
void file_io::seek(unsigned int offset) {
	file_io::filestream.seekg(offset, ios::beg);
}

/*! returns the current file offset
 */
unsigned int file_io::get_current_offset() {
	return file_io::filestream.tellg();
}

/*! writes a block to the current file (offset)
 *  @param data a pointer to the block we want to write
 *  @param size the size of the block
 */
void file_io::write_block(char* data, unsigned int size) {
	file_io::filestream.write(data, (streamsize)strlen(data));
	if(strlen(data) < size) {
		unsigned int x = size - strlen(data);
		for(unsigned int i = 0; i < x; i++) {
			put_char(0x00);
		}
	}
}

/*! puts an unsigned int into the current file stream
 *  @param uint the unsigned int we want to write/put into the file stream
 */
void file_io::put_uint(unsigned int uint) {
	file_io::filestream.put((uint >> 24) & 0xFF);
	file_io::filestream.put((uint >> 16) & 0xFF);
	file_io::filestream.put((uint >> 8) & 0xFF);
	file_io::filestream.put(uint & 0xFF);
}

/*! puts a swapped unsigned int into the current file stream
 *  @param uint the unsigned int we want to write/put into the file stream
 */
void file_io::put_swap_uint(unsigned int uint) {
	file_io::filestream.put(uint & 0xFF);
	file_io::filestream.put((uint >> 8) & 0xFF);
	file_io::filestream.put((uint >> 16) & 0xFF);
	file_io::filestream.put((uint >> 24) & 0xFF);
}

/*! puts a float into the current file stream
 *  @param flt the float we want to write/put into the file stream
 */
void file_io::put_float(float flt) {
	char* tmp = new char[4];
	memcpy(tmp, &flt, 4);
	file_io::filestream.write(tmp, 4);
	delete [] tmp;
}

/*! puts a bool into the current file stream
 *  @param b the bool we want to write/put into the file stream
 */
void file_io::put_bool(bool b) {
	b ? file_io::filestream.put(0x01) : file_io::filestream.put(0x00);
}

/*! puts a char into the current file stream
 *  @param c the char we want to write/put into the file stream
 */
void file_io::put_char(char c) {
	file_io::filestream.put(c);
}

/*! returns true if the file specified by filename exists
 *  @param filename the name of the file
 */
bool file_io::is_file(char* filename) {
	if(check_open()) {
		m->print(msg::MERROR, "file_io.cpp", "is_file(): a file is already opened! can't open another file!");
		return false;
	}

	file_io::filestream.open(filename, fstream::in | fstream::binary);

	if(!file_io::filestream.is_open()) {
		file_io::filestream.clear();
		return false;
	}

	file_io::filestream.close();
	file_io::filestream.clear();
	return true;
}

/*! checks if a file is already opened - if so, return true, otherwise false
 */
bool file_io::check_open() {
	if(file_io::filestream.is_open()) {
		file_io::filestream.clear();
		return true;
	}
	return false;
}


/*! checks if we reached the end of file ...
 */
bool file_io::eof() {
	return file_io::filestream.eof();
}
