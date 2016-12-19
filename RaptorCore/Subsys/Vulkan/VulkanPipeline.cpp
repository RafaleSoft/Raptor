// VulkanDevice.cpp: implementation of the CVulkanPipeline class.
//
//////////////////////////////////////////////////////////////////////
#include "Subsys/CodeGeneration.h"

#if !defined(AFX_RAPTORVULKANPIPELINE_H__C2997B30_C6E2_4EF2_AFE3_FCD27AB5CBB7__INCLUDED_)
	#include "Subsys/Vulkan/VulkanPipeline.h"
#endif
#if !defined(AFX_RAPTORVULKANSHADER_H__C188550F_1D1C_4531_B0A0_727CE9FF9450__INCLUDED_)
	#include "Subsys/Vulkan/VulkanShader.h"
#endif
#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
	#include "System/Raptor.h"
#endif
#if !defined(AFX_RAPTORERRORMANAGER_H__FA5A36CD_56BC_4AA1_A5F4_451734AD395E__INCLUDED_)
    #include "System/RaptorErrorManager.h"
#endif
#if !defined(AFX_RAPTORVULKANMEMORY_H__72256FF7_DBB9_4B9C_9BF7_C36F425CF811__INCLUDED_)
	#include "Subsys/Vulkan/VulkanMemory.h"
#endif
#if !defined(AFX_VULKANSHADERSTAGE_H__EF5769B8_470D_467F_9FDE_553142C81698__INCLUDED_)
	#include "GLHierarchy/VulkanShaderStage.h"
#endif


RAPTOR_NAMESPACE

CVulkanPipeline::CVulkanPipeline(	VkDevice d,
									VkRenderPass r,
									PFN_vkGetDeviceProcAddr vkGetDeviceProcAddr)
#if defined(VK_VERSION_1_0)
	: device(d), renderPass(r),
	pipeline(VK_NULL_HANDLE),
	descriptor_set_layout(VK_NULL_HANDLE),
	layout(VK_NULL_HANDLE),
	descriptor_pool(VK_NULL_HANDLE),
	descriptor_set(VK_NULL_HANDLE)
#endif
{
#if defined(VK_VERSION_1_0)
	vkCreateGraphicsPipelines = (PFN_vkCreateGraphicsPipelines)(vkGetDeviceProcAddr(device, "vkCreateGraphicsPipelines"));
	vkCreateComputePipelines = (PFN_vkCreateComputePipelines)(vkGetDeviceProcAddr(device, "vkCreateComputePipelines"));
	vkDestroyPipeline = (PFN_vkDestroyPipeline)(vkGetDeviceProcAddr(device, "vkDestroyPipeline"));
	vkCreatePipelineLayout = (PFN_vkCreatePipelineLayout)(vkGetDeviceProcAddr(device, "vkCreatePipelineLayout"));
	vkDestroyPipelineLayout = (PFN_vkDestroyPipelineLayout)(vkGetDeviceProcAddr(device, "vkDestroyPipelineLayout"));
	vkCreateShaderModule = (PFN_vkCreateShaderModule)(vkGetDeviceProcAddr(device, "vkCreateShaderModule"));
	vkDestroyShaderModule = (PFN_vkDestroyShaderModule)(vkGetDeviceProcAddr(device, "vkDestroyShaderModule"));
	vkCreateDescriptorSetLayout = (PFN_vkCreateDescriptorSetLayout)(vkGetDeviceProcAddr(device, "vkCreateDescriptorSetLayout"));
	vkDestroyDescriptorSetLayout = (PFN_vkDestroyDescriptorSetLayout)(vkGetDeviceProcAddr(device, "vkDestroyDescriptorSetLayout"));
	vkGetImageSubresourceLayout = (PFN_vkGetImageSubresourceLayout)(vkGetDeviceProcAddr(device, "vkGetImageSubresourceLayout"));
	vkCreatePipelineCache = (PFN_vkCreatePipelineCache)(vkGetDeviceProcAddr(device, "vkCreatePipelineCache"));
	vkDestroyPipelineCache = (PFN_vkDestroyPipelineCache)(vkGetDeviceProcAddr(device, "vkDestroyPipelineCache"));
	vkGetPipelineCacheData = (PFN_vkGetPipelineCacheData)(vkGetDeviceProcAddr(device, "vkGetPipelineCacheData"));
	vkMergePipelineCaches = (PFN_vkMergePipelineCaches)(vkGetDeviceProcAddr(device, "vkMergePipelineCaches"));
	vkCreateDescriptorPool = (PFN_vkCreateDescriptorPool)(vkGetDeviceProcAddr(device, "vkCreateDescriptorPool"));
	vkDestroyDescriptorPool = (PFN_vkDestroyDescriptorPool)(vkGetDeviceProcAddr(device, "vkDestroyDescriptorPool"));
	vkResetDescriptorPool = (PFN_vkResetDescriptorPool)(vkGetDeviceProcAddr(device, "vkResetDescriptorPool"));
	vkAllocateDescriptorSets = (PFN_vkAllocateDescriptorSets)(vkGetDeviceProcAddr(device, "vkAllocateDescriptorSets"));
	vkFreeDescriptorSets = (PFN_vkFreeDescriptorSets)(vkGetDeviceProcAddr(device, "vkFreeDescriptorSets"));
	vkUpdateDescriptorSets = (PFN_vkUpdateDescriptorSets)(vkGetDeviceProcAddr(device, "vkUpdateDescriptorSets"));
#endif
}

