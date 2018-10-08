// ViewPoint.h: interface for the CViewPoint class.
//
//////////////////////////////////////////////////////////////////////
#if !defined(AFX_VULKANVIEWPOINT_H__08D29395_9883_45F8_AE51_5174BD6BC19B__INCLUDED_)
#define AFX_VULKANVIEWPOINT_H__08D29395_9883_45F8_AE51_5174BD6BC19B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#if !defined(AFX_PERSISTENCE_H__5561BA28_831B_11D3_9142_EEB51CEBBDB0__INCLUDED_)
	#include "GLHierarchy/Persistence.h"
#endif
#if !defined(AFX_IVIEWPOINT_H__82071851_A036_4311_81CB_01E7E25F19E1__INCLUDED_)
	#include "Engine/IViewPoint.h"
#endif
#if !defined(AFX_3DENGINEMATRIX_H__6CD1110E_1174_4f38_A452_30FB312022D0__INCLUDED_)
	#include "Engine/3DEngineMatrix.h"
#endif


RAPTOR_NAMESPACE_BEGIN


class RAPTOR_API CVulkanViewPoint : public IViewPoint
{
public:
	CVulkanViewPoint(const std::string& name = "VULKAN_VIEW_POINT");
	virtual ~CVulkanViewPoint();

	typedef struct
	{
		GL_MATRIX modelview;
		GL_MATRIX projection;
	} Transform_t;


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
	//!	Update uniforms with viewpoint matrixes
	bool vkRenderUniforms(void);

	Transform_t transform;

	C3DEngineMatrix modelview;

	unsigned char *uniforms;
};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_VULKANVIEWPOINT_H__08D29395_9883_45F8_AE51_5174BD6BC19B__INCLUDED_)

