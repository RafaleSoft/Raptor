// Teapot.cpp: implementation of the CTeapot class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Test2.h"
#include "Teapot.h"


#include "TestEngine.h"

#include "SSE_Engine/SSE_BoundingBox.h"
#include "System/Raptor.h"
#include "System/RaptorMessages.h"
#include "Engine/Animator.h"
#include "Engine/ViewPoint.h"
#include "GLHierarchy/RenderingProperties.h"
#include "GLHierarchy/TextureSet.h"
#include "GLHierarchy/TextureFactory.h"
#include "GLHierarchy/TextureFactoryConfig.h"
#include "GLHierarchy/BumppedGeometry.h"
#include "GLHierarchy/3DSet.h"
#include "GLHierarchy/Shader.h"
#include "GLHierarchy/VertexShader.h"
#include "GLHierarchy/TextureObject.h"
#include "GLHierarchy/Material.h"

#include "ToolBox/RaptorToolBox.h"


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
	numdemo(CTest2App::BUMPDEMO)
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
/*
	LARGE_INTEGER	frequency;
	LARGE_INTEGER	l1,l2;
	if (!QueryPerformanceFrequency(&frequency))
		frequency.QuadPart=0;
	GL_COORD_VERTEX r_min;
	GL_COORD_VERTEX r_max;
	GL_MATRIX m;
	for (int i=0;i<16;i++) { m[i] = i; }
	CSSE_BoundingBox sseb;
	CBoundingBox bb;
	sseb.Set(-1,-1,-1,1,1,1);
	bb.Set(-1,-1,-1,1,1,1);
	QueryPerformanceCounter(&l1); 
	for (i=0;i<100000;i++)
	{
	//	bb.Get(r_min,r_max,m);
	//	sseb.Get(r_min,r_max,m);
   //     int tmu;
   //     glGetIntegerv(GL_TEXTURE_BINDING_2D,&tmu);
	}
	QueryPerformanceCounter(&l2);
	double nbl = 100000*((double)frequency.QuadPart)/(l2.QuadPart-l1.QuadPart);
	LONGLONG g1 = (l2.QuadPart-l1.QuadPart);
*/


	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

#if defined(GL_ARB_texture_compression)
	if (Raptor::glIsExtensionSupported("GL_ARB_texture_compression"))
	{
		glHint(GL_TEXTURE_COMPRESSION_HINT_ARB,GL_NICEST);
	}
#endif

	// point parameter settings
	if (Raptor::glIsExtensionSupported("GL_EXT_point_parameters"))
	{
		CVertexShader s;
		GL_COORD_VERTEX quadric(-40.0f, 0.0f, 0.1f, 1.0f);
		s.glPointParameter(0.0f,20.0,quadric);
	}

	CTextureFactory &f = CTextureFactory::getDefaultFactory();
	t = new CTextureSet("main_textures");

	CTextureObject*	T = f.glCreateTexture(CTextureObject::CGL_COLOR24_ALPHA,CTextureObject::CGL_ALPHA_TRANSPARENT,CTextureObject::CGL_BILINEAR);
	T->glSetTransparency(128);
	f.glLoadTexture(T,"Datas\\raptor.tga");
	//f.glLoadTexture(T,"Datas\\Mire.tga");
	t->addTexture(T);

	T = f.glCreateTexture(CTextureObject::CGL_COLOR24_ALPHA,CTextureObject::CGL_MULTIPLY,CTextureObject::CGL_BILINEAR);
	T->glSetTransparency(255);
	f.glLoadTexture(T,"Datas\\marble.jpg");
	t->addTexture(T);

#if defined(GL_ARB_texture_compression)
	#if(0)
        T = f.glCreateTexture(CTextureObject::CGL_COLOR24_ALPHA,CTextureObject::CGL_ALPHA_TRANSPARENT,CTextureObject::CGL_BILINEAR);
		T->glSetTransparency(128);
		f.glLoadCompressedTexture(T,"start.s3tc");
	#else
		T = f.glCreateTexture(CTextureObject::CGL_COLOR24_ALPHA,CTextureObject::CGL_ALPHA_TRANSPARENT,CTextureObject::CGL_BILINEAR);
		T->glSetTransparency(128);
		CTextureFactoryConfig& config = f.getConfig();
		const CTextureFactoryConfig::CCompressor *compressor = config.getCurrentCompressor();
		if (0 < config.getNumCompressors())
			config.setCurrentCompressor(config.getCompressor("OpenGL"));
		f.glLoadTexture(T,"Datas\\start.tga",CGL_USER_MIPMAPPED);
		t->addTexture(T);
	
	    if (Raptor::glIsExtensionSupported("GL_ARB_texture_compression"))
	    {
		    f.glExportCompressedTexture("start.s3tc",T);
	    }
    #endif
