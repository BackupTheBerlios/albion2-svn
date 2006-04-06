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

#include "md5anim2a2a.h"

/*!
 * \mainpage
 *
 * \author flo
 * \author julien rebetez (.md5anim loading routine)
 *
 * \date May 2005
 *
 * Albion 2 Engine - MD5ANIM to A2A Converter
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

MD5Anim* LoadAnim(char* str) {
	MD5Anim* anim = new MD5Anim();
	
	ifstream pFD;
	string buffer;
	int version;
	
	pFD.open(str, ios::in);
	
	if(pFD.fail()) {
		cout << "error loading md5anim file: " << str << endl;
		return 0;
	}
	
	pFD >> buffer;
	pFD >> version;
	
	if(version != 10) {
		cout << "non supported MD5 version: " << version << endl;
		return 0;
	}
	
	NEXT("numFrames")
	pFD >> anim->iNumFrames;
	NEXT("numJoints")
	pFD >> anim->iNumJoints;
	NEXT("frameRate")
	pFD >> anim->iFrameRate;
	NEXT("numAnimatedComponents")
	pFD >> anim->iNumAnimatedComponents;
	
	anim->pFrames = new float*[anim->iNumFrames];
	for(int i = 0; i < anim->iNumFrames; i++) {
		anim->pFrames[i] = new float[anim->iNumAnimatedComponents];
	}
		
	anim->pJointInfos = new MD5Anim::MD5JointInfos[anim->iNumJoints];
	
	anim->pBaseFrame = new float*[anim->iNumJoints];
	
	NEXT("hierarchy")
	for(int i = 0; i < anim->iNumJoints; i++){
		NEXT("\"")
		buffer = "";
		pFD >> anim->pJointInfos[i].iParent;
		pFD >> anim->pJointInfos[i].iFlags;
		pFD >> anim->pJointInfos[i].iStartIndex;
	}
	
	NEXT("baseframe")
	for(int i = 0; i < anim->iNumJoints; i++) {
		anim->pBaseFrame[i] = new float[6];
		NEXT("(")
		pFD >> anim->pBaseFrame[i][0];
		pFD >> anim->pBaseFrame[i][1];
		pFD >> anim->pBaseFrame[i][2];
		NEXT(")")
		NEXT("(")
		pFD >> anim->pBaseFrame[i][3];
		pFD >> anim->pBaseFrame[i][4];
		pFD >> anim->pBaseFrame[i][5];
		NEXT(")")
	}

	for(int i = 0; i < anim->iNumFrames; i++) {
		NEXT("frame")
		NEXT("{")
		for(int j = 0; j < anim->iNumAnimatedComponents; j++) {
			pFD >> anim->pFrames[i][j];
		}
		NEXT("}")
	}
	return anim;
}

int main(int argc, char *argv[])
{
	char* usage = "usage: md5anim2a2a <file.md5anim> <file.a2a>";
	if(argc == 1) {
		cout << "no .md5anim and .a2a file specified!" << endl << usage << endl;
		return 0;
	}
	if(argc == 2) {
		cout << "no .md5anim or .a2a file specified!" << endl << usage << endl;
		return 0;
	}

	char* md5anim_filename = argv[1];
	char* a2a_filename = argv[2];

	// load anim file
	MD5Anim* md5anim = LoadAnim(md5anim_filename);
	if(md5anim == NULL) {
		cout << "error loading md5anim file!" << endl;
		return 0;
	}

	// write data to a2a
	fstream afile;
	afile.open(a2a_filename, fstream::out | fstream::binary);

	// file type
	char* a2m_type = "A2EANIM";
	afile.write(a2m_type, 7);

	// joint count
	put_int(&afile, md5anim->iNumJoints);

	// write joint data
	for(int i = 0; i < md5anim->iNumJoints; i++) {
		// joint parent
		put_int(&afile, md5anim->pJointInfos[i].iParent);
		// joint flags
		put_int(&afile, md5anim->pJointInfos[i].iFlags);
		// joint start index
		put_int(&afile, md5anim->pJointInfos[i].iStartIndex);
		// joint base translation
		put_float(&afile, md5anim->pBaseFrame[i][0]);
		put_float(&afile, md5anim->pBaseFrame[i][1]);
		put_float(&afile, md5anim->pBaseFrame[i][2]);
		// joint base orientation
		quaternion base_orientation;
		base_orientation.x = md5anim->pBaseFrame[i][3];
		base_orientation.y = md5anim->pBaseFrame[i][4];
		base_orientation.z = md5anim->pBaseFrame[i][5];
		base_orientation.compute_r();
		put_float(&afile, base_orientation.r);
		put_float(&afile, base_orientation.x);
		put_float(&afile, base_orientation.y);
		put_float(&afile, base_orientation.z);
	}

	// frame count
	put_int(&afile, md5anim->iNumFrames);
	// frame time
	put_float(&afile, ((float)md5anim->iNumFrames / (float)md5anim->iFrameRate) * 2.0f);
	// animated components count
	put_int(&afile, md5anim->iNumAnimatedComponents);

	// write frame data
	for(int i = 0; i < md5anim->iNumFrames; i++) {
		// this code was first used for writing the whole translation and orientation data
		// of every joint in every frame (even if it isn't animated), but it consumes too
		// much memory, so i decided to store the translation and orientation data .md5anim
		// like - just write changed/animated data.
		/*int n = 0;
		for(int j = 0; j < md5anim->iNumJoints; j++) {
			// start index
			int s = md5anim->pJointInfos[j].iStartIndex;

			// frame joint translation
			if(md5anim->pJointInfos[j].iFlags & 1) { put_float(&afile, md5anim->pFrames[j][s+n]); n++; }
			else { put_float(&afile, md5anim->pBaseFrame[j][0]); }
			if(md5anim->pJointInfos[j].iFlags & 2) { put_float(&afile, md5anim->pFrames[j][s+n]); n++; }
			else { put_float(&afile, md5anim->pBaseFrame[j][1]); }
			if(md5anim->pJointInfos[j].iFlags & 4) { put_float(&afile, md5anim->pFrames[j][s+n]); n++; }
			else { put_float(&afile, md5anim->pBaseFrame[j][2]); }
			// frame joint orientation
			quaternion orientation;
			if(md5anim->pJointInfos[j].iFlags & 8) { orientation.x = md5anim->pFrames[j][s+n]; n++; }
			else { orientation.x = md5anim->pBaseFrame[j][3]; }
			if(md5anim->pJointInfos[j].iFlags & 16) { orientation.y = md5anim->pFrames[j][s+n]; n++; }
			else { orientation.y = md5anim->pBaseFrame[j][4]; }
			if(md5anim->pJointInfos[j].iFlags & 32) { orientation.z = md5anim->pFrames[j][s+n]; n++; }
			else { orientation.z = md5anim->pBaseFrame[j][5]; }
			orientation.ComputeR();
			put_float(&afile, orientation.r);
			put_float(&afile, orientation.x);
			put_float(&afile, orientation.y);
			put_float(&afile, orientation.z);
		}*/

		// animated components data
		for(int j = 0; j < md5anim->iNumAnimatedComponents; j++) {
			put_float(&afile, md5anim->pFrames[i][j]);
		}
	}

	// close (and save) file
	afile.close();

	cout << ".md5anim successfully converted to .a2a!" << endl;

	return 0;
}
