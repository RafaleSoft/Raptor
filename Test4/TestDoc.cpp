// TestDoc.cpp : implementation of the CTestDoc class
//

#include "StdAfx.h"
#include "TestDoc.h"

#include "System/Raptor.h"
#include "System/RaptorDisplay.h"
#include "GLHierarchy/IRenderingProperties.h"
#include "GLHierarchy/Light.h"
#include "GLHierarchy/ShaderProgram.h"
#include "Engine/IViewPoint.h"
#include "Engine/3DScene.h"
#include "Engine/Mirror.h"

#include "ToolBox/Filters/HDRFilter.h"
#include "ToolBox/Filters/MBFilter.h"
#include "ToolBox/Filters/ColorControlFilter.h"
#include "ToolBox/Filters.h"

#include "Reflection.h"
#include "Slide.h"
#include "PathObject.h"

const float CTestDoc::SPEED_TRESHOLD = 0.0005f;
const float CTestDoc::SPEED_FADE = 0.97f;

CTestDoc::CTestDoc(HWND wnd, CRaptorDisplay *pDisplay)
{
	m_wnd = wnd;
    m_pDisplay = pDisplay;
	m_currentSpeed = 0.0f;
	m_currentFade = 1.0f;
	m_hdr = 0;
	m_mb = 0;
}

CTestDoc::~CTestDoc()
{
}

void CTestDoc::highLight(bool HL)
{
    vector<CSlide*>::iterator it = m_slides.begin();
    (*it)->highLight(HL);

	m_mb->enableFilter(HL);
}

void CTestDoc::translateMenu(float dx)
{
    vector<CSlide*>::iterator it = m_slides.begin();
    while (it != m_slides.end())
    {
        CSlide *pSlide = *it++;
        pSlide->translate(dx);
    }

	m_currentSpeed = dx;
}


