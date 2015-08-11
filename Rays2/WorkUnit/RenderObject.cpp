// RenderObject.cpp: implementation of the CRenderObject class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RenderObject.h"
#include "GenericRay.h"
#include "BoundingBox.h"
#include "Texture.h"
#include "..\messages.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRenderObject::CRenderObject(rays_objectbase_t base):
	CGenericRenderObject(CString(base.name))
{
	surface = FLAT;
	bBox = new CBoundingBox();

	shading = base.shading;

	//	color should be specified with coordinates clamped to [0..1]
	color.Set(	(short)(0xFFFF * base.color.x),
				(short)(0xFFFF * base.color.y),
				(short)(0xFFFF * base.color.z),
				(short)(0xFFFF * base.color.h));

	m_pTexture = NULL;
}

CRenderObject::~CRenderObject()
{
	delete bBox;
}


CWVector& CRenderObject::GetLocalColor(const CGenericRay &ray)
{
	if (m_pTexture != NULL)
	{
		tmpVect = GetTexel(ray.hit);
		tmpClr = m_pTexture->GetMapColor(tmpVect);
	}
	else
	{
		tmpClr = color;
	}

	return tmpClr;
}

float CRenderObject::FastIntersect( CGenericRay &ray )
{ 
	return bBox->Intersect(ray);
}

CGenericVector<float>& CRenderObject::GetLocalNormal( const CGenericVector<float> &hit ) 
{
	if (m_pNormalMap != NULL)
	{
		tmpVect = GetTexel(hit);
		tmpClr = m_pNormalMap->GetMapColor(tmpVect);

		tmpVect = Normal(hit);

		//
		//	Compute the binormals
		//
		float normal[4];
		float binormal[4];
		float sqr;

		normal[1] = 0; //normal.y = 0;

		if ((tmpVect.X() == 0) && (tmpVect.Z() == 0))
		{
			normal[0] = 0.0f;//normal.x = 0.0f;
			normal[2] = 1.0f;//normal.z = 1.0f;
		}
		else
		{
			sqr = 1.0f / ( tmpVect.Z() * tmpVect.Z() + tmpVect.X() * tmpVect.X() ) ;
			normal[0] = sqrt( (tmpVect.Z() * tmpVect.Z()) * sqr );
			normal[2] = sqrt( (tmpVect.X() * tmpVect.X()) * sqr );
		}

		if (
				((tmpVect.X() > 0) && (tmpVect.Z() > 0)) || 
				((tmpVect.X() < 0) && (tmpVect.Z() < 0))
			)
			//	normal.z = -normal.z;
			normal[2] = -normal[2];

		binormal[0] = (tmpVect.Y() * normal[2] - tmpVect.Z() * normal[1]);
		binormal[1] = (tmpVect.Z() * normal[0] - tmpVect.X() * normal[2]);
		binormal[2] = (tmpVect.X() * normal[1] - tmpVect.Y() * normal[0]);

		//normal.h = 1.0f;
		//binormal.h = 1.0f;

		//
		//	Apply bump map perturbation
		//
		float bumpCoefs[4];
		bumpCoefs[0] = DEUX_SUR_65535 * tmpClr.X() - 1.0f;
		bumpCoefs[1] = DEUX_SUR_65535 * tmpClr.Y() - 1.0f;
		bumpCoefs[2] = DEUX_SUR_65535 * tmpClr.Z() - 1.0f;
		
		sqr = 1.0f / sqrt(bumpCoefs[0]*bumpCoefs[0] + bumpCoefs[1]*bumpCoefs[1] + bumpCoefs[2]*bumpCoefs[2]);
		bumpCoefs[0] = bumpCoefs[0]*sqr;
		bumpCoefs[1] = bumpCoefs[1]*sqr;
		bumpCoefs[2] = bumpCoefs[2]*sqr;

		//
		//	Texture is upside down, so reverse a coordinate
		//
		tmpVect.X() = -bumpCoefs[0] * normal[0] + bumpCoefs[1] * binormal[0] + bumpCoefs[2] * tmpVect.X();
		tmpVect.Y() = -bumpCoefs[0] * normal[1] + bumpCoefs[1] * binormal[1] + bumpCoefs[2] * tmpVect.Y();
		tmpVect.Z() = -bumpCoefs[0] * normal[2] + bumpCoefs[1] * binormal[2] + bumpCoefs[2] * tmpVect.Z();
		tmpVect.H(1.0f);

		return tmpVect;
	}
	else
		return Normal(hit);
}
