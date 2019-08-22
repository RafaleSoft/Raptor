// SkinningDisplay.cpp: implementation of the CSkinningDisplay class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SkinningDisplay.h"

#include "ToolBox/RaptorToolBox.h"

#include "Raptordll.h"
#include "GLHierarchy/GLFont.h"
#include "GLHierarchy/GLFontFactory.h"
#include "GLHierarchy/Persistence.h"
#include "GLHierarchy/3DSet.h"
#include "Engine/3DScene.h"
#include "GLHierarchy/TextureSet.h"
#include "GLHierarchy/TextureFactory.h"
#include "GLHierarchy/ShadedGeometry.h"
#include "GLHierarchy/TextureObject.h"
#include "GLHierarchy/TextureUnitSetup.h"
#include "GLHierarchy/Shader.h"
#include "GLHierarchy/SimpleObject.h"
#include "GLHierarchy/VertexShader.h"
#include "GLHierarchy/FragmentProgram.h"
#include "Engine/ImageModifier.h"
#include "System/Raptor.h"
#include "GLHierarchy/IRenderingProperties.h"
#include "GLHierarchy/Object3DInstance.h"
#include "SSE_Engine/SSE_GLLayer.h"


class MyModifier : public CImageModifier
{
public:
    MyModifier():CImageModifier(CModifier::CGL_TIME_CONSTANT) {};
    virtual ~MyModifier() {};

    virtual void onPrepareBuffer(void)
    {
        unsigned char *src = getFrontBuffer();
	    //	add ramdom points
	    for (int j=0;j<20;j++)
	    {
		    int pos = 4 * (rand() % 32 + 16) + (64 * 4) * ( 24 + (rand() % 16));
			*((unsigned int*)(src + pos)) = 0xFFFFFFFF;
	    }
    }
};

class SkinningBackGround : public CSimpleObject
{
public:
	SkinningBackGround(CTextureObject *background, CGLLayer	*layer)
		:m_background(background), m_layer(layer)
	{
		setBoundingBox(GL_COORD_VERTEX(-10.0f, -10.0f, -15.1f, 1.0f), GL_COORD_VERTEX(10.0f, 10.0f, -15.0f, 1.0f));

		bg.handle(glGenLists(1));
		glNewList(bg.handle(), GL_COMPILE);
			glBegin(GL_QUADS);
			glTexCoord2f(0.0f, 0.0f);
			glVertex3f(-10.0f, -10.0f, 0.0f);
			glTexCoord2f(1.0f, 0.0f);
			glVertex3f(10.0f, -10.0f, 0.0f);
			glTexCoord2f(1.0f, 2.0f);
			glVertex3f(10.0f, 10.0f, 0.0f);
			glTexCoord2f(0.0f, 2.0f);
			glVertex3f(-10.0f, 10.0f, 0.0f);
			glEnd();
		glEndList();
	};

	virtual ~SkinningBackGround() {};

	virtual void glRender();
	virtual void glClipRender() { glRender(); };

private:
	SkinningBackGround();

	CGLLayer		*m_layer;
	CTextureObject *m_background;
	RAPTOR_HANDLE	bg;
};

