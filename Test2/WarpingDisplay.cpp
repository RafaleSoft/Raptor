// WarpingDisplay.cpp: implementation of the CWarpingDisplay class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "WarpingDisplay.h"


#include "Engine/GeometricModifier.h"
#include "Engine/3DScene.h"
#include "GLHierarchy/TextureSet.h"
#include "GLHierarchy/TextureFactory.h"
#include "GLHierarchy/TextureUnitSetup.h"
#include "GLHierarchy/ITextureObject.h"
#include "Engine/3DEngine.h"
#include "GLHierarchy/ShadedGeometry.h"
#include "GLHierarchy/VertexProgram.h"
#include "GLHierarchy/Object3DInstance.h"
#include "GLHierarchy/SimpleObject.h"
#include "GLHierarchy/Shader.h"
#include "GLHierarchy/FragmentShader.h"
#include "System/Raptor.h"
#include "System/RaptorGLExtensions.h"
#include "GLHierarchy/IRenderingProperties.h"
#include "GLHierarchy/OpenGLShaderStage.h"

#include "ToolBox/BasicObjects.h"

#define BASE_WARP_WIDTH 512
#define BASE_WARP_HEIGHT 512


class CBackground : public CSimpleObject
{
public:
	CBackground()
	{
		setBoundingBox(	GL_COORD_VERTEX(-70.0f,-50.0f,0.0f,1.0f),
						GL_COORD_VERTEX(70.0f,50.0f,0.1f,1.0f));
		//	Use a default shader to create a simple geometry
		CVertexProgram s("GL_SHADER");
		GL_COORD_VERTEX texCoord1(0,0,0,1);

		//	background;
		list.handle(glGenLists(1));
		glNewList(list.handle(),GL_COMPILE);
			glBegin(GL_QUADS);
				glTexCoord2d(0.0,0.0);
				texCoord1.x = -1.0; texCoord1.y = -1.0;
				s.glMultiTexCoord(CProgramParameters::TEXCOORD1, texCoord1);
				glVertex3d(-70.0,-50.0,0.0);

				glTexCoord2d(1.0,0.0);
				texCoord1.x = 1.0; texCoord1.y = -1.0;
				s.glMultiTexCoord(CProgramParameters::TEXCOORD1, texCoord1);
				glVertex3d(70.0,-50.0,0.0);

				glTexCoord2d(1.0,1.0);
				texCoord1.x = 1.0; texCoord1.y = 1.0;
				s.glMultiTexCoord(CProgramParameters::TEXCOORD1, texCoord1);
				glVertex3d(70.0,50.0,0.0);

				glTexCoord2d(0.0,1.0);
				texCoord1.x = -1.0; texCoord1.y = 1.0;
				s.glMultiTexCoord(CProgramParameters::TEXCOORD1, texCoord1);
				glVertex3d(-70.0,50.0,0.0);
			glEnd();
		glEndList();

		CTextureSet	*t = NULL;
		CPersistence *p = CPersistence::FindObject("main_textures");
		if (p->getId().isSubClassOf(CTextureSet::CTextureSetClassID::GetClassId()))
			 t = (CTextureSet *)p;

		if (NULL != t)
		{
			tmu.setDiffuseMap(t->getTexture(2), CTextureUnitSetup::CGL_ALPHA_TRANSPARENT);
			tmu.setNormalMap(t->getTexture(0), CTextureUnitSetup::CGL_ALPHA_TRANSPARENT);
			bg = tmu.glBuildSetup();
		}
	};

	virtual ~CBackground() {};

	virtual void glClipRender(void)
	{ glRender(); };
	virtual void glRender(void)
	{
		tmu.glRender();
		//glCallList(bg.handle());
		glCallList(list.handle());

		const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();
		pExtensions->glActiveTextureARB(GL_TEXTURE1_ARB);
		glDisable(GL_TEXTURE_2D);
		pExtensions->glActiveTextureARB(GL_TEXTURE0_ARB);
	}

private:
	RAPTOR_HANDLE list;
	RAPTOR_HANDLE bg;
	CTextureUnitSetup tmu;
};


