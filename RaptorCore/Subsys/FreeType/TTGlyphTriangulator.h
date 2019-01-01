
#ifndef __TTGlyphTriangulator_h
#define __TTGlyphTriangulator_h


#ifndef __TTGlyphPolygonizerHandler_h
#include "TTGlyphPolygonizerHandler.h"
#endif

#ifndef __FTGlyphVectorizer_h
#include "FTGlyphVectorizer.h"
#endif

/////////////////////////////////////////////////////////////////////////////

class TTGlyphTriangulator: public TTGlyphPolygonizerHandler
{
public:
	typedef struct Triangle
	{
		FTGlyphVectorizer::POINT p1;
		FTGlyphVectorizer::POINT p2;
		FTGlyphVectorizer::POINT p3;
	};

	TTGlyphTriangulator();

	virtual ~TTGlyphTriangulator();

	unsigned int getNbTriangles(void) const { return nTriangles; };

	const Triangle& getTriangle(unsigned int i) const
	{
		if (i < nTriangles)
			return triangles[i];
		else
			return triangles[0];
	}

	bool triangulate(const FTGlyphVectorizer& vectorizer);


private:
	//!	Implementes polygonizer handler
	virtual void begin( int _type );

	//!	Implementes polygonizer handler
	virtual void vertex( const FTGlyphVectorizer::POINT& point );

	//!	Implementes polygonizer handler
	virtual void combine( const FTGlyphVectorizer::POINT& point );

	//!	Implementes polygonizer handler
	virtual void end();

	//!	Implementes polygonizer handler
	virtual void error(int);

	bool		 count_them;
	unsigned int nTriangles;
	int			 type;
	int			 vertex_index;
	FTGlyphVectorizer::POINT vertices[2];
	Triangle*			triangles;
};

/////////////////////////////////////////////////////////////////////////////

#endif // ifndef __TTGlyphTriangulator_h
