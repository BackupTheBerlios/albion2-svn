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

/*! Sets the counter to 0
 */
msg::msg():
	err_counter(0)
{
}

/*! Does nothing
 */
msg::~msg()
{
}

/*! Prints the error number and an error string
 *  @param type the type of msg that is printed (0 = normal msg; 1 = error msg; 2 = debug msg)
 *  @param file the file where the error has occurred respectively from where the msg is printed out
 *  @param str the error string
 */
void msg::print(unsigned int type, const char *file, const char *str, ...)
{
	va_list	argc;
	char* ostr = (char*)malloc(256);

	switch(type) {
		// normal msg output
		case msg::MMSG:
			if(str == NULL) {
				cout << "MSG: " << file << endl;
			}
			else if(file == NULL) {
				cout << "MSG: ";

				va_start(argc, str);
					vsprintf(ostr, str, argc);
				va_end(argc);

				cout << ostr << endl;
			}
			else {
				cout << "MSG: " << file << ": ";

				va_start(argc, str);
					vsprintf(ostr, str, argc);
				va_end(argc);

				cout << ostr << endl;
			}
			break;
		// error msg output
		case msg::MERROR:
			err_counter++;
			if(str == NULL) {
				cerr << "ERROR #" << err_counter << ": " << file << endl;
			}
			else {
				cerr << "ERROR #" << err_counter << ": " << file << ": ";

				va_start(argc, str);
					vsprintf(ostr, str, argc);
				va_end(argc);

				cout << ostr << endl;
			}
			break;
		// debug msg output
		case msg::MDEBUG:
			if(str == NULL) {
				cout << "DEBUG: " << file << endl;
			}
			else if(file == NULL) {
				cout << "DEBUG: ";

				va_start(argc, str);
					vsprintf(ostr, str, argc);
				va_end(argc);

				cout << ostr << endl;
			}
			else {
				cout << "DEBUG: " << file << ": ";

				va_start(argc, str);
					vsprintf(ostr, str, argc);
				va_end(argc);

				cout << ostr << endl;
			}
			break;
	}

	free(ostr);
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
