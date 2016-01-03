// RenderObject.h: interface for the CRenderObject class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RENDEROBJECT_H__249A9671_2914_4AC3_9045_705B561BB138__INCLUDED_)
#define AFX_RENDEROBJECT_H__249A9671_2914_4AC3_9045_705B561BB138__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "GenericRenderObject.h"

#if !defined(AFX_BOUNDINGBOX_H__DB24F01C_80B9_11D3_97C1_FC2841000000__INCLUDED_)
	#include "Engine/BoundingBox.h"
#endif
RAPTOR_NAMESPACE

class CRenderObject : public CGenericRenderObject
{
protected:
	CRenderObject(rays_objectbase_t base);

public:
	virtual ~CRenderObject();

public:
	virtual float FastIntersect( CGenericRay &ray );
	virtual CColor::RGBA& GetLocalColor(const CGenericRay &ray);
	
protected:
	CGenericVector<float>	tmpVect;
	CColor::RGBA			tmpClr;

	CBoundingBox			*bBox;

	virtual CGenericVector<float>& GetLocalNormal( const CGenericVector<float> &hit );
};

#endif // !defined(AFX_RENDEROBJECT_H__249A9671_2914_4AC3_9045_705B561BB138__INCLUDED_)
