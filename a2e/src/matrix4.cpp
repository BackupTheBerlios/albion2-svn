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

#include "matrix4.h"

/*! creates a matrix4 object
 */
matrix4::matrix4() {
	identity();
}

matrix4::matrix4(float m0, float m1, float m2, float m3,
			float m4, float m5, float m6, float m7,
			float m8, float m9, float m10, float m11,
			float m12, float m13, float m14, float m15) {
	matrix4::m[0][0] = m0;
	matrix4::m[0][1] = m1;
	matrix4::m[0][2] = m2;
	matrix4::m[0][3] = m3;
	matrix4::m[1][0] = m4;
	matrix4::m[1][1] = m5;
	matrix4::m[1][2] = m6;
	matrix4::m[1][3] = m7;
	matrix4::m[2][0] = m8;
	matrix4::m[2][1] = m9;
	matrix4::m[2][2] = m10;
	matrix4::m[2][3] = m11;
	matrix4::m[3][0] = m12;
	matrix4::m[3][1] = m13;
	matrix4::m[3][2] = m14;
	matrix4::m[3][3] = m15;
}

matrix4::matrix4(matrix4* mat) {
	memcpy(matrix4::m, mat, sizeof(matrix4));
}

/*! matrix4 destructor
 */
matrix4::~matrix4() {
}

matrix4 matrix4::operator*(matrix4& mat) {
	matrix4 new_mat;
	for(unsigned int i = 0; i < 4; i++) {
		for(unsigned int j = 0; j < 4; j++) {
			new_mat.m[i][j] = 0.0f;
			for(unsigned int k = 0; k < 4; k++){
				new_mat.m[i][j] += matrix4::m[i][k] * mat.m[k][j];
			}
		}
	}
	return new_mat;
}

matrix4& matrix4::operator*=(const matrix4& mat) {
	matrix4 new_mat;
	for(unsigned int i = 0; i < 4; i++) {
		for(unsigned int j = 0; j < 4; j++) {
			new_mat.m[i][j] = 0.0f;
			for(unsigned int k = 0; k < 4; k++){
				new_mat.m[i][j] += mat.m[i][k] * matrix4::m[k][j];
			}
		}
	}
	memcpy(matrix4::m, &new_mat, sizeof(matrix4));
	return *this;
}

/*! << operator overload
 */
A2E_API ostream& operator<<(ostream& o, matrix4& mat) {
	o.precision(2);
	for(unsigned int i = 0; i < 4; i++) {
		if(i == 0) o << "/ ";
		else if(i == 1 || i == 2) o << "| ";
		else if(i == 3) o << "\\ ";
		for(unsigned int j = 0; j < 4; j++) {
			o.width(9);
			o.setf(ios_base::right, ios_base::adjustfield);
			o.setf(ios_base::fixed, ios_base::floatfield);
			o << mat.m[i][j] << " ";
		}
		if(i == 0) o << "\\";
		else if(i == 1 || i == 2) o << "|";
		else if(i == 3) o << "/";
		//if(i != 3) o << endl;
		o << endl;
	}
	return o;
}

void matrix4::rotate_x(float x) {
	float sinx, cosx;
	sinx = sinf(x);
	cosx = cosf(x);

	matrix4::m[0][0] = 1.0f;
	matrix4::m[0][1] = 0.0f;
	matrix4::m[0][2] = 0.0f;
	matrix4::m[0][3] = 0.0f;

	matrix4::m[1][0] = 0.0f;
	matrix4::m[1][1] = cosx;
	matrix4::m[1][2] = -sinx;
	matrix4::m[1][3] = 0.0f;

	matrix4::m[2][0] = 0.0f;
	matrix4::m[2][1] = sinx;
	matrix4::m[2][2] = cosx;
	matrix4::m[2][3] = 0.0f;

	matrix4::m[3][0] = 0.0f;
	matrix4::m[3][1] = 0.0f;
	matrix4::m[3][2] = 0.0f;
	matrix4::m[3][3] = 1.0f;
}

