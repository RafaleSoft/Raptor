/***************************************************************************/
/*                                                                         */
/*  Geometry.cpp                                                           */
/*                                                                         */
/*    Raptor OpenGL & Vulkan realtime 3D Engine SDK.                       */
/*                                                                         */
/*  Copyright 1998-2019 by                                                 */
/*  Fabrice FERRAND.                                                       */
/*                                                                         */
/*  This file is part of the Raptor project, and may only be used,         */
/*  modified, and distributed under the terms of the Raptor project        */
/*  license, LICENSE.  By continuing to use, modify, or distribute         */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/


#include "Subsys/CodeGeneration.h"


#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
	#include "System/Raptor.h"
#endif
#if !defined(AFX_GEOMETRY_H__B42ABB87_80E8_11D3_97C2_DE5C28000000__INCLUDED_)
	#include "Geometry.h"
#endif
#if !defined(AFX_SHADER_H__4D405EC2_7151_465D_86B6_1CA99B906777__INCLUDED_)
	#include "Shader.h"
#endif
#if !defined(AFX_TEXTUREUNITSETUP_H__4A6ADC72_02E5_4F2A_931E_A736B6D6E0F0__INCLUDED_)
	#include "TextureUnitSetup.h"
#endif
#if !defined(AFX_RAPTORERRORMANAGER_H__FA5A36CD_56BC_4AA1_A5F4_451734AD395E__INCLUDED_)
	#include "System/RaptorErrorManager.h"
#endif
#if !defined(AFX_MEMORY_H__81A6CA9A_4ED9_4260_B6E4_C03276C38DBC__INCLUDED_)
	#include "System/Memory.h"
#endif
#if !defined(AFX_RAPTORGLEXTENSIONS_H__E5B5A1D9_60F8_4E20_B4E1_8E5A9CB7E0EB__INCLUDED_)
	#include "System/RaptorGLExtensions.h"
#endif
#if !defined(AFX_RAPTORIO_H__87D52C27_9117_4675_95DC_6AD2CCD2E78D__INCLUDED_)
	#include "System/RaptorIO.h"
#endif
#if !defined(AFX_OBJECTFACTORY_H__7F891C52_9E32_489C_B09C_5E5803522D91__INCLUDED_)
	#include "ObjectFactory.h"
#endif
#if !defined(AFX_GEOMETRYALLOCATOR_H__802B3C7A_43F7_46B2_A79E_DDDC9012D371__INCLUDED_)
	#include "Subsys/GeometryAllocator.h"
#endif
#if !defined(AFX_VERTEXPROGRAM_H__F2D3BBC6_87A1_4695_B667_2B8C3C4CF022__INCLUDED_)
	#include "VertexProgram.h"
#endif
#if !defined(AFX_GEOMETRYEDITOR_H__2D77E428_ED3D_416B_8DE9_DABFD45A38A7__INCLUDED_)
    #include "GeometryEditor.h"
#endif
#if !defined(AFX_OBJECT3DCONTOUR_H__C0C2B562_ABBC_4B04_A1E7_E0727FAC66AB__INCLUDED_)
	#include "GLHierarchy/Object3DContour.h"
#endif
#if !defined(AFX_IRENDERINGPROPERTIES_H__634BCF2B_84B4_47F2_B460_D7FDC0F3B698__INCLUDED_)
	#include "IRenderingProperties.h"
#endif
#if !defined(AFX_RAPTORVULKANCOMMANDBUFFER_H__0398BABD_747B_4DFE_94AA_B026BDBD03B1__INCLUDED_)
	#include "Subsys/Vulkan/VulkanCommandBuffer.h"
#endif
#if !defined(AFX_RAPTORINSTANCE_H__90219068_202B_46C2_BFF0_73C24D048903__INCLUDED_)
	#include "Subsys/RaptorInstance.h"
#endif


RAPTOR_NAMESPACE

static CGeometry::CGeometryClassID geometryID;
static CPersistentObjectType<CGeometry> geometryFactory(geometryID);
const CPersistence::CPersistenceClassID& CGeometry::CGeometryClassID::GetClassId(void)
{
	return geometryID;
}


const CGeometryEditor& CGeometry::getEditor(void)
{
    if (m_pEditor == NULL)
        m_pEditor = new CGeometryEditor(this);

    return *m_pEditor;
}

void CGeometry::setRenderingModel(CGeometry::RENDERING_MODEL model)
{
	m_renderingModel = (model & CGeometry::CGL_FULLRENDER);
	if ((m_renderingModel == 0) || (m_renderingModel > CGeometry::CGL_FULLRENDER))
		m_renderingModel = CGeometry::CGL_FRONT_GEOMETRY;
}

void CGeometry::addModel(CGeometry::RENDERING_MODEL model)
{
	unsigned int m = ((m_renderingModel | model) & CGeometry::CGL_FULLRENDER);
	m_renderingModel = m;
}

void CGeometry::removeModel(CGeometry::RENDERING_MODEL model)
{
	m_renderingModel &= ~model;
}


//////////////////////////////////////////////////////////////////////
// CGeometry implementation
//////////////////////////////////////////////////////////////////////
CGeometry::CGeometry(const std::string& name):
	CObject3D(geometryID,name),
#if defined(DATA_EXTENDED)
    geometry(NULL),
#elif defined(DATA_PACKED)
	vertex(NULL),normals(NULL),colors(NULL),
	texcoords(NULL),texcoords2(NULL),tangents(NULL),binormals(NULL),
	fogcoords(NULL),weightcoords(NULL),
#endif
    polys(NULL),m_renderingModel(CGeometry::CGL_FRONT_GEOMETRY),
	m_pEditor(NULL), m_pBinder(NULL)
{
	m_nbVertex = m_nbPolys = 0;
    m_bDataLocked = false;
    m_bUpdateBBox = false;

	CResourceAllocator::CResourceBinder *binder = new CResourceAllocator::CResourceBinder();
	m_pBinder = binder;
}

CGeometry::CGeometry(const std::string& name,const CPersistence::CPersistenceClassID &classID):
    CObject3D(classID,name),
#if defined(DATA_EXTENDED)
    geometry(NULL),
#elif defined(DATA_PACKED)
	vertex(NULL),normals(NULL),colors(NULL),
	texcoords(NULL),texcoords2(NULL),tangents(NULL),binormals(NULL),
	fogcoords(NULL),weightcoords(NULL),
#endif
	polys(NULL), m_renderingModel(CGeometry::CGL_FRONT_GEOMETRY),
	m_pEditor(NULL), m_pBinder(NULL)
{
    m_nbVertex = m_nbPolys = 0;
    m_bDataLocked = false;
    m_bUpdateBBox = false;

	CResourceAllocator::CResourceBinder *binder = new CResourceAllocator::CResourceBinder();
	m_pBinder = binder;
}

CGeometry::~CGeometry()
{
#if defined(DATA_EXTENDED)
    if (geometry != NULL)
        delete [] geometry;
#elif defined(DATA_PACKED)

    if (m_bDataLocked)
        glUnLockData();

	if (NULL != vertex)
		CGeometryAllocator::GetInstance()->releaseVertices((float*)vertex);
	if (NULL != normals)
		CGeometryAllocator::GetInstance()->releaseVertices((float*)normals);
	if (NULL != texcoords)
		CGeometryAllocator::GetInstance()->releaseVertices((float*)texcoords);
	if (NULL != texcoords2)
		CGeometryAllocator::GetInstance()->releaseVertices((float*)texcoords2);
	if (NULL != colors)
		CGeometryAllocator::GetInstance()->releaseVertices((float*)colors);
    if (NULL != tangents)
        CGeometryAllocator::GetInstance()->releaseVertices((float*)tangents);
	if (NULL != binormals)
        CGeometryAllocator::GetInstance()->releaseVertices((float*)binormals);
    if (NULL != weightcoords)
        CGeometryAllocator::GetInstance()->releaseVertices(weightcoords);
    if (NULL != fogcoords)
        CGeometryAllocator::GetInstance()->releaseVertices(fogcoords);

	if (polys != NULL)
		CGeometryAllocator::GetInstance()->releaseIndexes(polys);
#endif	

	if (0 != m_pPrimitives.size())
	{
		vector<CGeometryPrimitive*>::const_iterator itr = m_pPrimitives.begin();
		while (itr != m_pPrimitives.end())
			(*itr++)->releaseReference();
	}

    if (m_pEditor != NULL)
	{
		CGeometryEditor *ed = m_pEditor;
		m_pEditor = NULL;
		ed->destroy();
	}

	if (NULL != m_pBinder)
	{
		CResourceAllocator::CResourceBinder *binder = (CResourceAllocator::CResourceBinder *)m_pBinder;
		delete binder;
	}
}

vector<CObject3DContour*> CGeometry::createContours(void)
{
    vector<CObject3DContour*>  res;
    CObject3DContour *pContour = new CObject3DContour;
    res.push_back(pContour);

    pContour->init(this);

    return res;
}

