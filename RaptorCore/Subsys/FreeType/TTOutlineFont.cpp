
#include "TTOutlineFont.h"
#include "FTGlyphOutline.h"

/////////////////////////////////////////////////////////////////////////////

TTOutlineFont::TTOutlineFont()
{
}

/////////////////////////////////////////////////////////////////////////////

TTOutlineFont::~TTOutlineFont()
{
}

/////////////////////////////////////////////////////////////////////////////

FTGlyphOutline* TTOutlineFont::getOutline( int ascii_code )
{
	if( ascii_code < 0 || ascii_code > 255 )
		return 0;

	FTGlyph* glyph = getGlyph(ascii_code);
	if ((glyph == 0) || (FT_GLYPH_FORMAT_OUTLINE != glyph->getFormat()))
		return NULL;

	return (FTGlyphOutline*)glyph;
}

/////////////////////////////////////////////////////////////////////////////

FTGlyph* TTOutlineFont::createGlyph(FTFace* face,int ascii_code)
{
	FTGlyph* glyph= new FTGlyphOutline(face,ascii_code);
	if( ! glyph->create() )
	{
		delete glyph;
		glyph = NULL;
	}

	return glyph;
}

