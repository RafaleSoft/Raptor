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
#if !defined(AFX_RAPTORGLEXTENSIONS_H__E5B5A1D9_60F8_4E20_B4E1_8E5A9CB7E0EB__INCLUDED_)
	#include "System/RaptorGLExtensions.h"
#endif
#if !defined(AFX_VERTEXSHADER_H__204F7213_B40B_4B6A_9BCA_828409871B68__INCLUDED_)
	#include "GLHierarchy/VertexShader.h"
#endif
#if !defined(AFX_GEOMETRYSHADER_H__1981EA98_8F3C_4881_9429_A9ACA5B285D3__INCLUDED_)
	#include "GLHierarchy/GeometryShader.h"
#endif
#if !defined(AFX_SHADER_H__4D405EC2_7151_465D_86B6_1CA99B906777__INCLUDED_)
	#include "GLHierarchy/Shader.h"
#endif
#if !defined(AFX_OPENGLSHADERSTAGE_H__56B00FE3_E508_4FD6_9363_90E6E67446D9__INCLUDED_)
	#include "GLHierarchy/OpenGLShaderStage.h"
#endif
#if !defined(AFX_FRAGMENTSHADER_H__CC35D088_ADDF_4414_8CB6_C9D321F9D184__INCLUDED_)
	#include "GLHierarchy/FragmentShader.h"
#endif




static const std::string font_vp_src =
"#version 440 \n\
\n\
uniform vec4 viewport; \n\
\n\
layout(location = 0) in vec4 i_Position; \n\
layout(location = 8) in vec4 i_TexCoord; \n\
\n\
out vec4 v_texCoord; \n\
out vec2 v_size; \n\
\n\
void main (void) \n\
{\n\
	vec4 pos = vec4(i_Position.x / viewport.z - 1.0, 1.0 - i_Position.y / viewport.w, 0.0, 1.0); \n\
	gl_Position = pos; \n\
	v_texCoord = i_TexCoord; \n\
	v_size = vec2(i_Position.z / viewport.z, i_Position.w / viewport.w); \n\
}";

static const std::string font_gp_src =
"#version 440\n\
\n\
//	Expect the geometry shader extension to be available, warn if not. \n\
#extension GL_ARB_geometry_shader4 : enable \n\
\n\
in vec4 v_texCoord[]; \n\
in vec2 v_size[]; \n\
\n\
layout(points) in; \n\
layout(triangle_strip, max_vertices=4) out; \n\
layout(location = 1) out vec4 g_TexCoord; \n\
\n\
void main() \n\
{\n\
	gl_Position = gl_in[0].gl_Position; \n\
	g_TexCoord = vec4(v_texCoord[0].x, v_texCoord[0].y, 0.0, 0.0); \n\
	EmitVertex(); \n\
	\n\
	gl_Position = gl_in[0].gl_Position + vec4(v_size[0].x, 0.0, 0.0, 0.0); \n\
	g_TexCoord = vec4(v_texCoord[0].x + v_texCoord[0].z, v_texCoord[0].y, 0.0, 0.0); \n\
	EmitVertex(); \n\
	\n\
	gl_Position = gl_in[0].gl_Position + vec4(0.0, v_size[0].y, 0.0, 0.0); \n\
	g_TexCoord = vec4(v_texCoord[0].x, v_texCoord[0].y + v_texCoord[0].w, 0.0, 0.0); \n\
	EmitVertex(); \n\
	\n\
	gl_Position = gl_in[0].gl_Position + vec4(v_size[0].x,v_size[0].y, 0.0, 0.0); \n\
	g_TexCoord = vec4(v_texCoord[0].x + v_texCoord[0].z, v_texCoord[0].y + v_texCoord[0].w, 0.0, 0.0); \n\
	EmitVertex(); \n\
	\n\
	EndPrimitive(); \n\
}";

static const std::string font_fp_src =
"#version 440\n\
\n\
uniform	sampler2D diffuseMap; \n\
uniform vec4 color; \n\
\n\
layout(location = 1) in vec4 g_TexCoord; \n\
layout(location = 0) out vec4 o_Color;	\n\
\n\
void main (void) \n\
{\n\
	vec4 col = texture(diffuseMap,vec2(g_TexCoord.st)); \n\
	o_Color = color * vec4(col.x,col.x,col.x,col.w); \n\
}";



RAPTOR_NAMESPACE

static const size_t TEXTURE_WIDTH = 256;
static const size_t FONT_SIZE = 256;
static const size_t LINE_SIZE = 256;

struct FONT_CACHEELT_t
{
	GL_COORD_VERTEX coord;
	GL_COORD_VERTEX	texcoord;
	float			advance;
} FONT_CACHEELT;
static FONT_CACHEELT_t font_cache[FONT_SIZE];
static size_t FONT_CACHEPOINTER_SIZE = sizeof(FONT_CACHEELT)*FONT_SIZE / sizeof(float);
static size_t LINE_CACHEPOINTER_SIZE = sizeof(FONT_CACHEELT)*LINE_SIZE / sizeof(float);

static FONT_CACHEELT_t font_line[LINE_SIZE];
static float *font_linePointer = NULL;

CShader *CGL2DTextureFont::m_pShader = NULL;



CGL2DTextureFont::CGL2DTextureFont(const std::string& name)
	:CGL2DFont(name),
	m_texture(NULL),
	m_bAntialiased(false),
	m_char_w(0),
	m_char_h(0)
{
}


CGL2DTextureFont::~CGL2DTextureFont()
{
}

