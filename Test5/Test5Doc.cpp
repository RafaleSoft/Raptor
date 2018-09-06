#include "StdAfx.h"
#include "Test5Doc.h"
#include "System/Raptor.h"
#include "System/RaptorConfig.h"
#include "Engine/Animator.h"
#include "Engine/3DScene.h"
#include "Engine/ViewPoint.h"
#include "Engine/ViewModifier.h"
#include "Engine/LightModifier.h"
#include "GLHierarchy/FragmentProgram.h"
#include "GLHierarchy/GeometryEditor.h"
#include "GLHierarchy/Object3DInstance.h"
#include "GLHierarchy/RenderingProperties.h"
#include "GLHierarchy/Light.h"
#include "GLHierarchy/Shader.h"
#include "GLHierarchy/ShaderProgram.h"
#include "GLHierarchy/VertexProgram.h"
#include "GLHierarchy/VertexShader.h"
#include "GLHierarchy/VulkanShaderStage.h"
#include "GLHierarchy/TextureFactory.h"
#include "GLHierarchy/TextureFactoryConfig.h"
#include "GLHierarchy/TextureObject.h"
#include "System/RaptorConsole.h"
#include "System/RaptorErrorManager.h"
#include "System/RaptorExtensions.h"
#include "System/RaptorIO.h"
#include "ToolBox/BasicObjects.h"
#include "ToolBox/Imaging.h"


RAPTOR_NAMESPACE

#define VULKAN_TEST 1


class MySphere : public CBasicObjects::CGeoSphere
{
public:
	MySphere();
	~MySphere() {};

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
    Raptor::glInitRaptor(config);

	CImaging::installImagers();

	RECT r;
	GetClientRect((HWND)(device.handle),&r);
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
	bool res = m_pDisplay->glBindDisplay(device);
	if (res)
	{
		CRenderingProperties *props = m_pDisplay->getRenderingProperties();
		props->setLighting(CRenderingProperties::ENABLE);
		props->setTexturing(CRenderingProperties::ENABLE);
#ifdef VULKAN_TEST
#else
		CRaptorConsole *pConsole = Raptor::GetConsole();
        pConsole->glInit("",true);
        pConsole->showStatus(true);
        pConsole->activateConsole(true);
#endif
        GLInitContext();

		m_pDisplay->glUnBindDisplay();
	}
}

CTest5Doc::~CTest5Doc(void)
{
}

void CTest5Doc::resize(unsigned int width, unsigned int height)
{
	if (m_pDisplay->glBindDisplay(m_device))
    {
        m_pDisplay->glResize(width,height,0,0);
        m_pDisplay->glUnBindDisplay();
    }
}

void CTest5Doc::glRender(void)
{
	bool res = m_pDisplay->glBindDisplay(m_device);
    if (res)
	{
		m_pDisplay->glRender();

		m_pDisplay->glUnBindDisplay();
	}
}

