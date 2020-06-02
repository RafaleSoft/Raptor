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
#if !defined(AFX_GEOMETRYALLOCATOR_H__802B3C7A_43F7_46B2_A79E_DDDC9012D371__INCLUDED_)
	#include "Subsys/GeometryAllocator.h"
#endif
#if !defined(AFX_RAPTORINSTANCE_H__90219068_202B_46C2_BFF0_73C24D048903__INCLUDED_)
	#include "Subsys/RaptorInstance.h"
#endif
#if !defined(AFX_SHADER_H__4D405EC2_7151_465D_86B6_1CA99B906777__INCLUDED_)
	#include "GLHierarchy/Shader.h"
#endif
#if !defined(AFX_UNIFORMALLOCATOR_H__4DD62C99_E476_4FE5_AEE4_EEC71F7B0F38__INCLUDED_)
	#include "Subsys/UniformAllocator.h"
#endif
#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
	#include "System/Raptor.h"
#endif
#if !defined(AFX_OPENGLSHADERSTAGE_H__56B00FE3_E508_4FD6_9363_90E6E67446D9__INCLUDED_)
	#include "GLHierarchy/OpenGLShaderStage.h"
#endif


#include "Subsys/FreeType/FTFace.h"
#include "Subsys/FreeType/TTOutlineFont.h"
#include "Subsys/Hershey/Simplex.h"

RAPTOR_NAMESPACE

static const size_t FONT_SIZE = 256;
static const size_t LINE_SIZE = 256;
static const size_t NUM_STRIPS = 4;


//!	IMPORTANT : ensure alignment on a multiple number of vec4 size elements.
typedef struct VECTOR_CACHEELT_t
{
	GL_TEX_VERTEX	strip[56];
	float			advance;
	float			reserved[7];
} VECTOR_CACHEELT;
static VECTOR_CACHEELT_t vector_cache[FONT_SIZE];

typedef struct STRIPS_ELT_t
{
	uint32_t		strips[4];	// start and len combined.
} STRIPS_ELT;
static STRIPS_ELT_t strips[FONT_SIZE];

static const size_t VECTOR_CACHEELT_SIZE = sizeof(VECTOR_CACHEELT);

typedef struct LINE_ELT_t
{
	float	advance;
	float	line;
	float	num_char;
	float	num_strip;
} LINE_ELT;

static const size_t LINE_ELT_SIZE = sizeof(LINE_ELT);
static LINE_ELT font_line[LINE_SIZE];
static float *font_linePointer = NULL;



CGLVectorFont::CGLVectorFont(const std::string& name)
	:CGLFont(name),
	m_pBinder(NULL), m_fontUniformSize(0), m_fontUniform(NULL)
{
}


CGLVectorFont::~CGLVectorFont()
{
}

