#include <math.h>
#include <string.h>

#include "FTGlyphVectorizer.h"
#include "FTGlyphOutline.h"


#include <algorithm>
using namespace std;
/////////////////////////////////////////////////////////////////////////////

static FTGlyphVectorizer::Contour emptyContour;

// private
void FTGlyphVectorizer::Contour::add_point(FTGlyphOutline::FTContourPoint point)
{
	if( points == 0 )
	{
		max_points = 8;
		points = new POINT[ max_points ];
		nPoints = 0;
	}
	else
	{
		if( nPoints >= max_points )
		{
			int new_max_points = max_points + max_points / 2;
			POINT* new_points = new POINT [ new_max_points ];
			memcpy( (void*) new_points, (void*) points, nPoints * sizeof(points[0]) );
			delete [] points;
			points = new_points;
			max_points = new_max_points;
		}
	}

	register POINT* p = points + nPoints;

	if( nPoints > 0 )
	{
		POINT* prev= p - 1;
		if( fabs(prev->x - point.x) < 1e-8 && fabs(prev->y - point.y) < 1e-8 )
			return; // coincident vertices
	}

	p->x = point.x;
	p->y = point.y;

	if( point.x < x_min ) x_min = point.x;
	if( point.x > x_max ) x_max = point.x;
	if( point.y < y_min ) y_min = point.y;
	if( point.y > y_max ) y_max = point.y;

	if( nPoints > 0 )
	{
		--p;
		area += p->x*point.y - point.x*p->y;
	}

	++nPoints;
}

/////////////////////////////////////////////////////////////////////////////

FTGlyphVectorizer::FTGlyphVectorizer()
	:precision(0.0), advance(0.0), contours(0),nContours(0)
{
}

/////////////////////////////////////////////////////////////////////////////

FTGlyphVectorizer::~FTGlyphVectorizer()
{
	destroy();
}

/////////////////////////////////////////////////////////////////////////////

void FTGlyphVectorizer::destroy()
{
	if( contours != 0 )
	{
		for( unsigned int i= 0; i < nContours; ++i )
			delete contours[i];

		delete [] contours;
		contours = 0;
	}

	nContours = 0;
}

/////////////////////////////////////////////////////////////////////////////

void FTGlyphVectorizer::setPrecision(double _precision)
{
	precision = std::max( _precision, 0.01 );
}

/////////////////////////////////////////////////////////////////////////////

const FTGlyphVectorizer::Contour& FTGlyphVectorizer::getContour( unsigned int c ) const
{
	if ( c < nContours)
		return *contours[c];
	else
		return emptyContour;
}

/////////////////////////////////////////////////////////////////////////////

bool FTGlyphVectorizer::vectorize(FTGlyphOutline* glyph)
{
	if( glyph == 0 )
		return false;

	destroy();
	
	nContours = glyph->getNbContours();
	if (nContours > 0)
	{
		contours = new Contour*[ nContours ];
		for( unsigned int i = 0; i < nContours; ++i )
			contours[i] = new Contour;
	}
	else
		return false;

	advance = double(glyph->getAdvance());
	for( unsigned int i= 0; i < nContours; ++i )
	{
		if( ! vectorizeContour(i,glyph) )
			return false;
	}

	// remove null contours now
	for( unsigned int i = 0; i < nContours; ++i )
	{
		Contour* c = contours[i];
		if( c->getNPoints() < 2 )
		{
			delete c;

			for( unsigned int j= i+1; j < nContours; ++j )
				contours[j-1] = contours[j];

			contours[nContours-1]= 0;

			--nContours;
			--i;
		}
	}

	sortContours();
	return true;
}

/////////////////////////////////////////////////////////////////////////////

// 0 <= c < nContours
bool FTGlyphVectorizer::vectorizeContour(unsigned int c, FTGlyphOutline* glyph)
{
	if( (c >= nContours) || (0 == contours) )
		return false;

	Contour& contour = *contours[c];

	int first = glyph->getContourFirstIndex(c);
	int last = glyph->getContourLastIndex(c);

	int k1 = first;
	int k2 = k1+1;

	FTGlyphOutline::FTContourPoint point1 = glyph->getContourPoint(k1);
	FTGlyphOutline::FTContourPoint point2 = glyph->getContourPoint(k2);
	int skip_next= 0;

	for( int k=first+1; k <= last; ++k )
	{
		int k3 = (k==last) ? first : (k+1);
		FTGlyphOutline::FTContourPoint point3 = glyph->getContourPoint(k3);
		if(!skip_next)
		{
			if (point1.isOnContour)
			{
				if (point2.isOnContour)
				{
					contour.add_point(point1);
					if( k == last )
						contour.add_point(point2);
				}
				else
				{
					if (point3.isOnContour)
					{
						bezier(contour,point1.x,point1.y,point2.x,point2.y,point3.x,point3.y);
						if( k == last-1 )
							contour.add_point(point3);
						skip_next= 1;
					}
					else
					{
						double x23= (point2.x+point3.x) * .5;
						double y23= (point2.y+point3.y) * .5;
						bezier(contour,point1.x,point1.y,point2.x,point2.y,x23,y23);
					}
				}
			}
			else
			{
				if (point2.isOnContour)
				{
				}
				else
				{
					if (point3.isOnContour)
					{
						double x12= (point1.x+point2.x) * .5;
						double y12= (point1.y+point2.y) * .5;
						bezier(contour,x12,y12,point2.x,point2.y,point3.x,point3.y);
						if( k == last-1 )
							contour.add_point(point3);
						skip_next= 1;
					}
					else
					{
						double x12= (point1.x+point2.x) * .5;
						double y12= (point1.y+point2.y) * .5;
						double x23= (point2.x+point3.x) * .5;
						double y23= (point2.y+point3.y) * .5;
						bezier(contour,x12,y12,point2.x,point2.y,x23,y23);
					}
				}
			}
		}

		k1 = k2; k2 = k3;
		point1 = point2;
		point2 = point3;
		skip_next= 0;
	}

	if( contour.getNPoints() >= 2 )
	{
		contour.area += contour.getPoint(contour.getNPoints()-1).x
						*contour.getPoint(0                ).y
						- contour.getPoint(0                ).x
						*contour.getPoint(contour.getNPoints()-1).y;
		contour.area *= .5;
	}

	contour.clockwise = contour.getArea() < 0.;

	return true;
}

