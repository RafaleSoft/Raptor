// ShadowMapDisplay.cpp: implementation of the CShadowMapDisplay class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Test2.h"
#include "ShadowMapDisplay.h"

#include "../RaptorToolBox/RaptorToolBox.h"

#include "System/RaptorExtensions.h"
#include "GLHierarchy/GeometryEditor.h"
#include "GLHierarchy/GLFont.h"
#include "GLHierarchy/FragmentShader.h"
#include "GLHierarchy/SimpleObject.h"
#include "GLHierarchy/VertexShader.h"
#include "Engine/3DScene.h"
#include "GLHierarchy/TextureFactory.h"
#include "GLHierarchy/TextureFactoryConfig.h"
#include "GLHierarchy/TextureObject.h"
#include "GLHierarchy/TextureSet.h"
#include "GLHierarchy/TextureUnitSetup.h"
#include "GLHierarchy/Shader.h"
#include "GLHierarchy/Material.h"
#include "GLHierarchy/ShadedGeometry.h"
#include "GLHierarchy/3DSet.h"
#include "GLHierarchy/Object3DInstance.h"
#include "GLHierarchy\RenderingProperties.h"
#include "GLHierarchy/Light.h"
#include "System/Raptor.h"
#include "Engine/3DPath.h"
#include "Engine/ViewPoint.h"

static CShadowMapDisplay* display = NULL;
static const int NB_INSTANCES = 27;


static GL_COORD_VERTEX S;

class Ground : public CSimpleObject
{
public:
	Ground(RAPTOR_HANDLE& r1, RAPTOR_HANDLE& r2)
		:c1(r1),c2(r2),dt(0.0)
	{
		setBoundingBox(GL_COORD_VERTEX(-20.0f,-5.1f,-20.0f,1.0f),GL_COORD_VERTEX(20.0f,-5.0f,20.0f,1.0f));
	};
	virtual ~Ground() {};

	virtual void glRender();
	virtual void glClipRender() { glRender(); };

	RAPTOR_HANDLE c1;
	RAPTOR_HANDLE c2;
	float dt;
};

