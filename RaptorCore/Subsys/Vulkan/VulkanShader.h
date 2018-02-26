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

class CVulkanCommandBuffer;


class CVulkanShader
{
public:
	CVulkanShader(VkDevice device, PFN_vkGetDeviceProcAddr vkGetDeviceProcAddr);
	virtual ~CVulkanShader(void);

	//!	Clone this shader (not functional yet)
	CVulkanShader* vkClone(void) const;

	//!	Renders the shader, i.e. bind descriptor sets to current commandBuffer.
	//!	If uniformBuffer && size, descriptor sets are updated to uniform buffer.
	void vkRender(CVulkanCommandBuffer &commandBuffer,
				  VkBuffer uniformBuffer,
				  VkDeviceSize offset,
				  VkDeviceSize size);

	//!	Returns the number of shader stages loaded
	size_t getStageCount() const { return m_shaderModules.size(); };

	//!	Loads a shader stage.
	//!	Checks are done in CVulkanShaderStage class
	bool loadShader(const std::string &filename);

	VkPipelineShaderStageCreateInfo getShaderStage(size_t stage) const;

	VkPipelineLayout getPipelineLayout();


#if defined(VK_VERSION_1_0)
	//	On a per device basis, STATIC_LINKAGE is incorrect
	DECLARE_VK_pipeline(DEFAULT_LINKAGE)
#endif

private:
	//!	Forbidden operators
	CVulkanShader& operator=(const CVulkanShader&);

	//! Copy constructor.
	CVulkanShader(const CVulkanShader& shader);

#if defined(VK_VERSION_1_0)
	VkDevice					device;

	//!	Descriptor sets management
	static	VkDescriptorPool	descriptor_pool;
	VkDescriptorSetLayout	descriptor_set_layout;
	VkDescriptorSet			descriptor_set;
	VkPipelineLayout		layout;

	typedef struct SHADER_MODULE_t
	{
		VkShaderModule	shader_module;
		VkShaderStageFlagBits shader_stage;
	} SHADER_MODULE;

	typedef struct DESCRIPTOR_DATA_t
	{
		VkDeviceSize offset;
		VkDeviceSize size;
	} DESCRIPTOR_DATA;

	DESCRIPTOR_DATA			m_descriptorData;
	vector<SHADER_MODULE>	m_shaderModules;
#endif
};


RAPTOR_NAMESPACE_END

#endif // !defined(AFX_RAPTORVULKANSHADER_H__C188550F_1D1C_4531_B0A0_727CE9FF9450__INCLUDED_)
