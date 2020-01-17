/***************************************************************************/
/*                                                                         */
/*  VulkanPipeline.h                                                       */
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


#if !defined(AFX_RAPTORVULKANPIPELINE_H__C2997B30_C6E2_4EF2_AFE3_FCD27AB5CBB7__INCLUDED_)
#define AFX_RAPTORVULKANPIPELINE_H__C2997B30_C6E2_4EF2_AFE3_FCD27AB5CBB7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#if !defined(__RAPTOR_VKEXT_H__)
	#include "System/vkext.h"
#endif


RAPTOR_NAMESPACE_BEGIN

class CRaptorDisplayConfig;
class CVulkanShader;
class CVulkanShaderStage;
class CGeometry;

class CVulkanPipeline
{
public:
	CVulkanPipeline(VkDevice device,
					VkRenderPass renderPass);
	virtual ~CVulkanPipeline(void);

	bool initPipeline(const CVulkanShaderStage* shaderStages, const CGeometry* geometry);

	bool destroyPipeline(void);

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