void Ground::glRender()
{
	glColor4f(1.0f,1.0f,1.0f,1.0f);

	if (glIsEnabled(GL_TEXTURE_2D))
		glCallList(c1.handle);

	CVertexShader s("GL_SHADER");
	GL_COORD_VERTEX texCoord1(0,0,0,1);
	GL_COORD_VERTEX position(-20.0f,-5.0f,-20.0f,1.0f);

	glDisable(GL_LIGHTING);
	glNormal3f(1.0f,1.0f,1.0f);
    glColor4f(1.0f,1.0f,1.0f,1.0f);
	glBegin(GL_QUADS);
		texCoord1.x = 0.0; texCoord1.y = 0.0;
		s.glMultiTexCoord(CVertexShader::TEXCOORD1,texCoord1);
		s.glMultiTexCoord(CVertexShader::TEXCOORD0,texCoord1);
		s.glVertex(position);

		texCoord1.x = 4.0; texCoord1.y = 0.0;
		s.glMultiTexCoord(CVertexShader::TEXCOORD1,texCoord1);
		s.glMultiTexCoord(CVertexShader::TEXCOORD0,texCoord1);
		position.z = 20.0f;
		s.glVertex(position);

		texCoord1.x = 4.0; texCoord1.y = 4.0;
		s.glMultiTexCoord(CVertexShader::TEXCOORD1,texCoord1);
		s.glMultiTexCoord(CVertexShader::TEXCOORD0,texCoord1);
		position.x = 20.0f;
		s.glVertex(position);

		texCoord1.x = 0.0; texCoord1.y = 4.0;
		s.glMultiTexCoord(CVertexShader::TEXCOORD1,texCoord1);
		s.glMultiTexCoord(CVertexShader::TEXCOORD0,texCoord1);
		position.z = -20.0f;
		s.glVertex(position);

        position.x = -20.0;
        texCoord1.x = 0.0; texCoord1.y = 0.0;
		s.glMultiTexCoord(CVertexShader::TEXCOORD1,texCoord1);
		s.glMultiTexCoord(CVertexShader::TEXCOORD0,texCoord1);
		s.glVertex(position);

		texCoord1.x = 4.0; texCoord1.y = 0.0;
		s.glMultiTexCoord(CVertexShader::TEXCOORD1,texCoord1);
		s.glMultiTexCoord(CVertexShader::TEXCOORD0,texCoord1);
		position.x = 20.0f;
		s.glVertex(position);

		texCoord1.x = 4.0; texCoord1.y = 4.0;
		s.glMultiTexCoord(CVertexShader::TEXCOORD1,texCoord1);
		s.glMultiTexCoord(CVertexShader::TEXCOORD0,texCoord1);
		position.y = 35.0f;
		s.glVertex(position);

		texCoord1.x = 0.0; texCoord1.y = 4.0;
		s.glMultiTexCoord(CVertexShader::TEXCOORD1,texCoord1);
		s.glMultiTexCoord(CVertexShader::TEXCOORD0,texCoord1);
		position.x = -20.0f;
		s.glVertex(position);


//        glColor4f(1.0f,0.0f,0.0f,1.0f);
        position.y = -5.0; position.z = 20.0f;
        texCoord1.x = 0.0; texCoord1.y = 0.0;
		s.glMultiTexCoord(CVertexShader::TEXCOORD1,texCoord1);
		s.glMultiTexCoord(CVertexShader::TEXCOORD0,texCoord1);
		s.glVertex(position);

		texCoord1.x = 4.0; texCoord1.y = 0.0;
		s.glMultiTexCoord(CVertexShader::TEXCOORD1,texCoord1);
		s.glMultiTexCoord(CVertexShader::TEXCOORD0,texCoord1);
		position.z = -20.0f;
		s.glVertex(position);

		texCoord1.x = 4.0; texCoord1.y = 4.0;
		s.glMultiTexCoord(CVertexShader::TEXCOORD1,texCoord1);
		s.glMultiTexCoord(CVertexShader::TEXCOORD0,texCoord1);
		position.y = 35.0f;
		s.glVertex(position);

		texCoord1.x = 0.0; texCoord1.y = 4.0;
		s.glMultiTexCoord(CVertexShader::TEXCOORD1,texCoord1);
		s.glMultiTexCoord(CVertexShader::TEXCOORD0,texCoord1);
		position.z = 20.0f;
		s.glVertex(position);

        position.y = -5.0; position.z = -20.0f; position.x = 20.0f;
        texCoord1.x = 0.0; texCoord1.y = 0.0;
		s.glMultiTexCoord(CVertexShader::TEXCOORD1,texCoord1);
		s.glMultiTexCoord(CVertexShader::TEXCOORD0,texCoord1);
		s.glVertex(position);

		texCoord1.x = 4.0; texCoord1.y = 0.0;
		s.glMultiTexCoord(CVertexShader::TEXCOORD1,texCoord1);
		s.glMultiTexCoord(CVertexShader::TEXCOORD0,texCoord1);
		position.z = 20.0f;
		s.glVertex(position);

		texCoord1.x = 4.0; texCoord1.y = 4.0;
		s.glMultiTexCoord(CVertexShader::TEXCOORD1,texCoord1);
		s.glMultiTexCoord(CVertexShader::TEXCOORD0,texCoord1);
		position.y = 35.0f;
		s.glVertex(position);

		texCoord1.x = 0.0; texCoord1.y = 4.0;
		s.glMultiTexCoord(CVertexShader::TEXCOORD1,texCoord1);
		s.glMultiTexCoord(CVertexShader::TEXCOORD0,texCoord1);
		position.z = -20.0f;
		s.glVertex(position);

	glEnd();
	glEnable(GL_LIGHTING);

	if (glIsEnabled(GL_TEXTURE_2D))
		glCallList(c2.handle);

	dt+=0.001f;
	if (dt>1.0) dt=0.0;
}


