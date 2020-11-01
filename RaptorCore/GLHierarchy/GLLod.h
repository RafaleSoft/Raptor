// GLLod.h: interface for the CGLLod class.
//
//////////////////////////////////////////////////////////////////////
#if !defined(AFX_GLLOD_H__95ED92DC_1BF7_4869_912C_655779B2ED7B__INCLUDED_)
#define AFX_GLLOD_H__95ED92DC_1BF7_4869_912C_655779B2ED7B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if !defined(AFX_OBJECT3DINSTANCE_H__A2627662_F5F9_11D3_9142_CFEB8E9F2745__INCLUDED_)
	#include "Object3DInstance.h"
#endif


RAPTOR_NAMESPACE_BEGIN


class RAPTOR_API CGLLod : public CObject3DInstance
{
public:
    //! Constructor: 
    //! If level0 != NULL, it is added as the first, always visible ( depth = 0 ), level of this lod,
    //! having thus the same behaviour of @see CObject3DInstance.
    //! Otherwise, an instance of CGLLod is simply constructed uninitialised.
	CGLLod(CObject3D * const level0 = NULL, const std::string& name = "GL_LOD");
	virtual ~CGLLod();

    //! This method adds a level to the lod. Objects should be correlated enough
	//! If distance is closer, the previous level if it exists is selected during rendering.
	//!	A level is always needed for rendering and shall not interfere with near/far clipping:
	//!	the first and last level will be rendered at 0 or infinite distance, even if the user
	//! does not provide fromDepth equal to 0.
    //! @param fromDepth : distance to the viewpoint from which the level is viewable.
	//! @param obj : the object to be rendered and representing the level.
    //! @return false is case of an error, true otherwise.
	virtual bool addLevel(float fromDepth, CObject3D *obj);

    //! Returns the number of levels of this lod.
	virtual size_t getNbLod(void) const { return lods.size(); };

    //! Returns the 'numLod' level;
	virtual CObject3D* const getLod(size_t numLod) const;

    //! The method that returns the appropriate level
    CObject3D* const glGetLod(void) const;

    //! This method renders the object clipped to viewport.
    //! @see CObject3D.
	virtual void glClipRender();

	//! Rendering : selects the appropriate level and assign it to the reference ( of the instance ).
    //! Then, the rendering method called is of CObject3DInstance.
	virtual void glRender();


    //! Implements CObject3D: creates the list of contours for this objet.
    virtual vector<CObject3DContour*> createContours(void);

    //! Implements CPersistence: serialization
	DECLARE_IO
	DECLARE_CLASS_ID(CGLLodClassID,"Lod",CObject3DInstance)


private:
    //! implements 
    virtual void unLink(const CPersistence* obj);

    //! A helper to import a level
    bool importLevel(CRaptorIO& io);

    typedef struct LOD_t
    {
	    float		fromDepth;
	    CObject3D	*obj;
    } LOD;

	vector<LOD>		lods;
};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_GLLOD_H__95ED92DC_1BF7_4869_912C_655779B2ED7B__INCLUDED_)