CVulkanPipeline::~CVulkanPipeline(void)
{
}


bool CVulkanPipeline::destroyPipeline()
{
	if (VK_NULL_HANDLE == device)
		return false;

	if (VK_NULL_HANDLE != descriptor_set)
		vkResetDescriptorPool(device, descriptor_pool, 0);

	if (VK_NULL_HANDLE != descriptor_pool)
		vkDestroyDescriptorPool(device, descriptor_pool, CVulkanMemory::GetAllocator());

	if (VK_NULL_HANDLE == descriptor_set_layout)
		return false;
	vkDestroyDescriptorSetLayout(device, descriptor_set_layout, CVulkanMemory::GetAllocator());

	if (VK_NULL_HANDLE == layout)
		return false;
	vkDestroyPipelineLayout(device, layout, CVulkanMemory::GetAllocator());

	if (VK_NULL_HANDLE == pipeline)
		return false;
	vkDestroyPipeline(device, pipeline, CVulkanMemory::GetAllocator());

	return true;
}


bool CVulkanPipeline::initPipeline(const CVulkanShaderStage& shaderStages)
{
	VkResult res = VK_NOT_READY;

	//VkPipelineCache pipelineCache;
	const uint32_t poolSizeCount = 1;
	VkDescriptorPoolSize pPoolSizes[poolSizeCount] = { { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1 } };
	VkDescriptorPoolCreateInfo pool_create_info = { VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
													NULL,
													0, //VkDescriptorPoolCreateFlagBits
													1, // maxSets
													poolSizeCount, // poolSizeCount
													pPoolSizes };
	res = vkCreateDescriptorPool(device, &pool_create_info, CVulkanMemory::GetAllocator(), &descriptor_pool);
	CATCH_VK_ERROR(res);
	if (VK_SUCCESS != res)
		return false;

	uint32_t stageCount = shaderStages.getStageCount();
	VkPipelineShaderStageCreateInfo *shader_stages = new VkPipelineShaderStageCreateInfo[stageCount];
	for (uint32_t i=0;i<stageCount;i++)
	{
		CVulkanShader* pShader = shaderStages.getShader(i);
		shader_stages[i] = pShader->getShaderStage();
	}
	
	VkVertexInputBindingDescription vertex_binding_description_info = {	0,	// binding number
																		4*sizeof(float),	//stride
																		VK_VERTEX_INPUT_RATE_VERTEX };
	VkVertexInputBindingDescription colors_binding_description_info = { 1,	// binding number
																		4 * sizeof(float),	//stride
																		VK_VERTEX_INPUT_RATE_VERTEX };
	VkVertexInputBindingDescription bindings[2] = { vertex_binding_description_info, colors_binding_description_info };

	VkVertexInputAttributeDescription vertex_input_attribute_info[2] = { {	0,
																			vertex_binding_description_info.binding,
																			VK_FORMAT_R32G32B32A32_SFLOAT,
																			0 },
																		  { 1,
																			colors_binding_description_info.binding,
																			VK_FORMAT_R32G32B32A32_SFLOAT,
																			0 } }; // 4 * sizeof(float)


	VkPipelineVertexInputStateCreateInfo vertex_input_state_create_info = {	VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
																			NULL,
																			0,	//VkPipelineVertexInputStateCreateFlags
																			2, //vertexBindingDescriptionCount
																			&bindings[0], //VkVertexInputBindingDescription
																			2,	//vertexAttributeDescriptionCount
																			&vertex_input_attribute_info[0] }; // VkVertexInputAttributeDescription



	VkPipelineInputAssemblyStateCreateInfo input_assembly_state_create_info = {	VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
																				NULL,
																				0, //VkPipelineInputAssemblyStateCreateFlags
																				VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
																				VK_FALSE };	//primitiveRestartEnable
	//VkPipelineTessellationStateCreateInfo tesselation_state_create_info;
	VkPipelineViewportStateCreateInfo  viewport_state_create_info = {	VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
																		NULL,
																		0,	//VkPipelineViewportStateCreateFlags
																		1,	//viewportCount;
																		NULL, //&viewport,
																		1,	//scissorCount;
																		NULL }; //&scissor };

	VkPipelineRasterizationStateCreateInfo rasterization_state_create_info = {	VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
																				NULL,
																				0,
																				VK_FALSE, //depthClampEnable
																				VK_FALSE, //rasterizerDiscardEnable
																				VK_POLYGON_MODE_FILL, //polygonMode
																				VK_CULL_MODE_BACK_BIT, //cullMode
																				VK_FRONT_FACE_COUNTER_CLOCKWISE, //frontFace
																				VK_FALSE, //depthBiasEnable;
																				0.0f, //depthBiasConstantFactor;
																				0.0f, //depthBiasClamp;
																				0.0f, //depthBiasSlopeFactor;
																				1.0f };	// lineWidth

	VkPipelineMultisampleStateCreateInfo multisample_state_create_info = {	VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
																			NULL,
																			0,
																			VK_SAMPLE_COUNT_1_BIT,
																			VK_FALSE,
																			1.0f,
																			NULL, //	VkSampleMask
																			VK_FALSE,	//alphaToCoverageEnable
																			VK_FALSE };	//alphaToOneEnable
	//VkPipelineDepthStencilStateCreateInfo depth_stencil_state_create_info;

	VkPipelineColorBlendAttachmentState color_blend_attachment_state = {	VK_FALSE, //VkBool32 blendEnable;
																			VK_BLEND_FACTOR_ONE, //VkBlendFactor srcColorBlendFactor;
																			VK_BLEND_FACTOR_ZERO, //VkBlendFactor dstColorBlendFactor;
																			VK_BLEND_OP_ADD, //VkBlendOp colorBlendOp;
																			VK_BLEND_FACTOR_ONE, //VkBlendFactor srcAlphaBlendFactor;
																			VK_BLEND_FACTOR_ZERO, //VkBlendFactor dstAlphaBlendFactor;
																			VK_BLEND_OP_ADD, //VkBlendOp alphaBlendOp;
																			VK_COLOR_COMPONENT_R_BIT |
																			VK_COLOR_COMPONENT_G_BIT |
																			VK_COLOR_COMPONENT_B_BIT |
																			VK_COLOR_COMPONENT_A_BIT }; // VkColorComponentFlags colorWriteMask };
	VkPipelineColorBlendStateCreateInfo color_blend_state_create_info = {	VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
																			NULL,
																			0, //VkPipelineColorBlendStateCreateFlags flags
																			VK_FALSE, //VkBool32 logicOpEnable
																			VK_LOGIC_OP_COPY, //VkLogicOp logicOp
																			1, //uint32_t attachmentCount
																			&color_blend_attachment_state,
																			{ 0.0f, 0.0f, 0.0f, 0.0f }};

	VkDynamicState dynamic_states[2] = {VK_DYNAMIC_STATE_VIEWPORT,
										VK_DYNAMIC_STATE_SCISSOR }; 
	VkPipelineDynamicStateCreateInfo dynamic_state_create_info  = {	VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
																	NULL,
																	0,	// VkPipelineDynamicStateCreateFlags
																	2,
																	&dynamic_states[0]};

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
	res = vkCreateDescriptorSetLayout(	device,
										&descriptor_set_layout_create_info,
										CVulkanMemory::GetAllocator(),
										&descriptor_set_layout);
	CATCH_VK_ERROR(res);
	if (VK_SUCCESS != res)
		return false;

	VkDescriptorSetAllocateInfo descriptor_allocate_info = {VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
															NULL,
															descriptor_pool,
															1, //descriptorSetCount;
															&descriptor_set_layout };
	res = vkAllocateDescriptorSets(device, &descriptor_allocate_info, &descriptor_set);
	//shaderStages->updateDescriptors(descriptor_set);


	const uint32_t setLayoutCount = 1;
	VkDescriptorSetLayout pSetLayouts[setLayoutCount] = { descriptor_set_layout };
	VkPipelineLayoutCreateInfo layout_create_info = {	VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
														NULL,
														0,	// flags
														setLayoutCount, // uint32_t setLayoutCount
														pSetLayouts, //VkDescriptorSetLayout   *pSetLayouts
														0, //uint32_t pushConstantRangeCount
														NULL }; //VkPushConstantRange     *pPushConstantRanges
	
	res = vkCreatePipelineLayout(device,
								&layout_create_info,
								CVulkanMemory::GetAllocator(),
								&layout);
	CATCH_VK_ERROR(res);
	if (VK_SUCCESS != res)
		return false;

	uint32_t subpass = 0;
	VkPipeline basePipelineHandle = VK_NULL_HANDLE;
	int32_t basePipelineIndex = -1;

	VkGraphicsPipelineCreateInfo pipeline_create_info = {VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
														NULL,	// const void *pNext 
														VK_PIPELINE_CREATE_ALLOW_DERIVATIVES_BIT, // VkPipelineCreateFlags
														stageCount,      // uint32_t stageCount 
														shader_stages,		// const VkPipelineShaderStageCreateInfo *
														&vertex_input_state_create_info,// const VkPipelineVertexInputStateCreateInfo *
														&input_assembly_state_create_info,// const VkPipelineInputAssemblyStateCreateInfo *
														NULL,// const VkPipelineTessellationStateCreateInfo *
														&viewport_state_create_info, // const VkPipelineViewportStateCreateInfo *
														&rasterization_state_create_info, // const VkPipelineRasterizationStateCreateInfo *
														&multisample_state_create_info, // const VkPipelineMultisampleStateCreateInfo *
														NULL,	// const VkPipelineDepthStencilStateCreateInfo *
														&color_blend_state_create_info, // const VkPipelineColorBlendStateCreateInfo *
														&dynamic_state_create_info, // const VkPipelineDynamicStateCreateInfo *
														layout,// VkPipelineLayout
														renderPass, // VkRenderPass
														subpass,// uint32_t subpass 
														basePipelineHandle, // VkPipeline
														basePipelineIndex }; //int32_t basePipelineIndex
	res = vkCreateGraphicsPipelines(device,
									VK_NULL_HANDLE,
									1, &pipeline_create_info,
									CVulkanMemory::GetAllocator(),
									&pipeline );
	delete[] shader_stages;

	CATCH_VK_ERROR(res);
	if (VK_SUCCESS != res)
		return false;
	
	return true;
}