//////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////
CGeometry& CGeometry::operator=(const CGeometry &geo)
{
    if (&geo == this)
        return *this;

    m_nbVertex = geo.m_nbVertex;
	m_nbPolys = geo.m_nbPolys;

#if defined(DATA_EXTENDED)

    if (geometry != NULL) delete [] geometry;
    geometry = new GL_VERTEX_DATA[nbVertex];

	for (unsigned int i=0;i<geo.m_nbVertex;i++)
	    geometry[i] = geo.geometry[i];

#elif defined(DATA_PACKED)

    CGeometryAllocator *pAllocator = CGeometryAllocator::GetInstance();

    //! Free previous geometry data. 
    //! Data cannot be pointed to, because it would be necessary to manage relative or actual data.
    //! If geometries are identical, one should better use instances of the same geometry.
	pAllocator->releaseVertices((float*)vertex);
	pAllocator->releaseVertices((float*)normals);
	pAllocator->releaseVertices((float*)texcoords);
	pAllocator->releaseVertices((float*)texcoords2);
	pAllocator->releaseVertices((float*)colors);
    if (NULL != polys) pAllocator->releaseIndexes(polys);

	pAllocator->releaseVertices(fogcoords);
	pAllocator->releaseVertices(weightcoords);
	pAllocator->releaseVertices((float*)tangents);
	pAllocator->releaseVertices((float*)binormals);

    //! Allocate new data.
	vertex = (GL_COORD_VERTEX*)(pAllocator->allocateVertices(m_nbVertex*4));
	normals = (GL_COORD_VERTEX*)(pAllocator->allocateVertices(m_nbVertex*4));
    colors = NULL;
    texcoords = NULL;
	tangents = NULL;
	binormals = NULL;

	CResourceAllocator::CResourceBinder *binder = (CResourceAllocator::CResourceBinder *)m_pBinder;
	binder->setArray(CProgramParameters::POSITION, vertex);
	binder->setArray(CProgramParameters::NORMAL, normals);

    //! Transfer normals and vertex data.
    GL_COORD_VERTEX* geo_vertex = geo.vertex;
    GL_COORD_VERTEX* geo_normals = geo.normals;
	pAllocator->glvkCopyPointer((float*)vertex, (float*)geo.vertex, m_nbVertex * 4);
	pAllocator->glvkCopyPointer((float*)normals, (float*)geo.normals, m_nbVertex * 4);

    //! Transfer texture coordinates
    if (geo.texcoords != NULL)
	{
		texcoords = (GL_TEX_VERTEX*)(pAllocator->allocateVertices(m_nbVertex*2));
		binder->setArray(CProgramParameters::TEXCOORD0, texcoords);
		pAllocator->glvkCopyPointer((float*)texcoords, (float*)geo.texcoords, m_nbVertex * 2);
	}

	//! Transfer additional texture coordinates
    if (geo.texcoords2 != NULL)
	{
		texcoords2 = (GL_TEX_VERTEX*)(pAllocator->allocateVertices(m_nbVertex*2));
        binder->setArray(CProgramParameters::TEXCOORD1, texcoords2);
		pAllocator->glvkCopyPointer((float*)texcoords2, (float*)geo.texcoords2, m_nbVertex * 2);
	}

    //! Transfer colors.
    if (geo.colors != NULL) 
	{
		colors = (CColor::RGBA*)(pAllocator->allocateVertices(m_nbVertex*4));
		binder->setArray(CProgramParameters::PRIMARY_COLOR, colors);
		pAllocator->glvkCopyPointer((float*)colors, (float*)geo.colors, m_nbVertex * 4);
	}
#endif


    //! Transfer polygon informations
	polys = pAllocator->allocateIndexes(3 * m_nbPolys);
	pAllocator->glvkCopyPointer(polys, geo.polys, m_nbPolys * 3);
	

	if (geo.fogcoords != NULL)
	{
		fogcoords = pAllocator->allocateVertices(m_nbVertex);
		binder->setArray(CProgramParameters::FOG_COORDINATE, fogcoords);
		pAllocator->glvkCopyPointer((float*)fogcoords, (float*)geo.fogcoords, m_nbVertex);
	}

	if (geo.weightcoords != NULL)
	{
		weightcoords = pAllocator->allocateVertices(m_nbVertex);
		binder->setArray(CProgramParameters::WEIGHTS, weightcoords);
		pAllocator->glvkCopyPointer((float*)weightcoords, (float*)geo.weightcoords, m_nbVertex);
	}

	m_renderingModel = geo.m_renderingModel;

	if (geo.m_pPrimitives.size() > 0)
	{
		for (size_t i=0; i<m_pPrimitives.size(); i++)
			m_pPrimitives[i]->releaseReference();

		m_pPrimitives.clear();

		for (size_t i=0; i<geo.m_pPrimitives.size(); i++)
		{
			CGeometryPrimitive* prim = geo.m_pPrimitives[i];
			prim->addReference();
			m_pPrimitives.push_back(prim);
		}
	}

    CObject3D::operator =(geo);

	return *this;
}




//////////////////////////////////////////////////////////////////////
// Movement
//////////////////////////////////////////////////////////////////////
void CGeometry::scale(float sx,float sy,float sz)
{
	scaleBoundingBox(sx,sy,sz);
	GL_COORD_VERTEX Center;
	boundingBox()->getCenter(Center);

#if defined(DATA_PACKED)
    if (CGeometryAllocator::GetInstance()->isMemoryRelocated())
        vertex = (GL_COORD_VERTEX*)(CGeometryAllocator::GetInstance()->glvkMapPointer((float*)vertex));
#endif

	for(unsigned int i=0;i<m_nbVertex;i++)
	{
		VERTEX(i).x = (VERTEX(i).x - Center.x)*sx + Center.x;
		VERTEX(i).y = (VERTEX(i).y - Center.y)*sy + Center.y;
		VERTEX(i).z = (VERTEX(i).z - Center.z)*sz + Center.z;
	}

#if defined(DATA_PACKED)
    if (CGeometryAllocator::GetInstance()->isMemoryRelocated())
        vertex = (GL_COORD_VERTEX*)(CGeometryAllocator::GetInstance()->glvkUnMapPointer((float*)vertex));
#endif
}

void CGeometry::translateAbsolute(float tx,float ty,float tz)
{
	GL_COORD_VERTEX c;
	getCenter(c);

	translateCenter(tx - c.x,ty - c.y,tz - c.z);

#if defined(DATA_PACKED)
    if (CGeometryAllocator::GetInstance()->isMemoryRelocated())
        vertex = (GL_COORD_VERTEX*)(CGeometryAllocator::GetInstance()->glvkMapPointer((float*)vertex));
#endif

	for(unsigned int i=0;i<m_nbVertex;i++)
	{
		VERTEX(i).x += tx - c.x;
		VERTEX(i).y += ty - c.y;
		VERTEX(i).z += tz - c.z;
	}

#if defined(DATA_PACKED)
    if (CGeometryAllocator::GetInstance()->isMemoryRelocated())
        vertex = (GL_COORD_VERTEX*)(CGeometryAllocator::GetInstance()->glvkUnMapPointer((float*)vertex));
#endif
}

void CGeometry::translate(float tx,float ty,float tz)
{
	translateCenter(tx,ty,tz);

#if defined(DATA_PACKED)
    if (CGeometryAllocator::GetInstance()->isMemoryRelocated())
        vertex = (GL_COORD_VERTEX*)(CGeometryAllocator::GetInstance()->glvkMapPointer((float*)vertex));
#endif

	for(unsigned int i=0;i<m_nbVertex;i++)
	{
		VERTEX(i).x += tx;
		VERTEX(i).y += ty;
		VERTEX(i).z += tz;
	}

#if defined(DATA_PACKED)
    if (CGeometryAllocator::GetInstance()->isMemoryRelocated())
        vertex = (GL_COORD_VERTEX*)(CGeometryAllocator::GetInstance()->glvkUnMapPointer((float*)vertex));
#endif
}

void CGeometry::rotationX(float rx)
{
	float c = (float)cos(TO_RADIAN(rx));
	float s = (float)sin(TO_RADIAN(rx));
	float y,z,Y,Z;

    GL_COORD_VERTEX Min;
    GL_COORD_VERTEX Max;
    GL_COORD_VERTEX Center;

	getCenter(Center);
	getBoundingBox(Min,Max);
	Min.y = FLT_MAX;
	Max.y = -FLT_MAX;
	Min.z = FLT_MAX;
	Max.z = -FLT_MAX;

#if defined(DATA_PACKED)
    if (CGeometryAllocator::GetInstance()->isMemoryRelocated())
    {
        vertex = (GL_COORD_VERTEX*)(CGeometryAllocator::GetInstance()->glvkMapPointer((float*)vertex));
        normals = (GL_COORD_VERTEX*)(CGeometryAllocator::GetInstance()->glvkMapPointer((float*)normals));
    }
#endif

	for (unsigned int i=0;i<m_nbVertex;i++)
	{
		y = VERTEX(i).y - Center.y;
		z = VERTEX(i).z - Center.z;
		VERTEX(i).y = Y = c*y - s*z + Center.y;
		VERTEX(i).z = Z = s*y + c*z + Center.z;

		if (Y<Min.y) Min.y = Y; else if (Y>Max.y) Max.y = Y;
		if (Z<Min.z) Min.z = Z; else if (Z>Max.z) Max.z = Z;

		y = NORMAL(i).y;
		z = NORMAL(i).z;
		NORMAL(i).y = c*y - s*z;
		NORMAL(i).z = s*y + c*z;
	}

#if defined(DATA_PACKED)
    if (CGeometryAllocator::GetInstance()->isMemoryRelocated())
    {
        vertex = (GL_COORD_VERTEX*)(CGeometryAllocator::GetInstance()->glvkUnMapPointer((float*)vertex));
        normals = (GL_COORD_VERTEX*)(CGeometryAllocator::GetInstance()->glvkUnMapPointer((float*)normals));
    }
#endif

	setBoundingBox(Min,Max);
}

