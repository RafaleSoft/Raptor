// Geometry.cpp: implementation of the CGeometry class.
//
//////////////////////////////////////////////////////////////////////
#include "Subsys/CodeGeneration.h"


#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
	#include "System/Raptor.h"
#endif
#if !defined(AFX_GEOMETRY_H__B42ABB87_80E8_11D3_97C2_DE5C28000000__INCLUDED_)
	#include "Geometry.h"
#endif
#if !defined(AFX_3DENGINE_H__DB24F018_80B9_11D3_97C1_FC2841000000__INCLUDED_)
	#include "Engine/3DEngine.h"
#endif
#if !defined(AFX_SHADER_H__4D405EC2_7151_465D_86B6_1CA99B906777__INCLUDED_)
	#include "Shader.h"
#endif
#if !defined(AFX_TEXTUREUNITSETUP_H__4A6ADC72_02E5_4F2A_931E_A736B6D6E0F0__INCLUDED_)
	#include "TextureUnitSetup.h"
#endif
#ifndef __GLOBAL_H__
	#include "System/Global.h"
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
#if !defined(AFX_VERTEXSHADER_H__F2D3BBC6_87A1_4695_B667_2B8C3C4CF022__INCLUDED_)
	#include "VertexShader.h"
#endif
#if !defined(AFX_GEOMETRYEDITOR_H__2D77E428_ED3D_416B_8DE9_DABFD45A38A7__INCLUDED_)
    #include "GeometryEditor.h"
#endif
#if !defined(AFX_OBJECT3DCONTOUR_H__C0C2B562_ABBC_4B04_A1E7_E0727FAC66AB__INCLUDED_)
	#include "GLHierarchy/Object3DContour.h"
#endif
#if !defined(AFX_RENDERINGPROPERTIES_H__634BCF2B_84B4_47F2_B460_D7FDC0F3B698__INCLUDED_)
	#include "RenderingProperties.h"
#endif
#if !defined(AFX_RAPTORVULKANCOMMANDBUFFER_H__0398BABD_747B_4DFE_94AA_B026BDBD03B1__INCLUDED_)
	#include "Subsys/Vulkan/VulkanCommandBuffer.h"
#endif

RAPTOR_NAMESPACE

static CGeometry::CGeometryClassID geometryID;
static CPersistentObjectType<CGeometry> geometryFactory(geometryID);
const CPersistence::CPersistenceClassID& CGeometry::CGeometryClassID::GetClassId(void)
{
	return geometryID;
}

//////////////////////////////////////////////////////////////////////
// Rendering Model Implementation
//////////////////////////////////////////////////////////////////////
void CGeometry::CRenderingModel::addModel(MODEL model)
{
	unsigned int m = ((m_renderingModel | model) & CGL_FULLRENDER);
	m_renderingModel = m;
}

void CGeometry::CRenderingModel::removeModel(MODEL model)
{
	m_renderingModel &= ~model;
}

CGeometry::CRenderingModel::CRenderingModel(long model)
{
	m_renderingModel = (model & CGL_FULLRENDER);
	if ((m_renderingModel == 0) || (m_renderingModel > CGL_FULLRENDER))
		m_renderingModel = CGL_FRONT_GEOMETRY;
}

const CGeometry::CRenderingModel& CGeometry::CRenderingModel::operator=(const CGeometry::CRenderingModel& model)
{
	m_renderingModel = (model.m_renderingModel & CGL_FULLRENDER);
	if ((m_renderingModel == 0) || (m_renderingModel > CGL_FULLRENDER))
		m_renderingModel = CGL_FRONT_GEOMETRY;
	return *this;
}

