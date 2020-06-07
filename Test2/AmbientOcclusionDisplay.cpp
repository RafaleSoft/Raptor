// AmbientOcclusionDisplay.cpp: implementation of the CAmbientOcclusionDisplay class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "Test2.h"
#include "AmbientOcclusionDisplay.h"

#include "System/Color.h"
#include "System/Raptor.h"
#include "System/RaptorDisplay.h"
#include "System/RaptorIO.h"
#include "Engine/3DScene.h"
#include "Engine/Environment.h"
#include "Engine/ViewModifier.h"
#include "Engine/3DEngineMatrix.h"
#include "GLHierarchy/SimpleObject.h"
#include "GLHierarchy/ShadedGeometry.h"
#include "GLHierarchy/GeometryEditor.h"
#include "GLHierarchy/IRenderingProperties.h"
#include "GLHierarchy/Object3DInstance.h"
#include "GLHierarchy/Shader.h"
#include "GLHierarchy/VertexShader.h"
#include "GLHierarchy/FragmentShader.h"
#include "GLHierarchy/TextureFactory.h"
#include "GLHierarchy/TextureFactoryConfig.h"
#include "GLHierarchy/TextureSet.h"
#include "GLHierarchy/TextureUnitSetup.h"
#include "GLHierarchy/Light.h"
#include "GLHierarchy/OpenGLShaderStage.h"

#include "Toolbox/BasicObjects.h"


string AO_vp_src =
"#version 120 \n\
uniform sampler2D AOMap; \n\
varying float color; \n\
void main (void) \n\
{\n\
	gl_Position = ftransform(); \n\
	vec2 tc = 0.5 * (vec2(1.0,1.0) + gl_MultiTexCoord2.xy); \n\
	vec4 ao = texture2D(AOMap,tc); \n\
	color = gl_Color.r - (1.0 - ao.r); \n\
}";

string AO_fp_src = 
"#version 120 \n\
varying float color; \n\
void main (void) \n\
{\n\
	gl_FragColor = vec4(color,color,color,1.0); \n\
}";


CAmbientOcclusionDisplay::CAmbientOcclusionDisplay(void)
	:m_pTeapot(NULL),m_pLight(NULL)
{
}

CAmbientOcclusionDisplay::~CAmbientOcclusionDisplay(void)
{
}

