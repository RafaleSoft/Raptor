/***************************************************************************/
/*                                                                         */
/*  GL2DTextureFont.cpp                                                    */
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

#include "Subsys/FreeType/TTBitmapFont.h"
#include "Subsys/FreeType/FTGlyphBitmap.h"

#if !defined(AFX_GL2DTEXTUREFONT_H__7122B2F2_8D47_492F_8738_71FE06D8BA21__INCLUDED_)
	#include "GL2DTextureFont.h"
#endif
#if !defined(AFX_TEXTUREFACTORY_H__1B470EC4_4B68_11D3_9142_9A502CBADC6B__INCLUDED_)
	#include "GLHierarchy/TextureFactory.h"
#endif
#if !defined(AFX_RAPTORERRORMANAGER_H__FA5A36CD_56BC_4AA1_A5F4_451734AD395E__INCLUDED_)
	#include "System/RaptorErrorManager.h"
#endif
#if !defined(AFX_GEOMETRYALLOCATOR_H__802B3C7A_43F7_46B2_A79E_DDDC9012D371__INCLUDED_)
	#include "Subsys/GeometryAllocator.h"
#endif
#if !defined(AFX_SHADER_H__4D405EC2_7151_465D_86B6_1CA99B906777__INCLUDED_)
	#include "GLHierarchy/Shader.h"
#endif
#if !defined(AFX_OPENGLSHADERSTAGE_H__56B00FE3_E508_4FD6_9363_90E6E67446D9__INCLUDED_)
	#include "GLHierarchy/OpenGLShaderStage.h"
#endif
#if !defined(AFX_RAPTORINSTANCE_H__90219068_202B_46C2_BFF0_73C24D048903__INCLUDED_)
	#include "Subsys/RaptorInstance.h"
#endif


RAPTOR_NAMESPACE

static const size_t TEXTURE_WIDTH = 256;
static const size_t FONT_SIZE = 256;
static const size_t LINE_SIZE = 256;

typedef struct FONT_CACHEELT_t
{
	GL_COORD_VERTEX coord;
	GL_COORD_VERTEX	texcoord;
	float			advance;
} FONT_CACHEELT;
static FONT_CACHEELT_t font_cache[FONT_SIZE];
static size_t FONT_CACHEPOINTER_SIZE = sizeof(FONT_CACHEELT)*FONT_SIZE / sizeof(float);
static size_t LINE_CACHEPOINTER_SIZE = sizeof(FONT_CACHEELT)*LINE_SIZE / sizeof(float);
static size_t FONT_CACHEELT_SIZE = sizeof(FONT_CACHEELT) / sizeof(float);

static FONT_CACHEELT_t font_line[LINE_SIZE];
static float *font_linePointer = NULL;



CGL2DTextureFont::CGL2DTextureFont(const std::string& name)
	:CGL2DFont(name),
	m_texture(NULL),
	m_bAntialiased(false),
	m_char_w(0),
	m_char_h(0),
	m_pBinder(NULL)
{
}


CGL2DTextureFont::~CGL2DTextureFont()
{
	if (NULL != m_pBinder)
	{
		CResourceAllocator::CResourceBinder *binder = (CResourceAllocator::CResourceBinder*)m_pBinder;
		delete binder;
	}
}