const CGeometryEditor& CGeometry::getEditor(void)
{
    if (m_pEditor == NULL)
        m_pEditor = new CGeometryEditor(this);

    return *m_pEditor;
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
    polys(NULL),m_renderingModel(CRenderingModel::CGL_FRONT_GEOMETRY),
    m_pEditor(NULL)
{
	m_nbVertex = m_nbPolys = 0;
    m_bDataLocked = false;
    m_bUpdateBBox = false;
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
	polys(NULL),m_renderingModel(CRenderingModel::CGL_FRONT_GEOMETRY),
    m_pEditor(NULL)
{
    m_nbVertex = m_nbPolys = 0;
    m_bDataLocked = false;
    m_bUpdateBBox = false;
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

	if (fogcoords != NULL)
		delete [] fogcoords;

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
}

void CGeometry::setRenderingModel(const CRenderingModel& model) 
{
	m_renderingModel = model;	//	Preserve all bits for derived classes

	//	But remove unsupported extensions
#ifdef GL_EXT_vertex_weighting
	if (!Raptor::glIsExtensionSupported(GL_EXT_VERTEX_WEIGHTING_EXTENSION_NAME))
		m_renderingModel.removeModel(CRenderingModel::CGL_WEIGHT);
#endif

	if (!Raptor::glIsExtensionSupported(GL_EXT_FOG_COORD_EXTENSION_NAME))
		m_renderingModel.removeModel(CRenderingModel::CGL_FOG);
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
	if (NULL != vertex) pAllocator->releaseVertices((float*)vertex);
	if (NULL != normals) pAllocator->releaseVertices((float*)normals);
	if (NULL != texcoords) pAllocator->releaseVertices((float*)texcoords);
	if (NULL != texcoords2) pAllocator->releaseVertices((float*)texcoords2);
	if (NULL != colors) pAllocator->releaseVertices((float*)colors);
    if (NULL != polys) pAllocator->releaseIndexes(polys);

	if (NULL != fogcoords) pAllocator->releaseVertices(fogcoords);
	if (NULL != weightcoords) pAllocator->releaseVertices(weightcoords);
	if (NULL != tangents) pAllocator->releaseVertices((float*)tangents);
	if (NULL != binormals) pAllocator->releaseVertices((float*)binormals);

    //! Allocate new data.
	vertex = (GL_COORD_VERTEX*)(pAllocator->allocateVertices(m_nbVertex*4));
	normals = (GL_COORD_VERTEX*)(pAllocator->allocateVertices(m_nbVertex*4));
    colors = NULL;
    texcoords = NULL;
	tangents = NULL;
	binormals = NULL;

    

    //! Transfer normals and vertex data.
    GL_COORD_VERTEX* geo_vertex = geo.vertex;
    GL_COORD_VERTEX* geo_normals = geo.normals;
    if (pAllocator->isMemoryRelocated())
    {
        vertex = (GL_COORD_VERTEX*)(pAllocator->glvkMapPointer((float*)vertex));
        normals = (GL_COORD_VERTEX*)(pAllocator->glvkMapPointer((float*)normals));
        geo_vertex = (GL_COORD_VERTEX*)(pAllocator->glvkMapPointer((float*)geo.vertex));
        geo_normals = (GL_COORD_VERTEX*)(pAllocator->glvkMapPointer((float*)geo.normals));
    }
	for (unsigned int i = 0; i<geo.m_nbVertex; i++)
	{
		vertex[i] = geo_vertex[i];
		normals[i] = geo_normals[i];
	}
    if (pAllocator->isMemoryRelocated())
    {
        vertex = (GL_COORD_VERTEX*)(pAllocator->glvkUnMapPointer((float*)vertex));
        normals = (GL_COORD_VERTEX*)(pAllocator->glvkUnMapPointer((float*)normals));
        pAllocator->glvkUnMapPointer((float*)geo_vertex);
        pAllocator->glvkUnMapPointer((float*)geo_normals);
    }

    //! Transfer texture coordinates
    if (geo.texcoords != NULL)
	{
		texcoords = (GL_TEX_VERTEX*)(pAllocator->allocateVertices(m_nbVertex*2));
        GL_TEX_VERTEX* geo_texcoords = geo.texcoords;

        if (pAllocator->isMemoryRelocated())
        {
            texcoords = (GL_TEX_VERTEX*)(pAllocator->glvkMapPointer((float*)texcoords));
            geo_texcoords = (GL_TEX_VERTEX*)(pAllocator->glvkMapPointer((float*)geo.texcoords));
        }

		for (unsigned int i = 0; i<m_nbVertex; i++)
			texcoords[i] = geo_texcoords[i];

        if (pAllocator->isMemoryRelocated())
        {
            texcoords = (GL_TEX_VERTEX*)(pAllocator->glvkUnMapPointer((float*)texcoords));
            pAllocator->glvkUnMapPointer((float*)geo_texcoords);
        }
	}

	//! Transfer additional texture coordinates
    if (geo.texcoords2 != NULL)
	{
		texcoords2 = (GL_TEX_VERTEX*)(pAllocator->allocateVertices(m_nbVertex*2));
        GL_TEX_VERTEX* geo_texcoords = geo.texcoords2;

        if (pAllocator->isMemoryRelocated())
        {
            texcoords2 = (GL_TEX_VERTEX*)(pAllocator->glvkMapPointer((float*)texcoords2));
            geo_texcoords = (GL_TEX_VERTEX*)(pAllocator->glvkMapPointer((float*)geo.texcoords2));
        }

		for (unsigned int i = 0; i<m_nbVertex; i++)
			texcoords2[i] = geo_texcoords[i];

        if (pAllocator->isMemoryRelocated())
        {
            texcoords2 = (GL_TEX_VERTEX*)(pAllocator->glvkUnMapPointer((float*)texcoords2));
            pAllocator->glvkUnMapPointer((float*)geo_texcoords);
        }
	}

    //! Transfer colors.
    if (geo.colors != NULL) 
	{
		colors = (CColor::RGBA*)(pAllocator->allocateVertices(m_nbVertex*4));
        CColor::RGBA* geo_colors = geo.colors;

        if (pAllocator->isMemoryRelocated())
        {
            colors = (CColor::RGBA*)(pAllocator->glvkMapPointer((float*)colors));
            geo_colors = (CColor::RGBA*)(pAllocator->glvkMapPointer((float*)geo.colors));
        }

		for (unsigned int i = 0; i<m_nbVertex; i++)
			colors[i] = geo_colors[i];

        if (pAllocator->isMemoryRelocated())
        {
            colors = (CColor::RGBA*)(pAllocator->glvkUnMapPointer((float*)colors));
            pAllocator->glvkUnMapPointer((float*)geo_colors);
        }
	}
#endif


    //! Transfer polygon informations
	polys = pAllocator->allocateIndexes(3 * m_nbPolys);
    unsigned short *geo_polys = geo.polys;

    if (pAllocator->isMemoryRelocated())
    {
        polys = (unsigned short*)(pAllocator->glvkMapPointer(polys));
        geo_polys = (unsigned short*)(pAllocator->glvkMapPointer(geo.polys));
    }

	for (unsigned int i = 0; i<3 * m_nbPolys; i++)
		polys[i] = geo_polys[i];

    if (pAllocator->isMemoryRelocated())
    {
        polys = (unsigned short*)(pAllocator->glvkUnMapPointer(polys));
        pAllocator->glvkUnMapPointer(geo_polys);
    }



	if (geo.fogcoords != NULL)
	{
		fogcoords = pAllocator->allocateVertices(m_nbVertex);
        if (pAllocator->isMemoryRelocated())
            fogcoords = pAllocator->glvkMapPointer(fogcoords);

		for (unsigned int i = 0; i<m_nbVertex; i++)
			fogcoords[i] = geo.fogcoords[i];

        if (pAllocator->isMemoryRelocated())
            fogcoords = pAllocator->glvkUnMapPointer(fogcoords);
	}

	if (geo.weightcoords != NULL)
	{
		weightcoords = pAllocator->allocateVertices(m_nbVertex);
        if (pAllocator->isMemoryRelocated())
            weightcoords = pAllocator->glvkMapPointer(weightcoords);

		for (unsigned int i = 0; i<m_nbVertex; i++)
			weightcoords[i] = geo.weightcoords[i];

        if (pAllocator->isMemoryRelocated())
            weightcoords = pAllocator->glvkUnMapPointer(weightcoords);
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
	VkBuffer bindings[3] = { vertexBinding, vertexBinding, vertexBinding };
	VkDeviceSize offsets[3] = { (VkDeviceSize)&vertex[0], (VkDeviceSize)&colors[0], (VkDeviceSize)&texcoords[0] };
	commandBuffer.vkCmdBindVertexBuffers(	commandBuffer.commandBuffer, 
											0, 3, &bindings[0], &offsets[0]);

	commandBuffer.vkCmdBindIndexBuffer(	commandBuffer.commandBuffer,
										indexBinding,
										(VkDeviceSize)&polys[0],
										VK_INDEX_TYPE_UINT16);

	commandBuffer.vkCmdDrawIndexed(commandBuffer.commandBuffer, 3 * m_nbPolys, 1, 0, 0, 0);
}

void CGeometry::glRender()
{
    if (!properties.isVisible())
		return;

    glRenderGeometry();
}

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
    CRenderingProperties *props = CRenderingProperties::GetCurrentProperties();
    bool popNormalArray = false;
    bool popTangentArray = false;
    bool popColorArray = false;
    bool popTexCoordArray = false;
    bool popWeightArray = false;
    bool popFogArray = false;
    bool proceedLighting = (props->getCurrentLighting() == CRenderingProperties::ENABLE);
    bool proceedTexturing = (props->getCurrentTexturing() == CRenderingProperties::ENABLE);
    

	if (m_renderingModel.hasModel(CRenderingModel::CGL_BACK_GEOMETRY))
		glCullFace(GL_FRONT);

	// always extract geometry
	glEnableClientState(GL_VERTEX_ARRAY);
#if defined(DATA_EXTENDED)
    glVertexPointer( 3,GL_FLOAT,sizeof(GL_VERTEX_DATA),&geometry[0].vertex);
#elif defined(DATA_PACKED)
	glVertexPointer( 3 , GL_FLOAT , sizeof(GL_COORD_VERTEX) , vertex);
#endif

	// extract normals
	if (m_renderingModel.hasModel(CRenderingModel::CGL_NORMALS) && proceedLighting
#if defined (DATA_EXTENDED)
		&& (geometry != NULL))
#elif defined(DATA_PACKED)
		&& (NULL != normals))
#endif
	{
		glEnableClientState(GL_NORMAL_ARRAY);
        popNormalArray = true;
#if defined(DATA_EXTENDED)
        glNormalPointer( GL_FLOAT , sizeof(GL_VERTEX_DATA) , &geometry[0].normal);
#elif defined(DATA_PACKED)
		glNormalPointer( GL_FLOAT , sizeof(GL_COORD_VERTEX) , normals);
#endif
	}

    // extract tangents
#if defined(GL_ARB_vertex_program)
	if (m_renderingModel.hasModel(CRenderingModel::CGL_TANGENTS) && proceedLighting
#if defined (DATA_EXTENDED)
		&& (geometry != NULL))
#elif defined(DATA_PACKED)
		&& (NULL != tangents))
#endif
	{
        popTangentArray = true;
		pExtensions->glEnableVertexAttribArrayARB(CProgramParameters::ADDITIONAL_PARAM1);
#if defined(DATA_EXTENDED)
        pExtensions->glVertexAttribPointerARB(CShaderProgram::ADDITIONAL_PARAM1,4,GL_FLOAT,false,sizeof(GL_VERTEX_DATA),&geometry[0].tangent);
#elif defined(DATA_PACKED)
        if (tangents != NULL)
			pExtensions->glVertexAttribPointerARB(CProgramParameters::ADDITIONAL_PARAM1, 4, GL_FLOAT, false, 0, tangents);
#endif
	}
#endif

	// extract colors
	if (m_renderingModel.hasModel(CRenderingModel::CGL_COLORS)
#if defined (DATA_EXTENDED)
		&& (geometry != NULL))
#elif defined(DATA_PACKED)
		&& (NULL != colors))
