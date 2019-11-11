/***************************************************************************/
/*                                                                         */
/*  ObjectStore.cpp                                                        */
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

#include "System/Raptor.h"
#include "GLHierarchy/3DSet.h"
#include "GLHierarchy/Material.h"
#include "GLHierarchy/Light.h"
#include "GLHierarchy/GLLod.h"
#include "GLHierarchy/Projector.h"
#include "GLHierarchy/TextureSet.h"
#include "GLHierarchy/TextureFactory.h"
#include "GLHierarchy/Geometry.h"
#include "GLHierarchy/BumppedGeometry.h"
#include "GLHierarchy/Object3DInstance.h"
#include "GLHierarchy/Object3DShadow.h"
#include "GLHierarchy/Particle.h"
#include "GLHierarchy/TextureUnitSetup.h"
#include "GLHierarchy/TextureObject.h"
#include "GLHierarchy/TextureFactoryConfig.h"
#include "GLHierarchy/Shader.h"
#include "GLHierarchy/FragmentProgram.h"
#include "GLHierarchy/ShadedGeometry.h"
#include "System/RaptorDisplay.h"
#include "Engine/3DScene.h"
#include "GLHierarchy/Object3DInstanceSet.h"
#include "Engine/ViewModifier.h"
#include "Engine/LightModifier.h"
#include "Engine/ParticleManager.h"
#include "GLHierarchy/IRenderingProperties.h"
#include "System/RaptorMessages.h"
#include "System/RaptorIO.h"
#include "GLHierarchy/LightObserver.h"
#include "GLHierarchy/GeometryEditor.h"
#include "GLHierarchy/ObjectFactory.h"

#include "ToolBox/RaptorToolBox.h"
#include "DataManager/RaptorDataManager.h"

#include "ObjectStore.h"

#include "strstream"


static std::string MODELSHIGH = "Datas\\Columns.3DS";
static std::string MODELSLOW = "Datas\\ColumnsLow.3DS";
static std::string COLUMNLOW = "Datas\\colonneLow.3DS";
static string MODELS;

static std::string MARBLE_0 = "Datas\\Marble.jpg";
static std::string MARBLE_1 = "Datas\\Marble2.jpg";
static std::string MARBLE_2 = "Datas\\Marble3.jpg";
static std::string MARBLE_3 = "Datas\\Marble4.jpg";
static std::string MARBLE_4 = "Datas\\Marble5.jpg";
static std::string MARBLE_5 = "Datas\\Marble6.jpg";
static std::string BRICKS_0 = "Datas\\BRICKS.jpg";
static std::string BRICKS_1 = "Datas\\Bricks2.jpg";
static std::string BRICKS_2 = "Datas\\Bricks3.jpg";
static std::string BRICKS_3 = "Datas\\Bricks4.jpg";
static std::string BRICKS_4 = "Datas\\Bricks5.jpg";
static std::string BRICKS_5 = "Datas\\Bricks6.jpg";
static std::string BRICKS_6 = "Datas\\Bricks7.jpg";
static std::string BRICKS_7 = "Datas\\Bricks8.jpg";
static std::string BRICKS_8 = "Datas\\Bricks9.jpg";
static std::string BRICKS_9 = "Datas\\Bricks10.jpg";
static std::string BUMP_0 = "Datas\\Sculpt.tga";
static std::string OLDWOOD_1 = "Datas\\OLDWOOD2.JPG";
static std::string FLARE_BASE = "Datas\\LensFlare2.tga";
static std::string ROCKSCULPT = "Datas\\lrock049.jpg";


#define MARBLE0 0
#define MARBLE1 1
#define MARBLE2 2
#define MARBLE3 3
#define MARBLE4 4
#define MARBLE5 5
#define BRICKS	6
#define SCULPT	7
#define FLAREBASE	8
#define WOOD	9

#define NB_STAIR_STEPS	50


static float    LIGHT_AMBIENT=0.0f;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CObjectStore::CObjectStore():
	m_column(NULL),
    m_columnLow(NULL),
	m_groundTile(NULL),
	m_groundTileLow(NULL),
	m_ground(NULL),
	m_autel(NULL),
	m_knot(NULL),
    m_projector(NULL),
	m_torch(NULL),
	m_pBumpKnot(NULL),
	m_knotShadow(NULL),
    m_groundInstance(NULL),
    m_pRoofShader(NULL),
    m_pTranslator(NULL)
{
	m_textures = new CTextureSet("DEMO_TEXTURE_SET");
	m_material = new CMaterial;
}

