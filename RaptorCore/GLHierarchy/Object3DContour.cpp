// Object3DContour.cpp: implementation of the CObject3DContour class.
//
//////////////////////////////////////////////////////////////////////
#include "Subsys/CodeGeneration.h"


#if !defined(AFX_OBJECT3DCONTOUR_H__C0C2B562_ABBC_4B04_A1E7_E0727FAC66AB__INCLUDED_)
	#include "Object3DContour.h"
#endif
#if !defined(AFX_GEOMETRY_H__B42ABB87_80E8_11D3_97C2_DE5C28000000__INCLUDED_)
	#include "GLHierarchy/Geometry.h"
#endif
#if !defined(AFX_3DSET_H__DB24F01B_80B9_11D3_97C1_FC2841000000__INCLUDED_)
	#include "GLHierarchy/3DSet.h"
#endif
#if !defined(AFX_MEMORY_H__81A6CA9A_4ED9_4260_B6E4_C03276C38DBC__INCLUDED_)
	#include "System/Memory.h"
#endif
#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
	#include "System/Raptor.h"
#endif
#ifdef RAPTOR_SSE_CODE_GENERATION
	#if !defined(AFX_SSE_BOUNDINGBOX_H__DD1B683A_C345_4399_B1CF_4059B5C3A744__INCLUDED_)
		#include "SSE_Engine/SSE_BoundingBox.h"
	#endif
#endif
#if !defined(AFX_CONTOURATTRIBUTES_H__6ADB3B0A_FE00_4385_B7EC_3A239298653D__INCLUDED_)
	#include "Subsys/ContourAttributes.h"
#endif
#if !defined(AFX_RAPTORERRORMANAGER_H__FA5A36CD_56BC_4AA1_A5F4_451734AD395E__INCLUDED_)
    #include "System/RaptorErrorManager.h"
#endif
#if !defined(AFX_GEOMETRYEDITOR_H__2D77E428_ED3D_416B_8DE9_DABFD45A38A7__INCLUDED_)
    #include "GeometryEditor.h"
#endif
#if !defined(AFX_OBJECT3DCONTAINERNOTIFIER_H__BF1EABCD_500E_4D7C_8561_2C535DF0640A__INCLUDED_)
    #include "Subsys/Object3DContainerNotifier.h"
#endif


RAPTOR_NAMESPACE

