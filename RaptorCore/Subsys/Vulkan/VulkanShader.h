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
#if !defined(AFX_PROGRAMPARAMETERS_H__E28A74BB_DE78_470A_A8A2_5A3EBB3F4F90__INCLUDED_)
	#include "GLHierarchy/ProgramParameters.h"
#endif

RAPTOR_NAMESPACE_BEGIN

class CVulkanCommandBuffer;


class CVulkanShader
{
public:
	CVulkanShader(VkDevice d);
	virtual ~CVulkanShader(void);

	//!	Clone this shader (not functional yet)
	CVulkanShader* vkClone(void) const;

	//!	Renders the shader, i.e. bind descriptor sets to current commandBuffer.
	//!	If uniformBuffer && size, descriptor sets are updated to uniform buffer.
	void vkRender(CVulkanCommandBuffer &commandBuffer,
				  const VkDescriptorBufferInfo& bufferInfo,
				  const VkDescriptorImageInfo& imageInfo);

	//!	Returns the number of shader stages loaded
	size_t getStageCount() const { return m_shaderModules.size(); };

	//!	Loads a shader stage.
	//!	Checks are done in CVulkanShaderStage class
	bool loadShader(const std::string &filename);

	//! Returns the shader module, if any, for the pipeline requested stage.
	VkPipelineShaderStageCreateInfo getShaderStage(size_t stage) const;

	//! Compute descriptor sets layout and create the pipeline layout
	//! corresponding to eh parameter set.
	VkPipelineLayout createPipelineLayout(const CProgramParameters &v);
	
	#if defined(VK_VERSION_1_0)
	DECLARE_VK_pipeline(STATIC_LINKAGE)
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
