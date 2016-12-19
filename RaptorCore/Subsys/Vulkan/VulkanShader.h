// VulkanShader.h: interface for the CVulkanShader class.
//
//////////////////////////////////////////////////////////////////////


#if !defined(AFX_RAPTORVULKANSHADER_H__C188550F_1D1C_4531_B0A0_727CE9FF9450__INCLUDED_)
#define AFX_RAPTORVULKANSHADER_H__C188550F_1D1C_4531_B0A0_727CE9FF9450__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#if !defined(__RAPTOR_VKEXT_H__)
	#include "System/vkext.h"
#endif


RAPTOR_NAMESPACE_BEGIN


class CVulkanShader
{
public:
	CVulkanShader(VkDevice device, PFN_vkGetDeviceProcAddr vkGetDeviceProcAddr);
	virtual ~CVulkanShader(void);

	//!	Clone this shader (not functional yet)
	CVulkanShader* vkClone(void) const;

	bool loadShader(const std::string &filename);

	//VkShaderModule getModule(void) const { return shader_module; };

	//VkShaderStageFlagBits getStage(void) const { return shader_stage; };

	VkPipelineShaderStageCreateInfo getShaderStage() const;


#if defined(VK_VERSION_1_0)
	//	On a per device basis, static linkage is incorrect
	DEFAULT_LINKAGE PFN_vkCreateShaderModule vkCreateShaderModule;
	DEFAULT_LINKAGE PFN_vkDestroyShaderModule vkDestroyShaderModule;
	DEFAULT_LINKAGE PFN_vkCreateDescriptorPool vkCreateDescriptorPool;
	DEFAULT_LINKAGE PFN_vkDestroyDescriptorPool vkDestroyDescriptorPool;
#endif

private:
	//!	Forbidden operators
	CVulkanShader& operator=(const CVulkanShader&);

	//! Copy constructor.
	CVulkanShader(const CVulkanShader& shader);

#if defined(VK_VERSION_1_0)
	VkDevice		device;
	VkShaderModule	shader_module;
	VkShaderStageFlagBits shader_stage;
#endif
};


RAPTOR_NAMESPACE_END

#endif // !defined(AFX_RAPTORVULKANSHADER_H__C188550F_1D1C_4531_B0A0_727CE9FF9450__INCLUDED_)
