
#include <string.h>

#include "FTGlyphBitmap.h"
#include "FTGlyph.h"
#include "FTFace.h"

/////////////////////////////////////////////////////////////////////////////

FTGlyphBitmap::FTGlyphBitmap(FTFace* _instance,int ascii_code)
	:FTGlyph(_instance,ascii_code),
	m_rowSize(0), m_pBuffer(NULL),m_delta_x(0),m_delta_y(0)
{
	
}

/////////////////////////////////////////////////////////////////////////////

FTGlyphBitmap::~FTGlyphBitmap()
{
	if (0 != m_pBuffer)
		delete [] m_pBuffer;
}

/////////////////////////////////////////////////////////////////////////////

bool FTGlyphBitmap::create()
{
	if( face == 0 )
		return false;

	int glyph_index= face->getGlyphIndex(ascii_code);
	FT_Face _face = face->getFace();
	FT_Error error = FT_Load_Glyph(	_face,			/* handle to face object */
									glyph_index,    /* glyph index           */
									FT_LOAD_DEFAULT );
	FT_Bitmap bitmap = FT_Bitmap();
	if (0 == error)
	{
		FT_GlyphSlot _glyph = _face->glyph;
		metrics = _glyph->metrics;
		if (FT_GLYPH_FORMAT_BITMAP == _glyph->format)
			bitmap = _glyph->bitmap;
		if (FT_GLYPH_FORMAT_BITMAP != format)
		{
			error = FT_Render_Glyph(_face->glyph,   /* glyph slot  */
									//FT_RENDER_MODE_NORMAL );
									 FT_RENDER_MODE_MONO);
			if (0 != error)
				return false;
			else
			{
				bitmap = _glyph->bitmap;
				format = FT_GLYPH_FORMAT_BITMAP;
				m_delta_x = _glyph->bitmap_left;
				m_delta_y = _glyph->bitmap_top;
			}
		}
	}
	else
		return false;

	m_width = bitmap.width;
	m_height = bitmap.rows;
	m_rowSize = abs(bitmap.pitch);
	advance = metrics.horiAdvance >> 6;
	m_delta_y -= m_height;
	
	int size = abs(bitmap.pitch) * bitmap.rows;
	if (0 != m_pBuffer)
		delete [] m_pBuffer;
	m_pBuffer = new unsigned char [ size ];

	for(unsigned int i=0;i<bitmap.rows;i++)
	{
		unsigned char *src = bitmap.buffer + i*m_rowSize;
		unsigned char *dst = m_pBuffer + (bitmap.rows - i - 1)*m_rowSize;
		memcpy(dst,src,m_rowSize);
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////
/*
bool FTGlyphBitmap::expandBitmap(unsigned char* buffer,unsigned int stride) const
{
	if ((0 == buffer) || (0 == m_pBuffer))
		return false;

	int x = w * (i % (TEXTURE_WIDTH / w));
	int y = h * (i / (TEXTURE_WIDTH / w));
			
	size_t pos = 0;
	for (size_t k=0;k<m_height;k++)
	{
		pos = 2*(y+k)*W + 2*x;
		for (size_t j=0;j<m_width;j++)
		{
			size_t ofs = (j >> 3) + k*m_rowSize;
			unsigned char val = m_pBuffer[ofs];
			int column = 128 >> (j%8);

			unsigned char b = (val & column) ? 255 : 0;
			buffer[pos++] = b;
			buffer[pos++] = b;
		}
	}

	return true;
}
*/