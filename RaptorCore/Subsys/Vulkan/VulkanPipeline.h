/***************************************************************************/
/*                                                                         */
/*  VulkanPipeline.h                                                       */
/*                                                                         */
/*    Raptor OpenGL & Vulkan realtime 3D Engine SDK.                       */
/*                                                                         */
/*  Copyright 1998-2021 by                                                 */
/*  Fabrice FERRAND.                                                       */
/*                                                                         */
/*  This file is part of the Raptor project, and may only be used,         */
/*  modified, and distributed under the terms of the Raptor project        */
/*  license, LICENSE.  By continuing to use, modify, or distribute         */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/


#if !defined(AFX_RAPTORVULKANPIPELINE_H__C2997B30_C6E2_4EF2_AFE3_FCD27AB5CBB7__INCLUDED_)
#define AFX_RAPTORVULKANPIPELINE_H__C2997B30_C6E2_4EF2_AFE3_FCD27AB5CBB7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#if !defined(__RAPTOR_VKEXT_H__)
	#include "System/vkext.h"
#endif
#if !defined(AFX_RAPTORPIPELINE_972B6860_BDAE_4303_962E_3DAC4ECF7F7B__INCLUDED_)
	#include "GLHierarchy/IRaptorPipeline.h"
#endif


RAPTOR_NAMESPACE_BEGIN

class CRaptorDisplayConfig;
class CVulkanShader;


class CVulkanPipeline : public IRaptorPipeline
{
public:
	CVulkanPipeline(VkDevice device,
					VkRenderPass renderPass);
	virtual ~CVulkanPipeline(void);

	//! Implement base class. @see IRaptorPipeline.
	virtual bool initPipeline(const CShaderProgram* shaderStages, const CGeometry* geometry);

	//! Implement base class. @see IRaptorPipeline.
	virtual bool destroyPipeline(void);


	VkPipeline getPipeline(void) const { return pipeline; };


#if defined(VK_VERSION_1_0)
	DECLARE_VK_pipeline(STATIC_LINKAGE)

private:
	VkDevice		device;
	VkRenderPass	renderPass;
	VkPipeline		pipeline;
#endif
};


RAPTOR_NAMESPACE_END

#endif // !defined(AFX_RAPTORVULKANPIPELINE_H__C2997B30_C6E2_4EF2_AFE3_FCD27AB5CBB7__INCLUDED_)
