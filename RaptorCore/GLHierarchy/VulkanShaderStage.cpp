// VulkanShaderStage.cpp: implementation of the CVulkanShaderStage class.
//
//////////////////////////////////////////////////////////////////////
#include "Subsys/CodeGeneration.h"


#if !defined(AFX_VULKANSHADERSTAGE_H__EF5769B8_470D_467F_9FDE_553142C81698__INCLUDED_)
	#include "VulkanShaderStage.h"
#endif
#if !defined(AFX_RAPTORVULKANDEVICE_H__2FDEDD40_444E_4CC2_96AA_CBF9E79C3ABE__INCLUDED_)
	#include "Subsys/Vulkan/VulkanDevice.h"
#endif
#if !defined(AFX_RAPTORERRORMANAGER_H__FA5A36CD_56BC_4AA1_A5F4_451734AD395E__INCLUDED_)
    #include "System/RaptorErrorManager.h"
#endif
#if !defined(AFX_RAPTORVULKANSHADER_H__C188550F_1D1C_4531_B0A0_727CE9FF9450__INCLUDED_)
	#include "Subsys/Vulkan/VulkanShader.h"
#endif
#if !defined(AFX_UNIFORMALLOCATOR_H__4DD62C99_E476_4FE5_AEE4_EEC71F7B0F38__INCLUDED_)
	#include "Subsys/UniformAllocator.h"
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

CVulkanShaderStage::CVulkanShaderStage(const std::string& name)
	:CShaderProgram(stageId, name), m_bValid(false),
	m_pShaderStages(NULL), uniforms(NULL)
{
}

CVulkanShaderStage::~CVulkanShaderStage(void)
{
	if (NULL != m_pShaderStages)
		delete m_pShaderStages;
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

bool CVulkanShaderStage::vkLoadShader(const std::string& filename)
{
	m_bValid = false;

	if (filename.empty())
		return false;

	if (NULL == m_pShaderStages)
	{
		CVulkanDevice* pDevice = CVulkanDevice::getCurrentDevice();
		if (NULL != pDevice)
		{
			CVulkanShader* pShader = pDevice->createShader();
			if (NULL != pShader)
				m_pShaderStages = pShader;
		}
	}

	if (NULL != m_pShaderStages)
		m_bValid = m_pShaderStages->loadShader(filename);
		
	return m_bValid;
}

bool CVulkanShaderStage::vkSetData(void *src, uint64_t size)
{
	CUniformAllocator*	pUAllocator = CUniformAllocator::GetInstance();

	if (NULL == uniforms)
		uniforms = pUAllocator->allocateUniforms(size);

	pUAllocator->glvkCopyPointer(uniforms, (unsigned char*)src, size);
	if (NULL != m_pShaderStages)
		m_pShaderStages->vkSetData((VkDeviceSize)uniforms, size);

	return true;
}

