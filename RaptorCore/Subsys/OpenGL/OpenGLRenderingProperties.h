// RenderingProperties.h: interface for the CRenderingProperties class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OPENGLRENDERINGPROPERTIES_H__1F0F1E67_FC84_4772_A6EE_923BD81F91D3__INCLUDED_)
#define AFX_OPENGLRENDERINGPROPERTIES_H__1F0F1E67_FC84_4772_A6EE_923BD81F91D3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#if !defined(AFX_IRENDERINGPROPERTIES_H__634BCF2B_84B4_47F2_B460_D7FDC0F3B698__INCLUDED_)
	#include "GLHierarchy/IRenderingProperties.h"
#endif

RAPTOR_NAMESPACE_BEGIN


//! This class manages global rendering properties
//! in a scene such as texturing, lighting, fogging, ...
class COpenGLRenderingProperties : public IRenderingProperties
{
public:
	//! Constructor.
	COpenGLRenderingProperties();

	//! Copy Constructor.
	COpenGLRenderingProperties(const IRenderingProperties &properties);

	//! Destructor.
	virtual ~COpenGLRenderingProperties();


	//! Applies all the properties.
    //! The current property is made current ( provided it is not already current )
    virtual void glPushProperties(void);

    //! Restore previous properties. 
    //! The property set popped MUST be the last pushed,
    //! otherwise this method has no effects.
    virtual void glPopProperties(void);

	//!	Read all current OpenGL rendering properties from the active display
	virtual void glGrabProperties(void);


private:
	//!	Internal rendering of gl properties
    void glRender(void);

	//!	Save global properties berore rendering and restore them after pop properties.
	PROPERTY_SETTING	globalTexturing;
	PROPERTY_SETTING	globalLighting;
	PROPERTY_SETTING	globalDepth;
	PROPERTY_SETTING	globalCull;
	PROPERTY_SETTING	globalBlend;
	PROPERTY_SETTING	globalStencil;
	PROPERTY_SETTING	globalMultisample;
	PROPERTY_SETTING	globalClampFloat;
	PROPERTY_SETTING	globalWireframe;
};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_OPENGLRENDERINGPROPERTIES_H__1F0F1E67_FC84_4772_A6EE_923BD81F91D3__INCLUDED_)

