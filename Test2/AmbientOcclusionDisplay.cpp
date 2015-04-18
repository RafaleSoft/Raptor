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
#include "Engine/ViewModifier.h"
#include "Engine/3DEngineMatrix.h"
#include "GLHierarchy/SimpleObject.h"
#include "GLHierarchy/ShadedGeometry.h"
#include "GLHierarchy/GeometryEditor.h"
#include "GLHierarchy/RenderingProperties.h"
#include "GLHierarchy/Object3DInstance.h"
#include "GLHierarchy/Shader.h"
#include "GLHierarchy/VertexProgram.h"
#include "GLHierarchy/FragmentProgram.h"
#include "GLHierarchy/TextureFactory.h"
#include "GLHierarchy/TextureFactoryConfig.h"
#include "GLHierarchy/TextureSet.h"
#include "GLHierarchy/TextureUnitSetup.h"
#include "GLHierarchy/Light.h"

#include "Toolbox/BasicObjects.h"


string AO_vp_src =
"varying float color; \
uniform sampler2D AOMap; \
void main (void) \
{\
	gl_Position = ftransform(); \
	vec2 tc = 0.5 * (vec2(1.0,1.0) + gl_MultiTexCoord2); \
	vec4 ao = texture2D(AOMap,tc); \
	color = gl_Color.r - (1.0 - ao.r); \
}\
";

string AO_fp_src = 
"varying float color; \
void main (void) \
{\
	gl_FragColor = vec4(color,color,color,1.0); \
}\
";


string AO_vp_src2 =
"varying float color; \
void main (void) \
{\
	gl_Position = ftransform(); \
	gl_TexCoord[0] = gl_MultiTexCoord0; \
	color = gl_Color.r; \
}\
";

string AO_fp_src2 = 
"varying float color; \
uniform sampler2D diffuse; \
void main (void) \
{\
	vec4 D = vec4(color,color,color,1.0); \
	gl_FragColor = vec4(D.x,D.y,D.z,1.0); \
}\
";
	//vec4 D = color * texture2D(diffuse,vec2(gl_TexCoord[0].st)); \


CAmbientOcclusionDisplay::CAmbientOcclusionDisplay(void)
	:m_pTeapot(NULL),m_pMesh(NULL),m_pLight(NULL)
{
}

CAmbientOcclusionDisplay::~CAmbientOcclusionDisplay(void)
{
}

class ShowAO : public CSimpleObject
{
public:
	ShowAO(C3DScene* pScene)
		:m_pScene(pScene) 
    { 
        getProperties().setCastShadow(false); 
        getProperties().setReceiveShadow(false); 
        setBoundingBox(GL_COORD_VERTEX(-1.0f,-1.0f,-10.0f,1.0f),GL_COORD_VERTEX(1.0f,1.0f,-1.0f,1.0f));
    };

	virtual ~ShowAO() {};

	virtual void glRender();
	virtual void glClipRender() 
	{ 
		glRender(); 
	};

private:
	C3DScene* m_pScene;
};

void ShowAO::glRender()
{
	glPushAttrib(GL_ENABLE_BIT);

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(-1.33f,1.33f,-1.0f,1.0f,1.0,20.0);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glDisable(GL_LIGHTING);
	glColor4f(0.0f,0.0f,0.0f,1.0f);

	CEnvironment *pEnv = m_pScene->getEnvironment(CEnvironment::AMBIENT_OCCLUSION);
    pEnv->glRenderTexture();

	glBegin(GL_QUADS);
        glTexCoord2f(0.0f,0.0f);	glVertex3f(-1.33f,0.5f,-1.0f);
		glTexCoord2f(1.0f,0.0f);	glVertex3f(-0.83f,0.5f,-1.0f);
		glTexCoord2f(1.0f,1.0f);	glVertex3f(-0.83f,1.0f,-1.0f);
		glTexCoord2f(0.0f,1.0f);	glVertex3f(-1.33f,1.0f,-1.0f);
	glEnd();
	
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);

	glPopAttrib();
}

