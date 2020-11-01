// ProjectionDisplay.cpp: implementation of the CProjectionDisplay class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "ProjectionDisplay.h"
#include "Engine/3DEngine.h"
#include "Engine/3DScene.h"
#include "Engine/TimeObject.h"
#include "Engine/LightModifier.h"
#include "GLHierarchy/ShadedGeometry.h"
#include "GLHierarchy/Object3DInstance.h"
#include "GLHierarchy/OpenGLShaderStage.h"
#include "GLHierarchy/FragmentShader.h"
#include "GLHierarchy/TextureSet.h"
#include "GLHierarchy/TextureFactory.h"
#include "GLHierarchy/TextureFactoryConfig.h"
#include "GLHierarchy/TextureUnitSetup.h"
#include "GLHierarchy/ITextureObject.h"
#include "GLHierarchy/Material.h"
#include "GLHierarchy/Light.h"
#include "System/RaptorDisplay.h"
#include "GLHierarchy/Shader.h"
#include "GLHierarchy/Projector.h"
#include "GLHierarchy/IRenderingProperties.h"
#include "GLHierarchy/SimpleObject.h"


class CPlane : public CSimpleObject
{
public:
	CPlane(COpenGLShaderStage* s, CTextureUnitSetup *tmu)
		:m_pStage(s), m_pTmu(tmu)
	{
		setBoundingBox(	GL_COORD_VERTEX(-20.0f,0.0f,-20.1f,1.0f),
						GL_COORD_VERTEX(20.0f,0.1f,20.0f,1.0f));
	};

	virtual ~CPlane() {};

	virtual void glClipRender(void)
	{ glRender(); };
	virtual void glRender(void)
	{
		if (NULL != m_pTmu)
			m_pTmu->glRender();
		if (NULL != m_pStage)
			m_pStage->glRender();

		glNormal3f(0.0f,1.0f,0.0f);
		glBegin(GL_QUADS);	
			glTexCoord4f(0.0f,0.0f,0.0f,1.0f);
			glVertex3f(-20.0f,0.0f,-20.0f);
			glTexCoord4f(2.0f,0.0f,0.0f,1.0f);
			glVertex3f(-20.0f,0.0f,20.0f);
			glTexCoord4f(2.0f,2.0f,0.0f,1.0f);
			glVertex3f(20.0f,0.0f,20.0f);
			glTexCoord4f(0.0f,2.0f,0.0f,1.0f);
			glVertex3f(20.0f,0.0f,-20.0f);
		glEnd();
	}

private:
	COpenGLShaderStage	*m_pStage;
	CTextureUnitSetup *m_pTmu;
};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CProjectionDisplay::CProjectionDisplay():
	m_caustics(NULL),
	m_projector(NULL),
    m_shader(NULL),
	m_pScene(NULL),
	m_pBall(NULL),
	m_light(NULL),
	dt(0.0f)
{
}

CProjectionDisplay::~CProjectionDisplay()
{
    // All destructins are done at Raptor exit.
}