class CWarpObject : public CGeometricModifier,
					public CSimpleObject
{
public:
	CWarpObject(float width,float height,int hcels = 50,int vcels = 50);
	~CWarpObject() 
	{
		setGeometry(NULL);

		if (m_d_grid != NULL)
		{
			delete [] m_d_grid;
			m_d_grid = NULL;
		}
	};

	void glClipRender(void) { glRender(); };
	void glRender(void);

	virtual void RAPTOR_FASTCALL updateVertices(float dt,GL_COORD_VERTEX *vertices);

private:
	float		m_orgx;
	float		m_orgy;
	float		m_stepx;
	float		m_stepy;

	int			m_hcels;
	int			m_vcels;

	int			m_index;

	GLfloat		*m_d_grid;

	ITextureObject	*m_captureBuffer;
};


CWarpObject::CWarpObject(float width,float height,int hcels,int vcels)
	:CGeometricModifier(CGL_TIME_LINEAR,0,1,0,0,NULL,CModifier::CModifierClassID::GetClassId()),
	m_hcels(hcels),
	m_vcels(vcels),
	m_index(0),
	m_d_grid(NULL)
{
	m_orgx = -width * 0.5f;
	m_orgy = -height * 0.5f;
	m_stepx = width / (hcels-1);
	m_stepy = height / (vcels-1);

	setBoundingBox(	GL_COORD_VERTEX(m_orgx,m_orgy,-0.1f,1.0f),
					GL_COORD_VERTEX(-m_orgx,-m_orgy,0.1f,1.0f));

	m_d_grid = new float[3 * m_hcels * m_vcels];

	//	The border indexes are not used to avoid
	//	texture border artifacts
	int pos = 0;
	memset(m_d_grid,0,3*m_hcels*m_vcels*sizeof(float));

	CTextureFactory &f = CTextureFactory::getDefaultFactory();
    m_captureBuffer = f.glCreateDynamicTexture(	ITextureObject::CGL_COLOR24_ALPHA,
												ITextureObject::CGL_BILINEAR,
												CRaptorDisplay::GetCurrentDisplay());
    f.glResizeTexture(m_captureBuffer,BASE_WARP_WIDTH,BASE_WARP_HEIGHT);

	CBasicObjects::CRectMesh *mesh = new CBasicObjects::CRectMesh();
	mesh->setDimensions(width,height,m_hcels,m_vcels);
	mesh->removeModel(CGeometry::CGL_NORMALS);

	setGeometry(mesh);
}

void RAPTOR_FASTCALL CWarpObject::updateVertices(float dt,GL_COORD_VERTEX *vertices)
{
	int offset = 3 * m_hcels * m_index;

	for (int i=0;i<m_hcels;i++)
	{
		float rd = 0.1f * ((float)rand() / RAND_MAX) - 0.05f;
		m_d_grid[offset + 3 * i] = rd;

		rd = 0.1f * ((float)rand() / RAND_MAX) - 0.05f;
		m_d_grid[offset + 3 * i + 1] = rd;
	}

	int index = m_index;

	for (int j=0;j<m_vcels;j++)
	{
		int offset2 = 3 * m_hcels * index;

		for (int i=0;i<m_hcels;i++)
		{
			GL_COORD_VERTEX &V = vertices[m_hcels * j + i];
			V.x = m_orgx + i * m_stepx;
			V.y = m_orgy + j * m_stepy;
			V.z = 0.0f;
			V.h = 1.0f;

			bool border = (i==0) || (j==0) || (j==(m_vcels-1)) || (i==(m_hcels-1));
			if (!border)
			{
				V.x += m_d_grid[offset2 + 3 * i];
				V.y += m_d_grid[offset2 + 3 * i + 1];
				V.z += m_d_grid[offset2 + 3 * i + 2];
			}
		}

		index++;
		if( index == m_vcels)
			index = 0;
	}

	m_index--;
	if( m_index < 0)
		m_index = m_vcels - 1;
}

void CWarpObject::glRender()
{
	GL_COORD_VERTEX v1(m_orgx , m_orgy , 0.0f , 1.0f );
	GL_COORD_VERTEX v2( -m_orgx , -m_orgy , 0.0f , 1.0f );

	C3DEngine::Get3DEngine()->glProject(v1);
	C3DEngine::Get3DEngine()->glProject(v2);

	float x_sz = (v2.x - v1.x)/BASE_WARP_WIDTH;
	float y_sz = (v2.y - v1.y)/BASE_WARP_HEIGHT;

	//	save GPU time, just copy pixels that will be shaded
	//	and take a margin for AA/BilinearFiltering cases
	int w = x_sz * m_captureBuffer->getWidth();
	int h = y_sz * m_captureBuffer->getHeight();
    m_captureBuffer->setGenerationSize(floor(v1.x+0.5f), floor(v1.y+0.5f), w+2, h+2);
	m_captureBuffer->glvkRender();

	glMatrixMode(GL_TEXTURE);
	glPushMatrix();
	glScalef(x_sz,y_sz,1.0f);

	getObject()->glRender();

	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
}


