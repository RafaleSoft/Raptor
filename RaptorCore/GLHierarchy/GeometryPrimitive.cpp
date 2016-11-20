// GeometryPrimitive.cpp: implementation of the CGeometryPrimitive class.
//
//////////////////////////////////////////////////////////////////////

#include "Subsys/CodeGeneration.h"

#ifndef __CGLTYPES_HPP__
    #include "System/CGLTypes.h"
#endif
#ifndef __RAPTOR_GLEXT_H__
	#include "System/Glext.h"
#endif
#if !defined(AFX_GEOMETRYPRIMITIVE_H__890D2E18_2DAC_4094_AE20_BDF7D6FA5DBF__INCLUDED_)
	#include "GeometryPrimitive.h"
#endif
#if !defined(AFX_GEOMETRYALLOCATOR_H__802B3C7A_43F7_46B2_A79E_DDDC9012D371__INCLUDED_)
	#include "Subsys/GeometryAllocator.h"
#endif
#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
	#include "System/Raptor.h"
#endif
#if !defined(AFX_RAPTORERRORMANAGER_H__FA5A36CD_56BC_4AA1_A5F4_451734AD395E__INCLUDED_)
    #include "System/RaptorErrorManager.h"
#endif
#if !defined(AFX_GEOMETRY_H__B42ABB87_80E8_11D3_97C2_DE5C28000000__INCLUDED_)
	#include "Geometry.h"
#endif
#ifndef __GLOBAL_H__
	#include "System/Global.h"
#endif

RAPTOR_NAMESPACE


#define GL_COORD_VERTEX_STRIDE sizeof(GL_COORD_VERTEX)/sizeof(float)
#define GL_TEX_VERTEX_STRIDE sizeof(GL_TEX_VERTEX)/sizeof(float)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CGeometryPrimitive::CGeometryPrimitive(PRIMITIVE_KIND kind)
	:m_size(0),m_faces(NULL),m_kind(kind),
	m_uorder(0),m_vorder(0),m_uprecision(0),m_vprecision(0),
	m_polygonsSize(0),m_polygons(NULL)
{
}

CGeometryPrimitive::~CGeometryPrimitive()
{
	if (m_polygons != NULL)
		delete [] m_polygons;

	if (m_faces != NULL)
		CGeometryAllocator::GetInstance()->releaseIndexes(m_faces);
}

void CGeometryPrimitive::glRender(void)
{
	if (HIGH_ORDER_SURFACE != m_kind)
		if ((m_size < 1) || (m_faces == NULL))
			return;

	switch(m_kind)
	{
		case POINT:
			glDrawElements( GL_POINTS, m_size, GL_UNSIGNED_SHORT,m_faces);
			break;
		case LINE:
			glDrawElements( GL_LINES, m_size, GL_UNSIGNED_SHORT,m_faces);
			break;
		case TRIANGLE:
			glDrawElements( GL_TRIANGLES, m_size, GL_UNSIGNED_SHORT,m_faces);
			break;
		case QUAD:
			glDrawElements( GL_QUADS, m_size, GL_UNSIGNED_SHORT,m_faces);
			break;
		case LINE_STRIP:
			glDrawElements( GL_LINE_STRIP, m_size, GL_UNSIGNED_SHORT,m_faces);
			break;
		case TRIANGLE_STRIP:
			glDrawElements( GL_TRIANGLE_STRIP, m_size, GL_UNSIGNED_SHORT,m_faces);
			break;
		case QUAD_STRIP:
			glDrawElements( GL_QUAD_STRIP, m_size, GL_UNSIGNED_SHORT,m_faces);
			break;
		case LINE_LOOP:
			glDrawElements( GL_LINE_LOOP, m_size, GL_UNSIGNED_SHORT,m_faces);
			break;
		case TRIANGLE_FAN:
			glDrawElements( GL_TRIANGLE_FAN, m_size, GL_UNSIGNED_SHORT,m_faces);
			break;
		case POLYGON:
		{
			unsigned int offset = 0;
			for (unsigned int i=0;i<m_polygonsSize;i++)
			{
				glDrawElements( GL_POLYGON, 
								m_polygons[i], 
								GL_UNSIGNED_SHORT,
								m_faces + offset);

				offset += m_polygons[i];
			}
			break;
		}
		case HIGH_ORDER_SURFACE:
		{
			glEnable(GL_AUTO_NORMAL);
			glEnable(GL_MAP2_NORMAL);
			CGeometryAllocator *pAllocator = CGeometryAllocator::GetInstance();

			float	*vp = NULL;
            glGetPointerv(GL_VERTEX_ARRAY_POINTER,(void**)&vp);
			float	*tp = NULL;
            glGetPointerv(GL_TEXTURE_COORD_ARRAY_POINTER,(void**)&tp);

			if (pAllocator->isMemoryLocked())
			{
				pAllocator->glvkLockMemory(false);
				vp = pAllocator->glvkMapPointer(vp);
				tp = pAllocator->glvkMapPointer(tp);
			}

			if (vp != NULL)
			{
				glEnable(GL_MAP2_VERTEX_3);
				glMap2f(GL_MAP2_VERTEX_3,
						0,1,GL_COORD_VERTEX_STRIDE,m_uorder,
						0,1,GL_COORD_VERTEX_STRIDE*m_uorder,m_vorder,
						vp);
			}
			if (tp != NULL)
			{
				glEnable(GL_MAP2_TEXTURE_COORD_2);
				glMap2f(GL_MAP2_TEXTURE_COORD_2,
						0,1,GL_TEX_VERTEX_STRIDE,m_uorder,
						0,1,GL_TEX_VERTEX_STRIDE*m_uorder,m_vorder,
						tp);
			}

			glFrontFace(GL_CW);
			glMapGrid2f(m_uprecision, 0, 1, m_vprecision, 0, 1);
			glEvalMesh2(GL_FILL,0,m_uprecision,0,m_vprecision);
			glFrontFace(GL_CCW);

			if (!pAllocator->isMemoryLocked())
			{
				pAllocator->glDiscardPointer(vp);
				pAllocator->glDiscardPointer(tp);
				pAllocator->glvkLockMemory(true);
			}

			break;
		}
		default:
			break;
	}

	Global::GetInstance().getCurrentStatus().iRenderedObjects++;
	Global::GetInstance().getCurrentStatus().iRenderedTriangles += m_size;

    CATCH_GL_ERROR
}