void matrix4::rotate_y(float y) {
	float siny, cosy;
	siny = sinf(y);
	cosy = cosf(y);

	matrix4::m[0][0] = cosy;
	matrix4::m[0][1] = 0.0f;
	matrix4::m[0][2] = siny;
	matrix4::m[0][3] = 0.0f;

	matrix4::m[1][0] = 0.0f;
	matrix4::m[1][1] = 1.0f;
	matrix4::m[1][2] = 0.0f;
	matrix4::m[1][3] = 0.0f;

	matrix4::m[2][0] = -siny;
	matrix4::m[2][1] = 0.0f;
	matrix4::m[2][2] = cosy;
	matrix4::m[2][3] = 0.0f;

	matrix4::m[3][0] = 0.0f;
	matrix4::m[3][1] = 0.0f;
	matrix4::m[3][2] = 0.0f;
	matrix4::m[3][3] = 1.0f;
}

void matrix4::rotate_z(float z) {
	float sinz, cosz;
	sinz = sinf(z);
	cosz = cosf(z);

	matrix4::m[0][0] = cosz;
	matrix4::m[0][1] = -sinz;
	matrix4::m[0][2] = 0.0f;
	matrix4::m[0][3] = 0.0f;

	matrix4::m[1][0] = sinz;
	matrix4::m[1][1] = cosz;
	matrix4::m[1][2] = 0.0f;
	matrix4::m[1][3] = 0.0f;

	matrix4::m[2][0] = 0.0f;
	matrix4::m[2][1] = 0.0f;
	matrix4::m[2][2] = 1.0f;
	matrix4::m[2][3] = 0.0f;

	matrix4::m[3][0] = 0.0f;
	matrix4::m[3][1] = 0.0f;
	matrix4::m[3][2] = 0.0f;
	matrix4::m[3][3] = 1.0f;
}

void matrix4::rotate(float x, float y) {
	float sinx, siny, cosx, cosy;
	sinx = sinf(x);
	siny = sinf(y);
	cosx = cosf(x);
	cosy = cosf(y);

	matrix4::m[0][0] = cosy;
	matrix4::m[0][1] = 0.0f;
	matrix4::m[0][2] = -siny;
	matrix4::m[0][3] = 0.0f;

	matrix4::m[1][0] = -sinx * siny;
	matrix4::m[1][1] = cosx;
	matrix4::m[1][2] = -sinx * cosy;
	matrix4::m[1][3] = 0.0f;

	matrix4::m[2][0] = cosx * siny;
	matrix4::m[2][1] = sinx;
	matrix4::m[2][2] = cosx * cosy;
	matrix4::m[2][3] = 0.0f;

	matrix4::m[3][0] = 0.0f;
	matrix4::m[3][1] = 0.0f;
	matrix4::m[3][2] = 0.0f;
	matrix4::m[3][3] = 1.0f;
}

void matrix4::rotate(float x, float y, float z) {
	float sinx, siny, sinz, cosx, cosy, cosz;
	sinx = sinf(x);
	siny = sinf(y);
	sinz = sinf(z);
	cosx = cosf(x);
	cosy = cosf(y);
	cosz = cosf(z);

	matrix4::m[0][0] = cosy * cosz + sinx * siny * sinz;
	matrix4::m[0][1] = -cosx * sinz;
	matrix4::m[0][2] = sinx * cosy * sinz - siny * cosz;
	matrix4::m[0][3] = 0.0f;

	matrix4::m[1][0] = cosy * sinz - sinx * siny * cosz;
	matrix4::m[1][1] = cosx * cosz;
	matrix4::m[1][2] = -siny * sinz - sinx * cosy * cosz;
	matrix4::m[1][3] = 0.0f;

	matrix4::m[2][0] = cosx * siny;
	matrix4::m[2][1] = sinx;
	matrix4::m[2][2] = cosx * cosy;
	matrix4::m[2][3] = 0.0f;

	matrix4::m[3][0] = 0.0f;
	matrix4::m[3][1] = 0.0f;
	matrix4::m[3][2] = 0.0f;
	matrix4::m[3][3] = 1.0f;
}