void CTest5Doc::GLInitContext(void)
{
#ifdef VULKAN_TEST
#else
	CBasicObjects::CGeoSphere *obj = new MySphere();
	obj->setDimensions(2.0f,32,32);
	obj->getEditor().genBinormals();
	obj->getEditor().scaleTexCoords(4.0f,4.0f);

	obj->getRenderingModel().addModel(CGeometry::CRenderingModel::CGL_TANGENTS);
#endif

	C3DScene *pScene = m_pDisplay->getRootScene();
	//CShader *shader = new CShader("uniforms-shader");
	//CVertexProgram *p = shader->glGetVertexProgram("uniforms");
	//p->glLoadProgramFromStream(*shdr);
	//bool res = shader->glCompileShader();

	CTextureFactory &f = CTextureFactory::getDefaultFactory();

#ifdef VULKAN_TEST
	
	CBasicObjects::CIsocahedron *obj = new CBasicObjects::CIsocahedron();
	obj->setDimensions(0.5f, 3);
	obj->getEditor().genBinormals();
	obj->getEditor().scaleTexCoords(4.0f, 4.0f);
	/*
	CShadedGeometry *obj = new CShadedGeometry("VULKAN_GEOMETRY");
	//obj->getRenderingModel().addModel(CGeometry::CRenderingModel::CGL_COLORS);
	obj->getRenderingModel().addModel(CGeometry::CRenderingModel::CGL_TEXTURE);

	obj->glSetVertices(4, NULL);
	obj->glSetColors(4, NULL);
	obj->glSetTexCoords(4, NULL);
	obj->glSetPolygons(2, NULL);
	GL_COORD_VERTEX VertexData[5] =
	{
		GL_COORD_VERTEX(-0.7f, -0.7f, -2.0f, 1.0f),	// TODO : check / configure depth
		GL_COORD_VERTEX(-0.7f, 0.7f, -5.0f, 1.0f),
		GL_COORD_VERTEX(0.7f, -0.7f, -1.5f, 1.0f),
		GL_COORD_VERTEX(0.7f, 0.7f, -3.5f, 1.0f),
		GL_COORD_VERTEX(0.0f, 0.0f, 0.1f, 1.0f)
	};
	GL_TEX_VERTEX TexCoordData[4] =
	{
		GL_TEX_VERTEX(0.0f, 0.0f),	// TODO : check / configure depth
		GL_TEX_VERTEX(0.0f, 1.0f),
		GL_TEX_VERTEX(1.0f, 0.0f),
		GL_TEX_VERTEX(1.0f, 1.0f),
	};
	CColor::RGBA ColorData[4] =
	{
		CColor::RGBA(1.0f, 0.0f, 0.0f, 0.0f),
		CColor::RGBA(0.0f, 1.0f, 0.0f, 0.0f),
		CColor::RGBA(0.0f, 0.0f, 1.0f, 0.0f),
		CColor::RGBA(0.3f, 0.3f, 0.3f, 0.0f)
	};
	unsigned short VertexIndices[6] =
	{
		3, 2, 0, 3, 0, 1
	};
	obj->glSetVertices(4, VertexData);
	obj->glSetColors(4, ColorData);
	obj->glSetTexCoords(4, TexCoordData);
	obj->glSetPolygons(2, VertexIndices);
	*/
	m_pTexture = f.vkCreateTexture(ITextureObject::CGL_COLOR24_ALPHA, CTextureObject::CGL_ALPHA_TRANSPARENT, ITextureObject::CGL_BILINEAR);
	f.glLoadTexture(m_pTexture, "earth.TGA");

	CShader* s = obj->getShader();
	CVulkanShaderStage *ss = s->vkGetVulkanProgram();
	ss->vkLoadShader("shader3.vert");
	ss->vkLoadShader("shader3.frag");
	CTextureUnitSetup *tus = s->glGetTextureUnitsSetup();
	tus->setDiffuseMap(m_pTexture);

	typedef struct 
	{ 
		GL_MATRIX M;
		GL_MATRIX P;
	} Transform_t;
	Transform_t T;
	
	IDENT_MATRIX(T.M);
	IDENT_MATRIX(T.P);
	T.M[12] = 0.5f;
	T.M[14] = -1.0f;
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
	param.name("modelview");
	param.locationIndex = 0;
	parameters.addParameter(param);

	CProgramParameters::CParameter<CTextureUnitSetup::TEXTURE_IMAGE_UNIT> param2(CTextureUnitSetup::IMAGE_UNIT_0);
	param2.name("diffusemap");
	param2.locationIndex = 1;
	parameters.addParameter(param2);

	ss->setProgramParameters(parameters);

	pScene->addObject(obj);
#else
	m_pTexture = f.glCreateTexture(ITextureObject::CGL_COLOR24_ALPHA, CTextureObject::CGL_ALPHA_TRANSPARENT, ITextureObject::CGL_BILINEAR);
	f.glLoadTexture(m_pTexture,"earth.TGA");
	CTextureUnitSetup *tus = obj->getShader()->glGetTextureUnitsSetup();
	tus->setDiffuseMap(m_pTexture);
	m_pTexture = f.glCreateTexture(ITextureObject::CGL_COLOR24_ALPHA,CTextureObject::CGL_MULTIPLY,ITextureObject::CGL_BILINEAR);
    f.glLoadTexture(m_pTexture,"bump3.tga",CVaArray<CImage::IImageOP::OP_KIND>(CImage::IImageOP::BUMPMAP_LOADER));
	//f.getConfig().setBumpAmplitude(4.0f);
	//f.glLoadTexture(m_pTexture,"BlurCircle.TGA",CGL_CREATE_NORMAL_MAP);
	tus->setNormalMap(m_pTexture);

	CMaterial *pMat = obj->getShader()->getMaterial();
	pMat->setAmbient(0.02f,0.02f,0.02f,1.0f);
	pMat->setDiffuse(0.3f,0.4f,0.8f,1.0f);
	pMat->setSpecular(0.7f,0.7f,0.7f,1.0f);
	pMat->setEmission(0.0f,0.0f,0.0f,1.0f);
	pMat->setShininess(128.0f);

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

	CViewPoint *vp = m_pDisplay->getViewPoint();
    vp->setPosition(0,0,3.5,CViewPoint::EYE);
    vp->setPosition(0,0,0,CViewPoint::TARGET);

	CTimeObject::setTimeFactor(1.0f);
	CAnimator *pAnimator = new CAnimator();
	CAnimator::SetAnimator(pAnimator);

}
