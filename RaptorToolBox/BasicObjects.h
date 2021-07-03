/***************************************************************************/
/*                                                                         */
/*  BasicObjects.h                                                         */
/*                                                                         */
/*    Raptor OpenGL & Vulkan realtime 3D Engine SDK.                       */
/*                                                                         */
/*  Copyright 1998-2021 by                                                 */
/*  Fabrice FERRAND.                                                       */
/*                                                                         */
/*  This file is part of the Raptor project, and may only be used,         */
/*  modified, and distributed under the terms of the Raptor project        */
/*  license, LICENSE.  By continuing to use, modify, or distribute         */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/


#if !defined(AFX_BASICOBJECTS_H__2E0B717D_862A_4096_9D99_8EE0654F4678__INCLUDED_)
#define AFX_BASICOBJECTS_H__2E0B717D_862A_4096_9D99_8EE0654F4678__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if !defined(AFX_SHADEDGEOMETRY_H__E56C66F7_2DF6_497B_AA0F_19DDC11390F9__INCLUDED_)
    #include "GLHierarchy/ShadedGeometry.h"
#endif



RAPTOR_NAMESPACE


class CBasicObjects  
{
public:
	class RAPTOR_API CCube : public CShadedGeometry
	{
	public:
		//! Constructor with default size of 1.0. 
		CCube();
		virtual ~CCube();

		//! Define cube's (parallelepiped ) dimensions. Previous values are lost.
		//! If one size is wrong ( <= 0 ), it is defaulted to 1.0
		void setDimensions(float width,float height,float depth);
	};

	class RAPTOR_API CRectangle : public CShadedGeometry
	{
	public:
		//!	Constructor with default size of 1.0f centered at (0,0)
		CRectangle();
		virtual ~CRectangle();

		//! Define quad's dimensions.
		//! @param width : the width of the simple quad.
		//! @param height : the height of the simple quad.
		void setDimensions(float width,float height);
	};

	class RAPTOR_API CIsocahedron : public CShadedGeometry
	{
	public:
		//!	Constructor with default size of 1.0f / 20
		CIsocahedron();
		virtual ~CIsocahedron();

		//! Define radius and tesselation. Previous values are lost
		//! If one size is wrong, it is defaulted to 1.0 / 20.
		//! @param radius : the radius of the minimal surrounding sphere on which the icosahedron is built
		//! @param nbLevel : the number of subdivisions of the initial icosahedron ( 20 faces for level 0 )
		void setDimensions(float radius,unsigned int nblevel);
	};

	class RAPTOR_API CGeoSphere : public CShadedGeometry
	{
	public:
		//!	Constructor with default size of 1.0f / 20
		CGeoSphere();
		virtual ~CGeoSphere();

		//! Define radius and tesselation. Previous values are lost
		//! If one size is wrong, it is defaulted to 1.0 / 20.
		//! @param radius : the radius of the minimal surrounding sphere on which the icosahedron is built
		//! @param nbLevel : the number of subdivisions of the initial icosahedron ( 20 faces for level 0 )
		void setDimensions(float radius,unsigned int nbMeridiens,unsigned int nbParallels);
	};

	class RAPTOR_API CRectMesh : public CShadedGeometry
	{
	public:
		//!	Constructor with default size 1, 1 and 50 x 50 cells
		CRectMesh();
		virtual ~CRectMesh();

		//! Define level of tesselation. Previous values are lost
		//! @param width : the width geometry dimension of the rect
		//! @param height : the height geometry dimension of the rect
		//! @param hcels : the number of horizontal knots in the mesh
		//! @param vcels : the number of vertical knots in the mesh
		void setDimensions(	float width,float height,
							unsigned int hcels,
							unsigned int vcels);
	};


private:
	CBasicObjects();
	virtual ~CBasicObjects() {};

};

#endif // !defined(AFX_BASICOBJECTS_H__2E0B717D_862A_4096_9D99_8EE0654F4678__INCLUDED_)
