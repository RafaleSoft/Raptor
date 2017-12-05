
#ifndef __FTGlyph_h
#define __FTGlyph_h

#include <ft2build.h>
#include <freetype.h>

class FTInstance;
class FTFace;

/////////////////////////////////////////////////////////////////////////////

class FTGlyph
{
public:
	FTGlyph(FTFace* _face,int _ascii_code)
		:face(_face),ascii_code(_ascii_code),advance(0),m_width(0),m_height(0)
	{ }


	virtual ~FTGlyph() {};

	//! Glyph factory interface
	virtual bool create( ) = 0;

	//! Return the ascii code of this glyph
	int getAsciiCode() const { return ascii_code; }

	//! Return the distance leftmost and righmost positions
	//! of the bounding box of this glyph
	//! (value is in  1 / 64th of pixels).
	int getAdvance() const { return advance; };

	//! Return the widht of this glyph
	unsigned int getWidth() const { return m_width; }

	//! Return the height of this glyph
	unsigned int getHeight() const { return m_height; }

	//! Return the format of the glyph: outline or bitmap
	FT_Glyph_Format getFormat() const { return format; };


protected:
	FTFace*	face;
	int		ascii_code;
	int		advance;
	unsigned int m_width;
	unsigned int m_height;

	FT_Glyph_Metrics	metrics;
	FT_Glyph_Format		format;
};

/////////////////////////////////////////////////////////////////////////////

#endif // ifndef __FTGlyph_h
