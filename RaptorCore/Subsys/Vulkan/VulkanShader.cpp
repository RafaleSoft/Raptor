/***************************************************************************/
/*                                                                         */
/*  VulkanShader.cpp                                                       */
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



#include "Subsys\CodeGeneration.h"

#if !defined(AFX_RAPTORVULKANSHADER_H__C188550F_1D1C_4531_B0A0_727CE9FF9450__INCLUDED_)
	#include "Subsys/Vulkan/VulkanShader.h"
#endif
#if !defined(AFX_RAPTORVULKANMEMORY_H__72256FF7_DBB9_4B9C_9BF7_C36F425CF811__INCLUDED_)
	#include "Subsys/Vulkan/VulkanMemory.h"
#endif
#if !defined(AFX_RAPTORVULKANCOMMANDBUFFER_H__0398BABD_747B_4DFE_94AA_B026BDBD03B1__INCLUDED_)
	#include "Subsys/Vulkan/VulkanCommandBuffer.h"
#endif
#ifndef __vkext_macros_h_
	#include "System/VKEXTMacros.h"
#endif
#if !defined(AFX_RAPTORIO_H__87D52C27_9117_4675_95DC_6AD2CCD2E78D__INCLUDED_)
	#include "System/RaptorIO.h"
#endif


RAPTOR_NAMESPACE

#if defined(VK_VERSION_1_0)
	const uint32_t poolSizeCount = 2;
	VkDescriptorPool CVulkanShader::descriptor_pool = VK_NULL_HANDLE;

	IMPLEMENT_RAPTOR_VK_pipeline(CVulkanShader)
#endif

CVulkanShader::CVulkanShader(VkDevice d)
#if defined(VK_VERSION_1_0)
	: device(d),
	  descriptor_set_layout(VK_NULL_HANDLE),
	  descriptor_set(VK_NULL_HANDLE),
	  layout(VK_NULL_HANDLE)
#endif
{
	//! TOTO : move this to ShaderProgram::glInitShaders()
	if (VK_NULL_HANDLE == descriptor_pool)
	{
		VkDescriptorPoolSize pPoolSizes[poolSizeCount] = {
															{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1 },
															{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1}
														};

		VkDescriptorPoolCreateInfo pool_create_info = { VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
														NULL,
														VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT, // VkDescriptorPoolCreateFlagBits
														1, // maxSets
														poolSizeCount, // poolSizeCount
														pPoolSizes };

		VkResult res = VK_NOT_READY;
		res = vkCreateDescriptorPool(device, &pool_create_info, CVulkanMemory::GetAllocator(), &descriptor_pool);
		CATCH_VK_ERROR(res);
	}
}

CVulkanShader::CVulkanShader(const CVulkanShader& shader)
#if defined(VK_VERSION_1_0)
	: device(shader.device),
	  descriptor_set_layout(shader.descriptor_set_layout),
	  descriptor_set(shader.descriptor_set),
	  layout(shader.layout)
#endif
{
}

CVulkanShader::~CVulkanShader(void)
{
#if defined(VK_VERSION_1_0)
	if (VK_NULL_HANDLE != layout)
		vkDestroyPipelineLayout(device, layout, CVulkanMemory::GetAllocator());

	for (size_t i = 0; i < m_shaderModules.size(); i++)
	{
		SHADER_MODULE &module = m_shaderModules[i];
		if (VK_NULL_HANDLE != module.shader_module)
			vkDestroyShaderModule(device, module.shader_module, NULL);
	}
	m_shaderModules.clear();

	if (VK_NULL_HANDLE != descriptor_set_layout)
		vkDestroyDescriptorSetLayout(device, descriptor_set_layout, CVulkanMemory::GetAllocator());

	if (VK_NULL_HANDLE != descriptor_pool)
		vkDestroyDescriptorPool(device, descriptor_pool, CVulkanMemory::GetAllocator());

#endif
}

CVulkanShader* CVulkanShader::vkClone(void) const
{
	return new CVulkanShader(*this);
}


