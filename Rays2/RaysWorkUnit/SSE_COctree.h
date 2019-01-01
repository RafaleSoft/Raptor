// SSE_COctree.h: interface for the SSE_COctree class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SSE_COCTREE_H__D63279D6_4259_4346_82CA_039C8D6DB5E3__INCLUDED_)
#define AFX_SSE_COCTREE_H__D63279D6_4259_4346_82CA_039C8D6DB5E3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SSE_CTriangle.h"

class SSE_COctree : public CSSE_BoundingBox
{
public:
	SSE_COctree(unsigned int nbPolygons,const CSSE_BoundingBox &size);
	virtual ~SSE_COctree();

	float Intersect( CGenericRay &ray,std::vector<SSE_CTriangle*> &triangles );

	bool Intersect( SSE_CTriangle *&t );

	void AddTriangle(std::vector<SSE_CTriangle*> &triangles,unsigned int i);

	bool Subdivide(std::vector<SSE_CTriangle*> &triangles);

	int GetNbChild(void);
	int GetNbEmpty(void);
	void RemoveEmpty(void);


public:
	int				m_idxIntersected;
	static float	m_intersected;

private:
	vector<SSE_COctree*>	m_childs;

	vector<unsigned int>	m_subMesh;

	SSE_COctree	*m_childInterected[8];
	float		m_childIntersections[8];

	static CGenericVector<float> tmpVect;
	static unsigned int CALIBRATE_POLYGONS;
};

#endif // !defined(AFX_SSE_COCTREE_H__D63279D6_4259_4346_82CA_039C8D6DB5E3__INCLUDED_)
