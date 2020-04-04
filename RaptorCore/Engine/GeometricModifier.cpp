// GeometricModifier.cpp: implementation of the CGeometricModifier class.
//
//////////////////////////////////////////////////////////////////////
#include "Subsys/CodeGeneration.h"

#if !defined(AFX_GEOMETRICMODIFIER_H__5C0729A2_6F0C_42CA_9033_CAFB26247761__INCLUDED_)
	#include "GeometricModifier.h"
#endif
#if !defined(AFX_GEOMETRY_H__B42ABB87_80E8_11D3_97C2_DE5C28000000__INCLUDED_)
	#include "GLHierarchy/Geometry.h"
#endif


RAPTOR_NAMESPACE_BEGIN

//! Modifier geometry
class CGeometryWrapper : public CObject3D
{
public:
	CGeometryWrapper(CGeometry *geometry,CGeometricModifier *pModifier)	
		:CObject3D(CObject3D::CObject3DClassID::GetClassId(),"GEOMETRY_WRAPPER"),
		m_pModifier(pModifier),
		m_pGeometry(NULL),
		vertexWrap(NULL),
		normalWrap(NULL),
		texWrap(NULL),
		m_bUpdateVertices(false),
		m_bUpdateNormals(false),
		m_bUpdateTexCoords(false)
	{
		if (geometry != NULL)
		{
			m_pGeometry = geometry;
			m_pGeometry->registerDestruction(this);
		}
	}

	virtual ~CGeometryWrapper()
	{
		if (m_pGeometry != NULL)
			m_pGeometry->unregisterDestruction(this);

		if (vertexWrap != NULL)
			delete [] vertexWrap;
		if (normalWrap != NULL)
			delete [] normalWrap;
		if (texWrap != NULL)
			delete [] texWrap;
	}

	virtual void unLink(const CPersistence*)
	{
		m_pGeometry = NULL;
	}

	DECLARE_OBJECT3D_NOMANIPULATORS

	CGeometry	*m_pGeometry;
	GL_COORD_VERTEX	*vertexWrap;
	GL_COORD_VERTEX	*normalWrap;
	GL_TEX_VERTEX	*texWrap;
	bool	m_bUpdateVertices;
	bool	m_bUpdateNormals;
	bool	m_bUpdateTexCoords;

private:
	CGeometricModifier *m_pModifier;
};

void CGeometryWrapper::glRender()
{
	unsigned int nbVertex = m_pGeometry->nbVertex();

	if (m_bUpdateVertices && (vertexWrap != NULL))
	{
		m_bUpdateVertices = false;
		m_pGeometry->glSetVertices(nbVertex,vertexWrap);
	}

	if (m_bUpdateNormals && (normalWrap != NULL))
	{
		m_bUpdateNormals = false;
		m_pGeometry->glSetNormals(nbVertex,normalWrap);
	}

	if (m_bUpdateTexCoords && (texWrap != NULL))
	{
		m_bUpdateTexCoords = false;
		m_pGeometry->glSetTexCoords(nbVertex,texWrap);
	}

	m_pGeometry->glRender();
}


RAPTOR_NAMESPACE_END

RAPTOR_NAMESPACE

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGeometricModifier::CGeometricModifier(	TIME_FUNCTION_TYPE	timeFunction,
										float				timeArg0,
										float				timeArg1,
										float				timeArg2,
										float				timeArg3,
										USER_FUNCTION		pUserFunction,
										const CPersistence::CPersistenceClassID& id,
										const std::string& name)
	:CModifier(timeFunction,timeArg0,timeArg1,timeArg2,timeArg3,pUserFunction,id,name),
	m_pWrapper(NULL)
{
}

CGeometricModifier::~CGeometricModifier()
{
	if (m_pWrapper != NULL)
	{
		m_pWrapper->unregisterDestruction(this);
		delete m_pWrapper;
		m_pWrapper = NULL;
	}
}

void CGeometricModifier::unLink(const CPersistence* object)
{
	if (object == static_cast<CPersistence*>(m_pWrapper))
		m_pWrapper = NULL;
}

CObject3D *CGeometricModifier::getObject() const
{
	return m_pWrapper; 
}

void RAPTOR_FASTCALL CGeometricModifier::deltaTime(float dt)
{
	if (m_pWrapper != NULL)
	{
		m_pWrapper->m_bUpdateVertices = true;
		m_pWrapper->m_bUpdateNormals = true;
		m_pWrapper->m_bUpdateTexCoords = true;
	}

	CModifier::deltaTime(dt);

	if (m_pWrapper != NULL)
	{
		if (m_pWrapper->vertexWrap != NULL)
			updateVertices(dt,m_pWrapper->vertexWrap);
		if (m_pWrapper->normalWrap != NULL)
			updateNormals(dt,m_pWrapper->normalWrap);
		if (m_pWrapper->texWrap != NULL)
			updateTexCoords(dt,m_pWrapper->texWrap);
	}
}

void RAPTOR_FASTCALL CGeometricModifier::updateVertices(float ,GL_COORD_VERTEX *)
{
	if (m_pWrapper != NULL)
	{
		m_pWrapper->m_bUpdateVertices = false;
	}
}

void RAPTOR_FASTCALL CGeometricModifier::updateNormals(float ,GL_COORD_VERTEX *)
{
	if (m_pWrapper != NULL)
	{
		m_pWrapper->m_bUpdateNormals = false;
	}
}

void RAPTOR_FASTCALL CGeometricModifier::updateTexCoords(float ,GL_TEX_VERTEX *)
{
	if (m_pWrapper != NULL)
	{
		m_pWrapper->m_bUpdateTexCoords = false;
	}
}

void CGeometricModifier::setGeometry(CGeometry *geometry)
{
	if (m_pWrapper != NULL)
	{
		delete m_pWrapper;
		m_pWrapper = NULL;
	}

	if (geometry != NULL)
	{
		m_pWrapper = new CGeometryWrapper(geometry,this);
		m_pWrapper->registerDestruction(this);

		//	Always extract geometry
		//	TODO: what to decide if geometry is not loaded ?
		//	(i.e. nbVertex == 0, even if allocated)
		m_pWrapper->vertexWrap = new GL_COORD_VERTEX[geometry->nbVertex()];

		if (geometry->hasModel(CGeometry::CGL_NORMALS))
		{
			m_pWrapper->normalWrap = new GL_COORD_VERTEX[geometry->nbVertex()];
		}
		if (geometry->hasModel(CGeometry::CGL_TEXTURE))
		{
			m_pWrapper->texWrap = new GL_TEX_VERTEX[geometry->nbVertex()];
		}
	}
}