CObjectStore::~CObjectStore()
{
	if (m_textures != NULL)
		delete m_textures;
	if (m_material != NULL)
		delete m_material;
    if (m_projector != NULL)
        delete m_projector;
	if (m_knotShadow != NULL)
		delete m_knotShadow;
	if (m_pBumpKnot != NULL)
		delete m_pBumpKnot;
    if (m_groundInstance != NULL)
        delete m_groundInstance;
    if (m_pRoofShader != NULL)
        m_pRoofShader->releaseReference();
    if (m_pTranslator != NULL)
        delete m_pTranslator;
}

void CObjectStore::SetPerf(bool lowdef)
{
	if (lowdef)
	{
		MODELS = MODELSLOW;
	}
	else
	{
		MODELS = MODELSHIGH;
	}
}

bool CObjectStore::IsAColumn(CGeometry *&g)
{
	bool res = false;

	string name = g->getName().data();
    CShadedGeometry *geo = static_cast<CShadedGeometry*>(g);
	if (name == "Column")
	{
        const CGeometryEditor &pEditor = g->getEditor();
		pEditor.scaleTexCoords(4.0f,4.0f);

		*geo->getShader()->getMaterial() = *m_material;
		CTextureUnitSetup *tmuSetup = geo->getShader()->glGetTextureUnitsSetup();
		tmuSetup->setDiffuseMap(m_textures->getTexture(MARBLE4));
        tmuSetup->enableImageUnit(CTextureUnitSetup::IMAGE_UNIT_1,false);
		tmuSetup->enableImageUnit(CTextureUnitSetup::IMAGE_UNIT_2,false);
		tmuSetup->enableImageUnit(CTextureUnitSetup::IMAGE_UNIT_3,false);
		res = true;
	}
	else if (name == "ColumnBase")
	{
		CBumppedGeometry* bump = new CBumppedGeometry("BUMP_COLUMN");

		*bump = *g;
		delete g;
		g = bump;

        const CGeometryEditor &pEditor = g->getEditor();
		pEditor.scaleTexCoords(4.0f,1.0f);
		bump->getShader()->setColor(1.0f,1.0f,1.0f,1.0f);	// for ambient in shadows
        *bump->getShader()->getMaterial() = *m_material;
		bump->getShader()->getMaterial()->setShininess(4.0f);
		bump->setDiffuseMap(m_textures->getTexture(ROCKSCULPT));

		CTextureFactory &factory = CTextureFactory::getDefaultFactory();
		CTextureObject *normalMap = factory.glCreateTexture(ITextureObject::CGL_COLOR24_ALPHA,
                                                            CTextureObject::CGL_MULTIPLY,
                                                            ITextureObject::CGL_TRILINEAR);
		factory.glLoadTexture(normalMap,BUMP_0);
		bump->setNormalMap(normalMap);

        CGeometry::CRenderingModel l_model(0);
        l_model.addModel(CGeometry::CRenderingModel::CGL_TEXTURE);
        l_model.addModel(CGeometry::CRenderingModel::CGL_FRONT_GEOMETRY);
        bump->setRenderingModel(l_model);

		m_columnBump = bump;
		res = true;
	}
	else if (name == "ColumnTop2")
	{
		*geo->getShader()->getMaterial() = *m_material;
		CTextureUnitSetup *tmuSetup = geo->getShader()->glGetTextureUnitsSetup();
		tmuSetup->setDiffuseMap(m_textures->getTexture(MARBLE5));
		tmuSetup->enableImageUnit(CTextureUnitSetup::IMAGE_UNIT_2,false);
		res = true;
	}
	else if (name == "ColumnDow0")	// 3DSMax Sucks !!!
	{
		*geo->getShader()->getMaterial() = *m_material;
		CTextureUnitSetup *tmuSetup = geo->getShader()->glGetTextureUnitsSetup();
		tmuSetup->setDiffuseMap(m_textures->getTexture(MARBLE5));
		tmuSetup->enableImageUnit(CTextureUnitSetup::IMAGE_UNIT_2,false);
		res = true;
	}

	return res;
}

