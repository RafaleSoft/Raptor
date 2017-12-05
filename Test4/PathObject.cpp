#include "StdAfx.h"

#include "Engine/3DPath.h"

#include "PathObject.h"


CPathObject::CPathObject(void)
{
    m_pPath = new C3DPath("slide_path");
    m_pPath->setPathType(C3DPath::CGL_CATMULLROMEX);
    m_pPath->addKnot(-15.0f,2.0f,-10.0f,0.0f,0.0f);
    m_pPath->addKnot(-10.0f,2.0f,-6.0f,0.2f,0.0f);
    m_pPath->addKnot(-5.0f,2.0f,-3.0f,0.4f,0.0f);
    m_pPath->addKnot(0.0f,2.0f,-1.0f,0.5f,0.0f);
    m_pPath->addKnot(5.0f,2.0f,-3.0f,0.6f,0.0f);
    m_pPath->addKnot(10.0f,2.0f,-6.0f,0.8f,0.0f);
    m_pPath->addKnot(15.0f,2.0f,-10.0f,1.0f,0.0f);

    GL_COORD_VERTEX Min(-10.0f,1.5f,-5.0f,1.0f);
    GL_COORD_VERTEX Max(10.0f,2.5f,-1.0f,1.0f);
    setBoundingBox(Min,Max);
}

CPathObject::~CPathObject(void)
{
    delete m_pPath;
}

GL_COORD_VERTEX CPathObject::getTPos(float t)
{
    CGenericVector<float> pos = m_pPath->eval(t);

    GL_COORD_VERTEX v(pos.X(),pos.Y(),pos.Z(),1.0f);
    return v;
}

void CPathObject::glRender(void)
{/*
    glPushMatrix();

    glColor4f(0.0,0.0,1.0,1.0);
	glBegin(GL_LINE_STRIP);
		for (int i=0;i<100;i++)
		{
			float u = ((float)i)/100.0f;
			CGenericVector<float> &res = m_pPath->eval(u);
			glVertex3f(res.X(),res.Y(),res.Z());
		}
	glEnd();

    glColor4f(1.0,1.0,1.0,1.0);

    glPopMatrix();
    */
}