void SkinningBackGround::glRender()
{
	m_layer->glRender();

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	m_background->glvkRender();
		
	float dt = CTimeObject::GetGlobalTime();
	glPushMatrix();
		glRotatef(360.0f*dt, 0, 0, 1);
		glTranslatef(0, 0, -15.0f);
		glCallList(bg.handle());
	glPopMatrix();
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSkinningDisplay::CSkinningDisplay()
	:modifier(NULL),layer(NULL),
	skinningMatrix(GL_MATRIX())
{

}

CSkinningDisplay::~CSkinningDisplay()
{
}

void CSkinningDisplay::Init()
{
	CGenericDisplay::Init();

	C3DSet	*set;
	GL_COORD_VERTEX c;

	CPersistence *p = CPersistence::FindObject("main_textures");

	if (p->getId().isSubClassOf(CTextureSet::CTextureSetClassID::GetClassId()))
		 texture = (CTextureSet *)p;

	CGL2DFont *font = CGLFontFactory::create2DFont("Datas\\kld.ttf", 20, "main_font");

    CRaptorToolBox::SCENE_LOADER_OPTIONS options;
    CRaptorToolBox::load3DStudioScene("Datas\\Cylinder.3DS",set,&options);

    C3DSet::C3DSetIterator it = set->getIterator();
	tube = (CShadedGeometry*)(set->getChild(it++));
    CShader *s = tube->getShader();
	s->setColor(0.2f,0.7f,0.8f,1.0f);
	CTextureUnitSetup *ts = s->glGetTextureUnitsSetup();
	ts->setDiffuseMap(texture->getTexture(0));

	tube->scale(0.1f,0.1f,0.1f);
	tube->rotationX(-90.0f);
	tube->getCenter(c);
	tube->translate(-c.x,-c.y,-c.z);
    CGeometry::CRenderingModel l_model(CGeometry::CRenderingModel::CGL_FRONT_GEOMETRY);
    l_model.addModel(CGeometry::CRenderingModel::CGL_NORMALS);
    l_model.addModel(CGeometry::CRenderingModel::CGL_TEXTURE);
	tube->setRenderingModel(l_model);

	tube2 = new CShadedGeometry();
	*tube2 = *tube;

	GL_COORD_VERTEX m,M;
	GL_VERTEX_DATA v;
	tube2->getBoundingBox(m,M);

    l_model.addModel(CGeometry::CRenderingModel::CGL_WEIGHT);
    tube2->setRenderingModel(l_model);
	int nbV = tube2->nbVertex();
    tube2->glSetWeights(nbV);
	tube2->glLockData();
	for (int i=0;i<nbV;i++)
	{
		tube2->getVertex(i,v);
		tube2->setWeight(i,(v.vertex.y-m.y)/(M.y-m.y));
	}
	tube2->glUnLockData();

	layer = new CSSE_GLLayer(100,100,200,200);
	layer->clear(0xC0008000);

	layer->drawLine(100,100,160,180,0x80FF0000);
	layer->drawLine(100,100,160,20,0x80FF0000);
	layer->drawLine(100,100,40,180,0x80FF0000);
	layer->drawLine(100,100,40,20,0x80FF0000);

	layer->drawText(50,150,"RAPTOR",font,0x80C0E000);

	layer->drawALine(100,100,180,160,0x80FF0000);
	layer->drawALine(100,100,180,40,0x80FF0000);
	layer->drawALine(100,100,20,160,0x80FF0000);
	layer->drawALine(100,100,20,40,0x80FF0000);

	layer->drawAPoint(180,160,0xFF0000FF);
	layer->drawAPoint(160,180,0xFF0000FF);
	layer->drawAPoint(180,40,0xFF0000FF);
	layer->drawAPoint(160,20,0xFF0000FF);
	layer->drawAPoint(20,160,0xFF0000FF);
	layer->drawAPoint(40,180,0xFF0000FF);
	layer->drawAPoint(20,40,0xFF0000FF);
	layer->drawAPoint(40,20,0xFF0000FF);

	layer->drawARectangle(50,50,100,100,0x800000FF);
	layer->drawPoint(100,100,0xFFFFFFFF);

	stringstream vrs;
	vrs << "Version ";
	vrs << Raptor::GetVersionString();
	layer->drawAText(20,30,vrs.str(),font,0x80C0E000);

	CTextureFactory &f = CTextureFactory::getDefaultFactory();
	t2 = f.glCreateSprite(ITextureObject::CGL_COLOR24_ALPHA);
	t2->glSetTransparency(128);
	f.glLoadTexture(t2,"Datas\\sprite.tga");

	t2->glSetTransparentColor(0,0,0);
	layer->manageSprite(t2,75,75,45);

    modifier = new MyModifier();
	modifier->selectModifierFunction(CImageModifier::CGL_BLOWFADER_MODIFIER,0x05010101);

    CTextureObject*	T = f.glCreateDynamicTexture(ITextureObject::CGL_COLOR24_ALPHA,CTextureObject::CGL_OPAQUE,ITextureObject::CGL_BILINEAR,modifier);
    f.glResizeTexture(T,64,64);
    modifier->setImage(T);
	layer->manageSprite(T,150,75,0);

    t = f.glCreateDynamicTexture(ITextureObject::CGL_COLOR24_ALPHA,CTextureObject::CGL_OPAQUE,ITextureObject::CGL_BILINEAR,CRaptorDisplay::GetCurrentDisplay());
    f.glResizeTexture(t,512,256);
	t->setGenerationSize(51,101,510,254);	// avoid artefacts on border due to bilinear filterings
	t->glvkUpdateClamping(ITextureObject::CGL_EDGECLAMP);

    string skinning_vp_src =
"uniform mat4 skinningMatrix; \
varying vec3 normal; \
attribute float weight; \
void main (void) \
{\
	gl_Position = gl_ModelViewProjectionMatrix * mix(gl_Vertex,(skinningMatrix * gl_Vertex),weight); \
	gl_TexCoord[0] = gl_MultiTexCoord0; \
	gl_FrontColor = gl_Color; \
}\
";
    string skinning_fp_src = 
"uniform sampler2D diffuseMap; \
void main (void) \
{\
    gl_FragColor = texture2DProj(diffuseMap,gl_TexCoord[0]); \
}\
";
    
    skinning  = new CShader("SKINNING_SHADER");
	tube2->setShader(skinning);
	CVertexShader *vp = skinning->glGetVertexProgram("Skinning_VP");
    CFragmentProgram *fp = skinning->glGetFragmentProgram("Skinning_FP");

    if (vp->glLoadProgram(skinning_vp_src) &&
        fp->glLoadProgram(skinning_fp_src))
	{
		params.addParameter("skinningMatrix",GL_MATRIX());
		params.addParameter("weight", CProgramParameters::WEIGHTS);

        //params2.addParameter("diffuseMap",CTextureUnitSetup::IMAGE_UNIT_0);

        skinning->glCompileShader();
	}

	C3DScene *pScene = new C3DScene("SKINNING_SCENE");
	CObject3DInstance *inst1 = new CObject3DInstance(tube);
	inst1->translate(-5.0f,0.0f,-10.0f);
	pScene->addObject(inst1);
	CObject3DInstance *inst2 = new CObject3DInstance(tube2);
	inst2->translate(5.0f,0.0f,-10.0f);
	pScene->addObject(inst2);
	pScene->addObject(new SkinningBackGround(t,layer));

	CRaptorDisplay* pDisplay = CRaptorDisplay::GetCurrentDisplay();
	pDisplay->addScene(pScene);
}

void CSkinningDisplay::ReInit()
{
	CGenericDisplay::ReInit();

    CRaptorDisplay* pDisplay = CRaptorDisplay::GetCurrentDisplay();
	IRenderingProperties &rp = pDisplay->getRenderingProperties();
	rp.setTexturing(IRenderingProperties::ENABLE);
	rp.setLighting(IRenderingProperties::DISABLE);
	rp.setBlending(IRenderingProperties::ENABLE);

	modifier->animate(true);

	pDisplay->selectScene("SKINNING_SCENE");
}

void CSkinningDisplay::UnInit()
{
}

void CSkinningDisplay::Display()
{
	if (reinit)
		ReInit();

	float dt = CTimeObject::GetGlobalTime();
	
	CGenericMatrix<float> gm,gm2;
	GL_MATRIX m;
	RZ_MATRIX(m,2*cos(2*PI*dt)*180);
	gm = m;
	RY_MATRIX(m,2*cos(1.5*PI*dt)*180);
	gm2 = m;
	gm *= gm2;

	C3DEngine::Generic_to_MATRIX(skinningMatrix.p, gm);
	CProgramParameters::CParameterBase &matrix = params[0];
	matrix.copy(skinningMatrix);
    skinning->glGetVertexProgram("Skinning_VP")->setProgramParameters(params);

	glColor4f(1.0f,1.0f,1.0f,1.0f);
	layer->manageSprite(t2,75,75,dt*360);
}
