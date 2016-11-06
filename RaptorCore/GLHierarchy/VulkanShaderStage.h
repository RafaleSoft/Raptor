// VulkanShaderStage.h: interface for the CVulkanShaderStage class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VULKANSHADERSTAGE_H__EF5769B8_470D_467F_9FDE_553142C81698__INCLUDED_)
#define AFX_VULKANSHADERSTAGE_H__EF5769B8_470D_467F_9FDE_553142C81698__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Subsys/CodeGeneration.h"

#if !defined(AFX_PERSISTENCE_H__5561BA28_831B_11D3_9142_EEB51CEBBDB0__INCLUDED_)
	#include "Persistence.h"
#endif


RAPTOR_NAMESPACE_BEGIN

class CVulkanShader;

class RAPTOR_API CVulkanShaderStage : public CPersistence
{
public:
	CVulkanShaderStage(const std::string& name);

	virtual ~CVulkanShaderStage(void);

	//!	Clone the whole shader stage.
	CVulkanShaderStage* vkClone(void) const;

	size_t getStageCount(void) const { return m_shaderStages.size(); };


	//! Inherited from CPersistence
    DECLARE_IO
	DECLARE_CLASS_ID(CVulkanShaderStageClassID,"VulkanShaderStage",CPersistence)


private:
	//! Denied operators
	CVulkanShaderStage();
    CVulkanShaderStage& operator=(const CVulkanShaderStage& ) { return *this;};

	std::vector<CVulkanShader*>	m_shaderStages;
};


RAPTOR_NAMESPACE_END

#endif // !defined(AFX_VULKANSHADERSTAGE_H__EF5769B8_470D_467F_9FDE_553142C81698__INCLUDED_)