bool CObjectStore::loadColumn(void)
{
    bool res = false;

    CRaptorToolBox::SCENE_LOADER_OPTIONS options;
    options.texturePath = "Datas";

    CRaptorToolBox::load3DStudioScene(COLUMNLOW,m_columnLow,&options);
    m_columnLow->setName("ColumnLow_Set");
    C3DSet::C3DSetIterator it = m_columnLow->getIterator();
	CShadedGeometry *g = (CShadedGeometry*)(m_columnLow->getChild(it++));
	while (g != NULL)
	{
		CShadedGeometry *next = (CShadedGeometry*)(m_columnLow->getChild(it++));

        CGeometry::CRenderingModel l_model(CGeometry::CRenderingModel::CGL_FRONT_GEOMETRY);
        l_model.addModel(CGeometry::CRenderingModel::CGL_NORMALS);
        l_model.addModel(CGeometry::CRenderingModel::CGL_TEXTURE);
		g->setRenderingModel(l_model);

        const string& name = g->getName();
        if (name == "Column#2")
	    {
            const CGeometryEditor &pEditor = g->getEditor();
		    pEditor.scaleTexCoords(4.0f,4.0f);
		    *g->getShader()->getMaterial() = *m_material;
		    CTextureUnitSetup *tmuSetup = g->getShader()->glGetTextureUnitsSetup();
		    tmuSetup->setDiffuseMap(m_textures->getTexture(MARBLE4));
	    }
	    else if (name == "ColumnBase")
	    {
            const CGeometryEditor &pEditor = g->getEditor();
		    pEditor.scaleTexCoords(4.0f,1.0f);
            *g->getShader()->getMaterial() = *m_material;
			g->getShader()->getMaterial()->setShininess(4.0f);
			g->getShader()->getMaterial()->setDiffuse(0.6f,0.6f,0.6f,1.0f);
			g->getShader()->getMaterial()->setSpecular(0.4f,0.4f,0.4f,1.0f);
		    CTextureUnitSetup *tmuSetup = g->getShader()->glGetTextureUnitsSetup();
		    tmuSetup->setDiffuseMap(m_textures->getTexture(ROCKSCULPT));
	    }
	    else if (name == "ColumnTop2#2")
	    {
		    *g->getShader()->getMaterial() = *m_material;
		    CTextureUnitSetup *tmuSetup = g->getShader()->glGetTextureUnitsSetup();
		    tmuSetup->setDiffuseMap(m_textures->getTexture(MARBLE5));
	    }
	    else if (name == "ColumnDow0#2")	// 3DSMax Sucks !!!
	    {
		    *g->getShader()->getMaterial() = *m_material;
		    CTextureUnitSetup *tmuSetup = g->getShader()->glGetTextureUnitsSetup();
		    tmuSetup->setDiffuseMap(m_textures->getTexture(MARBLE5));
	    }

        g = next;
    }

    return res;
}

void CObjectStore::GenTexCoords(CGeometry *g)
{
	int nbVrtx = g->nbVertex();
	GL_VERTEX_DATA v;

	g->glSetTexCoords(nbVrtx);

    g->glLockData();

	for (int i=0;i<nbVrtx;i++)
	{
		g->getVertex(i,v);
		g->setTexCoord(i,0.05f * v.vertex.x,0.05f * v.vertex.y);
		g->setNormal(i,0.0f,0.0f,1.0f,1.0f);
	}

    g->glUnLockData();
}

bool CObjectStore::IsAGround(CGeometry *g)
{
	string name = g->getName().data();
    CShadedGeometry *geo = static_cast<CShadedGeometry*>(g);

	if ((name == "GStar") || (name == "GQuad") || (name == "GBorder") ||
		(name == "GBorder2") || (name == "GCorner"))
    {
        GenTexCoords(g);
        *geo->getShader()->getMaterial() = *m_material;

        CTextureUnitSetup *tmuSetup = geo->getShader()->glGetTextureUnitsSetup();
        tmuSetup->enableImageUnit(CTextureUnitSetup::IMAGE_UNIT_1,false);
		tmuSetup->enableImageUnit(CTextureUnitSetup::IMAGE_UNIT_2,false);
		tmuSetup->enableImageUnit(CTextureUnitSetup::IMAGE_UNIT_3,false);
//        tmuSetup->getTMUShader(CTextureUnitSetup::IMAGE_UNIT_3).shaderOperation = CGL_TEXTURE_GEN_PROJECTION;

        const CGeometryEditor &pEditor = g->getEditor();
        pEditor.scaleTexCoords(4.0f,4.0f);

		if (name == "GStar")
		    tmuSetup->setDiffuseMap(m_textures->getTexture(MARBLE2));
		else if (name == "GQuad")
		    tmuSetup->setDiffuseMap(m_textures->getTexture(MARBLE0));
		else if (name == "GBorder")	// 3DSMax Sucks !!!
		    tmuSetup->setDiffuseMap(m_textures->getTexture(MARBLE1));
		else if (name == "GBorder2")	// 3DSMax Sucks !!!
		    tmuSetup->setDiffuseMap(m_textures->getTexture(MARBLE1));
		else if (name == "GCorner")	// 3DSMax Sucks !!!
		    tmuSetup->setDiffuseMap(m_textures->getTexture(MARBLE3));

        return true;
    }
	else
		return false;
}


