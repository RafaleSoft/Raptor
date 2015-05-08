// SplineDisplay.cpp: implementation of the CSplineDisplay class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SplineDisplay.h"


#include "System\CGLTypes.h"
#include "Engine\3DPath.h"
#include "Engine\3DScene.h"
#include "GLHierarchy\GLFontFactory.h"
#include "GLHierarchy\GLFont.h"
#include "GLHierarchy\GL2DFont.h"
#include "GLHierarchy\GL3DFont.h"
#include "GLHierarchy\TextureSet.h"
#include "GLHierarchy\TextureFactory.h"
#include "Engine\ViewPoint.h"
#include "GLHierarchy\Shader.h"
#include "GLHierarchy\TextureUnitSetup.h"
#include "GLHierarchy\Material.h"
#include "System\RaptorDisplay.h"
#include "GLHierarchy\TextureObject.h"
#include "GLHierarchy\ShadedGeometry.h"
#include "GLHierarchy\RenderingProperties.h"


static GLfloat light_ambient[] = { 1.0f, 1.0f, 1.0f, 1.0f };
static GLfloat light_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
static GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSplineDisplay::CSplineDisplay():
path(NULL),bspline(NULL),font(NULL),vp(NULL)
{

}


CSplineDisplay::~CSplineDisplay()
{
}

void CSplineDisplay::Init()
{
	CGenericDisplay::Init();

	CPersistence *p = CPersistence::FindObject("main_textures");

	if (p->getId().isSubClassOf(CTextureSet::CTextureSetClassID::GetClassId()))
		 texture = (CTextureSet *)p;

	path = new C3DPath();
	path->setPathType(C3DPath::CGL_CATMULLROMEX);
	path->addKnot(-1.0f,-1.0f,-1.0f,0.0f,-1.0f);
	path->addKnot(0.0f,1.0f,1.0f,0.25f,-1.0f);
	path->addKnot(1.0f,-1.0f,0.0f,0.50f,-1.0f);
	path->addKnot(2.0f,1.0f,-2.0f,0.75f,-1.0f);
	path->addKnot(0.0f,2.0f,0.0f,1.0f,-1.0f);

	CShadedGeometry *sh = new CShadedGeometry();
	CShader *s = sh->getShader();
	CTextureUnitSetup *ts = s->glGetTextureUnitsSetup();
	ts->setDiffuseMap(texture->getTexture(1));
	CMaterial *m = s->getMaterial();
	m->setAmbient(0.1f,0.1f,0.1f,1.0f);
	m->setDiffuse(0.5f,0.5f,0.5f,1.0f);
	m->setSpecular(0.5f,0.5f,0.5f,1.0f);
	m->setShininess(50.0f);
    CGeometry::CRenderingModel l_model(CGeometry::CRenderingModel::CGL_FRONT_GEOMETRY);
    l_model.addModel(CGeometry::CRenderingModel::CGL_TEXTURE);
	sh->setRenderingModel(l_model);
	GLfloat ctrlpoints[16][4] = 
	{
		{-1.5,-1.5,4.0,1.0},{-0.5,-1.5,2.0,1.0},{0.5,-1.5,-1.0,1.0},{1.5,-1.5,2.0,1.0},
		{-1.5,-0.5,1.0,1.0},{-0.5,-0.5,3.0,1.0},{0.5,-0.5,0.0,1.0},{1.5,-0.5,-1.0,1.0},
		{-1.5,0.5,4.0,1.0},{-0.5,0.5,0.0,1.0},{0.5,0.5,3.0,1.0},{1.5,0.5,4.0,1.0},
		{-1.5,1.5,-2.0,1.0},{-0.5,1.5,-2.0,1.0},{0.5,1.5,0.0,1.0},{1.5,1.5,-1.0,1.0}
	};
	sh->glSetVertices(16);
	sh->glSetTexCoords(16);
	sh->glLockData();

	for (int i=0;i<16;i++)
		sh->addVertex(ctrlpoints[i][0],ctrlpoints[i][1],ctrlpoints[i][2],ctrlpoints[i][3]);
	for (int j=0;j<4;j++)
		for (int i=0;i<4;i++)
			sh->setTexCoord(i+j*4,i*1.0f/3.0f,j*1.0f/3.0f);
	CGeometryPrimitive *gp = sh->createPrimitive(CGeometryPrimitive::HIGH_ORDER_SURFACE);
	gp->setOrder(4,4,20,20);
	sh->glUnLockData();

	bspline = sh;

	CGL3DFont *font3d = CGLFontFactory::create3DFont("Datas\\kld.ttf", 20, true, true, "main_font_3d");
	font = font3d;
	font->glGenGlyphs(1, 5, 2.0f);
	text = (font3d->glWriteList("Raptor",0)).handle;
	font = CGLFontFactory::create2DFont("Datas\\kld.ttf", 20, "main_font");

	vp = new CViewPoint();
    vp->setPosition(5.0,0.0,5.0,CViewPoint::EYE);
    vp->setPosition(0.0,0.0,0.0,CViewPoint::TARGET);
	vp->setViewVolume(-1.33f,1.33f,-1.0f,1.0f,1.0f,10000,CViewPoint::PERSPECTIVE);

	C3DScene *pScene = new C3DScene("SPLINE_SCENE");
	pScene->addObject(bspline);

	CRaptorDisplay* pDisplay = CRaptorDisplay::GetCurrentDisplay();
	pDisplay->addScene(pScene);
}

