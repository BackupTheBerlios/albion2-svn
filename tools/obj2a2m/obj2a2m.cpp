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

#include "obj2a2m.h"

/*!
 * \mainpage
 *
 * \author flo
 *
 * \date v1: August 2004 - November 2005 /// v2: January - February 2006
 *
 * Albion 2 Engine Tool - Alias Wavefront .obj -> A2E .a2m Converter
 *
 *
 * little specification of the A2E static model format:
 *
 * [A2EMODEL - 8 bytes]
 * [TYPE - 1 byte (0x00)]
 * [VERTEX/TC COUNT - 4 bytes]
 * [VERTICES - 4 bytes * 3 * VERTEX/TC COUNT]
 * [TEXTURE COORDINATES - 4 bytes * 2 * VERTEX/TC COUNT]
 * [OBJECT COUNT - 4 bytes]
 * [FOR EACH OBJECT COUNT]
 * 	[INDEX COUNT - 4 bytes]
 * 	[INDICES - 4 bytes * 3 * INDEX COUNT]
 * [END FOR]
 */

void put_uint(fstream* file, unsigned int uint) {
	file->put((uint >> 24) & 0xFF);
	file->put((uint >> 16) & 0xFF);
	file->put((uint >> 8) & 0xFF);
	file->put(uint & 0xFF);
}

void put_float(fstream* file, float flt) {
	char* tmp = new char[4];
	memcpy(tmp, &flt, 4);
	file->write(tmp, 4);
	delete [] tmp;
}

