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

	virtual PROPERTY_SETTING getCurrentTexturing(void) const;

	virtual PROPERTY_SETTING getCurrentLighting(void) const;


private:
	//!	Internal rendering of gl properties
    void glRender(void);

	static COpenGLRenderingProperties m_globalProperties;
};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_OPENGLRENDERINGPROPERTIES_H__1F0F1E67_FC84_4772_A6EE_923BD81F91D3__INCLUDED_)

