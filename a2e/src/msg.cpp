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
 
#include <iostream>
#include <cstdarg>
using namespace std;
#include "msg.h"

/*! create and initialize the msg object
 */
msg::msg() {
	msg::err_counter = 0;
	msg::log = true;
	msg::file = new fstream();
	msg::file->open("log.txt", fstream::out);
}

/*! close/delete everything
 */
msg::~msg() {
	file->close();
	delete file;
}

/*! prints a message to the console
 *  @param type the type of msg that is printed (0 = normal msg; 1 = error msg; 2 = debug msg)
 *  @param file the file where the error has occurred respectively from where the msg is printed out
 *  @param str the error string
 */
void msg::print(unsigned int type, const char* file, const char* str, ...)
{
	va_list	argc;
	unsigned int msg_size = 256;
	char* ostr = new char[msg_size];

	switch(type) {
		// normal msg output
		case msg::MMSG:
			if(str == NULL) {
				cout << "MSG: " << file << endl;
				if(msg::log) {
					*(msg::file) << "MSG: " << file << endl;
				}
			}
			else if(file == NULL) {
				cout << "MSG: ";

				va_start(argc, str);
					vsnprintf(ostr, msg_size, str, argc);
				va_end(argc);

				cout << ostr << endl;

				if(msg::log) {
					*(msg::file) << "MSG: " << ostr << endl;
				}
			}
			else {
				cout << "MSG: " << file << ": ";

				va_start(argc, str);
					vsnprintf(ostr, msg_size, str, argc);
				va_end(argc);

				cout << ostr << endl;

				if(msg::log) {
					*(msg::file) << "MSG: " << file << ": " << ostr << endl;
				}
			}
			break;
		// error msg output
		case msg::MERROR:
			err_counter++;
			if(str == NULL) {
				cerr << "ERROR #" << err_counter << ": " << file << endl;

				if(msg::log) {
					*(msg::file) << "ERROR #" << err_counter << ": " << file << endl;
				}
			}
			else if(file == NULL) {
				cerr << "ERROR #" << err_counter << ": ";

				va_start(argc, str);
					vsnprintf(ostr, msg_size, str, argc);
				va_end(argc);

				cout << ostr << endl;

				if(msg::log) {
					*(msg::file) << "ERROR #" << err_counter << ": " << ostr << endl;
				}
			}
			else {
				cerr << "ERROR #" << err_counter << ": " << file << ": ";

				va_start(argc, str);
					vsnprintf(ostr, msg_size, str, argc);
				va_end(argc);

				cout << ostr << endl;

				if(msg::log) {
					*(msg::file) << "ERROR #" << err_counter << ": " << file << ": " << ostr << endl;
				}
			}
			break;
		// debug msg output
		case msg::MDEBUG:
			if(str == NULL) {
				cout << "DEBUG: " << file << endl;

				if(msg::log) {
					*(msg::file) << "DEBUG: " << file << endl;
				}
			}
			else if(file == NULL) {
				cout << "DEBUG: ";

				va_start(argc, str);
					vsnprintf(ostr, msg_size, str, argc);
				va_end(argc);

				cout << ostr << endl;

				if(msg::log) {
					*(msg::file) << "DEBUG: " << ostr << endl;
				}
			}
			else {
				cout << "DEBUG: " << file << ": ";

				va_start(argc, str);
					vsnprintf(ostr, msg_size, str, argc);
				va_end(argc);

				cout << ostr << endl;

				if(msg::log) {
					*(msg::file) << "DEBUG: " << file << ": " << ostr << endl;
				}
			}
			break;
	}

	delete [] ostr;
}

/*! gets user console input and returns it as a char*
 *  @param length the length of the string that should be read out
 *  @param str the string to which the input is written
 */
void msg::scan(unsigned int length, char* str) {
	char input[512];
	cin.getline(input, length);
	for(int i = 0; i < 512; i++) {
		str[i] = input[i];
	}
}

/*! sets the log mode
 *  @param mode the mode (true = enabled, false = disabled)
 */
void msg::set_log_mode(bool mode) {
	msg::log = mode;
}

/*! returns true if logging is enabled and false if it is disabled
 */
bool msg::get_log_mode() {
	return msg::log;
}