void CGeometry::rotationY(float ry)
{
	float c=(float)cos(TO_RADIAN(ry));
	float s=(float)sin(TO_RADIAN(ry));
	float x,z,X,Z;

	GL_COORD_VERTEX Min;
    GL_COORD_VERTEX Max;
    GL_COORD_VERTEX Center;

	getCenter(Center);
	getBoundingBox(Min,Max);
	Min.x = FLT_MAX;
	Max.x = -FLT_MAX;
	Min.z = FLT_MAX;
	Max.z = -FLT_MAX;

#if defined(DATA_PACKED)
    if (CGeometryAllocator::GetInstance()->isMemoryRelocated())
    {
        vertex = (GL_COORD_VERTEX*)(CGeometryAllocator::GetInstance()->glvkMapPointer((float*)vertex));
        normals = (GL_COORD_VERTEX*)(CGeometryAllocator::GetInstance()->glvkMapPointer((float*)normals));
    }
#endif

	for (unsigned int i=0;i<m_nbVertex;i++)
	{
		x = VERTEX(i).x - Center.x;
		z = VERTEX(i).z - Center.z;
		VERTEX(i).x = X = c*x - s*z + Center.x;
		VERTEX(i).z = Z = s*x + c*z+ Center.z;

		if (X<Min.x) Min.x = X; else if (X>Max.x) Max.x = X;
		if (Z<Min.z) Min.z = Z; else if (Z>Max.z) Max.z = Z;

		x = NORMAL(i).x;
		z = NORMAL(i).z;
		NORMAL(i).x = c*x - s*z;
		NORMAL(i).z = s*x + c*z;
	}

#if defined(DATA_PACKED)
    if (CGeometryAllocator::GetInstance()->isMemoryRelocated())
    {
        vertex = (GL_COORD_VERTEX*)(CGeometryAllocator::GetInstance()->glvkUnMapPointer((float*)vertex));
        normals = (GL_COORD_VERTEX*)(CGeometryAllocator::GetInstance()->glvkUnMapPointer((float*)normals));
    }
#endif

	setBoundingBox(Min,Max);
}

void CGeometry::rotationZ(float rz)
{
	float c = (float)cos(TO_RADIAN(rz));
	float s = (float)sin(TO_RADIAN(rz));
	float x,y,X,Y;
	
    GL_COORD_VERTEX Min;
    GL_COORD_VERTEX Max;
    GL_COORD_VERTEX Center;

	getCenter(Center);
	getBoundingBox(Min,Max);
	Min.x = FLT_MAX;
	Max.x = -FLT_MAX;
	Min.y = FLT_MAX;
	Max.y = -FLT_MAX;

#if defined(DATA_PACKED)
    if (CGeometryAllocator::GetInstance()->isMemoryRelocated())
    {
        vertex = (GL_COORD_VERTEX*)(CGeometryAllocator::GetInstance()->glvkMapPointer((float*)vertex));
        normals = (GL_COORD_VERTEX*)(CGeometryAllocator::GetInstance()->glvkMapPointer((float*)normals));
    }
#endif

	for (unsigned int i=0;i<m_nbVertex;i++)
	{
		x = VERTEX(i).x - Center.x;
		y = VERTEX(i).y - Center.y;
		VERTEX(i).x = X = c*x - s*y + Center.x;
		VERTEX(i).y = Y = s*x + c*y + Center.y;

		if (X<Min.x) Min.x = X; else if (X>Max.x) Max.x = X;
		if (Y<Min.y) Min.y = Y; else if (Y>Max.y) Max.y = Y;

		x = NORMAL(i).x;
		y = NORMAL(i).y;
		NORMAL(i).x = c*x - s*y;
		NORMAL(i).y = s*x + c*y;
	}

#if defined(DATA_PACKED)
    if (CGeometryAllocator::GetInstance()->isMemoryRelocated())
    {
        vertex = (GL_COORD_VERTEX*)(CGeometryAllocator::GetInstance()->glvkUnMapPointer((float*)vertex));
        normals = (GL_COORD_VERTEX*)(CGeometryAllocator::GetInstance()->glvkUnMapPointer((float*)normals));
    }
#endif

	setBoundingBox(Min,Max);
}

void CGeometry::rotationX(float rx,float cy,float cz)
{
	float c=(float)cos(TO_RADIAN(rx));
	float s=(float)sin(TO_RADIAN(rx));
	float y,z,Y,Z;
	
    GL_COORD_VERTEX Min;
    GL_COORD_VERTEX Max;
	getBoundingBox(Min,Max);
	Min.y = FLT_MAX;
	Max.y = -FLT_MAX;
	Min.z = FLT_MAX;
	Max.z = -FLT_MAX;

#if defined(DATA_PACKED)
    if (CGeometryAllocator::GetInstance()->isMemoryRelocated())
    {
        vertex = (GL_COORD_VERTEX*)(CGeometryAllocator::GetInstance()->glvkMapPointer((float*)vertex));
        normals = (GL_COORD_VERTEX*)(CGeometryAllocator::GetInstance()->glvkMapPointer((float*)normals));
    }
#endif

	for (unsigned int i=0;i<m_nbVertex;i++)
	{
		y = VERTEX(i).y - cy;
		z = VERTEX(i).z - cz;
		VERTEX(i).y = Y = c*y - s*z + cy;
		VERTEX(i).z = Z = s*y + c*z + cz;

		if (Y<Min.y) Min.y = Y; else if (Y>Max.y) Max.y = Y;
		if (Z<Min.z) Min.z = Z; else if (Z>Max.z) Max.z = Z;

		y = NORMAL(i).y;
		z = NORMAL(i).z;
		NORMAL(i).y = c*y - s*z;
		NORMAL(i).z = s*y + c*z;
	}

#if defined(DATA_PACKED)
    if (CGeometryAllocator::GetInstance()->isMemoryRelocated())
    {
        vertex = (GL_COORD_VERTEX*)(CGeometryAllocator::GetInstance()->glvkUnMapPointer((float*)vertex));
        normals = (GL_COORD_VERTEX*)(CGeometryAllocator::GetInstance()->glvkUnMapPointer((float*)normals));
    }
#endif

	setBoundingBox(Min,Max);
}

void CGeometry::rotationY(float ry,float cx,float cz)
{
	float c=(float)cos(TO_RADIAN(ry));
	float s=(float)sin(TO_RADIAN(ry));
	float x,z,X,Z;
	
    GL_COORD_VERTEX Min;
    GL_COORD_VERTEX Max;
	getBoundingBox(Min,Max);

	Min.x = FLT_MAX;
	Max.x = -FLT_MAX;
	Min.z = FLT_MAX;
	Max.z = -FLT_MAX;

#if defined(DATA_PACKED)
    if (CGeometryAllocator::GetInstance()->isMemoryRelocated())
    {
        vertex = (GL_COORD_VERTEX*)(CGeometryAllocator::GetInstance()->glvkMapPointer((float*)vertex));
        normals = (GL_COORD_VERTEX*)(CGeometryAllocator::GetInstance()->glvkMapPointer((float*)normals));
    }
#endif

	for (unsigned int i=0;i<m_nbVertex;i++)
	{
		x = VERTEX(i).x - cx;
		z = VERTEX(i).z - cz;
		VERTEX(i).x = X = c*x - s*z + cx;
		VERTEX(i).z = Z = s*x + c*z + cz;

		if (X<Min.x) Min.x = X; else if (X>Max.x) Max.x = X;
		if (Z<Min.z) Min.z = Z; else if (Z>Max.z) Max.z = Z;

		x = NORMAL(i).x;
		z = NORMAL(i).z;
		NORMAL(i).x = c*x - s*z;
		NORMAL(i).z = s*x + c*z;
	}

#if defined(DATA_PACKED)
    if (CGeometryAllocator::GetInstance()->isMemoryRelocated())
    {
        vertex = (GL_COORD_VERTEX*)(CGeometryAllocator::GetInstance()->glvkUnMapPointer((float*)vertex));
        normals = (GL_COORD_VERTEX*)(CGeometryAllocator::GetInstance()->glvkUnMapPointer((float*)normals));
    }
#endif

	setBoundingBox(Min,Max);
}

void CGeometry::rotationZ(float rz,float cx,float cy)
{
	float c=(float)cos(TO_RADIAN(rz));
	float s=(float)sin(TO_RADIAN(rz));
	float x,y,X,Y;

    GL_COORD_VERTEX Min;
    GL_COORD_VERTEX Max;
	getBoundingBox(Min,Max);

	Min.x = FLT_MAX;
	Max.x = -FLT_MAX;
	Min.y = FLT_MAX;
	Max.y = -FLT_MAX;

#if defined(DATA_PACKED)
    if (CGeometryAllocator::GetInstance()->isMemoryRelocated())
    {
        vertex = (GL_COORD_VERTEX*)(CGeometryAllocator::GetInstance()->glvkMapPointer((float*)vertex));
        normals = (GL_COORD_VERTEX*)(CGeometryAllocator::GetInstance()->glvkMapPointer((float*)normals));
    }
#endif

	for (unsigned int i=0;i<m_nbVertex;i++)
	{
		x = VERTEX(i).x - cx;
		y = VERTEX(i).y - cy;
		VERTEX(i).x = X = c*x - s*y + cx;
		VERTEX(i).y = Y = s*x + c*y + cy;

		if (X<Min.x) Min.x = X; else if (X>Max.x) Max.x = X;
		if (Y<Min.y) Min.y = Y; else if (Y>Max.y) Max.y = Y;

		x = NORMAL(i).x;
		y = NORMAL(i).y;
		NORMAL(i).x = c*x - s*y;
		NORMAL(i).y = s*x + c*y;
	}

#if defined(DATA_PACKED)
    if (CGeometryAllocator::GetInstance()->isMemoryRelocated())
    {
        vertex = (GL_COORD_VERTEX*)(CGeometryAllocator::GetInstance()->glvkUnMapPointer((float*)vertex));
        normals = (GL_COORD_VERTEX*)(CGeometryAllocator::GetInstance()->glvkUnMapPointer((float*)normals));
    }
#endif

	setBoundingBox(Min,Max);
}

