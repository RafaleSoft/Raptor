
#ifndef __TTOutlineFont_h
#define __TTOutlineFont_h

#include "TTFont.h"

class FTFace;
class FTGlyphOutline;

/////////////////////////////////////////////////////////////////////////////

class TTOutlineFont : public TTFont
{
public:
	TTOutlineFont( );

	virtual ~TTOutlineFont();

	FTGlyphOutline* getOutline( int ascii_code );

private:
	//!	Forbidden constructor
	TTOutlineFont(const TTOutlineFont&);

	//!	Forbidden assignment operator
	TTOutlineFont& operator=(const TTOutlineFont&);

	//!	Outline glyph factory
	virtual FTGlyph* createGlyph(FTFace* face,int ascii_code);
};

/////////////////////////////////////////////////////////////////////////////

#endif // ifndef __TTOutlineFont_h
