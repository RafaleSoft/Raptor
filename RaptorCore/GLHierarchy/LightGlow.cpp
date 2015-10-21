// LightGlow.cpp: implementation of the CLightGlow class.
//
//////////////////////////////////////////////////////////////////////

#include "Subsys/CodeGeneration.h"

#if !defined(AFX_LIGHTGLOW_H__577C39B3_EE0B_4A07_8974_BC250BA2960A__INCLUDED_)
    #include "LightGlow.h"
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


RAPTOR_NAMESPACE_BEGIN

static CLightGlow::CPersistenceClassID lightglowId;

RAPTOR_NAMESPACE_END


RAPTOR_NAMESPACE

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLightGlow::CLightGlow(const std::string& name):
    CPersistence(lightglowId,name),
	m_glowSize(1.0f),
	m_bRebuildGlow(true),
	m_uiGlow(0)
{
}

CLightGlow::~CLightGlow(void)
{
	m_glow = NULL;
}


void CLightGlow::glRender(void)
{
    if ((NULL == m_glow) || (m_glowSize <= 0))
        return;

    glPushAttrib(GL_ENABLE_BIT);
    int	blendSrc;
	int	blendDst;
    glGetIntegerv(GL_BLEND_SRC,&blendSrc);
	glGetIntegerv(GL_BLEND_DST,&blendDst);

    m_glow->glRender();
    float s = m_glowSize;	// shorter to write

    if (m_bRebuildGlow)
    {
        m_bRebuildGlow = false;
        if (m_uiGlow > 0)
            glDeleteLists(m_uiGlow,1);

        m_uiGlow = glGenLists(1);
        glNewList(m_uiGlow,GL_COMPILE_AND_EXECUTE);

	        glDisable(GL_LIGHTING);
            glEnable(GL_TEXTURE_2D);
            glEnable(GL_BLEND);
            glDepthMask(GL_FALSE);
            glBlendFunc(GL_SRC_ALPHA,GL_ONE);
	            
            glBegin(GL_QUADS);
	            glTexCoord2f(0.0f,0.0f);    glVertex3f(-s,-s,0.0f);
	            glTexCoord2f(1.0f,0.0f);    glVertex3f(+s,-s,0.0f);
	            glTexCoord2f(1.0f,1.0f);    glVertex3f(+s,+s,0.0f);
	            glTexCoord2f(0.0f,1.0f);    glVertex3f(-s,+s,0.0f);
            glEnd();

            glDepthMask(GL_TRUE);
        glEndList();
    }
    else if (m_uiGlow > 0)
        glCallList(m_uiGlow);


    glBlendFunc(blendSrc,blendDst);
    glPopAttrib();
}

bool CLightGlow::importObject(CRaptorIO& io)
{
    bool activate = false;

    string name;
    io >> name;

	string setName = "";
    string textureName = "";
    
	string data = io.getValueName();
    while (!data.empty())
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
            CTextureObject *t = tset->getTexture(textureName);
			m_glow = t;
        }
    }

	io >> name;
	return true;
}

bool CLightGlow::exportObject(CRaptorIO& o)
{
	return true;
}