void CGeometry::rotation(double angle,float ax,float ay,float az)
{
	CGenericMatrix<float> temp;

	CGenericVector<float> axis(ax,ay,az,1.0f);
	C3DEngine::Get3DEngine()->generateRotation(angle,axis,temp);

	float zmin = FLT_MAX;
	float zmax = -FLT_MAX;
	float xmin = FLT_MAX;
	float xmax = -FLT_MAX;
	float ymin = FLT_MAX;;
	float ymax = -FLT_MAX;

#if defined(DATA_PACKED)
    if (CGeometryAllocator::GetInstance()->isMemoryRelocated())
    {
        vertex = (GL_COORD_VERTEX*)(CGeometryAllocator::GetInstance()->glvkMapPointer((float*)vertex));
        normals = (GL_COORD_VERTEX*)(CGeometryAllocator::GetInstance()->glvkMapPointer((float*)normals));
    }
#endif

	for (unsigned int i=0;i<m_nbVertex;i++)
	{
		axis.Set(VERTEX(i).x,VERTEX(i).y,VERTEX(i).z,1.0f);
		axis *= temp;

		VERTEX(i).x = axis.X();
		VERTEX(i).y = axis.Y();
		VERTEX(i).z = axis.Z();

		if (axis.X()<xmin) xmin = axis.X(); else if (axis.X()>xmax) xmax = axis.X();
		if (axis.Y()<ymin) ymin = axis.Y(); else if (axis.Y()>ymax) ymax = axis.Y();
		if (axis.Z()<zmin) zmin = axis.Z(); else if (axis.Z()>zmax) zmax = axis.Z();

		axis.Set(NORMAL(i).x,NORMAL(i).y,NORMAL(i).z,0.0f);
		axis *= temp;

		NORMAL(i).x = axis.X();
		NORMAL(i).y = axis.Y();
		NORMAL(i).z = axis.Z();
	}

#if defined(DATA_PACKED)
    if (CGeometryAllocator::GetInstance()->isMemoryRelocated())
    {
        vertex = (GL_COORD_VERTEX*)(CGeometryAllocator::GetInstance()->glvkUnMapPointer((float*)vertex));
        normals = (GL_COORD_VERTEX*)(CGeometryAllocator::GetInstance()->glvkUnMapPointer((float*)normals));
    }
#endif

	setBoundingBox(GL_COORD_VERTEX(xmin,ymin,zmin,1.0f),GL_COORD_VERTEX(xmax,ymax,zmax,1.0f));
}

void CGeometry::transform(GL_MATRIX &m)
{
	CGenericMatrix<float> temp;
	CGenericVector<float> axis;

	float zmin = FLT_MAX;
	float zmax = -FLT_MAX;
	float xmin = FLT_MAX;
	float xmax = -FLT_MAX;
	float ymin = FLT_MAX;
	float ymax = -FLT_MAX;

    temp = m;

#if defined(DATA_PACKED)
    if (CGeometryAllocator::GetInstance()->isMemoryRelocated())
    {
        vertex = (GL_COORD_VERTEX*)(CGeometryAllocator::GetInstance()->glvkMapPointer((float*)vertex));
        normals = (GL_COORD_VERTEX*)(CGeometryAllocator::GetInstance()->glvkMapPointer((float*)normals));
    }
#endif

	for (unsigned int i=0;i<m_nbVertex;i++)
	{
		axis.Set(VERTEX(i).x,VERTEX(i).y,VERTEX(i).z,1.0f);
		axis *= temp;

		VERTEX(i).x = axis.X();
		VERTEX(i).y = axis.Y();
		VERTEX(i).z = axis.Z();

		if (axis.X()<xmin) xmin = axis.X(); else if (axis.X()>xmax) xmax = axis.X();
		if (axis.Y()<ymin) ymin = axis.Y(); else if (axis.Y()>ymax) ymax = axis.Y();
		if (axis.Z()<zmin) zmin = axis.Z(); else if (axis.Z()>zmax) zmax = axis.Z();

		axis.Set(NORMAL(i).x,NORMAL(i).y,NORMAL(i).z,0.0f);
		axis *= temp;

		NORMAL(i).x = axis.X();
		NORMAL(i).y = axis.Y();
		NORMAL(i).z = axis.Z();
	}

#if defined(DATA_PACKED)
    if (CGeometryAllocator::GetInstance()->isMemoryRelocated())
    {
        vertex = (GL_COORD_VERTEX*)(CGeometryAllocator::GetInstance()->glvkUnMapPointer((float*)vertex));
        normals = (GL_COORD_VERTEX*)(CGeometryAllocator::GetInstance()->glvkUnMapPointer((float*)normals));
    }
#endif

	setBoundingBox(GL_COORD_VERTEX(xmin,ymin,zmin,1.0f),GL_COORD_VERTEX(xmax,ymax,zmax,1.0f));
}

//////////////////////////////////////////////////////////////////////
// Rendering
//////////////////////////////////////////////////////////////////////
void CGeometry::vkRender(CVulkanCommandBuffer& commandBuffer,
						 VkBuffer vertexBinding,
						 VkBuffer indexBinding)
{
	VkBuffer bindings[3] = { vertexBinding, 0, 0 };
	VkDeviceSize offsets[3] = { (VkDeviceSize)&vertex[0], 0, 0 };
	uint32_t nb_bindings = 1;	// always extract geometry

	if (hasModel(CGeometry::CGL_TEXTURE))
	{
		bindings[nb_bindings] = vertexBinding;
		offsets[nb_bindings] = (VkDeviceSize)&texcoords[0];
		nb_bindings++;
	}

	if (hasModel(CGeometry::CGL_COLORS))
	{
		bindings[nb_bindings] = vertexBinding;
		offsets[nb_bindings] = (VkDeviceSize)&colors[0];
		nb_bindings++;
	}

	commandBuffer.vkCmdBindVertexBuffers(commandBuffer.commandBuffer, 
										 0, nb_bindings, bindings, offsets);

	commandBuffer.vkCmdBindIndexBuffer(	commandBuffer.commandBuffer,
										indexBinding,
										(VkDeviceSize)&polys[0],
										VK_INDEX_TYPE_UINT16);

	commandBuffer.vkCmdDrawIndexed(commandBuffer.commandBuffer, 3 * m_nbPolys, 1, 0, 0, 0);
}

bool CGeometry::getVertexInputState( std::vector<VkVertexInputBindingDescription>& bindings,
									 std::vector<VkVertexInputAttributeDescription>& vertexInput) const
{
	bindings.clear();
	vertexInput.clear();
	uint32_t nb_bindings = 0;

	//!	Vertex
	bindings.push_back({ nb_bindings, 4 * sizeof(float), VK_VERTEX_INPUT_RATE_VERTEX });
	vertexInput.push_back({ 0, 0, VK_FORMAT_R32G32B32A32_SFLOAT, 0 });
	nb_bindings++; 	// always extract geometry

	//!	TexCoords
	IRenderingProperties *props = IRenderingProperties::GetCurrentProperties();
	if ((hasModel(CGeometry::CGL_TEXTURE)))
//		(props->getCurrentTexturing() == IRenderingProperties::ENABLE))
	{
		bindings.push_back({ nb_bindings, 2 * sizeof(float), VK_VERTEX_INPUT_RATE_VERTEX });
		vertexInput.push_back({ CProgramParameters::TEXCOORD0, nb_bindings, VK_FORMAT_R32G32_SFLOAT, 0 });
		nb_bindings++;
	}

	//!	Colors
	if (hasModel(CGeometry::CGL_COLORS))
	{
		bindings.push_back({ nb_bindings, 4 * sizeof(float), VK_VERTEX_INPUT_RATE_VERTEX });
		vertexInput.push_back({ CProgramParameters::PRIMARY_COLOR, nb_bindings, VK_FORMAT_R32G32B32A32_SFLOAT, 0 });
		nb_bindings++;
	}

	return true;
}

void CGeometry::glRender()
{
    if (!properties.isVisible())
		return;

    glRenderGeometry();
}


#if defined(DATA_EXTENDED)

#elif defined(DATA_PACKED)

void CGeometry::glRenderGeometry()
{
#ifdef RAPTOR_DEBUG_MODE_GENERATION
	if (m_bUpdateBBox)
	{
		Raptor::GetErrorManager()->generateRaptorError(CGeometry::CGeometryClassID::GetClassId(),
													   CRaptorErrorManager::RAPTOR_WARNING,
													   "A geometry is requested for rendering with an unfinished bounding box!");
		glLockData();
		glUnLockData();
	}
#endif

	const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();


	//	Store arrays state + texture state
	IRenderingProperties *props = IRenderingProperties::GetCurrentProperties();
	bool popNormalArray = false;
	bool popTangentArray = false;
	bool popBinormalArray = false;
	bool popColorArray = false;
	bool popTexCoordArray = false;
	bool popWeightArray = false;
	bool popFogArray = false;
	bool proceedLighting = (props->getCurrentLighting() == IRenderingProperties::ENABLE);
	bool proceedTexturing = (props->getCurrentTexturing() == IRenderingProperties::ENABLE);


	if (hasModel(CGeometry::CGL_BACK_GEOMETRY))
		glCullFace(GL_FRONT);

	CResourceAllocator::CResourceBinder *binder = (CResourceAllocator::CResourceBinder *)m_pBinder;

	// always extract geometry
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, sizeof(GL_COORD_VERTEX), vertex);

	// extract normals
	if (hasModel(CGeometry::CGL_NORMALS) && proceedLighting && (NULL != normals))
	{
		popNormalArray = true;
		pExtensions->glEnableVertexAttribArrayARB(CProgramParameters::NORMAL);
		pExtensions->glVertexAttribPointerARB(CProgramParameters::NORMAL, 4, GL_FLOAT, false, 0, normals);
	}

	// extract tangents