void CSplineDisplay::ReInit()
{
	CGenericDisplay::ReInit();

	glEnable(GL_LIGHT0);
	glDisable(GL_LIGHT1);
	float position[4] = {0,10,20,1.0};

	glLightfv(GL_LIGHT0,GL_POSITION,position);
	glLightfv(GL_LIGHT0,GL_AMBIENT,light_ambient);
	glLightfv(GL_LIGHT0,GL_DIFFUSE,light_diffuse);
	glLightfv(GL_LIGHT0,GL_SPECULAR,light_specular);

	glPointSize(20.0);

	CRaptorDisplay* const pDisplay = CRaptorDisplay::GetCurrentDisplay();
	pDisplay->setViewPoint(vp);
    CRenderingProperties *rp = pDisplay->getRenderingProperties();
    rp->setTexturing(CRenderingProperties::ENABLE);
    rp->setLighting(CRenderingProperties::ENABLE);
    rp->setBlending(CRenderingProperties::DISABLE);

	pDisplay->selectScene("SPLINE_SCENE");
}

void CSplineDisplay::UnInit()
{
}

void CSplineDisplay::Display()
{
	if (reinit)
		ReInit();

    vp->setPosition((float)(5.0*cos(dt*2*PI)),0.0f,5.0f,CViewPoint::EYE);

	glPushMatrix();

	glTranslatef(3.0f,0.0f,2.0f);
    glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);

	glColor4f(1.0,1.0,1.0,1.0);
	glBegin(GL_LINE_STRIP);
		for (int i=0;i<100;i++)
		{
			float u = ((float)i)/100.0f;
			CGenericVector<float> &res = path->eval(u);
			glVertex3f(res.X(),res.Y(),res.Z());
		}
	glEnd();

	glColor4f(1.0,0.0,0.0,1.0);
	glBegin(GL_POINTS);
		CGenericVector<float> &res = path->eval(dt);
		glVertex3f(res.X(),res.Y(),res.Z()+0.001f);
	glEnd();

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

	glTranslatef(-40.0,20.0,-100.0);

	glColor4f(1.0,1.0,1.0,1.0);
	CTextureObject* T = texture->getTexture(1);
	T->glRender();
	glCallList(text);

	glPopMatrix();

	dt+=0.002f;
	if (dt>1.0) dt=0.0;
}