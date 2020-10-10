/***************************************************************************/
/*                                                                         */
/*  PhongShader.cpp                                                        */
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
#if !defined(AFX_OPENGLSHADERSTAGE_H__56B00FE3_E508_4FD6_9363_90E6E67446D9__INCLUDED_)
	#include "GLHierarchy/OpenGLShaderStage.h"
#endif
#if !defined(AFX_LIGHT_H__AA8BABD6_059A_4939_A4B6_A0A036E12E1E__INCLUDED_)
	#include "GLHierarchy/Light.h"
#endif


RAPTOR_NAMESPACE


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
	CPhongShader* phong = new CPhongShader(*this);
	if (!newShaderName.empty())
		phong->setName(newShaderName);

	if (hasOpenGLShader())
		phong->glInit();

	return phong;
}

typedef struct LightProduct_t
{
	GL_COORD_VERTEX position;
	GL_COORD_VERTEX attenuation;
	CColor::RGBA	ambient;
	CColor::RGBA	diffuse;
	CColor::RGBA	specular;
	float			shininess;
	float			reserved[3];
	bool			enable;
	float			reserved2[3];
} R_LightProduct;

typedef struct LightProducts_t
{
	R_LightProduct lights[5];
} R_LightProducts;

static R_LightProducts products;

void CPhongShader::glInit(void)
{
	COpenGLShaderStage *stage = glGetOpenGLShader("PHONG_SHADER_PROGRAM");

	stage->glGetVertexShader("PPIXEL_PHONG_VTX_PROGRAM");
	stage->glGetFragmentShader("PPIXEL_PHONG_TEX_PROGRAM");

	CProgramParameters params;
	params.addParameter("diffuseMap", CTextureUnitSetup::IMAGE_UNIT_0);

#if defined(GL_ARB_uniform_buffer_object)
	CProgramParameters::CParameter<R_LightProducts> material("LightProducts", products);
	material.locationType = GL_UNIFORM_BLOCK_BINDING_ARB;
	params.addParameter(material);
#endif

	stage->setProgramParameters(params);

	stage->glCompileShader();
}

__inline CColor::RGBA operator*(const CColor::RGBA& l_color, const CColor::RGBA& r_color)
{
	CColor::RGBA c(l_color.r * r_color.r,
		l_color.g * r_color.g,
		l_color.b * r_color.b,
		l_color.a * r_color.a);
	return c;
}


void CPhongShader::glRender(void)
{
	COpenGLShaderStage *stage = glGetOpenGLShader();
	CProgramParameters params;

	int numl = 0;
	CMaterial *M = getMaterial();
	CLight **olights = CLightAttributes::getOrderedLights();
	for (int i = 0; i < CLightAttributes::MAX_LIGHTS; i++)
	{
		CLight *pLight = olights[i];
		products.lights[i].enable = false;

		if (NULL != pLight)
		{
			R_LightProduct& lp = products.lights[numl++];
			lp.ambient = M->getAmbient() * pLight->getAmbient();
			lp.diffuse = M->getDiffuse() * pLight->getDiffuse();
			lp.specular = M->getSpecular() * pLight->getSpecular();
			lp.shininess = M->getShininess();
			lp.enable = true;
			const CGenericVector<float, 4> &p = pLight->getLightViewPosition();
			lp.position = GL_COORD_VERTEX(p.X(), p.Y(), p.Z(), p.H());
			lp.attenuation = pLight->getSpotParams();
		}
	}

#if defined(GL_ARB_uniform_buffer_object)
	CProgramParameters::CParameter<R_LightProducts> material("LightProducts", products);
	material.locationType = GL_UNIFORM_BLOCK_BINDING_ARB;
	params.addParameter(material);
#endif

	stage->updateProgramParameters(params);

	CShader::glRender();
}