#if defined(GL_ARB_vertex_program)
	if (hasModel(CGeometry::CGL_TANGENTS) && proceedLighting && (NULL != tangents))
	{
		popTangentArray = true;
		pExtensions->glEnableVertexAttribArrayARB(CProgramParameters::ADDITIONAL_PARAM1);
		pExtensions->glVertexAttribPointerARB(CProgramParameters::ADDITIONAL_PARAM1, 4, GL_FLOAT, false, 0, tangents);
	}
	if (hasModel(CGeometry::CGL_BINORMALS) && proceedLighting && (NULL != binormals))
	{
		popBinormalArray = true;
		pExtensions->glEnableVertexAttribArrayARB(CProgramParameters::ADDITIONAL_PARAM2);
		pExtensions->glVertexAttribPointerARB(CProgramParameters::ADDITIONAL_PARAM2, 4, GL_FLOAT, false, 0, binormals);
	}
#endif

	// extract colors
	if (hasModel(CGeometry::CGL_COLORS) && (NULL != colors))
	{
		pExtensions->glEnableVertexAttribArrayARB(CProgramParameters::PRIMARY_COLOR);
		popColorArray = true;
		pExtensions->glVertexAttribPointerARB(CProgramParameters::PRIMARY_COLOR, 4, GL_FLOAT, false, 0, colors);
	}

	// extract texture
	if (hasModel(CGeometry::CGL_TEXTURE) && proceedTexturing && (texcoords != NULL))
	{
		pExtensions->glEnableVertexAttribArrayARB(CProgramParameters::TEXCOORD0);
		popTexCoordArray = true;
		pExtensions->glVertexAttribPointerARB(CProgramParameters::TEXCOORD0, 2, GL_FLOAT, false, 0, texcoords);
	}

	// Vertex weighting is no supported anymore by drivers because
	// shaders enable a more powerfull blending.

	// extract vertex weighting
	if (hasModel(CGeometry::CGL_WEIGHT) && (NULL != weightcoords))
	{
#ifdef GL_EXT_vertex_weighting
		if (pExtensions->glVertexWeightPointerEXT != NULL)
		{
			glEnable(GL_VERTEX_WEIGHTING_EXT);
			glEnableClientState(GL_VERTEX_WEIGHT_ARRAY_EXT);
			popWeightArray = true;
			pExtensions->glVertexWeightPointerEXT(1, GL_FLOAT, 0, weightcoords);
		}
		else
#else
		{
			pExtensions->glEnableVertexAttribArrayARB(CProgramParameters::WEIGHTS);
			popWeightArray = true;
			pExtensions->glVertexAttribPointerARB(CProgramParameters::WEIGHTS, 1, GL_FLOAT, false, 0, weightcoords);
		}
#endif
	}

#ifdef GL_EXT_fog_coord
	//	extract fog coords
	if (hasModel(CGeometry::CGL_FOG))
	{
		if (pExtensions->glFogCoordPointerEXT != NULL)
		{
			glFogi(GL_FOG_COORDINATE_SOURCE_EXT, GL_FOG_COORDINATE_EXT);
			glEnableClientState(GL_FOG_COORDINATE_ARRAY_EXT);
			popFogArray = true;
			pExtensions->glFogCoordPointerEXT(GL_FLOAT, 0, fogcoords);
		}
	}
#endif

	if (m_pPrimitives.size() == 0)
		glDrawElements(GL_TRIANGLES, 3 * m_nbPolys, GL_UNSIGNED_SHORT, polys);
	else
	{
		vector<CGeometryPrimitive*>::const_iterator itr = m_pPrimitives.begin();
		while (itr != m_pPrimitives.end())
		{
			(*itr)->glRender();
			itr++;
		}
	}


	CATCH_GL_ERROR

#ifdef GL_EXT_fog_coord
	//	restore fog model
	if (hasModel(CGeometry::CGL_FOG))
		glFogi(GL_FOG_COORDINATE_SOURCE_EXT, GL_FRAGMENT_DEPTH_EXT);
#endif

	//	Restore status	
	if (hasModel(CGeometry::CGL_BACK_GEOMETRY))
		glCullFace(GL_BACK);


	glDisableClientState(GL_VERTEX_ARRAY);
	if (popNormalArray)
		pExtensions->glDisableVertexAttribArrayARB(CProgramParameters::NORMAL);
	if (popColorArray)
		pExtensions->glDisableVertexAttribArrayARB(CProgramParameters::PRIMARY_COLOR);
	if (popTexCoordArray)
		pExtensions->glDisableVertexAttribArrayARB(CProgramParameters::TEXCOORD0);
#ifdef GL_EXT_vertex_weighting
	if (popWeightArray)
	{
		glDisableClientState(GL_VERTEX_WEIGHT_ARRAY_EXT);
		glDisable(GL_VERTEX_WEIGHTING_EXT);
	}
#else
	if (popWeightArray)
		pExtensions->glDisableVertexAttribArrayARB(CProgramParameters::WEIGHTS);
#endif
#ifdef GL_EXT_fog_coord
	if (popFogArray)
		glDisableClientState(GL_FOG_COORDINATE_ARRAY_EXT);
#endif
#if defined(GL_ARB_vertex_program)
	if (popTangentArray)
		pExtensions->glDisableVertexAttribArrayARB(CProgramParameters::ADDITIONAL_PARAM1);
	if (popBinormalArray)
		pExtensions->glDisableVertexAttribArrayARB(CProgramParameters::ADDITIONAL_PARAM2);
#endif

	CRaptorInstance::GetInstance().iRenderedObjects++;
	CRaptorInstance::GetInstance().iRenderedTriangles += m_nbPolys;
}

#endif


//////////////////////////////////////////////////////////////////////
//	Manual data loading
//////////////////////////////////////////////////////////////////////
CGeometryPrimitive*	CGeometry::createPrimitive(CGeometryPrimitive::PRIMITIVE_KIND kind)
{
    CGeometryPrimitive*	primitive = new CGeometryPrimitive(kind);
    m_pPrimitives.push_back(primitive);

	return primitive;
}

bool CGeometry::removePrimitive(CGeometryPrimitive *primitive)
{
	if (primitive == NULL)
	{
		for (size_t i = 0; i< m_pPrimitives.size(); i++)
			m_pPrimitives[i]->releaseReference();
		m_pPrimitives.clear();
		return true;
	}
	else
	{
		vector<CGeometryPrimitive*>::iterator it = m_pPrimitives.begin();
		while (it != m_pPrimitives.end())
		{
			if (*it == primitive)
			{
				(*it)->releaseReference();
				m_pPrimitives.erase(it);
				return true;
			}
			it++;
		}
		return false;
	}
}

void CGeometry::glSetPolygons(size_t nbP, unsigned short* polygons)
{
	if (polygons == NULL)
	{
		if (nbP * 3 > 65535)
		{
			Raptor::GetErrorManager()->generateRaptorError(CGeometry::CGeometryClassID::GetClassId(),
														   CRaptorErrorManager::RAPTOR_ERROR,
														   CRaptorMessages::ID_NO_RESOURCE);
		}

		if (polys!=NULL)
			CGeometryAllocator::GetInstance()->releaseIndexes(polys);

		polys = CGeometryAllocator::GetInstance()->allocateIndexes(3*nbP);

		m_nbPolys = 0;
	}
	else if ((nbP > 0) && (polys != NULL))
	{
		CGeometryAllocator *pAllocator = CGeometryAllocator::GetInstance();
		if (!pAllocator->isMemoryRelocated() || m_bDataLocked)
			memcpy(polys,polygons,3*nbP*sizeof(unsigned short));
		else
			pAllocator->glvkSetPointerData(polys,polygons,3*nbP);
		m_nbPolys = nbP;
	}
}

void CGeometry::glSetVertices(size_t nbV, GL_COORD_VERTEX* vertices)
{
	if (vertices == NULL)
	{
#if defined(DATA_EXTENDED)
		if (geometry != NULL)
			delete [] geometry;
		geometry = new GL_VERTEX_DATA[nbV];
#elif defined(DATA_PACKED)
		if (vertex != NULL)
			CGeometryAllocator::GetInstance()->releaseVertices((float*)vertex);
		if (normals!=NULL)
			CGeometryAllocator::GetInstance()->releaseVertices((float*)normals);
		
		vertex = (GL_COORD_VERTEX*)(CGeometryAllocator::GetInstance()->allocateVertices(nbV*4));
		normals = (GL_COORD_VERTEX*)(CGeometryAllocator::GetInstance()->allocateVertices(nbV*4));

		CResourceAllocator::CResourceBinder *binder = (CResourceAllocator::CResourceBinder *)m_pBinder;
		binder->setArray(CProgramParameters::POSITION, vertex, 3);
		binder->setArray(CProgramParameters::NORMAL, normals);
#endif

		m_nbVertex = 0;
	}
	else if ((nbV > 0) && (vertex != NULL))
#if defined (DATA_PACKED)
	{
		CGeometryAllocator *pAllocator = CGeometryAllocator::GetInstance();
		if (!pAllocator->isMemoryRelocated() || m_bDataLocked)
			memcpy(vertex,vertices,4*nbV*sizeof(float));
		else
			pAllocator->glvkSetPointerData(*vertex,*vertices,4*nbV);
		m_nbVertex = nbV;
	}
#endif
}