class ShowPBuffer : public CSimpleObject
{
public:
	ShowPBuffer(C3DScene* pScene)
		:m_pScene(pScene) 
    { 
        getProperties().setCastShadow(false); 
        getProperties().setReceiveShadow(false); 
        setBoundingBox(GL_COORD_VERTEX(-1.0f,-1.0f,-1.0f,1.0f),GL_COORD_VERTEX(1.0f,1.0f,1.0f,1.0f));
    };

	virtual ~ShowPBuffer() {};

	virtual void glRender();
	virtual void glClipRender() { glRender(); };

private:
	C3DScene* m_pScene;
};

void ShowPBuffer::glRender()
{
	glPushAttrib(GL_ENABLE_BIT);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(-1.33f,1.33f,-1.0f,1.0f,-1.0,1.0);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
	glColor4f(1.0f,1.0f,1.0f,1.0f);

    CEnvironment *pEnv = m_pScene->getEnvironment(CEnvironment::SHADOW_MAP);
    pEnv->glRenderTexture();

	glBegin(GL_QUADS);
        glTexCoord2f(0.0f,0.0f);	glVertex3f(-1.33f,0.5f,0.0f);
		glTexCoord2f(1.0f,0.0f);	glVertex3f(-0.83f,0.5f,0.0f);
		glTexCoord2f(1.0f,1.0f);	glVertex3f(-0.83f,1.0f,0.0f);
		glTexCoord2f(0.0f,1.0f);	glVertex3f(-1.33f,1.0f,0.0f);

/*
        // ma = rz = -1
        //  back faces
        glTexCoord4f(-1.0f,-1.0f,-0.7f,1.0f);	glVertex3f(-1.33f,0.5f,0.0f);   //  sc = -rx = -1; tc = -ry = -1;  s = 0 ; t = 0
		glTexCoord4f(1.0f,-1.0f,-0.7f,1.0f);	glVertex3f(-0.83f,0.5f,0.0f);   //  sc = 1 ; tc = 1 ;  s = 1 ; t = 1
		glTexCoord4f(1.0f,1.0f,-0.7f,1.0f);	    glVertex3f(-0.83f,1.0f,0.0f);
		glTexCoord4f(-1.0f,1.0f,-0.7f,1.0f);	glVertex3f(-1.33f,1.0f,0.0f);

        // front faces
		glTexCoord4f(-1.0f,-1.0f,0.7f,1.0f);	glVertex3f(-0.83f,0.5f,0.0f);
		glTexCoord4f(1.0f,-1.0f,0.7f,1.0f);	    glVertex3f(-0.33f,0.5f,0.0f);
		glTexCoord4f(1.0f,1.0f,0.7f,1.0f);	    glVertex3f(-0.33f,1.0f,0.0f);
		glTexCoord4f(-1.0f,1.0f,0.7f,1.0f);	    glVertex3f(-0.83f,1.0f,0.0f);


        // down faces
        glTexCoord4f(-1.0f,-0.7f,-1.0f,1.0f);	glVertex3f(0.33f,0.5f,0.0f);
		glTexCoord4f(-1.0f,-0.7f,1.0f,1.0f);	glVertex3f(0.83f,0.5f,0.0f);
		glTexCoord4f(1.0f,-0.7f,1.0f,1.0f);	    glVertex3f(0.83f,1.0f,0.0f);
		glTexCoord4f(1.0f,-0.7f,-1.0f,1.0f);	glVertex3f(0.33f,1.0f,0.0f);

        // up faces
        glTexCoord4f(-1.0f,0.7f,-1.0f,1.0f);	glVertex3f(0.83f,0.5f,0.0f);    // sc = rz = -1 ; tc = -ry = 1 ; s = 0 ; t = 1
		glTexCoord4f(-1.0f,0.7f,1.0f,1.0f);	    glVertex3f(1.33f,0.5f,0.0f);    // sc = 1 ; tc = 1 ; s = 1 ; t = 1
		glTexCoord4f(1.0f,0.7f,1.0f,1.0f);	    glVertex3f(1.33f,1.0f,0.0f);
		glTexCoord4f(1.0f,0.7f,-1.0f,1.0f);	    glVertex3f(0.83f,1.0f,0.0f);
*/
	glEnd();
	
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);

	glPopAttrib();
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CShadowMapDisplay::CShadowMapDisplay():
	m_light(NULL),
	redMat(NULL),
	redBall(NULL),
	greenBall(NULL),
	greenMat(NULL)
{
	display = this;

	m_pInstances = new CObject3DInstance*[NB_INSTANCES];
	for (int i=0;i<NB_INSTANCES;i++)
		m_pInstances[i] = NULL;
	pSimpleObject = NULL;
}

