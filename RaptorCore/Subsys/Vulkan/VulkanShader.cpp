// VulkanDevice.cpp: implementation of the CVulkanPipeline class.
//
//////////////////////////////////////////////////////////////////////
#include "Subsys\CodeGeneration.h"

#if !defined(AFX_RAPTORVULKANSHADER_H__C188550F_1D1C_4531_B0A0_727CE9FF9450__INCLUDED_)
	#include "Subsys/Vulkan/VulkanShader.h"
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

CVulkanShader::CVulkanShader(VkDevice d)
#if defined(VK_VERSION_1_0)
	:device(d),shader_module(VK_NULL_HANDLE),shader_stage(VK_SHADER_STAGE_ALL)
#endif
{
#if defined(VK_VERSION_1_0)
	vkCreateShaderModule = NULL;
#endif
}

CVulkanShader::~CVulkanShader(void)
{
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
	if (io->getStatus() == CRaptorIO::IO_OK)
	{
		size_t code_size = io->getSize();
		uint32_t *code = new uint32_t[code_size/4];
		io->read(code,code_size);
		
		VkResult res = VK_NOT_READY;
		CRaptorErrorManager *pErrMgr = Raptor::GetErrorManager();


		VkShaderModuleCreateInfo shader_module_create_info = {	VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO, // VkStructureType
																NULL,                                        // const void
																0,                                           // VkShaderModuleCreateFlags
																code_size,                                   // size_t
																code										 // const uint32_t*
															  }; 

		res = vkCreateShaderModule( device, &shader_module_create_info, NULL, &shader_module );
		if(VK_SUCCESS != res)
		{
			pErrMgr->vkGetError(res,__FILE__,__LINE__);
			return false;
		}
		return true;
	}
	else
		return false;
}