void CGeometry::glSetNormals(size_t nbN, GL_COORD_VERTEX* norms)
{
#if defined (DATA_PACKED)
	if ((norms != NULL) && (nbN > 0) && (normals != NULL))
	{
		CGeometryAllocator *pAllocator = CGeometryAllocator::GetInstance();
		if (!pAllocator->isMemoryRelocated() || m_bDataLocked)
			memcpy(normals,norms,4*nbN*sizeof(float));
		else
			pAllocator->glvkSetPointerData(*normals,*norms,4*nbN);
	}
#endif
}

void CGeometry::glSetTangents(size_t nbT, GL_COORD_VERTEX* tans)
{
#if defined (DATA_PACKED)
	if (tans == NULL)
	{
		if (tangents != NULL)
			CGeometryAllocator::GetInstance()->releaseVertices((float*)tangents);

		tangents = (GL_COORD_VERTEX*)(CGeometryAllocator::GetInstance()->allocateVertices(nbT * 4));

		CResourceAllocator::CResourceBinder *binder = (CResourceAllocator::CResourceBinder *)m_pBinder;
		binder->setArray(CProgramParameters::ADDITIONAL_PARAM1, tangents);
	}
	else if ((nbT > 0) && (tangents != NULL))
	{
		CGeometryAllocator *pAllocator = CGeometryAllocator::GetInstance();
		if (!pAllocator->isMemoryRelocated() || m_bDataLocked)
			memcpy(tangents, tans, nbT * sizeof(GL_COORD_VERTEX));
		else
			pAllocator->glvkSetPointerData(*tangents, *tans, 4 * nbT);
	}
#endif
}

void CGeometry::glSetBinormals(size_t nbB, GL_COORD_VERTEX* binorms)
{
#if defined (DATA_PACKED)
	if (binorms == NULL)
	{
		if (binormals != NULL)
			CGeometryAllocator::GetInstance()->releaseVertices((float*)binormals);

		binormals = (GL_COORD_VERTEX*)(CGeometryAllocator::GetInstance()->allocateVertices(nbB * 4));

		CResourceAllocator::CResourceBinder *binder = (CResourceAllocator::CResourceBinder *)m_pBinder;
		binder->setArray(CProgramParameters::ADDITIONAL_PARAM2, binormals);
	}
	else if ((nbB > 0) && (binormals != NULL))
	{
		CGeometryAllocator *pAllocator = CGeometryAllocator::GetInstance();
		if (!pAllocator->isMemoryRelocated() || m_bDataLocked)
			memcpy(binormals, binorms, nbB * sizeof(GL_COORD_VERTEX));
		else
			pAllocator->glvkSetPointerData(*binormals, *binorms, 4 * nbB);
	}
#endif
}

void CGeometry::glSetTexCoords(size_t nbT, GL_TEX_VERTEX* texCoords)
{
#if defined (DATA_PACKED)
	if (texCoords == NULL)
	{
		if (texcoords != NULL)
			CGeometryAllocator::GetInstance()->releaseVertices((float*)texcoords);

		texcoords = (GL_TEX_VERTEX*)(CGeometryAllocator::GetInstance()->allocateVertices(nbT*2));

		CResourceAllocator::CResourceBinder *binder = (CResourceAllocator::CResourceBinder *)m_pBinder;
		binder->setArray(CProgramParameters::TEXCOORD0, texcoords, 2);
	}
	else if ((nbT > 0) && (texcoords != NULL))
	{
		CGeometryAllocator *pAllocator = CGeometryAllocator::GetInstance();
		if (!pAllocator->isMemoryRelocated() || m_bDataLocked)
			memcpy(texcoords,texCoords,nbT*sizeof(GL_TEX_VERTEX));
		else
			pAllocator->glvkSetPointerData(*texcoords,*texCoords,2*nbT);
	}
#endif
}

void CGeometry::glSetTexCoords2(size_t nbT, GL_TEX_VERTEX* texCoords)
{
#if defined (DATA_PACKED)
	if (texCoords == NULL)
	{
		if (texcoords2 != NULL)
			CGeometryAllocator::GetInstance()->releaseVertices((float*)texcoords2);

		texcoords2 = (GL_TEX_VERTEX*)(CGeometryAllocator::GetInstance()->allocateVertices(nbT*2));

		CResourceAllocator::CResourceBinder *binder = (CResourceAllocator::CResourceBinder *)m_pBinder;
		binder->setArray(CProgramParameters::TEXCOORD1, texcoords2, 2);
	}
	else if ((nbT > 0) && (texcoords2 != NULL))
	{
		CGeometryAllocator *pAllocator = CGeometryAllocator::GetInstance();
		if (!pAllocator->isMemoryRelocated() || m_bDataLocked)
			memcpy(texcoords2,texCoords,nbT*sizeof(GL_TEX_VERTEX));
		else
			pAllocator->glvkSetPointerData(*texcoords2,*texCoords,2*nbT);
	}
#endif
}

void CGeometry::glSetWeights(size_t nbW, float* weights)
{
#if defined (DATA_PACKED)
	if (weights == NULL)
	{
		if (weightcoords!=NULL)
			CGeometryAllocator::GetInstance()->releaseVertices(weightcoords);

		weightcoords = CGeometryAllocator::GetInstance()->allocateVertices(nbW);

		CResourceAllocator::CResourceBinder *binder = (CResourceAllocator::CResourceBinder *)m_pBinder;
		binder->setArray(CProgramParameters::WEIGHTS, weightcoords, 1);
	}
	else if ((nbW > 0) && (weightcoords != NULL))
	{
		CGeometryAllocator *pAllocator = CGeometryAllocator::GetInstance();
		if (!pAllocator->isMemoryRelocated() || m_bDataLocked)
			memcpy(weightcoords,weights,nbW*sizeof(float));
		else
			pAllocator->glvkSetPointerData(weightcoords,weights,nbW);
	}
#endif
}

void CGeometry::glSetColors(size_t nbC, CColor::RGBA* rgbaColors)
{
#if defined (DATA_PACKED)
	if (rgbaColors == NULL)
	{
		if (colors != NULL)
			CGeometryAllocator::GetInstance()->releaseVertices((float*)colors);

		colors = (CColor::RGBA*)(CGeometryAllocator::GetInstance()->allocateVertices(nbC*4));

		CResourceAllocator::CResourceBinder *binder = (CResourceAllocator::CResourceBinder *)m_pBinder;
		binder->setArray(CProgramParameters::PRIMARY_COLOR, colors);
	}
	else if ((nbC > 0) && (colors != NULL))
	{
		CGeometryAllocator *pAllocator = CGeometryAllocator::GetInstance();
		if (!pAllocator->isMemoryRelocated() || m_bDataLocked)
			memcpy(colors,rgbaColors,4*nbC*sizeof(float));
		else
			pAllocator->glvkSetPointerData(*colors,*rgbaColors,4*nbC);
	}
#endif
}

void CGeometry::glSetFogs(size_t nbF, float* fogs)
{
#if defined (DATA_PACKED)
	if (fogs == NULL)
	{
		if (fogcoords != NULL)
			CGeometryAllocator::GetInstance()->releaseVertices(fogcoords);

		fogcoords = CGeometryAllocator::GetInstance()->allocateVertices(nbF);

		CResourceAllocator::CResourceBinder *binder = (CResourceAllocator::CResourceBinder *)m_pBinder;
		binder->setArray(CProgramParameters::FOG_COORDINATE, weightcoords, 1);
	}
	else if ((nbF > 0) && (fogcoords != NULL))
	{
		CGeometryAllocator *pAllocator = CGeometryAllocator::GetInstance();
		if (!pAllocator->isMemoryRelocated() || m_bDataLocked)
			memcpy(fogcoords, fogs, nbF*sizeof(float));
		else
			pAllocator->glvkSetPointerData(fogcoords, fogs, nbF);
	}
#endif
}

bool CGeometry::glLockData()
{
    CGeometryAllocator *pAllocator = CGeometryAllocator::GetInstance();

    if ((!pAllocator->isMemoryRelocated()) || (m_bDataLocked))
        return false;

    vertex = (GL_COORD_VERTEX*)(pAllocator->glvkMapPointer((float*)vertex));
    normals = (GL_COORD_VERTEX*)(pAllocator->glvkMapPointer((float*)normals));
    
	if (texcoords != NULL)
		texcoords = (GL_TEX_VERTEX*)(pAllocator->glvkMapPointer((float*)texcoords));

	if (texcoords2 != NULL)
		texcoords2 = (GL_TEX_VERTEX*)(pAllocator->glvkMapPointer((float*)texcoords2));
    
	if (colors != NULL)
		colors = (CColor::RGBA*)(pAllocator->glvkMapPointer((float*)colors));

	if (tangents != NULL)
		tangents = (GL_COORD_VERTEX*)(pAllocator->glvkMapPointer((float*)tangents));

	if (binormals != NULL)
		binormals = (GL_COORD_VERTEX*)(pAllocator->glvkMapPointer((float*)binormals));
    
	if (weightcoords != NULL)
		weightcoords = pAllocator->glvkMapPointer(weightcoords);
    
	if (fogcoords != NULL)
		fogcoords = pAllocator->glvkMapPointer(fogcoords);

	if (polys != NULL)
		polys = pAllocator->glvkMapPointer(polys);

    m_bDataLocked = true;

    return true;
}

