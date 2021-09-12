#include "StdAfx.h"
#include "SOARDoc.h"
#include "SOARTerrainIndexed.h"
#include "SOARTerrainMorphed.h"

#include "System/RaptorDisplay.h"
#include "Engine/IViewPoint.h"
#include "System/Raptor.h"
#include "System/RaptorConsole.h"
#include "GLHierarchy/IRenderingProperties.h"
#include "GLHierarchy/SimpleObject.h"
#include "GLHierarchy/ShadedGeometry.h"
#include "GLHierarchy/Shader.h"
#include "GLHierarchy/Material.h"
#include "GLHierarchy/Light.h"
#include "Engine/Animator.h"
#include "Engine/3DScene.h"

#include <strstream>
using std::ostrstream;

class CTerrain : //public CSOARTerrainIndexed 
				 public CSOARTerrainMorphed
{
public:
	CTerrain();
	virtual ~CTerrain();

	virtual void glClipRender(void);

private:
	unsigned int	count;
	float			zpos;
	float			angle;
};

CTerrain::CTerrain()
{
	zpos = 0.0f;
	angle = 0.0f;
}

CTerrain::~CTerrain() 
{
}

void CTerrain::glClipRender()
{
	glPushMatrix();

	glTranslatef(0.0f,0.0f,zpos);
	//glRotatef(-70.0f,1.0f,0.0f,0.0f);
	//glRotatef(angle,0.0f,0.0f,1.0f);

	zpos = zpos - 0.02f;

	getShader()->glRenderMaterial();

	float fov = (float)(10.0 * TO_RADIAN(67.0f / 640.0f));
	VECTOR3f vp;
	float mv[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, mv);
    for (int i = 0; i != 3; i++) 
	{
		vp[i] = 0.0f;
		for (int j = 0; j != 3; j++)
			vp[i] -= mv[4 * i + j] * mv[12 + j];
	}
	//CSOARTerrainIndexed::glClipRender(fov,GL_COORD_VERTEX(vp[0],vp[1],vp[2],1.0f));
	CSOARTerrainMorphed::glClipRender(fov,GL_COORD_VERTEX(vp[0],vp[1],vp[2],1.0f));

	angle += 0.5f;
	if (angle > 360)
		angle = 0;

	glPopMatrix();
}

////////////////////////////////////////////////////////////////////////


CSOARDoc::CSOARDoc(const RAPTOR_HANDLE& wnd,CRaptorDisplay* pDisplay)
{
	m_pDisplay = pDisplay;
    m_wnd = wnd;
	m_pTerrain = NULL;
}

CSOARDoc::~CSOARDoc(void)
{
}