void CProjectionDisplay::Init()
{
	CGenericDisplay::Init();

	tindex = 0;

	m_caustics = new CTextureSet();
    m_projector = new CProjector();

	CPersistence *p = CPersistence::FindObject("main_textures");

	if (p->getId().isSubClassOf(CTextureSet::CTextureSetClassID::GetClassId()))
		 texture = (CTextureSet *)p;
	
	CShadedGeometry *ball = NULL;
	p = CPersistence::FindObject("GeoSphere0");
	if (p->getId().isSubClassOf(CShadedGeometry::CShadedGeometryClassID::GetClassId()))
		ball = (CShadedGeometry*)p;
	
	ball->setRenderingModel(CGeometry::CGL_FRONT_GEOMETRY);
	ball->addModel(CGeometry::CGL_TEXTURE);
	ball->addModel(CGeometry::CGL_NORMALS);
	ball->translate(0,-5,0);
	ball->scale(5,5,5);

	CTextureFactory &f = CTextureFactory::glGetDefaultFactory();
    int i=0;
	for (i=0;i<32;i++)
	{
		char fname[32];
		sprintf(fname,"Datas\\caust%02d.tga",i);

		ITextureObject *T = f.glCreateTexture(	ITextureObject::CGL_COLOR24_ALPHA,
												ITextureObject::CGL_BILINEAR);
		f.glSetTransparency(T, 128);
		f.glLoadTexture(T,fname);
		m_caustics->addTexture(T);
	}

	m_light = new CLight("PROJECTOR");
	m_light->setProjector(m_projector);
	m_light->setLightPosition(GL_COORD_VERTEX(0.0f,10.0f,10.0f));
	m_light->setLightDirection(GL_COORD_VERTEX(0.0f,-1.0f,-1.0f));
	m_light->setAmbient(1.0f,1.0f,1.0f,1.0f);
	m_light->setDiffuse(1.0f,1.0f,1.0f,1.0f);
	m_light->setSpecular(1.0f,1.0f,1.0f,1.0f);
	m_light->setShininess(20.0f);
	//m_light->setCutOff(10.0f);
	m_projector->setUnitDistance(10.0f);

	CLightModifier *lm = new CLightModifier("PROJECTOR_ANIMATOR");
    lm->setLight(m_light);
    CModifier::TIME_FUNCTION lx;
    CModifier::TIME_FUNCTION ly;
    CModifier::TIME_FUNCTION lz;
	lx.timeFunction = CModifier::CGL_TIME_SINE;
	lx.a1 = 1.0f;
	lx.a2 = 10.0f;
	ly.timeFunction = CModifier::CGL_TIME_CONSTANT;
	ly.a0 = 10.0f;
    lz.timeFunction = CModifier::CGL_TIME_COSINE;
	lz.a1 = 1.0f;
	lz.a2 = 10.0f;
    lm->addAction(CLightModifier::SET_POSITION,lx,ly,lz);

	std::string program =
"#version 120							\n\
uniform	sampler2D diffuseMap;			\n\
uniform	sampler2D projectionMap;		\n\
\n\
void main(void)							\n\
{										\n\
	vec4 diffuse = texture2D(diffuseMap, gl_TexCoord[0].st); \n\
	vec4 projection = texture2D(projectionMap, 0.1*gl_TexCoord[3].st); \n\
	gl_FragColor = mix(diffuse, projection, 0.5); \n\
}";
		
	CShader *pShader = ball->getShader();
	CTextureUnitSetup *pSetup = pShader->glGetTextureUnitsSetup();
	pSetup->setDiffuseMap(texture->getTexture(1), CTextureUnitSetup::CGL_MULTIPLY);
	pSetup->enableImageUnit(CTextureUnitSetup::IMAGE_UNIT_1,false);
	pSetup->enableImageUnit(CTextureUnitSetup::IMAGE_UNIT_2,false);
	pSetup->enableImageUnit(CTextureUnitSetup::IMAGE_UNIT_3,false);
	pSetup->getTMUShader(CTextureUnitSetup::IMAGE_UNIT_3).shaderOperation = CGL_TEXTURE_GEN_PROJECTION;
	COpenGLShaderStage *stage = pShader->glGetOpenGLShader("PROJECTION_SHADER");
	CFragmentShader *fs = stage->glGetFragmentShader("PROJECTION_FS_SHADER");
	fs->glLoadProgram(program);
	CProgramParameters params;
	params.addParameter("diffuseMap", CTextureUnitSetup::IMAGE_UNIT_0);
	params.addParameter("projectionMap", CTextureUnitSetup::IMAGE_UNIT_3);
	stage->setProgramParameters(params);
	stage->glCompileShader();


	m_pPlane = new CObject3DInstance(new CPlane(stage, pSetup));
	m_pPlane->translate(0.0f,-6.0f,-18.0f);
	m_pBall = new CObject3DInstance(ball);
	m_pBall->translate(0.0f,-1.0f,-18.0f);

	m_pScene = new C3DScene("PROJECTION_SCENE");
	//m_pScene->useZSort();
	m_pScene->addObject(m_pBall);
	m_pScene->addObject(m_pPlane);
	m_pScene->addLight(m_light);

	CRaptorDisplay* pDisplay = CRaptorDisplay::GetCurrentDisplay();
	pDisplay->addScene(m_pScene);
}

void CProjectionDisplay::ReInit()
{
	CGenericDisplay::ReInit();
	
	tindex = 0;
	caustics = false;

	m_light->glDeActivate();
	m_light->glActivate();

	CPersistence *p = CPersistence::FindObject("PROJECTOR_ANIMATOR");
	if (p != NULL)
		((CLightModifier*)p)->animate(true);

    CRaptorDisplay* const pDisplay = CRaptorDisplay::GetCurrentDisplay();
	IRenderingProperties &rp = pDisplay->getRenderingProperties();
	rp.setTexturing(IRenderingProperties::ENABLE);
	rp.setLighting(IRenderingProperties::ENABLE);

	pDisplay->selectScene("PROJECTION_SCENE");
}

void CProjectionDisplay::UnInit()
{
	m_light->glDeActivate();
}

void CProjectionDisplay::Display()
{
	if (reinit)
		ReInit();

	m_pBall->rotationY(0.02f);
	m_pPlane->rotationY(0.02f);

	if (caustics)
        m_projector->glSetProjection(m_caustics->getTexture(tindex));
	else
        m_projector->glSetProjection(texture->getTexture(0));

	if ((CTimeObject::GetGlobalTime() - dt) > 0.02)
	{
		tindex = (tindex + 1) % 32;
		dt = CTimeObject::GetGlobalTime();
	}
}