bool CObjectStore::IsAGroundLow(CGeometry *g)
{
	string name = g->getName().data();
    CShadedGeometry *geo = static_cast<CShadedGeometry*>(g);

    if (name.substr(0,6) == "Ground")
    {
        GenTexCoords(g);
        *geo->getShader()->getMaterial() = *m_material;

        CTextureUnitSetup *tmuSetup = geo->getShader()->glGetTextureUnitsSetup();
        tmuSetup->enableImageUnit(CTextureUnitSetup::IMAGE_UNIT_1,false);
		tmuSetup->enableImageUnit(CTextureUnitSetup::IMAGE_UNIT_2,false);
		tmuSetup->enableImageUnit(CTextureUnitSetup::IMAGE_UNIT_3,false);
//        tmuSetup->getTMUShader(CTextureUnitSetup::IMAGE_UNIT_3).shaderOperation = CGL_TEXTURE_GEN_PROJECTION;

        const CGeometryEditor &pEditor = g->getEditor();
        pEditor.scaleTexCoords(4.0f,4.0f);

	    if (name == "GroundStar")
		    tmuSetup->setDiffuseMap(m_textures->getTexture(MARBLE2));
	    else if (name == "GroundQuad")
		    tmuSetup->setDiffuseMap(m_textures->getTexture(MARBLE0));
	    else if (name == "GroundBord")	// 3DSMax Sucks !!!
		    tmuSetup->setDiffuseMap(m_textures->getTexture(MARBLE1));
	    else if (name == "GroundBor0")	// 3DSMax Sucks !!!
		    tmuSetup->setDiffuseMap(m_textures->getTexture(MARBLE1));
	    else if (name == "GroundCorn")	// 3DSMax Sucks !!!
		    tmuSetup->setDiffuseMap(m_textures->getTexture(MARBLE3));

        return true;
    }
	else
		return false;
}

bool CObjectStore::IsAStair(CGeometry *g)
{
	string name = g->getName().data();
    CShadedGeometry *geo = static_cast<CShadedGeometry*>(g);

	if (name == "StairPivot")
	{
		*geo->getShader()->getMaterial() = *m_material;
		CTextureUnitSetup *tmuSetup = geo->getShader()->glGetTextureUnitsSetup();
		tmuSetup->setDiffuseMap(m_textures->getTexture(BRICKS));
		return true;
	}
	else if (name.substr(0,9) == "StairStep")
	{
		GenTexCoords(geo);
		const CGeometryEditor &pEditor = geo->getEditor();
		pEditor.scaleTexCoords(4.0f,4.0f);
		*geo->getShader()->getMaterial() = *m_material;
		CTextureUnitSetup *tmuSetup = geo->getShader()->glGetTextureUnitsSetup();
		tmuSetup->setDiffuseMap(m_textures->getTexture(BRICKS));
		return true;
	}
	else if (name == "StairWall")
	{
		const CGeometryEditor &pEditor = geo->getEditor();
		pEditor.scaleTexCoords(4.0f,2.0f);
		pEditor.flipNormals();
		*geo->getShader()->getMaterial() = *m_material;
		CTextureUnitSetup *tmuSetup = geo->getShader()->glGetTextureUnitsSetup();
		tmuSetup->setDiffuseMap(m_textures->getTexture(BRICKS));
		return true;
	}
	else
		return false;
}

bool CObjectStore::IsARoof(CGeometry *g)
{
	string name = g->getName().data();
    CShadedGeometry *geo = static_cast<CShadedGeometry*>(g);

	if (name.substr(0,4) == "Roof")
	{
        geo->setShader(m_pRoofShader);
		return true;
	}
	else if (name == "TopRoof")
	{
        const CGeometryEditor &pEditor = g->getEditor();
		pEditor.scaleTexCoords(1.0f,4.0f);
        geo->setShader(m_pRoofShader);
	}
	
	return false;
}

