/***************************************************************************/
/*                                                                         */
/*  ShaderModifier.cpp                                                     */
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

#if !defined(AFX_SHADERMODIFIER_H__5ED408B3_1FAE_4869_8A40_6ABF39D8DF1E__INCLUDED_)
	#include "ShaderModifier.h"
#endif
#if !defined(AFX_SHADER_H__4D405EC2_7151_465D_86B6_1CA99B906777__INCLUDED_)
	#include "GLHierarchy/Shader.h"
#endif
#if !defined(AFX_OPENGLPROGRAMSTAGE_H__0BCE3B42_6E10_4F50_BB27_1993345ADBCF__INCLUDED_)
	#include "GLHierarchy/OpenGLProgramStage.h"
#endif
#if !defined(AFX_OPENGLSHADERSTAGE_H__56B00FE3_E508_4FD6_9363_90E6E67446D9__INCLUDED_)
	#include "GLHierarchy/OpenGLShaderStage.h"
#endif


RAPTOR_NAMESPACE_BEGIN

//! Modifier geometry
class CShaderWrapper : public CShader
{
public:
	CShaderWrapper(CShader *shader, CShaderModifier *pModifier)
		:CShader("SHADER_WRAPPER"),
		m_pModifier(pModifier),
		m_pShader(NULL),
		m_bUpdateParameters(false)
	{
		if (shader != NULL)
		{
			m_pShader = shader;
			m_pShader->registerDestruction(this);
		}
	}

	virtual ~CShaderWrapper()
	{
		if (m_pShader != NULL)
			m_pShader->unregisterDestruction(this);
	}

	virtual void glRender();

	virtual void unLink(const CPersistence*)
	{
		m_pShader = NULL;
	}

	bool				m_bUpdateParameters;
	CProgramParameters	params;

	CShader	*m_pShader;

private:
	CShaderModifier *m_pModifier;
};

void CShaderWrapper::glRender()
{
	if (m_bUpdateParameters)
	{
		if (m_pShader->hasOpenGLShader())
		{
			COpenGLShaderStage* stage = m_pShader->glGetOpenGLShader();
			stage->setProgramParameters(params);
		}
		else if (m_pShader->hasOpenGLProgram())
		{
			COpenGLProgramStage* stage = m_pShader->glGetOpenGLProgram();
			stage->setProgramParameters(params);
		}

		m_bUpdateParameters = false;
	}

	m_pShader->glRender();
}


RAPTOR_NAMESPACE_END

RAPTOR_NAMESPACE

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CShaderModifier::CShaderModifier(TIME_FUNCTION_TYPE	timeFunction,
								float				timeArg0,
								float				timeArg1,
								float				timeArg2,
								float				timeArg3,
								USER_FUNCTION		pUserFunction,
								const CPersistence::CPersistenceClassID& id,
								const std::string& name)
	:CModifier(timeFunction, timeArg0, timeArg1, timeArg2, timeArg3, pUserFunction, id, name),
	m_pWrapper(NULL)
{
}

CShaderModifier::~CShaderModifier()
{
	if (m_pWrapper != NULL)
	{
		m_pWrapper->unregisterDestruction(this);
		delete m_pWrapper;
		m_pWrapper = NULL;
	}
}

void CShaderModifier::unLink(const CPersistence* object)
{
	if (object == static_cast<CPersistence*>(m_pWrapper))
		m_pWrapper = NULL;
}

CShader *CShaderModifier::getObject() const
{
	return m_pWrapper;
}

void RAPTOR_FASTCALL CShaderModifier::deltaTime(float dt)
{
	if (m_pWrapper != NULL)
		m_pWrapper->m_bUpdateParameters = true;

	CModifier::deltaTime(dt);

	if (m_pWrapper != NULL)
		updateShaderParameters(dt, m_pWrapper->params);
}

void RAPTOR_FASTCALL CShaderModifier::updateShaderParameters(float dt, CProgramParameters &v)
{
	if (m_pWrapper != NULL)
	{
		m_pWrapper->m_bUpdateParameters = false;
	}
}

void CShaderModifier::setShader(CShader *shader)
{
	if (m_pWrapper != NULL)
	{
		delete m_pWrapper;
		m_pWrapper = NULL;
	}

	if (shader != NULL)
	{
		m_pWrapper = new CShaderWrapper(shader, this);
		m_pWrapper->registerDestruction(this);
	}
}

