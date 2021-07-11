/***************************************************************************/
/*                                                                         */
/*  LightGlow.cpp                                                          */
/*                                                                         */
/*    Raptor OpenGL & Vulkan realtime 3D Engine SDK.                       */
/*                                                                         */
/*  Copyright 1998-2021 by                                                 */
/*  Fabrice FERRAND.                                                       */
/*                                                                         */
/*  This file is part of the Raptor project, and may only be used,         */
/*  modified, and distributed under the terms of the Raptor project        */
/*  license, LICENSE.  By continuing to use, modify, or distribute         */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/


#include "Subsys/CodeGeneration.h"

#if !defined(AFX_LIGHTGLOW_H__577C39B3_EE0B_4A07_8974_BC250BA2960A__INCLUDED_)
    #include "GLHierarchy/LightGlow.h"
#endif
#if !defined(AFX_RAPTORIO_H__87D52C27_9117_4675_95DC_6AD2CCD2E78D__INCLUDED_)
	#include "System/RaptorIO.h"
#endif
#if !defined(AFX_TEXTUREFACTORY_H__1B470EC4_4B68_11D3_9142_9A502CBADC6B__INCLUDED_)
	#include "GLHierarchy/TextureSet.h"
#endif
#if !defined(AFX_ITEXTUREOBJECT_H__3AA8C89E_BB23_483C_A547_C8A4CC53E551__INCLUDED_)
	#include "GLHierarchy/ITextureObject.h"
#endif
#if !defined(AFX_TEXTUREQUAD_H__1712AF34_6723_4E39_BC72_05ED6FA28418__INCLUDED_)
	#include "GLHierarchy/TextureQuad.h"
#endif

RAPTOR_NAMESPACE_BEGIN

static CLightGlow::CLightGlowClassID lightglowId;
const CPersistence::CPersistenceClassID& CLightGlow::CLightGlowClassID::GetClassId(void)
{
	return lightglowId;
}

RAPTOR_NAMESPACE_END


RAPTOR_NAMESPACE

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLightGlow::CLightGlow(const std::string& name):
    CPersistence(lightglowId,name),
	m_glowSize(1.0f),
	m_pGlow(NULL)
{
}

CLightGlow::~CLightGlow(void)
{
	m_pGlow = NULL;
}

void CLightGlow::glRender(void)
{
	if ((NULL == m_pGlow) || (m_glowSize <= 0))
        return;

    glPushAttrib(GL_ENABLE_BIT);

	glEnable(GL_BLEND);

	m_pGlow->glRender();

    glPopAttrib();
}

bool CLightGlow::importObject(CRaptorIO& io)
{
	if (NULL != m_pGlow)
		delete m_pGlow;
	m_pGlow = NULL;

    string name;
    io >> name;

	string setName = "";
    string textureName = "";
    
	string data = io.getValueName();
    while (io.hasMoreValues())
    {
		if (data == "name")
			CPersistence::importObject(io);
		else if (data == "set")
            io >> setName;
        else if (data == "texname")
            io >> textureName;
        else if (data == "size")
            io >> m_glowSize;
		else
			io >> name;

		data = io.getValueName();
	}
	
	if (!setName.empty() && !textureName.empty())
    {
        CPersistence *p = CPersistence::FindObject(setName);
		if ((p != NULL) && 
			(p->getId().isSubClassOf(CTextureSet::CTextureSetClassID::GetClassId())))
        {
            CTextureSet *tset = (CTextureSet*)p;
            ITextureObject *t = tset->getTexture(textureName);

			m_pGlow = new CTextureQuad();
			m_pGlow->setQuadTexture(t);
			m_pGlow->glSetQuadAttributes(GL_COORD_VERTEX(0.0f, 0.0f, 0.0f, 1.0f),
										 CColor::RGBA(1.0f, 1.0f, 1.0f, 1.0f),
										 GL_COORD_VERTEX(m_glowSize, m_glowSize, 0.0f, 0.0f));
        }
    }

	io >> name;
	return true;
}

bool CLightGlow::exportObject(CRaptorIO& o)
{
	return true;
}