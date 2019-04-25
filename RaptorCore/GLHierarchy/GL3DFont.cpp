// GL3DFont.cpp: implementation of the CGL3DFont class.
//
//////////////////////////////////////////////////////////////////////
#include "Subsys/CodeGeneration.h"

#include "Subsys/FreeType/FTFace.h"
#include "Subsys/FreeType/TTOutlineFont.h"
#include "Subsys/FreeType/FTGlyphVectorizer.h"
#include "Subsys/FreeType/TTGlyphTriangulator.h"

#if !defined(AFX_GL3DFONT_H__81553B3D_92A7_4C82_80E7_B6F98CBD75FA__INCLUDED_)
	#include "GL3DFont.h"
#endif
#if !defined(AFX_RAPTORERRORMANAGER_H__FA5A36CD_56BC_4AA1_A5F4_451734AD395E__INCLUDED_)
	#include "System/RaptorErrorManager.h"
#endif
#if !defined(AFX_GEOMETRY_H__B42ABB87_80E8_11D3_97C2_DE5C28000000__INCLUDED_)
	#include "GLHierarchy/Geometry.h"
#endif
#if !defined(AFX_GEOMETRYEDITOR_H__2D77E428_ED3D_416B_8DE9_DABFD45A38A7__INCLUDED_)
	#include "GLHierarchy/GeometryEditor.h"
#endif
#if !defined(AFX_3DSET_H__DB24F01B_80B9_11D3_97C1_FC2841000000__INCLUDED_)
	#include "GLHierarchy/3DSet.h"
#endif


