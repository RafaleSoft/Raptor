// RenderObject.h: interface for the CRenderObject class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RENDEROBJECT_H__249A9671_2914_4AC3_9045_705B561BB138__INCLUDED_)
#define AFX_RENDEROBJECT_H__249A9671_2914_4AC3_9045_705B561BB138__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "GenericRenderObject.h"

class CBoundingBox;

class CRenderObject : public CGenericRenderObject
{
protected:
	CRenderObject(rays_objectbase_t base);

public:
	virtual ~CRenderObject();

public:
	virtual float FastIntersect( CGenericRay &ray );
	virtual CWVector& GetLocalColor(const CGenericRay &ray);
	
protected:
	CGenericVector<float>	tmpVect;
	CWVector				tmpClr;

	CBoundingBox			*bBox;

	virtual CGenericVector<float>& GetLocalNormal( const CGenericVector<float> &hit );
};

#endif // !defined(AFX_RENDEROBJECT_H__249A9671_2914_4AC3_9045_705B561BB138__INCLUDED_)