bool CGL2DTextureFont::glInit(const std::string &filename, unsigned int size, bool antialiased)
{
	m_bAntialiased = antialiased;

	if (CGL2DFont::init(filename, size))
	{
		unsigned int h = 0;
		unsigned int w = 0;

		// find size of biggest char
		for (size_t i = 0; i < FONT_SIZE; i++)
		{
			char c = (char)i;
			FTGlyphBitmap* gbitmap = m_bmfont->getBitmap(c);
			if (gbitmap == 0)
				continue;
			if (gbitmap->getBitmap() != 0)
			{
				h = MAX(h, gbitmap->getHeight());
				w = MAX(w, gbitmap->getWidth());
			}
		}

		// Create a texture of appropriate size
		CTextureFactory &factory = CTextureFactory::getDefaultFactory();
		if (m_texture == NULL)
		{
			m_texture = factory.glCreateTexture(ITextureObject::CGL_COLOR24_ALPHA,
												ITextureObject::CGL_BILINEAR);
		}

		if (m_bAntialiased)
		{
			w += 2;
			h += 2;
		}
		w = pow(2, ceil(log((float)w) / log(2.0)));
		h = pow(2, ceil(log((float)h) / log(2.0)));
		m_char_w = w;
		m_char_h = h;

		int nb = TEXTURE_WIDTH / w;
		uint32_t W = TEXTURE_WIDTH;
		uint32_t H = h * w; //h * 256 / nb;
		factory.glResizeTexture(m_texture, W, H);
		m_texture->glvkRender();

		unsigned char *buffer = new unsigned char[2 * W*H];	// lightmap alpha
		memset(buffer, 0, 2 * W*H);
		for (unsigned int i = 0; i<FONT_SIZE; i++)
		{
			char c = (char)i;
			FTGlyphBitmap* gbitmap = m_bmfont->getBitmap(c);
			if (gbitmap == 0)
				continue;

			size_t bw = gbitmap->getWidth();
			size_t bh = gbitmap->getHeight();
			size_t br = gbitmap->getRowSize();

			int x = w * (i % (TEXTURE_WIDTH / w));
			int y = h * (i / (TEXTURE_WIDTH / w));

			const unsigned char *srcbuffer = (const GLubyte*)gbitmap->getBitmap();
			size_t pos = 0;
			for (size_t k = 0; k<bh; k++)
			{
				pos = 2 * (y + k)*W + 2 * x;
				for (size_t j = 0; j<bw; j++)
				{
					size_t ofs = (j >> 3) + k*br;
					unsigned char val = srcbuffer[ofs];
					int column = 128 >> (j % 8);

					unsigned char b = (val & column) ? 255 : 0;
					buffer[pos++] = b;
					buffer[pos++] = b;
				}
			}
		}

		if (m_bAntialiased)
		{
			// Antialias font buffer
			unsigned char *AAbuffer = new unsigned char[2 * W*H];		// lightmap alpha
			for (size_t j = 0; j<H; j++)
			{
				for (size_t i = 0; i<W; i++)
				{
					size_t pos = 2 * (W*j + i);
					unsigned int nbElt = 0;
					unsigned int base = 0;

					if (i > 0)
					{
						nbElt++;
						base += buffer[pos - 2];
						if (j > 0)
						{
							nbElt++;
							base += buffer[pos - 2 - W - W];
						}
						if (j < H - 1)
						{
							nbElt++;
							base += buffer[pos - 2 + W + W];
						}
					}
					if (i < W - 1)
					{
						nbElt++;
						base += buffer[pos + 2];
						if (j > 0)
						{
							nbElt++;
							base += buffer[pos + 2 - W - W];
						}
						if (j < H - 1)
						{
							nbElt++;
							base += buffer[pos + 2 + W + W];
						}
					}
					if (j > 0)
					{
						nbElt++;
						base += buffer[pos - W - W];
					}
					if (j < H - 1)
					{
						nbElt++;
						base += buffer[pos + W + W];
					}

					base = MIN(buffer[pos] + base / nbElt, 255);
					AAbuffer[pos] = (base > 0) ? 255 : 0;
					AAbuffer[pos + 1] = base;
				}
			}

			delete[] buffer;
			buffer = AAbuffer;
		}

		// Final load
		glTexImage2D(GL_TEXTURE_2D, 0,
					 GL_RGBA4,
					 W, H, 0,
					 GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE,
					 buffer);

		delete[] buffer;
		CATCH_GL_ERROR
		//		factory.glExportTexture(m_texture,"FontTexture.tga");

		return true;
	}
	else
		return false;
}


bool CGL2DTextureFont::glGenGlyphs(float precision,
								   float extrusion,
								   float scale)
{
	glyph *glyphs = new glyph[FONT_SIZE];
	if (NULL == m_bmfont)
		return false;
	
	if (NULL == font_linePointer)
	{
		CGeometryAllocator *pAllocator = CGeometryAllocator::GetInstance();
		if (pAllocator->isMemoryRelocated())
			font_linePointer = pAllocator->allocateVertices(LINE_CACHEPOINTER_SIZE);
		else
			font_linePointer = &font_line[0].coord.x;
	}

	if (NULL == m_pBinder)
	{
		FONT_CACHEELT_t* pCache = (FONT_CACHEELT_t*)font_linePointer;
		CResourceAllocator::CResourceBinder *binder = new CResourceAllocator::CResourceBinder();
		binder->setArray(CProgramParameters::POSITION, &pCache[0].coord, 4, sizeof(FONT_CACHEELT));
		binder->setArray(CProgramParameters::TEXCOORD0, &pCache[0].texcoord, 4, sizeof(FONT_CACHEELT));
		binder->useVertexArrayObjects();
		m_pBinder = binder;
	}

	int H = m_char_w * m_char_h;
	for (int i = 0; i < FONT_SIZE; i++)
	{
		FTGlyphBitmap* gbitmap = m_bmfont->getBitmap(i);
		if (gbitmap != 0)
		{
			float u = m_char_w * (i % (TEXTURE_WIDTH / m_char_w));
			float v = m_char_h * (i / (TEXTURE_WIDTH / m_char_w));

			float dx = gbitmap->getDeltaX();
			float dy = gbitmap->getDeltaY();
			float ww = gbitmap->getWidth();
			float hh = gbitmap->getHeight();

			glyphs[i].halfWidth = -0.5f * ww;
			glyphs[i].height = hh;
			glyphs[i].advance = gbitmap->getAdvance();
			glyphs[i].glList = 0;


			font_cache[i].coord = GL_COORD_VERTEX(dx, dy, ww, hh);
			font_cache[i].texcoord = GL_COORD_VERTEX(u / TEXTURE_WIDTH, v / H, ww / TEXTURE_WIDTH, hh / H);
			font_cache[i].advance = glyphs[i].advance;
		}
	}

	m_glfontglyph.push_back(glyphs);
	return CGLFont::glGenGlyphs(precision, extrusion, scale);
}

