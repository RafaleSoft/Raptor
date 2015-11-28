// Mirror.cpp: implementation of the CMirror class.
//
//////////////////////////////////////////////////////////////////////
#include "Subsys/CodeGeneration.h"

#if !defined(AFX_LIGHT_H__AA8BABD6_059A_4939_A4B6_A0A036E12E1E__INCLUDED_)
	#include "GLHierarchy/Light.h"
#endif
#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
	#include "System/Raptor.h"
#endif
#if !defined(AFX_OBJECT3D_H__DB24F017_80B9_11D3_97C1_FC2841000000__INCLUDED_)
	#include "GLHierarchy/Object3D.h"
#endif
#if !defined(AFX_RENDERINGPROPERTIES_H__634BCF2B_84B4_47F2_B460_D7FDC0F3B698__INCLUDED_)
	#include "GLHierarchy/RenderingProperties.h"
#endif

#ifndef __GLOBAL_H__
	#include "System/Global.h"
#endif

#if !defined(AFX_RAPTORIO_H__87D52C27_9117_4675_95DC_6AD2CCD2E78D__INCLUDED_)
	#include "System/RaptorIO.h"
#endif

#if !defined(AFX_MIRROR_H__BA9C578A_40A8_451B_9EA3_C27CB04288FA__INCLUDED_)
    #include "Mirror.h"
#endif


RAPTOR_NAMESPACE

//////////////////////////////////////////////////////////////////////
// Mirror subclass
//////////////////////////////////////////////////////////////////////
CMirror::CMirror()
	:m_bApplied(false)
{
	IDENT_MATRIX(m_mirror);

    m_pProperties = new CRenderingProperties();
    m_pProperties->setTexturing(CRenderingProperties::ENABLE);
    m_pProperties->setLighting(CRenderingProperties::ENABLE);
    m_pProperties->setStencilTest(CRenderingProperties::DISABLE);
    m_pProperties->clear(0);
}

CMirror::~CMirror() 
{
	delete m_pProperties;
}

bool CMirror::addObject(CObject3D *mirrorObject)
{
    bool res = true;

    for (unsigned int i=0;res && (i<mirrorObjects.size());i++)
    {
        if (mirrorObjects[i] == mirrorObject)
            res = false;
    }

    if (res)
    {
        mirrorObject->getProperties().setMirror(true);
        mirrorObjects.push_back(mirrorObject);
    }

    return res;
}

void CMirror::glClipRender(void)
{
    vector<CObject3D*>::const_iterator it = mirrorObjects.begin();

    while(it != mirrorObjects.end())
		(*it++)->glClipRender();
}

void CMirror::setMirrorPlane(const GL_COORD_VERTEX& n,
							 const GL_COORD_VERTEX& m) 
{
    m_mirror.rowx.x = (1 - 2 * n.x * n.x);
    m_mirror.rowy.x = -2 * n.x * n.y;
    m_mirror.rowz.x = -2 * n.x * n.z;

    m_mirror.rowx.y = -2 * n.y * n.x;
    m_mirror.rowy.y = (1 - 2 * n.y * n.y);
    m_mirror.rowz.y = -2 * n.y * n.z;

    m_mirror.rowx.z = -2 * n.z * n.x;
    m_mirror.rowy.z = -2 * n.z * n.y;
    m_mirror.rowz.z = (1 - 2 * n.z * n.z);

    m_mirror.rowh.x = 2 * n.x * (n.x * m.x + n.y * m.y + n.z * m.z);
    m_mirror.rowh.y = 2 * n.y * (n.x * m.x + n.y * m.y + n.z * m.z);
    m_mirror.rowh.z = 2 * n.z * (n.x * m.x + n.y * m.y + n.z * m.z);
    m_mirror.rowx.h = 0;
    m_mirror.rowy.h = 0;
    m_mirror.rowz.h = 0;
    m_mirror.rowh.h = 1.0f;
}

void CMirror::glApplyMirror(bool apply)
{
    if (!m_bApplied && apply)
    {
        glPushMatrix();
		glMultMatrixf(m_mirror);
        glCullFace(GL_FRONT);

		m_pProperties->glPushProperties();

        CObject3D::enableEarlyClip(false);
        m_bApplied = true;
    }
    else if (m_bApplied && !apply)
    {
        glPopMatrix();
        glCullFace(GL_BACK);

		m_pProperties->glPopProperties();

        CObject3D::enableEarlyClip(true);
        m_bApplied = false;
    }

    CATCH_GL_ERROR
}


