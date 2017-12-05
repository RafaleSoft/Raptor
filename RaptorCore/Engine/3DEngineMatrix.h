// 3DEngineMatrix.h: interface for the C3DEngineMatrix class.
//
//////////////////////////////////////////////////////////////////////
#if !defined(AFX_3DENGINEMATRIX_H__6CD1110E_1174_4f38_A452_30FB312022D0__INCLUDED_)
#define AFX_3DENGINEMATRIX_H__6CD1110E_1174_4f38_A452_30FB312022D0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Subsys/CodeGeneration.h"

#ifndef __CGLTYPES_HPP__
	#include "System/CGLTypes.h"
#endif

RAPTOR_NAMESPACE_BEGIN

//!
//!	This class implements a matrix that is built with a rotation
//!	and a translation and nomogeneous coordinates: inversible (Determinant > 0)
//!
class C3DEngineMatrix :	public CGenericMatrix<float>
{
public:
	C3DEngineMatrix(void);
	C3DEngineMatrix(const CGenericMatrix<float>& m);
	C3DEngineMatrix(const GL_MATRIX& M);
	virtual ~C3DEngineMatrix(void);

    //! This method computes the inverse of a matrix that is built in the same way 
    //! as OpenGL transforms : a 4x4 matrix holding a 3x3 rotation, a translation vector
    //! and an homogenous vector. This method cannot be used to inverse any kind of matrix !
	C3DEngineMatrix & RAPTOR_FASTCALL Inverse(void);

	//! This method computes the inverse of the matrix parameter m and assigns
	//! the inverse result to this.
	//! As above, this method cannot be used to inverse any kind of matrix !
	C3DEngineMatrix& setInverseOf(const CGenericMatrix<float> &m);

    //! This method returns the transpose of an OpenGL matrix.
	C3DEngineMatrix & RAPTOR_FASTCALL Transpose(void);
};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_3DENGINEMATRIX_H__6CD1110E_1174_4f38_A452_30FB312022D0__INCLUDED_)