// GLFont.cpp: implementation of the CGLFont class.
//
//////////////////////////////////////////////////////////////////////
#include "Subsys/CodeGeneration.h"

#ifndef __RAPTOR_GLEXT_H__
	#include "System/Glext.h"
#endif

#if !defined(AFX_GLFONT_H__D451FE62_5FE1_11D3_9142_BA23BC92E77C__INCLUDED_)
	#include "GLFont.h"
#endif
#if !defined(AFX_GEOMETRY_H__B42ABB87_80E8_11D3_97C2_DE5C28000000__INCLUDED_)
	#include "Geometry.h"
#endif
#if !defined(AFX_GEOMETRYEDITOR_H__2D77E428_ED3D_416B_8DE9_DABFD45A38A7__INCLUDED_)
    #include "GeometryEditor.h"
#endif
#if !defined(AFX_3DSET_H__DB24F01B_80B9_11D3_97C1_FC2841000000__INCLUDED_)
	#include "3DSet.h"
#endif
#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
	#include "System/Raptor.h"
#endif
#ifndef __GLOBAL_H__
	#include "System/Global.h"
#endif
#if !defined(AFX_TEXTUREFACTORY_H__1B470EC4_4B68_11D3_9142_9A502CBADC6B__INCLUDED_)
	#include "GLHierarchy/TextureFactory.h"
#endif

#include "Subsys/FreeType/TTFont.h"
#include "Subsys/FreeType/FTFace.h"
#include "Subsys/FreeType/TTBitmapFont.h"
#include "Subsys/FreeType/TTOutlineFont.h"
#include "Subsys/FreeType/FTGlyphBitmap.h"
#include "Subsys/FreeType/FTGlyphOutline.h"
#include "Subsys/FreeType/FTGlyphVectorizer.h"
#include "Subsys/FreeType/TTGlyphTriangulator.h"


#include <math.h>

#include "Subsys/Hershey/Simplex.h"


RAPTOR_NAMESPACE

static CGLFont::CGLFontClassID fontId;
const CPersistence::CPersistenceClassID& CGLFont::CGLFontClassID::GetClassId(void)
{
	return fontId;
}

#define TEXTURE_WIDTH 256

//////////////////////////////////////////////////////////////////////
// Tools non members
//////////////////////////////////////////////////////////////////////
#pragma warning (disable : 4711)	// automatic inline expansion for normalize
void normalize(float v[3],float norm)
{
	float sq = (float)sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
	if (sq > 0 )
	{
		GLfloat d=norm/sq;
		v[0] = v[0] * d;
		v[1] = v[1] * d;
		v[2] = v[2] * d;
	}
}

#define GENVERTEX(x,y,z,tu,tv,mode)\
if (mode)\
{\
	onTexCoord(tu,tv);\
	glTexCoord2f(tu,tv);\
}\
onVertex(x,y,z);\
glVertex3f(x,y,z);

typedef struct glyph_t
{
	GLuint		glList;
	GLfloat		halfWidth;
	GLfloat		height;
	GLfloat		advance;
} glyph;

// glyph settings
typedef struct glyphSettings_t
{
	float		precision;
	float		extrusion;
	float		scale;
} glyphSettings;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CGLFont::CGLFont(const std::string& name)
	:CPersistence(fontId,name),
	m_face(NULL),m_font(NULL),m_bmfont(NULL),m_texture(NULL),
	m_fontFileName(""),m_size(0),
	m_bTexturedFace(false),m_bTexturedSide(false),m_bVectorized(false),m_bTextured(false)
{
}

bool CGLFont::create2D(const std::string &name, unsigned int size)
{
	if ((m_face != NULL) && (name != m_fontFileName))
	{
		delete m_face;
		m_face = NULL;

		if (m_bmfont != NULL)
		{
			delete m_bmfont;
			m_bmfont = NULL;
		}
	}

	if (m_face == NULL)
	{
		m_face=new FTFace;
		m_fontFileName = name;
		if(!m_face->open(name.data()))
		{
			delete m_face;
			m_face = NULL;
#ifdef RAPTOR_DEBUG_MODE_GENERATION
            Raptor::GetErrorManager()->generateRaptorError(	CGLFont::CGLFontClassID::GetClassId(),
                                                            CRaptorErrorManager::RAPTOR_ERROR,
															CRaptorMessages::ID_SUBSYS_FAILED);
#endif
			return false;
		}
	}

	if ((m_bmfont != NULL) && (size != m_size))
	{
		delete m_bmfont;
		m_bmfont = NULL;
	}

	if (m_bmfont == NULL)
	{
		m_bmfont = new TTBitmapFont;
		if(! m_bmfont->create(m_face,size))
		{
			delete m_bmfont;
			m_bmfont = NULL;
			return false;
		}
	}

	return true;
}

