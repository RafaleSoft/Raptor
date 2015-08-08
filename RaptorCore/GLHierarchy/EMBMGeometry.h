// EMBMGeometry.h: interface for the CEMBMGeometry class.
//
//////////////////////////////////////////////////////////////////////
#if !defined(AFX_EMBMGEOMETRY_H__0270535E_F6CF_4C2B_9F52_3BDB34627468__INCLUDED_)
#define AFX_EMBMGEOMETRY_H__0270535E_F6CF_4C2B_9F52_3BDB34627468__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if !defined(AFX_BUMPPEDGEOMETRY_H__FDCE89B9_B923_4325_AB0D_A12486C6756D__INCLUDED_)
	#include "BumppedGeometry.h"
#endif


RAPTOR_NAMESPACE_BEGIN

class RAPTOR_API CEMBMGeometry : public CBumppedGeometry
{
public:
	CEMBMGeometry(const std::string& name = "EMBM GEOMETRY");
	virtual ~CEMBMGeometry();

	//!	Rendering ( see base class )
	//!	Rendering is always done using the current rendering model.
	virtual void glRender();

	//!	Specific additional bump rendering properties
	//!	to the CGeometry rendering model ( CGeometry::SetRenderingModel
	//!	is called from this method, so only one set is necessary )
	virtual void setRenderingModel(const CRenderingModel& model);

};

RAPTOR_NAMESPACE_END

#endif
