#include "StdAfx.h"
#include "Test5Doc.h"
#include "System/Raptor.h"
#include "System/RaptorConfig.h"
#include "Engine/Animator.h"
#include "Engine/3DScene.h"
#include "Engine/IViewPoint.h"
#include "Engine/ViewModifier.h"
#include "Engine/LightModifier.h"
#include "GLHierarchy/FragmentShader.h"
#include "GLHierarchy/GeometryEditor.h"
#include "GLHierarchy/Object3DInstance.h"
#include "GLHierarchy/IRenderingProperties.h"
#include "GLHierarchy/Light.h"
#include "GLHierarchy/Shader.h"
#include "GLHierarchy/ShaderProgram.h"
#include "GLHierarchy/VertexShader.h"
#include "GLHierarchy/VertexProgram.h"
#include "GLHierarchy/VulkanShaderStage.h"
#include "GLHierarchy/TextureFactory.h"
#include "GLHierarchy/TextureFactoryConfig.h"
#include "GLHierarchy/TextureObject.h"
#include "GLHierarchy/TextureUnitSetup.h"
#include "System/RaptorConsole.h"
#include "System/RaptorErrorManager.h"
#include "System/RaptorIO.h"

#include "ToolBox/BasicObjects.h"
#include "ToolBox/Imaging.h"
#include "ToolBox/Imaging/BumpmapLoader.h"

RAPTOR_NAMESPACE

//#define VULKAN_TEST 1


class MySphere : public CBasicObjects::CGeoSphere
{
public:
	MySphere();
	~MySphere() {};

	virtual void glClipRender();
private:
};

MySphere::MySphere()
{
#ifndef VULKAN_TEST
	//CShader *pShader = CShader::getShader("BLINN_SHADER").glClone("BLINN");
	//CShader *pShader = CShader::getShader("PHONG_SHADER").glClone("PHONG");
	CShader *pShader = CShader::getShader("BUMP_SHADER").glClone("BUMP");
	setShader(pShader);
#endif
}

void MySphere::glClipRender()
{
	CBasicObjects::CGeoSphere::glClipRender();
}

float lposx(float dt)
{
    return 15 * (float)(cos(2*PI*dt*0.1)*cos(4*PI*dt*0.1));
}
float lposy(float dt)
{
	return 15 * (float)(sin(4*PI*dt*0.1)*cos(2*PI*dt*0.1));
}
float lposz(float dt)
{
	return 15 * (float)(sin(3*PI*dt*0.1)); //*sin(3*PI*dt*0.1));
}



CTest5Doc::CTest5Doc(const RAPTOR_HANDLE& device,const char* title)
{
	m_pDisplay = NULL;
	m_pDisplayBuffer = NULL;
	m_pTexture = NULL;
	m_device = device;

	CRaptorConfig config;
	config.m_logFile = "Test5.log";
    config.m_bRelocation = true;
	config.m_bVulkan = true;
	config.m_uiTexels = 2048*1024;
    config.m_uiPolygons = 20000;
    config.m_uiVertices = 50000;
	config.m_uiUniforms = 100000;
    Raptor::glInitRaptor(config);

	CImaging::installImagers();

	RECT r;
	GetClientRect((HWND)(device.handle()),&r);
	CRaptorDisplayConfig glcs;
	glcs.width = r.right - r.left;
	glcs.height = r.bottom - r.top;
	glcs.x = 0;
	glcs.y = 0;
	glcs.caption = title;
#ifdef VULKAN_TEST
	glcs.renderer = CRaptorDisplayConfig::VULKAN;
#else
	glcs.acceleration = CRaptorDisplayConfig::HARDWARE;
	glcs.antialias = CRaptorDisplayConfig::ANTIALIAS_16X;
#endif
	//glcs.framebufferState.colorClearValue = CColor::RGBA(0.5f,0.6f,0.7f,1.0f);
	glcs.double_buffer = true;
	glcs.depth_buffer = true;
	glcs.display_mode = CGL_RGBA | CGL_DEPTH;
	glcs.draw_logo = true;

	m_pDisplay = Raptor::glCreateDisplay(glcs);
	bool res = m_pDisplay->glvkBindDisplay(device);
	if (res)
	{
		IRenderingProperties &props = m_pDisplay->getRenderingProperties();
		props.enableLighting.enableTexturing;
#ifdef VULKAN_TEST
#else
		CRaptorConsole *pConsole = Raptor::GetConsole();
        pConsole->glInit("",true);
        pConsole->showStatus(true);
        pConsole->activateConsole(true);
#endif
        GLInitContext();

		m_pDisplay->glvkUnBindDisplay();
	}
}

CTest5Doc::~CTest5Doc(void)
{
}

void CTest5Doc::glDestroy()
{
	if (m_pDisplay->glvkBindDisplay(m_device))
	{
		m_pDisplay->glvkReleaseResources();
		m_pDisplay->glvkUnBindDisplay();
	}

	Raptor::glDestroyDisplay(m_pDisplay);
	m_pDisplay = NULL;
}

