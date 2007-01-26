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
 
#ifndef __MD5ANIM2A2M_H__
#define __MD5ANIM2A2M_H__

#ifdef WIN32
#include <windows.h>
#endif

#include <cstdlib>
#include <iostream>
#include <cmath>
#include <string>
#include <fstream>
#include <quaternion.h>
#include <vertex3.h>
#include <omp.h>
using namespace std;

#define NEXT(str) while(buffer.find(str) == string::npos) pFD >> buffer;

class Plane {
public:
	Plane () : vNormal(0,0,0), fD(0.0f) {}
	Plane (vertex3 v, float f) : vNormal(v), fD(f) {}
	
	void SetNormal (vertex3 v) { vNormal = v; }
	void SetD (float f) { fD = f; }
	vertex3 GetNormal () { return vNormal; }
	float GetD () { return fD; }
	
	void SetFromPoints (const vertex3& v1, const vertex3& v2, const vertex3& v3);
	
	float GetDistance (const vertex3& v);
	int ClassifyPoint (const vertex3& v);
	
	bool IntersectRay (const vertex3& R0, const vertex3& end, vertex3& cP);
	
	void Normalize ();

	vertex3 vNormal;
	float fD;
};

struct Face {
	unsigned int pIndex[3];
	Plane plane;
	bool visible;
};

struct Vertice {
	vertex3 vPosition;
	float s, t;
	vertex3 sTangent, tTangent;
	vertex3 vNormal;
	vertex3 vTangentSpaceLight;
};

struct MD5Vert {
	int iWeightIndex;
	int iNumWeights;
	float pTexCoords[2];
};

struct MD5Weight {
	int iBoneIndex;
	float fBias;
	vertex3 vWeights;
};

struct MD5Triangle : public Face {
};

struct MD5Mesh {
	int iNumVerts;
	MD5Vert* pVerts;
	
	int iNumWeights;
	MD5Weight* pWeights;
	
	int iNumTris;
	MD5Triangle* pTriangles;
	
	unsigned int* pIndexes;
	
	std::string sColor;
	std::string sNormal;
};

struct MD5Anim {
	char* sName;
	int iNumFrames;
	int iNumJoints;
	int iFrameRate;
	int iNumAnimatedComponents;
	float** pFrames;
	float** pBaseFrame;
	struct MD5JointInfos {
		int iParent;
		int iFlags;
		int iStartIndex;
	};
	MD5JointInfos* pJointInfos;
};

struct MD5Joint
{
	string sName;
	int iNumber;
	int iParent;
	vertex3 vPosition;
	quaternion qOrientation;
	
	MD5Joint* pParent;
	MD5Joint** pChildrens;
	int iNumChildrens;
};

MD5Joint* pJoints;
MD5Mesh* pMeshes;

int iNumJoints;
int iNumBaseJoints;
int iNumMeshes;

void put_int(fstream* file, int integer);
void put_float(fstream* file, float flt);
MD5Anim* LoadAnim(char* str);

#endif
