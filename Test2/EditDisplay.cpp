// EditDisplay.cpp: implementation of the CEditDisplay class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "EditDisplay.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#include "Raptordll.h"
#include "..\RaptorToolBox\RaptorToolBox.h"

#include "GLHierarchy/3DSet.h"
#include "GLHierarchy/Geometry.h"
#include "GLHierarchy/GeometryEditor.h"
#include "GLHierarchy/Object3DInstance.h"
#include "GLHierarchy/3DSqueleton.h"
//#include "GLHierarchy/StrippedGeometry.h"
#include "Engine/GLRunner.h"

static GLfloat light_ambient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
static GLfloat light_diffuse[] = { 0.8f, 0.8f, 0.2f, 1.0f };
static GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEditDisplay::CEditDisplay()
{
	strip = NULL;
    runner = NULL;
}

CEditDisplay::~CEditDisplay()
{
    // Done at Raptor exit.
    /*
	if (strip != NULL)
		delete strip;
    */
}

void CEditDisplay::Init()
{
	CGenericDisplay::Init();

	GL_VERTEX_DATA v;
	GL_COORD_VERTEX c;

    CRaptorToolBox::load3DStudioScene("Datas\\StarFlash.3DS",set,NULL);

	editObject = (CGeometry*)(set->getFirstChild());
	editObject = (CGeometry*)(editObject->GetAfter());
	editObject->GetCenter(c);
	editObject->translate(-c.x,-c.y,-c.z);
    CGeometry::CRenderingModel l_model(CGeometry::CRenderingModel::CGL_FRONT_GEOMETRY);
    l_model.addModel(CGeometry::CRenderingModel::CGL_COLORS);
	editObject->SetRenderingModel(l_model);
	editObject->RotationX(90.0f);

	int nbV = editObject->AllocColors(editObject->NbVertex());

	for (int i=0;i<nbV;i++)
	{
		editObject->GetVertex(i,v);
		float n = (float)(sqrt(v.vertex.x*v.vertex.x + v.vertex.y*v.vertex.y + v.vertex.z * v.vertex.z));
		editObject->SetColor(i,1.0f,n/10.0f,0.0f,n/20.0f);
	}

	editVertex = 0;
	editColor[0] = editColor[1] = editColor[2] = editColor[3] = 255;

	instEdit = new CObject3DInstance(editObject);

    editForce = new CPhysics::CForce;
	editForce->dirac = false;

	editObject->GetVertex(0,v);
	editForce->pos.Set(5,5,-5,1.0f);
	editForce->vect.Set(-1.0f,0.0f,1.0f,0.0f);
	editForce->position_attached = true;
	editForce->vector_attached = true;

    CRaptorToolBox::load3DStudioScene("Datas\\Arm.3DS",set,NULL);
	
	CGeometry* g = (CGeometry*)(set->getFirstChild());
	CGeometry* pivot4 = NULL;
	CGeometry* pivot2 = NULL;
	CGeometry* bras = NULL;
	CGeometry* avantbras = NULL;
	GL_COORD_VERTEX origin;
	GL_COORD_VERTEX c2;

	while (g != NULL)
	{
		if (g->getName() == "Base")
		{
			squeleton = new C3DSqueleton(g);
			g->GetCenter(origin);
			g->TranslateAbsolute(0,0,0);
		}
		else if (g->getName() == "Pivot4")
			pivot4 = g;
		else if (g->getName() == "Pivot2")
			pivot2 = g;
		else if (g->getName() == "Bras")
			bras = g;
		else if (g->getName() == "AvantBras")
			avantbras = g;

		g = (CGeometry*)(g->GetAfter());
	}

	pivot4->GetCenter(c);
	pivot4->TranslateAbsolute(0,0,0);
	bras->GetCenter(c2);
	bras->TranslateAbsolute(0,0,0);

	CGenericVector<float> pos(c.x - origin.x,c.y - origin.y,c.z - origin.z,1.0);
	CGenericVector<float> axis(0,0,1,1);
	CGenericVector<float> length(c2.x - c.x,c2.y - c.y,c2.z - c.z,1);

	object_link* lnk = squeleton->addLink(squeleton->getRootBone(),pos,axis,C3DSqueleton::PIVOT,pivot4);
	bone* b = squeleton->addBone(lnk,length,bras);

	pivot2->GetCenter(c);
	pivot2->TranslateAbsolute(0,0,0);
	avantbras->TranslateAbsolute(0,0,0);

	pos.Set(c.x - origin.x,c.y - origin.y,c.z - origin.z,1.0);
	length.Set(0,0,20,1);

	object_link* lnk2 = squeleton->addLink(b,pos,axis,C3DSqueleton::PIVOT,pivot2);
	squeleton->addBone(lnk2,length,avantbras);

	//strip = new CStrippedGeometry("STRIP");
    strip = new CGeometry("STRIP");
	g = (CGeometry*)CPersistence::FindObject("Cylinder01");
	*strip = *g;
    
    const CGeometryEditor *pEditor = strip->getEditor();
	pEditor->minimize();
	pEditor->strip();
/*
	unsigned int nb = strip->GetNbStrips();
	for (unsigned int j=0;j<nb;j++)
	{
		float r = ((float)(rand()))/RAND_MAX;
		float g = ((float)(rand()))/RAND_MAX;
		float b = ((float)(rand()))/RAND_MAX;
		strip->SetColor(j,r,b,g,1.0f);
	}
*/
	//runner = Raptor::glAnimator()->LoadScript("Datas\\RaptorScriptTest.lex");
}

