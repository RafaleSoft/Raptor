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
	//PFN_vkCreateShaderModule CVulkanShader::vkCreateShaderModule = VK_NULL_HANDLE;
	//PFN_vkDestroyShaderModule CVulkanShader::vkDestroyShaderModule = VK_NULL_HANDLE;
#endif

CVulkanShader::CVulkanShader(VkDevice d, PFN_vkGetDeviceProcAddr vkGetDeviceProcAddr)
#if defined(VK_VERSION_1_0)
	: device(d),
	shader_module(VK_NULL_HANDLE),
	shader_stage(VK_SHADER_STAGE_ALL)
#endif
{
	vkCreateShaderModule = (PFN_vkCreateShaderModule)(vkGetDeviceProcAddr(device, "vkCreateShaderModule"));
	vkDestroyShaderModule = (PFN_vkDestroyShaderModule)(vkGetDeviceProcAddr(device, "vkDestroyShaderModule"));
	vkCreateDescriptorPool = (PFN_vkCreateDescriptorPool)(vkGetDeviceProcAddr(device, "vkCreateDescriptorPool"));
	vkDestroyDescriptorPool = (PFN_vkDestroyDescriptorPool)(vkGetDeviceProcAddr(device, "vkDestroyDescriptorPool"));
}

CVulkanShader::~CVulkanShader(void)
{
#if defined(VK_VERSION_1_0)
	if (VK_NULL_HANDLE != shader_module)
		vkDestroyShaderModule(device,shader_module,NULL);
#endif
}

CVulkanShader* CVulkanShader::vkClone(void) const
{
	return new CVulkanShader(*this);
}

VkPipelineShaderStageCreateInfo CVulkanShader::getShaderStage() const
{
	VkPipelineShaderStageCreateInfo shaderStage = { VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
													NULL,
													0, //VkPipelineShaderStageCreateFlags               
													shader_stage,
													shader_module,
													"main",
													NULL };	// VkSpecializationInfo
	return shaderStage;
}


CVulkanShader::CVulkanShader(const CVulkanShader& shader)
{
#if defined(VK_VERSION_1_0)
	device = shader.device;
	shader_module = VK_NULL_HANDLE;
	shader_stage = VK_SHADER_STAGE_ALL;
#endif
}

bool CVulkanShader::loadShader(const std::string &filename)
{
	if (filename.empty())
		return false;

	//std::string::size_type pos = filename.find(".spv",filename.length()-4);
	//if (std::string::npos == pos)
	//{
		// try to compile SPIR-V ?
		//return false;
	//}
#if defined(VK_VERSION_1_0)
	std::string fname = filename;
	if ((std::string::npos != filename.find(".frag",filename.length()-5)) ||
		(std::string::npos != filename.find("frag.spv",filename.length()-8)))
	{
		shader_stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	}
	else if ((std::string::npos != filename.find(".vert",filename.length()-5)) ||
			(std::string::npos != filename.find("vert.spv",filename.length()-8)))
	{
		shader_stage = VK_SHADER_STAGE_VERTEX_BIT;
	}

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
		CRaptorErrorManager *pErrMgr = Raptor::GetErrorManager();


		VkShaderModuleCreateInfo shader_module_create_info = {	VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO, // VkStructureType
																NULL,                                        // const void
																0,                                           // VkShaderModuleCreateFlags
																code_size,                                   // size_t
																code										 // const uint32_t*
															  }; 

		res = vkCreateShaderModule( device, &shader_module_create_info, NULL, &shader_module );
		delete [] code;
		if(VK_SUCCESS != res)
		{
			pErrMgr->vkGetError(res,__FILE__,__LINE__);
			return false;
		}
		return true;
	}
	else
		return false;
#else
	return false;
#endif
}
