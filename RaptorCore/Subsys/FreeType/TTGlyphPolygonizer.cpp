
#include <stdio.h>

#include "FTGlyph.h"
#include "FTGlyphVectorizer.h"
#include "TTGlyphPolygonizer.h"
#include "TTGlyphPolygonizerHandler.h"

#include <windows.h>

#include <GL/gl.h>
#include <GL/glu.h>


/////////////////////////////////////////////////////////////////////////////
static TTGlyphPolygonizerHandler* handler= 0;

typedef void (CALLBACK *glu_tess_callback)();

static void CALLBACK gltt_polygonizer_begin( GLenum type )
{
	if( handler != 0 )
		handler->begin(int(type));
}

static void CALLBACK gltt_polygonizer_vertex( void* data )
{
	if( handler != 0 )
		handler->vertex( *((FTGlyphVectorizer::POINT*) data) );
}

static void CALLBACK gltt_polygonizer_end()
{
	if( handler != 0 )
		handler->end();
}

static void CALLBACK gltt_polygonizer_combine(	GLdouble coords[3],
												void * vertex_data[4],
												GLfloat weight[4],
												void **outData)
{
	FTGlyphVectorizer::POINT *newVertex = new FTGlyphVectorizer::POINT;
    newVertex->x = coords[0];
    newVertex->y = coords[1];

    *outData = newVertex;

	if( handler != 0 )
		handler->combine( *newVertex );
}

static void CALLBACK gltt_polygonizer_error( GLenum error )
{
	if( handler != 0 )
		handler->error(int(error));
}

/////////////////////////////////////////////////////////////////////////////

bool TTGlyphPolygonizer::polygonize(TTGlyphPolygonizerHandler* poly_handler,
									const FTGlyphVectorizer& vectorizer)
{
	if (0 == poly_handler)
		return false;

	GLUtriangulatorObj* tobj = gluNewTess();
	if( tobj == 0 )
		return false;

	::handler = poly_handler;

	gluTessCallback( tobj, GLU_BEGIN, (glu_tess_callback)gltt_polygonizer_begin );
	gluTessCallback( tobj, GLU_VERTEX, (glu_tess_callback)gltt_polygonizer_vertex );
	gluTessCallback( tobj, GLU_END, (glu_tess_callback)gltt_polygonizer_end );
	gluTessCallback( tobj, GLU_TESS_COMBINE, (glu_tess_callback)gltt_polygonizer_combine );
	gluTessCallback( tobj, GLU_ERROR, (glu_tess_callback)gltt_polygonizer_error );

	int nContours = vectorizer.getNContours();
	bool in_polygon = false;
	bool first_contour = false;

	for( int j= 0; j < nContours; ++j )
    {
		const FTGlyphVectorizer::Contour& contour= vectorizer.getContour(j);
		if( contour.exterior() )
		{
			if( in_polygon )
				gluEndPolygon(tobj);

			gluBeginPolygon(tobj);
			in_polygon = true;
			first_contour = true;
		}

		if( ! in_polygon )
			continue;

		if( first_contour )
		{
			gluNextContour( tobj, GLenum(GLU_EXTERIOR) );
			first_contour= false;
		}
		else
			gluNextContour( tobj, GLenum(GLU_INTERIOR) );

		int nPoints= contour.getNPoints();
		for( int k=nPoints-1; k >= 0; --k )
		{
			const FTGlyphVectorizer::POINT& p = contour.getPoint(k);

			GLdouble data[3];
			data[0] = p.x;
			data[1] = p.y;
			data[2] = 0.;
			gluTessVertex( tobj, data, (void*) &(contour.getPoint(k)) );
		}
	}

	if( in_polygon )
		gluEndPolygon(tobj);

	gluDeleteTess(tobj);

	::handler= 0;

	return true;
}
