// EMBMDisplay.cpp: implementation of the CEMBMDisplay class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "System\CGLTypes.h"
#include "GLHierarchy\Light.h"
#include "GLHierarchy\Shader.h"
#include "GLHierarchy\TextureSet.h"
#include "GLHierarchy\TextureFactory.h"
#include "GLHierarchy\VertexShader.h"
#include "GLHierarchy\TextureUnitSetup.h"
#include "GLHierarchy\TextureObject.h"
#include "GLHierarchy\SimpleObject.h"
#include "GLHierarchy\EMBMGeometry.h"
#include "Engine\3DScene.h"
#include "System\RaptorDisplay.h"
#include "EMBMDisplay.h"
#include "Engine\ViewModifier.h"
#include "GLHierarchy\Object3DInstance.h"
#include "GLHierarchy\RenderingProperties.h"

static GL_COORD_VERTEX lpos;


class BackGround2 : public CSimpleObject
{
public:
	BackGround2():dt(0)
    { setBoundingBox(GL_COORD_VERTEX(-1.0f,-1.0f,-1.0f,1.0f),GL_COORD_VERTEX(1.0f,1.0f,1.0f,1.0f)); };        // We need a dummy BBox to be always visible

	virtual ~BackGround2() {};

	virtual void glRender();
	virtual void glClipRender() { glRender(); };

	float dt;
	CLight	*light;
};

void BackGround2::glRender()
{
	lpos.x = 15 * (float)(cos(2*PI*dt)*cos(4*PI*dt));
	lpos.y = 15 * (float)(sin(4*PI*dt)*cos(2*PI*dt));
	lpos.z = -20.0 + 15;

	light->setLightPosition(lpos);
//    light->glRender();
	
    glPushAttrib(GL_ENABLE_BIT);
	glDisable(GL_TEXTURE_2D);
    glDisable(GL_LIGHTING);
	glColor4f(1.0f,0.0f,0.0f,1.0f);
	glBegin(GL_LINES);
		glVertex3f(0.0f,0.0f,-20.0f);
		glVertex3f(lpos.x,lpos.y,lpos.z);
	glEnd();

	glPopAttrib();

	dt+=0.001f;
	if (dt>1.0) dt=0.0;
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEMBMDisplay::CEMBMDisplay()
	:embm(NULL)
{
	lpos.x=15.0;
	lpos.y=0.0;
	lpos.z=5.0;
}

CEMBMDisplay::~CEMBMDisplay()
{
    // Done at Raptor exit.
	//if (embm != NULL)
	//	delete embm;
}

void CEMBMDisplay::Init()
{
	CGenericDisplay::Init();

	BackGround2 *pbg = new BackGround2();

	CTextureFactory &f = CTextureFactory::getDefaultFactory();
	CPersistence *p = CPersistence::FindObject("main_textures");
	CTextureSet	*tf = (CTextureSet*)p;

	p = CPersistence::FindObject("Bump teapot");
	CBumppedGeometry *teapot = (CBumppedGeometry *)p;

	p = CPersistence::FindObject("Bump light");
	pbg->light = (CLight*)p;
	embm = new CEMBMGeometry();
    *embm = *teapot;

    CGeometry::CRenderingModel l_model(CGeometry::CRenderingModel::CGL_FRONT_GEOMETRY);
    l_model.addModel(CGeometry::CRenderingModel::CGL_NORMALS);
    l_model.addModel(CGeometry::CRenderingModel::CGL_TEXTURE);

	//embm->SetEnvironmentMap(reflect);
    embm->getShader()->setColor(0.2f,0.2f,0.2f,1.0f);
    embm->setEnvironmentMap(tf->getTexture(4));
	CTextureObject* normalMap = f.glCreateTexture(CTextureObject::CGL_COLOR24_ALPHA,CTextureObject::CGL_MULTIPLY,CTextureObject::CGL_BILINEAR);
	normalMap->glSetTransparency(255);
	f.glLoadTexture(normalMap,"Datas\\bump4.tga");
	embm->setNormalMap(normalMap);
    embm->setDiffuseMap(tf->getTexture(2));
	embm->setRenderingModel(l_model);

	CViewModifier *vm = new CViewModifier("EMBM_ANIMATOR");
	vm->setObject(embm);
	CModifier::TIME_FUNCTION tx;
	CModifier::TIME_FUNCTION ty;
	CModifier::TIME_FUNCTION tz;
	vm->addAction(CViewModifier::RESET_TRANSFORM,tx,ty,tz);

	tz.timeFunction = CModifier::CGL_TIME_CONSTANT;
	tz.a0 = -20;
	vm->addAction(CViewModifier::TRANSLATEABSOLUTE,tx,ty,tz);

	tz.a0 = 0;
	tx.a0 = 30.0f;
	vm->addAction(CViewModifier::ROTATE_VIEW,tx,ty,tz);

	tx.timeFunction = CModifier::CGL_TIME_COSINE;
	tx.a0 = 0.0;
	tx.a1 = 4.0;	// 1 radian ~ 60° / sec
	tx.a2 = 50.0;
	
	ty.timeFunction = CModifier::CGL_TIME_COSINE;
	ty.a0 = 0.0;
	ty.a1 = 3.0;
	ty.a2 = 100.0;
	
	tz.timeFunction = CModifier::CGL_TIME_COSINE;
	tz.a0 = 0.0;
	tz.a1 = 5.0;
	tz.a2 = 50.0;

	vm->addAction(CViewModifier::ROTATE_VIEW,tx,ty,tz);

	m_pScene = new C3DScene("EMBM_SCENE");
	//m_pScene->UseZSort();
	m_pScene->addLight(pbg->light);
	m_pScene->addObject(pbg);
	m_pScene->addObject(vm->getObject());

	CRaptorDisplay* pDisplay = CRaptorDisplay::GetCurrentDisplay();
	pDisplay->addScene(m_pScene);
}

void CEMBMDisplay::ReInit()
{
	CGenericDisplay::ReInit();

	glEnable(GL_LIGHT0);
	glDisable(GL_LIGHT1);

	CPersistence *p = CPersistence::FindObject("EMBM_ANIMATOR");
	if (p != NULL)
	{
		((CViewModifier*)p)->animate(true);
	}

	CRaptorDisplay* pDisplay = CRaptorDisplay::GetCurrentDisplay();
	pDisplay->selectScene("EMBM_SCENE");
	CRenderingProperties *rp = pDisplay->getRenderingProperties();
	rp->setTexturing(CRenderingProperties::ENABLE);
    rp->setLighting(CRenderingProperties::ENABLE);
}

void CEMBMDisplay::UnInit()
{
}


void CEMBMDisplay::Display()
{
	if (reinit)
		ReInit();
}

