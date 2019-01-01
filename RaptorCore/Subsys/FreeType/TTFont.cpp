
#include "TTFont.h"

#include "FTGlyph.h"
#include "FTFace.h"

/////////////////////////////////////////////////////////////////////////////

TTFont::TTFont()
	:face(0),glyphs(0)
{
}

/////////////////////////////////////////////////////////////////////////////

TTFont::~TTFont()
{
	destroy();
}

/////////////////////////////////////////////////////////////////////////////

void TTFont::destroy()
{
	if( glyphs != 0 )
	{
		for( int i= 0; i < 256; ++i )
			delete glyphs[i];

		delete glyphs;
		glyphs= 0;
	}
}

/////////////////////////////////////////////////////////////////////////////

bool TTFont::create(FTFace* _face, int point_size )
{
	destroy();

	glyphs = new FTGlyph* [ 256 ];
	for( int i= 0; i < 256; ++i )
		glyphs[i]= 0;

	face = _face;
	if( point_size < 1 )
		point_size= 1;

	if (!face->setResolutions(96,96))
		return false;

	if (!face->setSizes(point_size,point_size))
		return false;

	for( int i= 0; i < 256; ++i )
		glyphs[i] = createGlyph(face,i);

	return true;
}

/////////////////////////////////////////////////////////////////////////////

FTGlyph* TTFont::getGlyph( int i ) const
{
	if( glyphs == 0 )
		return 0;

	if( i < 0 || i > 255 )
		return 0;

	return glyphs[i];
}

/////////////////////////////////////////////////////////////////////////////

int TTFont::getHeight() const
{
	if( face == 0 )
		return 0;

	return -1; //face->getHeight();
}

/////////////////////////////////////////////////////////////////////////////

int TTFont::getDescender() const
{
	if( face == 0 )
		return 0;

	return -1; //face->getDescender();
}

/////////////////////////////////////////////////////////////////////////////

int TTFont::getWidth( const char* text )
{
	if( text == 0 || glyphs == 0 )
		return 0;

	int w = 0;
	for(;;)
	{
		int ch = (unsigned char) *(text++);
		if( ch == 0 )
			break;

		if( glyphs[ch] == 0 )
			continue;

		w += glyphs[ch]->getAdvance();
	}

	return w;
}