bool CGLFont::create3D(const std::string &name, unsigned int size,bool faceTextured,bool sideTextured)
{
	if ((m_face != NULL) && (name != m_fontFileName))
	{
		delete m_face;
		m_face = NULL;

		if (m_font != NULL)
		{
			delete m_font;
			m_font = NULL;
		}
	}

	if (m_face == NULL)
	{
		m_face=new FTFace;
		m_fontFileName = name;
		if(!m_face->open(name.data()))
		{
			delete m_face;
			m_face = NULL;
#ifdef RAPTOR_DEBUG_MODE_GENERATION
            Raptor::GetErrorManager()->generateRaptorError(	CGLFont::CGLFontClassID::GetClassId(),
                                                            CRaptorErrorManager::RAPTOR_ERROR,
															CRaptorMessages::ID_SUBSYS_FAILED);
#endif
			return false;
		}
	}

	if ((m_font != NULL) && (size != m_size))
	{
		delete m_font;
		m_font = NULL;
	}

	if (m_font == NULL)
	{
		m_font=new TTOutlineFont;
		if(! m_font->create(m_face,size))
		{
			delete m_font;
			m_font = NULL;
	#ifdef RAPTOR_DEBUG_MODE_GENERATION
    		Raptor::GetErrorManager()->generateRaptorError(	CGLFont::CGLFontClassID::GetClassId(),
															CRaptorErrorManager::RAPTOR_ERROR,
															CRaptorMessages::ID_CREATE_FAILED);
	#endif
			return false;
		}
	}

	m_bTexturedFace = faceTextured;
	m_bTexturedSide = sideTextured;

	return true;
}

bool CGLFont::createVector(const std::string &name, unsigned int size)
{
	m_bVectorized = true;

	if (name.empty())
	{
		return true;
	}
	else	/*	TODO	*/
		return false;
}


CGLFont::~CGLFont()
{
	if (m_bmfont != NULL)
		delete m_bmfont;
	if (m_font != NULL)
		delete m_font;
	if (m_face != NULL)
		delete m_face;

	vector<void*>::iterator itr = m_glfontglyph.begin();
	while (itr != m_glfontglyph.end())
	{
		glyph *g = (glyph*)(*itr);
		for (int i=0;i<256;i++)
			glDeleteLists(g[i].glList,1);
		itr = m_glfontglyph.erase(itr);
		delete [] g;
	}

	itr = m_glfontglyphsettings.begin();
	while (itr != m_glfontglyphsettings.end())
	{
		glyphSettings *gs = (glyphSettings*)(*itr);
		itr = m_glfontglyphsettings.erase(itr);
		delete gs;
	}

	m_glfontglyph.clear();
	m_glfontglyphsettings.clear();
}


//////////////////////////////////////////////////////////////////////
// Implementation
//////////////////////////////////////////////////////////////////////
float CGLFont::getWidth(const std::string &s, unsigned int numglyphset) const
{
#ifdef RAPTOR_DEBUG_MODE_GENERATION
    if ((m_font == NULL) || (m_face == NULL) ||
	    (numglyphset < 0) || (numglyphset>m_glfontglyph.size()))
    {
			Raptor::GetErrorManager()->generateRaptorError(	CGLFont::CGLFontClassID::GetClassId(),
                                                            CRaptorErrorManager::RAPTOR_ERROR,
															CRaptorMessages::ID_INIT_FAILED);

            return 0.0f;
    }
#endif

	GLfloat w=0;
	glyph	*glyphs = (glyph*)m_glfontglyph.at(numglyphset);

	for (unsigned int i=0;i<s.size();i++)
	{
		glyph g = glyphs[(unsigned char)s[i]];
		w += g.halfWidth;
	}
	return -2*w;
}

float CGLFont::getWidth(const std::string &s) const
{
#ifdef RAPTOR_DEBUG_MODE_GENERATION
    if ((m_bmfont == NULL) ||
	    (m_face == NULL))
    {
    	Raptor::GetErrorManager()->generateRaptorError(	CGLFont::CGLFontClassID::GetClassId(),
                                                        CRaptorErrorManager::RAPTOR_ERROR,
				            	    					CRaptorMessages::ID_INIT_FAILED);
        return 0.0f;
    }
#endif

	return (float)m_bmfont->getWidth(s.data());
}


float CGLFont::getHeight(const std::string &s, unsigned int numglyphset) const
{
#ifdef RAPTOR_DEBUG_MODE_GENERATION
    if ((m_font == NULL) ||
	    (m_face == NULL) ||
	    (numglyphset < 0) ||
        (numglyphset>m_glfontglyph.size()))
    {
			Raptor::GetErrorManager()->generateRaptorError(	CGLFont::CGLFontClassID::GetClassId(),
															CRaptorErrorManager::RAPTOR_ERROR,
															CRaptorMessages::ID_INIT_FAILED);
            return 0.0f;
    }
#endif
    
	GLfloat h=0;
	glyph	g;
	glyph	*glyphs = (glyph*)m_glfontglyph.at(numglyphset);

	for (unsigned int i=0;i<s.size();i++)
	{
		g = glyphs[(unsigned char)s[i]];
		if (g.height<h) 
			h = g.height;
	}
	return -h;
}


