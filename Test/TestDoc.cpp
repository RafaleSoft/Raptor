/***************************************************************************/
/*                                                                         */
/*  TestDoc.cpp                                                            */
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



#include "StdAfx.h"
#include <sstream>

#include "TestDoc.h"
#include "ColorController.h"

#include "System/RaptorDisplay.h"
#include "GLHierarchy/ShadedGeometry.h"
#include "GLHierarchy/Object3DInstance.h"
#include "GLHierarchy/TextureFactory.h"
#include "GLHierarchy/TextureFactoryConfig.h"
#include "GLHierarchy/ITextureObject.h"
#include "GLHierarchy/Shader.h"
#include "Engine/ViewModifier.h"
#include "Engine/3DScene.h"
#include "Engine/Animator.h"
#include "System/Raptor.h"
#include "GLHierarchy/Material.h"
#include "GLHierarchy/Light.h"
#include "GLHierarchy/IRenderingProperties.h"
#include "GLHierarchy/3DSet.h"
#include "System/RaptorConsole.h"

#include "ToolBox/RaptorToolBox.h"
#include "ToolBox/BasicObjects.h"

#include "ToolBox/Filters/ColorControlFilter.h"
#include "ToolBox/Filters/HDRFilter.h"
#include "ToolBox/Filters/MagnifierFilter.h"
#include "ToolBox/Filters/MBFilter.h"
#include "ToolBox/Filters/BlurFilter.h"
#include "ToolBox/Filters/DOFFilter.h"


CTestDoc::CTestDoc(const RAPTOR_HANDLE& wnd,CRaptorDisplay *pDisplay)
{
	m_pSG = NULL;
    m_pDisplay = pDisplay;
    m_wnd = wnd;
    hdr = NULL;
	bwf = NULL;
	mbf = NULL;
}

CTestDoc::~CTestDoc()
{
}


class Foreground : public CBasicObjects::CRectangle
{
public:
	Foreground(CMagnifierFilter *mf):fgMag(mf),status(!mf->isEnabled())
	{
		CTextureFactory &fct = CTextureFactory::getDefaultFactory();
		ITextureObject *T = fct.glCreateTexture(ITextureObject::CGL_COLOR24_ALPHA);
		fct.glLoadTexture(T,"lrock049.jpg");

		fgMag->setColorInput(T);
		ITextureObject *output = fgMag->glCreateColorOutput();
		fgMag->glInitFilter();

		CShader *s = getShader();
		CTextureUnitSetup *tmu = s->glGetTextureUnitsSetup();
		tmu->setDiffuseMap(output);
	};

	virtual ~Foreground() {};

	virtual void glRender()
	{
		if (status != fgMag->isEnabled())
		{
			status = !status;
			glRenderFilter();
		}
		CShadedGeometry::glRender();
	};

private:
	void glRenderFilter(void)
	{
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();

		glOrtho(-1,1,-1,1,1,100);
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();

		fgMag->glRender();

		glPopMatrix();
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
	}

	bool status;
	CMagnifierFilter *fgMag;
};

