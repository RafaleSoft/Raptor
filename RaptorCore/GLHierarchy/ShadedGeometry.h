/***************************************************************************/
/*                                                                         */
/*  ShadedGeometry.h                                                       */
/*                                                                         */
/*    Raptor OpenGL & Vulkan realtime 3D Engine SDK.                       */
/*                                                                         */
/*  Copyright 1998-2019 by                                                 */
/*  Fabrice FERRAND.                                                       */
/*                                                                         */
/*  This file is part of the Raptor project, and may only be used,         */
/*  modified, and distributed under the terms of the Raptor project        */
/*  license, LICENSE.  By continuing to use, modify, or distribute         */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/


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
class IRenderingProperties;
class CAmbientOcclusionShader;
RAPTOR_INTERFACE IRaptorPipeline;


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
	virtual void vkRender(CVulkanCommandBuffer& commandBuffer,
						  VkBuffer vertexBinding,
						  VkBuffer indexBinding);

	//!	Create the pipeline for rendering in a scene.
	IRaptorPipeline* glvkCreatePipeline(void);

	//!	Assignement to be able to create a shaded geometry from a base geometry.
	//!	Shader is not modified of left uninitialised.
	CShadedGeometry& operator=(const CGeometry& geometry);

    //! Standard assignment operator. The embedded shader is released
    //! to be assigned to 'geometry'
    CShadedGeometry& operator=(const CShadedGeometry& shadedGeometry);

	//!	Returns true if the geometry has a shaders.
	//!	This helper avoids the creation of unnecessary shaders
	bool hasShader(void) const { return (m_pShader != NULL); };

	//!	Returns the base shader of this object.
    //! The shader can be initialized by the shadedgeometry automatically
    //! or it can be a shader given with SetShader. 
	virtual CShader * const getShader(void);

	//!	Returns the list of shaders in this object hierachy.
	virtual std::vector<CShader*> getShaders(void);

	//!	Returns an ambient occlusion shader for this object.
    //! The shader is configured and properly rendered by the 
	//!	ambient occlusion environment (if any initialized on the current scene).
	virtual CShader * const getAmbientOcclusionShader(void);

    //! Assigns a shader to the shadedgeometry. The previous one is released.
    virtual void setShader(CShader *shader);

	//!	Implement base class to override shading.
	virtual void removeModel(CGeometry::RENDERING_MODEL);


	//! Implements CPersistence
    DECLARE_IO
	DECLARE_CLASS_ID(CShadedGeometryClassID,"ShadedGeometry",CGeometry)



protected:
	//!	Inherit from persistence to observe shader life-cycle
	void unLink(const CPersistence* obj);

	//! Override Display rendering properties tu use local object specific shading.
	void overrideShading(const IRenderingProperties& override);


private:
	CShader					*m_pShader;
	CShader					*m_pAOShader;
	IRenderingProperties	*m_pOverride;
};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_SHADEDGEOMETRY_H__E56C66F7_2DF6_497B_AA0F_19DDC11390F9__INCLUDED_)

