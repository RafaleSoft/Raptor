#ifndef __TTGlyphPolygonizerHandler_h
#define __TTGlyphPolygonizerHandler_h

#ifndef __FTGlyphVectorizer_h
#include "FTGlyphVectorizer.h"
#endif


/////////////////////////////////////////////////////////////////////////////

class TTGlyphPolygonizerHandler
{
public:
	TTGlyphPolygonizerHandler() {};

	virtual ~TTGlyphPolygonizerHandler() {};

	virtual void begin( int type ) = 0;
	virtual void vertex( const FTGlyphVectorizer::POINT& point ) = 0;
	virtual void combine( const FTGlyphVectorizer::POINT& point ) = 0;
	virtual void end() = 0;
	virtual void error( int error ) = 0;
};

/////////////////////////////////////////////////////////////////////////////

#endif // ifndef __TTGlyphPolygonizerHandler_h