bool CGLVectorFont::glGenGlyphs(float precision,
								float extrusion,
								float scale)
{
	glyph *glyphs = new glyph[FONT_SIZE];

	if (NULL == font_linePointer)
	{
		CUniformAllocator*	pUAllocator = CUniformAllocator::GetInstance();
		if (NULL != m_fontUniform)
			pUAllocator->releaseUniforms(m_fontUniform);
		else
		{
			m_fontUniformSize = VECTOR_CACHEELT_SIZE * (126 - 32) + 1;
#ifndef RAPTOR_DEBUG_MODE_GENERATION
			int S = 0;
			glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &S);
			if (m_fontUniformSize > S)
			{
				Raptor::GetErrorManager()->generateRaptorError( CGLFont::CGLFontClassID::GetClassId(),
																CRaptorErrorManager::RAPTOR_ERROR,
																CRaptorMessages::ID_INIT_FAILED);
				return false;
			}
#endif
			m_fontUniform = pUAllocator->allocateUniforms(m_fontUniformSize);
		}

		CGeometryAllocator *pAllocator = CGeometryAllocator::GetInstance();
		if (pAllocator->isMemoryRelocated())
			font_linePointer = pAllocator->allocateVertices(LINE_SIZE * LINE_ELT_SIZE / sizeof(float));
		else
			font_linePointer = &font_line[0].advance;
	}

	if (NULL == m_pBinder)
	{
		LINE_ELT* pCache = (LINE_ELT*)font_linePointer;
		CResourceAllocator::CResourceBinder *binder = new CResourceAllocator::CResourceBinder();
		binder->setArray(CProgramParameters::POSITION, &font_linePointer[0], 4, 0);
		binder->useVertexArrayObjects();
		m_pBinder = binder;
	}

	string c;
	for (size_t i = 0; i < FONT_SIZE; i++)
	{
		STRIPS_ELT_t& rStrip = strips[i];
		rStrip.strips[3] = rStrip.strips[2] = rStrip.strips[1] = rStrip.strips[0] = 0;

		c = (char)i;
		RAPTOR_HANDLE h = glBuildVectors(c, scale, 
										 &(glyphs[i].halfWidth),
										 &(glyphs[i].height));
		glyphs[i].advance = -2 * glyphs[i].halfWidth;
		glyphs[i].glList = h.glname();

		VECTOR_CACHEELT_t& rCache = vector_cache[i];
		rCache.advance = -2 * glyphs[i].halfWidth;
	}

	m_glfontglyph.push_back(glyphs);

	if (NULL != m_fontUniform)
	{
		CUniformAllocator*	pUAllocator = CUniformAllocator::GetInstance();
		//vector_cache[33].strip[0].u = 4;
		//vector_cache[33].strip[0].v = 2;
		VECTOR_CACHEELT_t *cache = &vector_cache[32];
		pUAllocator->glvkSetPointerData(m_fontUniform, (uint8_t*)cache, m_fontUniformSize);
	}

	return CGLFont::glGenGlyphs(precision, extrusion, scale);
}

RAPTOR_HANDLE CGLVectorFont::glBuildVectors(const std::string &str,
											float scale,
											float *width,
											float *height)
{
	float w = 0.0f;
	const char* text = str.data();
	const size_t text_length = str.size();

	RAPTOR_HANDLE result(CGLFont::CGLFontClassID::GetClassId().ID(), glGenLists(1));
	glNewList(result.glname(), GL_COMPILE);

	if (height != NULL)
		*height = -25.0f * scale;

	for (size_t i = 0; i < text_length; ++i)
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

	if (width != NULL)
		*width = -0.5f * w;
	
	for (size_t i = 0; i < text_length; ++i)
	{
		int ch = (unsigned char)text[i];
		if ((ch >= 32) && (ch <= 126))
		{
			VECTOR_CACHEELT& rCache = vector_cache[ch];
			STRIPS_ELT_t& rStrip = strips[ch];
			size_t num_strip = 0;

			ch -= 32;
			int nb = 2 * simplex[ch][0];

			GL_TEX_VERTEX *strip = &rCache.strip[0];
			uint32_t strip_start = 0;
			uint32_t strip_size = 0;
			uint32_t strip_pos = 0;

			for (int j = 2; j <= nb; j += 2)
			{
				if (simplex[ch][j] == -1)
				{
					rStrip.strips[num_strip] = (strip_start << 16) + strip_size;

					strip_size = 0;
					strip_start = strip_pos;
					num_strip++;
				}
				else
				{
					strip[strip_pos].u = scale * simplex[ch][j];
					strip[strip_pos].v = scale * simplex[ch][j + 1];
					strip_size++;
					strip_pos++;
				}
			}
			rStrip.strips[num_strip] = (strip_start << 16) + strip_size;
		}
	}

	CATCH_GL_ERROR

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
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

	selectCurrentGlyphset(0);
	glLineWidth(3.0f);
	CGLFont::glWrite(text, x, y, CColor::RGBA(0.0f, 0.0f, 0.0f, 1.0f));
	glLineWidth(1.1f);
	CGLFont::glWrite(text, x, y, color);

	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);

	glLineWidth(lineWidth);
}