CShadowMapDisplay::~CShadowMapDisplay()
{
    // Done at Raptor exit.
    delete [] m_pInstances;
}

void CShadowMapDisplay::Init()
{
	CGenericDisplay::Init();

	CPersistence *p = CPersistence::FindObject("main_textures");

	CTextureFactory &f = CTextureFactory::getDefaultFactory();
	if (p->getId().isSubClassOf(CTextureSet::CTextureSetClassID::GetClassId()))
		 texture = (CTextureSet *)p;

	CShader *redShader = new CShader;
	CShader *greenShader = new CShader;
	redMat = redShader->getMaterial();
	redMat->setAmbient(0.5f,0.0f,0.0f,1.0f);
	redMat->setDiffuse(1.0f,0.0f,0.0f,1.0f);
	redMat->setSpecular(1.0f,1.0f,1.0f,1.0f);
	redMat->setShininess(50.0f);
	redShader->setColor(1.0f,1.0f,1.0f,1.0f);

	greenMat = greenShader->getMaterial();
	greenMat->setAmbient(0.0f,0.5f,0.0f,1.0f);
	greenMat->setDiffuse(0.0f,1.0f,0.0f,1.0f);
	greenMat->setSpecular(1.0f,1.0f,1.0f,1.0f);
	greenMat->setShininess(50.0f);
	greenShader->setColor(1.0f,1.0f,1.0f,1.0f);
	
	redBall = new CShadedGeometry("red ball");
	greenBall = new CShadedGeometry("green ball");
	CGeometry *obj = NULL;
	C3DSet *set = NULL;
	
    CRaptorToolBox::load3DStudioScene("Datas\\smallTeapot.3DS",set,NULL);

    C3DSet::C3DSetIterator it = set->getIterator();
	p = (CGeometry*)(set->getChild(it++));
	if (p->getId().isSubClassOf(CGeometry::CGeometryClassID::GetClassId()))
	{
		obj = (CGeometry *)p;
		obj->translateAbsolute(0,0,0);
		obj->rotationX(-90.0f);
        CGeometry::CRenderingModel l_model(CGeometry::CRenderingModel::CGL_FRONT_GEOMETRY);
        l_model.addModel(CGeometry::CRenderingModel::CGL_NORMALS);
        l_model.addModel(CGeometry::CRenderingModel::CGL_TEXTURE);
		obj->setRenderingModel(l_model);
		
		*redBall = *obj;
        const CGeometryEditor &pEditor = redBall->getEditor();
		if (!pEditor.isMinimized())
			pEditor.minimize();
		pEditor.genNormals(true);
		redBall->scale(0.5f,0.5f,0.5f);
		redBall->setShader(redShader);

		*greenBall = *obj;
		const CGeometryEditor &pEditor2 = greenBall->getEditor();
		if (!pEditor.isMinimized())
			pEditor2.minimize();
		pEditor2.genNormals(true);
		greenBall->scale(0.5f,0.5f,0.5f);
		greenBall->setShader(greenShader);
	}


	CGenericVector<float> LL(-1.0f,-1.0f,0.0f,1.0f);
	GL_COORD_VERTEX S(8.0f,8.0f,0.0f,1.0f);
	LL.Normalize();
    GL_COORD_VERTEX L(LL.X(),LL.Y(),LL.Z(),LL.H());

	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER ,GL_FALSE);

	m_light = new CLight();
	m_light->setLightPosition(S);
	m_light->setLightDirection(L);
	m_light->setAmbient(0.2f,0.2f,0.2f,1.0f);
	m_light->setDiffuse(0.8f,0.8f,0.8f,0.8f);
	m_light->setSpecular(1.0f,1.0f,1.0f,1.0f);

	string fname = "Datas\\oldwood.jpg";

    CTextureFactoryConfig& config = f.getConfig();
    config.setCurrentAnisotropy(16.0f);

	CTextureObject *T = f.glCreateTexture(	CTextureObject::CGL_COLOR24_ALPHA,
											CTextureObject::CGL_ALPHA_TRANSPARENT,
											CTextureObject::CGL_ANISOTROPIC);
	T->glSetTransparency(255);
	f.glLoadTexture(T,"Datas\\oldwood.jpg");
    T->selectMipMapLevel(1);
    f.glLoadTexture(T,"Datas\\oldwood2.jpg");
    T->selectMipMapLevel(2);
    f.glLoadTexture(T,"Datas\\oldwood3.jpg");
    T->selectMipMapLevel(3);
    f.glLoadTexture(T,"Datas\\oldwood4.jpg");
    T->selectMipMapLevel(4);
    f.glLoadTexture(T,"Datas\\oldwood5.jpg");
    T->selectMipMapLevel(5);
    f.glLoadTexture(T,"Datas\\oldwood6.jpg");
    T->selectMipMapLevel(6);
    f.glLoadTexture(T,"Datas\\oldwood7.jpg");
    T->selectMipMapLevel(7);
    f.glLoadTexture(T,"Datas\\oldwood8.jpg");
    T->selectMipMapLevel(8);
    f.glLoadTexture(T,"Datas\\oldwood9.jpg");
    T->selectMipMapLevel(9);
    f.glLoadTexture(T,"Datas\\oldwood10.jpg");

    config.setCurrentAnisotropy(1.0f);
    

	CTextureUnitSetup tmu;
	tmu.setDiffuseMap(T); 
	tmu.enableImageUnit(CTextureUnitSetup::IMAGE_UNIT_2,false);
	c1 = tmu.glBuildSetup();

   	fname = "Datas\\Start.tga";

	T = f.glCreateTexture(	CTextureObject::CGL_COLOR24_ALPHA,
							CTextureObject::CGL_ALPHA_TRANSPARENT,
							CTextureObject::CGL_BILINEAR);
	T->glSetTransparency(255);
	f.glLoadTexture(T,fname);

	tmu.getTMUCombiner(CTextureUnitSetup::IMAGE_UNIT_0).rgb_combiner = true;
	tmu.getTMUCombiner(CTextureUnitSetup::IMAGE_UNIT_0).alpha_combiner = false;
	tmu.getTMUCombiner(CTextureUnitSetup::IMAGE_UNIT_0).rgb_op = GL_MODULATE;
	tmu.getTMUCombiner(CTextureUnitSetup::IMAGE_UNIT_0).src_rgb_0 = GL_TEXTURE;
