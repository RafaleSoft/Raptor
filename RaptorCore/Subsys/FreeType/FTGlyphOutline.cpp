
#include <string.h>

#include "FTGlyphOutline.h"
#include "FTGlyph.h"
#include "FTFace.h"

/////////////////////////////////////////////////////////////////////////////

FTGlyphOutline::FTGlyphOutline(FTFace* _instance,int ascii_code)
	:FTGlyph(_instance,ascii_code),m_nContour(0),m_nPoints(0),
	contours(NULL),indexes(NULL)
{
}

/////////////////////////////////////////////////////////////////////////////

FTGlyphOutline::~FTGlyphOutline()
{
	if (0 != contours)
		delete[] contours;
	if (0 != indexes)
		delete [] indexes;
}

/////////////////////////////////////////////////////////////////////////////

bool FTGlyphOutline::create()
{
	if( face == 0 )
		return false;

	int glyph_index= face->getGlyphIndex(ascii_code);
	FT_Face _face = face->getFace();
	FT_Error error = FT_Load_Glyph(	_face,			/* handle to face object */
									glyph_index,    /* glyph index           */
									FT_LOAD_DEFAULT );
	if (0 == error)
	{
		FT_GlyphSlot _glyph = _face->glyph;
		metrics = _glyph->metrics;
		format = _glyph->format;
		advance = metrics.horiAdvance >> 6;

		if (FT_GLYPH_FORMAT_OUTLINE == _glyph->format)
		{
			FT_Outline outline = _glyph->outline;
			m_nContour = outline.n_contours;
			m_nPoints = outline.n_points;

			const double scale= 1. / 64.;
			if (0 != contours)
				delete[] contours;
			contours = new FTContourPoint[m_nPoints];
			
			float	miny = 1e20f;
			float	maxy = -1e20f;
			float	minx = 1e20f;
			float	maxx = -1e20f;

			for (unsigned int i=0;i<m_nPoints;i++)
			{
				FTContourPoint &p = contours[i];
				p.x = outline.points[i].x * scale;
				p.y = outline.points[i].y * scale;
				p.isOnContour = (0 != (outline.tags[i] & 1));

				if (p.x < minx) minx = (float)p.x;
				if (p.x > maxx) maxx = (float)p.x;
				if (p.y < miny) miny = (float)p.y;
				if (p.y > maxy) maxy = (float)p.y;
			}

			//! Not very happy with this : need proper rounding :-(
			m_width = (unsigned int)(maxx - minx + 0.5f);
			m_height = (unsigned int)(maxy - miny + 0.5f);

			if (0 != indexes)
				delete[] indexes;
			indexes = new short[m_nContour];
			for (unsigned int i=0;i<m_nContour;i++)
				indexes[i] = outline.contours[i];
		}
		else
			return false;
	}
	else
		return false;
	
	return true;
}

/////////////////////////////////////////////////////////////////////////////

int FTGlyphOutline::getContourFirstIndex(unsigned int nContour)
{
	if (nContour < m_nContour)
		return (nContour == 0) ? 0 : indexes[nContour-1] + 1;
	else
		return -1;
}

/////////////////////////////////////////////////////////////////////////////

int FTGlyphOutline::getContourLastIndex(unsigned int nContour)
{
	if (nContour < m_nContour)
		return indexes[nContour];
	else
		return -1;
}

/////////////////////////////////////////////////////////////////////////////

FTGlyphOutline::FTContourPoint FTGlyphOutline::getContourPoint(unsigned int nPoint)
{
	FTContourPoint res = {0,0,false};

	if (nPoint < m_nPoints)
		res = contours[nPoint];

	return res;
}

