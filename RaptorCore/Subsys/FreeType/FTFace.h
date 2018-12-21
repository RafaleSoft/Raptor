
#ifndef __FTFace_h
#define __FTFace_h

#include <ft2build.h>
#include FT_FREETYPE_H

/////////////////////////////////////////////////////////////////////////////

class FTFace
{
public:
	FTFace( );

	virtual ~FTFace();

	bool open( const char* filename );

	int CP_Translate( int code ) const;

	FT_Face getFace() const { return face; }

	bool setResolutions( int x_resolution, int y_resolution ) const;
	bool setSizes( unsigned int width, unsigned int height) const;
	bool selectSize( unsigned int size_index) const;

	int getGlyphIndex( int ascii_code ) const
    {
		if( ascii_code < 0 || ascii_code > 255 )
			ascii_code= 0;
		return indices[ascii_code];
    }


private:
	bool makeIndicesTable();

	FT_Face face;
	int indices[256];
};

/////////////////////////////////////////////////////////////////////////////

#endif // ifndef __FTFace_h
