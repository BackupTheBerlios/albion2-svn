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

#include "xml.h"

/*! create and initialize the xml class
 */
xml::xml(msg* m) {
	LIBXML_TEST_VERSION
	xml::m = m;

	reader = NULL;
	fname = NULL;

	tmp = NULL;
}

/*! delete everything
 */
xml::~xml() {
    if(tmp != NULL) { delete tmp; }
}

bool xml::open(char* filename) {
	if(reader != NULL) {
		m->print(msg::MERROR, "xml.cpp", "open(): can't parse two files at the same time!");
		return false;
	}

	fname = filename;
	reader = new xmlTextReaderPtr();
    *reader = xmlReaderForFile(filename, NULL, 0);
    if(reader == NULL) {
		m->print(msg::MERROR, "xml.cpp", "open(): unable to open %s!", fname);
		return false;
	}

	return true;
}

void xml::close() {
	xmlCleanupParser();
	xmlMemoryDump();
	delete reader;
	reader = NULL;
}

bool xml::process() {
	int ret = xmlTextReaderRead(*reader);
	if(ret != 1) {
		end(ret);
		return false;
	}
	return true;
}

void xml::end(int ret) {
	xmlFreeTextReader(*reader);
    if(ret != 0) {
		m->print(msg::MERROR, "xml.cpp", "open(): failure while parsing %s!", fname);
    }
}

char* xml::get_node_name() {
	xmlNodePtr node = xmlTextReaderExpand(*reader);
	return (char*)node->name;
}

int xml::get_attribute_count() {
	return xmlTextReaderAttributeCount(*reader);
}

char* xml::get_attribute(char* att_name) {
    if(tmp != NULL) { free(tmp); }
    tmp = (char*)xmlTextReaderGetAttribute(*reader, (const xmlChar*)att_name);
	return tmp;
}

char* xml::get_nattribute(unsigned int num) {
	return (char*)xmlTextReaderGetAttributeNo(*reader, num);
}
