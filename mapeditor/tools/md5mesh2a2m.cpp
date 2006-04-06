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

#include "md5mesh2a2m.h"

/*!
 * \mainpage
 *
 * \author flo
 * \author julien rebetez (.md5mesh loading routine)
 *
 * \date May 2005
 *
 * Albion 2 Engine - MD5MESH to A2M Converter
 */

void put_int(fstream* file, int integer) {
	file->put((integer >> 24) & 0xFF);
	file->put((integer >> 16) & 0xFF);
	file->put((integer >> 8) & 0xFF);
	file->put(integer & 0xFF);
}

void put_float(fstream* file, float flt) {
	char* tmp = new char[4];
	memcpy(tmp, &flt, 4);
	file->write(tmp, 4);
	delete tmp;
}

bool LoadMesh(char* str) {
	ifstream pFD;
	string buffer;
	int version;

	pFD.open(str, ios::in);

	if(pFD.fail()) {
		cout << "error loading mesh file: " << str << endl;
		return false;
	}

	pFD >> buffer;
	pFD >> version;
	
	if(version != 10) {
		cout << "non supported MD5 version: " << version << endl;
		return false;
	}
	
	NEXT("numJoints")
	pFD >> iNumJoints;
	
	NEXT("numMeshes")
	pFD >> iNumMeshes;
	
	iNumBaseJoints = 0;
	pJoints = new MD5Joint[iNumJoints];
	
	for(int i = 0; i < iNumJoints; i++) {
		NEXT("\"")

		pFD >> pJoints[i].iParent;
		if(pJoints[i].iParent < 0) {
			iNumBaseJoints++;
		}

		NEXT("(")
		pFD >> pJoints[i].vPosition.x >> pJoints[i].vPosition.y >> pJoints[i].vPosition.z;
		NEXT(")")
		NEXT("(")
		pFD >> pJoints[i].qOrientation.x >> pJoints[i].qOrientation.y >> pJoints[i].qOrientation.z;
		pJoints[i].qOrientation.compute_r();
	}

	pMeshes = new MD5Mesh[iNumMeshes];
	for(int i = 0; i < iNumMeshes; i++) {		
		NEXT("numverts")
		pFD >> pMeshes[i].iNumVerts;
		pFD >> buffer;
		
		pMeshes[i].pVerts = new MD5Vert[pMeshes[i].iNumVerts];
		for(int j = 0; j < pMeshes[i].iNumVerts; j++) {
			MD5Vert& vert = pMeshes[i].pVerts[j];
			NEXT("vert")
			NEXT("(")
			pFD >> vert.pTexCoords[0] >> vert.pTexCoords[1];
			NEXT(")")
			pFD >> vert.iWeightIndex >> vert.iNumWeights;
			
		}
		NEXT("numtris")
		pFD >> pMeshes[i].iNumTris;
		
		pMeshes[i].pTriangles = new MD5Triangle[pMeshes[i].iNumTris];
		int iC = 0;
		pFD >> buffer;
		for(int j = 0; j < pMeshes[i].iNumTris; j++) {
			MD5Triangle& tri = pMeshes[i].pTriangles[j];
			NEXT("tri")
			pFD >> buffer;
			pFD >> tri.pIndex[0] >> tri.pIndex[1] >> tri.pIndex[2];
		}
		
		NEXT("numweights")
		pFD >> pMeshes[i].iNumWeights;
		pFD >> buffer;
		pMeshes[i].pWeights = new MD5Weight[pMeshes[i].iNumWeights];
		for(int j = 0; j < pMeshes[i].iNumWeights; j++) {
			MD5Weight& w = pMeshes[i].pWeights[j];
			NEXT("weight")
			pFD >> buffer;
			pFD >> w.iBoneIndex >> w.fBias;
			NEXT("(")
			pFD >> w.vWeights.x >> w.vWeights.y >> w.vWeights.z;
		}
	}

	return true;
}

int main(int argc, char *argv[])
{
	char* usage = "usage: md5mesh2a2m <file.md5mesh> <file.a2m>";
	if(argc == 1) {
		cout << "no .md5mesh and .a2m file specified!" << endl << usage << endl;
		return 0;
	}
	if(argc == 2) {
		cout << "no .md5mesh or .a2m file specified!" << endl << usage << endl;
		return 0;
	}

	char* md5mesh_filename = argv[1];
	char* a2m_filename = argv[2];

	// load mesh file
	if(!LoadMesh(md5mesh_filename)) {
		cout << "error loading md5mesh file!" << endl;
		return 0;
	}

	// write data to a2m
	fstream afile;
	afile.open(a2m_filename, fstream::out | fstream::binary);

	// file type
	char* a2m_type = "A2EMODEL";
	afile.write(a2m_type, 8);
	// model type (0x00 = static model / 0x01 = animated/dynamic model)
	afile.put(0x01);

	// joint count
	put_int(&afile, iNumJoints);
	// base joint count
	put_int(&afile, iNumBaseJoints);

	// write joint data
	for(int i = 0; i < iNumJoints; i++) {
		// joint parent
		put_int(&afile, pJoints[i].iParent);
		// joint translation
		put_float(&afile, pJoints[i].vPosition.x);
		put_float(&afile, pJoints[i].vPosition.y);
		put_float(&afile, pJoints[i].vPosition.z);
		// joint orientation
		put_float(&afile, pJoints[i].qOrientation.r);
		put_float(&afile, pJoints[i].qOrientation.x);
		put_float(&afile, pJoints[i].qOrientation.y);
		put_float(&afile, pJoints[i].qOrientation.z);
	}

	// object/mesh count
	put_int(&afile, iNumMeshes);

	// write mesh data
	for(int i = 0; i < iNumMeshes; i++) {
		// vertex count
		put_int(&afile, pMeshes[i].iNumVerts);

		// vertex data
		for(int j = 0; j < pMeshes[i].iNumVerts; j++) {
			// uv coordinates
			put_float(&afile, pMeshes[i].pVerts[j].pTexCoords[0]);
			put_float(&afile, pMeshes[i].pVerts[j].pTexCoords[1]);
			// weight index
			put_int(&afile, pMeshes[i].pVerts[j].iWeightIndex);
			// weight count
			put_int(&afile, pMeshes[i].pVerts[j].iNumWeights);
		}

		// triangle count
		put_int(&afile, pMeshes[i].iNumTris);

		// triangle data
		for(int j = 0; j < pMeshes[i].iNumTris; j++) {
			// indices
			put_int(&afile, pMeshes[i].pTriangles[j].pIndex[0]);
			put_int(&afile, pMeshes[i].pTriangles[j].pIndex[1]);
			put_int(&afile, pMeshes[i].pTriangles[j].pIndex[2]);
		}

		// weight count
		put_int(&afile, pMeshes[i].iNumWeights);

		// weight data
		for(int j = 0; j < pMeshes[i].iNumWeights; j++) {
			// bone index
			put_int(&afile, pMeshes[i].pWeights[j].iBoneIndex);
			// weight strength
			put_float(&afile, pMeshes[i].pWeights[j].fBias);
			// weight itself
			put_float(&afile, pMeshes[i].pWeights[j].vWeights.x);
			put_float(&afile, pMeshes[i].pWeights[j].vWeights.y);
			put_float(&afile, pMeshes[i].pWeights[j].vWeights.z);
		}
	}

	// close (and save) file
	afile.close();

	cout << ".md5mesh successfully converted to .a2m!" << endl;

	return 0;
}
