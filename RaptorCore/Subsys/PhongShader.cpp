// PhongShader.cpp: implementation of the CPhongShader class.
//
//////////////////////////////////////////////////////////////////////
#include "Subsys/CodeGeneration.h"

#if !defined(AFX_PHONGSHADER_H__C6904117_2A9C_42A8_A20B_A71A22A9C00B__INCLUDED_)
	#include "PhongShader.h"
#endif

#if !defined(AFX_LIGHTATTRIBUTES_H__B0A3AF95_90DC_4185_9747_B7F631DDB2BF__INCLUDED_)
	#include "LightAttributes.h"
#endif

#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
	#include "System/Raptor.h"
#endif
#if !defined(AFX_RAPTOREXTENSIONS_H__E5B5A1D9_60F8_4E20_B4E1_8E5A9CB7E0EB__INCLUDED_)
	#include "System/RaptorExtensions.h"
#endif
#if !defined(AFX_TEXTUREUNITSETUP_H__4A6ADC72_02E5_4F2A_931E_A736B6D6E0F0__INCLUDED_)
	#include "GLHierarchy/TextureUnitSetup.h"
#endif


RAPTOR_NAMESPACE

int CPhongShader::lightEnable = -1;
int CPhongShader::diffuseMap = -1;

CPhongShader::CPhongShader(void)
	:CShader("PHONG_SHADER"),
	m_p3LightsTextured(NULL)
{
}

CPhongShader::~CPhongShader(void)
{
	if (m_p3LightsTextured != NULL)
		m_p3LightsTextured->unregisterDestruction(this);
}

void CPhongShader::glInit(void)
{
	glGetVertexProgram("PPIXEL_PHONG_VTX_PROGRAM_0LIGHTS");
	glGetFragmentProgram("PPIXEL_PHONG_TEX_PROGRAM_0LIGHTS");
	glCompileShader();

	m_p3LightsTextured = new CShader("PHONG_SHADER_3LIGHTS");
	m_p3LightsTextured->glGetVertexProgram("PPIXEL_PHONG_VTX_PROGRAM");
	m_p3LightsTextured->glGetFragmentProgram("PPIXEL_PHONG_TEX_PROGRAM");
	m_p3LightsTextured->glCompileShader();
	m_p3LightsTextured->registerDestruction(this);
}

void CPhongShader::glStop(void)
{
	if (glIsEnabled(GL_LIGHTING) == GL_TRUE)
		m_p3LightsTextured->glStop();
	else
		CShader::glStop();
}

void CPhongShader::glRender(void)
{
#if defined(GL_ARB_shader_objects)
	const CRaptorExtensions *const pExtensions = Raptor::glGetExtensions();

	if (glIsEnabled(GL_LIGHTING) == GL_TRUE)
	{
		bool *pGLLights = CLightAttributes::getActiveGLLights();
		int bLights[CLightAttributes::MAX_LIGHTS] = {0,0,0,0,0,0,0,0};

		// TODO : find a faster way
		// when no lighting, only vertex processing is required.
		for (unsigned int i=0;i<CLightAttributes::MAX_LIGHTS;i++)
			bLights[i] = (pGLLights[i] ? 1 : 0);
				
		m_p3LightsTextured->glRender();

		if (lightEnable < 0)
		{
			GLhandleARB program = pExtensions->glGetHandleARB(GL_PROGRAM_OBJECT_ARB);
			lightEnable = pExtensions->glGetUniformLocationARB(program,"lightEnable");	
		}
		pExtensions->glUniform1ivARB(lightEnable,CLightAttributes::MAX_LIGHTS,bLights);
	}
	else
		CShader::glRender();

	if (diffuseMap < 0)
	{
		GLhandleARB program = pExtensions->glGetHandleARB(GL_PROGRAM_OBJECT_ARB);
		diffuseMap = pExtensions->glGetUniformLocationARB(program,"diffuseMap");
	}

	pExtensions->glUniform1iARB(diffuseMap,CTextureUnitSetup::IMAGE_UNIT_0);
#else
	CShader::glRender();
#endif
}