void normalize(float v[3], float norm)
{
	float sq = (float)sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
	if (sq > 0)
	{
		GLfloat d = norm / sq;
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


RAPTOR_NAMESPACE

CGL3DFont::CGL3DFont(const std::string& name)
	:CGLFont(name), m_font(NULL), m_face(0),
	m_bTexturedFace(false), m_bTexturedSide(false)

{
}


CGL3DFont::~CGL3DFont()
{
	if (m_font != NULL)
		delete m_font;
	if (m_face != NULL)
		delete m_face;
}

bool CGL3DFont::init(const std::string &filename,
					 unsigned int size,
					 bool faceTextured,
					 bool sideTextured)
{
	if (m_face == NULL)
	{
		m_face = new FTFace;
		m_fontFileName = filename;
		if (!m_face->open(filename.data()))
		{
			delete m_face;
			m_face = NULL;
#ifdef RAPTOR_DEBUG_MODE_GENERATION
			Raptor::GetErrorManager()->generateRaptorError(CGLFont::CGLFontClassID::GetClassId(),
				CRaptorErrorManager::RAPTOR_ERROR,
				CRaptorMessages::ID_SUBSYS_FAILED);
#endif
			return false;
		}
	}

	if (m_font == NULL)
	{
		m_font = new TTOutlineFont;
		if (!m_font->create(m_face, size))
		{
			delete m_font;
			m_font = NULL;
#ifdef RAPTOR_DEBUG_MODE_GENERATION
			Raptor::GetErrorManager()->generateRaptorError(CGLFont::CGLFontClassID::GetClassId(),
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


bool CGL3DFont::glGenGlyphs(float precision,
							float extrusion,
							float scale)
{
	glyph *glyphs = new glyph[256];

	string c;
	for (int i = 0; i < 256; i++)
	{
		c = (char)i;
		RAPTOR_HANDLE h = glTTBuildGeometry(c, precision, extrusion, scale,
											&(glyphs[i].halfWidth),
											&(glyphs[i].height));
		glyphs[i].advance = -2 * glyphs[i].halfWidth;
		glyphs[i].glList = h.handle();
	}

	m_glfontglyph.push_back(glyphs);
	return CGLFont::glGenGlyphs(precision, extrusion, scale);
}


RAPTOR_HANDLE CGL3DFont::glWriteList(const std::string &text,
									 unsigned int glyphset,
									 float *width,
									 float *height)
{
	RAPTOR_HANDLE result;

	if (glyphset >= m_glfontglyph.size())
		return result;

	if ((m_font == NULL) || (m_face == NULL))
		return result;

	glyph *glyphs = (glyph*)m_glfontglyph.at(glyphset);
	GLuint TTList = glGenLists(1);
	GLfloat w = 0;
	GLfloat h = 0;

	glNewList(TTList, GL_COMPILE);
	for (unsigned int i = 0; i<text.size(); i++)
	{
		glyph g = glyphs[(unsigned char)text[i]];
		glCallList(g.glList);
		glTranslatef(-2 * g.halfWidth, 0.0, 0.0);
		if (g.height<h)
			h = g.height;
		w += g.halfWidth;
	}
	glEndList();
	if (width != NULL)
		*width = w;
	if (height != NULL)
		*height = h;

	CATCH_GL_ERROR

	result.handle(TTList);
	result.hClass(CGLFont::CGLFontClassID::GetClassId().ID());

	return result;
}


CGeometry* CGL3DFont::glBuildGeometry(const std::string &str,
									  float precision,
									  float extrusion,
									  float scale)
{
#ifdef RAPTOR_DEBUG_MODE_GENERATION
	if ((m_font == NULL) ||
		(m_face == NULL))
	{
		Raptor::GetErrorManager()->generateRaptorError(CGLFont::CGLFontClassID::GetClassId(),
			CRaptorErrorManager::RAPTOR_ERROR,
			CRaptorMessages::ID_INIT_FAILED);
		return NULL;
	}
#endif

	CGeometry* geo = new CGeometry("Font Geometry");

	float base_x = 0;
	float miny = 1e20f, maxy = -1e20f;
	float minx = 1e20f, maxx = -1e20f;
	float v3[3];

	const char* text = str.data();
	const unsigned int text_length = str.size();

	FTGlyphVectorizer* vec = new FTGlyphVectorizer[text_length];
	TTGlyphTriangulator** tri = new TTGlyphTriangulator*[text_length];

	for (unsigned int i = 0; i < text_length; ++i)
	{
		tri[i] = new TTGlyphTriangulator();
		int ch = (unsigned char)text[i];
		FTGlyphOutline* g = m_font->getOutline(ch);

		vec[i].setPrecision(precision);
		if (vec[i].vectorize(g))
			tri[i]->triangulate(vec[i]);
	}

	unsigned int nbTriangles = 0;
	for (unsigned int i = 0; i < text_length; ++i)
		nbTriangles += tri[i]->getNbTriangles();

	if (extrusion > 0.0f)
	{
		nbTriangles *= 2;

		int nbPoints = 0;
		for (unsigned int i = 0; i<text_length; i++)
		{
			for (unsigned int j = 0; j < vec[i].getNContours(); j++)
			{
				const FTGlyphVectorizer::Contour& cnt = vec[i].getContour(j);
				if (cnt.y_min<miny)
					miny = (float)cnt.y_min;
				if (cnt.y_max>maxy)
					maxy = (float)cnt.y_max;
				if (cnt.x_min<minx)
					minx = (float)cnt.x_min;
				if (cnt.x_max>maxx)
					maxx = (float)cnt.x_max;
				nbPoints += cnt.getNPoints();
			}
		}

		nbTriangles += 2 * nbPoints;
	}

	geo->glSetVertices(3 * nbTriangles);
	geo->glSetPolygons(nbTriangles);
	if (m_bTexturedFace || m_bTexturedSide)
		geo->glSetTexCoords(3 * nbTriangles);
	geo->glLockData();

	for (unsigned int i = 0; i < text_length; ++i)
	{
		float x, y, z;
		float tu, tv;

		for (unsigned int j = 0; j < tri[i]->getNbTriangles(); ++j)
		{
			const TTGlyphTriangulator::Triangle& t = tri[i]->getTriangle(j);

			z = extrusion * scale;
			x = (float)(t.p1.x + base_x)*scale;
			y = (float)t.p1.y*scale;
			tu = (x - minx) / (maxx - minx);
			tv = (y - miny) / (maxy - miny);

			geo->addVertex(x, y, z, 1.0f);
			geo->setTexCoord(geo->nbVertex() - 1, tu, tv);

			x = (float)(t.p2.x + base_x)*scale;
			y = (float)t.p2.y*scale;
			tu = (x - minx) / (maxx - minx);
			tv = (y - miny) / (maxy - miny);

			geo->addVertex(x, y, z, 1.0f);
			geo->setTexCoord(geo->nbVertex() - 1, tu, tv);

			x = (float)(t.p3.x + base_x)*scale;
			y = (float)t.p3.y*scale;
			tu = (x - minx) / (maxx - minx);
			tv = (y - miny) / (maxy - miny);

			geo->addVertex(x, y, z, 1.0f);
			geo->setTexCoord(geo->nbVertex() - 1, tu, tv);

			geo->addFace(geo->nbVertex() - 3, geo->nbVertex() - 2, geo->nbVertex() - 1);

			if (extrusion > 0.0f)
			{
				z = 0;
				x = (float)(t.p1.x + base_x)*scale;
				y = (float)t.p1.y*scale;
				tu = -(minx - x) / (maxx - minx);
				tv = (y - miny) / (maxy - miny);

				geo->addVertex(x, y, z, 1.0f);
				geo->setTexCoord(geo->nbVertex() - 1, tu, tv);

				x = (float)(t.p3.x + base_x)*scale;
				y = (float)t.p3.y*scale;
				tu = -(minx - x) / (maxx - minx);
				tv = (y - miny) / (maxy - miny);

				geo->addVertex(x, y, z, 1.0f);
				geo->setTexCoord(geo->nbVertex() - 1, tu, tv);

				x = (float)(t.p2.x + base_x)*scale;
				y = (float)t.p2.y*scale;
				tu = -(minx - x) / (maxx - minx);
				tv = (y - miny) / (maxy - miny);

				geo->addVertex(x, y, z, 1.0f);
				geo->setTexCoord(geo->nbVertex() - 1, tu, tv);

				geo->addFace(geo->nbVertex() - 3, geo->nbVertex() - 2, geo->nbVertex() - 1);
			}
		}


		if (extrusion>0.0)
			for (unsigned int c = 0; c < vec[i].getNContours(); ++c)
			{
				const FTGlyphVectorizer::Contour& contour = vec[i].getContour(c);
				FTGlyphVectorizer::POINT* points = contour.points;
				if (points == 0 || contour.getNPoints() <= 0)
					continue;

				v3[2] = 0;

				float oldx = (float)(points->x + base_x)*scale;
				float oldy = (float)points->y*scale;

				for (int k = 0; k < contour.getNPoints(); ++k, points++)
				{
					x = (float)(points->x + base_x)*scale;
					y = (float)points->y*scale;

					float dist = (float)(sqrt((x - oldx)*(x - oldx) + (y - oldy)*(y - oldy)));
					oldx = x;
					oldy = y;

					z = 0;
					tu = 0.0f;
					tv = dist * scale / (maxx - minx);
					float oldtv = tv;

					geo->addVertex(x, y, z, 1.0f);
					geo->setTexCoord(geo->nbVertex() - 1, tu, tv);

					x = oldx;
					y = oldy;
					z = extrusion*scale;
					tu = 1;
					tv = oldtv;

					geo->addVertex(x, y, z, 1.0f);
					geo->setTexCoord(geo->nbVertex() - 1, tu, tv);

					if (k > 0)
					{
						geo->addFace(geo->nbVertex() - 4, geo->nbVertex() - 3, geo->nbVertex() - 1);
						geo->addFace(geo->nbVertex() - 1, geo->nbVertex() - 2, geo->nbVertex() - 4);
					}
				}

				points = contour.points;

				x = (float)(points->x + base_x)*scale;
				y = (float)points->y*scale;
				z = 0;
				tu = 0.0f;
				tv = 1.0f;

				geo->addVertex(x, y, z, 1.0f);
				geo->setTexCoord(geo->nbVertex() - 1, tu, tv);

				x = (float)(points->x + base_x)*scale;
				y = (float)points->y*scale;
				z = extrusion*scale;
				tu = 1.0f;
				tv = 1.0f;

				geo->addVertex(x, y, z, 1.0f);
				geo->setTexCoord(geo->nbVertex() - 1, tu, tv);

				geo->addFace(geo->nbVertex() - 4, geo->nbVertex() - 3, geo->nbVertex() - 1);
				geo->addFace(geo->nbVertex() - 1, geo->nbVertex() - 2, geo->nbVertex() - 4);
			}

		base_x += (float)vec[i].getAdvance();
	}

	for (unsigned int i = 0; i < text_length; ++i)
		delete tri[i];
	delete[] tri;
	delete[] vec;

	CATCH_GL_ERROR

	const CGeometryEditor &pEditor = geo->getEditor();
	pEditor.genNormals(true);

	geo->glUnLockData();

	return geo;
}


RAPTOR_HANDLE CGL3DFont::glTTBuildGeometry(const std::string &str,
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
		Raptor::GetErrorManager()->generateRaptorError(CGLFont::CGLFontClassID::GetClassId(),
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

	const char* text = str.data();
	const unsigned int text_length = str.size();

	FTGlyphVectorizer* vec = new FTGlyphVectorizer[text_length];
	TTGlyphTriangulator** tri = new TTGlyphTriangulator*[text_length];

	for (unsigned int i = 0; i < text_length; ++i)
	{
		tri[i] = new TTGlyphTriangulator();
		int ch = (unsigned char)text[i];

		vec[i].setPrecision(precision);
		FTGlyphOutline* g = m_font->getOutline(ch);
		if ((0 != g) && (vec[i].vectorize(g)))
		{
			for (unsigned int c = 0; c < vec[i].getNContours(); ++c)
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

	TTList = glGenLists(1);
	glNewList(TTList, GL_COMPILE);
	glPushMatrix();

	for (unsigned int i = 0; i < text_length; ++i)
	{
		float x, y, z;
		float tu, tv;

		glBegin(GL_TRIANGLES);
		for (unsigned int j = 0; j < tri[i]->getNbTriangles(); ++j)
		{
			const TTGlyphTriangulator::Triangle& t = tri[i]->getTriangle(j);

			glNormal3f(0.0, 0.0, 1.0);

			x = (float)(t.p1.x + base_x)*scale;
			y = (float)t.p1.y*scale;
			z = extrusion * scale;
			tu = (x - minx) / (maxx - minx);
			tv = (y - miny) / (maxy - miny);
			GENVERTEX(x, y, z, tu, tv, m_bTexturedFace)

			x = (float)(t.p2.x + base_x)*scale;
			y = (float)t.p2.y*scale;
			z = extrusion * scale;
			tu = (x - minx) / (maxx - minx);
			tv = (y - miny) / (maxy - miny);
			GENVERTEX(x, y, z, tu, tv, m_bTexturedFace)

			x = (float)(t.p3.x + base_x)*scale;
			y = (float)t.p3.y*scale;
			z = extrusion * scale;
			tu = (x - minx) / (maxx - minx);
			tv = (y - miny) / (maxy - miny);
			GENVERTEX(x, y, z, tu, tv, m_bTexturedFace)

			if (extrusion>0.0)
			{
				glNormal3f(0.0, 0.0, -1.0);

				z = 0;
				x = (float)(t.p1.x + base_x)*scale;
				y = (float)t.p1.y*scale;
				tu = -(minx - x) / (maxx - minx);
				tv = (y - miny) / (maxy - miny);
				GENVERTEX(x, y, z, tu, tv, m_bTexturedFace)

					x = (float)(t.p3.x + base_x)*scale;
				y = (float)t.p3.y*scale;
				tu = -(minx - x) / (maxx - minx);
				tv = (y - miny) / (maxy - miny);
				GENVERTEX(x, y, z, tu, tv, m_bTexturedFace)

					x = (float)(t.p2.x + base_x)*scale;
				y = (float)t.p2.y*scale;
				tu = -(minx - x) / (maxx - minx);
				tv = (y - miny) / (maxy - miny);
				GENVERTEX(x, y, z, tu, tv, m_bTexturedFace)
			}
		}
		glEnd();

		if (extrusion>0.0)
			for (unsigned int c = 0; c < vec[i].getNContours(); ++c)
			{
				const FTGlyphVectorizer::Contour& contour = vec[i].getContour(c);
				FTGlyphVectorizer::POINT* points = contour.points;
				if (points == 0 || contour.getNPoints() <= 0)
					continue;

				v3[2] = 0;
				glBegin(GL_QUAD_STRIP);
				float oldx = (float)(points->x + base_x)*scale;
				float oldy = (float)points->y*scale;

				for (int k = 0; k < contour.getNPoints(); ++k, points++)
				{
					if (k<contour.getNPoints() - 1)
					{
						v3[0] = (float)points->y - (float)(points + 1)->y;
						v3[1] = (float)(points + 1)->x - (float)points->x;
					}
					else
					{
						v3[0] = (float)points->y - (float)contour.points->y;
						v3[1] = (float)contour.points->x - (float)points->x;
					}
					normalize(v3, 1);
					glNormal3fv(v3);

					x = (float)(points->x + base_x)*scale;
					y = (float)points->y*scale;

					float dist = (float)(sqrt((x - oldx)*(x - oldx) + (y - oldy)*(y - oldy)));
					oldx = x;
					oldy = y;

					z = 0;
					tu = 0.0f;
					tv = dist * scale / (maxx - minx);
					float oldtv = tv;
					GENVERTEX(x, y, z, tu, tv, m_bTexturedSide)

					x = oldx;
					y = oldy;
					z = extrusion*scale;
					tu = 1;
					tv = oldtv;
					GENVERTEX(x, y, z, tu, tv, m_bTexturedSide)
				}

				points = contour.points;

				x = (float)(points->x + base_x)*scale;
				y = (float)points->y*scale;
				z = 0;
				tu = 0.0f;
				tv = 1.0f;
				GENVERTEX(x, y, z, tu, tv, m_bTexturedSide)

				x = (float)(points->x + base_x)*scale;
				y = (float)points->y*scale;
				z = extrusion*scale;
				tu = 1.0f;
				tv = 1.0f;
				GENVERTEX(x, y, z, tu, tv, m_bTexturedSide)
				glEnd();
			}
		base_x += (float)vec[i].getAdvance();
	}
	glPopMatrix();
	glEndList();

	for (unsigned int i = 0; i < text_length; ++i)
		delete tri[i];
	delete[] tri;
	delete[] vec;

	if (width != NULL)
		*width = -base_x*0.5f*scale;
	if (height != NULL)
		*height = -(maxy + miny)*0.5f*scale;

	CATCH_GL_ERROR

	RAPTOR_HANDLE result;
	result.handle(TTList);
	result.hClass(CGLFont::CGLFontClassID::GetClassId().ID());

	return result;
}


C3DSet* CGL3DFont::glBuildSet(const std::string& str,
							  float precision,
							  float extrusion,
							  float scale)
{
	C3DSet *set = new C3DSet("font set");
	C3DSet::C3DSetIterator it = set->getIterator();
	string c;

	for (unsigned int i = 0; i<str.size(); i++)
	{
		c = str[i];
		CGeometry *g = glBuildGeometry(c, precision, extrusion, scale);

		set->addChild(g, it);
	}

	CATCH_GL_ERROR

	return set;
}