// This drawing method is only kept for compatibility and debugging purposes
/*
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
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

	std::vector<CGLFont::FONT_TEXT_ITEM> dark_lines = lines;
	for (size_t i = 0; i < dark_lines.size(); i++)
		dark_lines[i].color = CColor::RGBA(0.0f, 0.0f, 0.0f, 0.5f);

	selectCurrentGlyphset(0);
	glLineWidth(3.0f);
	CGLFont::glWrite(dark_lines);

	glLineWidth(1.1f);
	CGLFont::glWrite(lines);

	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);

	glLineWidth(lineWidth);
}
*/
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
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

	selectCurrentGlyphset(0);
	
	CResourceAllocator::CResourceBinder *binder = (CResourceAllocator::CResourceBinder*)m_pBinder;
	binder->glvkBindArrays();
	
	std::vector<CGLFont::FONT_TEXT_ITEM> test_lines = lines;
	FONT_TEXT_ITEM alphabet;
	alphabet.text = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	alphabet.x_offset = 0;
	alphabet.y_offset = 40;
	test_lines.push_back(alphabet);
	alphabet.text = "abcdefghijklmnopqrstuvwxyz";
	alphabet.y_offset = 50;
	test_lines.push_back(alphabet);
	size_t count = 0;
	for (size_t j = 0; j < test_lines.size(); j++)
	{
		const FONT_TEXT_ITEM &item = test_lines[j];

		float advance = item.x_offset;
		size_t sz = item.text.size();
		for (size_t i = 0; i < sz; i++)
		{
			uint8_t ch = item.text[i];

			VECTOR_CACHEELT_t &elt = vector_cache[ch];
			STRIPS_ELT_t& rStrip = strips[ch];

			uint8_t nb_strips = (rStrip.strips[0] > 0 ? 1 : 0);
			nb_strips = nb_strips + (rStrip.strips[1] > 0 ? 1 : 0);
			nb_strips = nb_strips + (rStrip.strips[2] > 0 ? 1 : 0);
			nb_strips = nb_strips + (rStrip.strips[3] > 0 ? 1 : 0);

			for (uint8_t strip = 0; strip < nb_strips; strip++)
			{
				font_line[count].advance = advance;
				font_line[count].line = viewport[3] - item.y_offset - 1;
				font_line[count].num_char = (ch - 32);
				font_line[count].num_strip = rStrip.strips[strip];
				count++;
			}

			advance = advance + elt.advance;
		}
	}
	
	CRaptorInstance &instance = CRaptorInstance::GetInstance();

	CGeometryAllocator *pAllocator = CGeometryAllocator::GetInstance();
	if (pAllocator->isMemoryRelocated())
		pAllocator->glvkSetPointerData((float*)font_linePointer, (float*)&font_line[0], (GLsizei)(LINE_ELT_SIZE / sizeof(float) * count));
	
	CUniformAllocator*	pUAllocator = CUniformAllocator::GetInstance();
	pUAllocator->glvkBindUniform(m_fontUniform, 0);

	CProgramParameters params;
	GL_COORD_VERTEX vp(viewport[0], viewport[1], 0.5f * viewport[2], 0.5f * viewport[3]);
	CColor::RGBA color(0.0f, 0.0f, 0.0f, 0.5f);
	params.addParameter("viewport", vp);
	params.addParameter("color", color);
	COpenGLShaderStage *stage = instance.m_pVectorFontShader->glGetOpenGLShader();
	stage->updateProgramParameters(params);

	//!	First rendering : a dark surrounding with line width of 3
	glLineWidth(3.0f);
	instance.m_pVectorFontShader->glRender();
	glDrawArrays(GL_POINTS, 0, (GLsizei)count);
	
	params.clear();
	color = CColor::RGBA(1.0f, 0.0f, 0.0f, 1.0f);
	params.addParameter("color", color);
	stage->updateProgramParameters(params);
	
	//!	Second rendering : a dark surrounding with line width of 1.1
	glLineWidth(1.1f);
	instance.m_pVectorFontShader->glRender();
	glDrawArrays(GL_POINTS, 0, (GLsizei)count);

	instance.m_pVectorFontShader->glStop();
	binder->glvkUnbindArrays();


	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);

	glLineWidth(lineWidth);
}
