/***************************************************************************/
/*                                                                         */
/*  LightShader.cpp                                                        */
/*                                                                         */
/*    Raptor OpenGL & Vulkan realtime 3D Engine SDK.                       */
/*                                                                         */
/*  Copyright 1998-2021 by                                                 */
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


#if !defined(AFX_LIGHTSHADER_H__0DD43C10_10BE_4516_80CF_83D1DC8B6312__INCLUDED_)
	#include "LightShader.h"
#endif
#if !defined(AFX_SHADERBLOC_H__56C73DCA_292E_4722_8881_82DC1BF53EA5__INCLUDED_)
	#include "GLHierarchy/ShaderBloc.h"
#endif
#if !defined(AFX_LIGHT_H__AA8BABD6_059A_4939_A4B6_A0A036E12E1E__INCLUDED_)
	#include "GLHierarchy/Light.h"
#endif
#if !defined(AFX_SHADER_H__4D405EC2_7151_465D_86B6_1CA99B906777__INCLUDED_)
	#include "GLHierarchy/Shader.h"
#endif
#if !defined(AFX_LIGHTATTRIBUTES_H__B0A3AF95_90DC_4185_9747_B7F631DDB2BF__INCLUDED_)
	#include "LightAttributes.h"
#endif


RAPTOR_NAMESPACE


CLightShader::CLightShader(void)
{
}

CLightShader::~CLightShader(void)
{
}

void CLightShader::glRenderShaderBloc(CShaderBloc& bloc, const CMaterial &material)
{
	if (!bloc.isExternal())
	{
		CLight::R_LightProducts products;
		CLight **olights = CLightAttributes::getOrderedLights();
		
		for (int i = 0, numl = 0; (i < CLightAttributes::MAX_LIGHTS) && (numl < 5); i++)
		{
			products.lights[i].enable = shader_false;
			CLight *pLight = olights[i];

			if (NULL != pLight)
			{
				CLight::R_LightProduct& lp = products.lights[numl++];
				lp = pLight->computeLightProduct(material);
			}
		}
		products.scene_ambient = CShader::getAmbient();
		products.shininess = material.getShininess();

		bloc.glvkUpdateBloc((uint8_t*)&products);
	}
}


