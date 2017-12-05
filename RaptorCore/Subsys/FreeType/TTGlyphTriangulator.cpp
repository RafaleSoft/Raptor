
#include "TTGlyphTriangulator.h"

#include "FTGlyphVectorizer.h"
#include "TTGlyphPolygonizer.h"

  #include <windows.h>
  #include <GL/gl.h>
  #include <GL/glu.h>

/////////////////////////////////////////////////////////////////////////////

TTGlyphTriangulator::TTGlyphTriangulator()
	:TTGlyphPolygonizerHandler(),
	triangles(0),
    nTriangles(0),
	vertex_index(0),
	count_them(true)
{
	triangles = new Triangle [1];
}

/////////////////////////////////////////////////////////////////////////////

TTGlyphTriangulator::~TTGlyphTriangulator()
{
    if (0 != triangles)
	{
        delete [] triangles;
		triangles= 0;
	}
}

/////////////////////////////////////////////////////////////////////////////

bool TTGlyphTriangulator::triangulate(const FTGlyphVectorizer& vectorizer)
{
	// First pass : count them only !
	count_them = true;
	vertex_index = 0;

	nTriangles = 0;
	if (!TTGlyphPolygonizer::polygonize(this,vectorizer))
		return false;

	// Second pass : contruct triangles !
	count_them = false;
	vertex_index = 0;

	if (triangles != 0)
		delete [] triangles;
	triangles = new Triangle [ nTriangles + 1 ];

	nTriangles = 0;
	return TTGlyphPolygonizer::polygonize(this,vectorizer);
}

/////////////////////////////////////////////////////////////////////////////

void TTGlyphTriangulator::error(int)
{
}

/////////////////////////////////////////////////////////////////////////////

void TTGlyphTriangulator::begin(int _type)
{
	type = _type;
}

/////////////////////////////////////////////////////////////////////////////

void TTGlyphTriangulator::vertex( const FTGlyphVectorizer::POINT& point )
{
	if (type == 0)
		return;

	if (vertex_index++ < 2)
	{
		vertices[vertex_index-1] = point;
		return;
	}

	if( count_them )
	{
		++nTriangles;
	}
	else
	{
		triangles[nTriangles].p1 = vertices[0];
		triangles[nTriangles].p2 = vertices[1];
		triangles[nTriangles].p3 = point;
		++nTriangles;
	}

	switch( type )
	{
		case GL_TRIANGLE_STRIP:
		{
			if( vertex_index & 1 )
				vertices[0] = point;
			else
				vertices[1] = point;
			break;
		}
		case GL_TRIANGLE_FAN:
		{
			vertices[1] = point;
			break;
		}
		case GL_TRIANGLES:
		default:
		{
			vertex_index = 0;
			break;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////

void TTGlyphTriangulator::combine( const FTGlyphVectorizer::POINT& point )
{
}

/////////////////////////////////////////////////////////////////////////////

void TTGlyphTriangulator::end()
{
	type = 0;
	vertex_index = 0;
}

/////////////////////////////////////////////////////////////////////////////
