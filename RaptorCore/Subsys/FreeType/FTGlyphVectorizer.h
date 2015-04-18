#ifndef __FTGlyphVectorizer_h
#define __FTGlyphVectorizer_h



#include <ft2build.h>
#include <freetype.h>

#include "FTGlyphOutline.h"

/////////////////////////////////////////////////////////////////////////////

class FTGlyphVectorizer
{
public:
	struct POINT
	{
		double x, y;
	};

	class Contour
    {
	public:
		Contour()
			:clockwise(false),
			nPoints(0),points(0),
			max_points(0),area(0.),
			x_min(1e20),y_min(1e20),x_max(-1e20),y_max(-1e20)
		{ }
		
		~Contour()
		{
			delete [] points;
			points= 0;
			nPoints= 0;
		}

		bool exterior() const { return clockwise; }
		int getNPoints() const { return nPoints; }
		double getArea() const { return area; }
		const POINT& getPoint(unsigned int p) const { return points[p]; }
		void add_point(FTGlyphOutline::FTContourPoint point);

	public:
		double area;
		bool clockwise;
		double x_min, x_max;
		double y_min, y_max;
		POINT* points;

	private:
		int max_points;
		int nPoints;
	};

public:
	FTGlyphVectorizer();

	virtual ~FTGlyphVectorizer();

	void destroy();

	void setPrecision( double _precision );

	bool vectorize(FTGlyphOutline* _glyph);

	unsigned int getNContours() const { return nContours; }

	const Contour& getContour( unsigned int c ) const;

	double getAdvance() const { return advance; };

protected:
	// needed distance between two bezier curve evaluations (in pixels)
	double precision;
	double advance;

	Contour** contours;
	unsigned int nContours;

private:
	bool vectorizeContour(unsigned int c, FTGlyphOutline* _glyph);
	void add_point(int c, double x, double y);
	void bezier(Contour &contour,double x1,double y1,double x2,double y2,double x3,double y3);
	void sortContours();
};

/////////////////////////////////////////////////////////////////////////////

#endif // ifndef __FTGlyphVectorizer_h
