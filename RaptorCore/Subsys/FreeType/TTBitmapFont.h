
#ifndef __TTBitmapFont_h
#define __TTBitmapFont_h

#include "TTFont.h"

class FTFace;
class FTGlyphBitmap;

/////////////////////////////////////////////////////////////////////////////

class TTBitmapFont : public TTFont
{
public:
	TTBitmapFont( );

	virtual ~TTBitmapFont();

	FTGlyphBitmap* getBitmap( int ascii_code ) const;

private:
	//!	Forbidden constructor
	TTBitmapFont(const TTBitmapFont&);

	//!	Forbidden assignment operator
	TTBitmapFont& operator=(const TTBitmapFont&);

	//!	Bitmap glyph factory
	virtual FTGlyph* createGlyph(FTFace* face,int ascii_code);
};

/////////////////////////////////////////////////////////////////////////////

#endif // ifndef __TTBitmapFont_h