bool CGeometry::glUnLockData()
{
    CGeometryAllocator *pAllocator = CGeometryAllocator::GetInstance();

    if (!pAllocator->isMemoryRelocated()) 
    {
        if (m_bUpdateBBox)
            updateBBox();
        return true;
    }
    else if (m_bDataLocked)
    {
        if (m_bUpdateBBox)
            updateBBox();

        vertex = (GL_COORD_VERTEX*)(pAllocator->glvkUnMapPointer((float*)vertex));
        normals = (GL_COORD_VERTEX*)(pAllocator->glvkUnMapPointer((float*)normals));

		if (texcoords != NULL)
			texcoords = (GL_TEX_VERTEX*)(pAllocator->glvkUnMapPointer((float*)texcoords));

		if (texcoords2 != NULL)
			texcoords2 = (GL_TEX_VERTEX*)(pAllocator->glvkUnMapPointer((float*)texcoords2));
        
		if (colors != NULL)
			colors = (CColor::RGBA*)(pAllocator->glvkUnMapPointer((float*)colors));

		if (tangents!= NULL)
			tangents = (GL_COORD_VERTEX*)(pAllocator->glvkUnMapPointer((float*)tangents));
        
		if (binormals != NULL)
			binormals = (GL_COORD_VERTEX*)(pAllocator->glvkUnMapPointer((float*)binormals));
        
		if (weightcoords != NULL)
			weightcoords = pAllocator->glvkUnMapPointer(weightcoords);
        
		if (fogcoords != NULL)
			fogcoords = pAllocator->glvkUnMapPointer(fogcoords);

		if (polys != NULL)
			polys = pAllocator->glvkUnMapPointer(polys);

        m_bDataLocked = false;
    
        return true;
    }
    else
        return false;
}

void CGeometry::updateBBox(void)
{
    GL_COORD_VERTEX Min(FLT_MAX,FLT_MAX,FLT_MAX,1.0f);
    GL_COORD_VERTEX Max(-FLT_MAX,-FLT_MAX,-FLT_MAX,1.0f);

    CBoundingBox box;
    box.set(Min,Max);

    for (unsigned int i=0;i<m_nbVertex;i++)
        box.extendTo(VERTEX(i).x,VERTEX(i).y,VERTEX(i).z);
    
    box.get(Min,Max);
    setBoundingBox(Min,Max);

    m_bUpdateBBox = false;
}

void CGeometry::addVertex(float x, float y, float z,float h)
{
#if defined (DATA_EXTENDED)
    if (geometry != NULL)
#elif defined (DATA_PACKED)
	if ((vertex != NULL) && (normals != NULL))
#endif
	{
		VERTEX(m_nbVertex).x = x;
		VERTEX(m_nbVertex).y = y;
		VERTEX(m_nbVertex).z = z;
		VERTEX(m_nbVertex).h = h;
		NORMAL(m_nbVertex).x = 0.0f;
		NORMAL(m_nbVertex).y = 0.0f;
		NORMAL(m_nbVertex).z = 0.0f;
		NORMAL(m_nbVertex).h = 1.0f;
	
        m_bUpdateBBox = true;

		m_nbVertex++;
	}
}

void CGeometry::setCoord(size_t numvtx, GLfloat x, GLfloat y, GLfloat z, GLfloat h)
{
#if defined (DATA_EXTENDED)
    if ((numvtx<m_nbVertex) && (geometry != NULL))
#elif defined (DATA_PACKED)
	if ((numvtx<m_nbVertex) && (vertex != NULL))
#endif
	{
		VERTEX(numvtx).x = x;
		VERTEX(numvtx).y = y;
		VERTEX(numvtx).z = z;
		VERTEX(numvtx).h = h;

        m_bUpdateBBox = true;
	}
}

void CGeometry::setTexCoord(size_t numvtx, float u, float v)
{
#if defined (DATA_EXTENDED)
    if ((numvtx<m_nbVertex) && (geometry != NULL))
#elif defined (DATA_PACKED)
	if ((numvtx<m_nbVertex) && (texcoords != NULL))
#endif
	{
		TEXCOORD0(numvtx).u = u;
		TEXCOORD0(numvtx).v = v;
	}
}

void CGeometry::setTexCoord2(size_t numvtx, float u, float v)
{
#if defined (DATA_EXTENDED)
	if ((numvtx < m_nbVertex) && (geometry != NULL))
#elif defined (DATA_PACKED)
	if ((numvtx < m_nbVertex) && (texcoords2 != NULL))
#endif
	{
		TEXCOORD1(numvtx).u = u;
		TEXCOORD1(numvtx).v = v;
	}
}

void CGeometry::setTangent(size_t numvtx, float x, float y, float z, float h)
{
#if defined (DATA_EXTENDED)
	if ((numvtx < m_nbVertex) && (geometry != NULL))
#elif defined (DATA_PACKED)
	if ((numvtx < m_nbVertex) && (tangents != NULL))
#endif
	{
		TANGENT(numvtx).x = x;
		TANGENT(numvtx).y = y;
		TANGENT(numvtx).z = z;
		TANGENT(numvtx).h = h;
	}
}

void CGeometry::setBinormal(size_t numvtx, float x, float y, float z, float h)
{
#if defined (DATA_EXTENDED)
	if ((numvtx < m_nbVertex) && (geometry != NULL))
#elif defined (DATA_PACKED)
	if ((numvtx < m_nbVertex) && (tangents != NULL))
#endif
	{
		BINORMAL(numvtx).x = x;
		BINORMAL(numvtx).y = y;
		BINORMAL(numvtx).z = z;
		BINORMAL(numvtx).h = h;
	}
}

void CGeometry::addFace(int p1,int p2,int p3)
{
	if (polys != NULL)
	{
		GL_COORD_VERTEX normal;

		polys[3*m_nbPolys] = p1;
		polys[3*m_nbPolys+1] = p2;
		polys[3*m_nbPolys+2] = p3;
		m_nbPolys++;

		if ((p1!=p2)&&(p1!=p3)&&(p2!=p3))
		{
			C3DEngine::TriangleNormal(VERTEX(p1),VERTEX(p2),VERTEX(p3),normal);

			//	smooth shading normal
			NORMAL(p1).x += normal.x;
			NORMAL(p2).x += normal.x;
			NORMAL(p3).x += normal.x;
			NORMAL(p1).y += normal.y;
			NORMAL(p2).y += normal.y;
			NORMAL(p3).y += normal.y;
			NORMAL(p1).z += normal.z;
			NORMAL(p2).z += normal.z;
			NORMAL(p3).z += normal.z;
			NORMAL(p1).h +=1.0f;
			NORMAL(p2).h +=1.0f;
			NORMAL(p3).h +=1.0f;
		}
	}
}

void CGeometry::getFace(size_t numface, unsigned int &p1, unsigned int &p2, unsigned int &p3) const
{
	if ((numface<m_nbPolys) && (polys != NULL))
	{
		p1 = polys[3 * numface];
		p2 = polys[3 * numface+1];
		p3 = polys[3 * numface+2];
	}
	else
	{
		p1 = p2 = p3 = 0;
	}
}

void CGeometry::setWeight(size_t numvtx, float w)
{
#if defined (DATA_EXTENDED)
    if ((numvtx<m_nbVertex) && (geometry != NULL))
#elif defined (DATA_PACKED)
	if ((numvtx<m_nbVertex) && (weightcoords != NULL))
#endif
		WEIGHT(numvtx) = w;
}

float CGeometry::getWeight(size_t numvtx) const
{
#if defined (DATA_EXTENDED)
    if ((numvtx<m_nbVertex) && (geometry != NULL))
#elif defined (DATA_PACKED)
	if ((numvtx<m_nbVertex) && (weightcoords != NULL))
#endif
		return WEIGHT(numvtx);
	else
		return 0.0f;
}


void CGeometry::setFogCoord(size_t numvtx, GLfloat f)
{
#if defined (DATA_EXTENDED)
    if ((numvtx<m_nbVertex) && (geometry != NULL))
#elif defined (DATA_PACKED)
	if ((numvtx<m_nbVertex) && (fogcoords != NULL))
#endif
		FOGCOORD(numvtx) = f;
}

float CGeometry::getFogCoord(size_t numvtx) const
{
#if defined (DATA_EXTENDED)
    if ((numvtx<m_nbVertex) && (geometry != NULL))
#elif defined (DATA_PACKED)
	if ((numvtx<m_nbVertex) && (fogcoords != NULL))
#endif
		return FOGCOORD(numvtx);
	else
		return 0.0f;
}

void CGeometry::getCoord(size_t numvtx, GL_COORD_VERTEX &v) const
{
#if defined (DATA_EXTENDED)
    if ((numvtx<m_nbVertex) && (geometry != NULL))
#elif defined (DATA_PACKED)
	if ((numvtx<m_nbVertex) && (vertex != NULL))
#endif
		v = VERTEX(numvtx);
}

void CGeometry::getVertex(size_t numvtx, GL_VERTEX_DATA &v) const
{
#if defined (DATA_EXTENDED)
    if ((numvtx<m_nbVertex) && (geometry != NULL))
        v = geometry[numvtx];
#elif defined (DATA_PACKED)
	if (numvtx<m_nbVertex)
	{
		if (vertex != NULL)
			v.vertex = vertex[numvtx];
		if (normals != NULL)
			v.normal = normals[numvtx];
		if (colors != NULL)
			v.color = colors[numvtx];
		if (texcoords != NULL)
		{
			v.texCoord0.u = texcoords[numvtx].u;
			v.texCoord0.v = texcoords[numvtx].v;
		}
        if (tangents != NULL)
            v.tangent = tangents[numvtx];
        if (binormals != NULL)
            v.binormal = binormals[numvtx];
		if (weightcoords != NULL)
			v.weight = weightcoords[numvtx];
        if (fogcoords != NULL)
			v.fog = fogcoords[numvtx];
	}
#endif
}