#if defined(GL_ARB_texture_env_combine)
	tmu.getTMUCombiner(CTextureUnitSetup::IMAGE_UNIT_0).src_rgb_1 = GL_PREVIOUS_ARB;
#endif
	tmu.setDiffuseMap(T);
	tmu.enableImageUnit(CTextureUnitSetup::IMAGE_UNIT_2,false);
	c2 = tmu.glBuildSetup();

	int nbI = 0;
    int i=0;	
	for (i=0;i<3;i++)
	{
		for (int j=0;j<3;j++)
		{
			for (int k=0;k<3;k++)
			{
				if (((i+j+k) % 2) == 0)
					m_pInstances[nbI] = new CObject3DInstance(redBall);
				else
					m_pInstances[nbI] = new CObject3DInstance(greenBall);
				m_pInstances[nbI++]->translate(i*3.0f - 3.0f,j*3.0f,k*3.0f - 3.0f);
			}
		}
	}

	pSimpleObject = new Ground(c1,c2);
	m_pScene = new C3DScene("SHADOW_MAP_SCENE");
	pSimpleObject2 = new ShowPBuffer(m_pScene);
	
	m_pScene->useZSort();
	m_pScene->addObject(pSimpleObject2);
	m_pScene->addLight(m_light);
	m_pScene->addObject(pSimpleObject);
	for (i=0;i<NB_INSTANCES;i++)
		m_pScene->addObject(m_pInstances[i]);

    m_pScene->glManageEnvironment(CEnvironment::SHADOW_MAP,1024,1024);
    //m_pScene->glManageEnvironment(CEnvironment::OMNI_SHADOW_MAP,1024,1024);

	C3DPath *eyePositionPath = new C3DPath();
	C3DPath *targetPath = new C3DPath();
	eyePositionPath->setPathType(C3DPath::CGL_CATMULLROM);
	targetPath->setPathType(C3DPath::CGL_CATMULLROM);

	for (i=0;i<9;i++)
		eyePositionPath->addKnot(10.0*sin(i/4.0*PI),4.0,10.0*cos(i/4.0*PI),i/8.0);

	targetPath->addKnot(0.0f,0.0f,0.0f,0.0f);
	targetPath->addKnot(0.0f,0.0f,0.0f,0.25f);
	targetPath->addKnot(0.0f,0.0f,0.0f,0.5f);
	targetPath->addKnot(0.0f,0.0f,0.0f,0.75f);
	targetPath->addKnot(0.0f,0.0f,0.0f,1.0f);

	vp = new CViewPoint();
    vp->setPosition(0.0,4.0,10.0,CViewPoint::EYE);
    vp->setPosition(0.0,0.0,0.0,CViewPoint::TARGET);
	vp->setViewVolume(-1.33f,1.33f,-1.0f,1.0f,1.0f,10000.0f,CViewPoint::PERSPECTIVE);
	vp->addPath(eyePositionPath,targetPath);
	vp->setCurrentPath(0);
	vp->setTimeInterval(0.0f,20.0f);

	CRaptorDisplay* pDisplay = CRaptorDisplay::GetCurrentDisplay();
	pDisplay->addScene(m_pScene);
}