#endif
	{
		glEnableClientState(GL_COLOR_ARRAY);
        popColorArray = true;
#if defined(DATA_EXTENDED)
        glColorPointer( 4 , GL_FLOAT , sizeof(GL_VERTEX_DATA), &geometry[0].color);
#elif defined(DATA_PACKED)
		glColorPointer( 4 , GL_FLOAT , 0, colors);
#endif
	}

	// extract texture
	if (m_renderingModel.hasModel(CRenderingModel::CGL_TEXTURE) && proceedTexturing
#if defined (DATA_EXTENDED)
        && (geometry != NULL))
#elif defined(DATA_PACKED)
		&& (texcoords != NULL))
#endif
	{
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
            popTexCoordArray = true;
#if defined(DATA_EXTENDED)
            glTexCoordPointer( 2,GL_FLOAT,sizeof(GL_VERTEX_DATA),&geometry[0].texCoord0);
#elif defined(DATA_PACKED)
			glTexCoordPointer( 2,GL_FLOAT,0,texcoords);
#endif
	}

// Vertex weighting is no supported anymore by drivers because
// shaders enable a more powerfull blending.

	// extract vertex weighting
	if (m_renderingModel.hasModel(CRenderingModel::CGL_WEIGHT))
	{
#ifdef GL_EXT_vertex_weighting
		if (pExtensions->glVertexWeightPointerEXT != NULL)
		{
			glEnable(GL_VERTEX_WEIGHTING_EXT);
			glEnableClientState(GL_VERTEX_WEIGHT_ARRAY_EXT);
            popWeightArray = true;
			pExtensions->glVertexWeightPointerEXT(1,GL_FLOAT,0,weightcoords);
			CGenericMatrix<float> gm,gm2;
			glGetFloatv(GL_MODELVIEW_MATRIX,gm.matrix());
			CGenericMatrix<float> gm2;
			gm2 = weightMatrix;
			gm2 *= gm;
			glMatrixMode(GL_MODELVIEW1_EXT);
			glLoadMatrixf(gm2.matrix());
			glMatrixMode(GL_MODELVIEW0_EXT);
		}
        else
#else
        {
			pExtensions->glEnableVertexAttribArrayARB(CProgramParameters::WEIGHTS);
#if defined(DATA_EXTENDED)
            pExtensions->glVertexAttribPointerARB(CShaderProgram::WEIGHTS,1,GL_FLOAT,false,sizeof(GL_VERTEX_DATA),&geometry[0].weight);
#elif defined(DATA_PACKED)
            if (weightcoords != NULL)
				pExtensions->glVertexAttribPointerARB(CProgramParameters::WEIGHTS, 1, GL_FLOAT, false, 0, weightcoords);
#endif
        }
#endif
	}