void CGeometry::setNormal(size_t numvtx, GLfloat x, GLfloat y, GLfloat z, GLfloat h)
{
#if defined (DATA_EXTENDED)
    if ((numvtx<m_nbVertex) && (geometry != NULL))
#elif defined (DATA_PACKED)
	if ((numvtx<m_nbVertex) && (normals != NULL))
#endif
	{
		NORMAL(numvtx).x = x;
		NORMAL(numvtx).y = y;
		NORMAL(numvtx).z = z;
		NORMAL(numvtx).h = h;
	}
}

void CGeometry::getNormal(size_t numvtx, GL_COORD_VERTEX &v) const
{
#if defined (DATA_EXTENDED)
    if ((numvtx<m_nbVertex) && (geometry != NULL))
#elif defined (DATA_PACKED)
	if ((numvtx<m_nbVertex) && (normals != NULL))
#endif
		v = NORMAL(numvtx);
}

void CGeometry::getTangent(size_t numvtx, GL_COORD_VERTEX &v) const
{
#if defined (DATA_EXTENDED)
    if ((numvtx<m_nbVertex) && (geometry != NULL))
#elif defined (DATA_PACKED)
	if ((numvtx<m_nbVertex) && (tangents != NULL))
#endif
		v = TANGENT(numvtx);
}

void CGeometry::getBiNormal(size_t numvtx, GL_COORD_VERTEX &v) const
{
#if defined (DATA_EXTENDED)
    if ((numvtx<m_nbVertex) && (geometry != NULL))
#elif defined (DATA_PACKED)
	if ((numvtx<m_nbVertex) && (binormals != NULL))
#endif
		v = BINORMAL(numvtx);
}

void CGeometry::setColor(size_t numvtx, float r, float g, float b, float a)
{
#if defined (DATA_EXTENDED)
    if ((numvtx<m_nbVertex) && (geometry != NULL))
#elif defined (DATA_PACKED)
	if ((numvtx<m_nbVertex) && (colors != NULL))
#endif
	{
		COLOR(numvtx).r = r;
		COLOR(numvtx).g = g;
		COLOR(numvtx).b = b;
		COLOR(numvtx).a = a;
	}
}

void CGeometry::getColor(size_t numvtx, CColor::RGBA &v) const
{
#if defined (DATA_EXTENDED)
    if ((numvtx<m_nbVertex) && (geometry != NULL))
#elif defined (DATA_PACKED)
	if ((numvtx<m_nbVertex) && (colors != NULL))
#endif
		v = COLOR(numvtx);
}


//////////////////////////////////////////////////////////////////////
//	Streaming
//////////////////////////////////////////////////////////////////////
bool CGeometry::exportObject(CRaptorIO& o)
{
	CPersistence::exportObject(o);
	

	GL_COORD_VERTEX Min;
	GL_COORD_VERTEX Max;
    getBoundingBox(Min,Max);

	o << Min.x;
	o << Min.y;
	o << Min.z;
	o << Max.x;
	o << Max.y;
	o << Max.z;
	

	o << m_nbVertex;
	o << m_nbPolys;
	
	// Import shader !
#if defined(DATA_EXTENDED)
    if (geometry != NULL)
    {
        o<<(char)1;
        if (polys != NULL)
		    o<<(char)1;
	    else
		    o<<(char)0;
        o<<(char)1;
        o<<(char)1;
    }
#elif defined(DATA_PACKED)
	if (vertex != NULL)
		o<<(char)1;
	else
		o<<(char)0;

	if (polys != NULL)
		o<<(char)1;
	else
		o<<(char)0;

	if (normals != NULL)
		o<<(char)1;
	else
		o<<(char)0;

	if (colors != NULL)
		o<<(char)1;
	else
		o<<(char)0;
#endif

#if defined(DATA_EXTENDED)
    if (geometry != NULL)
#elif defined(DATA_PACKED)
	if (vertex != NULL)
#endif
		for (unsigned int i=0;i<m_nbVertex;i++)
			o << VERTEX(i);

	if (polys != NULL)
		for (unsigned int i=0;i<3*m_nbPolys;i++)
			o << polys[i];
	
#if defined(DATA_EXTENDED)
    if (geometry != NULL)
#elif defined(DATA_PACKED)
	if (normals != NULL)
#endif
		for (unsigned int i=0;i<m_nbVertex;i++)		
			o << NORMAL(i);

#if defined(DATA_EXTENDED)
    if (geometry != NULL)
#elif defined(DATA_PACKED)
	if (colors != NULL)
#endif
		for (unsigned int i=0;i<m_nbVertex;i++)
			o << COLOR(i);

	return true;
}

bool CGeometry::importRenderingModel(CRaptorIO& io)
{
    string name;
    io >> name;

    string data = io.getValueName();
    while (!data.empty())
    {
		bool bool_value = false;
        if (data == "front_geometry")
        {    
            io >> bool_value; 
            if (bool_value)
				addModel(CGeometry::CGL_FRONT_GEOMETRY);
            else
				removeModel(CGeometry::CGL_FRONT_GEOMETRY);
        }
        else if (data == "back_geometry")
        {    
            io >> bool_value; 
            if (bool_value)
				addModel(CGeometry::CGL_BACK_GEOMETRY);
            else
				removeModel(CGeometry::CGL_BACK_GEOMETRY);
        }
        else if (data == "normals")
        {    
            io >> bool_value; 
            if (bool_value)
				addModel(CGeometry::CGL_NORMALS);
            else
				removeModel(CGeometry::CGL_NORMALS);
        }
        else if (data == "tangents")
        {    
            io >> bool_value; 
            if (bool_value)
				addModel(CGeometry::CGL_TANGENTS);
            else
				removeModel(CGeometry::CGL_TANGENTS);
        }
        else if (data == "texture")
        {    
            io >> bool_value; 
            if (bool_value)
				addModel(CGeometry::CGL_TEXTURE);
            else
				removeModel(CGeometry::CGL_TEXTURE);
        }
        else if (data == "weights")
        {    
            io >> bool_value; 
            if (bool_value)
				addModel(CGeometry::CGL_WEIGHT);
            else
				removeModel(CGeometry::CGL_WEIGHT);
        }
        else if (data == "colors")
        {    
            io >> bool_value; 
            if (bool_value)
				addModel(CGeometry::CGL_COLORS);
            else
				removeModel(CGeometry::CGL_COLORS);
        }
        else if (data == "fog")
        {    
            io >> bool_value; 
            if (bool_value)
				addModel(CGeometry::CGL_FOG);
            else
				removeModel(CGeometry::CGL_FOG);
        }
        else
			io >> name;

        data = io.getValueName();
    }
    io >> name;

    return true;
}


bool CGeometry::importObject(CRaptorIO& io)
{
    string name;
    io >> name;
    
	vector<GL_COORD_VERTEX> coords;
	vector<GL_TEX_VERTEX> tcoords;
	vector<CColor::RGBA> rgbacolors;
	vector<unsigned int> faces;
	bool modelImported = false;

    string data = io.getValueName();
    while (!data.empty())
    {
		if (data == "name")
			CPersistence::importObject(io);
        else if (data =="Object")
            CObject3D::importObject(io);
        else if (data == "RenderingModel")
            modelImported = importRenderingModel(io);
		else if (data == "Vertex")
		{
    		io >> name;
			data = io.getValueName();
			while (!data.empty())
			{
				if (data == "Coord")
				{
					GL_COORD_VERTEX coord;
					io >> coord;
					coords.push_back(coord);
				}
				else if (data == "TexCoord")
				{
					GL_TEX_VERTEX tcoord;
					io >> tcoord;
					tcoords.push_back(tcoord);
				}
				else if (data == "Color")
				{
					CColor::RGBA color;
					io >> color;
					rgbacolors.push_back(color);
				}
				else
					io >> name;

				data = io.getValueName();
			}
			io >> name;
		}
		else if (data == "Face")
		{
			unsigned int p1=0,p2=0,p3=0;
			io >> name;
			data = io.getValueName();
			while (!data.empty())
			{
				if (data == "vertex1")
					io >> p1;
				else if (data == "vertex2")
					io >> p2;
				else if (data == "vertex3")
					io >> p3;

				data = io.getValueName();
			}
			faces.push_back(p1);
			faces.push_back(p2);
			faces.push_back(p3);
			io >> name;
		}
		else
			io >> name;

		data = io.getValueName();
    }
	io >> name;

	if (!coords.empty())
	{
		if (!modelImported)
			addModel(CGeometry::CGL_FRONT_GEOMETRY);
		glSetVertices(coords.size());
	}
	
	if (!tcoords.empty())
	{
		if (!modelImported)
			addModel(CGeometry::CGL_TEXTURE);
		glSetTexCoords(tcoords.size());
	}

	if (!faces.empty())
		glSetPolygons(faces.size() / 3);

	if (!rgbacolors.empty())
	{
		if (!modelImported)
			addModel(CGeometry::CGL_COLORS);
		glSetColors(rgbacolors.size());
	}

    glLockData();

    unsigned int i=0;
    for (i=0;i<coords.size();i++)
	    addVertex(coords[i].x,coords[i].y,coords[i].z,coords[i].h);
    for (i=0;i<tcoords.size();i++)
		setTexCoord(i,tcoords[i].u,tcoords[i].v);
    for (i=0;i<faces.size();i+=3)
		addFace(faces[i],faces[i+1],faces[i+2]);
    for (i=0;i<rgbacolors.size();i++)
		setColor(i,rgbacolors[i].r,rgbacolors[i].g,rgbacolors[i].b,rgbacolors[i].a);

	const CGeometryEditor& editor = getEditor();
	editor.genNormals(false);

    glUnLockData();

	return true;
}