float CGLFont::getHeight(const std::string &s) const
{
#ifdef RAPTOR_DEBUG_MODE_GENERATION
    if ((m_bmfont == NULL) ||
	    (m_face == NULL))
    {
    	Raptor::GetErrorManager()->generateRaptorError(	CGLFont::CGLFontClassID::GetClassId(),
														CRaptorErrorManager::RAPTOR_ERROR,
				            	    				    CRaptorMessages::ID_INIT_FAILED);
        return 0.0f;
    }
#endif

	return (float)m_bmfont->getHeight();
}


#pragma warning ( disable:4056)
RAPTOR_HANDLE CGLFont::glTTBuildGeometry(	const std::string &str,
											float precision,
		                                    float extrusion,
		                                    float scale,
		                                    float *width,
		                                    float *height)
{

#ifdef RAPTOR_DEBUG_MODE_GENERATION
    if ((m_font == NULL) ||
	    (m_face == NULL))
    {
			Raptor::GetErrorManager()->generateRaptorError(	CGLFont::CGLFontClassID::GetClassId(),
															CRaptorErrorManager::RAPTOR_ERROR,
															CRaptorMessages::ID_INIT_FAILED);
            RAPTOR_HANDLE rh;
            return rh;
    }
#endif

	GLuint	TTList;
	float	base_x = 0;
	float	miny = 1e20f;
	float	maxy = -1e20f;
	float	minx = 1e20f;
	float	maxx = -1e20f;
	float	v3[3];

	const char* text= str.data();
	const unsigned int text_length= str.size();
	
	FTGlyphVectorizer* vec= new FTGlyphVectorizer [ text_length ];
	TTGlyphTriangulator** tri= new TTGlyphTriangulator* [ text_length ];

	for( unsigned int i = 0; i < text_length; ++i )
	{
		tri[i] = new TTGlyphTriangulator();
		int ch= (unsigned char)text[i];

		vec[i].setPrecision(precision);
		FTGlyphOutline* g = m_font->getOutline(ch);
		if( (0 != g) && (vec[i].vectorize(g)) )
		{
			for(unsigned int c=0; c < vec[i].getNContours(); ++c )
			{
				const FTGlyphVectorizer::Contour& cnt = vec[i].getContour(c);
				if (cnt.y_min<miny)
					miny = (float)cnt.y_min;
				if (cnt.y_max>maxy)
					maxy = (float)cnt.y_max;
				if (cnt.x_min<minx)
					minx = (float)cnt.x_min;
				if (cnt.x_max>maxx)
					maxx = (float)cnt.x_max;
			}

			tri[i]->triangulate(vec[i]);
		}
	}

	TTList=glGenLists(1);
	glNewList(TTList,GL_COMPILE);
	glPushMatrix();

	for( unsigned int i = 0; i < text_length; ++i )
	{
		float x,y,z;
		float tu,tv;

		glBegin(GL_TRIANGLES);
		for( unsigned int j = 0; j < tri[i]->getNbTriangles(); ++j )
		{
			const TTGlyphTriangulator::Triangle& t = tri[i]->getTriangle(j);
		
			glNormal3f(0.0,0.0,1.0);

			x = (float)(t.p1.x+base_x)*scale;
			y = (float)t.p1.y*scale;
			z = extrusion * scale;
			tu = (x - minx)/(maxx-minx);
			tv = (y - miny)/(maxy-miny);
			GENVERTEX(x,y,z,tu,tv,m_bTexturedFace)
			
			x = (float)(t.p2.x+base_x)*scale;
			y = (float)t.p2.y*scale;
			z = extrusion * scale;
			tu = (x - minx)/(maxx-minx);
			tv = (y - miny)/(maxy-miny);
			GENVERTEX(x,y,z,tu,tv,m_bTexturedFace)

			x = (float)(t.p3.x+base_x)*scale;
			y = (float)t.p3.y*scale;
			z = extrusion * scale;
			tu = (x - minx)/(maxx-minx);
			tv = (y - miny)/(maxy-miny);
			GENVERTEX(x,y,z,tu,tv,m_bTexturedFace)

			if (extrusion>0.0)
			{
				glNormal3f(0.0,0.0,-1.0);

				z = 0;
				x = (float)(t.p1.x+base_x)*scale;
				y = (float)t.p1.y*scale;
				tu = -(minx - x)/(maxx-minx);
				tv = (y - miny)/(maxy-miny);
				GENVERTEX(x,y,z,tu,tv,m_bTexturedFace)

				x = (float)(t.p3.x+base_x)*scale;
				y = (float)t.p3.y*scale;
				tu = -(minx - x)/(maxx-minx);
				tv = (y - miny)/(maxy-miny);
				GENVERTEX(x,y,z,tu,tv,m_bTexturedFace)

				x = (float)(t.p2.x+base_x)*scale;
				y = (float)t.p2.y*scale;
				tu = -(minx - x)/(maxx-minx);
				tv = (y - miny)/(maxy-miny);
				GENVERTEX(x,y,z,tu,tv,m_bTexturedFace)
			}
		}
		glEnd();

		if (extrusion>0.0)
		for(unsigned int c = 0; c < vec[i].getNContours(); ++c )
		{
			const FTGlyphVectorizer::Contour& contour= vec[i].getContour(c);
			FTGlyphVectorizer::POINT* points= contour.points;
			if( points == 0 || contour.getNPoints() <= 0 )
				continue;

			v3[2]=0;
			glBegin(GL_QUAD_STRIP);
				float oldx = (float)(points->x +base_x)*scale;
				float oldy = (float)points->y*scale;

				for( int k = 0; k < contour.getNPoints(); ++k,points++)
				{
					if (k<contour.getNPoints()-1)
					{
						v3[0]=(float)points->y-(float)(points+1)->y;
						v3[1]=(float)(points+1)->x-(float)points->x;	
					}
					else
					{
						v3[0]=(float)points->y-(float)contour.points->y;
						v3[1]=(float)contour.points->x-(float)points->x;	
					}
					normalize(v3,1);
					glNormal3fv(v3);

					x = (float)(points->x +base_x)*scale;
					y = (float)points->y*scale;

					float dist = (float)(sqrt((x-oldx)*(x-oldx) + (y-oldy)*(y-oldy)));
					oldx = x;
					oldy = y;

					z = 0;
					tu = 0.0f;
					tv = dist * scale / (maxx-minx);
					float oldtv = tv;
					GENVERTEX(x,y,z,tu,tv,m_bTexturedSide)

					x = oldx;
					y = oldy;
					z = extrusion*scale;
					tu = 1;
					tv = oldtv;
					GENVERTEX(x,y,z,tu,tv,m_bTexturedSide)
				}

				points = contour.points;

				x = (float)(points->x +base_x)*scale;
				y = (float)points->y*scale;
				z = 0;
				tu = 0.0f;
				tv = 1.0f;
				GENVERTEX(x,y,z,tu,tv,m_bTexturedSide)

				x = (float)(points->x +base_x)*scale;
				y = (float)points->y*scale;
				z = extrusion*scale;
				tu = 1.0f;
				tv = 1.0f;
				GENVERTEX(x,y,z,tu,tv,m_bTexturedSide)
			glEnd();
		}
		base_x += (float)vec[i].getAdvance();  
	}
	glPopMatrix();
	glEndList();

	for( unsigned int i = 0; i < text_length; ++i )
		delete tri[i];
	delete [] tri;
	delete [] vec;
	
	if (width != NULL)
		*width = -base_x*0.5f*scale;
	if (height != NULL)
		*height = -(maxy+miny)*0.5f*scale;

	CATCH_GL_ERROR

	RAPTOR_HANDLE result;
	result.handle = TTList;
	result.hClass = CGLFont::CGLFontClassID::GetClassId().ID();

	return result;
}
#pragma warning ( default:4056 )