#ifdef GL_EXT_fog_coord
	//	extract fog coords
	if (m_renderingModel.hasModel(CRenderingModel::CGL_FOG))
	{
		if (pExtensions->glFogCoordPointerEXT != NULL)
		{
			glFogi(GL_FOG_COORDINATE_SOURCE_EXT,GL_FOG_COORDINATE_EXT);
			glEnableClientState(GL_FOG_COORDINATE_ARRAY_EXT);
            popFogArray = true;
			pExtensions->glFogCoordPointerEXT(GL_FLOAT,0,fogcoords);
		}
	}
#endif

	if (m_pPrimitives.size() == 0)
		glDrawElements( GL_TRIANGLES, 3*m_nbPolys, GL_UNSIGNED_SHORT,polys);
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
	if (m_renderingModel.hasModel(CRenderingModel::CGL_FOG))
		glFogi(GL_FOG_COORDINATE_SOURCE_EXT,GL_FRAGMENT_DEPTH_EXT);
#endif

	//	Restore status	
	if (m_renderingModel.hasModel(CRenderingModel::CGL_BACK_GEOMETRY))
		glCullFace(GL_BACK);
	
    glDisableClientState(GL_VERTEX_ARRAY);
    if (popNormalArray)
        glDisableClientState(GL_NORMAL_ARRAY);
    if (popColorArray)
        glDisableClientState(GL_COLOR_ARRAY);
    if (popTexCoordArray)
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
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
#endif

	Global::GetInstance().getCurrentStatus().iRenderedObjects++;
	Global::GetInstance().getCurrentStatus().iRenderedTriangles += m_nbPolys;
}

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
		for (unsigned int i=m_pPrimitives.size();i>0;i--)
			m_pPrimitives[i-1]->releaseReference();
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