void CEditDisplay::ReInit()
{
	CGenericDisplay::ReInit();

	float position[4] = {0,0,100,1.0};

    glDisable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glDisable(GL_LIGHT1);

	glLightfv(GL_LIGHT0,GL_POSITION,position);
	glLightfv(GL_LIGHT0,GL_AMBIENT,light_ambient);
	glLightfv(GL_LIGHT0,GL_DIFFUSE,light_diffuse);
	glLightfv(GL_LIGHT0,GL_SPECULAR,light_specular);
}

void CEditDisplay::UnInit()
{
}

void CEditDisplay::Display()
{
	if (reinit)
		ReInit();

	glPushMatrix();

	if (runner)
		runner->glRender();

	glTranslatef(0,0,10);
	for (int i=0;i<1;i++)
	{
        glColor4f(1.0f,0.0f,0.0f,1.0f);
		glTranslatef(0,0,-5);
	glPushMatrix();
		glTranslatef(-5,-5,-10);
		glRotatef(360.0f * dt,0,1,0);
		glRotatef(360.0f * 2 * dt,1,0,0);
		glRotatef(360.0f * 3 * dt,0,0,1);

		strip->glRender();
	glPopMatrix();

    glColor4f(0.0f,1.0f,0.0f,1.0f);
	glPushMatrix();
		glTranslatef(5,-5,-10);
		glRotatef(360.0f * dt,0,1,0);
		glRotatef(360.0f * 2 * dt,1,0,0);
		glRotatef(360.0f * 3 * dt,0,0,1);

		strip->glRender();
	glPopMatrix();

    glColor4f(0.0f,0.0f,1.0f,1.0f);
	glPushMatrix();
		glTranslatef(-5,5,-10);
		glRotatef(360.0f * dt,0,1,0);
		glRotatef(360.0f * 2 * dt,1,0,0);
		glRotatef(360.0f * 3 * dt,0,0,1);

		strip->glRender();
	glPopMatrix();

	glPushMatrix();
		glTranslatef(5,5,-10);
		glRotatef(360.0f * dt,0,1,0);
		glRotatef(360.0f * 2 * dt,1,0,0);
		glRotatef(360.0f * 3 * dt,0,0,1);

		strip->glRender();
	glPopMatrix();

	glPushMatrix();
		glTranslatef(-5,-5,-20);
		glRotatef(360.0f * dt,0,1,0);
		glRotatef(360.0f * 2 * dt,1,0,0);
		glRotatef(360.0f * 3 * dt,0,0,1);

		strip->glRender();
	glPopMatrix();

	glPushMatrix();
		glTranslatef(5,-5,-20);
		glRotatef(360.0f * dt,0,1,0);
		glRotatef(360.0f * 2 * dt,1,0,0);
		glRotatef(360.0f * 3 * dt,0,0,1);

		strip->glRender();
	glPopMatrix();

	glPushMatrix();
		glTranslatef(-5,5,-20);
		glRotatef(360.0f * dt,0,1,0);
		glRotatef(360.0f * 2 * dt,1,0,0);
		glRotatef(360.0f * 3 * dt,0,0,1);

		strip->glRender();
	glPopMatrix();

	glPushMatrix();
		glTranslatef(5,5,-20);
		glRotatef(360.0f * dt,0,1,0);
		glRotatef(360.0f * 2 * dt,1,0,0);
		glRotatef(360.0f * 3 * dt,0,0,1);

		strip->glRender();
	glPopMatrix();
	}

	glPopMatrix();

	dt+=0.002f;
	if (dt>1.0) dt=0.0;
}