CGeometry* CGLFont::glBuildGeometry(const std::string &str,
									float precision,
		                            float extrusion,
		                            float scale)
{
#ifdef RAPTOR_DEBUG_MODE_GENERATION
    if ((m_font == NULL) ||
	    (m_face == NULL))
    {
		Raptor::GetErrorManager()->generateRaptorError(	CGLFont::CGLFontClassID::GetClassId(),
														CRaptorErrorManager::RAPTOR_ERROR,
														CRaptorMessages::ID_INIT_FAILED);
		return NULL;
    }
#endif

	CGeometry* geo = new CGeometry("Font Geometry");

	float base_x= 0;
	float miny=1e20f,maxy=-1e20f;
	float minx=1e20f,maxx=-1e20f;
	float v3[3];

	const char* text= str.data();
	const unsigned int text_length = str.size();
	
	FTGlyphVectorizer* vec= new FTGlyphVectorizer [ text_length ];
	TTGlyphTriangulator** tri= new TTGlyphTriangulator* [ text_length ];

	for(unsigned int i = 0; i < text_length; ++i )
	{
		tri[i] = new TTGlyphTriangulator();
		int ch = (unsigned char)text[i];
		FTGlyphOutline* g = m_font->getOutline(ch);

		vec[i].setPrecision(precision);
		if( vec[i].vectorize(g) )
			tri[i]->triangulate(vec[i]);
	}

	unsigned int nbTriangles = 0;
	for( unsigned int i = 0; i < text_length; ++i )
		nbTriangles += tri[i]->getNbTriangles();

	if (extrusion > 0.0f)
	{
		nbTriangles *= 2;

		int nbPoints = 0;
		for (unsigned int i=0;i<text_length;i++)
		{
			for(unsigned int j = 0; j < vec[i].getNContours(); j++ )
			{
				const FTGlyphVectorizer::Contour& cnt = vec[i].getContour(j);
				if (cnt.y_min<miny)
					miny=(float)cnt.y_min;
				if (cnt.y_max>maxy)
					maxy=(float)cnt.y_max;
				if (cnt.x_min<minx)
					minx=(float)cnt.x_min;
				if (cnt.x_max>maxx)
					maxx=(float)cnt.x_max;
				nbPoints += cnt.getNPoints();
			}
		}

		nbTriangles += 2 * nbPoints;
	}

	geo->glSetVertices( 3 * nbTriangles );
	geo->glSetPolygons( nbTriangles );
	if (m_bTexturedFace || m_bTexturedSide)
		geo->glSetTexCoords( 3 * nbTriangles );
	geo->glLockData();

	for( unsigned int i = 0; i < text_length; ++i )
	{
		float x,y,z;
		float tu,tv;

		for( unsigned int j= 0; j < tri[i]->getNbTriangles(); ++j )
		{
			const TTGlyphTriangulator::Triangle& t = tri[i]->getTriangle(j);
		
			z = extrusion * scale;
			x = (float)(t.p1.x+base_x)*scale;
			y = (float)t.p1.y*scale;
			tu = (x - minx)/(maxx-minx);
			tv = (y - miny)/(maxy-miny);

			geo->addVertex(x,y,z,1.0f);
			geo->setTexCoord(geo->nbVertex()-1,tu,tv);
						
			x = (float)(t.p2.x+base_x)*scale;
			y = (float)t.p2.y*scale;
			tu = (x - minx)/(maxx-minx);
			tv = (y - miny)/(maxy-miny);

			geo->addVertex(x,y,z,1.0f);
			geo->setTexCoord(geo->nbVertex()-1,tu,tv);

			x = (float)(t.p3.x+base_x)*scale;
			y = (float)t.p3.y*scale;
			tu = (x - minx)/(maxx-minx);
			tv = (y - miny)/(maxy-miny);

			geo->addVertex(x,y,z,1.0f);
			geo->setTexCoord(geo->nbVertex()-1,tu,tv);

			geo->addFace(geo->nbVertex()-3,geo->nbVertex()-2,geo->nbVertex()-1);

			if (extrusion > 0.0f)
			{
				z = 0;
				x = (float)(t.p1.x+base_x)*scale;
				y = (float)t.p1.y*scale;
				tu = -(minx - x)/(maxx-minx);
				tv = (y - miny)/(maxy-miny);

				geo->addVertex(x,y,z,1.0f);
				geo->setTexCoord(geo->nbVertex()-1,tu,tv);

				x = (float)(t.p3.x+base_x)*scale;
				y = (float)t.p3.y*scale;
				tu = -(minx - x)/(maxx-minx);
				tv = (y - miny)/(maxy-miny);

				geo->addVertex(x,y,z,1.0f);
				geo->setTexCoord(geo->nbVertex()-1,tu,tv);

				x = (float)(t.p2.x+base_x)*scale;
				y = (float)t.p2.y*scale;
				tu = -(minx - x)/(maxx-minx);
				tv = (y - miny)/(maxy-miny);

				geo->addVertex(x,y,z,1.0f);
				geo->setTexCoord(geo->nbVertex()-1,tu,tv);

				geo->addFace(geo->nbVertex()-3,geo->nbVertex()-2,geo->nbVertex()-1);
			}
		}

		
		if (extrusion>0.0)
		for(unsigned int c = 0; c < vec[i].getNContours(); ++c )
		{
			const FTGlyphVectorizer::Contour& contour = vec[i].getContour(c);
			FTGlyphVectorizer::POINT* points= contour.points;
			if( points == 0 || contour.getNPoints() <= 0 )
				continue;

			v3[2]=0;

			float oldx = (float)(points->x +base_x)*scale;
			float oldy = (float)points->y*scale;

			for( int k = 0; k < contour.getNPoints(); ++k,points++)
			{
				x = (float)(points->x +base_x)*scale;
				y = (float)points->y*scale;

				float dist = (float)(sqrt((x-oldx)*(x-oldx) + (y-oldy)*(y-oldy)));
				oldx = x;
				oldy = y;

				z = 0;
				tu = 0.0f;
				tv = dist * scale / (maxx-minx);
				float oldtv = tv;

				geo->addVertex(x,y,z,1.0f);
				geo->setTexCoord(geo->nbVertex()-1,tu,tv);

				x = oldx;
				y = oldy;
				z = extrusion*scale;
				tu = 1;
				tv = oldtv;

				geo->addVertex(x,y,z,1.0f);
				geo->setTexCoord(geo->nbVertex()-1,tu,tv);

				if (k > 0)
				{
					geo->addFace(geo->nbVertex()-4,geo->nbVertex()-3,geo->nbVertex()-1);
					geo->addFace(geo->nbVertex()-1,geo->nbVertex()-2,geo->nbVertex()-4);
				}
			}

			points = contour.points;

			x = (float)(points->x +base_x)*scale;
			y = (float)points->y*scale;
			z = 0;
			tu = 0.0f;
			tv = 1.0f;

			geo->addVertex(x,y,z,1.0f);
			geo->setTexCoord(geo->nbVertex()-1,tu,tv);

			x = (float)(points->x +base_x)*scale;
			y = (float)points->y*scale;
			z = extrusion*scale;
			tu = 1.0f;
			tv = 1.0f;

			geo->addVertex(x,y,z,1.0f);
			geo->setTexCoord(geo->nbVertex()-1,tu,tv);

			geo->addFace(geo->nbVertex()-4,geo->nbVertex()-3,geo->nbVertex()-1);
			geo->addFace(geo->nbVertex()-1,geo->nbVertex()-2,geo->nbVertex()-4);
		}
		
		base_x += (float)vec[i].getAdvance();  
	}

	for( unsigned int i = 0; i < text_length; ++i )
		delete tri[i];
	delete [] tri;
	delete [] vec;
	
	CATCH_GL_ERROR
	
	const CGeometryEditor &pEditor = geo->getEditor();
	pEditor.genNormals(true);

	geo->glUnLockData();

	return geo;
}


