/***************************************************************************/
/*                                                                         */
/*  Teapot.cpp                                                             */
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



#include "stdafx.h"
#include "Test2.h"
#include "Teapot.h"


#include "TestEngine.h"

#include "SSE_Engine/SSE_BoundingBox.h"
#include "System/Raptor.h"
#include "System/RaptorMessages.h"
#include "Engine/Animator.h"
#include "Engine/IViewPoint.h"
#include "GLHierarchy/IRenderingProperties.h"
#include "GLHierarchy/TextureSet.h"
#include "GLHierarchy/TextureFactory.h"
#include "GLHierarchy/TextureFactoryConfig.h"
#include "GLHierarchy/BumppedGeometry.h"
#include "GLHierarchy/3DSet.h"
#include "GLHierarchy/Shader.h"
#include "GLHierarchy/VertexProgram.h"
#include "GLHierarchy/ITextureObject.h"
#include "GLHierarchy/ShadedGeometry.h"
#include "GLHierarchy/GeometryEditor.h"
#include "GLHierarchy/Material.h"

#include "ToolBox/RaptorToolBox.h"
#include "ToolBox/Imaging/BumpmapLoader.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CTeapot, CGLDocument)

void CTeapot::SwitchDemo(unsigned int numDemo)
{ 
	// Stop animated objects
	CAnimator::GetAnimator()->animateAll(false);

	// Remove view point because almost
	// all displays do not need it
	m_globalDisplay->setViewPoint(NULL);

	stopdemo = numdemo;
	numdemo = numDemo; 
	SetTitle(CA2T(displays[numDemo]->getTitle()));

	displays[numDemo]->DoInit();
}

CTeapot::CTeapot()
    :rx(0),ry(0),rz(0),currentDisplay(NULL),
    stopdemo(-1),m_globalDisplay(NULL),
	//numdemo(CTest2App::PARTICLEDEMO)
	//numdemo(CTest2App::AMBIENTOCCLUSIONDEMO)
	//numdemo(CTest2App::BUMPDEMO)
	//numdemo(CTest2App::SHADOWMAPDEMO)
	numdemo(CTest2App::VRTXSHADERSDEMO)
	//numdemo(CTest2App::PROJECTIONDEMO)
	//numdemo(CTest2App::SPLINEDEMO)
{
}

CTeapot::~CTeapot()
{
    // All destructions are done at Raptor exit.
}

void CTeapot::GLDisplayFunc(int )
{
    if ((stopdemo >0) && (stopdemo != numdemo))
    {
        if (currentDisplay != NULL)
            currentDisplay->UnInit();
        stopdemo = -1;
    }

	currentDisplay = displays[numdemo];
	currentDisplay->Display();
}


void CTeapot::GLInitContext()
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

#if defined(GL_ARB_texture_compression)
	if (Raptor::glIsExtensionSupported(GL_ARB_TEXTURE_COMPRESSION_EXTENSION_NAME))
		glHint(GL_TEXTURE_COMPRESSION_HINT_ARB,GL_NICEST);
#endif

	// point parameter settings
	if (Raptor::glIsExtensionSupported(GL_EXT_POINT_PARAMETERS_EXTENSION_NAME))
	{
		CVertexProgram s;
		GL_COORD_VERTEX quadric(-40.0f, 0.0f, 0.1f, 1.0f);
		s.glPointParameter(0.0f,20.0,quadric);
	}

	CTextureFactory &f = CTextureFactory::glGetDefaultFactory();
	t = new CTextureSet("main_textures");
	CTextureFactoryConfig& config = f.getConfig();

	ITextureObject*	T = f.glCreateTexture(	ITextureObject::CGL_COLOR24_ALPHA,
											ITextureObject::CGL_BILINEAR);
	f.glSetTransparency(T, 128);
	f.glLoadTexture(T,"Datas\\raptor.tga");
	f.glExportTexture(T, "raptor.jpg");
	t->addTexture(T);

	T = f.glCreateTexture(	ITextureObject::CGL_COLOR24_ALPHA,
							ITextureObject::CGL_BILINEAR);
	f.glSetTransparency(T, 255);
	f.glLoadTexture(T,"Datas\\marble.jpg");
	t->addTexture(T);

	T = f.glCreateTexture(	ITextureObject::CGL_COLOR24_ALPHA,
							ITextureObject::CGL_BILINEAR);
	f.glSetTransparency(T, 128);
	t->addTexture(T);