/////////////////////////////////////////////////////////////////////////////
// CTestDoc commands
//	Default virtual implementation
void CTestDoc::GLInitContext(HDC hdc)
{
    RAPTOR_HANDLE display;
    display.ptr(hdc);
	display.hClass(CLIENT_HANDLE_CLASS);

	m_pDisplay->glvkBindDisplay(display);
	CFilters::glInstallFilters();

    glClearColor(0.0f,0.0f,0.0f,0.0f);

	CRaptorDisplay *dsp = CRaptorDisplay::GetCurrentDisplay();
	IRenderingProperties &props = dsp->getRenderingProperties();
	props.setLighting(IRenderingProperties::ENABLE);

	IViewPoint *vp = dsp->getViewPoint();
    vp->setPosition(0,2,4.5f,IViewPoint::EYE);
    vp->setPosition(0,0,0,IViewPoint::TARGET);

	C3DScene *pScene = dsp->getRootScene();
    pScene->useZSort();
    pScene->useSceneGlobalAmbient(CColor::RGBA(0.1f,0.1f,0.1f,1.0f));

    CReflection *pMirrorPlane = new CReflection(30.0f,12.0f,30,12);
	pMirrorPlane->rotationX(-90.0f);
	pMirrorPlane->translate(0.0f,-0.2f,-2.0f);
    CMirror *pMirror = new CMirror();
    pMirror->addObject(pMirrorPlane);
    GL_COORD_VERTEX    normal(0.0f,1.0f,0.0f,1.0f);
    GL_COORD_VERTEX    point(0.0f,-0.2f,0.0f,1.0f);
    pMirror->setMirrorPlane(normal,point);
    pScene->addMirror(pMirror);

    CPathObject *pPath = new CPathObject();
    pScene->addObject(pPath);
    
    CSlide *pSlide = new CSlide("slide1",*pPath);
    pSlide->translate(0.0f);
    pScene->addObject(pSlide);
    m_slides.push_back(pSlide);
    pSlide->setVideo("Datas\\rapsplsh.avi");

    pSlide = new CSlide("slide2",*pPath);
    pSlide->translate(0.1f);
    pScene->addObject(pSlide);
    m_slides.push_back(pSlide);
    pSlide->setVideo("Datas\\ruby.avi");

    pSlide = new CSlide("slide3",*pPath);
    pSlide->translate(0.2f);
    pScene->addObject(pSlide);
    m_slides.push_back(pSlide);
    pSlide->setVideo("Datas\\cigarette.avi");

    pSlide = new CSlide("slide4",*pPath);
    pSlide->translate(0.3f);
    pScene->addObject(pSlide);
    m_slides.push_back(pSlide);
    pSlide->setVideo("Datas\\intro.wmv");

    pSlide = new CSlide("slide5",*pPath);
    pSlide->translate(0.4f);
    pScene->addObject(pSlide);
    m_slides.push_back(pSlide);
	//pSlide->setVideo("Datas\\plush1_720p_10s.m2v");

    pSlide = new CSlide("slide6",*pPath);
    pSlide->translate(-0.1f);
    pScene->addObject(pSlide);
    m_slides.push_back(pSlide);
    pSlide->setVideo("Datas\\GLOBE.AVI");

    pSlide = new CSlide("slide7",*pPath);
    pSlide->translate(-0.2f);
    pScene->addObject(pSlide);
    m_slides.push_back(pSlide);
    pSlide->setVideo("Datas\\explosion.avi");

    pSlide = new CSlide("slide8",*pPath);
    pSlide->translate(-0.3f);
    pScene->addObject(pSlide);
    m_slides.push_back(pSlide);
	pSlide->setVideo("Datas\\sub_scene.avi");

    pSlide = new CSlide("slide9",*pPath);
    pSlide->translate(-0.4f);
    pScene->addObject(pSlide);
    m_slides.push_back(pSlide);
	pSlide->setVideo("Datas\\cube.mp4");


    CLight *pLight = new CLight;
    pLight->setAmbient(1.0f,1.0f,1.0f,1.0f);
    pLight->setDiffuse(1.0f,1.0f,1.0f,1.0f);
    pLight->setSpecular(1.0f,1.0f,1.0f,1.0f);
    pLight->setLightPosition(GL_COORD_VERTEX(0.0f,5.0f,5.0f,1.0f));
    pLight->setLightDirection(GL_COORD_VERTEX(0.0f,-5.0f,-5.0f,1.0f));
    pLight->setAttenuation(0.01f,0.0f,0.55f);
    pLight->glActivate();
    pScene->addLight(pLight);

#if defined(GL_ARB_color_buffer_float)
	if (IRaptor::glIsExtensionSupported(GL_ARB_COLOR_BUFFER_FLOAT_EXTENSION_NAME))
    {
        CRaptorDisplayConfig rda;
        CRaptorDisplay::GetCurrentDisplay()->glQueryStatus(rda,GL_CONFIG_STATE_QUERY);
        rda.caption = "Raptor HDR Filter";

        CHDRFilter *hdr = new CHDRFilter(rda);
		hdr->setFilterModel(CRaptorDisplayFilter::RENDER_TEXTURE);
		CRaptorDisplay::GetCurrentDisplay()->glvkBindDisplay(*hdr);
        hdr->enableFilter(true);
		
		m_hdr = hdr;

        CColorControlFilter *bwf = new CColorControlFilter;
		bwf->setFilterModel(CRaptorDisplayFilter::RENDER_TEXTURE);
        //CRaptorDisplay::GetCurrentDisplay()->glBindDisplay(*bwf);
        bwf->setColorBlend(1.1f,0.8f,0.7f,1.0f,0.65f);

        CMBFilter *mb = new CMBFilter();
        mb->setPercentage(0.95f,0.95f,0.95f,1.0f);
		mb->setFilterModel(CRaptorDisplayFilter::RENDER_TEXTURE);
		CRaptorDisplay::GetCurrentDisplay()->glvkBindDisplay(*mb);
		mb->enableFilter(false);
		
		m_mb = mb;
    }
#endif

	m_pDisplay->glvkUnBindDisplay();
}


void CTestDoc::glRender()
{
    HDC hdc = GetDC(m_wnd);
    if (hdc)
    {
        glDisplay(hdc);
        ReleaseDC(m_wnd,hdc);
    }
}

void CTestDoc::glDisplay(HDC hdc)
{

	if (fabs(m_currentSpeed) > SPEED_TRESHOLD)
	{
		// find the nearest slide and go for it in 50 frames
		float nextpos = 100.0f;
		float n = log(SPEED_TRESHOLD / fabs(m_currentSpeed)) / log(SPEED_FADE);

		float cs = m_currentSpeed * SPEED_FADE;
		float d = cs;
		for (int i=0;i<n;i++)
			d += (cs *= SPEED_FADE);

		vector<CSlide*>::iterator it = m_slides.begin();
		while (it != m_slides.end())
		{
			float pos = (*it++)->getPos();
			if (fabs(pos + d - 0.5f) < nextpos)
				nextpos = fabs(pos + d - 0.5f);
		}

		if (nextpos < fabs(d))
			m_currentSpeed *= SPEED_FADE;

		translateMenu(m_currentSpeed);
	}

    RAPTOR_HANDLE display;
    display.ptr(hdc);
	display.hClass(CLIENT_HANDLE_CLASS);

	m_pDisplay->glvkBindDisplay(display);

	m_pDisplay->glRender();

	m_pDisplay->glvkUnBindDisplay();
}

