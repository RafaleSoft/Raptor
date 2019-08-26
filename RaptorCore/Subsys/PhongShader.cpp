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
#if !defined(AFX_RAPTORGLEXTENSIONS_H__E5B5A1D9_60F8_4E20_B4E1_8E5A9CB7E0EB__INCLUDED_)
	#include "System/RaptorGLExtensions.h"
#endif
#if !defined(AFX_TEXTUREUNITSETUP_H__4A6ADC72_02E5_4F2A_931E_A736B6D6E0F0__INCLUDED_)
	#include "GLHierarchy/TextureUnitSetup.h"
#endif


RAPTOR_NAMESPACE

int CPhongShader::lightEnable = -1;
int CPhongShader::diffuseMap = -1;

CPhongShader::CPhongShader(void)
	:CShader("PHONG_SHADER")
{
}

CPhongShader::CPhongShader(const CPhongShader& shader)
	: CShader(shader)
{
}

CPhongShader::~CPhongShader(void)
{
}

CShader* CPhongShader::glClone(const std::string& newShaderName) const
{
	return new CPhongShader(*this);
}

void CPhongShader::glInit(void)
{
	glGetVertexShader("PPIXEL_PHONG_VTX_PROGRAM");
	glGetFragmentShader("PPIXEL_PHONG_TEX_PROGRAM");
	glCompileShader();
}

void CPhongShader::glRender(void)
{
	CShader::glRender();

#if defined(GL_ARB_shader_objects)
	const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();

	if ((lightEnable < 0) || (diffuseMap < 0))
	{
		GLhandleARB program = pExtensions->glGetHandleARB(GL_PROGRAM_OBJECT_ARB);
		lightEnable = pExtensions->glGetUniformLocationARB(program, "lightEnable");
		diffuseMap = pExtensions->glGetUniformLocationARB(program, "diffuseMap");
	}

	int *bLights = CLightAttributes::getLightOrder();
	if ((lightEnable >= 0) && (NULL != bLights))
		pExtensions->glUniform1ivARB(lightEnable,CLightAttributes::MAX_LIGHTS,bLights);

	if (diffuseMap >= 0)
		pExtensions->glUniform1iARB(diffuseMap,CTextureUnitSetup::IMAGE_UNIT_0);
#endif
}