C3DSet* CGLFont::glBuildSet(const std::string& str,
							float precision,
		                    float extrusion,
		                    float scale)
{

	C3DSet *set = new C3DSet("font set");
    C3DSet::C3DSetIterator it = set->getIterator();
    string c;

	for (unsigned int i=0;i<str.size();i++)
	{    
        c = str[i];
		CGeometry *g = glBuildGeometry(c,precision,extrusion,scale);

		set->addChild(g,it);
	}
	
	CATCH_GL_ERROR
	
	return set;
}

RAPTOR_HANDLE CGLFont::glBuildVectors(	const std::string &str, 
										float precision,
										float scale,
										float *width,
										float *height)
{
	RAPTOR_HANDLE result;
	result.hClass = Global::COpenGLClassID::GetClassId().ID();
	float w = 0.0f;

    if ((m_font == NULL) || (m_face == NULL))	// Use Hershey Font
    {
		const char* text= str.data();
		const int text_length= str.size();
		
		result.handle = glGenLists(1);
		glNewList(result.handle,GL_COMPILE);

		if (height != NULL)
			*height = -25.0f;

		for(int i= 0; i < text_length; ++i )
		{
			int ch = (unsigned char)text[i];
			if ((ch >= 32) && (ch <= 126))
			{
				ch -= 32;
				w += scale * simplex[ch][1];
				int nb = 2 * simplex[ch][0];

				glBegin(GL_LINE_STRIP);
					for (int j=2;j<=nb;j+=2)
					{
						if (simplex[ch][j] == -1)
						{
							glEnd();
							glBegin(GL_LINE_STRIP);
						}
						else
							glVertex2f(	scale * simplex[ch][j],
										scale * simplex[ch][j+1]);
					}
				glEnd();
			}
		}

		glEndList();
    }
	else	// TODO
	{
	}

	CATCH_GL_ERROR

	if (width != NULL)
		*width = -0.5f * w;

	return result;
}