void CObjectStore::LoadPack(void)
{
	CRaptorDataManager  *dataManager = CRaptorDataManager::GetInstance();
	if (dataManager != NULL)
	{
		//	Change package and erase previous files in case of updates
		dataManager->managePackage("Demo.pck");
		
		BRICKS_0 = dataManager->ExportFile("BRICKS.jpg");
		if (BRICKS_0.empty()) return;
		BRICKS_1 = dataManager->ExportFile("BRICKS2.jpg");
		if (BRICKS_1.empty()) return;
		BRICKS_2 = dataManager->ExportFile("BRICKS3.jpg");
		if (BRICKS_2.empty()) return;
		BRICKS_3 = dataManager->ExportFile("BRICKS4.jpg");
		if (BRICKS_3.empty()) return;
		BRICKS_4 = dataManager->ExportFile("BRICKS5.jpg");
		if (BRICKS_4.empty()) return;
		BRICKS_5 = dataManager->ExportFile("BRICKS6.jpg");
		if (BRICKS_5.empty()) return;
		BRICKS_6 = dataManager->ExportFile("BRICKS7.jpg");
		if (BRICKS_6.empty()) return;
		BRICKS_7 = dataManager->ExportFile("BRICKS8.jpg");
		if (BRICKS_7.empty()) return;
		BRICKS_8 = dataManager->ExportFile("BRICKS9.jpg");
		if (BRICKS_8.empty()) return;
		BRICKS_9 = dataManager->ExportFile("BRICKS10.jpg");
		if (BRICKS_9.empty()) return;

		COLUMNLOW = dataManager->ExportFile("colonneLow.3DS");
		if (COLUMNLOW.empty()) return;
		MODELSHIGH = dataManager->ExportFile("Columns.3DS");
		if (MODELSHIGH.empty()) return;
		MODELSLOW = dataManager->ExportFile("ColumnsLow.3DS");
		if (MODELSLOW.empty()) return;

		MARBLE_0 = dataManager->ExportFile("Marble.jpg");
		if (MARBLE_0.empty()) return;
		MARBLE_1 = dataManager->ExportFile("Marble2.jpg");
		if (MARBLE_1.empty()) return;
		MARBLE_2 = dataManager->ExportFile("Marble3.jpg");
		if (MARBLE_2.empty()) return;
		MARBLE_3 = dataManager->ExportFile("Marble4.jpg");
		if (MARBLE_3.empty()) return;
		MARBLE_4 = dataManager->ExportFile("Marble5.jpg");
		if (MARBLE_4.empty()) return;
		MARBLE_5 = dataManager->ExportFile("Marble6.jpg");
		if (MARBLE_5.empty()) return;

		BUMP_0 = dataManager->ExportFile("Sculpt.tga");
		if (BUMP_0.empty()) return;

		OLDWOOD_1 = dataManager->ExportFile("OLDWOOD2.JPG");
		if (OLDWOOD_1.empty()) return;

		FLARE_BASE = dataManager->ExportFile("LensFlare2.tga");
		if (FLARE_BASE.empty()) return;
		ROCKSCULPT = dataManager->ExportFile("lrock049.jpg");
		if (ROCKSCULPT.empty()) return;
	}
}


