
#include "TTBitmapFont.h"
#include "FTGlyphBitmap.h"

/////////////////////////////////////////////////////////////////////////////

TTBitmapFont::TTBitmapFont()
{
}

/////////////////////////////////////////////////////////////////////////////

TTBitmapFont::~TTBitmapFont()
{
}

/////////////////////////////////////////////////////////////////////////////

FTGlyphBitmap* TTBitmapFont::getBitmap( int ascii_code )
{
	if( ascii_code < 0 || ascii_code > 255 )
		return 0;

	FTGlyph* glyph = getGlyph(ascii_code);
	if ((glyph == 0) || (FT_GLYPH_FORMAT_BITMAP != glyph->getFormat()))
		return NULL;

	return (FTGlyphBitmap*)glyph;
}

/////////////////////////////////////////////////////////////////////////////

FTGlyph* TTBitmapFont::createGlyph(FTFace* face,int ascii_code)
{
	FTGlyph* glyph= new FTGlyphBitmap(face,ascii_code);
	if( ! glyph->create() )
	{
		delete glyph;
		glyph = NULL;
	}

	return glyph;
}

