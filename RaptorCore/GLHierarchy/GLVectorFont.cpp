// GLVectorFont.cpp: implementation of the CGLVectorFont class.
//
//////////////////////////////////////////////////////////////////////
#include "Subsys/CodeGeneration.h"

#if !defined(AFX_GLVECTORFONT_H__C21ADBEA_705D_43EA_A45F_F016233F7507__INCLUDED_)
	#include "GLVectorFont.h"
#endif
#if !defined(AFX_RAPTORERRORMANAGER_H__FA5A36CD_56BC_4AA1_A5F4_451734AD395E__INCLUDED_)
	#include "System/RaptorErrorManager.h"
#endif

#include "Subsys/FreeType/FTFace.h"
#include "Subsys/FreeType/TTOutlineFont.h"
#include "Subsys/Hershey/Simplex.h"

RAPTOR_NAMESPACE

CGLVectorFont::CGLVectorFont(const std::string& name)
	:CGLFont(name)
{
}


CGLVectorFont::~CGLVectorFont()
{
}

bool CGLVectorFont::glGenGlyphs(float precision,
								float extrusion,
								float scale)
{
	glyph *glyphs = new glyph[256];

	string c;
	for (int i = 0; i < 256; i++)
	{
		c = (char)i;
		RAPTOR_HANDLE h = glBuildVectors(c, scale, 
										 &(glyphs[i].halfWidth),
										 &(glyphs[i].height));
		glyphs[i].advance = -2 * glyphs[i].halfWidth;
		glyphs[i].glList = h.glname();
	}

	m_glfontglyph.push_back(glyphs);
	return CGLFont::glGenGlyphs(precision, extrusion, scale);
}

RAPTOR_HANDLE CGLVectorFont::glBuildVectors(const std::string &str,
											float scale,
											float *width,
											float *height)
{
	float w = 0.0f;
	const char* text = str.data();
	const int text_length = str.size();

	RAPTOR_HANDLE result(CGLFont::CGLFontClassID::GetClassId().ID(), glGenLists(1));
	glNewList(result.glname(), GL_COMPILE);

	if (height != NULL)
		*height = -25.0f * scale;

	for (int i = 0; i < text_length; ++i)
	{
		int ch = (unsigned char)text[i];
		if ((ch >= 32) && (ch <= 126))
		{
			ch -= 32;
			w += scale * simplex[ch][1];
			int nb = 2 * simplex[ch][0];

			glBegin(GL_LINE_STRIP);
			for (int j = 2; j <= nb; j += 2)
			{
				if (simplex[ch][j] == -1)
				{
					glEnd();
					glBegin(GL_LINE_STRIP);
				}
				else
					glVertex2f(	scale * simplex[ch][j],
								scale * simplex[ch][j + 1]);
			}
			glEnd();
		}
	}

	glEndList();

	CATCH_GL_ERROR

	if (width != NULL)
		*width = -0.5f * w;

	return result;
}

void CGLVectorFont::glWrite(const std::string &text, int x, int y, const CColor::RGBA &color)
{
	float lineWidth = 1.0f;
	glGetFloatv(GL_LINE_WIDTH, &lineWidth);

	GLfloat viewport[4];
	glGetFloatv(GL_VIEWPORT, viewport);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(viewport[0], viewport[2], viewport[1], viewport[3], 1, 20);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glTranslatef(0.0f, 0.0f, -5.0f);
	glEnable(GL_BLEND);
	glEnable(GL_LINE_SMOOTH);
	glHint(GL_LINE_SMOOTH, GL_NICEST);

	selectCurrentGlyphset(0);
	glPushMatrix();
	glLineWidth(3.0f);
	CGLFont::glWrite(text, x, y, CColor::RGBA(0.0f, 0.0f, 0.0f, 1.0f));
	glLineWidth(1.1f);
	CGLFont::glWrite(text, x, y, color);
	glPopMatrix();

	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);

	glLineWidth(lineWidth);
}

void CGLVectorFont::glWrite(const std::vector<CGLFont::FONT_TEXT_ITEM> &lines)
{
	float lineWidth = 1.0f;
	glGetFloatv(GL_LINE_WIDTH, &lineWidth);

	GLfloat viewport[4];
	glGetFloatv(GL_VIEWPORT, viewport);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(viewport[0], viewport[2], viewport[1], viewport[3], 1, 20);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glTranslatef(0.0f, 0.0f, -5.0f);
	glEnable(GL_BLEND);
	glEnable(GL_LINE_SMOOTH);
	glHint(GL_LINE_SMOOTH, GL_NICEST);

	selectCurrentGlyphset(0);
	glPushMatrix();
	glColor4f(0.0f, 0.0f, 0.0f, 1.0f);
	glLineWidth(3.0f);
	CGLFont::glWrite(lines);
	glLineWidth(1.1f);
	CGLFont::glWrite(lines);
	glPopMatrix();

	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);

	glLineWidth(lineWidth);
}