void CObjectStore::LoadModels(void)
{
	m_material->setAmbient(1.0f,1.0f,1.0f,1.0f);
	m_material->setDiffuse(0.7f,0.7f,0.7f,1.0f);
	m_material->setSpecular(0.9f,0.9f,0.9f,1.0f);
	m_material->setShininess(20.0f);

	CTextureFactory &factory = CTextureFactory::getDefaultFactory();
	CTextureFactoryConfig& config = factory.getConfig();
	if (0 < config.getNumCompressors())
	{
		const CTextureFactoryConfig::ICompressor *compressor = config.getCompressor("DXT1");
		if (compressor == NULL)
			compressor = config.getCompressor("OpenGL");

		config.setCurrentCompressor(compressor);
	}
	CTextureObject* T = NULL;
	T = factory.glCreateTexture(ITextureObject::CGL_COLOR24_ALPHA,
								CTextureObject::CGL_MULTIPLY,
								ITextureObject::CGL_BILINEAR);
	T->glSetTransparency(192);
    factory.glLoadTexture(T,MARBLE_0);
	m_textures->addTexture(T);

	T = factory.glCreateTexture(ITextureObject::CGL_COLOR24_ALPHA,
								CTextureObject::CGL_MULTIPLY,
								ITextureObject::CGL_BILINEAR);
	T->glSetTransparency(255);
	factory.glLoadTexture(T,MARBLE_1);
	m_textures->addTexture(T);

	T = factory.glCreateTexture(ITextureObject::CGL_COLOR24_ALPHA,
								CTextureObject::CGL_MULTIPLY,
								ITextureObject::CGL_BILINEAR);
	T->glSetTransparency(192);
	factory.glLoadTexture(T,MARBLE_2);
	m_textures->addTexture(T);

	T = factory.glCreateTexture(ITextureObject::CGL_COLOR24_ALPHA,
								CTextureObject::CGL_MULTIPLY,
								ITextureObject::CGL_BILINEAR);
	T->glSetTransparency(255);
	factory.glLoadTexture(T,MARBLE_3);
	m_textures->addTexture(T);

	T = factory.glCreateTexture(ITextureObject::CGL_COLOR24_ALPHA,
								CTextureObject::CGL_MULTIPLY,
								ITextureObject::CGL_BILINEAR);
	factory.glLoadTexture(T,MARBLE_4);
	m_textures->addTexture(T);

	T = factory.glCreateTexture(ITextureObject::CGL_COLOR24_ALPHA,
								CTextureObject::CGL_MULTIPLY,
								ITextureObject::CGL_BILINEAR);
	factory.glLoadTexture(T,MARBLE_5);
	m_textures->addTexture(T);
    
    config.setCurrentAnisotropy(16.0f);

	T = factory.glCreateTexture(ITextureObject::CGL_COLOR24_ALPHA,
								CTextureObject::CGL_MULTIPLY,
								ITextureObject::CGL_ANISOTROPIC);
	config.setGenerateMipmap(false);
	T->glSetTransparency(255);
	factory.glLoadTexture(T,BRICKS_0);
	T->selectMipMapLevel(1);
	factory.glLoadTexture(T,BRICKS_1);
	T->selectMipMapLevel(2);
	factory.glLoadTexture(T,BRICKS_2);
	T->selectMipMapLevel(3);
	factory.glLoadTexture(T,BRICKS_3);
	T->selectMipMapLevel(4);
	factory.glLoadTexture(T,BRICKS_4);
	T->selectMipMapLevel(5);
	factory.glLoadTexture(T,BRICKS_5);
	T->selectMipMapLevel(6);
	factory.glLoadTexture(T,BRICKS_6);
	T->selectMipMapLevel(7);
	factory.glLoadTexture(T,BRICKS_7);
	T->selectMipMapLevel(8);
	factory.glLoadTexture(T,BRICKS_8);
	T->selectMipMapLevel(9);
	factory.glLoadTexture(T,BRICKS_9);
	m_textures->addTexture(T);
	config.setGenerateMipmap(true);

	T = factory.glCreateTexture(ITextureObject::CGL_COLOR24_ALPHA,
								CTextureObject::CGL_MULTIPLY,
								ITextureObject::CGL_BILINEAR);
	factory.glLoadTexture(T,MARBLE_2);
	m_textures->addTexture(T);

	T = factory.glCreateTexture(ITextureObject::CGL_COLOR24_ALPHA,
								CTextureObject::CGL_MULTIPLY,
								ITextureObject::CGL_BILINEAR);
	factory.glLoadTexture(T,FLARE_BASE);
	m_textures->addTexture(T);

	T = factory.glCreateTexture(ITextureObject::CGL_COLOR24_ALPHA,
								CTextureObject::CGL_MULTIPLY, 
								ITextureObject::CGL_BILINEAR);
	T->glSetTransparency(255);
	factory.glLoadTexture(T,OLDWOOD_1);
	m_textures->addTexture(T);

    T = factory.glCreateTexture(ITextureObject::CGL_COLOR24_ALPHA,
								CTextureObject::CGL_MULTIPLY,
								ITextureObject::CGL_BILINEAR);
	factory.glLoadTexture(T,ROCKSCULPT);
	m_textures->addTexture(T);

    m_pRoofShader = new CShader("ROOF_SHADER");
    m_pRoofShader->setColor(0.2f,0.2f,0.2f,1.0f);
	*m_pRoofShader->getMaterial() = *m_material;
	CTextureUnitSetup *tmuSetup = m_pRoofShader->glGetTextureUnitsSetup();
	tmuSetup->setDiffuseMap(m_textures->getTexture(WOOD));
	tmuSetup->enableImageUnit(CTextureUnitSetup::IMAGE_UNIT_1,false);
	tmuSetup->enableImageUnit(CTextureUnitSetup::IMAGE_UNIT_2,false);
	tmuSetup->enableImageUnit(CTextureUnitSetup::IMAGE_UNIT_3,false);
//    tmuSetup->getTMUShader(CTextureUnitSetup::IMAGE_UNIT_3).shaderOperation = CGL_TEXTURE_GEN_PROJECTION;

    const char *discard[13] = {	"Autel2",
								"Autel21",
								"Autel01",
								"Autel0",
								"Autel11",
								"Autel1",
								"StairPivot",
								"StairStep",
								"StairStep1",
								"StairStep2",
								"StairStep3",
								"StairWall",
								NULL };
    CRaptorToolBox::SCENE_LOADER_OPTIONS options;
    options.discardGeometry = discard;
    options.texturePath = "Datas";

    C3DSet* root = NULL;
    CRaptorToolBox::load3DStudioScene(MODELS,root,&options);
	if (NULL == root)
		return;

	m_groundTile = new C3DSet("GoundTile_Set");
	m_groundTileLow = new C3DSet("GoundTile_SetLow");
	m_column = new C3DSet("Column_Set");
	m_stair = new C3DSet("Stair_Set");
	m_roof = new C3DSet("Roof_Set");

    GL_COORD_VERTEX min,max;
    root->scale(0.1f,0.1f,0.1f);

    C3DSet::C3DSetIterator it = root->getIterator();
	CGeometry *g = (CGeometry*)(root->getChild(it++));
	while (g != NULL)
	{
		CGeometry *next = (CGeometry*)(root->getChild(it++));
		
        CGeometry::CRenderingModel l_model(CGeometry::CRenderingModel::CGL_FRONT_GEOMETRY);
        l_model.addModel(CGeometry::CRenderingModel::CGL_NORMALS);
        l_model.addModel(CGeometry::CRenderingModel::CGL_TEXTURE);
		g->setRenderingModel(l_model);

		if (IsAColumn(g))
        {
            C3DSet::C3DSetIterator pos = m_column->getIterator();
			m_column->addChild(g,pos);
        }
		else if (IsAGround(g))
        {
            C3DSet::C3DSetIterator pos = m_groundTile->getIterator();
			m_groundTile->addChild(g,pos);
        }
		else if (IsAGroundLow(g))
        {
            C3DSet::C3DSetIterator pos = m_groundTileLow->getIterator();
			m_groundTileLow->addChild(g,pos);
        }
		else if (g->getName() == "Knot")
			m_knot = (static_cast<CShadedGeometry*>(g));
		else if (IsARoof(g))
        {
            C3DSet::C3DSetIterator pos = m_roof->getIterator();
			m_roof->addChild(g,pos);
        }
		else if (g->getName() == "TopRoof")
			m_topRoof = g;
        else if (g->getName() == "WallBloc")
        {
            CShadedGeometry *geo = static_cast<CShadedGeometry*>(g);
            CShader * const s = geo->getShader();
            *s->getMaterial() = *m_material;
            CTextureUnitSetup *wallTmuSetup = s->glGetTextureUnitsSetup();
			wallTmuSetup->setDiffuseMap(m_textures->getTexture(BRICKS));
            const CGeometryEditor &pEditor = g->getEditor();
		    pEditor.scaleTexCoords(2.0f,2.0f);
        }
        else if (g->getName() == "WallBack")
        {
            CShadedGeometry *geo = static_cast<CShadedGeometry*>(g);
            CShader * const s = geo->getShader();
            *s->getMaterial() = *m_material;
            CTextureUnitSetup *wallTmuSetup = s->glGetTextureUnitsSetup();
			wallTmuSetup->setDiffuseMap(m_textures->getTexture(BRICKS));
            const CGeometryEditor &pEditor = g->getEditor();
		    pEditor.scaleTexCoords(2.0f,2.0f);
        }

		g = next;
	}

	m_groundTile->translateAbsolute(0.0f,0.0f,0.0f);
	m_groundTile->rotationX(-90.0f);
	m_groundTileLow->translateAbsolute(0.0f,0.0f,0.0f);
	m_groundTileLow->rotationX(-90.0f);
	
	m_column->translateAbsolute(0.0f,0.0f,0.0f);
	m_column->scale(0.5f,0.5f,0.5f);
	m_column->getBoundingBox(min,max);
	m_column->translate(0.0f,-min.z,0.0f);
	m_column->rotationX(-90.0f);

	float t = -2.0f * min.z;
	m_roof->translateAbsolute(0.0f,0.0f,0.0f);
	m_roof->scale(0.75f,0.75f,0.75f);
	m_roof->getBoundingBox(min,max);
	m_roof->translate(0.0f,t - min.z,0.0f);
	m_roof->rotationX(-90.0f);
	t = t - 2.0f * min.z;
	m_topRoof->translateAbsolute(0.0f,0.0f,0.0f);
	m_topRoof->scale(0.5f,1.0f,0.5f);
	m_topRoof->getBoundingBox(min,max);
	m_topRoof->translate(0.0f,t - min.z,0.0f);
	m_topRoof->rotationX(-90.0f);

    loadColumn();
    m_columnLow->scale(0.05f,0.05f,0.05f);
    m_columnLow->translateAbsolute(0.0f,0.0f,0.0f);
	m_columnLow->rotationX(-90.0f);
    m_columnLow->getBoundingBox(min,max);
	m_columnLow->translate(0.0f,-min.y,0.0f);
}




