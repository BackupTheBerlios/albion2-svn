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
	writer = NULL;
	fname = NULL;
	fname_write = NULL;

	inner_loop = 0;
}

/*! delete everything
 */
xml::~xml() {
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
		m->print(msg::MERROR, "xml.cpp", "end(): failure while parsing %s!", fname);
	}
}

char* xml::get_node_name() {
	return (char*)xmlTextReaderExpand(*reader)->name;
}

int xml::get_attribute_count() {
	return xmlTextReaderAttributeCount(*reader);
}

const char* xml::get_attribute(char* att_name) {
	return (const char*)xmlTextReaderGetAttribute(*reader, (const xmlChar*)att_name);
}

char* xml::get_nattribute(unsigned int num) {
	return (char*)xmlTextReaderGetAttributeNo(*reader, num);
}


bool xml::open_write(char* filename) {
	if(writer != NULL) {
		m->print(msg::MERROR, "xml.cpp", "open_write(): can't write two files at the same time!");
		return false;
	}

	fname_write = filename;
	writer = new xmlTextWriterPtr();
	*writer = xmlNewTextWriterFilename(filename, 0);
	if(writer == NULL) {
		m->print(msg::MERROR, "xml.cpp", "open_write(): unable to open %s!", fname_write);
		return false;
	}

	/*if(xmlTextWriterStartDocument(*writer, NULL, "UTF-8", NULL) < 0) {
		m->print(msg::MERROR, "xml.cpp", "open_write(): unable to start document %s!", fname_write);
		close_write();
		return false;
	}*/
	xmlTextWriterWriteRaw(*writer, (const xmlChar*)"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n");

	inner_loop = 0;

	return true;
}

void xml::close_write() {
	xmlMemoryDump();
    xmlFreeTextWriter(*writer);
	delete writer;
	writer = NULL;
}

void xml::start_element(const char* name) {
	if(inner_loop % 2 == 1) {
		xmlTextWriterWriteRaw(*writer, (const xmlChar*)"\r\n");
		inner_loop++;
	}
	for(unsigned int i = 0; i < inner_loop / 2; i++) {
		xmlTextWriterWriteRaw(*writer, (const xmlChar*)"\t");
	}
	if(xmlTextWriterStartElement(*writer, BAD_CAST name) < 0) {
		m->print(msg::MERROR, "xml.cpp", "start_element(): unable to start element in file %s!", fname_write);
	}
	if(inner_loop % 2 == 0) inner_loop++;
}

void xml::end_element() {
	if(inner_loop % 2 == 0) {
		for(unsigned int i = 0; i < (inner_loop / 2)-1; i++) {
			xmlTextWriterWriteRaw(*writer, (const xmlChar*)"\t");
		}
	}
	if(xmlTextWriterEndElement(*writer) < 0) {
		m->print(msg::MERROR, "xml.cpp", "start_element(): unable to end element in file %s!", fname_write);
	}
	xmlTextWriterWriteRaw(*writer, (const xmlChar*)"\r\n");

	inner_loop -= inner_loop % 2 == 0 ? 2 : 1;
}

void xml::write_attribute(const char* attribute, const char* value) {
	if(xmlTextWriterWriteAttribute(*writer, BAD_CAST attribute, BAD_CAST value) < 0) {
		m->print(msg::MERROR, "xml.cpp", "start_element(): error while writing attribute %s in file %s!",
			attribute, fname_write);
	}
}
