// SSE_CMesh.h: interface for the SSE_CMesh class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SSE_CMESH_H__907096C8_B796_4EE8_9368_076D478E18E1__INCLUDED_)
#define AFX_SSE_CMESH_H__907096C8_B796_4EE8_9368_076D478E18E1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "RaysWorkUnit.h"
#include "SSE_CRenderObject.h"


class SSE_CTriangle;
class SSE_COctree;

class SSE_CMesh : public SSE_CRenderObject  
{
public:
	SSE_CMesh(rays_composition_header_t &mesh);
	virtual ~SSE_CMesh();

	//	Base class implementation
	virtual void Scale(float sx,float sy,float sz);
	virtual void Translate(float tx,float ty,float tz);
	virtual void Rotate(float angle,float ax,float ay,float az);

	virtual CGenericVector<float>& GetTexel(const CGenericVector<float> &hit );

	virtual OBJECTTYPE GetType() const { return MESH; };

	virtual float Intersect( CGenericRay &ray );


	//	CMesh implementation
	void AddTriangle(rays_triangle_t &t,rays_vertex_t *v);

	void ReBuildOctree(void);
	void ReBuildNormals(float crease_angle);

private:
	vector<SSE_CTriangle*>	m_triangles;
	vector<vector<unsigned int>* >	m_normals;
	vector<rays_triangle_t>	m_indexes;
	
	int					m_idxIntersected;
	SSE_COctree			*m_root;

	virtual CGenericVector<float>& Normal( const CGenericVector<float> &hit );
};

#endif // !defined(AFX_SSE_CMESH_H__907096C8_B796_4EE8_9368_076D478E18E1__INCLUDED_)