// 2D font writer
static int __w = 0;
static int __h = 0;

bool CGLFont::glGenGlyphs(	float precision,
							float extrusion,
							float scale)
{
	glyph *glyphs = new glyph[256];
	glyphSettings *glyphSts = new glyphSettings;
	glyphSts->precision = precision;
	glyphSts->extrusion = extrusion;
	glyphSts->scale = scale;

    string c;

	for (int i=0;i<256;i++)
	{
        c = (char)i;
		RAPTOR_HANDLE h;

		if (m_bVectorized)
		{
			h = glBuildVectors(c, precision, scale, &(glyphs[i].halfWidth), &(glyphs[i].height));
			glyphs[i].advance = -2*glyphs[i].halfWidth;
		}
		else if ((m_bTextured) && (m_bmfont != NULL) && (m_texture != NULL))
		{
			FTGlyphBitmap* gbitmap= m_bmfont->getBitmap(i);
			if( gbitmap != 0 )
			{
				float u = __w * (i % (TEXTURE_WIDTH / __w));
				float v = __h * (i / (TEXTURE_WIDTH / __w));

				float dx = gbitmap->getDeltaX();
				float dy = gbitmap->getDeltaY();
				
				int H = __w * __h;

				float ww = gbitmap->getWidth();
				float hh = gbitmap->getHeight();

				h.handle = glGenLists(1);
				glNewList(h.handle,GL_COMPILE);
					glBegin(GL_QUADS);
						glTexCoord2f(u / TEXTURE_WIDTH	,v / H);				glVertex2f(dx,	  dy);
						glTexCoord2f((u + ww) / TEXTURE_WIDTH,v / H);			glVertex2f(dx+ww, dy);
						glTexCoord2f((u + ww) / TEXTURE_WIDTH,(v + hh) / H);	glVertex2f(dx+ww, dy+hh);
						glTexCoord2f(u / TEXTURE_WIDTH	,(v + hh) / H);			glVertex2f(dx,	  dy+hh);
					glEnd();
				glEndList();

				glyphs[i].halfWidth = -0.5f * ww;
				glyphs[i].height = hh;
				glyphs[i].advance = gbitmap->getAdvance();
			}
		}
		else if (m_font != NULL)
		{
			h = glTTBuildGeometry(	c, precision, extrusion, scale,
									&(glyphs[i].halfWidth),
									&(glyphs[i].height));
			glyphs[i].advance = -2*glyphs[i].halfWidth;
		}

		glyphs[i].glList = h.handle;
	}

	m_glfontglyph.push_back(glyphs);
	m_glfontglyphsettings.push_back(glyphSts);
	return true;
}




