/***************************************************************************/
/*                                                                         */
/*  LightFlare.cpp                                                         */
/*                                                                         */
/*    Raptor OpenGL & Vulkan realtime 3D Engine SDK.                       */
/*                                                                         */
/*  Copyright 1998-2019 by                                                 */
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

#if !defined(AFX_LIGHTFLARE_H__373B5695_C92B_4ED8_8DDF_81273BF34FE3__INCLUDED_)
    #include "LightFlare.h"
#endif
#if !defined(AFX_RAPTORIO_H__87D52C27_9117_4675_95DC_6AD2CCD2E78D__INCLUDED_)
	#include "System/RaptorIO.h"
#endif
#if !defined(AFX_TEXTUREFACTORY_H__1B470EC4_4B68_11D3_9142_9A502CBADC6B__INCLUDED_)
	#include "TextureSet.h"
#endif
#if !defined(AFX_TEXTUREOBJECT_H__D32B6294_B42B_4E6F_AB73_13B33C544AD0__INCLUDED_)
	#include "TextureObject.h"
#endif
#if !defined(AFX_TEXTUREQUAD_H__1712AF34_6723_4E39_BC72_05ED6FA28418__INCLUDED_)
	#include "TextureQuad.h"
#endif

RAPTOR_NAMESPACE_BEGIN

static CLightFlare::CLightFlareClassID lightflareId;
const CPersistence::CPersistenceClassID& CLightFlare::CLightFlareClassID::GetClassId(void)
{
	return lightflareId;
}

RAPTOR_NAMESPACE_END


RAPTOR_NAMESPACE

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLightFlare::CLightFlare(const std::string& name) :
    CPersistence(lightflareId,name),
	m_fLightVolumeSize(0.0f),
	m_volumeVisibility(0),
	m_visibilityQuery(0),
	m_fLightVolumeVisibility(0.0f)
{
}

CLightFlare::~CLightFlare(void)
{
	mFlares.clear();
}

void CLightFlare::glRender(float dx, float dy)
{
    if (0 == mFlares.size())
        return;

	glEnable(GL_TEXTURE_2D);

	for (unsigned int i = 0; i < mFlares.size(); i++)
	{
		const flare_item& flare = mFlares[i];
		flare.pFlare->glvkRender();

		glBegin(GL_QUADS);
			glTexCoord2f(0.0f, 0.0f);    glVertex3f(-flare.fSize, -flare.fSize, 0.0f);
			glTexCoord2f(1.0f, 0.0f);    glVertex3f(+flare.fSize, -flare.fSize, 0.0f);
			glTexCoord2f(1.0f, 1.0f);    glVertex3f(+flare.fSize, +flare.fSize, 0.0f);
			glTexCoord2f(0.0f, 1.0f);    glVertex3f(-flare.fSize, +flare.fSize, 0.0f);
		glEnd();

		glTranslatef(dx, dy, 0); //dz
	}
}

bool CLightFlare::importObject(CRaptorIO& io)
{
	string name;
	io >> name;

	string setName = "";
	string textureName = "";
	float gSize = 1.0f;

	string data = io.getValueName();
	while (!data.empty())
	{
		if (data == "set")
			io >> setName;
		else if (data == "texname")
			io >> textureName;
		else if (data == "size")
			io >> gSize;
		else
			io >> name;

		data = io.getValueName();
	}
	io >> name;

	if (!setName.empty() && !textureName.empty())
	{
		CPersistence *p = CPersistence::FindObject(setName);
		if ((p != NULL) &&
			(p->getId().isSubClassOf(CTextureSet::CTextureSetClassID::GetClassId())))
		{
			CTextureSet *tset = (CTextureSet*)p;
			CTextureObject *t = tset->getTexture(textureName);
			if (t != NULL)
			{
				CLightFlare::flare_item flare;
				flare.pFlare = t;
				flare.fSize = gSize;
				flare.fDistance = 1.0f;
				mFlares.push_back(flare);
			}
			/*
        {
			m_glow = t;

			m_pGlow = new CTextureQuad();
			m_pGlow->setQuadTexture(t);
			m_pGlow->glSetQuadAttributes(GL_COORD_VERTEX(0.0f, 0.0f, 0.0f, 1.0f),
										 CColor::RGBA(1.0f, 1.0f, 1.0f, 1.0f),
										 GL_COORD_VERTEX(gSize, gSize, 0.0f, 0.0f));
        }
			*/
		}
	}

	return true;
}

bool CLightFlare::exportObject(CRaptorIO& o)
{
	return true;
}