VkPipelineShaderStageCreateInfo CVulkanShader::getShaderStage(size_t stage) const
{
	VkPipelineShaderStageCreateInfo shaderStage = { VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
													NULL,
													0, //VkPipelineShaderStageCreateFlags               
													VK_SHADER_STAGE_ALL,
													VK_NULL_HANDLE,
													"main",
													NULL };	// VkSpecializationInfo

	if (stage < m_shaderModules.size())
	{
		const SHADER_MODULE &module = m_shaderModules[stage];
		shaderStage.module = module.shader_module;
		shaderStage.stage = module.shader_stage;
	}

	return shaderStage;
}

VkPipelineLayout CVulkanShader::createPipelineLayout(const CProgramParameters &v)
{
	//!
	//! Check internal layouts and descriptors and release them.
	//!
	if (VK_NULL_HANDLE != layout)
		vkDestroyPipelineLayout(device, layout, CVulkanMemory::GetAllocator());
	layout = VK_NULL_HANDLE;

	if (VK_NULL_HANDLE != descriptor_set_layout)
		vkDestroyDescriptorSetLayout(device, descriptor_set_layout, CVulkanMemory::GetAllocator());
	descriptor_set_layout = VK_NULL_HANDLE;

	if (VK_NULL_HANDLE != descriptor_set)
		vkFreeDescriptorSets(device, descriptor_pool, 1, &descriptor_set);
	descriptor_set = VK_NULL_HANDLE;
		
	//!
	//! Create descriptor sets layouts mapping given shader parameters
	//!
	std::vector<VkDescriptorSetLayoutBinding> bindings;
	for (size_t i = 0; i < v.getNbParameters(); i++)
	{
		const CProgramParameters::CParameterBase& param_value = v[i];

		VkDescriptorType type = (VkDescriptorType)param_value.locationType;
		VkDescriptorSetLayoutBinding layoutBinding = {	param_value.locationIndex, // binding. TODO: binding shall be unsigned
														type,	// descriptorType
														1, // descriptorCount
														VK_SHADER_STAGE_VERTEX_BIT, // stageFlags
														NULL };
		if ((VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER == type) ||
			(VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE == type))
			layoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

		bindings.push_back(layoutBinding);
	}

	VkDescriptorSetLayoutCreateInfo descriptor_set_layout_create_info = {	VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
																			NULL,
																			0,	// flags
																			bindings.size(),	// bingind count
																			bindings.data() };
	VkResult res = VK_NOT_READY;
	res = vkCreateDescriptorSetLayout(	device,
										&descriptor_set_layout_create_info,
										CVulkanMemory::GetAllocator(),
										&descriptor_set_layout);
	CATCH_VK_ERROR(res);
	if ((VK_SUCCESS != res) || (VK_NULL_HANDLE == descriptor_set_layout))
		return VK_NULL_HANDLE;


	//!
	//! Allocate descriptor sets with new created layout
	//!
	VkDescriptorSetAllocateInfo descriptor_allocate_info = {VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
															NULL,
															descriptor_pool,
															1, //descriptorSetCount;
															&descriptor_set_layout };

	res = vkAllocateDescriptorSets(device, &descriptor_allocate_info, &descriptor_set);
	CATCH_VK_ERROR(res);
	if ((VK_SUCCESS != res) || (VK_NULL_HANDLE == descriptor_set))
		return VK_NULL_HANDLE;


	//!
	//! Finally create pipeline layout
	//!
	const uint32_t setLayoutCount = 1;
	VkDescriptorSetLayout pSetLayouts[setLayoutCount];
	pSetLayouts[0] = descriptor_set_layout;

	VkPipelineLayoutCreateInfo layout_create_info = {	VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
														NULL,	// next
														0,		// flags
														setLayoutCount, // uint32_t setLayoutCount
														pSetLayouts, //VkDescriptorSetLayout   *pSetLayouts
														0,		//uint32_t pushConstantRangeCount
														NULL };	//VkPushConstantRange     *pPushConstantRanges
	res = vkCreatePipelineLayout(	device,
									&layout_create_info,
									CVulkanMemory::GetAllocator(),
									&layout);
	CATCH_VK_ERROR(res);
	if ((VK_SUCCESS != res) || (VK_NULL_HANDLE == layout))
		return VK_NULL_HANDLE;

	return layout;
}

