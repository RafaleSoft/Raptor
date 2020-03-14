// LodDisplay.cpp: implementation of the CLodDisplay class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "LodDisplay.h"

#include "Engine/IViewPoint.h"
#include "GLHierarchy/GLFont.h"
#include "GLHierarchy/GLLod.h"
#include "GLHierarchy/Material.h"
#include "GLHierarchy/SimpleObject.h"
#include "System/Raptor.h"
#include "GLHierarchy/3DSet.h"
#include "GLHierarchy/ShadedGeometry.h"
#include "SSE_Engine/SSE_GLLayer.h"
#include "GLHierarchy/TextureFactory.h"	// for cast
#include "GLHierarchy/TextureUnitSetup.h"
#include "GLHierarchy/Shader.h"
#include "System/RaptorDisplay.h"
#include "Engine/3DScene.h"
#include "GLHierarchy/IRenderingProperties.h"
#include "GLHierarchy/GLFontFactory.h"

#include "ToolBox/RaptorToolBox.h"


static GLfloat light_ambient[] = { 1.0f, 1.0f, 1.0f, 1.0f };
static GLfloat light_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
static GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };


class CCountFaces : public CSimpleObject
{
public:
	CCountFaces(CGLLod* pLod)
		:m_pLod(pLod)
	{
		CPersistence *p = CPersistence::FindObject("main_font");
		if (p->getId().isSubClassOf(CGLFont::CGLFontClassID::GetClassId()))
			 font = (CGL2DFont *)p;
		m_pLayer = new CSSE_GLLayer(400,10,200,100);
		m_pLayer->clear(0xC0800000);

		setBoundingBox(GL_COORD_VERTEX(-1.0f,-1.0f,-1.0f,1.0f),
					   GL_COORD_VERTEX(1.0f,1.0f,1.0f,1.0f));
	};

	virtual void glRender()
	{
		unsigned int nbFaces = 0;
		unsigned int nbVertex = 0;

		C3DSet *set = (C3DSet *)(m_pLod->getObject());

		C3DSet::C3DSetIterator it = set->getIterator();
		CGeometry *g = (CGeometry*)(set->getChild(it++));
		
		while (g != NULL)
		{
			nbFaces += g->nbFace();
			nbVertex += g->nbVertex();

			g = (CGeometry*)(set->getChild(it++));
		}

		m_pLayer->clear(0xC0800000);
		m_pLayer->drawText(5,70,"LOD info",font,0xC0FFFF00);

		stringstream face_str;
		face_str << "Faces: " << nbFaces;
		m_pLayer->drawText(5,40,face_str.str(),font,0xC0FFFF00);
		stringstream vrtx_str;
		vrtx_str << "Vertex: " << nbVertex;
		m_pLayer->drawText(5, 10, vrtx_str.str(), font, 0xC0FFFF00);

		m_pLayer->glRender();
	};

	virtual void glClipRender()
	{
		glRender();
	};

private:
	CGLLod* m_pLod;
	CGLLayer* m_pLayer;
	CGL2DFont	*font;
};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLodDisplay::CLodDisplay():
	lod(NULL),material(NULL)
{
}

CLodDisplay::~CLodDisplay()
{
    // Done at Raptor exit.
    /*
	if (lod != NULL)
		delete lod;
	if (layer != NULL)
		delete layer;
    */
}


