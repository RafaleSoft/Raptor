// VulkanDevice.cpp: implementation of the CVulkanPipeline class.
//
//////////////////////////////////////////////////////////////////////
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
#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
	#include "System/Raptor.h"
#endif
#if !defined(AFX_RAPTORERRORMANAGER_H__FA5A36CD_56BC_4AA1_A5F4_451734AD395E__INCLUDED_)
    #include "System/RaptorErrorManager.h"
#endif
#if !defined(AFX_RAPTORIO_H__87D52C27_9117_4675_95DC_6AD2CCD2E78D__INCLUDED_)
	#include "System/RaptorIO.h"
#endif


RAPTOR_NAMESPACE

#if defined(VK_VERSION_1_0)
	const uint32_t poolSizeCount = 1;
	VkDescriptorPool CVulkanShader::descriptor_pool = VK_NULL_HANDLE;
#endif

CVulkanShader::CVulkanShader(VkDevice d, PFN_vkGetDeviceProcAddr vkGetDeviceProcAddr)
#if defined(VK_VERSION_1_0)
	: device(d),
	  descriptor_set_layout(VK_NULL_HANDLE),
	  descriptor_set(VK_NULL_HANDLE),
	  layout(VK_NULL_HANDLE)
#endif
{
	vkCreateShaderModule = (PFN_vkCreateShaderModule)(vkGetDeviceProcAddr(device, "vkCreateShaderModule"));
	vkDestroyShaderModule = (PFN_vkDestroyShaderModule)(vkGetDeviceProcAddr(device, "vkDestroyShaderModule"));
	vkCreateDescriptorPool = (PFN_vkCreateDescriptorPool)(vkGetDeviceProcAddr(device, "vkCreateDescriptorPool"));
	vkDestroyDescriptorPool = (PFN_vkDestroyDescriptorPool)(vkGetDeviceProcAddr(device, "vkDestroyDescriptorPool"));
	vkResetDescriptorPool = (PFN_vkResetDescriptorPool)(vkGetDeviceProcAddr(device, "vkResetDescriptorPool"));
	vkCreateDescriptorSetLayout = (PFN_vkCreateDescriptorSetLayout)(vkGetDeviceProcAddr(device, "vkCreateDescriptorSetLayout"));
	vkDestroyDescriptorSetLayout = (PFN_vkDestroyDescriptorSetLayout)(vkGetDeviceProcAddr(device, "vkDestroyDescriptorSetLayout"));
	vkAllocateDescriptorSets = (PFN_vkAllocateDescriptorSets)(vkGetDeviceProcAddr(device, "vkAllocateDescriptorSets"));
	vkFreeDescriptorSets = (PFN_vkFreeDescriptorSets)(vkGetDeviceProcAddr(device, "vkFreeDescriptorSets"));
	vkUpdateDescriptorSets = (PFN_vkUpdateDescriptorSets)(vkGetDeviceProcAddr(device, "vkUpdateDescriptorSets"));
	vkCreatePipelineLayout = (PFN_vkCreatePipelineLayout)(vkGetDeviceProcAddr(device, "vkCreatePipelineLayout"));
	vkDestroyPipelineLayout = (PFN_vkDestroyPipelineLayout)(vkGetDeviceProcAddr(device, "vkDestroyPipelineLayout"));
	vkCreateGraphicsPipelines = NULL;
	vkCreateComputePipelines = NULL;
	vkDestroyPipeline = NULL;
	vkGetImageSubresourceLayout = NULL;
	vkCreatePipelineCache = NULL;
	vkDestroyPipelineCache = NULL;
	vkGetPipelineCacheData = NULL;
	vkMergePipelineCaches = NULL;


	//! TOTO : move this to ShaderProgram::glInitShaders()
	if (VK_NULL_HANDLE == descriptor_pool)
	{
		VkDescriptorPoolSize pPoolSizes[poolSizeCount] = {
															{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1 }
														};

		VkDescriptorPoolCreateInfo pool_create_info = { VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
														NULL,
														0, //VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT: VkDescriptorPoolCreateFlagBits
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

	//! If one pool is created per shader stages, we can call this,
	//!	Otherwise it shall be done at a global level (@see rmk on glInitShaders above)
	//if (VK_NULL_HANDLE != descriptor_set)
	//	vkResetDescriptorPool(device, descriptor_pool, 0);

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

VkPipelineLayout CVulkanShader::getPipelineLayout()
{
	if (VK_NULL_HANDLE != layout)
		return layout;

	if (VK_NULL_HANDLE == descriptor_set_layout)
	{
		VkDescriptorSetLayoutBinding uboLayoutBinding = {	0, // binding
															VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, // descriptorType
															1, // descriptorCount
															VK_SHADER_STAGE_VERTEX_BIT, // stageFlags
															NULL };

		VkDescriptorSetLayoutCreateInfo descriptor_set_layout_create_info = {	VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
																				NULL,
																				0,	// flags
																				1,	// bingind count
																				&uboLayoutBinding };
		VkResult res = VK_NOT_READY;
		res = vkCreateDescriptorSetLayout(	device,
											&descriptor_set_layout_create_info,
											CVulkanMemory::GetAllocator(),
											&descriptor_set_layout);
		CATCH_VK_ERROR(res);
		if (VK_SUCCESS != res)
			return VK_NULL_HANDLE;
	}

	if (VK_NULL_HANDLE != descriptor_set_layout)
	{
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
		VkResult res = VK_NOT_READY;
		res = vkCreatePipelineLayout(	device,
										&layout_create_info,
										CVulkanMemory::GetAllocator(),
										&layout);
		CATCH_VK_ERROR(res);
		if (VK_SUCCESS != res)
			return VK_NULL_HANDLE;
	}

	return layout;
}

void CVulkanShader::vkRender(CVulkanCommandBuffer& commandBuffer,
							 VkBuffer uniformBuffer,
							 VkDeviceSize offset,
							 VkDeviceSize size)
{
	if (VK_NULL_HANDLE == descriptor_set)
	{
		VkDescriptorSetAllocateInfo descriptor_allocate_info = {VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
																NULL,
																descriptor_pool,
																1, //descriptorSetCount;
																&descriptor_set_layout };

		VkResult res = vkAllocateDescriptorSets(device, &descriptor_allocate_info, &descriptor_set);
		CATCH_VK_ERROR(res);
		if ((VK_SUCCESS != res) || (VK_NULL_HANDLE == descriptor_set))
			return;
	}
	
	if ((VK_NULL_HANDLE != uniformBuffer) && (size > 0))
	{
		VkDescriptorBufferInfo bufferInfo = {	uniformBuffer, offset, size };
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
		vkUpdateDescriptorSets(device, 1, &descriptorWrite, 0, NULL);
	}

	commandBuffer.vkCmdBindDescriptorSets(	commandBuffer.commandBuffer,
											VK_PIPELINE_BIND_POINT_GRAPHICS,
											layout,
											0,
											1,
											&descriptor_set,
											0,
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
		CATCH_VK_ERROR(err)

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