bool CGL2DTextureFont::glInit(const std::string &filename, unsigned int size, bool antialiased)
{
	m_bAntialiased = antialiased;

	if (CGL2DFont::init(filename, size))
	{
		unsigned int h = 0;
		unsigned int w = 0;

		// find size of biggest char
		for (unsigned int i = 0; i<FONT_SIZE; i++)
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
			m_texture = factory.glCreateTexture(ITextureObject::CGL_LIGHTMAP_ALPHA,
												CTextureObject::CGL_MULTIPLY, // CGL_ALPHA_TRANSPARENT
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
		size_t W = TEXTURE_WIDTH;
		size_t H = h * w; //h * 256 / nb;
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
					 GL_LUMINANCE8_ALPHA8,
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

	if (NULL == m_pShader)
	{
		m_pShader = new CShader(getName() + "_SHADER");
		COpenGLShaderStage *stage = m_pShader->glGetOpenGLShader();

		CVertexShader *vp = stage->glGetVertexShader();
		bool res = vp->glLoadProgram(font_vp_src);
		CProgramParameters params;
		GL_COORD_VERTEX viewport(0, 0, 640, 480);
		params.addParameter("viewport", viewport);
		vp->setProgramParameters(params);

		CGeometryShader *gp = stage->glGetGeometryShader();
		gp->setGeometry(GL_POINTS, GL_TRIANGLE_STRIP, 4);
		res = res & gp->glLoadProgram(font_gp_src);

		CFragmentShader *fs = stage->glGetFragmentShader();
		res = res & fs->glLoadProgram(font_fp_src);
		params.clear();
		params.addParameter("diffuseMap", CTextureUnitSetup::IMAGE_UNIT_0);
		params.addParameter("color", CColor::RGBA(1.0, 0.0, 0.0, 1.0));
		fs->setProgramParameters(params);

		res = res & stage->glCompileShader();
		if (!res)
			return false;
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

	GLfloat viewport[4];
	glGetFloatv(GL_VIEWPORT, viewport);

	CProgramParameters params;
	GL_COORD_VERTEX vp(viewport[0], viewport[1], 0.5f * viewport[2], 0.5f * viewport[3]);
	params.addParameter("viewport", vp);
	m_pShader->glGetOpenGLShader()->glGetVertexShader()->updateProgramParameters(params);
	params.clear();
	params.addParameter("color", color);
	m_pShader->glGetOpenGLShader()->glGetFragmentShader()->updateProgramParameters(params);

	FONT_CACHEELT_t* pCache = (FONT_CACHEELT_t*)font_linePointer;
#if defined(GL_ARB_vertex_program)
	const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();
	pExtensions->glEnableVertexAttribArrayARB(CProgramParameters::POSITION);
	pExtensions->glEnableVertexAttribArrayARB(CProgramParameters::TEXCOORD0);

	pExtensions->glVertexAttribPointerARB(CProgramParameters::POSITION, 4, GL_FLOAT, false, sizeof(FONT_CACHEELT), &pCache[0].coord);
	pExtensions->glVertexAttribPointerARB(CProgramParameters::TEXCOORD0, 4, GL_FLOAT, false, sizeof(FONT_CACHEELT), &pCache[0].texcoord);
#else
#endif

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
		pAllocator->glvkCopyPointer(font_linePointer, (float*)&font_line[0], sizeof(FONT_CACHEELT) * sz / sizeof(float));

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	m_pShader->glRender();
	m_texture->glvkRender();

	glDrawArrays(GL_POINTS, 0, sz);

	m_pShader->glStop();

#if defined(GL_ARB_vertex_program)
	pExtensions->glDisableVertexAttribArrayARB(CProgramParameters::POSITION);
	pExtensions->glDisableVertexAttribArrayARB(CProgramParameters::TEXCOORD0);
#else
#endif

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

	GLfloat viewport[4];
	glGetFloatv(GL_VIEWPORT, viewport);
	CColor::RGBA color = CColor::RGBA(1.0f, 0.0f, 0.0f, 1.0f);

	FONT_CACHEELT_t* pCache = (FONT_CACHEELT_t*)font_linePointer;
#if defined(GL_ARB_vertex_program)
	const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();
	pExtensions->glEnableVertexAttribArrayARB(CProgramParameters::POSITION);
	pExtensions->glEnableVertexAttribArrayARB(CProgramParameters::TEXCOORD0);

	pExtensions->glVertexAttribPointerARB(CProgramParameters::POSITION, 4, GL_FLOAT, false, sizeof(FONT_CACHEELT), &pCache[0].coord);
	pExtensions->glVertexAttribPointerARB(CProgramParameters::TEXCOORD0, 4, GL_FLOAT, false, sizeof(FONT_CACHEELT), &pCache[0].texcoord);
#else
#endif

	CProgramParameters params;
	GL_COORD_VERTEX vp(viewport[0], viewport[1], 0.5f * viewport[2], 0.5f * viewport[3]);
	params.addParameter("viewport", vp);
	m_pShader->glGetOpenGLShader()->glGetVertexShader()->updateProgramParameters(params);
	params.clear();
	params.addParameter("color", color);
	m_pShader->glGetOpenGLShader()->glGetFragmentShader()->updateProgramParameters(params);

	GLsizei count = 0;
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
		pAllocator->glvkCopyPointer(font_linePointer, (float*)&font_line[0], sizeof(FONT_CACHEELT) * count / sizeof(float));

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	m_pShader->glRender();
	m_texture->glvkRender();

	glDrawArrays(GL_POINTS, 0, count);

	m_pShader->glStop();

#if defined(GL_ARB_vertex_program)
	pExtensions->glDisableVertexAttribArrayARB(CProgramParameters::POSITION);
	pExtensions->glDisableVertexAttribArrayARB(CProgramParameters::TEXCOORD0);
#else
#endif

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);

	CATCH_GL_ERROR
}