void CGeometry::glSetPolygons(unsigned int nbP, unsigned short* polygons)
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
			pAllocator->glvkCopyPointer(polys,polygons,3*nbP);
		m_nbPolys = nbP;
	}
}

void CGeometry::glSetVertices(unsigned int nbV, GL_COORD_VERTEX* vertices)
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
			pAllocator->glvkCopyPointer(*vertex,*vertices,4*nbV);
		m_nbVertex = nbV;
	}
#endif
}

void CGeometry::glSetNormals(unsigned int nbN, GL_COORD_VERTEX* norms)
{
#if defined (DATA_PACKED)
	if ((norms != NULL) && (nbN > 0) && (normals != NULL))
	{
		CGeometryAllocator *pAllocator = CGeometryAllocator::GetInstance();
		if (!pAllocator->isMemoryRelocated() || m_bDataLocked)
			memcpy(normals,norms,4*nbN*sizeof(float));
		else
			pAllocator->glvkCopyPointer(*normals,*norms,4*nbN);
	}
#endif
}

void CGeometry::glSetTexCoords(unsigned int nbT, GL_TEX_VERTEX* texCoords)
{
#if defined (DATA_PACKED)
	if (texCoords == NULL)
	{
		if (texcoords != NULL)
			CGeometryAllocator::GetInstance()->releaseVertices((float*)texcoords);

		texcoords = (GL_TEX_VERTEX*)(CGeometryAllocator::GetInstance()->allocateVertices(nbT*2));
	}
	else if ((nbT > 0) && (texcoords != NULL))
	{
		CGeometryAllocator *pAllocator = CGeometryAllocator::GetInstance();
		if (!pAllocator->isMemoryRelocated() || m_bDataLocked)
			memcpy(texcoords,texCoords,nbT*sizeof(GL_TEX_VERTEX));
		else
			pAllocator->glvkCopyPointer(*texcoords,*texCoords,2*nbT);
	}
#endif
}

