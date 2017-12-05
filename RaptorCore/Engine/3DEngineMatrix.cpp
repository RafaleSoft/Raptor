// 3DEngineMatrix.cpp: implementation of the C3DEngineMatrix class.
//
//////////////////////////////////////////////////////////////////////

#include "Subsys/CodeGeneration.h"

#if !defined(AFX_3DENGINEMATRIX_H__6CD1110E_1174_4f38_A452_30FB312022D0__INCLUDED_)
	#include "Engine/3DEngineMatrix.h"
#endif

RAPTOR_NAMESPACE

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
C3DEngineMatrix::C3DEngineMatrix(void)
{
}

C3DEngineMatrix::~C3DEngineMatrix(void)
{
}

C3DEngineMatrix::C3DEngineMatrix(const CGenericMatrix<float>& m)
:CGenericMatrix<float>(m)
{
}

C3DEngineMatrix::C3DEngineMatrix(const GL_MATRIX& M)
{
	CGenericMatrix<float>::operator =(M);
}

C3DEngineMatrix &C3DEngineMatrix::Inverse(void)
{
	float * const mm = matrix();

	//	transform translation => -Rt.T
	float tomm_3 = -(mm[0]*mm[3] + mm[4]*mm[7] + mm[8]*mm[11]);
	float tomm_7 = -(mm[1]*mm[3] + mm[5]*mm[7] + mm[9]*mm[11]);
	float tomm_11 = -(mm[2]*mm[3] + mm[6]*mm[7] + mm[10]*mm[11]);
	mm[3] = tomm_3;
	mm[7] = tomm_7;
	mm[11] = tomm_11;

	//	transpose rotation => inverse
	float tmp = mm[1];
	mm[1] = mm[4];
	mm[4] = tmp;

	tmp = mm[2];
	mm[2] = mm[8];
	mm[8] = tmp;

	tmp = mm[6];
	mm[6] = mm[9];
	mm[9] = tmp;

	// homogeneous coordinate
	mm[12] = mm[13] = mm[14] = 0.0f;
	mm[15] = 1.0f;

	return *this;
}

C3DEngineMatrix& C3DEngineMatrix::setInverseOf(const CGenericMatrix<float> &m)
{
	float * const mm = m.matrix();

	float * const tomm = matrix();

	//	transpose rotation
	tomm[1] = mm[4];
	tomm[4] = mm[1];

	tomm[2] = mm[8];
	tomm[8] = mm[2];

	tomm[6] = mm[9];
	tomm[9] = mm[6];

	tomm[0] = mm[0];
	tomm[5] = mm[5];
	tomm[10] = mm[10];

	//	transform translation
	tomm[3] = -(mm[0]*mm[3] + mm[4]*mm[7] + mm[8]*mm[11]);
	tomm[7] = -(mm[1]*mm[3] + mm[5]*mm[7] + mm[9]*mm[11]);
	tomm[11] = -(mm[2]*mm[3] + mm[6]*mm[7] + mm[10]*mm[11]);

	tomm[12] = tomm[13] = tomm[14] = 0.0f;
	tomm[15] = 1.0f;

	return *this;
}

C3DEngineMatrix &C3DEngineMatrix::Transpose(void)
{

	float * const mm = matrix();

	//	transpose rotation => inverse
	float tmp = mm[1];
	mm[1] = mm[4];
	mm[4] = tmp;

	tmp = mm[2];
	mm[2] = mm[8];
	mm[8] = tmp;

	tmp = mm[6];
	mm[6] = mm[9];
	mm[9] = tmp;

	tmp = mm[3];
	mm[3] = mm[12];
	mm[12] = tmp;

	tmp = mm[7];
	mm[7] = mm[13];
	mm[13] = tmp;

	tmp = mm[11];
	mm[11] = mm[14];
	mm[14] = tmp;

	return *this;
}
/*
C3DEngineMatrix &C3DEngineMatrix::InverseTranspose(const GL_MATRIX &m)
{
	float * const mm = (float*)&m;

	float * const tomm = C3DEngineAttributes::inverseKinematix.matrix();

	//	transpose rotation of transpose => Id
	tomm[1] = mm[1];
	tomm[4] = mm[4];

	tomm[2] = mm[2];
	tomm[8] = mm[8];

	tomm[6] = mm[6];
	tomm[9] = mm[9];

	tomm[0] = mm[0];
	tomm[5] = mm[5];
	tomm[10] = mm[10];

	tomm[3] = (mm[0]*mm[12] + mm[1]*mm[13] + mm[2]*mm[14]);
	tomm[7] = (mm[4]*mm[12] + mm[5]*mm[13] + mm[6]*mm[14]);
	tomm[11] = (mm[8]*mm[12] + mm[9]*mm[13] + mm[10]*mm[14]);

	tomm[12] = tomm[13] = tomm[14] = 0.0f;
	tomm[15] = 1.0f;

	return C3DEngineAttributes::inverseKinematix;
}
*/
