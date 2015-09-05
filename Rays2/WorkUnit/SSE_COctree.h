// SSE_COctree.h: interface for the SSE_COctree class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SSE_COCTREE_H__D63279D6_4259_4346_82CA_039C8D6DB5E3__INCLUDED_)
#define AFX_SSE_COCTREE_H__D63279D6_4259_4346_82CA_039C8D6DB5E3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SSE_BoundingBox.h"
#include "SSE_CTriangle.h"

class SSE_COctree : public SSE_CBoundingBox
{
public:
	SSE_COctree(unsigned int nbPolygons,SSE_CBoundingBox *size);
	virtual ~SSE_COctree();

	float Intersect( CGenericRay &ray,CArray<SSE_CTriangle*,SSE_CTriangle*&> &triangles );

	bool Intersect( SSE_CTriangle *&t );

	void AddTriangle(CArray<SSE_CTriangle*,SSE_CTriangle*&> &triangles,unsigned int i);

	bool Subdivide(CArray<SSE_CTriangle*,SSE_CTriangle*&> &triangles);

	int GetNbChild(void);
	int GetNbEmpty(void);
	void RemoveEmpty(void);

	friend	ostream& operator<< (ostream&, const SSE_COctree&) ;

public:
	int				m_idxIntersected;
	static float	m_intersected;

private:
	CArray<SSE_COctree*,SSE_COctree*>	m_childs;

	CDWordArray	m_subMesh;

	SSE_COctree	*m_childInterected[8];
	float		m_childIntersections[8];

	static CGenericVector<float> tmpVect;
	static unsigned int CALIBRATE_POLYGONS;
};

#endif // !defined(AFX_SSE_COCTREE_H__D63279D6_4259_4346_82CA_039C8D6DB5E3__INCLUDED_)
