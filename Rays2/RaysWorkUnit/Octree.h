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
	COctree(unsigned int nbPolygons,const CBoundingBox &size);
	virtual ~COctree();

	float Intersect( CGenericRay &ray,vector<CTriangle*> &triangles );

	bool Intersect( CTriangle *&t );

	void AddTriangle(vector<CTriangle*> &triangles,unsigned int i);

	bool Subdivide(vector<CTriangle*> &triangles);

	int GetNbChild(void);
	int GetNbEmpty(void);
	void RemoveEmpty(void);


public:
	int				m_idxIntersected;
	static float	m_intersected;

private:
	vector<COctree*>	m_childs;
	vector<unsigned int>	m_subMesh;

	COctree	*m_childInterected[8];
	float	m_childIntersections[8];

	static CGenericVector<float> tmpVect;
	static unsigned int CALIBRATE_POLYGONS;
};

#endif // !defined(AFX_OCTREE_H__96F94B45_415D_4B02_80DE_C46B01EBE4C1__INCLUDED_)