void CGeometry::glSetTexCoords2(unsigned int nbT, GL_TEX_VERTEX* texCoords)
{
#if defined (DATA_PACKED)
	if (texCoords == NULL)
	{
		if (texcoords2 != NULL)
			CGeometryAllocator::GetInstance()->releaseVertices((float*)texcoords2);

		texcoords2 = (GL_TEX_VERTEX*)(CGeometryAllocator::GetInstance()->allocateVertices(nbT*2));
	}
	else if ((nbT > 0) && (texcoords2 != NULL))
	{
		CGeometryAllocator *pAllocator = CGeometryAllocator::GetInstance();
		if (!pAllocator->isMemoryRelocated() || m_bDataLocked)
			memcpy(texcoords2,texCoords,nbT*sizeof(GL_TEX_VERTEX));
		else
			pAllocator->glvkCopyPointer(*texcoords2,*texCoords,2*nbT);
	}
#endif
}

void CGeometry::glSetWeights(unsigned int nbW, float* weights)
{
#if defined (DATA_PACKED)
	if (weights == NULL)
	{
		if (weightcoords!=NULL)
			CGeometryAllocator::GetInstance()->releaseVertices(weightcoords);

		weightcoords = CGeometryAllocator::GetInstance()->allocateVertices(nbW);
	}
	else if ((nbW > 0) && (weightcoords != NULL))
	{
		CGeometryAllocator *pAllocator = CGeometryAllocator::GetInstance();
		if (!pAllocator->isMemoryRelocated() || m_bDataLocked)
			memcpy(weightcoords,weights,nbW*sizeof(float));
		else
			pAllocator->glvkCopyPointer(weightcoords,weights,nbW);
	}
#endif
}