void CGeometryPrimitive::setOrder(	unsigned int uorder, unsigned int vorder,
									unsigned int uprecision, unsigned int vprecision)
{
	m_uorder = uorder;
	m_vorder = vorder;
	m_uprecision = uprecision;
	m_vprecision = vprecision;
}


void CGeometryPrimitive::setIndexes(unsigned short size,unsigned short* faces)
{
	if ((faces != NULL) && (m_kind != CGeometryPrimitive::HIGH_ORDER_SURFACE))
	{
		if (m_faces != NULL)
			CGeometryAllocator::GetInstance()->releaseIndexes(m_faces);

		m_size = size;
		m_faces = CGeometryAllocator::GetInstance()->allocateIndexes(size);
		if (m_faces == NULL)
			return;

		if (CGeometryAllocator::GetInstance()->isMemoryRelocated())
			CGeometryAllocator::GetInstance()->glvkCopyPointer(m_faces,faces,size);
		else
			memcpy(m_faces,faces,size*sizeof(unsigned short));
	}
#ifdef RAPTOR_DEBUG_MODE_GENERATION
	else
	{
        Raptor::GetErrorManager()->generateRaptorError(	CGeometry::CGeometryClassID::GetClassId(),
														CRaptorErrorManager::RAPTOR_WARNING,
														CRaptorMessages::ID_NULL_OBJECT);
	}
#endif
}

void CGeometryPrimitive::setIndexes(const vector<unsigned short> &faces)
{
	if (!faces.empty() && (m_kind != CGeometryPrimitive::HIGH_ORDER_SURFACE))
	{
		if (m_faces != NULL)
			CGeometryAllocator::GetInstance()->releaseIndexes(m_faces);

		m_size = faces.size();
		m_faces = CGeometryAllocator::GetInstance()->allocateIndexes(m_size);
		if (m_faces == NULL)
			return;

        if (CGeometryAllocator::GetInstance()->isMemoryRelocated())
            m_faces = CGeometryAllocator::GetInstance()->glvkMapPointer(m_faces);

		for (unsigned short i=0;i<m_size;i++)
			m_faces[i] = faces[i];

        if (CGeometryAllocator::GetInstance()->isMemoryRelocated())
            m_faces = CGeometryAllocator::GetInstance()->glvkUnMapPointer(m_faces);
	}
#ifdef RAPTOR_DEBUG_MODE_GENERATION
	else
	{
        Raptor::GetErrorManager()->generateRaptorError(	CGeometry::CGeometryClassID::GetClassId(),
														CRaptorErrorManager::RAPTOR_WARNING,
														CRaptorMessages::ID_NULL_OBJECT);
	}
#endif
}

