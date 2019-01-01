// 3DScene.h: interface for the C3DScene class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MIRROR_H__BA9C578A_40A8_451B_9EA3_C27CB04288FA__INCLUDED_)
#define AFX_MIRROR_H__BA9C578A_40A8_451B_9EA3_C27CB04288FA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __CGLTYPES_HPP__
	#include "System/CGLTypes.h"
#endif


RAPTOR_NAMESPACE_BEGIN

class CObject3D;
class IRenderingProperties;


//! This class implements a mirror plane in the scene. The mirror is defined by
//! a point and a normal and is drawn by objects.
class RAPTOR_API CMirror
{
public:
    CMirror();
    virtual ~CMirror();

    //! Add an object to render the mirror
    //! Returns false in case of error or if object is already added.
    bool addObject(CObject3D *mirrorObject);

    //! Rendering
    void glClipRender(void);

    //! Applies appropriate transform to render objects as viewed in the mirror
    void glApplyMirror(bool apply);

    //! Returns the status of the mirror
    bool isApplied(void) const { return m_bApplied; };

    //! Sets the main plane direction of the mirror and one of its points.
    //! As opposed to environmental reflexions, we define here a 'main direction'
    //! for a unique plane representing a mirror surfoce of the object holding this property.
    //! This can help a lot to optimize rendering of mirrors reflexions.
    //! Reflexions are bounded to holding object dimensions.
    //! ( the parameter is the plane equation )
    void setMirrorPlane(const GL_COORD_VERTEX& planeNormal,
						const GL_COORD_VERTEX& planePoint);

private:
	//! Forbid copy constructor
	CMirror(const CMirror&);

	//! Forbid assignment operator
	CMirror& operator=(const CMirror&);

    //! Indicates that the mirror is currently beeing drawn
    bool  m_bApplied;

    //! The list of objects building up the mirror surface
    vector<CObject3D*>  mirrorObjects;
    
    //! The mirror plane is defined by the plane's normal and a point that belongs to the mirror.
	GL_MATRIX	m_mirror;

    //! The properties applyed for mirror rendering.
	IRenderingProperties    *m_pProperties;
};


RAPTOR_NAMESPACE_END

#endif // !defined(AFX_MIRROR_H__BA9C578A_40A8_451B_9EA3_C27CB04288FA__INCLUDED_)

