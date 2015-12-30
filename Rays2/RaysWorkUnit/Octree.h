// Octree.h: interface for the COctree class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OCTREE_H__96F94B45_415D_4B02_80DE_C46B01EBE4C1__INCLUDED_)
#define AFX_OCTREE_H__96F94B45_415D_4B02_80DE_C46B01EBE4C1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if !defined(AFX_BOUNDINGBOX_H__DB24F01C_80B9_11D3_97C1_FC2841000000__INCLUDED_)
	#include "Engine/BoundingBox.h"
#endif

#include "Triangle.h"

class COctree : public CBoundingBox  
{
public:
	COctree(unsigned int nbPolygons,CBoundingBox *size);
	virtual ~COctree();

	float Intersect( CGenericRay &ray,CArray<CTriangle*,CTriangle*&> &triangles );

	bool Intersect( CTriangle *&t );

	void AddTriangle(CArray<CTriangle*,CTriangle*&> &triangles,unsigned int i);

	bool Subdivide(CArray<CTriangle*,CTriangle*&> &triangles);

	int GetNbChild(void);
	int GetNbEmpty(void);
	void RemoveEmpty(void);

	friend	ostream& operator<< (ostream&, const COctree&) ;

public:
	int				m_idxIntersected;
	static float	m_intersected;

private:
	CArray<COctree*,COctree*>	m_childs;

	CDWordArray	m_subMesh;

	COctree	*m_childInterected[8];
	float	m_childIntersections[8];

	static CGenericVector<float> tmpVect;
	static unsigned int CALIBRATE_POLYGONS;
};

#endif // !defined(AFX_OCTREE_H__96F94B45_415D_4B02_80DE_C46B01EBE4C1__INCLUDED_)
