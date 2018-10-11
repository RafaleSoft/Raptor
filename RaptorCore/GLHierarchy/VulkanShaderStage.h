// VulkanShaderStage.h: interface for the CVulkanShaderStage class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VULKANSHADERSTAGE_H__EF5769B8_470D_467F_9FDE_553142C81698__INCLUDED_)
#define AFX_VULKANSHADERSTAGE_H__EF5769B8_470D_467F_9FDE_553142C81698__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Subsys/CodeGeneration.h"

#if !defined(AFX_SHADERPROGRAM_H__936BEC73_3903_46CE_86C9_9CA0005B31F5__INCLUDED_)
	#include "ShaderProgram.h"
#endif


RAPTOR_NAMESPACE_BEGIN

class CVulkanShader;
class CVulkanCommandBuffer;

class RAPTOR_API CVulkanShaderStage : public CShaderProgram
{
public:
	CVulkanShaderStage(const std::string& name);

	virtual ~CVulkanShaderStage(void);

	//! Implements base class
	virtual bool glLoadProgram(const std::string &program)
	{
		return NULL;
	};

	//! Implements base class
	virtual void glRender(void)
	{
	};

	//! Apply parameters and bind descriptor sets for shader rendering
	void vkRender(CVulkanCommandBuffer &commandBuffer, CTextureUnitSetup *tmu_setup = NULL);
	
	//! Compute descriptor sets layout and create the pipeline layout
	VkPipelineLayout getPipelineLayout() const;

	//! Implements base class
	virtual void glStop(void) { };

	//! Implements base class
	virtual bool glGetProgramStatus(void)
	{
		return m_bValid;
	};

	virtual void glProgramParameter(unsigned int numParam,
									const GL_COORD_VERTEX &v) const { };

	virtual void glProgramParameter(unsigned int numParam,
									const CColor::RGBA &v) const { };

	//! Provide gl-like shader parameters from RaptorCore.
	//!	Attention : parameter shall be copied before next call because return value is reused.
	CProgramParameters::CParameterBase& getDefaultParameter(const std::string& parameter_name, int locationIndex);


	//!	@see CShaderProgram
	virtual void setProgramParameters(const CProgramParameters &v);

	//!	Clone the whole shader stage.
	CVulkanShaderStage* vkClone(void) const;

	//!	Returns the shader number numShader, or NULL if out of bounds.
	CVulkanShader* getShader(void) const
	{
		return m_pShaderStages;
	};

	//!	Loads a shader stage.
	bool vkLoadShader(const std::string& filename);


	//! Inherited from CPersistence
    DECLARE_IO
	DECLARE_CLASS_ID(CVulkanShaderStageClassID, "VulkanShaderStage", CShaderProgram)


private:
	//! Denied operators
	CVulkanShaderStage();
    CVulkanShaderStage& operator=(const CVulkanShaderStage& ) { return *this;};

	//!	Identify predefined OpenGL shader names variables
	bool IsPredefinedGLVariable(const std::string& name);


	//!	Vulkan shader modules
	CVulkanShader*	m_pShaderStages;

	//!	Last default parameter
	CProgramParameters::CParameterBase* m_param;

	//!	Uniform buffer
	unsigned char*	uniforms;
	uint64_t		uniforms_size;
};


RAPTOR_NAMESPACE_END

#endif // !defined(AFX_VULKANSHADERSTAGE_H__EF5769B8_470D_467F_9FDE_553142C81698__INCLUDED_)

