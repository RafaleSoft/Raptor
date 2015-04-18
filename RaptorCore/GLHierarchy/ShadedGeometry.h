// ShadedGeometry.h: interface for the CShadedGeometry class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SHADEDGEOMETRY_H__E56C66F7_2DF6_497B_AA0F_19DDC11390F9__INCLUDED_)
#define AFX_SHADEDGEOMETRY_H__E56C66F7_2DF6_497B_AA0F_19DDC11390F9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if !defined(AFX_GEOMETRY_H__B42ABB87_80E8_11D3_97C2_DE5C28000000__INCLUDED_)
	#include "Geometry.h"
#endif


RAPTOR_NAMESPACE_BEGIN

class CShader;
class CRenderingProperties;

class CAmbientOcclusionShader;

//!
//!	Class CShaded Geometry.
//!	This class implements a generic "shaded" geometry.
//!	A shaded geometry always own a light observer, though
//!	there is not necessarily a shader ( single geometric primitive ).
//!
class RAPTOR_API CShadedGeometry : public CGeometry  
{
public:
	//!	Constructor
	CShadedGeometry(const std::string& name = "SHADED GEOMETRY");
	
	//!	Destructor.
	//!	Light observer and shader are deleted. Do not store
	//!	them unless you register to destruction notification.
	virtual ~CShadedGeometry();

	//!	Renders the geometry shaded if it has one initialised.
	virtual void glRender();

	//!	Assignement to be able to create a shaded geometry from a base geometry.
	//!	Shader is not modified of left uninitialised.
	CShadedGeometry& operator=(const CGeometry& geometry);

    //! Standard assignment operator. The embedded shader is released
    //! to be assigned to 'geometry'
    CShadedGeometry& operator=(const CShadedGeometry& shadedGeometry);

	//!	Returns true if the geometry has a shaders.
	//!	This helper avoids the creation of unnecessary shaders
	bool hasShader(void) const { return (m_pShader != NULL); };

	//!	Returns the shader of this object.
    //! The shader can be initialized by the shadedgeometry automatically
    //! or it can be a shader given with SetShader. 
	virtual CShader * const getShader(void);

	//!	Returns an ambient occlusion shader for this object.
    //! The shader is configured and properly rendered by the 
	//!	ambient occlusion environment (if any initialized on the current scene).
	virtual CShader * const getAmbientOcclusionShader(void);

    //! Assigns a shader to the shadedgeometry. The previous one is released.
    virtual void setShader(CShader *shader);

    //! Override Display rendering properties tu use local object specific shading.
    void overrideShading(const CRenderingProperties& override);

	//! Implements CPersistence
    DECLARE_IO
	DECLARE_CLASS_ID(CShadedGeometryClassID,"ShadedGeometry",CGeometry)



protected:
	//!	Inherit from persistence to observe shader life-cycle
	void unLink(const CPersistence* obj);


private:
	CShader					*m_pShader;
	CShader					*m_pAOShader;
    CRenderingProperties	*m_pOverride;
};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_SHADEDGEOMETRY_H__E56C66F7_2DF6_497B_AA0F_19DDC11390F9__INCLUDED_)

