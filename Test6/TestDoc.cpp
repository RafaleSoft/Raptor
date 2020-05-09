// TestDoc.cpp : implementation of the CTestDoc class
//

#include "StdAfx.h"
#include <sstream>

#include "TestDoc.h"

#include "System/RaptorDisplay.h"
#include "GLHierarchy/ShadedGeometry.h"
#include "GLHierarchy/Object3DInstance.h"
#include "GLHierarchy/OpenGLShaderStage.h"
#include "GLHierarchy/TextureFactory.h"
#include "GLHierarchy/TextureFactoryConfig.h"
#include "GLHierarchy/TextureObject.h"
#include "GLHierarchy/FragmentShader.h"
#include "GLHierarchy/GeometryShader.h"
#include "GLHierarchy/VertexShader.h"
#include "GLHierarchy/Shader.h"
#include "Engine/ViewModifier.h"
#include "Engine/3DScene.h"
#include "Engine/Animator.h"
#include "Engine/IViewPoint.h"
#include "System/Raptor.h"
#include "GLHierarchy/Material.h"
#include "GLHierarchy/Light.h"
#include "GLHierarchy/IRenderingProperties.h"
#include "GLHierarchy/3DSet.h"
#include "System/RaptorConsole.h"

#include "ToolBox/RaptorToolBox.h"
#include "ToolBox/BasicObjects.h"


extern const std::string vp_src;
extern const std::string gp_src;
extern const std::string fp_src;

extern const std::string vp2_src;
extern const std::string gp2_src;
extern const std::string fp2_src;



class CPoints : public CShadedGeometry
{
public:
	CPoints() {};
	virtual ~CPoints()
	{
	};

	void Init(size_t s);

	virtual void glRender()
	{
		glPushAttrib(GL_ENABLE_BIT);

		GLint blendSrc;
		GLint blendDst;
		glGetIntegerv(GL_BLEND_SRC, &blendSrc);
		glGetIntegerv(GL_BLEND_DST, &blendDst);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);

		CShadedGeometry::glRender();

		glBlendFunc(blendSrc, blendDst);
		glPopAttrib();
	}
};

void CPoints::Init(size_t s)
{
	addModel(CGeometry::CGL_FRONT_GEOMETRY);
	addModel(CGeometry::CGL_COLORS);
	addModel(CGeometry::CGL_TEXTURE);
	addModel(CGeometry::CGL_WEIGHT);
	addModel(CGeometry::CGL_FOG);

	glSetVertices(s);
	glSetWeights(s);
	glSetFogs(s);
	glSetColors(s);

	glLockData();

	for (size_t i = 0; i < s; i++)
	{
		float x = 3.0f * ((float)rand() - 0.5f*RAND_MAX) / RAND_MAX;
		float y = 3.0f * ((float)rand() - 0.5f*RAND_MAX) / RAND_MAX;
		float z = 3.0f * ((float)rand() - 0.5f*RAND_MAX) / RAND_MAX;
		addVertex(x, y, z, 1.0f);
		setWeight(i, (0.5f * rand()) / RAND_MAX);
		setFogCoord(i, 2*PI*(rand()) / RAND_MAX);
		setColor(i, (float)(rand()) / RAND_MAX, (float)(rand()) / RAND_MAX, (float)(rand()) / RAND_MAX, (float)(rand()) / RAND_MAX);
	}

	glUnLockData();

	CGeometryPrimitive *p = createPrimitive(CGeometryPrimitive::POINT);

	unsigned short *points = new unsigned short[s];
	for (uint16_t i = 0; i < s; i++)
		points[i] = i;

	p->setIndexes(s, points);

	delete[] points;
}

class CCube : public CShadedGeometry
{
public:
	CCube() {};
	virtual ~CCube()
	{
	};

	void Init(size_t s);

	virtual void glRender()
	{
		glPushAttrib(GL_ENABLE_BIT);

		GLint blendSrc;
		GLint blendDst;
		glGetIntegerv(GL_BLEND_SRC, &blendSrc);
		glGetIntegerv(GL_BLEND_DST, &blendDst);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);

		CShadedGeometry::glRender();

		glBlendFunc(blendSrc, blendDst);
		glPopAttrib();
	}
};

void CCube::Init(size_t s)
{
	addModel(CGeometry::CGL_FRONT_GEOMETRY);
	addModel(CGeometry::CGL_COLORS);
	addModel(CGeometry::CGL_TEXTURE);
	addModel(CGeometry::CGL_TANGENTS);

	glSetVertices(s);
	glSetColors(s);
	glSetTangents(s);

	glLockData();

	srand(GetTickCount());
	for (size_t i = 0; i < s; i++)
	{
		float x = 3.0f * ((float)rand() - 0.5f*RAND_MAX) / RAND_MAX;
		float y = 3.0f * ((float)rand() - 0.5f*RAND_MAX) / RAND_MAX;
		float z = 3.0f * ((float)rand() - 0.5f*RAND_MAX) / RAND_MAX;

		float w = 0.1f * ((float)rand()) / RAND_MAX;
		float h = 0.1f * ((float)rand()) / RAND_MAX;
		float d = 0.1f * ((float)rand()) / RAND_MAX;

		addVertex(x - w, y - h, z - d, 1.0f);
		setTangent(i, x + w, y + h, z + d, 1.0f);

		setColor(i, (float)(rand()) / RAND_MAX, (float)(rand()) / RAND_MAX, (float)(rand()) / RAND_MAX, (float)(rand()) / RAND_MAX);
	}

	glUnLockData();

	CGeometryPrimitive *p = createPrimitive(CGeometryPrimitive::POINT);

	unsigned short *points = new unsigned short[s];
	for (uint16_t i = 0; i < s; i++)
		points[i] = i;

	p->setIndexes(s, points);

	delete[] points;
}

