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

#ifndef __XML_H__
#define __XML_H__

#ifdef WIN32
#include <windows.h>
#include <winnt.h>
#endif

#include <iostream>
#include <string>
#include <cmath>
#include <libxml/xmlreader.h>
#include "msg.h"
#include "core.h"

#include "win_dll_export.h"
using namespace std;

/*! @class xml
 *  @brief xml functions
 *  @author flo
 *
 *  some functions for parsing xml files, etc. ...
 */

class A2E_API xml
{
public:
	xml(msg* m);
	~xml();

	bool open(char* filename);
	void close();
	bool process();
	void end(int ret);

	char* get_node_name();
	int get_attribute_count();
	const char* get_attribute(char* att_name);
	char* get_nattribute(unsigned int num);

protected:
	msg* m;
	core* c;

	xmlTextReaderPtr* reader;
	char* fname;

	char* tmpc;
	string tmp;

};

#endif
