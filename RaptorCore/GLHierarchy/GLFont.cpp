// GLFont.cpp: implementation of the CGLFont class.
//
//////////////////////////////////////////////////////////////////////
#include "Subsys/CodeGeneration.h"

#ifndef __RAPTOR_GLEXT_H__
	#include "System/Glext.h"
#endif

#if !defined(AFX_GLFONT_H__D451FE62_5FE1_11D3_9142_BA23BC92E77C__INCLUDED_)
	#include "GLFont.h"
#endif
#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
	#include "System/Raptor.h"
#endif
#if !defined(AFX_TEXTUREFACTORY_H__1B470EC4_4B68_11D3_9142_9A502CBADC6B__INCLUDED_)
	#include "GLHierarchy/TextureFactory.h"
#endif
#if !defined(AFX_RAPTORERRORMANAGER_H__FA5A36CD_56BC_4AA1_A5F4_451734AD395E__INCLUDED_)
	#include "System/RaptorErrorManager.h"
#endif


RAPTOR_NAMESPACE

static CGLFont::CGLFontClassID fontId;
const CPersistence::CPersistenceClassID& CGLFont::CGLFontClassID::GetClassId(void)
{
	return fontId;
}

// glyph settings
typedef struct glyphSettings_t
{
	float		precision;
	float		extrusion;
	float		scale;
} glyphSettings;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CGLFont::CGLFont(const std::string& name)
	:CPersistence(fontId,name),
	m_fontFileName(""),
	m_size(0),
	m_currentGlyphset(0)
{
}

CGLFont::~CGLFont()
{
	vector<glyph*>::iterator itr = m_glfontglyph.begin();
	while (itr != m_glfontglyph.end())
	{
		glyph *g = (*itr);
		for (int i=0;i<256;i++)
			glDeleteLists(g[i].glList,1);
		itr = m_glfontglyph.erase(itr);
		delete [] g;
	}

	vector<void*>::iterator itr2 = m_glfontglyphsettings.begin();
	while (itr2 != m_glfontglyphsettings.end())
	{
		glyphSettings *gs = (glyphSettings*)(*itr2);
		itr2 = m_glfontglyphsettings.erase(itr2);
		delete gs;
	}

	m_glfontglyph.clear();
	m_glfontglyphsettings.clear();
}


//////////////////////////////////////////////////////////////////////
// Implementation
//////////////////////////////////////////////////////////////////////
float CGLFont::getWidth(const std::string &s, unsigned int numglyphset) const
{
#ifdef RAPTOR_DEBUG_MODE_GENERATION
	if ((numglyphset >= m_glfontglyph.size()) || m_glfontglyph.empty())
    {
			Raptor::GetErrorManager()->generateRaptorError(	CGLFont::CGLFontClassID::GetClassId(),
                                                            CRaptorErrorManager::RAPTOR_ERROR,
															CRaptorMessages::ID_INIT_FAILED);

            return 0.0f;
    }
#endif

	GLfloat w=0;
	glyph	*glyphs = (glyph*)m_glfontglyph.at(numglyphset);

	for (unsigned int i=0;i<s.size();i++)
	{
		glyph g = glyphs[(unsigned char)s[i]];
		w += g.halfWidth;
	}
	return -2*w;
}


float CGLFont::getHeight(const std::string &s, unsigned int numglyphset) const
{
#ifdef RAPTOR_DEBUG_MODE_GENERATION
	if ((numglyphset >= m_glfontglyph.size()) || m_glfontglyph.empty())
    {
			Raptor::GetErrorManager()->generateRaptorError(	CGLFont::CGLFontClassID::GetClassId(),
															CRaptorErrorManager::RAPTOR_ERROR,
															CRaptorMessages::ID_INIT_FAILED);
            return 0.0f;
    }
#endif
    
	GLfloat h = 0;
	glyph	*glyphs = (glyph*)m_glfontglyph.at(numglyphset);

	for (unsigned int i=0;i<s.size();i++)
	{
		glyph g = glyphs[(unsigned char)s[i]];
		if (g.height<h) 
			h = g.height;
	}
	return -h;
}

bool CGLFont::glGenGlyphs(	float precision,
							float extrusion,
							float scale)
{
	glyphSettings *glyphSts = new glyphSettings;
	glyphSts->precision = precision;
	glyphSts->extrusion = extrusion;
	glyphSts->scale = scale;

	m_glfontglyphsettings.push_back(glyphSts);
	return true;
}


bool CGLFont::getGlyphSettings(	float &precision,
								float &extrusion,
								float &scale,
								unsigned int glyphset)
{
	if (glyphset >= m_glfontglyphsettings.size())
		return false;

	glyphSettings *gs = (glyphSettings *)m_glfontglyphsettings.at(glyphset);
	precision = gs->precision;
	extrusion = gs->extrusion;
	scale = gs->scale;

	return true;
}

void CGLFont::glWrite(const std::string &text, int x, int y)
{
	unsigned int glyphset = m_currentGlyphset;
	if ((glyphset >= m_glfontglyph.size()) || m_glfontglyph.empty())
		return;

	glyph *glyphs = (glyph*)m_glfontglyph.at(glyphset);

	glPushMatrix();
	glTranslatef(x, y, 0.0);

	for (unsigned int i=0;i<text.size();i++)
	{
		glyph g = glyphs[(unsigned char)text[i]];
		glCallList(g.glList);
		glTranslatef(g.advance,0.0,0.0);
	}

	glPopMatrix();

	CATCH_GL_ERROR
}