void matrix4::invert() {
	matrix4 mat;

	float p00 = matrix4::m[2][2] * matrix4::m[3][3];
	float p01 = matrix4::m[3][2] * matrix4::m[2][3];
	float p02 = matrix4::m[1][2] * matrix4::m[3][3];
	float p03 = matrix4::m[3][2] * matrix4::m[1][3];
	float p04 = matrix4::m[1][2] * matrix4::m[2][3];
	float p05 = matrix4::m[2][2] * matrix4::m[1][3];
	float p06 = matrix4::m[0][2] * matrix4::m[3][3];
	float p07 = matrix4::m[3][2] * matrix4::m[0][3];
	float p08 = matrix4::m[0][2] * matrix4::m[2][3];
	float p09 = matrix4::m[2][2] * matrix4::m[0][3];
	float p10 = matrix4::m[0][2] * matrix4::m[1][3];
	float p11 = matrix4::m[1][2] * matrix4::m[0][3];

	mat.m[0][0] = (p00 * matrix4::m[1][1] + p03 * matrix4::m[2][1] + p04 * matrix4::m[3][1]) - (p01 * matrix4::m[1][1] + p02 * matrix4::m[2][1] + p05 * matrix4::m[3][1]);
	mat.m[0][1] = (p01 * matrix4::m[0][1] + p06 * matrix4::m[2][1] + p09 * matrix4::m[3][1]) - (p00 * matrix4::m[0][1] + p07 * matrix4::m[2][1] + p08 * matrix4::m[3][1]);
	mat.m[0][2] = (p02 * matrix4::m[0][1] + p07 * matrix4::m[1][1] + p10 * matrix4::m[3][1]) - (p03 * matrix4::m[0][1] + p06 * matrix4::m[1][1] + p11 * matrix4::m[3][1]);
	mat.m[0][3] = (p05 * matrix4::m[0][1] + p08 * matrix4::m[1][1] + p11 * matrix4::m[2][1]) - (p04 * matrix4::m[0][1] + p09 * matrix4::m[1][1] + p10 * matrix4::m[2][1]);
	mat.m[1][0] = (p01 * matrix4::m[1][0] + p02 * matrix4::m[2][0] + p05 * matrix4::m[3][0]) - (p00 * matrix4::m[1][0] + p03 * matrix4::m[2][0] + p04 * matrix4::m[3][0]);
	mat.m[1][1] = (p00 * matrix4::m[0][0] + p07 * matrix4::m[2][0] + p08 * matrix4::m[3][0]) - (p01 * matrix4::m[0][0] + p06 * matrix4::m[2][0] + p09 * matrix4::m[3][0]);
	mat.m[1][2] = (p03 * matrix4::m[0][0] + p06 * matrix4::m[1][0] + p11 * matrix4::m[3][0]) - (p02 * matrix4::m[0][0] + p07 * matrix4::m[1][0] + p10 * matrix4::m[3][0]);
	mat.m[1][3] = (p04 * matrix4::m[0][0] + p09 * matrix4::m[1][0] + p10 * matrix4::m[2][0]) - (p05 * matrix4::m[0][0] + p08 * matrix4::m[1][0] + p11 * matrix4::m[2][0]);

	float q00 = matrix4::m[2][0] * matrix4::m[3][1];
	float q01 = matrix4::m[3][0] * matrix4::m[2][1];
	float q02 = matrix4::m[1][0] * matrix4::m[3][1];
	float q03 = matrix4::m[3][0] * matrix4::m[1][1];
	float q04 = matrix4::m[1][0] * matrix4::m[2][1];
	float q05 = matrix4::m[2][0] * matrix4::m[1][1];
	float q06 = matrix4::m[0][0] * matrix4::m[3][1];
	float q07 = matrix4::m[3][0] * matrix4::m[0][1];
	float q08 = matrix4::m[0][0] * matrix4::m[2][1];
	float q09 = matrix4::m[2][0] * matrix4::m[0][1];
	float q10 = matrix4::m[0][0] * matrix4::m[1][1];
	float q11 = matrix4::m[1][0] * matrix4::m[0][1];

	mat.m[2][0] = (q00 * matrix4::m[1][3] + q03 * matrix4::m[2][3] + q04 * matrix4::m[3][3]) - (q01 * matrix4::m[1][3] + q02 * matrix4::m[2][3] + q05 * matrix4::m[3][3]);
	mat.m[2][1] = (q01 * matrix4::m[0][3] + q06 * matrix4::m[2][3] + q09 * matrix4::m[3][3]) - (q00 * matrix4::m[0][3] + q07 * matrix4::m[2][3] + q08 * matrix4::m[3][3]);
	mat.m[2][2] = (q02 * matrix4::m[0][3] + q07 * matrix4::m[1][3] + q10 * matrix4::m[3][3]) - (q03 * matrix4::m[0][3] + q06 * matrix4::m[1][3] + q11 * matrix4::m[3][3]);
	mat.m[2][3] = (q05 * matrix4::m[0][3] + q08 * matrix4::m[1][3] + q11 * matrix4::m[2][3]) - (q04 * matrix4::m[0][3] + q09 * matrix4::m[1][3] + q10 * matrix4::m[2][3]);
	mat.m[3][0] = (q02 * matrix4::m[2][2] + q05 * matrix4::m[3][2] + q01 * matrix4::m[1][2]) - (q04 * matrix4::m[3][2] + q00 * matrix4::m[1][2] + q03 * matrix4::m[2][2]);
	mat.m[3][1] = (q08 * matrix4::m[3][2] + q00 * matrix4::m[0][2] + q07 * matrix4::m[2][2]) - (q06 * matrix4::m[2][2] + q09 * matrix4::m[3][2] + q01 * matrix4::m[0][2]);
	mat.m[3][2] = (q06 * matrix4::m[1][2] + q11 * matrix4::m[3][2] + q03 * matrix4::m[0][2]) - (q10 * matrix4::m[3][2] + q02 * matrix4::m[0][2] + q07 * matrix4::m[1][2]);
	mat.m[3][3] = (q10 * matrix4::m[2][2] + q04 * matrix4::m[0][2] + q09 * matrix4::m[1][2]) - (q08 * matrix4::m[1][2] + q11 * matrix4::m[2][2] + q05 * matrix4::m[0][2]);

	float x = 1.0f / (matrix4::m[0][0] * mat.m[0][0] + matrix4::m[1][0] * mat.m[0][1] + matrix4::m[2][0] * mat.m[0][2] + matrix4::m[3][0] * mat.m[0][3]);

	for(unsigned int i = 0; i < 4; i++) {
		for(unsigned int j = 0; j < 4; j++) {
			mat.m[i][j] *= x;
		}
	}

	memcpy(matrix4::m, &mat, sizeof(matrix4));
}

