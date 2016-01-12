// TextureSet.cpp: implementation for the CTextureSet class.
//
//////////////////////////////////////////////////////////////////////

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

static CTextureSet::CTextureSetClassID textureId;
static CPersistentType<CTextureSet> textureSet(textureId);
const CPersistence::CPersistenceClassID& CTextureSet::CTextureSetClassID::GetClassId(void)
{
	return textureId;
}

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

bool CTextureSet::addTexture(CTextureObject *t)
{
	unsigned int pos = m_pTextures.size();
    for (unsigned int i=0;i<pos;i++)
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

bool CTextureSet::removeTexture(CTextureObject *t)
{
	vector<CTextureObject*>::iterator itr = m_pTextures.begin();
	bool found = false;

    while (itr != m_pTextures.end())
    {
        CTextureObject *T = *itr;
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
	vector<CTextureObject*>::iterator itr = m_pTextures.begin();
	while (itr != m_pTextures.end())
	{
		CTextureObject *T = *itr++;
		T->releaseReference();
	}

	m_pTextures.clear();
}

CTextureObject* const CTextureSet::getTexture(unsigned int index) const
{
    if (index >= m_pTextures.size())
        return NULL;
    else
        return m_pTextures[index];
}


CTextureObject* const CTextureSet::getTexture(const std::string& name) const
{
    bool found = false;
    vector<CTextureObject*>::const_iterator itr = m_pTextures.begin();
    CTextureObject *res = NULL;

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

	o << m_pTextures.size();

	for (unsigned int i=0;i<m_pTextures.size();i++)
	{
		CTextureObject* T = m_pTextures.at(i);

		o << T->getCurrentMipMapLevel();
		o << T->getWidth();
		o << T->getHeight();
        o << T->getDepth();
		o << T->getFunction();
		o << T->getFilter();
		o << T->getTransparency();
		o << T->getName().size();
		o << T->getName().data();

        CTextureObject::TEXEL_TYPE format = T->getTexelType();

        T->allocateTexels(format);
		unsigned char *texels = T->getTexels();

		glGetTexImage(GL_TEXTURE_2D,T->getCurrentMipMapLevel(),format,GL_UNSIGNED_BYTE,texels);

		o.write((unsigned char*)texels,0 /*size*/);

		T->releaseTexels();
	}

	return true;
}

bool CTextureSet::importTextureObject(CRaptorIO& io)
{
    string name;
    io >> name;

    string filename = "<unknown>";
    CTextureObject::TEXTURE_FUNCTION function = CTextureObject::CGL_OPAQUE;
    CTextureObject::TEXTURE_FILTER filter = CTextureObject::CGL_UNFILTERED;
    CTextureObject::TEXEL_TYPE texelType = CTextureObject::CGL_COLOR24_ALPHA;
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
                function = CTextureObject::CGL_OPAQUE;
            else if (name == "multiply")
                function = CTextureObject::CGL_MULTIPLY;
            else if (name == "alpha_transparent")
                function = CTextureObject::CGL_ALPHA_TRANSPARENT;
            else if (name == "constant_blended")
                function = CTextureObject::CGL_CONSTANT_BLENDED;
        }
        else if (data == "filter")
        {
            io >> name;
            if (name == "unfiltered")
                filter = CTextureObject::CGL_UNFILTERED;
            else if (name == "bilinear")
                filter = CTextureObject::CGL_BILINEAR;
            else if (name == "bilinear_mipmapped")
                filter = CTextureObject::CGL_BILINEAR_MIPMAPPED;
            else if (name == "trilinear")
                filter = CTextureObject::CGL_TRILINEAR;
            else if (name == "anisotropic")
                filter = CTextureObject::CGL_ANISOTROPIC;
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

    CTextureObject *T = f.glCreateTexture(texelType,function,filter);
	bool res = false;
    if (T != NULL)
    {
		addTexture(T);
        if (transparency > 0)
            T->glSetTransparency(255 * transparency);
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