class CGlassObject : public CModifier,
					 public CSimpleObject
{
public:
	CGlassObject(float width,float height,int hcels = 50,int vcels = 50);
	~CGlassObject() 
	{
		m_captureBuffer->releaseReference();
	};

	virtual CModifier::MODIFIER_TYPE getType(void) const 
	{ return CModifier::CGL_GEOMETRIC_MODIFIER; };

	void glClipRender(void) { glRender(); };
	void glRender(void);

private:
	float		m_orgx;
	float		m_orgy;

	ITextureObject	*m_captureBuffer;
	CShader			*m_pShader;
};

CGlassObject::CGlassObject(float width,float height,int hcels,int vcels)
	:CModifier(CGL_TIME_LINEAR,0,1,0,0,NULL,CModifier::CModifierClassID::GetClassId())
{
	m_orgx = -width * 0.5f;
	m_orgy = -height * 0.5f;

	setBoundingBox(	GL_COORD_VERTEX(m_orgx,m_orgy,-0.1f,1.0f),
					GL_COORD_VERTEX(-m_orgx,-m_orgy,0.1f,1.0f));

	CTextureFactory &f = CTextureFactory::getDefaultFactory();
    m_captureBuffer = f.glCreateDynamicTexture(	ITextureObject::CGL_COLOR24_ALPHA,
												ITextureObject::CGL_BILINEAR,
												CRaptorDisplay::GetCurrentDisplay());
    f.glResizeTexture(m_captureBuffer,BASE_WARP_WIDTH,BASE_WARP_HEIGHT);

	ITextureObject* T = f.glCreateTexture( ITextureObject::CGL_COLOR24_ALPHA,
                                           ITextureObject::CGL_BILINEAR);
	f.glLoadTexture(T,"Datas\\Bump2.tga");

	m_pShader = new CShader("GLASS_SHADER");
	CTextureUnitSetup *pSetup = m_pShader->glGetTextureUnitsSetup();
	pSetup->setDiffuseMap(m_captureBuffer, CTextureUnitSetup::CGL_OPAQUE);
	pSetup->setNormalMap(T, CTextureUnitSetup::CGL_OPAQUE);

#if defined(GL_ARB_fragment_shader)
	string program = 
"#version 120							\n\
uniform	sampler2D diffuseMap;			\n\
uniform	sampler2D normalMap;			\n\
										\n\
void main (void)						\n\
{										\n\
	vec4 offset = 0.04 * (texture2D(normalMap,vec2(gl_TexCoord[1].st)) - 0.5); \n\
	gl_FragColor = texture2D(diffuseMap,vec2(offset.rg) + vec2(gl_TexCoord[0].st)); \n\
}";

	CFragmentShader *fp = m_pShader->glGetOpenGLShader()->glGetFragmentShader();
	fp->glLoadProgram(program);
	CProgramParameters params;
	params.addParameter("diffuseMap",CTextureUnitSetup::IMAGE_UNIT_0);
	params.addParameter("normalMap",CTextureUnitSetup::IMAGE_UNIT_1);
	m_pShader->glGetOpenGLShader()->glCompileShader();
	m_pShader->glGetOpenGLShader()->setProgramParameters(params);
#endif
}