/////////////////////////////////////////////////////////////////////////////
// CTestDoc commands
//	Default virtual implementation
void CTestDoc::GLInitContext(void)
{
	CAnimator::SetAnimator(new CAnimator());
	glClearColor(0.0f,0.0f,0.0f,0.0f);

	CRaptorDisplay *dsp = CRaptorDisplay::GetCurrentDisplay();
	IRenderingProperties &props = dsp->getRenderingProperties();
	props.setLighting(IRenderingProperties::ENABLE);
	props.setTexturing(IRenderingProperties::ENABLE);
	props.setMultisampling(IRenderingProperties::ENABLE);

	CRaptorDisplayConfig state;
	bool res = dsp->glQueryStatus(state,	GL_CURRENT_STATE_QUERY|GL_ARRAYS_STATE_QUERY|
											GL_TRANSFORM_STATE_QUERY|GL_COLORING_STATE_QUERY|
											GL_LIGHT_STATE_QUERY|GL_RASTERIZE_STATE_QUERY|
											GL_TEXTURE_STATE_QUERY|GL_PIXEL_STATE_QUERY|
											GL_FRAME_STATE_QUERY|GL_HINT_STATE_QUERY);

    m_pSG = new CBasicObjects::CCube();
    CShader *pShader = m_pSG->getShader();
    CMaterial *pMaterial = pShader->getMaterial();
    pMaterial->setAmbient(0.2f,0.2f,0.2f,1.0f);
    pMaterial->setDiffuse(0.9f,0.3f,0.7f,1.0f);
    pMaterial->setSpecular(5.0f,4.0f,3.0f,1.0f);
    pMaterial->setShininess(10.0f);
	m_pSG->removeModel(CGeometry::CGL_TEXTURE);

    CLight *pLight = new CLight;
    pLight->setAmbient(1.0f,1.0f,1.0f,1.0f);
    pLight->setDiffuse(1.0f,1.0f,1.0f,1.0f);
    pLight->setSpecular(1.0f,1.0f,1.0f,1.0f);
    pLight->setLightPosition(GL_COORD_VERTEX(0.0f,0.0f,10.0f,1.0f));
    pLight->setLightDirection(GL_COORD_VERTEX(0.0f,0.0f,-5.0f,1.0f));
    pLight->glActivate();

	IViewPoint *vp = dsp->getViewPoint();
    vp->setPosition(0,0,5.5,IViewPoint::EYE);
    vp->setPosition(0,0,0,IViewPoint::TARGET);

	CViewModifier *vm = new CViewModifier("test");
	vm->setObject(m_pSG);

	CModifier::TIME_FUNCTION tz;
	tz.timeFunction = CModifier::CGL_TIME_CONSTANT;
	tz.a0 = 0.1f;
	CModifier::TIME_FUNCTION tx;
	tx.timeFunction = CModifier::CGL_TIME_CONSTANT;
	tx.a0 = 0.15f;
	CModifier::TIME_FUNCTION ty;
	ty.timeFunction = CModifier::CGL_TIME_COSINE;
	ty.a2 = 0.15f;
	ty.a1 = 0.1f;
	ty.a0 = 0;
	vm->addAction(CViewModifier::ROTATE_VIEW,tx,ty,tz);

    CRaptorToolBox::SCENE_LOADER_OPTIONS	options;
    options.texturePath = ".";
    options.compressTextures = true;

    C3DSet *sponge = NULL;
    CRaptorToolBox::load3DStudioScene("Sponge.3DS",sponge,&options);
    sponge->translateAbsolute(0.0f,0.0f,0.0f);
	sponge->rotationX(-90.0);
	sponge->scale(0.025f,0.025f,0.025f);

	C3DSet::C3DSetIterator it = sponge->getIterator();
    CShadedGeometry *g = (CShadedGeometry *)(sponge->getChild(it++));
	while (g != NULL)
	{
		CShader *s = g->getShader();
        s->getMaterial()->setAmbient(0.1f,0.1f,0.1f,1.0f);
        s->getMaterial()->setDiffuse(0.4f,0.4f,0.9f,1.0f);
        s->getMaterial()->setSpecular(1.4f,1.4f,1.6f,1.0f);
        s->getMaterial()->setShininess(20.0f);
		s->glGetTextureUnitsSetup()->setUnitFunction(CTextureUnitSetup::IMAGE_UNIT_0, CTextureUnitSetup::CGL_MULTIPLY);
		g->setRenderingModel(CGeometry::CGL_FRONT_GEOMETRY);
		g->addModel(CGeometry::CGL_NORMALS);
		g->addModel(CGeometry::CGL_TEXTURE);
        g = (CShadedGeometry *)(sponge->getChild(it++));
    }

    CViewModifier *vm2 = new CViewModifier("test");
	vm2->setObject(sponge);
    tx.a0 = 0.015f;
    tz.a0 = 0.01f;
	vm2->addAction(CViewModifier::ROTATE_VIEW,tz,tx,ty);

	CBasicObjects::CRectangle *background = new CBasicObjects::CRectangle;
	background->setDimensions(40.0f,24.0f);
	background->translate(0.0f,0.0f,-7.0f);
	background->setRenderingModel(CGeometry::CGL_FRONT_GEOMETRY);
	background->addModel(CGeometry::CGL_NORMALS);
	background->addModel(CGeometry::CGL_TEXTURE);

	background->glLockData();
		background->setTexCoord(0,0.0f,0.0f);
		background->setTexCoord(1,2.0f,0.0f);
		background->setTexCoord(2,2.0f,2.0f);
		background->setTexCoord(3,0.0f,2.0f);
    background->glUnLockData();

	CTextureFactory &fct = CTextureFactory::getDefaultFactory();
	ITextureObject *T = fct.glCreateTexture(ITextureObject::CGL_COLOR24_ALPHA);
	fct.glLoadTexture(T,"Gaussian_blur_test.jpg");
	CShader *s = background->getShader();
	CTextureUnitSetup *tmu = s->glGetTextureUnitsSetup();
	tmu->setDiffuseMap(T);
	

#if defined(GL_ARB_color_buffer_float)
	if (Raptor::glIsExtensionSupported(GL_ARB_COLOR_BUFFER_FLOAT_EXTENSION_NAME))
    {
        CRaptorDisplayConfig rda;
        CRaptorDisplay::GetCurrentDisplay()->glQueryStatus(rda,GL_CONFIG_STATE_QUERY);
        rda.caption = "Raptor HDR Filter";

        hdr = new CHDRFilter(rda);
		CRaptorDisplay::GetCurrentDisplay()->glvkBindDisplay(*hdr);
		hdr->setBlurNbPass(2);
		hdr->setHFTreshold(1.0f);

        pBlur = new CBlurFilter();
		pBlur->setBlurModel(CBlurFilter::BLUR_GAUSSIAN_LINEAR);
		pBlur->setBlurSize(9);
		CRaptorDisplay::GetCurrentDisplay()->glvkBindDisplay(*pBlur);

		bwf = new CColorControlFilter();
		CRaptorDisplay::GetCurrentDisplay()->glvkBindDisplay(*bwf);
		bwf->setColorBlend(1.1f,0.8f,0.7f,1.0f,0.65f);
		bwf->setCorrection(1.0f,1.0f);
		bwf->enableFilter(true);
	
		mbf = new CMBFilter();
		mbf->setPercentage(0.95f,0.95f,0.95f,1.0f);
		CRaptorDisplay::GetCurrentDisplay()->glvkBindDisplay(*mbf);
		mbf->enableFilter(false);

		dof = new CDOFFilter();
		dof->setDOFParams(0.65f, 50.0f);
//		CRaptorDisplay::GetCurrentDisplay()->glBindDisplay(*dof);

        pMag = new CMagnifierFilter();
        pMag->setSizeFactors(16.0f,16.0f);
        pMag->enableFilter(true);
        pMag->setMitchellNetravaliKernel();
    }
#endif

	ColorController *pCtrl = new ColorController(bwf,hdr,mbf,pBlur,pMag);

	C3DScene *pScene = dsp->getRootScene();
	pScene->addObject(vm->getObject());
    pScene->addLight(pLight);
    pScene->addObject(vm2->getObject());
	pScene->addObject(background);
	CBasicObjects::CRectangle *pForeground = new Foreground(pMag);
	pForeground->setDimensions(10.0f,10.0f);
	pForeground->translate(10.0f,-5.0f,-6.0f);
	pForeground->setRenderingModel(CGeometry::CGL_FRONT_GEOMETRY);
	pForeground->addModel(CGeometry::CGL_NORMALS);
	pForeground->addModel(CGeometry::CGL_TEXTURE);
	pScene->addObject(pForeground);
}


void CTestDoc::glRender()
{
	m_pDisplay->glvkBindDisplay(m_wnd);

	m_pDisplay->glRender();

	m_pDisplay->glvkUnBindDisplay();
}

