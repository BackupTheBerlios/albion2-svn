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
#include <iostream>
#include <fstream>
using namespace std;
#include <string.h>

#define MAX_OBJS 32

/*!
 * \mainpage
 *
 * \author flo
 *
 * \date August - December 2004
 *
 * Albion 2 Engine Tool - Alias Wavefront .obj -> A2E .a2m Converter
 *
 *
 * little specification of the A2E model format:
 *
 * [A2EMODEL - 8 bytes]
 * [NAME - 8 bytes]
 * [VERTEX COUNT - 4 bytes]
 * [VERTICES - 4 bytes * VERTEX COUNT]
 * [TEXTURE COUNT - 4 bytes]
 * [TEXTURE NAMES - 32 bytes * TEXTURE COUNT]
 * [OBJECT COUNT - 4 bytes]
 * [FOR EACH OBJECT COUNT]
 * 	[OBJECT NAME - 8 bytes]
 * 	[INDEX COUNT - 4 bytes]
 * 	[TEXTURE VALUE - 4 bytes]
 * 	[INDICES - 4 bytes * INDEX COUNT]
 * 	[TEXTURE COORDINATES - 4 bytes * INDEX COUNT * 3]
 * [END FOR]
 */

void load_materials(char* filename) {
	fstream mtlfile;
	mtlfile.open(filename, fstream::in | fstream::binary);
	if(!mtlfile.is_open()) {
		cout << "error opening .mtl file!" << endl;
		return;
	}
	char* mtlline = (char*)malloc(256);
	while(!mtlfile.eof()) {
		mtlfile.getline(mtlline, 256);
		switch(mtlline[0]) {
			case 'n': {
				// tokenize(tm) the line
				unsigned int x = 0;
				char* line_tok[8];
				line_tok[x] = strtok(mtlline, " ");
				while(line_tok[x] != NULL) {
					x++;
					line_tok[x] = strtok(NULL, " ");
				}

				if(strcmp(line_tok[0], "newmtl") == 0) {
					material_count++;
				}
			}
			break;
			default:
				break;
		}
	}

	mtlfile.clear();
	// seek to begining
	mtlfile.seekg(0, ios::beg);

	for(unsigned int i = 0; i < material_count; i++) {
		mat_names[i] = (char*)malloc(33);
		for(unsigned int j = 0; j < 33; j++) {
			mat_names[i][j] = 0;
		}
		tex_names[i] = (char*)malloc(33);
		for(unsigned int j = 0; j < 33; j++) {
			tex_names[i][j] = 0;
		}
	}

	unsigned int mc = 0;
	unsigned int tc = 0;

	while(!mtlfile.eof()) {
		mtlfile.getline(mtlline, 256);
		switch(mtlline[0]) {
			case 'n': {
				// tokenize(tm) the line
				unsigned int x = 0;
				char* line_tok[8];
				line_tok[x] = strtok(mtlline, " ");
				while(line_tok[x] != NULL) {
					x++;
					line_tok[x] = strtok(NULL, " ");
				}

				if(strcmp(line_tok[0], "newmtl") == 0) {
					sprintf(mat_names[mc], line_tok[1]);
					mc++;
				}
			}
			break;
			case 'm': {
				// tokenize(tm) the line
				unsigned int x = 0;
				char* line_tok[8];
				line_tok[x] = strtok(mtlline, " ");
				while(line_tok[x] != NULL) {
					x++;
					line_tok[x] = strtok(NULL, " ");
				}

				if(strcmp(line_tok[0], "map_Kd") == 0) {
					sprintf(tex_names[tc], line_tok[1]);
					texture_count++;
					tc++;
				}
			}
			break;
			default:
				break;
		}
	}

	free(mtlline);
	mtlfile.close();
}

