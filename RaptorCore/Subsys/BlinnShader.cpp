/***************************************************************************/
/*                                                                         */
/*  BlinnShader.cpp                                                        */
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

#if !defined(AFX_BLINNSHADER_H__7C69D948_B286_4840_8101_00E84647D812__INCLUDED_)
	#include "BlinnShader.h"
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
#if !defined(AFX_OPENGLSHADERSTAGE_H__56B00FE3_E508_4FD6_9363_90E6E67446D9__INCLUDED_)
	#include "GLHierarchy/OpenGLShaderStage.h"
#endif
#if !defined(AFX_LIGHT_H__AA8BABD6_059A_4939_A4B6_A0A036E12E1E__INCLUDED_)
	#include "GLHierarchy/Light.h"
#endif

RAPTOR_NAMESPACE


CBlinnShader::CBlinnShader(void)
	:CShader("BLINN_SHADER")
{
}

CBlinnShader::CBlinnShader(const CBlinnShader& shader)
	: CShader(shader)
{
}

CShader* CBlinnShader::glClone(const std::string& newShaderName) const
{
	CBlinnShader* blinn = new CBlinnShader(*this);
	if (!newShaderName.empty())
		blinn->setName(newShaderName);

	if (hasOpenGLShader())
		blinn->glInit();

	return blinn;
}

CBlinnShader::~CBlinnShader(void)
{
}

static CLight::R_LightProducts products;

void CBlinnShader::glInit()
{
	COpenGLShaderStage *stage = glGetOpenGLShader("BLINN_SHADER_PROGRAM");

	stage->glGetVertexShader("PPIXEL_BLINN_VTX_PROGRAM");
	stage->glGetFragmentShader("PPIXEL_BLINN_TEX_PROGRAM");

	CProgramParameters params;
	params.addParameter("diffuseMap", CTextureUnitSetup::IMAGE_UNIT_0);

#if defined(GL_ARB_uniform_buffer_object)
	CProgramParameters::CParameter<CLight::R_LightProducts> material("LightProducts", products);
	material.locationType = GL_UNIFORM_BLOCK_BINDING_ARB;
	params.addParameter(material);
#endif

	stage->setProgramParameters(params);

	stage->glCompileShader();
}

void CBlinnShader::glRender(void)
{
	COpenGLShaderStage *stage = glGetOpenGLShader();
	CProgramParameters params;

	int numl = 0;
	CMaterial *M = getMaterial();
	CLight **olights = CLightAttributes::getOrderedLights();
	for (int i = 0; (i < CLightAttributes::MAX_LIGHTS) && (numl < 5); i++)
	{
		CLight *pLight = olights[i];
		products.lights[min(i,4)].enable = false;

		if (NULL != pLight)
		{
			CLight::R_LightProduct& lp = products.lights[numl++];
			lp.ambient = M->getAmbient() * pLight->getAmbient();
			lp.diffuse = M->getDiffuse() * pLight->getDiffuse();
			lp.specular = M->getSpecular() * pLight->getSpecular();
			lp.shininess = M->getShininess();
			lp.enable = true;
			const CGenericVector<float,4> &p = pLight->getLightViewPosition();
			lp.position = GL_COORD_VERTEX(p.X(), p.Y(), p.Z(), p.H());
			lp.attenuation = pLight->getSpotParams();
		}
	}
	products.scene_ambient = CShader::getAmbient();

#if defined(GL_ARB_uniform_buffer_object)
	CProgramParameters::CParameter<CLight::R_LightProducts> material("LightProducts", products);
	material.locationType = GL_UNIFORM_BLOCK_BINDING_ARB;
	params.addParameter(material);
#endif

	stage->updateProgramParameters(params);

	CShader::glRender();
}


