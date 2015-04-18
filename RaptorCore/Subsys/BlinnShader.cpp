// BlinnShader.cpp: implementation of the CBlinnShader class.
//
//////////////////////////////////////////////////////////////////////
#include "Subsys/CodeGeneration.h"



#if !defined(AFX_BLINNSHADER_H__7C69D948_B286_4840_8101_00E84647D812__INCLUDED_)
	#include "BlinnShader.h"
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

int CBlinnShader::lightEnable = -1;
int CBlinnShader::diffuseMap = -1;

CBlinnShader::CBlinnShader(void)
	:CShader("BLINN_SHADER")
{
}

CBlinnShader::~CBlinnShader(void)
{
}

void CBlinnShader::glInit()
{
	glGetVertexProgram("PPIXEL_BLINN_VTX_PROGRAM");
	glGetFragmentProgram("PPIXEL_BLINN_TEX_PROGRAM");
	glCompileShader();
}

void CBlinnShader::glRender(void)
{
#if defined(GL_ARB_shader_objects)
	bool *pGLLights = CLightAttributes::getActiveGLLights();
	int bLights[CLightAttributes::MAX_LIGHTS]  = {0,0,0,0,0,0,0,0};

	for (unsigned int i=0;i<CLightAttributes::MAX_LIGHTS;i++)
		bLights[i] = (pGLLights[i] ? 1 : 0);

	CShader::glRender();

	const CRaptorExtensions *const pExtensions = Raptor::glGetExtensions();

	if ((lightEnable < 0) || (diffuseMap < 0))
	{
		GLhandleARB program = pExtensions->glGetHandleARB(GL_PROGRAM_OBJECT_ARB);
		lightEnable = pExtensions->glGetUniformLocationARB(program,"lightEnable");
		diffuseMap = pExtensions->glGetUniformLocationARB(program,"diffuseMap");
	}

	pExtensions->glUniform1iARB(diffuseMap,CTextureUnitSetup::IMAGE_UNIT_0);
	pExtensions->glUniform1ivARB(lightEnable,CLightAttributes::MAX_LIGHTS,bLights);
#else
	CShader::glRender();
#endif
}