void CAmbientOcclusionDisplay::Init()
{
	CGenericDisplay::Init();
	
	CShader *AO_shader = new CShader("AO_SHADER");
	COpenGLShaderStage *stage = AO_shader->glGetOpenGLShader();
	CVertexShader *vp = stage->glGetVertexShader("AO_VP");
	CFragmentShader *fp = stage->glGetFragmentShader("AO_FP");

	bool res = vp->glLoadProgram(AO_vp_src);
	res = res && fp->glLoadProgram(AO_fp_src);
	res = res && stage->glCompileShader();
	if (res)
	{
		CProgramParameters ao_params;
		ao_params.addParameter("AOMap", CTextureUnitSetup::IMAGE_UNIT_2);
		stage->setProgramParameters(ao_params);
	}
	

	m_pLight = new CLight("AOLight");
    m_pLight->setAmbient(1.0f,1.0f,1.0f,1.0f);
    m_pLight->setDiffuse(1.0f,1.0f,1.0f,1.0f);
	m_pLight->setSpecular(1.0f,1.0f,1.0f,1.0f);
	m_pLight->setLightPosition(GL_COORD_VERTEX(0.0,0.0,500.0,1.0));
	m_pLight->setLightDirection(GL_COORD_VERTEX(0.0,0.0,0.0,1.0));

	CPersistence *p = CPersistence::FindObject("Knot");
	if (p->getId().isSubClassOf(CGeometry::CGeometryClassID::GetClassId()))
	{
		m_pTeapot = new CShadedGeometry();
		*m_pTeapot = *((CGeometry *)p);
		m_pTeapot->translateAbsolute(0,0,0);
		m_pTeapot->setRenderingModel(CGeometry::CGL_FRONT_GEOMETRY);
		m_pTeapot->addModel(CGeometry::CGL_NORMALS);
		m_pTeapot->addModel(CGeometry::CGL_TEXTURE);
	}
	
	if (m_pTeapot != NULL)
	{
		// 
		//  View Modifier of the teapot
		m_pVM = new CViewModifier("AMBIENTOCCLUSION_TEAPOT");
		m_pVM->setObject(m_pTeapot);
		CModifier::TIME_FUNCTION tx;
		CModifier::TIME_FUNCTION ty;
		CModifier::TIME_FUNCTION tz;

		m_pVM->addAction(CViewModifier::RESET_TRANSFORM,tx,ty,tz);

		ty.timeFunction = CModifier::CGL_TIME_CONSTANT;
		ty.a0 = 1;
		tz.timeFunction = CModifier::CGL_TIME_CONSTANT;
		tz.a0 = -10; //-20;
		m_pVM->addAction(CViewModifier::TRANSLATEABSOLUTE,tx,ty,tz);

		tx.timeFunction = CModifier::CGL_TIME_COSINE;
		tx.a0 = 0.0f;
		tx.a1 = 0.4f;
		tx.a2 = 50.0f;       //  rx = 50.0 * cos ( 4.0 * t )
		ty.timeFunction = CModifier::CGL_TIME_COSINE;
		ty.a0 = 0.0f;
		ty.a1 = 0.3f;
		ty.a2 = 100.0f;      //  ry = 100.0 * cos ( 3.0 * t )
		tz.timeFunction = CModifier::CGL_TIME_COSINE;
		tz.a0 = 0.0f;
		tz.a1 = 0.5f;
		tz.a2 = 50.0f;       //  rz = 50.0 * cos ( 5.0 * t )
		m_pVM->addAction(CViewModifier::ROTATE_VIEW,tx,ty,tz);


		//	Prepare Ambient occlusion
		
		m_pTeapot->getEditor().genBinormals();
		m_pTeapot->addModel(CGeometry::CGL_TANGENTS);
		m_pTeapot->setShader(CShader::getShader("BUMP_SHADER").glClone("AO_BUMP"));
		
		CTextureFactory &f = CTextureFactory::getDefaultFactory();
		ITextureObject*	m_pTexture = f.glCreateTexture(	ITextureObject::CGL_COLOR24_ALPHA,
														ITextureObject::CGL_OPAQUE,
														ITextureObject::CGL_BILINEAR);
		f.glLoadTexture(m_pTexture,"Datas/MARBLE6.JPG");
		
		CTextureUnitSetup *tus = m_pTeapot->getShader()->glGetTextureUnitsSetup();
		tus->setDiffuseMap(m_pTexture, CTextureUnitSetup::CGL_OPAQUE);
		m_pTexture = f.glCreateTexture(	ITextureObject::CGL_COLOR24_ALPHA,
										ITextureObject::CGL_OPAQUE,
										ITextureObject::CGL_BILINEAR);
		f.glLoadTexture(m_pTexture,"Datas/BUMP4.TGA");
		tus->setNormalMap(m_pTexture, CTextureUnitSetup::CGL_OPAQUE);
		tus->useRegisterCombiners(false);
		
		CMaterial *pMat = m_pTeapot->getShader()->getMaterial();
		pMat->setAmbient(0.02f,0.02f,0.02f,1.0f);
		pMat->setDiffuse(0.8f,0.8f,0.8f,1.0f);
		pMat->setSpecular(0.7f,0.7f,0.7f,1.0f);
		pMat->setEmission(0.0f,0.0f,0.0f,1.0f);
		pMat->setShininess(20.0f);
		
		
		CBasicObjects::CRectMesh *RMesh = new CBasicObjects::CRectMesh();
		RMesh->setDimensions(20.0f,20.0f,50,50);

		CShadedGeometry *AOMesh = new CShadedGeometry("AO_MESH");
		*AOMesh = *RMesh;
		AOMesh->rotationX(-90.0f);
		AOMesh->translateAbsolute(0.0f,-6.0f,-10.0f);
		AOMesh->getEditor().genNormals(true);
		AOMesh->setShader(AO_shader);

		C3DScene *m_pScene = new C3DScene("AO_SCENE");
		m_pScene->addObject(AOMesh);
		m_pScene->addObject(m_pVM->getObject());
		m_pScene->addLight(m_pLight);

		CRaptorDisplay* pDisplay = CRaptorDisplay::GetCurrentDisplay();
		pDisplay->addScene(m_pScene);
		CEnvironment *ambient_occlusion = CEnvironment::glCreateEnvironment(*m_pScene, CEnvironment::AMBIENT_OCCLUSION, 0, 0);
		m_pScene->glManageEnvironment(ambient_occlusion);
	}
}

void CAmbientOcclusionDisplay::ReInit()
{
	CGenericDisplay::ReInit();

	CPersistence *p = CPersistence::FindObject("AMBIENTOCCLUSION_TEAPOT");
	if (p != NULL)
	{
		((CViewModifier*)p)->animate(true);
	}

    CRaptorDisplay* pDisplay = CRaptorDisplay::GetCurrentDisplay();
	IRenderingProperties &rp = pDisplay->getRenderingProperties();
	rp.setTexturing(IRenderingProperties::ENABLE);
	rp.setLighting(IRenderingProperties::ENABLE);
	//rp->setWireframe(IRenderingProperties::ENABLE);
	//rp->setCullFace(IRenderingProperties::DISABLE);

	pDisplay->selectScene("AO_SCENE");
}

void CAmbientOcclusionDisplay::UnInit()
{
}


void CAmbientOcclusionDisplay::Display()
{
	if (reinit)
		ReInit();
}
