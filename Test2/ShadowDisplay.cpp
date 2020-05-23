// ShadowDisplay.cpp: implementation of the CShadowDisplay class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ShadowDisplay.h"

#include "GLHierarchy/Persistence.h"
#include "Engine/3DScene.h"
#include "Engine/Environment.h"
#include "GLHierarchy/3DSet.h"
#include "GLHierarchy/GeometryEditor.h"
#include "GLHierarchy/ShadedGeometry.h"
#include "GLHierarchy/Shader.h"
#include "GLHierarchy/Light.h"
#include "System/RaptorDisplay.h"
#include "GLHierarchy/Object3DShadow.h"
#include "GLHierarchy/Object3DInstance.h"
#include "GLHierarchy/TextureUnitSetup.h"
#include "GLHierarchy/TextureSet.h"
#include "GLHierarchy/TextureFactory.h"
#include "GLHierarchy/TextureFactoryConfig.h"
#include "GLHierarchy/TextureObject.h"
#include "GLHierarchy/GLFont.h"
#include "GLHierarchy/GL3DFont.h"
#include "GLHierarchy/GLFontFactory.h"
#include "system/Raptor.h"
#include "GLHierarchy/IRenderingProperties.h"
#include "Engine/IViewPoint.h"

#include "ToolBox/RaptorToolBox.h"


static CGenericVector<float> LL(0.0f,-1.0f,-1.0f,1.0f);
static GL_COORD_VERTEX S(0.0f,0.0f,50.0f,1.0f);

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CShadowDisplay::CShadowDisplay()
{
	texture = NULL;
	material = NULL;

	sponge = NULL;
	contour = NULL;
	m_light = NULL;

	m_pVP = NULL;
}

CShadowDisplay::~CShadowDisplay()
{

}


void CShadowDisplay::Init()
{
	CGenericDisplay::Init();

	CPersistence *p = CPersistence::FindObject("main_textures");
	if (p->getId().isSubClassOf(CTextureSet::CTextureSetClassID::GetClassId()))
		 texture = (CTextureSet *)p;

    CRaptorToolBox::SCENE_LOADER_OPTIONS	options;
	options.texturePath = "Datas";
    options.compressTextures = true;
	
    CRaptorToolBox::load3DStudioScene("Datas\\Sponge.3DS",sponge,&options);
	
	sponge->translateAbsolute(0.0f,0.0f,0.0f);
	sponge->rotationX(-90.0);
	sponge->scale(0.3f,0.3f,0.3f);

	CShader *knotShader = NULL;
	p = CPersistence::FindObject("Knot");
	if (p->getId().isSubClassOf(CGeometry::CGeometryClassID::GetClassId()))
	{
		knot = (CShadedGeometry*)p;
		knot->setRenderingModel(CGeometry::CGL_FRONT_GEOMETRY);
		knot->addModel(CGeometry::CGL_NORMALS);
		knot->addModel(CGeometry::CGL_TEXTURE);
		knotShader = knot->getShader();
	}

	CTextureFactory &f = CTextureFactory::getDefaultFactory();
	CTextureObject *T = f.glCreateTexture(ITextureObject::CGL_COLOR24_ALPHA,ITextureObject::CGL_MULTIPLY,ITextureObject::CGL_BILINEAR);
	T->glSetTransparency(255);
	CTextureFactoryConfig& config = f.getConfig();
	const CTextureFactoryConfig::ICompressor *compressor = config.getCurrentCompressor();
	if (0 < config.getNumCompressors())
		config.setCurrentCompressor(config.getCompressor("OpenGL"));
	f.glLoadTexture(T,"Datas\\start.tga");
    knotShader->glGetTextureUnitsSetup()->setDiffuseMap(T);

	LL.Normalize();
	m_light = new CLight();
	m_light->setLightPosition(S);
    GL_COORD_VERTEX L(LL.X(),LL.Y(),LL.Z(),LL.H());
	m_light->setLightDirection(L);
	m_light->setAmbient(0.0f,0.0f,0.0f,1.0f);
	m_light->setDiffuse(1.0f,1.0f,1.0f,1.0f);
	m_light->setSpecular(1.0f,1.0f,1.0f,1.0f);

	
	CGL3DFont *font = CGLFontFactory::create3DFont("Datas\\kld.ttf", 20, true, true, "main_font");
    fulltext = new CShadedGeometry("FONT_Geometry");
    *fulltext = *font->glBuildGeometry("Raptor",1,5,2.0f);
	fulltext->scale(0.25f,0.25f,0.25f);
	fulltext->translateAbsolute(0.0f,0.0f,0.0f);
	delete font;

    CShader *textShader = fulltext->getShader();
    textShader->glGetTextureUnitsSetup()->setDiffuseMap(texture->getTexture(1));
    textShader->getMaterial()->setAmbient(0.5f,0.5f,0.5f,1.0f);
    textShader->getMaterial()->setDiffuse(0.6f,0.6f,0.6f,1.0f);
    textShader->getMaterial()->setSpecular(0.9f,0.9f,0.9f,1.0f);
    textShader->getMaterial()->setShininess(20.0f);
    textShader->setColor(0.1f,0.1f,0.1f,1.0f);
	fulltext->setRenderingModel(CGeometry::CGL_FRONT_GEOMETRY);
	fulltext->addModel(CGeometry::CGL_NORMALS);
	fulltext->addModel(CGeometry::CGL_TEXTURE);

    m_pScene = new C3DScene("SHADOWVOLUME_SCENE");
    m_pScene->addLight(m_light);
    m_pScene->addObject(fulltext);
    //sponge->getProperties().setCastShadow(false);
    //m_pScene->addObject(sponge);

	C3DSet::C3DSetIterator it = sponge->getIterator();
	CShadedGeometry *g = (CShadedGeometry*)(sponge->getChild(it++));
	while (g != NULL)
	{
		g->getShader()->setColor(0.2f,0.2f,0.2f,1.0f);
        g->getShader()->getMaterial()->setAmbient(0.5f,0.5f,0.5f,1.0f);
		g->setRenderingModel(CGeometry::CGL_FRONT_GEOMETRY);
		g->addModel(CGeometry::CGL_NORMALS);
		g->addModel(CGeometry::CGL_TEXTURE);
		g->getProperties().setCastShadow(false);
		g = (CShadedGeometry*)(sponge->getChild(it++));
		m_pScene->addObject(g);
	}

    CObject3DInstance *knotInstance = new CObject3DInstance(knot,"KNOT1");
    knotInstance->translate(-10,-10,-10);
    m_pScene->addObject(knotInstance);
    CObject3DInstance *knotInstance2 = new CObject3DInstance(knot,"KNOT2");
    knotInstance2->translate(10,-10,-10);
    m_pScene->addObject(knotInstance2);
    CObject3DInstance *knotInstance3 = new CObject3DInstance(knot,"KNOT3");
    knotInstance3->translate(10,10,-10);
    m_pScene->addObject(knotInstance3);
    CObject3DInstance *knotInstance4 = new CObject3DInstance(knot,"KNOT4");
    knotInstance4->translate(-10,10,-10);
    m_pScene->addObject(knotInstance4);
    CObject3DInstance *knotInstance5 = new CObject3DInstance(knot,"KNOT5");
    knotInstance5->translate(-10,-10,10);
    m_pScene->addObject(knotInstance5);
    CObject3DInstance *knotInstance6 = new CObject3DInstance(knot,"KNOT6");
    knotInstance6->translate(10,-10,10);
    m_pScene->addObject(knotInstance6);
    CObject3DInstance *knotInstance7 = new CObject3DInstance(knot,"KNOT7");
    knotInstance7->translate(10,10,10);
    m_pScene->addObject(knotInstance7);
    CObject3DInstance *knotInstance8 = new CObject3DInstance(knot,"KNOT8");
    knotInstance8->translate(-10,10,10);
    m_pScene->addObject(knotInstance8);

	CEnvironment *shadow_volume = CEnvironment::glCreateEnvironment(*m_pScene, CEnvironment::SHADOW_VOLUME, 0, 0);
    m_pScene->glManageEnvironment(shadow_volume);
    m_pScene->useZSort(); //Optimize surrounding bbox drawings to use it in this very special case.

	CRaptorDisplay* pDisplay = CRaptorDisplay::GetCurrentDisplay();
	m_pVP = pDisplay->createViewPoint();
    m_pVP->setPosition(0.0,0.0,25.0,IViewPoint::EYE);
    m_pVP->setPosition(0.0,0.0,0.0,IViewPoint::TARGET);
	m_pVP->setViewVolume(-1.33f,1.33f,-1.0f,1.0f,1.0f,10000,IViewPoint::PERSPECTIVE);

	pDisplay->addScene(m_pScene);
}