/////////////////////////////////////////////////////////////////////////////

void FTGlyphVectorizer::bezier(	Contour &contour,
								double x1,double y1,
								double x2,double y2,
								double x3,double y3)
{
	double extent = max( fabs(max(x1,max(x2,x3)) - min(x1,min(x2,x3))),
						fabs(max(y1,max(y2,y3)) - min(y1,min(y2,y3))) );
	double _subdivisions = extent / precision + .5;
	int subdivisions = int(_subdivisions);
	FTGlyphOutline::FTContourPoint p = { x1, y1, false };
	contour.add_point(p);
	if( subdivisions > 0 )
	{
		double dt = 1. / double(subdivisions);
		register double t = dt;
		for( int a = 1; a < subdivisions; ++a, t += dt )
		{
			register double tt = 1. - t;
			register double t1 = tt * tt;
			register double t2 = 2. * t * tt;
			register double t3 = t * t;
			register double x = t1 * (x1) + t2 * (x2) + t3 * (x3);
			register double y = t1 * (y1) + t2 * (y2) + t3 * (y3);
			FTGlyphOutline::FTContourPoint p2 = { x, y, false };
			contour.add_point(p2);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////

// What we'd like to have in contour[] is:
//  + first exterior contour
//  +   its interior contours
//  + next exterior contour
//  +   its interior contours
//  etc.
void FTGlyphVectorizer::sortContours()
{
	if( nContours == 0 )
		return;

	Contour** scontours= new Contour* [ nContours ];
	unsigned int nSContours= 0;

	for(;;)
	{
		// first, get the first exterior contour
		int iext= -1;
		for( unsigned int i= 0; i < nContours; ++i )
		{
			if( contours[i] == 0 )
				continue;
			if( contours[i]->exterior() )
			{
				iext = i;
				break;
			}
		}

		if( iext == -1 )
			break; // no more exterior contour

		Contour* ext= contours[iext];
		scontours[nSContours++]= ext;
		contours[iext]= 0; // remove it from the list

		// let's find its children interior contours
		for( unsigned int i= 0; i < nContours; ++i )
		{
			Contour* inte= contours[i];
			if( inte == 0 )
				continue;
			if( inte->exterior() )
				continue;

			// ok, we have an interior contour in *inte
			// Is it _into_ the *ext contour?

			// Check bounding boxes: the *inte bbox must be _into_ the
			// *ext bbox
			if( inte->x_min < ext->x_min || inte->x_max > ext->x_max ||
				inte->y_min < ext->y_min || inte->y_max > ext->y_max )
				continue;

			// ok, let's take the first point of *inte
			double x= inte->getPoint(0).x;
			double y= inte->getPoint(0).y;

			// now, count how many times the half line (-inf -> x, y)
			// intersects the *ext countour
			double x1= ext->getPoint(0).x;
			double y1= ext->getPoint(0).y;
			register double x2= 0.;
			register double y2= 0.;
			int nIntersections= 0;
			for( int j= 0; j < ext->getNPoints(); ++j, x1= x2, y1= y2 )
			{
				register int j2= j + 1;
				if( j2 == ext->getNPoints() )
					j2= 0;
				x2= ext->getPoint(j2).x;
				y2= ext->getPoint(j2).y;
				if( (y1 > y && y2 > y) || (y1 < y && y2 < y) )
					continue;
				if( y1 == y2 )
				{
					if( y1 == y && (x1 < x || x2 < x) )
						++nIntersections;
					continue;
				}
				register double Ix= x1 + (y - y1) * (x2 - x1) / (y2 - y1);
				if( Ix <= x )
					++nIntersections;
			}

			int into = (nIntersections & 1) != 0;
			if( ! into )
				continue;

			// ok, this contour is _into_ *ext
			// let's append it after exti
			scontours[nSContours++]= inte;
			contours[i]= 0;
		}
	}

	if( nSContours < nContours )
	{
		for( unsigned int i= 0; i < nContours; ++i )
		{
			Contour* c = contours[i];
			if( c == 0 )
				continue;
			scontours[nSContours++]= c;
			contours[i]= 0;
		}
	}

	delete [] contours;
	contours = scontours;
}

/////////////////////////////////////////////////////////////////////////////