void matrix4::translate(float x, float y, float z) {
	matrix4 tmat(1.0f, 0.0f, 0.0f, x,
				 0.0f, 1.0f, 0.0f, y,
				 0.0f, 0.0f, 1.0f, z,
				 0.0f, 0.0f, 0.0f, 1.0f);

	*this *= tmat;
}

void matrix4::scale(float x, float y, float z) {
	matrix4 tmat(x, 0.0f, 0.0f, 0.0f,
				 0.0f, y, 0.0f, 0.0f,
				 0.0f, 0.0f, z, 0.0f,
				 0.0f, 0.0f, 0.0f, 1.0f);

	*this *= tmat;
}

void matrix4::identity() {
	matrix4::m[0][0] = 1.0f;
	matrix4::m[0][1] = 0.0f;
	matrix4::m[0][2] = 0.0f;
	matrix4::m[0][3] = 0.0f;

	matrix4::m[1][0] = 0.0f;
	matrix4::m[1][1] = 1.0f;
	matrix4::m[1][2] = 0.0f;
	matrix4::m[1][3] = 0.0f;

	matrix4::m[2][0] = 0.0f;
	matrix4::m[2][1] = 0.0f;
	matrix4::m[2][2] = 1.0f;
	matrix4::m[2][3] = 0.0f;

	matrix4::m[3][0] = 0.0f;
	matrix4::m[3][1] = 0.0f;
	matrix4::m[3][2] = 0.0f;
	matrix4::m[3][3] = 1.0f;
}
