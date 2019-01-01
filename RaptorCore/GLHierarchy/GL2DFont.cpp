// GL2DFont.cpp: implementation of the CGL2DFont class.
//
//////////////////////////////////////////////////////////////////////
#include "Subsys/CodeGeneration.h"

#include "Subsys/FreeType/FTFace.h"
#include "Subsys/FreeType/TTBitmapFont.h"
#include "Subsys/FreeType/FTGlyphBitmap.h"

#if !defined(AFX_GL2DFONT_H__FE8026E7_74FD_46FC_A70F_D6927E565F8D__INCLUDED_)
	#include "GL2DFont.h"
#endif
#if !defined(AFX_RAPTORERRORMANAGER_H__FA5A36CD_56BC_4AA1_A5F4_451734AD395E__INCLUDED_)
	#include "System/RaptorErrorManager.h"
#endif

RAPTOR_NAMESPACE

CGL2DFont::CGL2DFont(const std::string& name)
	:CGLFont(name), m_bmfont(NULL), m_face(NULL)
{
}

bool CGL2DFont::init(const std::string &name, unsigned int size)
{
	if (m_face == NULL)
	{
		m_face = new FTFace;
		m_fontFileName = name;
		if (!m_face->open(name.data()))
		{
			delete m_face;
			m_face = NULL;
#ifdef RAPTOR_DEBUG_MODE_GENERATION
			Raptor::GetErrorManager()->generateRaptorError(CGLFont::CGLFontClassID::GetClassId(),
				CRaptorErrorManager::RAPTOR_ERROR,
				CRaptorMessages::ID_SUBSYS_FAILED);
#endif
			return false;
		}
	}

	if (m_bmfont == NULL)
	{
		m_bmfont = new TTBitmapFont;
		if (!m_bmfont->create(m_face, size))
		{
			delete m_bmfont;
			m_bmfont = NULL;
			return false;
		}
	}

	return true;
}


CGL2DFont::~CGL2DFont()
{
	if (m_bmfont != NULL)
		delete m_bmfont;
	if (m_face != NULL)
		delete m_face;
}

float CGL2DFont::getHeight(const std::string &s) const
{
#ifdef RAPTOR_DEBUG_MODE_GENERATION
	if ((m_bmfont == NULL) ||
		(m_face == NULL))
	{
		Raptor::GetErrorManager()->generateRaptorError(CGLFont::CGLFontClassID::GetClassId(),
			CRaptorErrorManager::RAPTOR_ERROR,
			CRaptorMessages::ID_INIT_FAILED);
		return 0.0f;
	}
#endif

	return (float)m_bmfont->getHeight();
}

float CGL2DFont::getWidth(const std::string &s) const
{
#ifdef RAPTOR_DEBUG_MODE_GENERATION
	if ((m_bmfont == NULL) ||
		(m_face == NULL))
	{
		Raptor::GetErrorManager()->generateRaptorError(CGLFont::CGLFontClassID::GetClassId(),
			CRaptorErrorManager::RAPTOR_ERROR,
			CRaptorMessages::ID_INIT_FAILED);
		return 0.0f;
	}
#endif

	return (float)m_bmfont->getWidth(s.data());
}



void CGL2DFont::glWrite(const std::string &text, int x, int y)
{
#ifdef RAPTOR_DEBUG_MODE_GENERATION
	if (m_bmfont == NULL)
	{
		Raptor::GetErrorManager()->generateRaptorError(CGLFont::CGLFontClassID::GetClassId(),
			CRaptorErrorManager::RAPTOR_ERROR,
			CRaptorMessages::ID_INIT_FAILED);
		return;
	}
#endif

	GLfloat viewport[4];

	glGetFloatv(GL_VIEWPORT, viewport);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(viewport[0], viewport[2], viewport[1], viewport[3], 1, 20);
	glMatrixMode(GL_MODELVIEW);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	GLint px = x;
	GLint py = viewport[3] - y;
	for (unsigned int i = 0; i<text.size(); i++)
	{
		unsigned char ch = text[i];

		FTGlyphBitmap* gbitmap = m_bmfont->getBitmap(ch);
		if (gbitmap == 0)
			continue;
		if (gbitmap->getBitmap() != 0)
		{
			GLint x_dst = px + gbitmap->getDeltaX();
			GLint y_dst = py + gbitmap->getDeltaY();

			glRasterPos4i(x_dst, y_dst, -1.0f, 1.0f);
			glBitmap(gbitmap->getWidth(),
					 gbitmap->getHeight(),
					 0., 0., // x,y orig
					 0., 0., // x,y move
					 (const GLubyte*)gbitmap->getBitmap());
		}
		px += gbitmap->getAdvance();
	}

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	CATCH_GL_ERROR
}

bool CGL2DFont::glGenGlyphs(float precision,
							float extrusion,
							float scale)
{
	return false;
}