void CVulkanShader::vkRender(CVulkanCommandBuffer& commandBuffer,
							 const VkDescriptorBufferInfo& bufferInfo,
							 const VkDescriptorImageInfo& imageInfo)
{
	if ((VK_NULL_HANDLE != bufferInfo.buffer) && (bufferInfo.range > 0))
	{
		VkWriteDescriptorSet descriptorWrite = {VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
												NULL,
												descriptor_set,
												0,	// dstBinding
												0,	// dstArrayElement
												1,	// descriptorCount;
												VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,	// descriptorType
												NULL,
												&bufferInfo,
												NULL };
		VkWriteDescriptorSet descriptorWrite2 = {	VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
													NULL,
													descriptor_set,
													1,	// dstBinding
													0,	// dstArrayElement
													1,	// descriptorCount;
													VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,	// descriptorType
													&imageInfo,
													NULL,
													NULL };
		const VkWriteDescriptorSet writes[2] = { descriptorWrite, descriptorWrite2};
		vkUpdateDescriptorSets(device, 2, writes, 0, NULL);
	}

	commandBuffer.vkCmdBindDescriptorSets(	commandBuffer.commandBuffer,
											VK_PIPELINE_BIND_POINT_GRAPHICS,
											layout,
											0,
											1,
											&descriptor_set,
											0,	//	No dynamic offsets (no DYNAMIC_BUFFER)
											NULL);
}

bool CVulkanShader::loadShader(const std::string &filename)
{
	//std::string::size_type pos = filename.find(".spv",filename.length()-4);
	//if (std::string::npos == pos)
	//{
		// try to compile SPIR-V ?
		//return false;
	//}
#if defined(VK_VERSION_1_0)
	SHADER_MODULE module;

	std::string fname = filename;
	if ((std::string::npos != filename.find(".frag",filename.length()-5)) ||
		(std::string::npos != filename.find("frag.spv",filename.length()-8)))
	{
		module.shader_stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	}
	else if ((std::string::npos != filename.find(".vert",filename.length()-5)) ||
			(std::string::npos != filename.find("vert.spv",filename.length()-8)))
	{
		module.shader_stage = VK_SHADER_STAGE_VERTEX_BIT;
	}
	else if ((std::string::npos != filename.find(".tesc", filename.length() - 5)) ||
			 (std::string::npos != filename.find("tesc.spv", filename.length() - 8)))
	{
		module.shader_stage = VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
	}
	else if ((std::string::npos != filename.find(".tese", filename.length() - 5)) ||
			 (std::string::npos != filename.find("tese.spv", filename.length() - 8)))
	{
		module.shader_stage = VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
	}
	else if ((std::string::npos != filename.find(".geom", filename.length() - 5)) ||
			 (std::string::npos != filename.find("geom.spv", filename.length() - 8)))
	{
		module.shader_stage = VK_SHADER_STAGE_GEOMETRY_BIT;
	}
	else if ((std::string::npos != filename.find(".comp", filename.length() - 5)) ||
			 (std::string::npos != filename.find("comp.spv", filename.length() - 8)))
	{
		module.shader_stage = VK_SHADER_STAGE_GEOMETRY_BIT;
	}
	else
		return false;

	CRaptorIO *io = CRaptorIO::Create(fname.c_str(),CRaptorIO::DISK_READ);
	if (NULL == io)
		return false;

	if (io->getStatus() == CRaptorIO::IO_OK)
	{
		size_t code_size = io->getSize();
		uint32_t *code = new uint32_t[code_size/4+1];
		io->read(code,code_size);
		delete io;
		
		VkResult res = VK_NOT_READY;
		VkShaderModuleCreateInfo shader_module_create_info = {	VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO, // VkStructureType
																NULL,                                        // const void
																0,                                           // VkShaderModuleCreateFlags
																code_size,                                   // size_t
																code										 // const uint32_t*
															  }; 

		res = vkCreateShaderModule( device, &shader_module_create_info, NULL, &module.shader_module );
		CATCH_VK_ERROR(res)

		delete [] code;
		if (VK_SUCCESS == res)
			m_shaderModules.push_back(module);

		return (VK_SUCCESS == res);
	}
	else
		return false;
#else
	return false;
#endif
}
