// Mesh.h: interface for the CMesh class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MESH_H__13C1CD6D_4257_4B69_9FC5_06160F7C090C__INCLUDED_)
#define AFX_MESH_H__13C1CD6D_4257_4B69_9FC5_06160F7C090C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "WorkUnit.h"
#include "RenderObject.h"

class CTriangle;
class COctree;


class CMesh : public CRenderObject  
{
public:
	CMesh(rays_composition_header_t &mesh);
	virtual ~CMesh();

	virtual void Scale(float sx,float sy,float sz);
	virtual void Translate(float tx,float ty,float tz);
	virtual void Rotate(float angle,float ax,float ay,float az);

	//	Base class implementation
	virtual CGenericVector<float>& GetTexel(const CGenericVector<float> &hit );

	virtual OBJECTTYPE GetType() const { return MESH; };

	virtual float Intersect( CGenericRay &ray );

	//	CMesh implementation
	void AddTriangle(rays_triangle_t &t,rays_vertex_t *v);

	void ReBuildOctree(void);
	void ReBuildNormals(float crease_angle);

private:
	CArray<CTriangle*,CTriangle*&>	
						m_triangles;

	CArray<CDWordArray*, CDWordArray*&>
						m_normals;
	CArray<rays_triangle_t,rays_triangle_t&>
						m_indexes;
	
	int					m_idxIntersected;

	COctree				*m_root;

	virtual CGenericVector<float>& Normal( const CGenericVector<float> &hit );
};

#endif // !defined(AFX_MESH_H__13C1CD6D_4257_4B69_9FC5_06160F7C090C__INCLUDED_)