CTestDoc::CTestDoc(const RAPTOR_HANDLE& wnd,CRaptorDisplay *pDisplay)
{
	m_pSG = NULL;
    m_pDisplay = pDisplay;
    m_wnd = wnd;
}

CTestDoc::~CTestDoc()
{
}


/////////////////////////////////////////////////////////////////////////////
// CTestDoc commands
//	Default virtual implementation
void CTestDoc::GLInitContext(void)
{
	CAnimator::SetAnimator(new CAnimator());
	
	glClearColor(0.0f,0.0f,0.0f,0.0f);

	CRaptorDisplay *dsp = CRaptorDisplay::GetCurrentDisplay();
	IRenderingProperties &props = dsp->getRenderingProperties();
	props.setLighting(IRenderingProperties::ENABLE);
	props.setTexturing(IRenderingProperties::ENABLE);
	props.setMultisampling(IRenderingProperties::ENABLE);

    CLight *pLight = new CLight;
    pLight->setAmbient(1.0f,1.0f,1.0f,1.0f);
    pLight->setDiffuse(1.0f,1.0f,1.0f,1.0f);
    pLight->setSpecular(1.0f,1.0f,1.0f,1.0f);
    pLight->setLightPosition(GL_COORD_VERTEX(0.0f,0.0f,10.0f,1.0f));
    pLight->setLightDirection(GL_COORD_VERTEX(0.0f,0.0f,-5.0f,1.0f));
    pLight->glActivate();
	
	IViewPoint *vp = dsp->getViewPoint();
    vp->setPosition(0,0,3.5,IViewPoint::EYE);
    vp->setPosition(0,0,0,IViewPoint::TARGET);

	CViewModifier *vm = new CViewModifier("test6");
	vm->setObject(m_pSG);

	CModifier::TIME_FUNCTION tz;
	tz.timeFunction = CModifier::CGL_TIME_CONSTANT;
	tz.a0 = 0.001f;
	CModifier::TIME_FUNCTION tx;
	tx.timeFunction = CModifier::CGL_TIME_CONSTANT;
	tx.a0 = 0.015f;
	CModifier::TIME_FUNCTION ty;
	ty.timeFunction = CModifier::CGL_TIME_COSINE;
	ty.a2 = 0.035f;
	ty.a1 = 0.02f;
	ty.a0 = 0;
	vm->addAction(CViewModifier::ROTATE_VIEW,tx,ty,tz);
	
	CTextureFactory &tf = CTextureFactory::getDefaultFactory();
	ITextureObject *T = tf.glCreateTexture(	ITextureObject::CGL_COLOR24_ALPHA,
											CTextureObject::CGL_OPAQUE,
											ITextureObject::CGL_BILINEAR);
	tf.glLoadTexture(T, "Start.tga");

	CPoints *points = new CPoints();
	{
		points->Init(1024);
		CShader *shader = points->getShader();
		COpenGLShaderStage *stage = shader->glGetOpenGLShader();
		CVertexShader *vs = stage->glGetVertexShader();
		bool res = vs->glLoadProgram(vp_src);
		CGeometryShader *gs = stage->glGetGeometryShader();
		gs->setGeometry(GL_POINTS, GL_TRIANGLE_STRIP, 4);
		res = res & gs->glLoadProgram(gp_src);
		CFragmentShader *fs = stage->glGetFragmentShader();
		res = res & fs->glLoadProgram(fp_src);
		CProgramParameters params;
		params.addParameter("diffuseMap", CTextureUnitSetup::IMAGE_UNIT_0);

		res = res & stage->glCompileShader();

		CTextureUnitSetup *ts = shader->glGetTextureUnitsSetup();
		ts->setDiffuseMap(T);
	}
	
	CCube *cube = new CCube();
	{
		cube->Init(1024);
		CShader *shader = cube->getShader();
		COpenGLShaderStage *stage = shader->glGetOpenGLShader();
		CVertexShader *vs = stage->glGetVertexShader();
		bool res = vs->glLoadProgram(vp2_src);
		CGeometryShader *gs = stage->glGetGeometryShader();
		gs->setGeometry(GL_POINTS, GL_TRIANGLE_STRIP, 18);
		res = res & gs->glLoadProgram(gp2_src);
		CFragmentShader *fs = stage->glGetFragmentShader();
		res = res & fs->glLoadProgram(fp2_src);
		CProgramParameters params;
		params.addParameter("diffuseMap", CTextureUnitSetup::IMAGE_UNIT_0);

		res = res & stage->glCompileShader();

		CTextureUnitSetup *ts = shader->glGetTextureUnitsSetup();
		ts->setDiffuseMap(T);
	}

	//vm->setObject(points);
	vm->setObject(cube);

	C3DScene *pScene = dsp->getRootScene();
	pScene->addObject(vm->getObject());
    pScene->addLight(pLight);
}


void CTestDoc::glRender()
{
	m_pDisplay->glvkBindDisplay(m_wnd);

	m_pDisplay->glRender();

	m_pDisplay->glvkUnBindDisplay();
}

