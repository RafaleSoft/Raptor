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
#if !defined(AFX_VERTEXSHADER_H__204F7213_B40B_4B6A_9BCA_828409871B68__INCLUDED_)
	#include "GLHierarchy/VertexShader.h"
#endif
#if !defined(AFX_VERTEXPROGRAM_H__F2D3BBC6_87A1_4695_B667_2B8C3C4CF022__INCLUDED_)
	#include "GLHierarchy/VertexProgram.h"
#endif
#if !defined(AFX_FRAGMENTSHADER_H__CC35D088_ADDF_4414_8CB6_C9D321F9D184__INCLUDED_)
	#include "GLHierarchy/FragmentShader.h"
#endif
#if !defined(AFX_FRAGMENTPROGRAM_H__DD0AD51D_3BFF_4C65_8099_BA7696D7BDDF__INCLUDED_)
	#include "GLHierarchy/FragmentProgram.h"
#endif
#if !defined(AFX_GEOMETRYSHADER_H__1981EA98_8F3C_4881_9429_A9ACA5B285D3__INCLUDED_)
	#include "GLHierarchy/GeometryShader.h"
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
		m_bUpdateVertexParameters(false),
		m_bUpdateFragmentParameters(false),
		m_bUpdateGeometryParameters(false)
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

	bool	m_bUpdateVertexParameters;
	bool	m_bUpdateFragmentParameters;
	bool	m_bUpdateGeometryParameters;
	CProgramParameters v;
	CProgramParameters f;
	CProgramParameters g;
	CShader	*m_pShader;

private:
	CShaderModifier *m_pModifier;
};

void CShaderWrapper::glRender()
{
	if (m_bUpdateVertexParameters)
	{
		if (m_pShader->hasOpenGLShader())
		{
			COpenGLShaderStage* stage = m_pShader->glGetOpenGLShader();
			if (stage->hasVertexShader())
				stage->glGetVertexShader()->setProgramParameters(v);
			if (stage->hasFragmentShader())
				stage->glGetFragmentShader()->setProgramParameters(f);
			if (stage->hasGeometryShader())
				stage->glGetGeometryShader()->setProgramParameters(g);
		}
		else if (m_pShader->hasOpenGLProgram())
		{
			COpenGLProgramStage* stage = m_pShader->glGetOpenGLProgram();
			if (stage->hasVertexProgram())
				stage->glGetVertexProgram()->setProgramParameters(v);
			if (stage->hasFragmentProgram())
				stage->glGetFragmentProgram()->setProgramParameters(f);
		}

		m_bUpdateVertexParameters = false;
		m_bUpdateFragmentParameters = false;
		m_bUpdateGeometryParameters = false;
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
	{
		m_pWrapper->m_bUpdateVertexParameters = true;
		m_pWrapper->m_bUpdateFragmentParameters = true;
		m_pWrapper->m_bUpdateGeometryParameters = true;
	}

	CModifier::deltaTime(dt);

	if (m_pWrapper != NULL)
	{
		updateVertexParameters(dt, m_pWrapper->v);
		updateFragmentParameters(dt, m_pWrapper->f);
		updateGeometryParameters(dt, m_pWrapper->g);
	}
}

void RAPTOR_FASTCALL CShaderModifier::updateVertexParameters(float dt, CProgramParameters &v)
{
	if (m_pWrapper != NULL)
	{
		m_pWrapper->m_bUpdateVertexParameters = false;
	}
}

void RAPTOR_FASTCALL CShaderModifier::updateFragmentParameters(float dt, CProgramParameters &v)
{
	if (m_pWrapper != NULL)
	{
		m_pWrapper->m_bUpdateFragmentParameters = false;
	}
}

void RAPTOR_FASTCALL CShaderModifier::updateGeometryParameters(float dt, CProgramParameters &v)
{
	if (m_pWrapper != NULL)
	{
		m_pWrapper->m_bUpdateGeometryParameters = false;
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