void CObjectStore::BuildObjects(void)
{
    //
    //  Build the ground set
    //
    m_ground = new C3DSet("Ground_set");
    C3DSet::C3DSetIterator pos = m_ground->getIterator();
    unsigned int numTile = 0;
	for (int i=0;i<12;i++)
	{
		for (int j=0;j<12;j++)
		{
			//CObject3DInstance *inst = new CObject3DInstance(m_groundTile);
			CGLLod *inst = new CGLLod();
			inst->addLevel(0.0f,m_groundTile);
			inst->addLevel(35.0f, m_groundTileLow);
			inst->translate(5.5f*(i-6),0,5.5f*(j-6));

			m_ground->addChild(inst,pos);
		}
	}
    
	//
	//	The bumpped knot
	//
	CTextureFactory &factory = CTextureFactory::getDefaultFactory();
	m_pBumpKnot = new CBumppedGeometry("BUMP_KNOT");
	*m_pBumpKnot = *m_knot;
	m_pBumpKnot->getProperties().setClippingMethod(CObjectProperties::CLIP_BSPHERE);

    m_pBumpKnot->getShader()->getMaterial()->setAmbient(1.0f,1.0f,1.0f,1.0f);
    const CGeometryEditor &pEditor = m_pBumpKnot->getEditor();
	pEditor.scaleTexCoords(48.0f,2.0f);
	CTextureObject* normalMap = factory.glCreateTexture(ITextureObject::CGL_COLOR24_ALPHA,
														CTextureObject::CGL_MULTIPLY,
														ITextureObject::CGL_TRILINEAR);
	factory.glLoadTexture(normalMap,BUMP_0);
    m_pBumpKnot->setDiffuseMap(m_textures->getTexture(MARBLE5));
	m_pBumpKnot->setNormalMap(normalMap);
	
    CGeometry::CRenderingModel l_model(CGeometry::CRenderingModel::CGL_FRONT_GEOMETRY);
    l_model.addModel(CGeometry::CRenderingModel::CGL_NORMALS);
    l_model.addModel(CGeometry::CRenderingModel::CGL_TEXTURE);
	m_pBumpKnot->setRenderingModel(l_model);

	if (!Raptor::glIsExtensionSupported(GL_ARB_VERTEX_PROGRAM_EXTENSION_NAME))
    {
        CRaptorMessages * const msg = Raptor::GetMessages();
        msg->displayMessage("Hardware unable to render bump mapping");
    }

    //m_knotShadow = new CObject3DShadow(m_knot,CObject3DShadow::SHADOW_BOUNDING_VOLUME);
	//m_knotShadow->setShadowColor(255,180,40,64);
    //m_knotShadow->setShadowExtrusion(50.0f);
}