static CObject3DContour::CObject3DContourClassID contourId;
const CPersistence::CPersistenceClassID& CObject3DContour::CObject3DContourClassID::GetClassId(void)
{
	return contourId;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CObject3DContour::CObject3DContour(const std::string& name)
{
	m_pOrigin = NULL;

	m_pContour = new CContourAttributes;
	m_pContour->pContourNormals = NULL;
    m_pContour->pContourEdges = NULL;
    m_pContour->pContourFaces = NULL;
    m_pContour->pContourVolume = NULL;

    m_contourVolume.volume = NULL;
    m_contourVolume.volumeSize = 0;
    m_contourVolume.darkCapSize = 0;
    m_contourVolume.lightCapSize = 0;
    m_contourVolume.volumeIndexes = 0;
    m_contourVolume.darkCapIndexes = 0;
    m_contourVolume.lightCapIndexes = 0;
    m_contourVolume.darkCapClipped = false;
    m_contourVolume.lightCapClipped = false;
    m_contourVolume.volumeClipped = false;

    m_pObserver = new CObject3DContainerNotifier<CObject3DContour,CObject3D*>(*this,&CObject3DContour::notifyFromOrigin);
}

CObject3DContour::~CObject3DContour()
{
    if (m_pOrigin != NULL)
    {
        m_pOrigin->removeContainerNotifier(m_pObserver);
        delete m_pOrigin;
    }

    delete m_pObserver;

	if (m_pContour->pContourNormals != NULL)
		CHostMemoryManager::GetInstance()->release(m_pContour->pContourNormals);

	if (m_pContour->backfaces != NULL)
		delete [] m_pContour->backfaces;

	if (m_pContour->darkcap != NULL)
		delete [] m_pContour->darkcap;

	if (m_pContour->lightcap != NULL)
		delete [] m_pContour->lightcap;

	if (m_pContour->volume != NULL)
		delete [] m_pContour->volume;

	if (m_pContour->pContourVolume != NULL)
		delete [] m_pContour->pContourVolume;

    if (m_pContour->pContourFaces != NULL)
        delete [] m_pContour->pContourFaces;

    if (m_pContour->pContourEdges != NULL)
        delete [] m_pContour->pContourEdges;

    if (m_pContour->edgeList != NULL)
        delete [] m_pContour->edgeList;

	delete m_pContour;
}

void CObject3DContour::notifyFromOrigin(CObject3D* child)
{
    if ((child != NULL) &&
        (m_pObserver->getEvent() == CContainerNotifier<CObject3D*>::DESTRUCTION))
    {
        if (child == m_pOrigin)
            m_pOrigin = NULL;
    } 
}  
        
CObject3DContour &CObject3DContour::operator=(const CObject3DContour &r_contour)
{
#ifdef RAPTOR_DEBUG_MODE_GENERATION
	Raptor::GetErrorManager()->generateRaptorError(CObject3DContour::CObject3DContourClassID::GetClassId(),
		CRaptorErrorManager::RAPTOR_WARNING,
		"CObject3DContour::operator= must not be called");
#endif
	return *this;
}

void CObject3DContour::boundingBox(GL_COORD_VERTEX &Min, GL_COORD_VERTEX &Max)
{
	if (m_pOrigin != NULL)
		m_pOrigin->boundingBox()->get(Min, Max);
}

const CBoundingBox * const CObject3DContour::getBoundingBox() const
{
	if (m_pOrigin != NULL)
		return m_pOrigin->boundingBox();
	else
		return NULL;
}

void CObject3DContour::init(CGeometry *geo)
{
#ifdef RAPTOR_DEBUG_MODE_GENERATION
    if (geo == NULL)
    {
		Raptor::GetErrorManager()->generateRaptorError(	CObject3DContour::CObject3DContourClassID::GetClassId(),
														CRaptorErrorManager::RAPTOR_ERROR,
				            	    					"An Object3DContour cannot be initialized with a null geometry");
        return;
    }
#endif

	if (m_pOrigin != NULL)
    {
        m_pOrigin->removeContainerNotifier(m_pObserver);
        delete m_pOrigin;
    }
	
    m_pOrigin = new CGeometry();
	*m_pOrigin = *geo;
    m_pOrigin->addContainerNotifier(m_pObserver);

    const CGeometryEditor &pEditor = m_pOrigin->getEditor();
	if (!pEditor.isMinimized())
		pEditor.minimize();
}


void CObject3DContour::init(C3DSet *set)
{
#ifdef RAPTOR_DEBUG_MODE_GENERATION
    if (set == NULL)
    {
		Raptor::GetErrorManager()->generateRaptorError(	CObject3DContour::CObject3DContourClassID::GetClassId(),
                                                        CRaptorErrorManager::RAPTOR_ERROR,
				            	    					"An Object3DContour cannot be initialized with a null set");
        return;
    }
#endif

	if (m_pOrigin != NULL)
    {
        m_pOrigin->removeContainerNotifier(m_pObserver);
        delete m_pOrigin;
    }

    m_pOrigin = new CGeometry();
    m_pOrigin->addContainerNotifier(m_pObserver);

    C3DSet::C3DSetIterator it = set->getIterator();
    CObject3D *pChild = set->getChild(it++);

    // First pass, count elements
    unsigned int nbVertex = 0;
    unsigned int nbPolys = 0;
    while (pChild != NULL)
    {
		if (pChild->getId().isSubClassOf(CGeometry::CGeometryClassID::GetClassId()))
        {
            CGeometry *geo = static_cast<CGeometry*>(pChild);
            nbVertex += geo->nbVertex();
            nbPolys += geo->nbFace();
        }
        else
        {
            pChild = NULL;
        }
        pChild = set->getChild(it++);
    }

    m_pOrigin->glSetVertices(nbVertex);
    m_pOrigin->glSetPolygons(nbPolys);
    m_pOrigin->glLockData();

    unsigned int vertexOffset = 0;
    it = set->getIterator();
    pChild = set->getChild(it++);

    while (pChild != NULL)
    {
		if (pChild->getId().isSubClassOf(CGeometry::CGeometryClassID::GetClassId()))
        {
            CGeometry *geo = static_cast<CGeometry*>(pChild);
            geo->glLockData();
     
            GL_VERTEX_DATA V;
            unsigned int i=0;
            for (i=0;i<geo->nbVertex();i++)
            {
                geo->getVertex(i,V);
                m_pOrigin->addVertex(V.vertex.x,V.vertex.y,V.vertex.z,V.vertex.h);
            }

            for (i=0;i<geo->nbFace();i++)
            {
                unsigned int p1,p2,p3;
                geo->getFace(i,p1,p2,p3);
                m_pOrigin->addFace(p1+vertexOffset,p2+vertexOffset,p3+vertexOffset);
            }

            vertexOffset += geo->nbVertex();
            geo->glUnLockData();
        }
        
        pChild = set->getChild(it++);
    }

    m_pOrigin->glUnLockData();

    const CGeometryEditor &pEditor = m_pOrigin->getEditor();
	if (!pEditor.isMinimized())
		pEditor.minimize();
}

void CObject3DContour::prepareContour(void)
{
	if (m_pContour->pContourNormals != NULL)
		CHostMemoryManager::GetInstance()->release(m_pContour->pContourNormals);
	
	//	Here we could collect faces from a complex set of geometries
	//	by computing the sum of the whole thing
	unsigned int nbNormals = m_pOrigin->nbFace();
	m_pContour->pContourNormals = (GL_COORD_VERTEX*)(CHostMemoryManager::GetInstance()->allocate(sizeof(GL_COORD_VERTEX),nbNormals,16));

	GL_COORD_VERTEX v1,v2,v3;
	unsigned int p1,p2,p3;
    m_pOrigin->glLockData();

	for (unsigned int i=0;i<nbNormals;i++)
	{
		m_pOrigin->getFace(i,p1,p2,p3);
		m_pOrigin->getCoord(p1,v1);
		m_pOrigin->getCoord(p2,v2);
		m_pOrigin->getCoord(p3,v3);
		C3DEngine::TriangleNormal(	v1, v2, v3,
									m_pContour->pContourNormals[i]);
	}

	if (m_pContour->backfaces != NULL)
		delete [] m_pContour->backfaces;
	m_pContour->backfaces = new bool[nbNormals];

	if (m_pContour->darkcap != NULL)
		delete [] m_pContour->darkcap;
	m_pContour->darkcap = new unsigned int [3*nbNormals];
	if (m_pContour->lightcap != NULL)
		delete [] m_pContour->lightcap;
	m_pContour->lightcap = new unsigned int [3*nbNormals];

	if (m_pContour->volume != NULL)
		delete [] m_pContour->volume;
	m_pContour->volume = new unsigned int [4*nbNormals];

	if (m_pContour->pContourVolume != NULL)
		delete [] m_pContour->pContourVolume;
	m_pContour->pContourVolume = new GL_COORD_VERTEX[m_pOrigin->nbVertex()*2];

	m_pContour->contourVolumeSize = m_pOrigin->nbVertex();
	for (unsigned int i=0;i<m_pOrigin->nbVertex();i++)
	{
		m_pOrigin->getCoord(i,v1);
		m_pContour->pContourVolume[i] = v1;//m_pOrigin->VERTEX(i);
	}

    if (m_pContour->pContourFaces != NULL)
        delete [] m_pContour->pContourFaces;
    m_pContour->pContourFaces = new unsigned short [3*nbNormals];
    for (unsigned int i=0;i<nbNormals;i++)
    {
		m_pOrigin->getFace(i,p1,p2,p3);
        m_pContour->pContourFaces[3*i] = p1;
		m_pContour->pContourFaces[3*i+1] = p2;
		m_pContour->pContourFaces[3*i+2] = p3;
    }

    findEdges();

    m_pOrigin->glUnLockData();
}

void CObject3DContour::findEdges()
{
	m_pContour->edges.clear();

	unsigned int nbFace = m_pOrigin->nbFace();
	unsigned int index = 0;

	// first pass : init all edges
	for (unsigned int i=0;i<nbFace;i++)
	{
		unsigned int p1,p2,p3;

        p1 = m_pContour->pContourFaces[index];
		p2 = m_pContour->pContourFaces[index+1];
		p3 = m_pContour->pContourFaces[index+2];
		index += 3;

		if ((p1 == p2) || (p2 == p3) || (p3 == p1))
			continue;

		CContourAttributes::line l1(p1,p2);
		CContourAttributes::edge &e1 = m_pContour->edges[l1];

		if (e1.point[0] == -1)
		{
			e1.point[0] = p1;
			e1.point[1] = p2;
		}
		if ((e1.point[0] == p1) && (e1.point[1] == p2))
			e1.front = i;
		else if ((e1.point[0] == p2) && (e1.point[1] == p1))
			e1.back = i;
		
#ifdef RAPTOR_DEBUG_MODE_GENERATION
		if (e1.back == e1.front)
		{
			Raptor::GetErrorManager()->generateRaptorError(	CObject3DContour::CObject3DContourClassID::GetClassId(),
															CRaptorErrorManager::RAPTOR_ERROR,
															"Raptor cannot build contour of an invalid geometry");
		}
#endif
		

		CContourAttributes::line l2(p2,p3);
		CContourAttributes::edge &e2 = m_pContour->edges[l2];

		if (e2.point[0] == -1)
		{
			e2.point[0] = p2;
			e2.point[1] = p3;
		}
		if ((e2.point[0] == p2) && (e2.point[1] == p3))
			e2.front = i;
		else if ((e2.point[0] == p3) && (e2.point[1] == p2))
			e2.back = i;

#ifdef RAPTOR_DEBUG_MODE_GENERATION
		if (e2.back == e2.front)
		{
			Raptor::GetErrorManager()->generateRaptorError(	CObject3DContour::CObject3DContourClassID::GetClassId(),
															CRaptorErrorManager::RAPTOR_ERROR,
															"Raptor cannot build contour of an invalid geometry");
		}
#endif
		

		CContourAttributes::line l3(p3,p1);
		CContourAttributes::edge &e3 = m_pContour->edges[l3];

		if (e3.point[0] == -1)
		{
			e3.point[0] = p3;
			e3.point[1] = p1;
		}
		if ((e3.point[0] == p3) && (e3.point[1] == p1))
			e3.front = i;
		else if ((e3.point[0] == p1) && (e3.point[1] == p3))
			e3.back = i;
	
#ifdef RAPTOR_DEBUG_MODE_GENERATION
		if (e3.back == e3.front)
		{
            Raptor::GetErrorManager()->generateRaptorError(	CObject3DContour::CObject3DContourClassID::GetClassId(),
															CRaptorErrorManager::RAPTOR_ERROR,
															"Raptor cannot build contour of an invalid geometry");
		}
#endif

	}

	if (m_pContour->pContourEdges != NULL)
		delete [] m_pContour->pContourEdges;
	m_pContour->pContourEdges = new CContourAttributes::line[m_pContour->edges.size()];
	m_pContour->contourSize = 0;

	m_pContour->edgeList = new CContourAttributes::edge[m_pContour->edges.size()];
	CContourAttributes::EDGE_ITR it = m_pContour->edges.begin();
	unsigned int numEdge = 0;
    bool malformed = false;
	while (it != m_pContour->edges.end())
    {
        const CContourAttributes::edge &edge = (*it++).second;
        if ((edge.back == -1) || (edge.front == -1))
           malformed = true;
		m_pContour->edgeList[numEdge++] = edge;
    }

#ifdef RAPTOR_DEBUG_MODE_GENERATION
    if (malformed)
    {
		Raptor::GetErrorManager()->generateRaptorError(	CObject3DContour::CObject3DContourClassID::GetClassId(),
														CRaptorErrorManager::RAPTOR_ERROR,
														"Geometry given for contour creation is malformed ! rendering will have craks.");
    }
#endif
}


void CObject3DContour::findBackFaces(const GL_COORD_VERTEX &pos)
{
	unsigned int nbFace = m_pOrigin->nbFace();

	unsigned int index = 0;

	unsigned int csize = m_pContour->contourVolumeSize;

	m_pContour->lightCapSize = 0;
	m_pContour->darkCapSize = 0;

	unsigned int *pLightCap = &m_pContour->lightcap[0];
	unsigned int *pDarkCap = &m_pContour->darkcap[0];

	for (unsigned int i = 0; i < nbFace; i++)
	{
		unsigned int p1, p2, p3;

		p1 = m_pContour->pContourFaces[index];
		p2 = m_pContour->pContourFaces[index + 1];
		p3 = m_pContour->pContourFaces[index + 2];

		//	lightVect % nomal should be tested using the mean point 
		//	of the triangle ( p1 + p2 + p3 ) / 3 ...
		float angle = (m_pContour->pContourVolume[p3].x - pos.x) * m_pContour->pContourNormals[i].x +
					  (m_pContour->pContourVolume[p3].y - pos.y) * m_pContour->pContourNormals[i].y +
					  (m_pContour->pContourVolume[p3].z - pos.z) * m_pContour->pContourNormals[i].z;

		if (angle < 0)
		{
			m_pContour->backfaces[i] = false;
			*pLightCap++ = p1;
			*pLightCap++ = p2;
			*pLightCap++ = p3;
		}
		else
		{
			m_pContour->backfaces[i] = true;
			*pDarkCap++ = p1 + csize;
			*pDarkCap++ = p2 + csize;
			*pDarkCap++ = p3 + csize;
		}

		index += 3;
	}

	m_pContour->lightCapSize = (pLightCap - m_pContour->lightcap);
	m_pContour->darkCapSize = (pDarkCap - m_pContour->darkcap);
}

void CObject3DContour::buildVolume(const GL_COORD_VERTEX &pos,float extrusion)
{
    findBackFaces(pos);

    {
	    unsigned int size = 0;
        size_t nbEdges = m_pContour->edges.size();
        unsigned int csize = m_pContour->contourVolumeSize;

        unsigned int *pVolume = &m_pContour->volume[0];

	    for (unsigned int i=0;i<nbEdges;i++)
	    {
		    const CContourAttributes::edge &e = m_pContour->edgeList[i];

		    bool bf = m_pContour->backfaces[e.front];
		    bool bb = bf;
		    if (e.back < 0)
			    bb = !bf;
		    else
			    bb = m_pContour->backfaces[e.back];

		    if ( bf != bb)
		    {
			    if (bf)
			    {
                    *pVolume++ = e.point[0];
		            *pVolume++ = e.point[1];
		            *pVolume++ = e.point[1]+csize;
		            *pVolume++ = e.point[0]+csize;
			    }
			    else
			    {
                    *pVolume++ = e.point[1];
		            *pVolume++ = e.point[0];
		            *pVolume++ = e.point[0]+csize;
		            *pVolume++ = e.point[1]+csize;
			    }
			    size++;
		    }
	    }

	    m_pContour->contourSize = size;
    }

    m_pContour->extrude(pos,extrusion);
    
	m_contourVolume.volume = m_pContour->pContourVolume;
	m_contourVolume.volumeIndexes = m_pContour->volume;
	m_contourVolume.volumeSize = 4*m_pContour->contourSize;
	m_contourVolume.darkCapIndexes = m_pContour->darkcap;
	m_contourVolume.lightCapIndexes = m_pContour->lightcap;
	m_contourVolume.darkCapSize = m_pContour->darkCapSize;
	m_contourVolume.lightCapSize = m_pContour->lightCapSize;
}

void CObject3DContour::clipVolume(bool volumeClipped, bool lightCapClipped, bool darkCapClipped)
{
    m_contourVolume.volumeClipped = volumeClipped;
    m_contourVolume.lightCapClipped = lightCapClipped;
    m_contourVolume.darkCapClipped = darkCapClipped;
}



