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
		glyphs[i].glList = h.handle();
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
	glNewList(result.handle(), GL_COMPILE);

	if (height != NULL)
		*height = -25.0f;

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
					glVertex2f(scale * simplex[ch][j],
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