int main(int argc, char *argv[])
{
	char* usage = "usage: obj_converter <file.obj> <file.a2m> [-3dsmax]";
	if(argc == 1) {
		cout << "no .obj and .a2m file specified!" << endl << usage << endl;
		return 0;
	}
	if(argc == 2) {
		cout << "no .obj or .a2m file specified!" << endl << usage << endl;
		return 0;
	}

	char* obj_filename = argv[1];
	char* a2m_filename = argv[2];

	fstream ofile;
	ofile.open(obj_filename, fstream::in | fstream::binary);
	if(!ofile.is_open()) {
		cout << "error opening .obj file!" << endl;
		return 0;
	}

	// init stuf
	index_count = (unsigned int*)malloc(sizeof(unsigned int)*MAX_OBJS);
	for(unsigned int i = 0; i < MAX_OBJS; i++) {
		index_count[i] = 0;
	}

	// end init

	char* line = (char*)malloc(256);
	// get the objs name and the vertex/index count
	while(!ofile.eof()) {
		ofile.getline(line, 256);
		switch(line[0]) {
			case 'g': {
				if(line[1] == ' ') {
					// tokenize(tm) the line
					unsigned int x = 0;
					char* line_tok[8];
					line_tok[x] = strtok(line, " ");
					while(line_tok[x] != NULL) {
						x++;
						line_tok[x] = strtok(NULL, " ");
					}

					obj_names[object_count] = (char*)malloc(9);
					for(unsigned int i = 0; i < 9; i++) {
						obj_names[object_count][i] = 0;
					}

					memcpy(obj_names[object_count], line_tok[1], 8);
					//obj_names[object_count][9] = 0;

					object_count++;
				}
			}
			break;
			case 'v':
				if(line[1] == ' ') {
                    vertex_count++;
				}
				else if(line[1] == 't') {
					tex_vertex_count++;
				}
				break;
			case 'f':
				index_count[object_count-1]++;
				total_index_count++;
				break;
			default:
				break;
		}
	}

	ofile.clear();
	// seek to begining
	ofile.seekg(0, ios::beg);

	vertices1 = (unsigned int*)malloc(sizeof(unsigned int)*vertex_count);
	vertices2 = (unsigned int*)malloc(sizeof(unsigned int)*vertex_count);
	vertices3 = (unsigned int*)malloc(sizeof(unsigned int)*vertex_count);

	texcords1 = (unsigned int*)malloc(sizeof(unsigned int)*tex_vertex_count);
	texcords2 = (unsigned int*)malloc(sizeof(unsigned int)*tex_vertex_count);
	texcords3 = (unsigned int*)malloc(sizeof(unsigned int)*tex_vertex_count);

	for(unsigned int i = 0; i < object_count; i++) {
		indices1[i] = (unsigned int*)malloc(sizeof(unsigned int)*index_count[i]);
		indices2[i] = (unsigned int*)malloc(sizeof(unsigned int)*index_count[i]);
		indices3[i] = (unsigned int*)malloc(sizeof(unsigned int)*index_count[i]);
		texindices1[i] = (unsigned int*)malloc(sizeof(unsigned int)*index_count[i]);
		texindices2[i] = (unsigned int*)malloc(sizeof(unsigned int)*index_count[i]);
		texindices3[i] = (unsigned int*)malloc(sizeof(unsigned int)*index_count[i]);
	}

	tex_number = (unsigned int*)malloc(sizeof(unsigned int)*object_count);

	unsigned int vc = 0;
	unsigned int ic = 0;
	unsigned int tc = 0;
	unsigned int oc = 0;
	bool is_first_obj = false;

	char* line_tok[8];

	// get the vertices and indices
	while(!ofile.eof()) {
		ofile.getline(line, 256);
		switch(line[0]) {
			case 'g': {
				if(line[1] == ' ') {
					is_first_obj = true;
				}
				else {
					if(is_first_obj) { oc++; ic = 0; }
				}
			}
			break;
			case 'v': {
				switch(line[1]) {
					case ' ': {
						// tokenize(tm) the line
						unsigned int x = 0;
						//char* line_tok[8];
						line_tok[x] = strtok(line, " ");
						while(line_tok[x] != NULL) {
							x++;
							line_tok[x] = strtok(NULL, " ");
						}

						float flt;

						flt = (float)atof(line_tok[1]);
						memcpy(&vertices1[vc], &flt, 4);
						flt = (float)atof(line_tok[2]);
						memcpy(&vertices2[vc], &flt, 4);
						flt = (float)atof(line_tok[3]);
						memcpy(&vertices3[vc], &flt, 4);

						vc++;
					}
					break;
					case 't': {
						// tokenize(tm) the line
						unsigned int x = 0;
						//char* line_tok[8];
						line_tok[x] = strtok(line, " ");
						while(line_tok[x] != NULL) {
							x++;
							line_tok[x] = strtok(NULL, " ");
						}

						float flt;

						flt = (float)atof(line_tok[1]);
						memcpy(&texcords1[tc], &flt, 4);
						flt = (float)atof(line_tok[2]);
						memcpy(&texcords2[tc], &flt, 4);
						flt = (float)atof(line_tok[3]);
						memcpy(&texcords3[tc], &flt, 4);

						tc++;
					}
					break;
					default:
						break;
				}
			}
			break;
			case 'f': {
				// tokenize(tm) the line
				unsigned int x = 0;
				//char* line_tok[8];
				line_tok[x] = strtok(line, " ");
				while(line_tok[x] != NULL) {
					x++;
					line_tok[x] = strtok(NULL, " ");
				}

				// tokenize(tm) the line_tok
				unsigned int y = 0;
				char* itok[8];
				itok[y] = strtok(line_tok[1], "/");
				while(itok[y] != NULL) {
					y++;
					itok[y] = strtok(NULL, "/");
				}
				indices1[oc][ic] = atoi(itok[0]) - 1;
				texindices1[oc][ic] = atoi(itok[1]) - 1;

				// and the same again ...
				y = 0;
				itok[y] = strtok(line_tok[2], "/");
				while(itok[y] != NULL) {
					y++;
					itok[y] = strtok(NULL, "/");
				}
				indices2[oc][ic] = atoi(itok[0]) - 1;
				texindices2[oc][ic] = atoi(itok[1]) - 1;

				// ... and again ...
				y = 0;
				itok[y] = strtok(line_tok[3], "/");
				while(itok[y] != NULL) {
					y++;
					itok[y] = strtok(NULL, "/");
				}
				indices3[oc][ic] = atoi(itok[0]) - 1;
				texindices3[oc][ic] = atoi(itok[1]) - 1;

				ic++;
			}
			break;
			case 'm': {
				// tokenize(tm) the line
				unsigned int x = 0;
				//char* line_tok[8];
				line_tok[x] = strtok(line, " ");
				while(line_tok[x] != NULL) {
					x++;
					line_tok[x] = strtok(NULL, " ");
				}

				if(strcmp(line_tok[0], "mtllib") == 0) {
					load_materials(line_tok[1]);
				}
			}
			break;
			case 'u': {
				// tokenize(tm) the line
				unsigned int x = 0;
				//char* line_tok[8];
				line_tok[x] = strtok(line, " ");
				while(line_tok[x] != NULL) {
					x++;
					line_tok[x] = strtok(NULL, " ");
				}

				if(strcmp(line_tok[0], "usemtl") == 0) {
					for(unsigned int i = 0; i < material_count; i++) {
						if(strcmp(line_tok[1], mat_names[i]) == 0) {
							tex_number[oc] = i;
						}
					}
				}
			}
			break;
			default:
				break;
		}
	}
	free(line);

	ofile.close();

	char tmp[4];
	fstream afile;
	afile.open(a2m_filename, fstream::out | fstream::binary);

	char* a2m_type = "A2EMODEL";
	afile.write(a2m_type, 8);
	afile.write(model_name, 8);

	tmp[0] = (char)(vertex_count >> 24) & 0xFF;
	tmp[1] = (char)(vertex_count >> 16) & 0xFF;
	tmp[2] = (char)(vertex_count >> 8) & 0xFF;
	tmp[3] = (char)(vertex_count & 0xFF);
	afile.write(tmp, 4);

	for(unsigned int i = 0; i < vertex_count; i++) {
		tmp[3] = (char)(vertices1[i] >> 24) & 0xFF;
		tmp[2] = (char)(vertices1[i] >> 16) & 0xFF;
		tmp[1] = (char)(vertices1[i] >> 8) & 0xFF;
		tmp[0] = (char)(vertices1[i] & 0xFF);
		afile.write(tmp, 4);

		tmp[3] = (char)(vertices2[i] >> 24) & 0xFF;
		tmp[2] = (char)(vertices2[i] >> 16) & 0xFF;
		tmp[1] = (char)(vertices2[i] >> 8) & 0xFF;
		tmp[0] = (char)(vertices2[i] & 0xFF);
		afile.write(tmp, 4);

		tmp[3] = (char)(vertices3[i] >> 24) & 0xFF;
		tmp[2] = (char)(vertices3[i] >> 16) & 0xFF;
		tmp[1] = (char)(vertices3[i] >> 8) & 0xFF;
		tmp[0] = (char)(vertices3[i] & 0xFF);
		afile.write(tmp, 4);
	}

	tmp[0] = (char)(texture_count >> 24) & 0xFF;
	tmp[1] = (char)(texture_count >> 16) & 0xFF;
	tmp[2] = (char)(texture_count >> 8) & 0xFF;
	tmp[3] = (char)(texture_count & 0xFF);
	afile.write(tmp, 4);

	for(unsigned int i = 0; i < texture_count; i++) {
		afile.write(tex_names[i], 32);
	}

	tmp[0] = (object_count >> 24) & 0xFF;
	tmp[1] = (object_count >> 16) & 0xFF;
	tmp[2] = (object_count >> 8) & 0xFF;
	tmp[3] = object_count & 0xFF;
	afile.write(tmp, 4);

	for(unsigned int i = 0; i < object_count; i++) {
		afile.write(obj_names[i], 8);

		tmp[0] = (index_count[i] >> 24) & 0xFF;
		tmp[1] = (index_count[i] >> 16) & 0xFF;
		tmp[2] = (index_count[i] >> 8) & 0xFF;
		tmp[3] = index_count[i] & 0xFF;
		afile.write(tmp, 4);

		tmp[0] = (tex_number[i] >> 24) & 0xFF;
		tmp[1] = (tex_number[i] >> 16) & 0xFF;
		tmp[2] = (tex_number[i] >> 8) & 0xFF;
		tmp[3] = tex_number[i] & 0xFF;
		afile.write(tmp, 4);

		for(unsigned int j = 0; j < index_count[i]; j++) {
			tmp[0] = (char)(indices1[i][j] >> 24) & 0xFF;
			tmp[1] = (char)(indices1[i][j] >> 16) & 0xFF;
			tmp[2] = (char)(indices1[i][j] >> 8) & 0xFF;
			tmp[3] = (char)(indices1[i][j] & 0xFF);
			afile.write(tmp, 4);

			tmp[0] = (char)(indices2[i][j] >> 24) & 0xFF;
			tmp[1] = (char)(indices2[i][j] >> 16) & 0xFF;
			tmp[2] = (char)(indices2[i][j] >> 8) & 0xFF;
			tmp[3] = (char)(indices2[i][j] & 0xFF);
			afile.write(tmp, 4);

			tmp[0] = (char)(indices3[i][j] >> 24) & 0xFF;
			tmp[1] = (char)(indices3[i][j] >> 16) & 0xFF;
			tmp[2] = (char)(indices3[i][j] >> 8) & 0xFF;
			tmp[3] = (char)(indices3[i][j] & 0xFF);
			afile.write(tmp, 4);
		}

		for(unsigned int j = 0; j < index_count[i]; j++) {
			tmp[3] = (char)(texcords1[texindices1[i][j]] >> 24) & 0xFF;
			tmp[2] = (char)(texcords1[texindices1[i][j]] >> 16) & 0xFF;
			tmp[1] = (char)(texcords1[texindices1[i][j]] >> 8) & 0xFF;
			tmp[0] = (char)(texcords1[texindices1[i][j]] & 0xFF);
			afile.write(tmp, 4);

			tmp[3] = (char)(texcords2[texindices1[i][j]] >> 24) & 0xFF;
			tmp[2] = (char)(texcords2[texindices1[i][j]] >> 16) & 0xFF;
			tmp[1] = (char)(texcords2[texindices1[i][j]] >> 8) & 0xFF;
			tmp[0] = (char)(texcords2[texindices1[i][j]] & 0xFF);
			afile.write(tmp, 4);

			tmp[3] = (char)(texcords3[texindices1[i][j]] >> 24) & 0xFF;
			tmp[2] = (char)(texcords3[texindices1[i][j]] >> 16) & 0xFF;
			tmp[1] = (char)(texcords3[texindices1[i][j]] >> 8) & 0xFF;
			tmp[0] = (char)(texcords3[texindices1[i][j]] & 0xFF);
			afile.write(tmp, 4);

			tmp[3] = (char)(texcords1[texindices2[i][j]] >> 24) & 0xFF;
			tmp[2] = (char)(texcords1[texindices2[i][j]] >> 16) & 0xFF;
			tmp[1] = (char)(texcords1[texindices2[i][j]] >> 8) & 0xFF;
			tmp[0] = (char)(texcords1[texindices2[i][j]] & 0xFF);
			afile.write(tmp, 4);

			tmp[3] = (char)(texcords2[texindices2[i][j]] >> 24) & 0xFF;
			tmp[2] = (char)(texcords2[texindices2[i][j]] >> 16) & 0xFF;
			tmp[1] = (char)(texcords2[texindices2[i][j]] >> 8) & 0xFF;
			tmp[0] = (char)(texcords2[texindices2[i][j]] & 0xFF);
			afile.write(tmp, 4);

			tmp[3] = (char)(texcords3[texindices2[i][j]] >> 24) & 0xFF;
			tmp[2] = (char)(texcords3[texindices2[i][j]] >> 16) & 0xFF;
			tmp[1] = (char)(texcords3[texindices2[i][j]] >> 8) & 0xFF;
			tmp[0] = (char)(texcords3[texindices2[i][j]] & 0xFF);
			afile.write(tmp, 4);

			tmp[3] = (char)(texcords1[texindices3[i][j]] >> 24) & 0xFF;
			tmp[2] = (char)(texcords1[texindices3[i][j]] >> 16) & 0xFF;
			tmp[1] = (char)(texcords1[texindices3[i][j]] >> 8) & 0xFF;
			tmp[0] = (char)(texcords1[texindices3[i][j]] & 0xFF);
			afile.write(tmp, 4);

			tmp[3] = (char)(texcords2[texindices3[i][j]] >> 24) & 0xFF;
			tmp[2] = (char)(texcords2[texindices3[i][j]] >> 16) & 0xFF;
			tmp[1] = (char)(texcords2[texindices3[i][j]] >> 8) & 0xFF;
			tmp[0] = (char)(texcords2[texindices3[i][j]] & 0xFF);
			afile.write(tmp, 4);

			tmp[3] = (char)(texcords3[texindices3[i][j]] >> 24) & 0xFF;
			tmp[2] = (char)(texcords3[texindices3[i][j]] >> 16) & 0xFF;
			tmp[1] = (char)(texcords3[texindices3[i][j]] >> 8) & 0xFF;
			tmp[0] = (char)(texcords3[texindices3[i][j]] & 0xFF);
			afile.write(tmp, 4);
		}
	}
	
	afile.close();

	cout << vertex_count << " vertices, " << total_index_count << " indices and " << total_index_count*3 << " texture vertices successfully converted!" << endl;

	return 0;
}
