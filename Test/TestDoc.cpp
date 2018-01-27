// TestDoc.cpp : implementation of the CTestDoc class
//

#include "StdAfx.h"
#include <sstream>

#include "TestDoc.h"
#include "ColorController.h"

#include "System/RaptorDisplay.h"
#include "GLHierarchy/ShadedGeometry.h"
#include "GLHierarchy/Object3DInstance.h"
#include "GLHierarchy/TextureFactory.h"
#include "GLHierarchy/TextureFactoryConfig.h"
#include "GLHierarchy/TextureObject.h"
#include "GLHierarchy/FragmentShader.h"
#include "GLHierarchy/VertexShader.h"
#include "GLHierarchy/Shader.h"
#include "Engine/ViewPoint.h"
#include "Engine/ViewModifier.h"
#include "Engine/3DScene.h"
#include "Engine/Animator.h"
#include "System/Raptor.h"
#include "GLHierarchy/Material.h"
#include "GLHierarchy/Light.h"
#include "GLHierarchy/RenderingProperties.h"
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
#include "ToolBox/Filters/TextureFilter.h"


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
	Foreground(CMagnifierFilter *mf):fgMag(mf),status(mf->isEnabled())
	{
		CTextureFactory &fct = CTextureFactory::getDefaultFactory();
		CTextureObject *T = fct.glCreateTexture(ITextureObject::CGL_COLOR24_ALPHA);
		fct.glLoadTexture(T,"lrock049.jpg");

		fgMag->setColorInput(T);
		CTextureObject *output = fgMag->glCreateColorOutput();
		fgMag->glInitFilter();

		CShader *s = getShader();
		CTextureUnitSetup *tmu = s->glGetTextureUnitsSetup();
		tmu->setDiffuseMap(output);
		tmu->glBuildSetup();

		glRenderFilter();
	};

	virtual ~Foreground() {};

	virtual void glRender()
	{
		if (status != fgMag->isEnabled())
		{
			status = !status;
			glRenderFilter();
		}
		//if (fgMag->isEnabled())
		//	glRenderFilter();
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
    //glClearColor(0.2f,0.6f,0.7f,0.0f);
	glClearColor(0.0f,0.0f,0.0f,0.0f);

	CRaptorDisplay *dsp = CRaptorDisplay::GetCurrentDisplay();
    CRenderingProperties *props = dsp->getRenderingProperties();
    props->setLighting(CRenderingProperties::ENABLE);
	props->setTexturing(CRenderingProperties::ENABLE);
	props->setMultisampling(CRenderingProperties::ENABLE);

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
    pMaterial->setSpecular(1.2f,0.8f,0.8f,1.0f);
    pMaterial->setShininess(10.0f);
	m_pSG->getRenderingModel().removeModel(CGeometry::CRenderingModel::CGL_TEXTURE);

    CLight *pLight = new CLight;
    pLight->setAmbient(1.0f,1.0f,1.0f,1.0f);
    pLight->setDiffuse(1.0f,1.0f,1.0f,1.0f);
    pLight->setSpecular(1.0f,1.0f,1.0f,1.0f);
    pLight->setLightPosition(GL_COORD_VERTEX(0.0f,0.0f,10.0f,1.0f));
    pLight->setLightDirection(GL_COORD_VERTEX(0.0f,0.0f,-5.0f,1.0f));
    pLight->glActivate();

	CViewPoint *vp = dsp->getViewPoint();
    vp->setPosition(0,0,5.5,CViewPoint::EYE);
    vp->setPosition(0,0,0,CViewPoint::TARGET);

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

    CGeometry::CRenderingModel l_model(0);
    l_model.addModel(CGeometry::CRenderingModel::CGL_FRONT_GEOMETRY);
    l_model.addModel(CGeometry::CRenderingModel::CGL_NORMALS);
	l_model.addModel(CGeometry::CRenderingModel::CGL_TEXTURE);
    C3DSet::C3DSetIterator it = sponge->getIterator();
    CShadedGeometry *g = (CShadedGeometry *)(sponge->getChild(it++));
	while (g != NULL)
	{
        g->getShader()->getMaterial()->setAmbient(0.1f,0.1f,0.1f,1.0f);
        g->getShader()->getMaterial()->setDiffuse(0.4f,0.4f,0.9f,1.0f);
        g->getShader()->getMaterial()->setSpecular(0.4f,0.4f,0.6f,1.0f);
        g->getShader()->getMaterial()->setShininess(20.0f);
        g->setRenderingModel(l_model);
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
	background->setRenderingModel(l_model);
	background->glLockData();
		background->setTexCoord(0,0,0);
		background->setTexCoord(1,2,0);
		background->setTexCoord(2,2,2);
		background->setTexCoord(3,0,2);
    background->glUnLockData();

	CTextureFactory &fct = CTextureFactory::getDefaultFactory();
	CTextureObject *T = fct.glCreateTexture(ITextureObject::CGL_COLOR24_ALPHA);
	fct.glLoadTexture(T,"Gaussian_blur_test.jpg");
	CShader *s = background->getShader();
	CTextureUnitSetup *tmu = s->glGetTextureUnitsSetup();
	tmu->setDiffuseMap(T);
	tmu->glBuildSetup();
	

#if defined(GL_ARB_color_buffer_float)
    if (Raptor::glIsExtensionSupported("GL_ARB_color_buffer_float"))
    {
        CRaptorDisplayConfig rda;
        CRaptorDisplay::GetCurrentDisplay()->glQueryStatus(rda,GL_CONFIG_STATE_QUERY);
        rda.caption = "Raptor HDR Filter";

        hdr = new CHDRFilter(rda);
        CRaptorDisplay::GetCurrentDisplay()->glBindDisplay(*hdr);
		hdr->setBlurNbPass(2);
		hdr->setHFTreshold(1.0f);

        pBlur = new CBlurFilter();
		pBlur->setBlurModel(CBlurFilter::BLUR_GAUSSIAN_LINEAR);
		pBlur->setBlurSize(9);
        CRaptorDisplay::GetCurrentDisplay()->glBindDisplay(*pBlur);

		bwf = new CColorControlFilter();
		CRaptorDisplay::GetCurrentDisplay()->glBindDisplay(*bwf);
		bwf->setColorBlend(1.1f,0.8f,0.7f,1.0f,0.65f);
		bwf->setCorrection(1.0f,1.0f);
		bwf->enableFilter(true);
	
		mbf = new CMBFilter();
		mbf->setPercentage(0.95f,0.95f,0.95f,1.0f);
		CRaptorDisplay::GetCurrentDisplay()->glBindDisplay(*mbf);
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
	pForeground->setRenderingModel(l_model);
	pScene->addObject(pForeground);
}


void CTestDoc::glRender()
{
	m_pDisplay->glBindDisplay(m_wnd);

	m_pDisplay->glRender();

	m_pDisplay->glUnBindDisplay();
}

