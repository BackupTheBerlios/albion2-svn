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
 
#ifndef __MSG_H__
#define __MSG_H__

#include "win_dll_export.h"
#include <fstream>
using namespace std;

/*! @class msg
 *  @brief msg handling
 *  @author laxity
 *  @author flo
 *  @todo logfile support
 *
 *  This is the msg handling class
 */

class A2E_API msg
{
public:
	msg();
	~msg();

	void print(unsigned int type, const char* file, const char* str = NULL, ...);
	void scan(unsigned int length, char* str);
	void set_log_mode(bool mode);
	bool get_log_mode();

	enum PRINT
	{
		MMSG,	//!< enum simple message
		MERROR,	//!< enum error message
		MDEBUG	//!< enum debug message
	};

protected:
	unsigned long int err_counter;
	bool log;
	fstream* file;

};

#endif