#else
	T = f.glCreateTexture(CTextureObject::CGL_COLOR24_ALPHA,CTextureObject::CGL_ALPHA_TRANSPARENT,CTextureObject::CGL_BILINEAR);
	T->glSetTransparency(128);
	f.glLoadTexture(T,"Datas\\start.tga");
	t->addTexture(T);
#endif

	T = f.glCreateTexture(CTextureObject::CGL_COLOR24_ALPHA,CTextureObject::CGL_ALPHA_TRANSPARENT,CTextureObject::CGL_BILINEAR);
	T->glSetTransparency(128);
	f.glLoadTexture(T,"Datas\\bump.tga");
	t->addTexture(T);

	if (Raptor::glIsExtensionSupported("GL_ARB_texture_cube_map"))
	{
		T = f.glCreateCubemap(CTextureObject::CGL_COLOR24_ALPHA,CTextureObject::CGL_ALPHA_TRANSPARENT,CTextureObject::CGL_BILINEAR);
		T->glSetTransparency(255);
		f.glLoadTexture(T,"Datas\\start.tga",CGL_USER_MIPMAPPED|CGL_CUBEMAP_PX);
		f.glLoadTexture(T,"Datas\\start.tga",CGL_USER_MIPMAPPED|CGL_CUBEMAP_PY);
		f.glLoadTexture(T,"Datas\\start.tga",CGL_USER_MIPMAPPED|CGL_CUBEMAP_PZ);
		f.glLoadTexture(T,"Datas\\start.tga",CGL_USER_MIPMAPPED|CGL_CUBEMAP_NX);
		f.glLoadTexture(T,"Datas\\start.tga",CGL_USER_MIPMAPPED|CGL_CUBEMAP_NY);
		f.glLoadTexture(T,"Datas\\start.tga",CGL_USER_MIPMAPPED|CGL_CUBEMAP_NZ);
		t->addTexture(T);
	}

	CRaptorDisplay *pDisplay = CRaptorDisplay::GetCurrentDisplay();
	CViewPoint *vp = pDisplay->getViewPoint();
	vp->setViewVolume(-1.33f,1.33f,-1.0f,1.0f,1.0f,10000,CViewPoint::PERSPECTIVE);
    vp->glRenderViewPointModel();   // switch demo removes the view  point, so render here and forget
	pDisplay->setViewPoint(vp);

	teapot = new CBumppedGeometry("Bump teapot");

    CRaptorToolBox::load3DStudioScene("Datas\\Teapot.3DS",set,NULL);

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
	CTextureObject* normalMap = f.glCreateTexture(CTextureObject::CGL_COLOR24_ALPHA,CTextureObject::CGL_MULTIPLY,CTextureObject::CGL_BILINEAR);
    f.glLoadTexture(normalMap,"Datas\\bump3.tga",CGL_USER_MIPMAPPED|CGL_CREATE_NORMAL_MAP);
	teapot->setNormalMap(normalMap);
	t->addTexture(normalMap);

    CGeometry::CRenderingModel l_model(CGeometry::CRenderingModel::CGL_FRONT_GEOMETRY);
    l_model.addModel(CGeometry::CRenderingModel::CGL_TEXTURE);
	teapot->setRenderingModel(l_model);
	if (!Raptor::glIsExtensionSupported("GL_ARB_vertex_program"))
		Raptor::GetMessages()->displayMessage("Hardware unable to render bump mapping");

	C3DEngine::Get3DEngine()->setCameraBBox(-1.0,-1.0,-1.0,1.0,1.0,1.0);

	CRaptorToolBox::SCENE_LOADER_OPTIONS options;
    CRaptorToolBox::load3DStudioScene("Datas\\Ball.3DS",set,&options);
    it = set->getIterator();
	CGeometry *ball = (CGeometry*)(set->getChild(it++));
	ball->scale(0.1f,0.1f,0.1f);
	ball->getCenter(c);
	ball->translate(-c.x,-c.y,-c.z);

    CRaptorToolBox::load3DStudioScene("Datas\\SkyDome.3DS",set,NULL);
    it = set->getIterator();
	CGeometry* g = (CGeometry*)(set->getChild(it++));
	g->setName("SKYDOME");

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

	collisionDisplay.Init();
	displays[CTest2App::COLLISIONDEMO] = &collisionDisplay;
	ambientOcclusionDisplay.Init();
	displays[CTest2App::AMBIENTOCCLUSIONDEMO] = &ambientOcclusionDisplay;

	//lodDisplay.Init();
	//vertexShadersDisplay.Init();

	//embmDisplay.Init();


	m_globalDisplay = CRaptorDisplay::GetCurrentDisplay();
	CRenderingProperties *props = m_globalDisplay->getRenderingProperties();
	props->setMultisampling(CRenderingProperties::ENABLE);

	SwitchDemo(numdemo);
}