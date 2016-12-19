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

	//!	Returns the number of successfully loaded shader.
	size_t getStageCount(void) const { return m_shaderStages.size(); };

	//!	Returns the shader number numShader, or NULL if out of bounds.
	CVulkanShader* getShader(size_t numShader) const;

	//!	Loads a shader stage.
	bool vkLoadShader(const std::string& filename);

	//!	Update uniform data
	bool vkSetData(void *src, uint64_t size);

	//bool updateDescriptors(VkDescriptorSet descriptorSet);

	//! Inherited from CPersistence
    DECLARE_IO
	DECLARE_CLASS_ID(CVulkanShaderStageClassID,"VulkanShaderStage",CPersistence)


private:
	//! Denied operators
	CVulkanShaderStage();
    CVulkanShaderStage& operator=(const CVulkanShaderStage& ) { return *this;};

	std::vector<CVulkanShader*>	m_shaderStages;
	unsigned char* uniforms;
};


RAPTOR_NAMESPACE_END

#endif // !defined(AFX_VULKANSHADERSTAGE_H__EF5769B8_470D_467F_9FDE_553142C81698__INCLUDED_)