void CTest5Doc::resize(unsigned int width, unsigned int height)
{
	if (m_pDisplay->glvkBindDisplay(m_device))
    {
        m_pDisplay->glResize(width,height,0,0);
		m_pDisplay->glvkUnBindDisplay();
    }
}

void CTest5Doc::glRender(void)
{
	if (NULL == m_pDisplay)
		return;

	bool res = m_pDisplay->glvkBindDisplay(m_device);
    if (res)
	{
		m_pDisplay->glRender();

		m_pDisplay->glvkUnBindDisplay();
	}
}

void CTest5Doc::GLInitContext(void)
{
#ifdef VULKAN_TEST
	CBasicObjects::CIsocahedron *obj = new CBasicObjects::CIsocahedron();
	obj->setDimensions(2.0f, 3);
	obj->getEditor().genBinormals();
	//obj->getEditor().scaleTexCoords(4.0f, 4.0f);
#else
	CBasicObjects::CGeoSphere *obj = new MySphere();
	obj->setDimensions(2.0f,32,32);
	obj->getEditor().genBinormals();
	obj->getEditor().scaleTexCoords(4.0f,4.0f);

	obj->addModel(CGeometry::CGL_TANGENTS);
#endif


	C3DScene *pScene = m_pDisplay->getRootScene();
	//CShader *shader = new CShader("uniforms-shader");
	//CVertexShader *p = shader->glGetVertexProgram("uniforms");
	//p->glLoadProgramFromStream(*shdr);
	//bool res = shader->glCompileShader();

	CTextureFactory &f = CTextureFactory::getDefaultFactory();
	CShader* s = obj->getShader();


#ifdef VULKAN_TEST
	m_pTexture = f.vkCreateTexture(ITextureObject::CGL_COLOR24_ALPHA, ITextureObject::CGL_ALPHA_TRANSPARENT, ITextureObject::CGL_BILINEAR);
#else
	m_pTexture = f.glCreateTexture(ITextureObject::CGL_COLOR24_ALPHA, ITextureObject::CGL_ALPHA_TRANSPARENT, ITextureObject::CGL_BILINEAR);
#endif


	f.glLoadTexture(m_pTexture, "earth.TGA");
	CTextureUnitSetup *tus = s->glGetTextureUnitsSetup();
	tus->setDiffuseMap(m_pTexture);

	CMaterial *pMat = s->getMaterial();
	pMat->setAmbient(0.02f, 0.02f, 0.02f, 1.0f);
	pMat->setDiffuse(0.3f, 0.4f, 0.8f, 1.0f);
	pMat->setSpecular(0.7f, 0.7f, 0.7f, 1.0f);
	pMat->setEmission(0.0f, 0.0f, 0.0f, 1.0f);
	pMat->setShininess(128.0f);


#ifdef VULKAN_TEST
	CVulkanShaderStage *ss = s->vkGetVulkanShader();
	ss->vkLoadShader("shader3.vert");
	ss->vkLoadShader("shader3.frag");
	
	typedef struct 
	{ 
		GL_MATRIX M;
		GL_MATRIX P;
	} Transform_t;
	Transform_t T;
	
	IDENT_MATRIX(T.M);
	IDENT_MATRIX(T.P);
	T.M[12] = 0.0f;
	T.M[14] = -3.5f;
	{
		CGenericMatrix<float, 4> frustum;
		m_pDisplay->getViewPoint()->getFrustum(frustum);

		CGenericMatrix<float, 4> view;
		view.Ident();
		view[10] = 0.5f;
		view[11] = 0.5f;
		view *= frustum;

		C3DEngine::Generic_to_MATRIX(T.P, view.Transpose());
	}
	
	CProgramParameters parameters;
	CProgramParameters::CParameter<Transform_t> param(T);
	param.name("ubo");
	param.locationIndex = 0;
	parameters.addParameter(param);
	//parameters.addParameter(ss->getDefaultParameter("gl_ModelViewMatrix", 0));

	CProgramParameters::CParameter<CTextureUnitSetup::TEXTURE_IMAGE_UNIT> param2(CTextureUnitSetup::IMAGE_UNIT_0);
	param2.name("diffusemap");
	param2.locationIndex = 1;
	parameters.addParameter(param2);

	ss->setProgramParameters(parameters);

	pScene->addObject(obj);
#else	
	m_pTexture = f.glCreateTexture(ITextureObject::CGL_COLOR24_ALPHA,ITextureObject::CGL_MULTIPLY,ITextureObject::CGL_BILINEAR);
	CBumpmapLoader *loader = new CBumpmapLoader(f.getConfig().getBumpAmplitude());
    f.glLoadTexture(m_pTexture,"bump3.tga", loader);
	//f.getConfig().setBumpAmplitude(4.0f);
	//f.glLoadTexture(m_pTexture,"BlurCircle.TGA",CGL_CREATE_NORMAL_MAP);
	tus->setNormalMap(m_pTexture);


	CViewModifier *vm = new CViewModifier("MySphere");
	vm->setObject(obj);

	CModifier::TIME_FUNCTION tz;
	tz.timeFunction = CModifier::CGL_TIME_CONSTANT;
	tz.a0 = 0.0f; //0.2f;
	CModifier::TIME_FUNCTION tx;
	tx.timeFunction = CModifier::CGL_TIME_CONSTANT;
	tx.a0 = 0.0f ; //0.3f;
	CModifier::TIME_FUNCTION ty;
	ty.timeFunction = CModifier::CGL_TIME_CONSTANT; //CModifier::CGL_TIME_COSINE;
	ty.a2 = 0.0f; //0.3f;
	ty.a1 = 0.0f; //0.2f;
	ty.a0 = 0.005f; //0;
	vm->addAction(CViewModifier::ROTATE_VIEW,tx,ty,tz);

	CLight *pLight = new CLight("MAIN_LIGHT");
	pLight->setAmbient(1.0f,1.0f,1.0f,1.0f);
	pLight->setDiffuse(1.0f,1.0f,1.0f,1.0f);
	pLight->setSpecular(1.0f,0.0f,1.0f,1.0f);
	pLight->setLightPosition(GL_COORD_VERTEX(10.0f,10.0f,10.0f,1.0f));
	pLight->setLightDirection(GL_COORD_VERTEX(0.0f,0.0f,0.0f,1.0f));

	CLight *pLight2 = new CLight("SECOND_LIGHT");
	pLight2->setAmbient(1.0f,1.0f,1.0f,1.0f);
	pLight2->setDiffuse(1.0f,1.0f,1.0f,1.0f);
	pLight2->setSpecular(0.0f,1.0f,1.0f,1.0f);
	pLight2->setLightPosition(GL_COORD_VERTEX(-10.0f,10.0f,10.0f,1.0f));
	pLight2->setLightDirection(GL_COORD_VERTEX(0.0f,0.0f,0.0f,1.0f));

	CLight *pLight3 = new CLight("THIRD_LIGHT");
	pLight3->setAmbient(1.0f,1.0f,1.0f,1.0f);
	pLight3->setDiffuse(1.0f,1.0f,1.0f,1.0f);
	pLight3->setSpecular(1.0f,1.0f,0.0f,1.0f);
	pLight3->setLightPosition(GL_COORD_VERTEX(10.0f,-10.0f,10.0f,1.0f));
	pLight3->setLightDirection(GL_COORD_VERTEX(0.0f,0.0f,0.0f,1.0f));
	
	
	pScene->addObject(vm->getObject());
	pScene->addLight(pLight);
	pScene->addLight(pLight2);
	pScene->addLight(pLight3);

	
	CLightModifier *lm = new CLightModifier("LIGHT_ANIMATOR");
    lm->setLight(pLight);
    CModifier::TIME_FUNCTION lx;
    CModifier::TIME_FUNCTION ly;
    CModifier::TIME_FUNCTION lz;
    lx.pUserFunction = lposx;
    lx.timeFunction = CModifier::CGL_TIME_USER;
    ly.pUserFunction = lposy;
    ly.timeFunction = CModifier::CGL_TIME_USER;
	lz.pUserFunction = lposz;
    lz.timeFunction = CModifier::CGL_TIME_USER;
    lm->addAction(CLightModifier::SET_POSITION,lx,ly,lz);

	CLightModifier *lm2 = new CLightModifier("LIGHT_ANIMATOR2");
    lm2->setLight(pLight2);
    lx.pUserFunction = lposy;
    lx.timeFunction = CModifier::CGL_TIME_USER;
    ly.pUserFunction = lposx;
    ly.timeFunction = CModifier::CGL_TIME_USER;
    lz.pUserFunction = lposz;
    lz.timeFunction = CModifier::CGL_TIME_USER;
    lm2->addAction(CLightModifier::SET_POSITION,lx,ly,lz);

	CLightModifier *lm3 = new CLightModifier("LIGHT_ANIMATOR3");
    lm3->setLight(pLight3);
    lx.pUserFunction = lposy;
    lx.timeFunction = CModifier::CGL_TIME_USER;
    ly.pUserFunction = lposz;
    ly.timeFunction = CModifier::CGL_TIME_USER;
    lz.pUserFunction = lposx;
    lz.timeFunction = CModifier::CGL_TIME_USER;
    lm3->addAction(CLightModifier::SET_POSITION,lx,ly,lz);
#endif


	IViewPoint *vp = m_pDisplay->getViewPoint();
    vp->setPosition(0,0,3.5,IViewPoint::EYE);
    vp->setPosition(0,0,0,IViewPoint::TARGET);

	CTimeObject::setTimeFactor(1.0f);
	CAnimator *pAnimator = new CAnimator();
	CAnimator::SetAnimator(pAnimator);

}
