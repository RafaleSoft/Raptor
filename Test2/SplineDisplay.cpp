// SplineDisplay.cpp: implementation of the CSplineDisplay class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SplineDisplay.h"


#include "System/CGLTypes.h"
#include "Engine/3DPath.h"
#include "Engine/3DScene.h"
#include "GLHierarchy/GLFontFactory.h"
#include "GLHierarchy/GLFont.h"
#include "GLHierarchy/GL2DFont.h"
#include "GLHierarchy/GL3DFont.h"
#include "GLHierarchy/Light.h"
#include "GLHierarchy/TextureSet.h"
#include "GLHierarchy/TextureFactory.h"
#include "Engine/IViewPoint.h"
#include "GLHierarchy/Shader.h"
#include "GLHierarchy/TextureUnitSetup.h"
#include "GLHierarchy/Material.h"
#include "System/RaptorDisplay.h"
#include "GLHierarchy/TextureObject.h"
#include "GLHierarchy/ShadedGeometry.h"
#include "GLHierarchy/IRenderingProperties.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSplineDisplay::CSplineDisplay():
path(NULL), bspline(NULL), font(NULL), vp(NULL), m_pLight(NULL)
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

	bspline = new CShadedGeometry();
	CShader *s = bspline->getShader();
	CTextureUnitSetup *ts = s->glGetTextureUnitsSetup();
	ts->setDiffuseMap(texture->getTexture(1));
	CMaterial *m = s->getMaterial();
	m->setAmbient(0.1f,0.1f,0.1f,1.0f);
	m->setDiffuse(0.5f,0.5f,0.5f,1.0f);
	m->setSpecular(0.5f,0.5f,0.5f,1.0f);
	m->setShininess(50.0f);
	bspline->setRenderingModel(CGeometry::CRenderingModel::CGL_FRONT_GEOMETRY);
	bspline->addModel(CGeometry::CRenderingModel::CGL_TEXTURE);
	bspline->addModel(CGeometry::CRenderingModel::CGL_NORMALS);
	GLfloat ctrlpoints[16][4] = 
	{
		{-1.5,-1.5,4.0,1.0},{-0.5,-1.5,2.0,1.0},{0.5,-1.5,-1.0,1.0},{1.5,-1.5,2.0,1.0},
		{-1.5,-0.5,1.0,1.0},{-0.5,-0.5,3.0,1.0},{0.5,-0.5,0.0,1.0},{1.5,-0.5,-1.0,1.0},
		{-1.5,0.5,4.0,1.0},{-0.5,0.5,0.0,1.0},{0.5,0.5,3.0,1.0},{1.5,0.5,4.0,1.0},
		{-1.5,1.5,-2.0,1.0},{-0.5,1.5,-2.0,1.0},{0.5,1.5,0.0,1.0},{1.5,1.5,-1.0,1.0}
	};
	bspline->glSetVertices(16);
	bspline->glSetTexCoords(16);
	bspline->glLockData();

	for (int i=0;i<16;i++)
		bspline->addVertex(ctrlpoints[i][0], ctrlpoints[i][1], ctrlpoints[i][2], ctrlpoints[i][3]);
	for (int j=0;j<4;j++)
		for (int i=0;i<4;i++)
			bspline->setTexCoord(i + j * 4, i*1.0f / 3.0f, j*1.0f / 3.0f);
	CGeometryPrimitive *gp = bspline->createPrimitive(CGeometryPrimitive::HIGH_ORDER_SURFACE);
	gp->setOrder(4,4,20,20);
	bspline->glUnLockData();

	CGL3DFont *font3d = CGLFontFactory::create3DFont("Datas\\kld.ttf", 20, true, true, "main_font_3d");
	font = font3d;
	font->glGenGlyphs(1, 5, 2.0f);
	text = (font3d->glWriteList("Raptor",0)).handle();

	CRaptorDisplay* pDisplay = CRaptorDisplay::GetCurrentDisplay();
	vp = pDisplay->createViewPoint();
    vp->setPosition(5.0,0.0,5.0,IViewPoint::EYE);
    vp->setPosition(0.0,0.0,0.0,IViewPoint::TARGET);
	vp->setViewVolume(-1.33f,1.33f,-1.0f,1.0f,1.0f,10000,IViewPoint::PERSPECTIVE);

	m_pLight = new CLight("Spline light");
	m_pLight->setAmbient(1.0f, 1.0f, 1.0f, 1.0f);
	m_pLight->setDiffuse(1.0f, 1.0f, 1.0f, 1.0f);
	m_pLight->setSpecular(1.0f, 1.0f, 1.0f, 1.0f);
	m_pLight->setLightDirection(GL_COORD_VERTEX(0, 0, -1, 1));
	m_pLight->setLightPosition(GL_COORD_VERTEX(0, 10, 20, 1));

	C3DScene *pScene = new C3DScene("SPLINE_SCENE");
	pScene->addObject(bspline);
	pScene->addLight(m_pLight);

	pDisplay->addScene(pScene);
}

void CSplineDisplay::ReInit()
{
	CGenericDisplay::ReInit();
	
	CRaptorDisplay* const pDisplay = CRaptorDisplay::GetCurrentDisplay();
	pDisplay->setViewPoint(vp);
	
	IRenderingProperties &rp = pDisplay->getRenderingProperties();
	rp.setTexturing(IRenderingProperties::ENABLE);
	rp.setLighting(IRenderingProperties::ENABLE);
	rp.setBlending(IRenderingProperties::DISABLE);
	
	pDisplay->selectScene("SPLINE_SCENE");
}

void CSplineDisplay::UnInit()
{
}

void CSplineDisplay::Display()
{
	if (reinit)
		ReInit();
	
	float dt = CTimeObject::GetGlobalTime();
    vp->setPosition((float)(5.0*cos(dt*2*PI)),0.0f,5.0f,IViewPoint::EYE);
	
	glPushMatrix();
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	
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

	glPointSize(50.0);
	glColor4f(1.0,0.0,0.0,1.0);
	glBegin(GL_POINTS);
		CGenericVector<float> &res = path->eval(dt - floor(dt));
		glVertex3f(res.X(),res.Y(),res.Z()+0.001f);
	glEnd();
	glPointSize(1.0);
	
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
	
	glTranslatef(-40.0,20.0,-100.0);

	CTextureObject* T = texture->getTexture(1);
	T->glvkRender();
	glCallList(text);
	
	glPopMatrix();
	glPopAttrib();
}