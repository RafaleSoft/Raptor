/***************************************************************************/
/*                                                                         */
/*  VulkanShaderStage.h                                                    */
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
	virtual bool glLoadProgram(const std::string &program);

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
	virtual bool glGetProgramStatus(void) const
	{
		return m_bValid;
	};

	//! Implements base class
	virtual std::string glGetProgramString(void) const { return ""; }

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
    CVulkanShaderStage& operator=(const CVulkanShaderStage& );

	//!	Identify predefined OpenGL shader names variables
	bool IsPredefinedGLVariable(const std::string& name);

	//!	Shader uniform blocks.
	typedef struct uniform_bloc_t
	{
		uint8_t * buffer;
		uint64_t	size;
		uint64_t	offset;
		size_t		parameter;
		bool		external;
	} uniform_bloc;
	std::vector<uniform_bloc> m_uniforms;

	//!	Vulkan shader modules
	CVulkanShader*	m_pShaderStages;

	//!	Last default parameter
	CProgramParameters::CParameterBase* m_param;

	//!	Uniform buffer
	VkDescriptorBufferInfo m_bufferInfo;
};


RAPTOR_NAMESPACE_END

#endif // !defined(AFX_VULKANSHADERSTAGE_H__EF5769B8_470D_467F_9FDE_553142C81698__INCLUDED_)

