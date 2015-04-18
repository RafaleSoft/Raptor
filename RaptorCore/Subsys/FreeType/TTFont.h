
#ifndef __TTFont_h
#define __TTFont_h

class FTFace;
class FTInstance;
class FTGlyph;

/////////////////////////////////////////////////////////////////////////////

class TTFont
{
public:
	TTFont(  );

	virtual ~TTFont();

	virtual void destroy();

	bool create(FTFace* face, int point_size );

	FTGlyph* getGlyph( int i ) const;

	int getHeight() const;

	int getDescender() const;

	virtual int getWidth( const char* text );

protected:
	FTFace*	face;

private:
	//! Forbidden constructor
	TTFont(const TTFont& );

	//! Forbidden assignment operator
	TTFont& operator=(const TTFont& );

	//! Glyph factory interface
	virtual FTGlyph* createGlyph(FTFace* face,int ascii_code) = 0;

	FTGlyph** glyphs;
};

/////////////////////////////////////////////////////////////////////////////

#endif // ifndef __TTFont_h
