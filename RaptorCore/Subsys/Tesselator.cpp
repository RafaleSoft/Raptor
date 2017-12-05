// Tesselator.cpp: implementation of the CTesselator class.
//
//////////////////////////////////////////////////////////////////////

#include "Subsys/CodeGeneration.h"

#include "SimdLib/Vector4f.cxx"
#include "Tesselator.h"

#include <float.h>		// for FLT_EPSILON

RAPTOR_NAMESPACE

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTesselator::CTesselator()
{
	m_nIndex = NULL;
	m_triangleIndexes = NULL;
	m_nMaxPoints = 0;
	normal.Set(0.0f,0.0f,1.0f,1.0f);
}

CTesselator::~CTesselator()
{
	if( m_nIndex ) 
		delete [] m_nIndex;
	if( m_triangleIndexes ) 
		delete [] m_triangleIndexes;
}


void CTesselator::allocIndex(int nCount)
{
	if( nCount > m_nMaxPoints )
	{
		if( m_nIndex ) delete [] m_nIndex;
		if( m_triangleIndexes ) delete [] m_triangleIndexes;

		m_nMaxPoints   = nCount+2;
		m_nIndex       = new int[m_nMaxPoints];
		m_triangleIndexes = new int[m_nMaxPoints*3];
	}

	m_nTriangle = 0;
	for( register int i=0 ; i < nCount ; i++ )
		m_nIndex[i] = i;
}

void tessBeginContour(void)
{
}


void CTesselator::addFace(	const CVector4f*,
							int i,int j,int k)
{
	m_triangleIndexes[3*m_nTriangle] = i;
	m_triangleIndexes[3*m_nTriangle+1] = j;
	m_triangleIndexes[3*m_nTriangle+2] = k;
}

int CTesselator::triangleArea(const CVector4f* points,
							  int i,int j,int k)
{
	m_e0=points[i];  m_e0-=points[k];
	m_e1=points[j];  m_e1-=points[k];
	//
	m_N = m_e0 ^ m_e1;
	//
	m_A = m_N % m_N;
	//
	// j is alligned from i to k ?
	//
	if( (-FLT_EPSILON) < m_A && m_A < FLT_EPSILON )
		return degenerate;
	//
	// test convexity :
	//
	return ( m_N % normal < 0.0 ? concave : convex );
}


bool CTesselator::isPointInside(const CVector4f& point,
								const CVector4f& q2) //const
{
	pmq2 = point;
	pmq2 -= q2;
	
	//
	float    B0,B1;
	//
	ntmp = pmq2 ^ m_e1;

	if( (B0 = m_N % ntmp) <= 0.0 ) 
		return false;


	ntmp = m_e0 ^ pmq2;

	if( (B1 = m_N % ntmp) <= 0.0 ) 
		return false;

	return ( (m_A-B0-B1) > 0.0 ? true : false );
}

bool CTesselator::isAnyPointInside(const CVector4f* points,
								   int i,int,int k,int nVertex)
{
	int   ik=m_nIndex[k];

	for( register int ip=0 ; ip < nVertex ; ip++ )
	{
		if( ( ip < i || ip > k ) &&
			isPointInside(points[m_nIndex[ip]],points[ik]) )
		{
			return true;
		}
	}

	return false;
}

int CTesselator::removeVertex( int j,int nVertex )
{
	while( ++j < nVertex )
		m_nIndex[j-1]=m_nIndex[j];

	return (nVertex-1);
}

int CTesselator::triangulate(const CVector4f* points,
							 int nCount)
{
	//
	// Alloca un vettore di interi ...
	//
	int   nVertex  = nCount;
	//
	allocIndex(nCount);
	//
	bool     bNoErrors   = true;
	//
	while( nVertex > 3 && bNoErrors )
	{
		//
		// tri to remove one vertex...
		//
		bNoErrors   = false;
		//
		for( int i=0 , j=1 , k=2 ; k < nVertex ; )
		{
			//
			switch( triangleArea(points      ,
				 m_nIndex[i] ,
				 m_nIndex[j] ,
				 m_nIndex[k] ) )
			{
				//
				// ok. flush face && remove vertex j
				//
				case convex:
				{
					//
					// Testing containement
					//
					if( isAnyPointInside(points,i,j,k,nVertex) )
					{
						//
						// go ahead..
						//
						i  = j;
						j  = k;
						k++;
					}
					else
					{
						addFace(points,m_nIndex[i],m_nIndex[j],m_nIndex[k]);
						m_nTriangle++;
						//
						// remove vertex j
						//
						nVertex  = removeVertex( j,nVertex );
						bNoErrors= true;
					}
					break;
				}
				case concave:
				{
					//
					// go ahead..
					//
					i  = j;
					j  = k;
					k++;
					break;
				}
				case degenerate:
				{
					//
					// remove vertex j
					//
					nVertex  = removeVertex( j,nVertex );
					bNoErrors= true;
					break;
				}
			}
		}
	}

	return m_nTriangle;
}