bool CGLFont::getGlyphSettings(	float &precision,
								float &extrusion,
								float &scale,
								unsigned int glyphset)
{
	if (glyphset >= m_glfontglyphsettings.size())
		return false;

	glyphSettings *gs = (glyphSettings *)m_glfontglyphsettings.at(glyphset);
	precision = gs->precision;
	extrusion = gs->extrusion;
	scale = gs->scale;

	return true;
}




// 3D font writer, returns Display list
RAPTOR_HANDLE CGLFont::glWriteList(	const std::string &text,
									unsigned int glyphset,
									float *width,
									float *height)
{
	RAPTOR_HANDLE result;
	result.handle = 0;
	result.hClass = 0;

	if (glyphset >= m_glfontglyph.size())
		return result;

	if ((m_font == NULL) || (m_face == NULL))
		return result;

	glyph *glyphs = (glyph*)m_glfontglyph.at(glyphset);
	GLuint TTList=glGenLists(1);
	GLfloat w = 0;
	GLfloat h = 0;
	glyph	g;

	glNewList(TTList,GL_COMPILE);
	for (unsigned int i=0;i<text.size();i++)
	{
		g=glyphs[(unsigned char)text[i]];
		glCallList(g.glList);
		glTranslatef(-2*g.halfWidth,0.0,0.0);
		if (g.height<h) 
			h = g.height;
		w += g.halfWidth;
	}
	glEndList();
	if (width!=NULL)
		*width = w;
	if (height!=NULL)
		*height = h;

	CATCH_GL_ERROR

	result.handle = TTList;
	result.hClass = CGLFont::CGLFontClassID::GetClassId().ID();

	return result;
}


void CGLFont::glWrite(const std::string &text, unsigned int glyphset)
{
	if (glyphset >= m_glfontglyph.size())
		return;

	glyph *glyphs = (glyph*)m_glfontglyph.at(glyphset);

	glPushMatrix();

	for (unsigned int i=0;i<text.size();i++)
	{
		glyph g = glyphs[(unsigned char)text[i]];
		glCallList(g.glList);
		glTranslatef(g.advance,0.0,0.0);
	}

	glPopMatrix();

	CATCH_GL_ERROR
}

void CGLFont::glWrite(const std::string &text, int x, int y)
{
#ifdef RAPTOR_DEBUG_MODE_GENERATION
    if (m_bmfont == NULL)
    {
		Raptor::GetErrorManager()->generateRaptorError(	CGLFont::CGLFontClassID::GetClassId(),
														CRaptorErrorManager::RAPTOR_ERROR,
														CRaptorMessages::ID_INIT_FAILED);
            return;
    }
#endif

	GLfloat viewport[4];
	
	glGetFloatv(GL_VIEWPORT,viewport);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);	
	glPushMatrix();
	glLoadIdentity();
	glOrtho(viewport[0],viewport[2],viewport[1],viewport[3],1,20);
	glMatrixMode(GL_MODELVIEW);
	glPixelStorei(GL_UNPACK_ALIGNMENT,1);

	if (m_bTextured)
	{
		
		glEnable(GL_TEXTURE_2D);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
		m_texture->glRender();

		glTranslatef(x,viewport[3] - y,-1.0f);
		for(unsigned int i=0;i<text.size();i++)
		{
			unsigned char ch = text[i];

			glyph *glyphs = (glyph*)m_glfontglyph.at(0);
			glyph g = glyphs[ch];
			glCallList(g.glList);
			glTranslatef(g.advance,0.0f,0.0f);
		}
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_BLEND);
	}
	else
	{
		GLint px = x;
		GLint py = viewport[3] - y;
		for(unsigned int i=0;i<text.size();i++)
		{
			unsigned char ch = text[i];

			FTGlyphBitmap* gbitmap= m_bmfont->getBitmap(ch);
			if( gbitmap == 0 )
				continue;
			if( gbitmap->getBitmap() != 0 )
			{
				GLint x_dst = px + gbitmap->getDeltaX();
				GLint y_dst = py + gbitmap->getDeltaY();

				glRasterPos4i(x_dst, y_dst, -1.0f, 1.0f );
				glBitmap(	gbitmap->getWidth(),
							gbitmap->getHeight(),
							0., 0., // x,y orig
							0., 0., // x,y move
							(const GLubyte*) gbitmap->getBitmap() );
			}
			px += gbitmap->getAdvance();
		}
	}
	
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	CATCH_GL_ERROR
}


