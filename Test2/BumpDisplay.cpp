// BumpDisplay.cpp: implementation of the CBumpDisplay class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BumpDisplay.h"
#include <math.h>

#include "GLHierarchy/SimpleObject.h"
#include "GLHierarchy/Light.h"
#include "GLHierarchy/TextureFactory.h"
#include "GLHierarchy/TextureFactoryConfig.h"
#include "GLHierarchy/ITextureObject.h"
#include "GLHierarchy/TextureUnitSetup.h"
#include "GLHierarchy/TextureSet.h"
#include "GLHierarchy/BumppedGeometry.h"
#include "GLHierarchy/Object3DInstance.h"
#include "GLHierarchy/IRenderingProperties.h"
#include "GLHierarchy/Shader.h"
#include "Engine/ViewModifier.h"
#include "Engine/LightModifier.h"
#include "Engine/3DScene.h"
#include "System/RaptorDisplay.h"

#include "GLHierarchy/PerlinNoise.h"
#include "Toolbox/BasicObjects.h"
#include "ToolBox/Imaging/BumpmapLoader.h"


class BackGround : public CBasicObjects::CRectangle
{
public:
	BackGround()
	{
		setDimensions(140.0f, 100.0f);
		translate(0.0f, 0.0f, -50.0f);

		CShader *pShader = getShader();
		CMaterial *m = pShader->getMaterial();
		m->setShininess(50.0f);
		m->setAmbient(0.1f,0.1f,0.1f,1.0f);
		m->setDiffuse(0.5f,0.5f,0.5f,1.0f);
		m->setSpecular(0.5f,0.5f,0.5f,1.0f);
    };

	virtual ~BackGround() {};

	virtual void glRender();
	virtual void glClipRender() { glRender();};

	CLight	*light;
    CLight	*light2;
};

void BackGround::glRender()
{
    glColor4f(1.0f,1.0f,1.0f,1.0f);
	CShadedGeometry::glRender();

    GL_COORD_VERTEX lpos;
    
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);

	glBegin(GL_LINES);
		glColor4fv(light->getSpecular());
		glVertex3f(0.0f,0.0f,-20.0f);
        lpos = light->getLightPosition();
		glVertex3f(lpos.x,lpos.y,lpos.z);

		glColor4fv(light2->getSpecular());
        glVertex3f(0.0f,0.0f,-20.0f);
        lpos = light2->getLightPosition();
		glVertex3f(lpos.x,lpos.y,lpos.z);
	glEnd();

	glEnable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBumpDisplay::CBumpDisplay()
{
}

CBumpDisplay::~CBumpDisplay()
{
}

float lposx(float dt)
{
    return 15 * (float)(cos(2*PI*dt*0.1)*cos(4*PI*dt*0.1));
}
float lposy(float dt)
{
	return 2.0f + 15 * (float)(sin(4*PI*dt*0.1)*cos(2*PI*dt*0.1));
}