void CLodDisplay::Init()
{
	CGenericDisplay::Init();

	CShadedGeometry *g = NULL;
	C3DSet *set = NULL;
	lod = new CGLLod(NULL,"PLANE_LOD");

	CShader *s = new CShader;
	material = s->getMaterial();
	material->setAmbient(0.1f,0.1f,0.1f,1.0f);
	material->setDiffuse(0.8f,0.8f,0.8f,1.0f);
	material->setSpecular(1.0f,1.0f,1.0f,1.0f);
	material->setShininess(10.0f);

	//	Load level 0
    CRaptorToolBox::SCENE_LOADER_OPTIONS options;
	options.useMaxTransform = true;

    CRaptorToolBox::load3DStudioScene("Datas\\Zipplane.3DS",set,&options);
	set->translateAbsolute(0.0f,0.0f,0.0f);

    C3DSet::C3DSetIterator it = set->getIterator();
	g = (CShadedGeometry*)(set->getChild(it++));
	while (g != NULL)
	{
		g->setShader(s);
		g->setRenderingModel(CGeometry::CRenderingModel::CGL_FRONT_GEOMETRY);
		g->addModel(CGeometry::CRenderingModel::CGL_NORMALS);
		g = (CShadedGeometry*)(set->getChild(it++));
	}
	lod->addLevel(0,set);

	//	Load level 1
    CRaptorToolBox::load3DStudioScene("Datas\\Zipplane2.3DS",set,&options);
	set->translateAbsolute(0.0f,0.0f,0.0f);

    it = set->getIterator();
	g = (CShadedGeometry*)(set->getChild(it++));
	while (g != NULL)
	{
		g->setShader(s);
		g->setRenderingModel(CGeometry::CRenderingModel::CGL_FRONT_GEOMETRY);
		g->addModel(CGeometry::CRenderingModel::CGL_NORMALS);
		g = (CShadedGeometry*)(set->getChild(it++));
	}
	lod->addLevel(600,set);

	//	Load level 2
    CRaptorToolBox::load3DStudioScene("Datas\\Zipplane3.3DS",set,&options);
	set->translateAbsolute(0.0f,0.0f,0.0f);

    it = set->getIterator();
	g = (CShadedGeometry*)(set->getChild(it++));
	while (g != NULL)
	{
		g->setShader(s);
		g->setRenderingModel(CGeometry::CRenderingModel::CGL_FRONT_GEOMETRY);
		g->addModel(CGeometry::CRenderingModel::CGL_NORMALS);
		g = (CShadedGeometry*)(set->getChild(it++));
	}
	lod->addLevel(900,set);

	//	Load level 3
    CRaptorToolBox::load3DStudioScene("Datas\\Zipplane4.3DS",set,&options);
	set->translateAbsolute(0.0f,0.0f,0.0f);

    it = set->getIterator();
	g = (CShadedGeometry*)(set->getChild(it++));
	while (g != NULL)
	{
		g->setShader(s);
		g->setRenderingModel(CGeometry::CRenderingModel::CGL_FRONT_GEOMETRY);
		g->addModel(CGeometry::CRenderingModel::CGL_NORMALS);
		g = (CShadedGeometry*)(set->getChild(it++));
	}
	lod->addLevel(1200,set);

	//	Load level 4
    CRaptorToolBox::load3DStudioScene("Datas\\Zipplane5.3DS",set,&options);
	set->translateAbsolute(0.0f,0.0f,0.0f);

    it = set->getIterator();
	g = (CShadedGeometry*)(set->getChild(it++));
	while (g != NULL)
	{
		g->setShader(s);
		g->setRenderingModel(CGeometry::CRenderingModel::CGL_FRONT_GEOMETRY);
		g->addModel(CGeometry::CRenderingModel::CGL_NORMALS);
		g = (CShadedGeometry*)(set->getChild(it++));
	}
	lod->addLevel(1600,set);

	lod->rotationY(-60.0f);
	lod->rotationX(60.0f);
	lod->translateAbsolute(0.0f,0.0f,-1300.0f);

	CRaptorDisplay* pDisplay = CRaptorDisplay::GetCurrentDisplay();
	vp = pDisplay->createViewPoint();
    vp->setPosition(0.0,0.0,5.0,IViewPoint::EYE);
    vp->setPosition(0.0,0.0,0.0,IViewPoint::TARGET);
	vp->setViewVolume(-1.33f,1.33f,-1.0f,1.0f,1.0f,10000,IViewPoint::PERSPECTIVE);

	C3DScene *pScene = new C3DScene("LOD_SCENE");
	pScene->addObject(lod);
	CCountFaces *countFaces = new CCountFaces(lod);
	pScene->addObject(countFaces);
		
	pDisplay->addScene(pScene);
}

void CLodDisplay::ReInit()
{
	CGenericDisplay::ReInit();

	glEnable(GL_LIGHT0);
	glDisable(GL_LIGHT1);
	float position[4] = {0,10,20,1.0};

	glLightfv(GL_LIGHT0,GL_POSITION,position);
	glLightfv(GL_LIGHT0,GL_AMBIENT,light_ambient);
	glLightfv(GL_LIGHT0,GL_DIFFUSE,light_diffuse);
	glLightfv(GL_LIGHT0,GL_SPECULAR,light_specular);
	glColor4f(1.0f,1.0f,1.0f,1.0f);

    CRaptorDisplay* const pDisplay = CRaptorDisplay::GetCurrentDisplay();
	pDisplay->setViewPoint(vp);
	IRenderingProperties &rp = pDisplay->getRenderingProperties();
	rp.setTexturing(IRenderingProperties::DISABLE);
	rp.setLighting(IRenderingProperties::ENABLE);
	rp.setCullFace(IRenderingProperties::DISABLE);

	pDisplay->selectScene("LOD_SCENE");
}

void CLodDisplay::UnInit()
{
}

void CLodDisplay::Display()
{
	if (reinit)
		ReInit();

	float dt = CTimeObject::GetGlobalTime();

    glEnable(GL_LIGHT0);

	lod->translateAbsolute(0.0f,0.0f,-1300.0f + cos(TWO_PI * dt) * 1000);
}