bool CGLFont::glCreateTexture(const std::string &name, unsigned int size,bool antialiased)
{
	m_bTextured = true;
	m_bAntialiased = antialiased;

	if (create2D(name,size))
	{
		unsigned int h = 0;
		unsigned int w = 0;

		// find size of biggest char
		for (unsigned int i=0;i<256;i++)
		{
			char c = (char)i;
			FTGlyphBitmap* gbitmap= m_bmfont->getBitmap(c);
			if( gbitmap == 0 )
				continue;
			if( gbitmap->getBitmap() != 0 )
			{
				h = MAX(h,gbitmap->getHeight());
				w = MAX(w,gbitmap->getWidth());
			}
		}

		// Create a texture of appropriate size
		CTextureFactory &factory = CTextureFactory::getDefaultFactory();
		if (m_texture == NULL)
		{
			m_texture = factory.glCreateTexture(CTextureObject::CGL_LIGHTMAP_ALPHA,
												CTextureObject::CGL_MULTIPLY, // CGL_ALPHA_TRANSPARENT
												CTextureObject::CGL_BILINEAR);
		}

		if (m_bAntialiased)
		{
			w += 2;
			h += 2;
		}
		w = pow(2,ceil(log((float)w)/log(2.0)));
		h = pow(2,ceil(log((float)h)/log(2.0)));
		__w = w;
		__h = h;

		int nb = TEXTURE_WIDTH / w;
		size_t W = TEXTURE_WIDTH;
		size_t H = h * w; //h * 256 / nb;
		factory.glResizeTexture(m_texture,W,H);
		m_texture->glRender();

		unsigned char *buffer = new unsigned char[2*W*H];
		memset(buffer,0,2*W*H);
		for (unsigned int i=0;i<256;i++)
		{
			char c = (char)i;
			FTGlyphBitmap* gbitmap= m_bmfont->getBitmap(c);
			if( gbitmap == 0 )
				continue;

			size_t bw = gbitmap->getWidth();
			size_t bh = gbitmap->getHeight();
			size_t br = gbitmap->getRowSize();

			int x = w * (i % (TEXTURE_WIDTH / w));
			int y = h * (i / (TEXTURE_WIDTH / w));
			
			const unsigned char *srcbuffer = (const GLubyte*) gbitmap->getBitmap();
			size_t pos = 0;
			for (size_t k=0;k<bh;k++)
			{
				pos = 2*(y+k)*W + 2*x;
				for (size_t j=0;j<bw;j++)
				{
					size_t ofs = (j >> 3) + k*br;
					unsigned char val = srcbuffer[ofs];
					int column = 128 >> (j%8);

					unsigned char b = (val & column) ? 255 : 0;
					buffer[pos++] = b;
					buffer[pos++] = b;
				}
			}
		}

		if (m_bAntialiased)
		{
			// Antialias font buffer
			unsigned char *AAbuffer = new unsigned char[2*W*H];
			for (size_t j=0;j<H;j++)
			{
				for (size_t i=0;i<W;i++)
				{
					size_t pos = 2*(W*j+i);
					unsigned int nbElt = 0;
					unsigned int base = 0;

					if (i > 0)
					{
						nbElt++;
						base += buffer[pos-2];
						if (j > 0)
						{
							nbElt++;
							base += buffer[pos-2-W-W];
						}
						if (j < H-1)
						{
							nbElt++;
							base += buffer[pos-2+W+W];
						}
					}
					if (i < W-1)
					{
						nbElt++;
						base += buffer[pos+2];
						if (j > 0)
						{
							nbElt++;
							base += buffer[pos+2-W-W];
						}
						if (j < H-1)
						{
							nbElt++;
							base += buffer[pos+2+W+W];
						}
					}
					if (j > 0)
					{
						nbElt++;
						base += buffer[pos-W-W];
					}
					if (j < H-1)
					{
						nbElt++;
						base += buffer[pos+W+W];
					}

					base = MIN(buffer[pos] + base / nbElt,255);
					AAbuffer[pos] = (base > 0) ? 255 : 0;
					AAbuffer[pos+1] = base;
				}
			}

			delete [] buffer;
			buffer = AAbuffer;
		}
		

		// Final load
		glTexImage2D(GL_TEXTURE_2D, 0,
					 GL_LUMINANCE8_ALPHA8,
					 W,H,0,
					 GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE,
					 buffer);

		delete [] buffer;

//		factory.glExportTexture(m_texture,"FontTexture.tga");

		CATCH_GL_ERROR

		return true;
	}
	else	/*	TODO	*/
		return false;
}
