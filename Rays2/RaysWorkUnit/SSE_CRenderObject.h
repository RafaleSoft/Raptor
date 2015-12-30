// SSE_CRenderObject.h: interface for the SSE_CRenderObject class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SSE_CRENDEROBJECT_H__FE263A33_3DE3_4672_AAFE_8DBB960E55C2__INCLUDED_)
#define AFX_SSE_CRENDEROBJECT_H__FE263A33_3DE3_4672_AAFE_8DBB960E55C2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GenericRenderObject.h"

#if !defined(AFX_SSE_BOUNDINGBOX_H__DD1B683A_C345_4399_B1CF_4059B5C3A744__INCLUDED_)
	#include "SSE_Engine/SSE_BoundingBox.h"
#endif

RAPTOR_NAMESPACE



class SSE_CRenderObject : public CGenericRenderObject  
{
protected:
	SSE_CRenderObject(rays_objectbase_t base);

public:
	virtual ~SSE_CRenderObject();

public:
	virtual float FastIntersect( CGenericRay &ray );
	virtual CWVector& GetLocalColor(const CGenericRay &ray);

protected:
	CGenericVector<float>	tmpVect;
	CWVector				tmpClr;

	CSSE_BoundingBox		*bBox;

	virtual CGenericVector<float>& GetLocalNormal( const CGenericVector<float> &hit );
};

#endif // !defined(AFX_SSE_CRENDEROBJECT_H__FE263A33_3DE3_4672_AAFE_8DBB960E55C2__INCLUDED_)