void CGL2DTextureFont::glWrite(const std::string &text, int x, int y, const CColor::RGBA &color)
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

	CRaptorInstance &instance = CRaptorInstance::GetInstance();

	GLfloat viewport[4];
	glGetFloatv(GL_VIEWPORT, viewport);

	CProgramParameters params;
	GL_COORD_VERTEX vp(viewport[0], viewport[1], 0.5f * viewport[2], 0.5f * viewport[3]);
	params.addParameter("viewport", vp);
	params.addParameter("color", color);
	COpenGLShaderStage *stage = instance.m_pFontShader->glGetOpenGLShader();
	stage->updateProgramParameters(params);


	CResourceAllocator::CResourceBinder *binder = (CResourceAllocator::CResourceBinder*)m_pBinder;
	binder->glvkBindArrays();

	float advance = x;
	size_t sz = text.size();
	for (size_t i = 0; i < sz; i++)
	{
		unsigned char ch = text[i];
		FONT_CACHEELT_t &elt = font_cache[ch];

		font_line[i] = elt;
		font_line[i].coord.x += advance;
		font_line[i].coord.y = y - font_line[i].coord.y;
		
		advance = advance + elt.advance;
	}

	CGeometryAllocator *pAllocator = CGeometryAllocator::GetInstance();
	if (pAllocator->isMemoryRelocated())
		pAllocator->glvkSetPointerData(font_linePointer, (float*)&font_line[0], FONT_CACHEELT_SIZE * sz);

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	instance.m_pFontShader->glRender();
	m_texture->glvkRender();

	glDrawArrays(GL_POINTS, 0, (GLsizei)sz);

	instance.m_pFontShader->glStop();

	binder->glvkUnbindArrays();

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);

	CATCH_GL_ERROR
}

void CGL2DTextureFont::glWrite(const std::vector<FONT_TEXT_ITEM> &lines)
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

	CRaptorInstance &instance = CRaptorInstance::GetInstance();

	GLfloat viewport[4];
	glGetFloatv(GL_VIEWPORT, viewport);
	CColor::RGBA color = CColor::RGBA(1.0f, 0.0f, 0.0f, 1.0f);

	CResourceAllocator::CResourceBinder *binder = (CResourceAllocator::CResourceBinder*)m_pBinder;
	binder->glvkBindArrays();

	CProgramParameters params;
	GL_COORD_VERTEX vp(viewport[0], viewport[1], 0.5f * viewport[2], 0.5f * viewport[3]);
	params.addParameter("viewport", vp);
	params.addParameter("color", color);
	COpenGLShaderStage *stage = instance.m_pFontShader->glGetOpenGLShader();
	stage->updateProgramParameters(params);

	size_t count = 0;
	for (size_t l = 0; l < lines.size(); l++)
	{
		const FONT_TEXT_ITEM& item = lines[l];

		float advance = item.x_offset;
		size_t sz = item.text.size();
		for (size_t i = 0; i < sz; i++)
		{
			unsigned char ch = item.text[i];
			FONT_CACHEELT_t &elt = font_cache[ch];

			font_line[i+count] = elt;
			font_line[i+count].coord.x += advance;
			font_line[i+count].coord.y = item.y_offset - font_line[i+count].coord.y;

			advance = advance + elt.advance;
		}

		count += sz;
	}

	CGeometryAllocator *pAllocator = CGeometryAllocator::GetInstance();
	if (pAllocator->isMemoryRelocated())
		pAllocator->glvkSetPointerData(font_linePointer, (float*)&font_line[0], (GLsizei)(FONT_CACHEELT_SIZE * count));

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	instance.m_pFontShader->glRender();
	m_texture->glvkRender();

	glDrawArrays(GL_POINTS, 0, (GLsizei)count);

	instance.m_pFontShader->glStop();

	binder->glvkUnbindArrays();

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);

	CATCH_GL_ERROR
}