void CObjectStore::BuildScene(void)
{
    if (m_pTranslator == NULL)
        m_pTranslator = CRaptorIO::Create("XMLIO",CRaptorIO::DISK_READ,CRaptorIO::ASCII_XML);
    CRaptorDisplay * const pCurrentDisplay = CRaptorDisplay::GetCurrentDisplay();

	//char shemaLocation[MAX_PATH];
	stringstream schemaLocation;
	schemaLocation << getenv("RAPTOR_ROOT");
	schemaLocation << "/Redist/bin/Raptor.xsd";
	m_pTranslator->parse(schemaLocation.str().c_str(), 0);
    m_pTranslator->parse("Demo.xml",0);

    string name;
	*m_pTranslator >> name;
    string data = m_pTranslator->getValueName();

    //  skip data intro
    *m_pTranslator  >> name; 
    data = m_pTranslator->getValueName();

    while (!data.empty())
    {
        CPersistence* obj = NULL;

        if (data == "Update")
        {
            *m_pTranslator >> name;
            data = m_pTranslator->getValueName();
            if (data == "name")
            {
                *m_pTranslator >> name;
                obj = CPersistence::FindObject(name);
            }
        }
        else
        {
            const CPersistentObject & po = CObjectFactory::GetInstance()->createObject(data);
            obj = po;
        }
        if (obj != NULL)
            obj->importObject(*m_pTranslator);
        else
            *m_pTranslator >> name;

        data = m_pTranslator->getValueName();
	}

    m_pLights[0] = (CLight*)(CPersistence::FindObject("mainlight"));
    for (unsigned int i=0;i<8;i++)
    {
        ostrstream lname;
        lname << "light" << i << ends;
        CLight *pLight = (CLight*)(CPersistence::FindObject(lname.str()));
        m_pLights[i+1] = pLight;
    }

    delete m_pTranslator;
}

