/***************************************************************************/
/*                                                                         */
/*  VulkanShaderStage.cpp                                                  */
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
#if !defined(AFX_VULKANTEXTUREOBJECT_H__5E3E26C2_441F_4051_986F_2207AF0B3F6D__INCLUDED_)
	#include "Subsys/Vulkan/VulkanTextureObject.h"
#endif
#if !defined(AFX_UNIFORMALLOCATOR_H__4DD62C99_E476_4FE5_AEE4_EEC71F7B0F38__INCLUDED_)
	#include "Subsys/UniformAllocator.h"
#endif
#if !defined(AFX_VULKANVIEWPOINT_H__08D29395_9883_45F8_AE51_5174BD6BC19B__INCLUDED_)
	#include "Subsys/Vulkan/VulkanViewPoint.h"
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
	:CShaderProgram(stageId, name), m_param(NULL),
	m_pShaderStages(NULL), uniforms(NULL), uniforms_size(0)
{
}

CVulkanShaderStage::~CVulkanShaderStage(void)
{
	if (NULL != m_param)
		delete m_param;
	if (NULL != m_pShaderStages)
		delete m_pShaderStages;
	if (NULL != uniforms)
	{
		CUniformAllocator*	pUAllocator = CUniformAllocator::GetInstance();
		pUAllocator->releaseUniforms(uniforms);
	}
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
		const CVulkanDevice& rDevice = CVulkanDevice::getCurrentDevice();
		CVulkanShader* pShader = rDevice.createShader();
		if (NULL != pShader)
			m_pShaderStages = pShader;
	}

	if (NULL != m_pShaderStages)
		m_bValid = m_pShaderStages->loadShader(filename);
		
	return m_bValid;
}


void CVulkanShaderStage::setProgramParameters(const CProgramParameters &v)
{
	CShaderProgram::setProgramParameters(v);

	if ((m_bApplyParameters) && (NULL != m_pShaderStages))
	{
		CUniformAllocator*	pUAllocator = CUniformAllocator::GetInstance();

		if (NULL == uniforms)
		{
			uint64_t size = 0;
			for (unsigned int idx = 0; idx < m_parameters.getNbParameters(); idx++)
			{
				CProgramParameters::CParameterBase& param_value = m_parameters[idx];
				CTextureUnitSetup::TEXTURE_IMAGE_UNIT sampler = CTextureUnitSetup::IMAGE_UNIT_0;
				if (param_value.isA(sampler))
					param_value.locationType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
				else if (!IsPredefinedGLVariable(param_value.name()))
				{
					param_value.locationType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
					size += param_value.size();
				}
			}
			uniforms = pUAllocator->allocateUniforms(size);
		}

		// TODO : handle the case where parameter sets have different size: release parameters + reallocate.
		// TODO : optimize : use a temp buffer and make a single CopyPointer call.
		uint64_t totalsize = 0;
		for (unsigned int idx = 0; idx < m_parameters.getNbParameters(); idx++)
		{
			const CProgramParameters::CParameterBase& param_value = m_parameters[idx];
			if (VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER == (VkDescriptorType)param_value.locationType)
			{
				uint64_t sz = param_value.size();
				const void *addr = param_value.addr();

				pUAllocator->glvkCopyPointer(uniforms + totalsize, (unsigned char*)addr, sz);
				totalsize += sz;
			}
		}
		uniforms_size = totalsize;
	}
}

VkPipelineLayout CVulkanShaderStage::getPipelineLayout() const
{
	VkPipelineLayout layout = 0;
	if (NULL != m_pShaderStages)
	{
		// TODO: assert locationType is initialised
		layout = m_pShaderStages->createPipelineLayout(m_parameters);
	}

	return layout;
}

void CVulkanShaderStage::vkRender(CVulkanCommandBuffer &commandBuffer,
								  CTextureUnitSetup *tmu_setup)
{
	if (NULL != m_pShaderStages)
	{
		if (m_bApplyParameters)
		{
			for (unsigned int idx = 0; idx < m_parameters.getNbParameters(); idx++)
			{
				const CProgramParameters::CParameterBase& param_value = m_parameters[idx];
				if ((VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER == param_value.locationType) ||
					(VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE == param_value.locationType))
				{
					CTextureUnitSetup::TEXTURE_IMAGE_UNIT unit;
					unit = ((const CProgramParameters::CParameter<CTextureUnitSetup::TEXTURE_IMAGE_UNIT>&)param_value).p;
					
				}
			}
			m_bApplyParameters = false;
		}

		//!	Does this cost a lot of CPU ?
		const CVulkanDevice& rDevice = CVulkanDevice::getCurrentDevice();
		VkBuffer uniformBuffer = rDevice.getMemory()->getLockedBuffer(IDeviceMemoryManager::IBufferObject::UNIFORM_BUFFER);
		
		//! TODO : Set this only once ? i.e only if m_bApplyParameters ?
		VkDescriptorBufferInfo bufferInfo = { uniformBuffer, (VkDeviceSize)uniforms, uniforms_size };
		CVulkanTextureObject* t = tmu_setup->getDiffuseMap()->getVulkanTextureObject();
		VkDescriptorImageInfo imageInfo = t->getCombinedImageSampler();

		m_pShaderStages->vkRender(commandBuffer, bufferInfo, imageInfo);
	}
}


CProgramParameters::CParameterBase& CVulkanShaderStage::getDefaultParameter(const std::string& parameter_name, int locationIndex)
{
	if (NULL != m_param)
		delete m_param;

	if (IsPredefinedGLVariable(parameter_name))
	{
		CVulkanViewPoint::Transform_t t;
		m_param = new CProgramParameters::CParameter<CVulkanViewPoint::Transform_t>(t);
		m_param->locationIndex = locationIndex;
	}

	return *m_param;
}

bool CVulkanShaderStage::IsPredefinedGLVariable(const std::string& name)
{
	return ((name == "gl_ModelViewMatrix") ||
			(name == "gl_ProjectionMatrix") ||
			(name == "gl_NormalMatrix"));
}