void CGeometryPrimitive::setIndexes(const vector<unsigned short> &polygonSizes,const vector<unsigned short> &polygonIndexes)
{
	if (!polygonIndexes.empty() && 
		!polygonSizes.empty() && 
		(m_kind == CGeometryPrimitive::POLYGON))
	{
		if (m_faces != NULL)
			CGeometryAllocator::GetInstance()->releaseIndexes(m_faces);
		if (m_polygons != NULL)
			delete [] m_polygons;

		m_polygonsSize = polygonSizes.size();
		m_polygons = new unsigned short[m_polygonsSize];

		m_size = polygonIndexes.size();
		m_faces = CGeometryAllocator::GetInstance()->allocateIndexes(m_size);
		if (m_faces == NULL)
			return;

		if (CGeometryAllocator::GetInstance()->isMemoryRelocated())
            m_faces = CGeometryAllocator::GetInstance()->glvkMapPointer(m_faces);

		unsigned short i=0;
		for (i=0;i<m_size;i++)
			m_faces[i] = polygonIndexes[i];

		for (i=0;i<m_polygonsSize;i++)
			m_polygons[i] = polygonSizes[i];

		if (CGeometryAllocator::GetInstance()->isMemoryRelocated())
            m_faces = CGeometryAllocator::GetInstance()->glvkUnMapPointer(m_faces);
	}
#ifdef RAPTOR_DEBUG_MODE_GENERATION
	else
	{
        Raptor::GetErrorManager()->generateRaptorError(	CGeometry::CGeometryClassID::GetClassId(),
														CRaptorErrorManager::RAPTOR_WARNING,
														CRaptorMessages::ID_NULL_OBJECT);
	}
#endif
}

void CGeometryPrimitive::getIndexes(unsigned short *faces)
{
    if ((faces != NULL) && (m_kind != CGeometryPrimitive::HIGH_ORDER_SURFACE))
	{
		if (m_size == 0)
			memset(faces,0,m_size*sizeof(unsigned short));
		else
		{
			if (CGeometryAllocator::GetInstance()->isMemoryRelocated())
				m_faces = CGeometryAllocator::GetInstance()->glvkMapPointer(m_faces);
			
			memcpy(faces,m_faces,m_size*sizeof(unsigned short));

			if (CGeometryAllocator::GetInstance()->isMemoryRelocated())
				m_faces = CGeometryAllocator::GetInstance()->glDiscardPointer(m_faces);
		}
    }
#ifdef RAPTOR_DEBUG_MODE_GENERATION
	else
	{
        Raptor::GetErrorManager()->generateRaptorError(	CGeometry::CGeometryClassID::GetClassId(),
														CRaptorErrorManager::RAPTOR_WARNING,
														CRaptorMessages::ID_NULL_OBJECT);
	}
#endif
}

void CGeometryPrimitive::getIndexes(vector<unsigned short> &polygonSizes,vector<unsigned short> &polygonIndexes)
{
	if (m_kind == CGeometryPrimitive::POLYGON)
	{
		if ((m_size == 0) || 
			(m_polygonsSize == 0) || 
			(m_faces == NULL) ||
			(m_polygons == NULL))
		{
			polygonSizes.clear();
			polygonIndexes.clear();
		}
		else
		{
			unsigned short i=0;
			for (i=0;i<m_polygonsSize;i++)
				polygonSizes.push_back(m_polygons[i]);

			if (CGeometryAllocator::GetInstance()->isMemoryRelocated())
				m_faces = CGeometryAllocator::GetInstance()->glvkMapPointer(m_faces);
			
			for (i=0;i<m_size;i++)
				polygonIndexes.push_back(m_faces[i]);

			if (CGeometryAllocator::GetInstance()->isMemoryRelocated())
				m_faces = CGeometryAllocator::GetInstance()->glDiscardPointer(m_faces);
		}
    }
#ifdef RAPTOR_DEBUG_MODE_GENERATION
	else
	{
		Raptor::GetErrorManager()->generateRaptorError(	CGeometry::CGeometryClassID::GetClassId(),
														CRaptorErrorManager::RAPTOR_WARNING,
														CRaptorMessages::ID_NULL_OBJECT);
	}
#endif
}