#if defined(GL_ARB_texture_compression)
	#if(0)
		f.glLoadCompressedTexture(T,"start.s3tc");
	#else
		if (0 < config.getNumCompressors())
			config.setCurrentCompressor(config.getCompressor("OpenGL"));
		f.glLoadTexture(T,"Datas\\start.tga");
		if (Raptor::glIsExtensionSupported(GL_ARB_TEXTURE_COMPRESSION_EXTENSION_NAME))
		    f.glExportCompressedTexture("start.s3tc",T);
    #endif
#else
	f.glLoadTexture(T,"Datas\\start.tga");
#endif

	T = f.glCreateTexture(	ITextureObject::CGL_COLOR24_ALPHA,
							ITextureObject::CGL_BILINEAR);
	f.glSetTransparency(T, 128);
	f.glLoadTexture(T,"Datas\\bump.tga");
	t->addTexture(T);

	if (Raptor::glIsExtensionSupported(GL_ARB_TEXTURE_CUBE_MAP_EXTENSION_NAME))
	{
#if defined(GL_ARB_texture_compression)
		//const CTextureFactoryConfig::ICompressor *compressor = config.getCurrentCompressor();
		if (0 < config.getNumCompressors())
			config.setCurrentCompressor(config.getCompressor("OpenGL"));
#endif
		T = f.glCreateCubemap(ITextureObject::CGL_COLOR24_ALPHA, ITextureObject::CGL_BILINEAR);
		f.glSetTransparency(T, 255);
		T->selectCubeFace(ITextureObject::CGL_CUBEMAP_PX);
		f.glLoadTexture(T,"Datas\\ciel_07_small.jpg");
		T->selectCubeFace(ITextureObject::CGL_CUBEMAP_PY);
		f.glLoadTexture(T,"Datas\\ciel_07_small.jpg");
		T->selectCubeFace(ITextureObject::CGL_CUBEMAP_PZ);
		f.glLoadTexture(T,"Datas\\ciel_07_small.jpg");
		T->selectCubeFace(ITextureObject::CGL_CUBEMAP_NX);
		f.glLoadTexture(T,"Datas\\ciel_07_small.jpg");
		T->selectCubeFace(ITextureObject::CGL_CUBEMAP_NY);
		f.glLoadTexture(T,"Datas\\ciel_07_small.jpg");
		T->selectCubeFace(ITextureObject::CGL_CUBEMAP_NZ);
		f.glLoadTexture(T,"Datas\\ciel_07_small.jpg");
		T->selectCubeFace(ITextureObject::CGL_CUBEMAP_NONE);
		t->addTexture(T);
	}

	CRaptorDisplay *pDisplay = CRaptorDisplay::GetCurrentDisplay();
	IViewPoint *vp = pDisplay->getViewPoint();
	vp->setViewVolume(-1.33f,1.33f,-1.0f,1.0f,1.0f,10000,IViewPoint::PERSPECTIVE);
    vp->glvkRenderViewPointModel();   // switch demo removes the view  point, so render here and forget
	pDisplay->setViewPoint(vp);

	CRaptorToolBox::SCENE_LOADER_OPTIONS options;
	options.texturePath = "Datas";
	options.compressTextures = true;

	teapot = new CBumppedGeometry("Bump teapot");
    CRaptorToolBox::load3DStudioScene("Datas\\Teapot.3DS",set,&options);
	set->scale(0.5f,0.5f,0.5f);
    C3DSet::C3DSetIterator it = set->getIterator();
	object = (CGeometry*)(set->getChild(it++));
	GL_COORD_VERTEX c;
	object->getCenter(c);
	object->translate(-c.x,-c.y,-c.z);
	object->rotationX(-90.0f);
	*teapot = *object;

	CShader *s = teapot->getShader();
	s->setColor(1.0f,1.0f,1.0f,1.0f);
    s->getMaterial()->setShininess(10.0f);
	teapot->setDiffuseMap(t->getTexture(0));
	ITextureObject* normalMap = f.glCreateTexture(	ITextureObject::CGL_COLOR24_ALPHA,
													ITextureObject::CGL_BILINEAR);
	CBumpmapLoader loader(f.getConfig().getBumpAmplitude());
    f.glLoadTexture(normalMap,"Datas\\bump3.tga",&loader);
	teapot->setNormalMap(normalMap);
	t->addTexture(normalMap);
	teapot->setEnvironmentMap(t->getTexture("Datas\\ciel_07_small.jpg"));

	teapot->setRenderingModel(CGeometry::CGL_FRONT_GEOMETRY);
	teapot->addModel(CGeometry::CGL_TEXTURE);
	teapot->addModel(CGeometry::CGL_NORMALS);
	if (!Raptor::glIsExtensionSupported(GL_ARB_VERTEX_PROGRAM_EXTENSION_NAME))
		Raptor::GetMessages()->displayMessage("Hardware unable to render bump mapping");

	C3DEngine::Get3DEngine()->setCameraBBox(-1.0,-1.0,-1.0,1.0,1.0,1.0);

    CRaptorToolBox::load3DStudioScene("Datas\\Ball.3DS",set, &options);
    it = set->getIterator();
	CGeometry *ball = (CGeometry*)(set->getChild(it++));
	ball->scale(0.1f,0.1f,0.1f);
	ball->getCenter(c);
	ball->translate(-c.x,-c.y,-c.z);

	CRaptorToolBox::load3DStudioScene("Datas\\SkyDome.3DS", set, &options);
    it = set->getIterator();
	CGeometry* g = (CGeometry*)(set->getChild(it++));
	g->setName("SKYDOME");

	C3DSet *setknot = NULL;
	CRaptorToolBox::load3DStudioScene("Datas\\Knot.3DS", setknot, &options);
	it = setknot->getIterator();
	CShadedGeometry *knot = (CShadedGeometry*)setknot->getChild(it++);
	CShader *knotShader = knot->getShader();
	knot->scale(0.1f, 0.1f, 0.1f);
	knot->getEditor().scaleTexCoords(48.0f, 2.0f);
	knotShader->setColor(0.1f, 0.1f, 0.1f, 1.0f);
	knotShader->getMaterial()->setAmbient(0.5f, 0.5f, 0.5f, 1.0f);
	knotShader->getMaterial()->setDiffuse(0.7f, 0.7f, 0.7f, 1.0f);
	knotShader->getMaterial()->setSpecular(1.0f, 1.0f, 1.0f, 1.0f);
	knotShader->getMaterial()->setShininess(20.0f);


	bumpDisplay.Init();
	displays[CTest2App::BUMPDEMO] = &bumpDisplay;
	splineDisplay.Init();
	displays[CTest2App::SPLINEDEMO] = &splineDisplay;
	skinningDisplay.Init();
	displays[CTest2App::SKINNINGDEMO] = &skinningDisplay;
	particleDisplay.Init();
	displays[CTest2App::PARTICLEDEMO] = &particleDisplay;
	shadowDisplay.Init();
	displays[CTest2App::SHADOWDEMO] = &shadowDisplay;
	shadowMapDisplay.Init();
	displays[CTest2App::SHADOWMAPDEMO] = &shadowMapDisplay;
	warpingDisplay.Init();
	displays[CTest2App::WARPINGDEMO] = &warpingDisplay;
	projectionDisplay.Init();
	displays[CTest2App::PROJECTIONDEMO] = &projectionDisplay;
	lodDisplay.Init();
	displays[CTest2App::LODDEMO] = &lodDisplay;
	collisionDisplay.Init();
	displays[CTest2App::COLLISIONDEMO] = &collisionDisplay;
	ambientOcclusionDisplay.Init();
	displays[CTest2App::AMBIENTOCCLUSIONDEMO] = &ambientOcclusionDisplay;
	vertexShadersDisplay.Init();
	displays[CTest2App::VRTXSHADERSDEMO] = &vertexShadersDisplay;

	m_globalDisplay = CRaptorDisplay::GetCurrentDisplay();
	IRenderingProperties &props = m_globalDisplay->getRenderingProperties();
	props.setMultisampling(IRenderingProperties::ENABLE);

	SwitchDemo(numdemo);
}