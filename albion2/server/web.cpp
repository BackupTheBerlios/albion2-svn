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

#include "web.h"

web::web(engine* e) {
	web::e = e;
	web::c = e->get_core();
	web::m = e->get_msg();
	web::fio = e->get_file_io();
	web::x = e->get_xml();

	buffer = new stringstream(stringstream::in | stringstream::out | stringstream::binary);
	data = new string();
}

web::~web() {
	delete buffer;
	data->clear();
	delete data;
}

void web::load_file(const char* file) {
	*data = "";
	fio->open_file(file, file_io::OT_READ);

	// get file content
	char* block = new char[65536];
	while(!fio->eof()) {
		fio->get_line(block);
		*data += block;
		*data += "\n";
	}
	delete [] block;

	fio->close_file();
}

string* web::get_data() {
	return data;
}

void web::replace(const char* find, const char* replace) {
	size_t pos;
	while((pos = data->find(find, 0)) != string::npos) {
		data->replace(pos, strlen(find), replace, strlen(replace));
	}
}
