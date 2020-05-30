/***************************************************************************/
/*                                                                         */
/*  TextureSet.cpp                                                         */
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

#if !defined(AFX_TEXTURESET_H__26F3022D_70FE_414D_9479_F9CCD3DCD445__INCLUDED_)
	#include "TextureSet.h"
#endif
#if !defined(AFX_TEXTUREFACTORY_H__1B470EC4_4B68_11D3_9142_9A502CBADC6B__INCLUDED_)
	#include "TextureFactory.h"
#endif
#if !defined(AFX_RAPTORIO_H__87D52C27_9117_4675_95DC_6AD2CCD2E78D__INCLUDED_)
	#include "System/RaptorIO.h"
#endif
#if !defined(AFX_OBJECTFACTORY_H__7F891C52_9E32_489C_B09C_5E5803522D91__INCLUDED_)
	#include "ObjectFactory.h"
#endif
#if !defined(AFX_TEXTUREFACTORYCONFIG_H__7A20D208_423F_4E02_AA4D_D736E0A7959F__INCLUDED_)
	#include "TextureFactoryConfig.h"
#endif


RAPTOR_NAMESPACE

IMPLEMENT_CLASS_ID(CTextureSet, textureId)


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTextureSet::CTextureSet(const std::string& name)
	:CPersistence(textureId,name)
{
}

CTextureSet::~CTextureSet(void)
{
	removeAllTextures();
}

CTextureSet::operator RAPTOR_HANDLE() const
{
    RAPTOR_HANDLE SetHandle(CTextureSet::CTextureSetClassID::GetClassId().ID(),(void*)this);
    return SetHandle;
}

bool CTextureSet::addTexture(ITextureObject *t)
{
	size_t pos = m_pTextures.size();
    for (size_t i=0; i<pos; i++)
    {
        if (m_pTextures[i] == t)
        {
            pos = i;
            break;
        }
    }

    if (pos < m_pTextures.size())
        return false;
    else
    {
		t->addReference();
        m_pTextures.push_back(t);
        return true;
    }
}

bool CTextureSet::removeTexture(ITextureObject *t)
{
	std::vector<ITextureObject*>::iterator itr = m_pTextures.begin();
	bool found = false;

    while (itr != m_pTextures.end())
    {
        ITextureObject *T = *itr;
		if (T == t)
		{
			T->releaseReference();
			m_pTextures.erase(itr);
			itr = m_pTextures.end();
			found = true;
		}
		else
			itr++;
    }

	return found;
}

void CTextureSet::removeAllTextures(void)
{
	std::vector<ITextureObject*>::iterator itr = m_pTextures.begin();
	while (itr != m_pTextures.end())
	{
		ITextureObject *T = *itr++;
		T->releaseReference();
	}

	m_pTextures.clear();
}

ITextureObject* const CTextureSet::getTexture(size_t index) const
{
    if (index >= m_pTextures.size())
        return NULL;
    else
        return m_pTextures[index];
}


ITextureObject* const CTextureSet::getTexture(const std::string& name) const
{
    bool found = false;
    std::vector<ITextureObject*>::const_iterator itr = m_pTextures.begin();
    ITextureObject *res = NULL;

    while ((itr != m_pTextures.end()) && !found)
    {
        res = (*itr++);
		found = (string::npos != res->getName().find(name));
    }

    if (!found)
        res = NULL;

    return res;
}

bool CTextureSet::exportObject(CRaptorIO& o)
{
	CPersistence::exportObject(o);

	return false;
}

bool CTextureSet::importTextureObject(CRaptorIO& io)
{
    string name;
    io >> name;

    string filename = "<unknown>";
    ITextureObject::TEXTURE_FUNCTION function = ITextureObject::CGL_OPAQUE;
    ITextureObject::TEXTURE_FILTER filter = ITextureObject::CGL_UNFILTERED;
    ITextureObject::TEXEL_TYPE texelType = ITextureObject::CGL_COLOR24_ALPHA;
    float transparency = -1.0f;
	bool compressed = false;

	string data = io.getValueName();
    while (!data.empty())
    {
		if (data == "name")
			io >> filename;
        else if (data =="function")
        {
            io >> name;
            if (name == "opaque")
                function = ITextureObject::CGL_OPAQUE;
            else if (name == "multiply")
                function = ITextureObject::CGL_MULTIPLY;
            else if (name == "alpha_transparent")
                function = ITextureObject::CGL_ALPHA_TRANSPARENT;
            else if (name == "constant_blended")
                function = ITextureObject::CGL_CONSTANT_BLENDED;
        }
        else if (data == "filter")
        {
            io >> name;
            if (name == "unfiltered")
                filter = ITextureObject::CGL_UNFILTERED;
            else if (name == "bilinear")
                filter = ITextureObject::CGL_BILINEAR;
            else if (name == "bilinear_mipmapped")
                filter = ITextureObject::CGL_BILINEAR_MIPMAPPED;
            else if (name == "trilinear")
                filter = ITextureObject::CGL_TRILINEAR;
            else if (name == "anisotropic")
                filter = ITextureObject::CGL_ANISOTROPIC;
        }
        else if (data == "mode")
        {
            io >> name;
            compressed = (name == "compressed");
        }
        else if (data =="transparency")
            io >> transparency;
		else
			io >> name;
		
		data = io.getValueName();
	}
	io >> name;

	CTextureFactory &f = CTextureFactory::getDefaultFactory();
	const CTextureFactoryConfig::ICompressor *oldCompressor  = f.getConfig().getCurrentCompressor();
	if ((compressed) && (0 < f.getConfig().getNumCompressors()))
		f.getConfig().setCurrentCompressor(f.getConfig().getCompressor("OpenGL"));

    ITextureObject *T = f.glCreateTexture(texelType,function,filter);
	bool res = false;
    if (T != NULL)
    {
		addTexture(T);
        if (transparency > 0)
            T->getGLTextureObject()->glSetTransparency(255 * transparency);
        res = f.glLoadTexture(T,filename);
    }
    
	f.getConfig().setCurrentCompressor(oldCompressor);
	return res;
}

bool CTextureSet::importObject(CRaptorIO& io)
{
    string name;
    io >> name;

	string data = io.getValueName();
    while (!data.empty())
    {
		if (data == "name")
			CPersistence::importObject(io);
		else if (data == "Texture")
            importTextureObject(io);
        else if (data == "Cubemap")
		{
		}
		else
			io >> name;
		
		data = io.getValueName();
	}
	io >> name;

	return true;
}