void CGlassObject::glRender()
{
	GL_COORD_VERTEX v1( m_orgx , m_orgy , 0.0f , 1.0f );
	GL_COORD_VERTEX v2( -m_orgx , -m_orgy , 0.0f , 1.0f );

	C3DEngine::Get3DEngine()->glProject(v1);
	C3DEngine::Get3DEngine()->glProject(v2);

	float x_sz = (v2.x - v1.x)/BASE_WARP_WIDTH;
	float y_sz = (v2.y - v1.y)/BASE_WARP_HEIGHT;

	//	save GPU time, just copy pixels that will be shaded
	int w = x_sz * m_captureBuffer->getWidth();
	int h = y_sz * m_captureBuffer->getHeight();
    m_captureBuffer->setGenerationSize(floor(v1.x+0.5f), floor(v1.y+0.5f), w+2, h+2);
	m_captureBuffer->glvkRender();
	m_pShader->glRenderTexture();
	m_pShader->glRender();
	
	//	Use a default shader to create a simple geometry
	CVertexProgram s("GL_SHADER");

	glBegin(GL_QUADS);
		glTexCoord2f(x_sz,y_sz);
		s.glMultiTexCoord(CProgramParameters::TEXCOORD1, GL_COORD_VERTEX(1, 1, 0, 1));
		glVertex3f(-m_orgx,-m_orgy,0.0f);

		glTexCoord2f(0,y_sz);
		s.glMultiTexCoord(CProgramParameters::TEXCOORD1, GL_COORD_VERTEX(0, 1, 0, 1));
		glVertex3f(m_orgx,-m_orgy,0.0f);

		glTexCoord2f(0,0);
		s.glMultiTexCoord(CProgramParameters::TEXCOORD1, GL_COORD_VERTEX(0, 0, 0, 1));
		glVertex3f(m_orgx,m_orgy,0.0f);

		glTexCoord2f(x_sz,0);
		s.glMultiTexCoord(CProgramParameters::TEXCOORD1, GL_COORD_VERTEX(1, 0, 0, 1));
		glVertex3f(-m_orgx,m_orgy,0.0f);
	glEnd();

	m_pShader->glStop();

	const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();
	pExtensions->glActiveTextureARB(GL_TEXTURE1_ARB);
	glDisable(GL_TEXTURE_2D);
	pExtensions->glActiveTextureARB(GL_TEXTURE0_ARB);
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CWarpingDisplay::CWarpingDisplay():
	m_pScene(NULL),
	m_pBall(NULL),
	m_pWarp(NULL),
	m_pGlass(NULL),
	m_pPlane(NULL)
{

}

CWarpingDisplay::~CWarpingDisplay()
{
    // Object Destructions are done at Raptor exit.
}

void CWarpingDisplay::Init()
{
	CGenericDisplay::Init();

	CWarpObject *warp = new CWarpObject(5.0f,5.0f,20,20);
	m_pWarp = new CObject3DInstance(warp);
	CGlassObject *glass = new CGlassObject(5.0f,5.0f,50,50);
	m_pGlass = new CObject3DInstance(glass);
	CBackground *back = new CBackground();
	m_pPlane = new CObject3DInstance(back);
	m_pPlane->translate(0.0f,0.0f,-50.0f);

	CShadedGeometry *ball = NULL;
	CPersistence *p = CPersistence::FindObject("GeoSphere0");
	if (p->getId().isSubClassOf(CShadedGeometry::CShadedGeometryClassID::GetClassId()))
		ball = (CShadedGeometry *)p;

	m_pBall = new CObject3DInstance(ball);
	m_pBall->translate(0.0f,-1.0f,-20.0f);
	m_pScene = new C3DScene("WARPING_SCENE");
	//m_pScene->useZSort();
	m_pScene->addObject(m_pPlane);
	m_pScene->addObject(m_pBall);
	m_pScene->addObject(m_pGlass);
	m_pScene->addObject(m_pWarp);

	CRaptorDisplay* pDisplay = CRaptorDisplay::GetCurrentDisplay();
	pDisplay->addScene(m_pScene);
}


void CWarpingDisplay::ReInit()
{
	CGenericDisplay::ReInit();

	((CWarpObject*)(m_pWarp->getObject()))->animate(true);
	((CGlassObject*)(m_pGlass->getObject()))->animate(true);
	glEnable(GL_NORMALIZE);

    CRaptorDisplay* const pDisplay = CRaptorDisplay::GetCurrentDisplay();
	IRenderingProperties &rp = pDisplay->getRenderingProperties();
	rp.setLighting(IRenderingProperties::DISABLE);
	rp.setTexturing(IRenderingProperties::ENABLE);

	pDisplay->selectScene("WARPING_SCENE");
}

void CWarpingDisplay::UnInit()
{
}

void CWarpingDisplay::Display()
{
	if (reinit)
		ReInit();
    
	glColor4f(1.0f,1.0f,1.0f,1.0f);

	m_pBall->translateAbsolute(10 * sin(TWO_PI * 4*dt),5 + 10 * cos(TWO_PI * 4*dt),-20.0f);
	m_pBall->rotationY(0.5f);

	m_pGlass->translateAbsolute(5 * cos(PI + TWO_PI * dt),5 * sin(PI + TWO_PI * dt),-5);
	m_pWarp->translateAbsolute(5 * cos(TWO_PI * dt),5 * sin(TWO_PI * dt),-5);

	dt+=0.0002f;
	if (dt>1.0) dt=0.0;
}