void CSOARDoc::GLInitContext(int argc, char* argv[])
{
    glClearColor(0.0f,0.6f,0.0f,0.0f);

	CRaptorDisplay *dsp = CRaptorDisplay::GetCurrentDisplay();
	IRenderingProperties &props = dsp->getRenderingProperties();
	props.setLighting(IRenderingProperties::ENABLE);
	props.setCullFace(IRenderingProperties::ENABLE);
	props.setTexturing(IRenderingProperties::DISABLE);
	props.setDepthTest(IRenderingProperties::ENABLE);
	props.setWireframe(IRenderingProperties::DISABLE);
	props.clear(CGL_DEPTH|CGL_RGBA);

	IViewPoint *vp = dsp->getViewPoint();
	vp->setViewVolume(-1.33f,1.33f,-1.0f,1.0f,2.0f,100000.0f,IViewPoint::PERSPECTIVE);
	vp->glvkRenderViewPointModel();
    vp->setPosition(40.0,40.0,8.0f,IViewPoint::EYE);
    vp->setPosition(40.0,40.0,0,IViewPoint::TARGET);
	vp->glvkRender();

	CAnimator::SetAnimator(new CAnimator());

	GL_COORD_VERTEX resolution(40.0f,40.0f,0.001f,1.0f);
	GL_COORD_VERTEX refPoint(0.0f,0.0f,0.0f,1.0f);
	C3DScene *pScene = dsp->getRootScene();
	m_pTerrain = new CTerrain();
	m_pTerrain->glInitFromDTED("E000N42.dt0",resolution,refPoint);

	refPoint.x = 0.0f;
	refPoint.y = 40.0f;
	refPoint.z = 0.0f;
	m_pTerrain2 = new CTerrain();
	m_pTerrain2->glInitFromDTED("E000N43.dt0",resolution,refPoint);

	refPoint.x = 40.0f;
	refPoint.y = 0.0f;
	refPoint.z = 0.0f;
	m_pTerrain3 = new CTerrain();
	m_pTerrain3->glInitFromDTED("E001N42.dt0",resolution,refPoint);

	refPoint.x = 40.0f;
	refPoint.y = 40.0f;
	refPoint.z = 0.0f;
	m_pTerrain4 = new CTerrain();
	m_pTerrain4->glInitFromDTED("E001N43.dt0",resolution,refPoint);

	m_pTerrain->setBrothers(m_pTerrain2,NULL,NULL,m_pTerrain3);
	m_pTerrain2->setBrothers(NULL,m_pTerrain,NULL,m_pTerrain4);
	m_pTerrain3->setBrothers(m_pTerrain4,NULL,m_pTerrain,NULL);
	m_pTerrain4->setBrothers(NULL,m_pTerrain3,m_pTerrain2,NULL);

	CShader *pShader = m_pTerrain->getShader();
	pShader->setColor(0.875f,0.625f,0.500f,1.0f);
	CMaterial *pMat = pShader->getMaterial();
	pMat->setAmbient(0.125f, 0.125f, 0.125f, 1.0f);
	pMat->setDiffuse(0.825f, 0.625f, 0.25f, 1.0f);
	pMat->setSpecular(0.5f, 0.5f, 0.5f, 1.0f);
	pMat->setShininess(10.0f);

	pShader = m_pTerrain2->getShader();
	pShader->setColor(0.500f,0.625f,0.875f,1.0f);
	pMat = pShader->getMaterial();
	pMat->setAmbient(0.125f, 0.125f, 0.125f, 1.0f);
	pMat->setDiffuse(0.25f, 0.625f, 0.825f, 1.0f);
	pMat->setSpecular(0.5f, 0.5f, 0.5f, 1.0f);
	pMat->setShininess(10.0f);

	pShader = m_pTerrain3->getShader();
	pShader->setColor(0.500f,0.875f,0.625f,1.0f);
	pMat = pShader->getMaterial();
	pMat->setAmbient(0.125f, 0.125f, 0.125f, 1.0f);
	pMat->setDiffuse(0.25f, 0.825f, 0.625f, 1.0f);
	pMat->setSpecular(0.5f, 0.5f, 0.5f, 1.0f);
	pMat->setShininess(10.0f);

	pShader = m_pTerrain4->getShader();
	pShader->setColor(1.0f, 1.0f, 1.0f, 1.0f);
	pMat = pShader->getMaterial();
	pMat->setAmbient(0.125f, 0.125f, 0.125f, 1.0f);
	pMat->setDiffuse(1.0f, 1.0f, 1.0f, 1.0f);
	pMat->setSpecular(0.5f, 0.5f, 0.5f, 1.0f);
	pMat->setShininess(10.0f);


	pScene->addObject(m_pTerrain);
	pScene->addObject(m_pTerrain2);
	pScene->addObject(m_pTerrain3);
	pScene->addObject(m_pTerrain4);


	CColor::RGBA white(1.0f, 1.0f, 1.0f, 1.0f );
	CLight *pLight = new CLight();
	pLight->setAmbient(white);
	pLight->setDiffuse(white);
	pLight->setSpecular(white);

	GL_COORD_VERTEX position(-10000,10000,10000,1.0);
	pLight->setLightPosition(position);
	pLight->setLightDirection(GL_COORD_VERTEX());

	pScene->addLight(pLight);
}



void CSOARDoc::glRender()
{
	CRaptorConsole *pConsole = IRaptor::GetConsole();
	pConsole->clear();
	
	CGLFont::FONT_TEXT_ITEM item;
	ostrstream s;
	s << "tri: " << m_pTerrain->getNbTriangles()+m_pTerrain2->getNbTriangles()+m_pTerrain3->getNbTriangles()+m_pTerrain4->getNbTriangles() << ends;
	item.text = s.str();
	item.x_offset = 10;
	pConsole->addItem(item);
	s.freeze(0);

	m_pDisplay->glvkBindDisplay(m_wnd);

	m_pDisplay->glRender();

	m_pDisplay->glvkUnBindDisplay();
}



