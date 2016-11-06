// VulkanShaderStage.cpp: implementation of the CVulkanShaderStage class.
//
//////////////////////////////////////////////////////////////////////
#include "Subsys/CodeGeneration.h"


#if !defined(AFX_VULKANSHADERSTAGE_H__EF5769B8_470D_467F_9FDE_553142C81698__INCLUDED_)
	#include "VulkanShaderStage.h"
#endif
#if !defined(AFX_RAPTORERRORMANAGER_H__FA5A36CD_56BC_4AA1_A5F4_451734AD395E__INCLUDED_)
    #include "System/RaptorErrorManager.h"
#endif
#if !defined(AFX_RAPTORVULKANSHADER_H__C188550F_1D1C_4531_B0A0_727CE9FF9450__INCLUDED_)
	#include "Subsys/Vulkan/VulkanShader.h"
#endif
#if !defined(AFX_RAPTORIO_H__87D52C27_9117_4675_95DC_6AD2CCD2E78D__INCLUDED_)
	#include "System/RaptorIO.h"
#endif


//////////////////////////////////////////////////////////////////////
// Static data
//////////////////////////////////////////////////////////////////////
RAPTOR_NAMESPACE_BEGIN

static CVulkanShaderStage::CVulkanShaderStageClassID stageId;
const CPersistence::CPersistenceClassID& CVulkanShaderStage::CVulkanShaderStageClassID::GetClassId(void)
{
	return stageId;
}

RAPTOR_NAMESPACE_END


RAPTOR_NAMESPACE

CVulkanShaderStage::CVulkanShaderStage(const std::string& name):
	CPersistence(stageId,name)
{
}

CVulkanShaderStage::~CVulkanShaderStage(void)
{
}

CVulkanShaderStage* CVulkanShaderStage::vkClone(void) const
{
	return NULL;
}


bool CVulkanShaderStage::exportObject(CRaptorIO& o)
{
	return true;
}

bool CVulkanShaderStage::importObject(CRaptorIO& i)
{
	return true;
}


