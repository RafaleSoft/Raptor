// LodDisplay.cpp: implementation of the CLodDisplay class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "LodDisplay.h"

#include "GLHierarchy/GLFont.h"
#include "GLHierarchy\GLLod.h"
#include "GLHierarchy\Material.h"
#include "System\Raptor.h"
#include "GLHierarchy\3DSet.h"
#include "GLHierarchy\ShadedGeometry.h"
#include "GLHierarchy\GLLayer.h"
#include "GLHierarchy\TextureFactory.h"	// for cast
#include "GLHierarchy\TextureUnitSetup.h"
#include "GLHierarchy\Shader.h"
#include "System\RaptorDisplay.h"
#include "Engine\3DScene.h"
#include "GLHierarchy\RenderingProperties.h"

#include "..\RaptorToolBox\RaptorToolBox.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLodDisplay::CLodDisplay():
	lod(NULL),material(NULL),layer(NULL),font(NULL)
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

	CPersistence *p = CPersistence::FindObject("main_font");

	if (p->getId().isSubClassOf(CGLFont::CGLFontClassID::GetClassId()))
		 font = (CGLFont *)p;

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

    CGeometry::CRenderingModel l_model(CGeometry::CRenderingModel::CGL_FRONT_GEOMETRY);
    l_model.addModel(CGeometry::CRenderingModel::CGL_NORMALS);

    C3DSet::C3DSetIterator it = set->getIterator();
	g = (CShadedGeometry*)(set->getChild(it++));
	while (g != NULL)
	{
		g->setShader(s);
		g->setRenderingModel(l_model);

		g = (CShadedGeometry*)(set->getChild(it++));
	}
	lod->addLevel(0,set);

	//	Load level 1
    CRaptorToolBox::load3DStudioScene("Datas\\Zipplane2.3DS",set,&options);

    it = set->getIterator();
	g = (CShadedGeometry*)(set->getChild(it++));
	while (g != NULL)
	{
		g->setShader(s);
		g->setRenderingModel(l_model);
		g = (CShadedGeometry*)(set->getChild(it++));
	}
	lod->addLevel(-600,set);

	//	Load level 2
    CRaptorToolBox::load3DStudioScene("Datas\\Zipplane3.3DS",set,&options);

    it = set->getIterator();
	g = (CShadedGeometry*)(set->getChild(it++));
	while (g != NULL)
	{
		g->setShader(s);
		g->setRenderingModel(l_model);
		g = (CShadedGeometry*)(set->getChild(it++));
	}
	lod->addLevel(-900,set);

	//	Load level 3
    CRaptorToolBox::load3DStudioScene("Datas\\Zipplane4.3DS",set,&options);

    it = set->getIterator();
	g = (CShadedGeometry*)(set->getChild(it++));
	while (g != NULL)
	{
		g->setShader(s);
		g->setRenderingModel(l_model);
		g = (CShadedGeometry*)(set->getChild(it++));
	}
	lod->addLevel(-1200,set);

	//	Load level 4
    CRaptorToolBox::load3DStudioScene("Datas\\Zipplane5.3DS",set,&options);

    it = set->getIterator();
	g = (CShadedGeometry*)(set->getChild(it++));
	while (g != NULL)
	{
		g->setShader(s);
		g->setRenderingModel(l_model);
		g = (CShadedGeometry*)(set->getChild(it++));
	}
	lod->addLevel(-1600,set);

	layer = new CGLLayer(400,10,200,100);
	layer->clear(0xC0800000);
}

void CLodDisplay::ReInit()
{
	CGenericDisplay::ReInit();

	glColor4f(1.0f,1.0f,1.0f,1.0f);

    CRaptorDisplay* const pDisplay = CRaptorDisplay::GetCurrentDisplay();
    CRenderingProperties *rp = pDisplay->getRenderingProperties();
    rp->setTexturing(CRenderingProperties::DISABLE);
    rp->setLighting(CRenderingProperties::ENABLE);
    rp->setCullFace(CRenderingProperties::DISABLE);
}

void CLodDisplay::UnInit()
{
}

void CLodDisplay::CountFaces(void)
{
	unsigned int nbFaces = 0;
	unsigned int nbVertex = 0;

	C3DSet *set = (C3DSet *)(lod->getObject());

    C3DSet::C3DSetIterator it = set->getIterator();
	CGeometry *g = (CGeometry*)(set->getChild(it++));
	
	while (g != NULL)
	{
		nbFaces += g->nbFace();
		nbVertex += g->nbVertex();

		g = (CGeometry*)(set->getChild(it++));
	}

	layer->clear(0xC0800000);

	layer->drawText(5,70,"LOD info",font,0xC0FFFF00);

    char text[32];
	sprintf(text,"Faces: %u",nbFaces);
	layer->drawText(5,40,text,font,0xC0FFFF00);
	sprintf(text,"Vertex: %u",nbVertex);
	layer->drawText(5,10,text,font,0xC0FFFF00);
}

void CLodDisplay::Display()
{
	if (reinit)
		ReInit();

    glEnable(GL_LIGHT0);

	GLfloat viewport[4];
	glGetFloatv(GL_VIEWPORT,viewport);

	glPushMatrix();
		glTranslatef(0.0f,0.0f,-1300.0f + cos(TWO_PI * dt) * 1000);
		glRotatef(60.0f,0.0f,1.0f,0.0f);
		glRotatef(-60.0f,1.0f,0.0f,0.0f);

		lod->glRender();
	glPopMatrix();

	CountFaces();
	layer->glRender();

	dt+=0.002f;
	if (dt>1.0) dt=0.0;
}