int main(int argc, char *argv[]) {
	string usage = "usage: obj2a2m <file.obj> <file.a2m>";
	if(argc == 1) {
		cout << "no .obj and .a2m file specified!" << endl << usage << endl;
		return 0;
	}
	else if(argc == 2) {
		cout << "no .obj or .a2m file specified!" << endl << usage << endl;
		return 0;
	}

	char* obj_filename;
	char* a2m_filename;
	if(argc == 4 && strcmp(argv[1], "-blender") == 0) {
		blender = true;

		obj_filename = argv[2];
		a2m_filename = argv[3];
	}
	else {
		obj_filename = argv[1];
		a2m_filename = argv[2];
	}
	/*char* obj_filename = "schwert.obj";
	char* a2m_filename = "schwert.a2m";
	blender = true;*/

	cout << "converting " << obj_filename << " to " << a2m_filename << " ..." << endl;

	fstream ifile;
	ifile.open(obj_filename, fstream::in | fstream::binary);
	if(!ifile.is_open() || ifile.fail()) {
		cout << "error opening .obj file!" << endl;
		return 0;
	}

	// .obj data loading
	string buffer;
	float x, y, z;
	bool obj = false;
	while(!ifile.eof()) {
		ifile >> buffer;

		if(!blender) {
			if(buffer == "g") {
				obj ? object_count++, obj = false : obj = true;
				if(obj) {
					ifile >> buffer;
					obj_names.push_back(*new string(buffer));
					if(obj_names.back() == "collision") {
						collision = true;
						col_object = (unsigned int)obj_names.size() - 1;
					}
				}
			}
		}
		else {
			if(buffer == "g") {
				ifile >> buffer;

				if(buffer.find('.', 0) != string::npos) 
					obj_names.push_back(*new string(buffer.substr(0, buffer.find('.', 0))));
				else
					obj_names.push_back(*new string(buffer));

				if(obj_names.back() == "collision") {
					collision = true;
					col_object = (unsigned int)obj_names.size() - 1;
				}
			}
		}

		buffer.clear();
	}

	ifile.clear();
	ifile.seekg(0, ios::beg);

	obj = false;
	object_count = 0;
	while(!ifile.eof()) {
		ifile >> buffer;

		if(buffer == "v") {
			if(collision && col_object == (object_count)) {
				if(col_vnum == -1) col_vnum = (unsigned int)vertices.size();
				ifile >> x;
				ifile >> y;
				ifile >> z;
				col_vertices.push_back(*new vertex3(x, y, z));
			}
			else {
				ifile >> x;
				ifile >> y;
				ifile >> z;
				vertices.push_back(*new vertex3(x, y, z));
			}
		}
		else if(buffer == "vt") {
			if(collision && col_object == (object_count)) {
				ifile >> x;
				ifile >> y;
				col_tex_count++;
				// do nothing with those ...
			}
			else {
				ifile >> x;
				ifile >> y;
				tex_coords.push_back(*new core::coord());
				tex_coords.back().u = x;
				tex_coords.back().v = y;
			}
		}
		else if(buffer == "g") {
			obj ? object_count++, obj = false : obj = true;
		}

		buffer.clear();
	}

	col_vertex_count = (unsigned int)col_vertices.size();

	// for blender
	if(blender) {
		object_count *= 2;
		object_count++;
	}

	//indices = new vector<core::index>[collision ? object_count-1 : object_count];
	ivertices.reserve(tex_coords.size());

	// 12.345678f indicates an uninitialized vertex
	vertex3 vui(12.345678f, 12.345678f, 12.345678f);
	for(unsigned int i = 0; i < tex_coords.size(); i++) {
		ivertices.push_back(*new vertex3(vui));
	}

	ifile.clear();
	ifile.seekg(0, ios::beg);
	msg* m = new msg();
	file_io* fio = new file_io(m);
	core* c = new core(m, fio);
	obj = false;
	if(blender) ginit = true;
	while(!ifile.eof()) {
		ifile >> buffer;

		if(buffer == "f" && (!collision || (collision && cur_object != col_object))) {
			indices.back().push_back(*new core::index);
			for(unsigned int i = 0; i < 3; i++) {
				ifile >> buffer;
				unsigned int dc = c->get_delimiter_count((char*)buffer.c_str(), 0x2F);
				if(dc == 0) {
					cout << "no texture coordinates specified! wrong file format?" << endl;
					return 0;
				}
				else if(dc > 2) {
					cout << "wrong file format!" << endl;
					return 0;
				}

				char** ind = c->tokenize((char*)buffer.c_str(), 0x2F);
				unsigned int vind = atoi(ind[0])-1;
				unsigned int tind = atoi(ind[1])-1;

				if(collision && cur_object >= col_object) {
					vind -= col_vertex_count;
					tind -= col_tex_count;
				}

				if(ivertices[tind] == vui) {
					ivertices[tind].set(&vertices.at(vind));
					if(i == 0) {
						indices.back().back().i1 = tind;
					}
					else if(i == 1) {
						indices.back().back().i2 = tind;
					}
					else {
						indices.back().back().i3 = tind;
					}
				}
				else if(ivertices[tind] != vui && ivertices[tind] != vertices.at(vind)) {
					bool found = false;
					for(unsigned int j = 0; j < tex_coords.size(); j++) {
						if(ivertices[j] == &vertices.at(vind)) {
							// equal vertex found
							if(tex_coords.at(j).u == tex_coords.at(tind).u &&
								tex_coords.at(j).v == tex_coords.at(tind).v) {
								// equal texture coordinate found
								found = true;
								if(i == 0) {
									indices.back().back().i1 = j;
								}
								else if(i == 1) {
									indices.back().back().i2 = j;
								}
								else {
									indices.back().back().i3 = j;
								}
							}
						}
					}

					if(!found) {
						// no equal vertex and texture coordinate found, add new ones
						ivertices.push_back(*new vertex3(vertices.at(vind)));
						tex_coords.push_back(*new core::coord());
						tex_coords.back().u = tex_coords.at(tind).u;
						tex_coords.back().v = tex_coords.at(tind).v;

						if(i == 0) {
							indices.back().back().i1 = (unsigned int)ivertices.size()-1;
						}
						else if(i == 1) {
							indices.back().back().i2 = (unsigned int)ivertices.size()-1;
						}
						else {
							indices.back().back().i3 = (unsigned int)ivertices.size()-1;
						}
					}
				}
				else {
					if(i == 0) {
						indices.back().back().i1 = tind;
					}
					else if(i == 1) {
						indices.back().back().i2 = tind;
					}
					else {
						indices.back().back().i3 = tind;
					}
				}
			}
		}
		else if(buffer == "f" && collision && cur_object == col_object) {
			col_indices.push_back(*new core::index);
			for(unsigned int i = 0; i < 3; i++) {
				ifile >> buffer;
				unsigned int dc = c->get_delimiter_count((char*)buffer.c_str(), 0x2F);
				if(dc == 0) {
					cout << "no texture coordinates specified! wrong file format?" << endl;
					return 0;
				}
				else if(dc > 2) {
					cout << "wrong file format!" << endl;
					return 0;
				}
				char** ind = c->tokenize((char*)buffer.c_str(), 0x2F);
				unsigned int vind = atoi(ind[0])-1;
				unsigned int tind = atoi(ind[1])-1;

				if(i == 0) col_indices.back().i1 = vind - col_vnum;
				else if(i == 1) col_indices.back().i2 = vind - col_vnum;
				else col_indices.back().i3 = vind - col_vnum;
			}
		}
		else if(buffer == "g") {
			if(!blender) {
				if((!ginit || obj) && (!collision || (collision && cur_object != col_object-1))) {
					indices.push_back(*new vector<core::index>);
				}
				ginit ? (obj ? cur_object++, obj = false : obj = true) : ginit = true;
			}
			else {
				indices.push_back(*new vector<core::index>);
				cur_object++;
			}
		}


		buffer.clear();
	}

	ifile.close();

	// write data to the .a2m file
	fstream ofile;
	ofile.open(a2m_filename, fstream::out | fstream::binary);

	ofile << "A2EMODEL" << char(collision ? 0x02 : 0x00);
	put_uint(&ofile, (unsigned int)tex_coords.size());

	for(unsigned int i = 0; i < tex_coords.size(); i++) {
		if(ivertices[i] != vui) {
			if(!blender) {
				put_float(&ofile, ivertices[i].x);
				put_float(&ofile, ivertices[i].z);
				put_float(&ofile, -ivertices[i].y);
			}
			else {
				put_float(&ofile, ivertices[i].x);
				put_float(&ofile, ivertices[i].y);
				put_float(&ofile, ivertices[i].z);
			}
		}
		else {
			cout << "unassigned vertex #" << i << "! (seems like the vertex is specified, but not used by any triangle - putting three 0.0f instead)" << endl;
			put_float(&ofile, 0.0f);
			put_float(&ofile, 0.0f);
			put_float(&ofile, 0.0f);
		}
	}

	for(vector<core::coord>::iterator tc_iter = tex_coords.begin(); tc_iter != tex_coords.end(); tc_iter++) {
		put_float(&ofile, tc_iter->u);
		put_float(&ofile, tc_iter->v);
	}

	put_uint(&ofile, collision ? object_count-1 : object_count);

	for(unsigned int i = 0; i < object_count; i++) {
		if(!collision || (collision && i != col_object)) {
			ofile.write(obj_names[i].c_str(), (unsigned int)obj_names[i].length());
			ofile << char(0xFF);
		}
	}

	for(vector<vector<core::index> >::iterator iter = indices.begin(); iter != indices.end(); iter++) {
		put_uint(&ofile, (unsigned int)iter->size());
		for(vector<core::index>::iterator ind_iter = iter->begin(); ind_iter != iter->end(); ind_iter++) {
			put_uint(&ofile, ind_iter->i1);
			put_uint(&ofile, ind_iter->i2);
			put_uint(&ofile, ind_iter->i3);
		}
	}

	if(collision) {
		put_uint(&ofile, (unsigned int)col_vertices.size());
		for(vector<vertex3>::iterator iter = col_vertices.begin(); iter != col_vertices.end(); iter++) {
			if(!blender) {
				put_float(&ofile, iter->x);
				put_float(&ofile, iter->z);
				put_float(&ofile, -iter->y);
			}
			else {
				put_float(&ofile, iter->x);
				put_float(&ofile, iter->y);
				put_float(&ofile, iter->z);
			}
		}
		put_uint(&ofile, (unsigned int)col_indices.size());
		for(vector<core::index>::iterator iter = col_indices.begin(); iter != col_indices.end(); iter++) {
			put_uint(&ofile, iter->i1);
			put_uint(&ofile, iter->i2);
			put_uint(&ofile, iter->i3);
		}
	}

	ofile.close();

	cout << "successfully converted " << obj_filename << " to " << a2m_filename << "!" << endl;
	cout << "object count: " << object_count << endl;

	delete c;
	delete fio;
	delete m;

#ifdef WIN32
	Sleep(1000);
#else
	usleep(1000000);
#endif

	return 0;
}