void CGeometry::glSetColors(unsigned int nbC, CColor::RGBA* rgbaColors)
{
#if defined (DATA_PACKED)
	if (rgbaColors == NULL)
	{
		if (colors != NULL)
			CGeometryAllocator::GetInstance()->releaseVertices((float*)colors);

		colors = (CColor::RGBA*)(CGeometryAllocator::GetInstance()->allocateVertices(nbC*4));
	}
	else if ((nbC > 0) && (colors != NULL))
	{
		CGeometryAllocator *pAllocator = CGeometryAllocator::GetInstance();
		if (!pAllocator->isMemoryRelocated() || m_bDataLocked)
			memcpy(colors,rgbaColors,4*nbC*sizeof(float));
		else
			pAllocator->glvkCopyPointer(*colors,*rgbaColors,4*nbC);
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

void CGeometry::setCoord(unsigned int numvtx,GLfloat x,GLfloat y,GLfloat z,GLfloat h)
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

void CGeometry::setTexCoord(unsigned int numvtx,float u,float v)
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

void CGeometry::getFace(unsigned int numface,unsigned int &p1,unsigned int &p2,unsigned int &p3) const
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

void CGeometry::setWeight(unsigned int numvtx,float w)
{
#if defined (DATA_EXTENDED)
    if ((numvtx<m_nbVertex) && (geometry != NULL))
#elif defined (DATA_PACKED)
	if ((numvtx<m_nbVertex) && (weightcoords != NULL))
#endif
		WEIGHT(numvtx) = w;
}

float CGeometry::getWeight(unsigned int numvtx) const
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


void CGeometry::setFogCoord(unsigned int numvtx,GLfloat f)
{
#if defined (DATA_EXTENDED)
    if ((numvtx<m_nbVertex) && (geometry != NULL))
#elif defined (DATA_PACKED)
	if ((numvtx<m_nbVertex) && (fogcoords != NULL))
#endif
		FOGCOORD(numvtx) = f;
}

float CGeometry::getFogCoord(unsigned int numvtx) const
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

void CGeometry::getCoord(unsigned int numvtx,GL_COORD_VERTEX &v) const
{
#if defined (DATA_EXTENDED)
    if ((numvtx<m_nbVertex) && (geometry != NULL))
#elif defined (DATA_PACKED)
	if ((numvtx<m_nbVertex) && (vertex != NULL))
#endif
		v = VERTEX(numvtx);
}

void CGeometry::getVertex(unsigned int numvtx,GL_VERTEX_DATA &v) const
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

void CGeometry::setNormal(unsigned int numvtx,GLfloat x,GLfloat y,GLfloat z,GLfloat h)
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

void CGeometry::getNormal(unsigned int numvtx,GL_COORD_VERTEX &v) const
{
#if defined (DATA_EXTENDED)
    if ((numvtx<m_nbVertex) && (geometry != NULL))
#elif defined (DATA_PACKED)
	if ((numvtx<m_nbVertex) && (normals != NULL))
#endif
		v = NORMAL(numvtx);
}

void CGeometry::getTangent(unsigned int numvtx,GL_COORD_VERTEX &v) const
{
#if defined (DATA_EXTENDED)
    if ((numvtx<m_nbVertex) && (geometry != NULL))
#elif defined (DATA_PACKED)
	if ((numvtx<m_nbVertex) && (tangents != NULL))
#endif
		v = TANGENT(numvtx);
}

void CGeometry::getBiNormal(unsigned int numvtx,GL_COORD_VERTEX &v) const
{
#if defined (DATA_EXTENDED)
    if ((numvtx<m_nbVertex) && (geometry != NULL))
#elif defined (DATA_PACKED)
	if ((numvtx<m_nbVertex) && (binormals != NULL))
#endif
		v = BINORMAL(numvtx);
}

void CGeometry::setColor(unsigned int numvtx,float r,float g,float b,float a)
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

void CGeometry::getColor(unsigned int numvtx,CColor::RGBA &v) const
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
                m_renderingModel.addModel(CRenderingModel::CGL_FRONT_GEOMETRY);
            else
                m_renderingModel.removeModel(CRenderingModel::CGL_FRONT_GEOMETRY);
        }
        else if (data == "back_geometry")
        {    
            io >> bool_value; 
            if (bool_value)
                m_renderingModel.addModel(CRenderingModel::CGL_BACK_GEOMETRY);
            else
                m_renderingModel.removeModel(CRenderingModel::CGL_BACK_GEOMETRY);
        }
        else if (data == "normals")
        {    
            io >> bool_value; 
            if (bool_value)
                m_renderingModel.addModel(CRenderingModel::CGL_NORMALS);
            else
                m_renderingModel.removeModel(CRenderingModel::CGL_NORMALS);
        }
        else if (data == "tangents")
        {    
            io >> bool_value; 
            if (bool_value)
                m_renderingModel.addModel(CRenderingModel::CGL_TANGENTS);
            else
                m_renderingModel.removeModel(CRenderingModel::CGL_TANGENTS);
        }
        else if (data == "texture")
        {    
            io >> bool_value; 
            if (bool_value)
                m_renderingModel.addModel(CRenderingModel::CGL_TEXTURE);
            else
                m_renderingModel.removeModel(CRenderingModel::CGL_TEXTURE);
        }
        else if (data == "weights")
        {    
            io >> bool_value; 
            if (bool_value)
                m_renderingModel.addModel(CRenderingModel::CGL_WEIGHT);
            else
                m_renderingModel.removeModel(CRenderingModel::CGL_WEIGHT);
        }
        else if (data == "colors")
        {    
            io >> bool_value; 
            if (bool_value)
                m_renderingModel.addModel(CRenderingModel::CGL_COLORS);
            else
                m_renderingModel.removeModel(CRenderingModel::CGL_COLORS);
        }
        else if (data == "fog")
        {    
            io >> bool_value; 
            if (bool_value)
                m_renderingModel.addModel(CRenderingModel::CGL_FOG);
            else
                m_renderingModel.removeModel(CRenderingModel::CGL_FOG);
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
	CRenderingModel l_model(getRenderingModel());
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
			l_model.addModel(CRenderingModel::CGL_FRONT_GEOMETRY);
		glSetVertices(coords.size());
	}
	
	if (!tcoords.empty())
	{
		if (!modelImported)
			l_model.addModel(CRenderingModel::CGL_TEXTURE);
		glSetTexCoords(tcoords.size());
	}

	if (!faces.empty())
		glSetPolygons(faces.size() / 3);

	if (!rgbacolors.empty())
	{
		if (!modelImported)
			l_model.addModel(CRenderingModel::CGL_COLORS);
		glSetColors(rgbacolors.size());
	}

	if (!modelImported)
		setRenderingModel(l_model);

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

