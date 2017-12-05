
#ifndef __FTGlyphOutline_h
#define __FTGlyphOutline_h

#include "FTGlyph.h"

/////////////////////////////////////////////////////////////////////////////

class FTGlyphOutline : public FTGlyph
{
public:
	typedef struct FTContourPoint_t
	{
		double x;
		double y;
		bool isOnContour;
	} FTContourPoint;

	FTGlyphOutline(FTFace* _instance,int ascii_code);

	virtual ~FTGlyphOutline();

	unsigned int getNbContours() const { return m_nContour; };

	int getContourFirstIndex(unsigned int nContour);

	int getContourLastIndex(unsigned int nContour);

	FTContourPoint getContourPoint(unsigned int nPoint);


private:
	//! Forbidden constructor
	FTGlyphOutline();

	//! Forbidden constructor
	FTGlyphOutline(const FTGlyphOutline&);

	//! Forbidden assignment operator
	FTGlyphOutline& operator=(const FTGlyphOutline&);

	//! Implements Glyph factory
	virtual bool create();

	unsigned int m_nContour;
	unsigned int m_nPoints;
	FTContourPoint* contours;
	short* indexes;
};

/////////////////////////////////////////////////////////////////////////////

#endif // ifndef __FTGlyphOutline_h