void CAmbientOcclusionDisplay::Init()
{
	CGenericDisplay::Init();

	CShader *AO_shader = new CShader("AO_SHADER");
	CVertexProgram *vp = AO_shader->glGetVertexProgram("AO_VP");
	CFragmentProgram *fp = AO_shader->glGetFragmentProgram("AO_FP");

	CShaderProgram::CProgramParameters ao_params;
	ao_params.addParameter("AOMap",CTextureUnitSetup::IMAGE_UNIT_2);
	vp->setProgramParameters(ao_params);

	bool res = vp->glLoadProgram(AO_vp_src);
	res &= fp->glLoadProgram(AO_fp_src);
	res &= AO_shader->glCompileShader();

	CShader *AO_shader2 = new CShader("AO_SHADER2");
	vp = AO_shader2->glGetVertexProgram("AO_VP2");
	fp = AO_shader2->glGetFragmentProgram("AO_FP2");

	CShaderProgram::CProgramParameters ao_params2;
	ao_params2.addParameter("diffuse",CTextureUnitSetup::IMAGE_UNIT_0);
	fp->setProgramParameters(ao_params2);

	res = vp->glLoadProgram(AO_vp_src2);
	res &= fp->glLoadProgram(AO_fp_src2);
	res &= AO_shader2->glCompileShader();

	m_pLight = new CLight("AOLight");
    m_pLight->setAmbient(1.0f,1.0f,1.0f,1.0f);
    m_pLight->setDiffuse(1.0f,1.0f,1.0f,1.0f);
	m_pLight->setSpecular(1.0f,1.0f,1.0f,1.0f);
	m_pLight->setLightPosition(GL_COORD_VERTEX(0.0,0.0,500.0,1.0));
	m_pLight->setLightDirection(GL_COORD_VERTEX(0.0,0.0,0.0,1.0));

	//CPersistence *p = CPersistence::FindObject("Teapot01");
	CPersistence *p = CPersistence::FindObject("Knot");

	if (p->getId().isSubClassOf(CGeometry::CGeometryClassID::GetClassId()))
	{
		CBasicObjects::CGeoSphere* sphere = new CBasicObjects::CGeoSphere();
		sphere->setDimensions(5.0f,36,18);

		m_pTeapot = new CShadedGeometry();
		*m_pTeapot = *((CGeometry *)p);
		m_pTeapot->translateAbsolute(0,0,0);
		//m_pTeapot = sphere;
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
		m_pTeapot->getRenderingModel().addModel(CGeometry::CRenderingModel::CGL_TANGENTS);
		m_pTeapot->setShader(CShader::glGetShader("BUMP_SHADER"));
		CTextureFactory &f = CTextureFactory::getDefaultFactory();
		CTextureObject*	m_pTexture = f.glCreateTexture(CTextureObject::CGL_COLOR24_ALPHA,CTextureObject::CGL_ALPHA_TRANSPARENT,CTextureObject::CGL_BILINEAR);
		f.glLoadTexture(m_pTexture,"Datas/MARBLE6.JPG");
		CTextureUnitSetup *tus = m_pTeapot->getShader()->glGetTextureUnitsSetup();
		tus->setDiffuseMap(m_pTexture);
		m_pTexture = f.glCreateTexture(CTextureObject::CGL_COLOR24_ALPHA,CTextureObject::CGL_MULTIPLY,CTextureObject::CGL_BILINEAR);
		f.glLoadTexture(m_pTexture,"Datas/BUMP4.TGA",CGL_USER_MIPMAPPED|CGL_NORMAL_MAP);
		//f.getConfig().setBumpAmplitude(4.0f);
		//f.glLoadTexture(m_pTexture,"Datas/BlurCircle.TGA",CGL_USER_MIPMAPPED|CGL_CREATE_NORMAL_MAP);
		//f.glExportTexture(m_pTexture,"testbmp.tga");
		tus->setNormalMap(m_pTexture);
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
		m_pMesh = AOMesh;

		C3DScene *m_pScene = new C3DScene("AO_SCENE");
		m_pScene->addObject(AOMesh);
		m_pScene->addObject(m_pVM->getObject());
		m_pScene->addLight(m_pLight);
		m_pScene->addObject(new ShowAO(m_pScene));

		CRaptorDisplay* pDisplay = CRaptorDisplay::GetCurrentDisplay();
		pDisplay->addScene(m_pScene);
		m_pScene->glManageEnvironment(CEnvironment::AMBIENT_OCCLUSION,0,0);
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
	CRenderingProperties *rp = pDisplay->getRenderingProperties();
	rp->setTexturing(CRenderingProperties::ENABLE);
    rp->setLighting(CRenderingProperties::ENABLE);
	//rp->setWireframe(CRenderingProperties::ENABLE);
	//rp->setCullFace(CRenderingProperties::DISABLE);

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
