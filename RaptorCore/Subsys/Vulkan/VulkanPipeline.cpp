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
	pipeline(VK_NULL_HANDLE)
#endif
{
#if defined(VK_VERSION_1_0)
	vkCreateGraphicsPipelines = (PFN_vkCreateGraphicsPipelines)(vkGetDeviceProcAddr(device, "vkCreateGraphicsPipelines"));
	vkCreateComputePipelines = (PFN_vkCreateComputePipelines)(vkGetDeviceProcAddr(device, "vkCreateComputePipelines"));
	vkDestroyPipeline = (PFN_vkDestroyPipeline)(vkGetDeviceProcAddr(device, "vkDestroyPipeline"));
	vkCreateShaderModule = NULL;
	vkDestroyShaderModule = NULL;
	vkCreateDescriptorSetLayout = NULL;
	vkDestroyDescriptorSetLayout = NULL;
	vkGetImageSubresourceLayout = (PFN_vkGetImageSubresourceLayout)(vkGetDeviceProcAddr(device, "vkGetImageSubresourceLayout"));
	vkCreatePipelineCache = (PFN_vkCreatePipelineCache)(vkGetDeviceProcAddr(device, "vkCreatePipelineCache"));
	vkDestroyPipelineCache = (PFN_vkDestroyPipelineCache)(vkGetDeviceProcAddr(device, "vkDestroyPipelineCache"));
	vkGetPipelineCacheData = (PFN_vkGetPipelineCacheData)(vkGetDeviceProcAddr(device, "vkGetPipelineCacheData"));
	vkMergePipelineCaches = (PFN_vkMergePipelineCaches)(vkGetDeviceProcAddr(device, "vkMergePipelineCaches"));
	vkCreateDescriptorPool = NULL;
	vkDestroyDescriptorPool = NULL;
	vkResetDescriptorPool = NULL;
	vkAllocateDescriptorSets = NULL;
	vkFreeDescriptorSets = NULL;
	vkUpdateDescriptorSets = NULL;
#endif
}

CVulkanPipeline::~CVulkanPipeline(void)
{
}


bool CVulkanPipeline::destroyPipeline()
{
	if (VK_NULL_HANDLE == device)
		return false;

	if (VK_NULL_HANDLE == pipeline)
		return false;
	vkDestroyPipeline(device, pipeline, CVulkanMemory::GetAllocator());

	return true;
}


bool CVulkanPipeline::initPipeline(const CVulkanShaderStage& shaderStages)
{
	VkResult res = VK_NOT_READY;

	//VkPipelineCache pipelineCache;

	CVulkanShader* pShaderStage = shaderStages.getShader();
	if (NULL == pShaderStage)
		return false;

	size_t stageCount = pShaderStage->getStageCount();
	VkPipelineShaderStageCreateInfo *shader_stages = new VkPipelineShaderStageCreateInfo[stageCount];
	for (size_t i=0;i<stageCount;i++)
		shader_stages[i] = pShaderStage->getShaderStage(i);
	
	VkVertexInputBindingDescription vertex_binding_description_info = {	0,	// binding number
																		4*sizeof(float),	//stride
																		VK_VERTEX_INPUT_RATE_VERTEX };
	VkVertexInputBindingDescription colors_binding_description_info = { 1,	// binding number
																		4 * sizeof(float),	//stride
																		VK_VERTEX_INPUT_RATE_VERTEX };
	VkVertexInputBindingDescription texcoords_binding_description_info = {	2,	// binding number
																			2 * sizeof(float),	//stride
																			VK_VERTEX_INPUT_RATE_VERTEX };
	VkVertexInputBindingDescription bindings[3] = { vertex_binding_description_info, colors_binding_description_info, texcoords_binding_description_info };

	VkVertexInputAttributeDescription vertex_input_attribute_info[3] = { {	0,
																			vertex_binding_description_info.binding,
																			VK_FORMAT_R32G32B32A32_SFLOAT,
																			0 },
																		  { 1,
																			colors_binding_description_info.binding,
																			VK_FORMAT_R32G32B32A32_SFLOAT,
																			0 },
																		  { 2,
																			texcoords_binding_description_info.binding,
																			VK_FORMAT_R32G32_SFLOAT,
																			0 } };


	VkPipelineVertexInputStateCreateInfo vertex_input_state_create_info = {	VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
																			NULL,
																			0,	//VkPipelineVertexInputStateCreateFlags
																			3, //vertexBindingDescriptionCount
																			&bindings[0], //VkVertexInputBindingDescription
																			3,	//vertexAttributeDescriptionCount
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
																		NULL, //&viewport, NULL == Dynamic
																		1,	//scissorCount;
																		NULL }; //&scissor, NULL == Dynamic

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
	uint32_t subpass = 0;
	VkPipeline basePipelineHandle = VK_NULL_HANDLE;
	int32_t basePipelineIndex = -1;
	VkPipelineLayout layout = shaderStages.getPipelineLayout();

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
