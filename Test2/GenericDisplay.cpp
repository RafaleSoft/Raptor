// GenericDisplay.cpp: implementation of the CGenericDisplay class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GenericDisplay.h"
#include "GLHierarchy/TextureUnitSetup.h"
#include "GLHierarchy/FragmentProgram_old.h"
#include "GLHierarchy/VertexProgram_old.h"
#include "GLHierarchy/IRenderingProperties.h"
#include "System/RaptorDisplay.h"


RAPTOR_HANDLE CGenericDisplay::reinitTMU;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGenericDisplay::CGenericDisplay()
:reinit(true)
{

}

CGenericDisplay::~CGenericDisplay()
{

}

void CGenericDisplay::ReInit()
{
	if (reinit)
	{
		CRaptorDisplay* pDisplay = CRaptorDisplay::GetCurrentDisplay();
		pDisplay->selectScene("ROOT_SCENE");
		IRenderingProperties &rp = pDisplay->getRenderingProperties();
		rp.setTexturing(IRenderingProperties::DISABLE);

		if (reinitTMU.handle() != 0)
			glCallList(reinitTMU.handle());

		//	When there is no view point,
		// we must reinitialize the modelview matrix.
		if (NULL == pDisplay->getViewPoint())
		{
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
		}

		glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_DEPTH_TEST);
		glDepthMask(GL_TRUE);

		glDisable(GL_STENCIL_TEST);

		reinit = false;
	}
}

void CGenericDisplay::Init()
{
	if (reinitTMU.handle() == 0)
	{
		CTextureUnitSetup initSetup;

        initSetup.enableImageUnit(CTextureUnitSetup::IMAGE_UNIT_0,true);
		initSetup.setDiffuseMap(NULL);
		initSetup.getTMUShader(CTextureUnitSetup::IMAGE_UNIT_0).shaderOperation = CGL_TEXTURE_GEN_COORD;
		initSetup.getTMUShader(CTextureUnitSetup::IMAGE_UNIT_0).genMode[0] = GL_NONE;
		initSetup.getTMUShader(CTextureUnitSetup::IMAGE_UNIT_0).genMode[1] = GL_NONE;
		initSetup.getTMUShader(CTextureUnitSetup::IMAGE_UNIT_0).genMode[2] = GL_NONE;
		initSetup.getTMUShader(CTextureUnitSetup::IMAGE_UNIT_0).genMode[3] = GL_NONE;

		initSetup.enableImageUnit(CTextureUnitSetup::IMAGE_UNIT_1,true);
		initSetup.setNormalMap(NULL);
		initSetup.getTMUShader(CTextureUnitSetup::IMAGE_UNIT_1).shaderOperation = CGL_TEXTURE_GEN_COORD;
		initSetup.getTMUShader(CTextureUnitSetup::IMAGE_UNIT_1).genMode[0] = GL_NONE;
		initSetup.getTMUShader(CTextureUnitSetup::IMAGE_UNIT_1).genMode[1] = GL_NONE;
		initSetup.getTMUShader(CTextureUnitSetup::IMAGE_UNIT_1).genMode[2] = GL_NONE;
		initSetup.getTMUShader(CTextureUnitSetup::IMAGE_UNIT_1).genMode[3] = GL_NONE;

		initSetup.enableImageUnit(CTextureUnitSetup::IMAGE_UNIT_2,true);
		initSetup.setLightMap(NULL);
		initSetup.enableImageUnit(CTextureUnitSetup::IMAGE_UNIT_3,true);
		initSetup.setEnvironmentMap(NULL);

		reinitTMU = initSetup.glBuildSetup();
	}
}