void CBumpDisplay::Init()
{
	CGenericDisplay::Init();

	CPersistence *p = CPersistence::FindObject("main_textures");

	CTextureFactory &f = CTextureFactory::getDefaultFactory();
	CTextureFactoryConfig& config = f.getConfig();
	config.setBumpAmplitude(3.5f);

	CTextureSet *t = NULL;
	if (p->getId().isSubClassOf(CTextureSet::CTextureSetClassID::GetClassId()))
		 t = (CTextureSet *)p;

    ITextureObject *tt = f.glCreateTexture( ITextureObject::CGL_COLOR24_ALPHA,
											ITextureObject::CGL_ALPHA_TRANSPARENT,
											ITextureObject::CGL_BILINEAR);
	f.glResizeTexture(tt,512,512);
	CBumpmapLoader loader(f.getConfig().getBumpAmplitude());
	CPerlinNoise noise(&loader);
	noise.setNoiseModel(CPerlinNoise::NOISE2);
	noise.generateMirrorTexture(true);
/*
	ITextureObject *tt = f.glCreateVolumeTexture(ITextureObject::CGL_COLOR24_ALPHA,
												 ITextureObject::CGL_ALPHA_TRANSPARENT,
												 ITextureObject::CGL_BILINEAR);
	f.glResizeTexture(tt,256,256,256);
*/
    noise.glGenerate(tt,0,0,512,512);

	p = CPersistence::FindObject("Bump teapot");
	if (p->getId().isSubClassOf(CBumppedGeometry::CBumppedGeometryClassID::GetClassId()))
		 teapot = (CBumppedGeometry *)p;
	teapot->setNormalMap(tt);

	CPerlinNoise noise2;
	ITextureObject *tt2 = f.glCreateTexture(ITextureObject::CGL_COLOR24_ALPHA,
                                            ITextureObject::CGL_ALPHA_TRANSPARENT,
                                            ITextureObject::CGL_BILINEAR);
	f.glResizeTexture(tt2,512,512);
    noise2.glGenerate(tt2,0,0,512,512);
	BackGround *pbg = new BackGround();
	CTextureUnitSetup *ptmu = pbg->getShader()->glGetTextureUnitsSetup();
	ptmu->setDiffuseMap(tt2);

    //
    //  Light and its modifier
    m_pLight = new CLight("Bump light");
    m_pLight->setAmbient(1.0f,1.0f,1.0f,1.0f);
    m_pLight->setDiffuse(1.0f,1.0f,1.0f,1.0f);
	m_pLight->setSpecular(0.2f,0.7f,1.0f,1.0f);
    m_pLight->setLightDirection(GL_COORD_VERTEX(0,0,0,1));
    m_pLight2 = new CLight("Bump light2");
    m_pLight2->setAmbient(1.0f,1.0f,1.0f,1.0f);
    m_pLight2->setDiffuse(1.0f,1.0f,1.0f,1.0f);
    m_pLight2->setSpecular(1.0f,0.6f,0.4f,1.0f);
    m_pLight2->setLightDirection(GL_COORD_VERTEX(0,0,0,1));

    CLightModifier *lm = new CLightModifier("BUMP_LIGHT_ANIMATOR");
    lm->setLight(m_pLight);
    CModifier::TIME_FUNCTION lx;
    CModifier::TIME_FUNCTION ly;
    CModifier::TIME_FUNCTION lz;
    lx.pUserFunction = lposx;
    lx.timeFunction = CModifier::CGL_TIME_USER;
    ly.pUserFunction = lposy;
    ly.timeFunction = CModifier::CGL_TIME_USER;
    lz.a0 = -5.0f;
    lz.timeFunction = CModifier::CGL_TIME_CONSTANT;
    lm->addAction(CLightModifier::SET_POSITION,lx,ly,lz);

    lm = new CLightModifier("BUMP_LIGHT_ANIMATOR2");
    lm->setLight(m_pLight2);
    lx.pUserFunction = lposy;
    lx.timeFunction = CModifier::CGL_TIME_USER;
    ly.pUserFunction = lposx;
    ly.timeFunction = CModifier::CGL_TIME_USER;
    lz.a0 = -4.0f;
    lz.timeFunction = CModifier::CGL_TIME_CONSTANT;
    lm->addAction(CLightModifier::SET_POSITION,lx,ly,lz);

    
    pbg->light = m_pLight;
    pbg->light2 = m_pLight2;

    // 
    //  View Modifier of the teapot
    CViewModifier *vm = new CViewModifier("BUMP_ANIMATOR");
    vm->setObject(teapot);
    CModifier::TIME_FUNCTION tx;
    CModifier::TIME_FUNCTION ty;
    CModifier::TIME_FUNCTION tz;

    vm->addAction(CViewModifier::RESET_TRANSFORM,tx,ty,tz);


    ty.timeFunction = CModifier::CGL_TIME_CONSTANT;
    ty.a0 = 2;
    tz.timeFunction = CModifier::CGL_TIME_CONSTANT;
    tz.a0 = -20;
    vm->addAction(CViewModifier::TRANSLATEABSOLUTE,tx,ty,tz);


    tx.timeFunction = CModifier::CGL_TIME_COSINE;
    tx.a0 = 0.0f;
    tx.a1 = 0.4f;
    tx.a2 = 50.0f;       //  rx = 50.0 * cos ( 0.4 * t )
    ty.timeFunction = CModifier::CGL_TIME_COSINE;
    ty.a0 = 0.0f;
    ty.a1 = 0.3f;
    ty.a2 = 100.0f;      //  ry = 100.0 * cos ( 0.3 * t )
    tz.timeFunction = CModifier::CGL_TIME_COSINE;
    tz.a0 = 0.0f;
    tz.a1 = 0.5f;
    tz.a2 = 50.0f;       //  rz = 50.0 * cos ( 0.5 * t )
    vm->addAction(CViewModifier::ROTATE_VIEW,tx,ty,tz);

	C3DScene *pScene = new C3DScene("BUMP_SCENE");
	pScene->useZSort();
    pScene->useSceneGlobalAmbient(CColor::RGBA(0.1f,0.1f,0.1f,1.0f));
	pScene->addLight(pbg->light);
    pScene->addLight(pbg->light2);
	pScene->addObject(pbg);
	pScene->addObject(vm->getObject());

	CRaptorDisplay* pDisplay = CRaptorDisplay::GetCurrentDisplay();
	pDisplay->addScene(pScene);
}


void CBumpDisplay::UnInit()
{
    m_pLight2->glDeActivate();
}

void CBumpDisplay::ReInit()
{
	CGenericDisplay::ReInit();

	CPersistence *p = CPersistence::FindObject("BUMP_ANIMATOR");
	if (p != NULL)
	{
		((CViewModifier*)p)->animate(true);
	}
    p = CPersistence::FindObject("BUMP_LIGHT_ANIMATOR");
	if (p != NULL)
	{
		((CLightModifier*)p)->animate(true);
	}
    p = CPersistence::FindObject("BUMP_LIGHT_ANIMATOR2");
	if (p != NULL)
	{
		((CLightModifier*)p)->animate(true);
	}

	CRaptorDisplay* pDisplay = CRaptorDisplay::GetCurrentDisplay();
	pDisplay->selectScene("BUMP_SCENE");
	IRenderingProperties &rp = pDisplay->getRenderingProperties();
	rp.setTexturing(IRenderingProperties::ENABLE);
	rp.setLighting(IRenderingProperties::ENABLE);
	rp.setBlending(IRenderingProperties::DISABLE);
}


void CBumpDisplay::Display()
{
	if (reinit)
		ReInit();
}