void CShadowDisplay::ReInit()
{
	CGenericDisplay::ReInit();

    glDisable(GL_LIGHT2);
	glDisable(GL_LIGHT1);

	m_light->glActivate();
	m_light->glRender();

	m_light->setLightPosition(S);
    GL_COORD_VERTEX L(LL.X(),LL.Y(),LL.Z(),LL.H());
	m_light->setLightDirection(L);
	m_light->glRender();

	CRaptorDisplay* pDisplay = CRaptorDisplay::GetCurrentDisplay();
	pDisplay->setViewPoint(m_pVP);
	IRenderingProperties &rp = pDisplay->getRenderingProperties();
	rp.setTexturing(IRenderingProperties::ENABLE);
	rp.setLighting(IRenderingProperties::ENABLE);
	rp.setBlending(IRenderingProperties::ENABLE);
	pDisplay->selectScene("SHADOWVOLUME_SCENE");
}

void CShadowDisplay::UnInit()
{
}

void CShadowDisplay::Display()
{
	if (reinit)
		ReInit();

	S.x = 50.0f * cos(8*PI*dt);
	S.y = 50.0f * sin(10*PI*dt);
	S.z = 50.0f * cos(12*PI*dt);
	m_light->setLightPosition(S);

	LL.X() = -S.x;
	LL.Y() = -S.y;
	LL.Z() = -S.z;
	LL.Normalize();
    GL_COORD_VERTEX L(LL.X(),LL.Y(),LL.Z(),LL.H());
	m_light->setLightDirection(L);

	float x = 25 * cos(10*PI*dt) * cos(10*PI*dt);
	float z = 25 * sin(10*PI*dt) * cos(10*PI*dt);
	float y = 25 * sin(10*PI*dt);
	m_pVP->setPosition(x,y,z,IViewPoint::EYE);

	glDisable(GL_TEXTURE_2D);
    glDisable(GL_LIGHTING);
    glBegin(GL_LINES);
        glColor4f(1.0f,1.0f,1.0f,1.0f);
        glVertex4f(0.0f,0.0f,0.0f,1.0f);
        glVertex4fv(S);
    glEnd();
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_LIGHTING);

	dt+=0.0001f;
	if (dt>1.0) dt=0.0;
}