// ViewPoint.h: interface for the CViewPoint class.
//
//////////////////////////////////////////////////////////////////////
#if !defined(AFX_OPENGLVIEWPOINT_H__94BDC36B_27AB_41FC_848E_DD28D1BDFC13__INCLUDED_)
#define AFX_OPENGLVIEWPOINT_H__94BDC36B_27AB_41FC_848E_DD28D1BDFC13__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if !defined(AFX_PERSISTENCE_H__5561BA28_831B_11D3_9142_EEB51CEBBDB0__INCLUDED_)
	#include "GLHierarchy/Persistence.h"
#endif

#if !defined(AFX_IVIEWPOINT_H__82071851_A036_4311_81CB_01E7E25F19E1__INCLUDED_)
	#include "Engine/IViewPoint.h"
#endif


RAPTOR_NAMESPACE_BEGIN


class RAPTOR_API COpenGLViewPoint :	public IViewPoint
{
public:
	COpenGLViewPoint(const std::string& name = "OPENGL_VIEW_POINT");
	virtual ~COpenGLViewPoint();


	//!
	//!	Rendering:
	//!
	//! This method applies the viewpoint transform to the current modelview transform.
	//!	The modelview is initialized from identity, thus rendering is not cumulative 
	//!	with current modelview status.
	virtual void glvkRender(void);
	//! This method is added to render the 'projection' model
	//! of the viewpoint. It is not part of the generic 'Render'
	//! method because it should be called only once, except
	//! in somme specific case ( camera effects )
	virtual void glvkRenderViewPointModel(void);


private:
};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_OPENGLVIEWPOINT_H__94BDC36B_27AB_41FC_848E_DD28D1BDFC13__INCLUDED_)

