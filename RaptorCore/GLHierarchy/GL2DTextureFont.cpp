// GL2DTextureFont.cpp: implementation of the CGL2DTextureFont class.
//
//////////////////////////////////////////////////////////////////////
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

#define TEXTURE_WIDTH 256

RAPTOR_NAMESPACE

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

bool CGL2DTextureFont::init(const std::string &filename, unsigned int size, bool antialiased)
{
	m_bAntialiased = antialiased;

	if (CGL2DFont::init(filename, size))
	{
		unsigned int h = 0;
		unsigned int w = 0;

		// find size of biggest char
		for (unsigned int i = 0; i<256; i++)
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
												CTextureObject::CGL_BILINEAR);
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

		unsigned char *buffer = new unsigned char[2 * W*H];
		memset(buffer, 0, 2 * W*H);
		for (unsigned int i = 0; i<256; i++)
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
			unsigned char *AAbuffer = new unsigned char[2 * W*H];
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
	glyph *glyphs = new glyph[256];
	if (NULL == m_bmfont)
		return false;

	string c;
	for (int i = 0; i < 256; i++)
	{
		c = (char)i;

		FTGlyphBitmap* gbitmap = m_bmfont->getBitmap(i);
		if (gbitmap != 0)
		{
			float u = m_char_w * (i % (TEXTURE_WIDTH / m_char_w));
			float v = m_char_h * (i / (TEXTURE_WIDTH / m_char_w));

			float dx = gbitmap->getDeltaX();
			float dy = gbitmap->getDeltaY();

			int H = m_char_w * m_char_h;

			float ww = gbitmap->getWidth();
			float hh = gbitmap->getHeight();

			RAPTOR_HANDLE h;
			h.hClass = CGLFont::CGLFontClassID::GetClassId().ID();
			h.handle = glGenLists(1);
			glNewList(h.handle, GL_COMPILE);
			glBegin(GL_QUADS);
				glTexCoord2f(u / TEXTURE_WIDTH, v / H);					glVertex2f(dx, dy);
				glTexCoord2f((u + ww) / TEXTURE_WIDTH, v / H);			glVertex2f(dx + ww, dy);
				glTexCoord2f((u + ww) / TEXTURE_WIDTH, (v + hh) / H);	glVertex2f(dx + ww, dy + hh);
				glTexCoord2f(u / TEXTURE_WIDTH, (v + hh) / H);			glVertex2f(dx, dy + hh);
			glEnd();
			glEndList();

			glyphs[i].halfWidth = -0.5f * ww;
			glyphs[i].height = hh;
			glyphs[i].advance = gbitmap->getAdvance();
			glyphs[i].glList = h.handle;
		}
	}

	m_glfontglyph.push_back(glyphs);
	return CGLFont::glGenGlyphs(precision, extrusion, scale);
}

void CGL2DTextureFont::glWrite(const std::string &text, int x, int y)
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

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	m_texture->glvkRender();

	glTranslatef(x, viewport[3] - y, -1.0f);
	for (unsigned int i = 0; i<text.size(); i++)
	{
		unsigned char ch = text[i];

		glyph *glyphs = (glyph*)m_glfontglyph.at(0);
		glyph g = glyphs[ch];
		glCallList(g.glList);
		glTranslatef(g.advance, 0.0f, 0.0f);
	}
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	CATCH_GL_ERROR
}

