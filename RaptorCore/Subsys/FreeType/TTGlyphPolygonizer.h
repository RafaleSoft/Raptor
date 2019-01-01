#ifndef __TTGlyphPolygonizer_h
#define __TTGlyphPolygonizer_h


class FTGlyphVectorizer;
class TTGlyphPolygonizerHandler;

/////////////////////////////////////////////////////////////////////////////

class TTGlyphPolygonizer
{
public:
	static bool polygonize( TTGlyphPolygonizerHandler* handler,
							const FTGlyphVectorizer& vectorizer);

private:
	TTGlyphPolygonizer( );
	virtual ~TTGlyphPolygonizer();
};

/////////////////////////////////////////////////////////////////////////////

#endif // ifndef __TTGlyphPolygonizer_h