void CShadowMapDisplay::UnInit()
{
}

void CShadowMapDisplay::ReInit()
{
	CGenericDisplay::ReInit();

	glDisable(GL_LIGHT0);
    glDisable(GL_LIGHT1);
    glDisable(GL_LIGHT2);
    glDisable(GL_LIGHT3);

	m_light->glDeActivate();
	m_light->glActivate();
	m_light->glRender();

	vp->animate(true);

	CRaptorDisplay* const pDisplay = CRaptorDisplay::GetCurrentDisplay();
	CRenderingProperties *rp = pDisplay->getRenderingProperties();
    rp->setTexturing(CRenderingProperties::ENABLE);
	rp->setLighting(CRenderingProperties::ENABLE);
	pDisplay->setViewPoint(vp);
	pDisplay->selectScene("SHADOW_MAP_SCENE");
}

void CShadowMapDisplay::Display()
{
	if (reinit)
		ReInit();

	float dt = ((Ground*)pSimpleObject)->dt;
    S.x = 10.0 * cos (2*PI*dt);
    S.y = 10.0f;
    S.z = 10.0 * sin(2*PI*dt);
    S.h = 1.0f;

    m_light->setLightPosition(S);
	GL_COORD_VERTEX L(-S.x,-S.y,-S.z,1.0f);
	m_light->setLightDirection(L);

    CGenericMatrix<float> M;
    CGenericVector<float> V(S);
    glGetFloatv(GL_TRANSPOSE_MODELVIEW_MATRIX_ARB,M.matrix());
    V *= M;
  
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_LIGHTING);
   
    glBegin(GL_LINES);
        glColor4f(1.0f,1.0f,1.0f,1.0f);
        glVertex4f(0.0f,0.0f,0.0f,1.0f);
        glVertex4fv(S);
    glEnd();
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_LIGHTING);

    S.x = V.X();
    S.y = V.Y();
    S.z = V.Z();
}