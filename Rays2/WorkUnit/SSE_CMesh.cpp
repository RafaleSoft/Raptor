// SSE_CMesh.cpp: implementation of the SSE_CMesh class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "WorkUnit.h"
#include "SSE_CMesh.h"
#include "SSE_CTriangle.h"
#include "SSE_COctree.h"
#include "SSE_BoundingBox.h"

#include <float.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
//

//
//////////////////////////////////////////////////////////////////////

SSE_CMesh::SSE_CMesh(rays_composition_header_t &mesh):
	SSE_CRenderObject(mesh.base),
	m_root(NULL),
	m_idxIntersected(-1)
{
}

SSE_CMesh::~SSE_CMesh()
{
	int nbT = m_triangles.GetSize();

	for (int i=0;i<nbT;i++)
		delete m_triangles[i];

	m_triangles.RemoveAll();
}

CGenericVector<float>& SSE_CMesh::GetTexel( const CGenericVector<float> &hit )
{
	if (m_root != NULL)
	{
		if (m_idxIntersected >= 0)
		{
			tmpVect = m_triangles[m_idxIntersected]->GetTexel(hit);
		}
		else
		{
			COUT << "No intersection !" << endl;
		}
	}
	
	return tmpVect;
}


float SSE_CMesh::Intersect( CGenericRay &ray )
{
	if (m_root != NULL)
	{
		m_root->m_intersected = HUGE_REAL;

		float hit = m_root->Intersect(ray,m_triangles);

		m_idxIntersected = m_root->m_idxIntersected;

		return hit;
	}
	else
		return HUGE_REAL;
}

CGenericVector<float>& SSE_CMesh::Normal( const CGenericVector<float> &hit )
{
	if (m_root != NULL)
	{
		if (m_idxIntersected >= 0)
		{
			tmpVect = m_triangles[m_idxIntersected]->Normal(hit);
		}
		else
		{
			COUT << "Error : No intersection !" << endl;
		}
	}

	return tmpVect;
}

void SSE_CMesh::AddTriangle(rays_triangle_t &t,rays_vertex_t *v)
{
	int p1 = t.p1;
	int p2 = t.p2;
	int p3 = t.p3;

	SSE_CTriangle *T = new SSE_CTriangle(v[p1],v[p2],v[p3]);

	bBox->ExtendTo(v[p1].position.x,v[p1].position.y,v[p1].position.z);
	bBox->ExtendTo(v[p2].position.x,v[p2].position.y,v[p2].position.z);
	bBox->ExtendTo(v[p3].position.x,v[p3].position.y,v[p3].position.z);

	int size = m_triangles.GetSize();

	int pmax = MAX(p1, MAX (p2, p3));
	if (m_normals.GetSize() <= pmax)
	{
		int pos = m_normals.GetSize();
		CDWordArray *p_dwArray = NULL;
		while (pos <= pmax)
		{
			p_dwArray = new CDWordArray;
			m_normals.Add(p_dwArray);
			pos++;
		}
	}

	m_normals[p1]->Add(size);
	m_normals[p2]->Add(size);
	m_normals[p3]->Add(size);

	m_indexes.Add(t);
	m_triangles.Add(T);
}

void SSE_CMesh::ReBuildOctree(void)
{
	if (m_root != NULL)
		delete m_root;

	float a,b,c,d,e,f;
	bBox->Get(a,b,c,d,e,f);

	m_root = new SSE_COctree(m_triangles.GetSize(),bBox);

	for (int i=0;i<m_triangles.GetSize();i++)
		m_root->AddTriangle(m_triangles,i);
	
	//	This limit should be configurable :
	//	there should not be more data to
	//	organize "empty" space than to
	//	store mesh data
	int oldsize = 0;
	int size = m_root->GetNbChild();
	while ((oldsize != size) && (size < m_triangles.GetSize()))
	{
		COUT << "Subdivide ... " << endl;
		m_root->Subdivide(m_triangles);
		oldsize = size;
		size = m_root->GetNbChild();
	}


	COUT << "Octree size: " << size << endl;
	COUT << "Octree empty space: " << m_root->GetNbEmpty() << endl;

	m_root->RemoveEmpty();
	m_root->RemoveEmpty();
	size = m_root->GetNbChild();

	COUT << "Octree size: " << size << endl;
	COUT << "Octree empty space: " << m_root->GetNbEmpty() << endl;
}

void SSE_CMesh::ReBuildNormals(float crease_angle)
{
	CArray<CGenericVector<float>,const CGenericVector<float>&> normals;
	CGenericVector<float> normal;
	CGenericVector<float> na;
	CGenericVector<float> nb;
	CGenericVector<float> nc;

	COUT << "Nb triangles: " << m_triangles.GetSize() << endl;
	COUT << "Nb normals: " << m_normals.GetSize() << endl;

	for (int i=0;i<m_normals.GetSize();i++)
	{
		CDWordArray *p_dwArray = m_normals[i];
		normal.Set(0,0,0,0);

		for (int j=0;j<p_dwArray->GetSize();j++)
		{
			int pos = p_dwArray->operator [](j);
			normal += m_triangles[pos]->FlatNormal();
		}
		normal *= (1.0f / p_dwArray->GetSize());
		normal.H() = 1.0f;
		normals.Add(normal);

		delete m_normals[i];
	}

	float crease;
	bool flat;
	int p1,p2,p3;

	for (i=0;i<m_indexes.GetSize();i++)
	{
		flat = true;
		p1 = m_indexes[i].p1;
		p2 = m_indexes[i].p2;
		p3 = m_indexes[i].p3;

		nc = nb = na = normal = m_triangles[i]->FlatNormal();
	
		crease = normal % normals[p1];
		if (crease > crease_angle)
		{
			flat = false;
			na = normals[p1];
		}

		crease = normal % normals[p2];
		if (crease > crease_angle)
		{
			flat = false;
			nb = normals[p2];
		}

		crease = normal % normals[p3];
		if (crease > crease_angle)
		{
			flat = false;
			nc = normals[p3];
		}
	
		//COUT << "Normals:" << endl;
		//COUT << "na: " << na << endl;
		//COUT << "nb: " << nb << endl;
		//COUT << "nc: " << nc << endl;
		//COUT << "n: " << normal << endl;
		//if (flat)
		//	COUT << "Triangle " << i << " is flat" << endl;
		//else
		//	COUT << "Triangle " << i << " is smoothed" << endl;
	
		m_triangles[i]->SetNormals(na,nb,nc,flat);
	}

	m_indexes.RemoveAll();
	m_normals.RemoveAll();
}

void SSE_CMesh::Scale(float sx,float sy,float sz)
{
	float cx,cy,cz;
	float x,y,z;

	bBox->GetCenter(cx,cy,cz);
	bBox->Scale(sx,sy,sz);

	for (int i=0;i<m_triangles.GetSize();i++)
	{
		x = (m_triangles[i]->a.X() - cx) * sx + cx;
		y = (m_triangles[i]->a.Y() - cy) * sy + cy;
		z = (m_triangles[i]->a.Z() - cz) * sz + cz;
		m_triangles[i]->a.Set(x,y,z,1.0f);

		x = (m_triangles[i]->b.X() - cx) * sx + cx;
		y = (m_triangles[i]->b.Y() - cy) * sy + cy;
		z = (m_triangles[i]->b.Z() - cz) * sz + cz;
		m_triangles[i]->b.Set(x,y,z,1.0f);

		x = (m_triangles[i]->c.X() - cx) * sx + cx;
		y = (m_triangles[i]->c.Y() - cy) * sy + cy;
		z = (m_triangles[i]->c.Z() - cz) * sz + cz;
		m_triangles[i]->c.Set(x,y,z,1.0f);

		m_triangles[i]->Init();
	}
}

void SSE_CMesh::Translate(float tx,float ty,float tz)
{
	float x,y,z;

	bBox->Translate(tx,ty,tz);

	for (int i=0;i<m_triangles.GetSize();i++)
	{
		x = m_triangles[i]->a.X() + tx;
		y = m_triangles[i]->a.Y() + ty;
		z = m_triangles[i]->a.Z() + tz;
		m_triangles[i]->a.Set(x,y,z,1.0f);

		x = m_triangles[i]->b.X() + tx;
		y = m_triangles[i]->b.Y() + ty;
		z = m_triangles[i]->b.Z() + tz;
		m_triangles[i]->b.Set(x,y,z,1.0f);

		x = m_triangles[i]->c.X() + tx;
		y = m_triangles[i]->c.Y() + ty;
		z = m_triangles[i]->c.Z() + tz;
		m_triangles[i]->c.Set(x,y,z,1.0f);

		m_triangles[i]->Init();
	}
}

void SSE_CMesh::Rotate(float angle,float ax,float ay,float az)
{
	CGenericMatrix<float> matrix;
	CGenericVector<float> axis(ax,ay,az,1.0f);

	float u = (float)cos(angle * RAD);
	float v = (float)sin(angle * RAD);

	CGenericMatrix<float> uu;

	uu.Ident();
	matrix.Ident();

	uu[0] = axis.X() * axis.X();
	uu[1] = uu[4] = axis.X() * axis.Y();
	uu[2] = uu[8] = axis.X() * axis.Z();
	uu[5] = axis.Y() * axis.Y();
	uu[6] = uu[9] = axis.Y() * axis.Z();
	uu[10] = axis.Z() * axis.Z();

	matrix -=  uu;
	matrix *= u;

	matrix += uu;
	
	uu[0] = uu[5] = uu[10] = 0;
	uu[4] = axis.Z();
	uu[1] = -axis.Z();
	uu[8] = -axis.Y();
	uu[2] = axis.Y();
	uu[9] = axis.X();
	uu[6] = -axis.X();

	uu *= v;
	uu[15] = 0.0f;

	//	matrix = axis.t(axis) + (I - axis.t(axis)).Cos(angle) + S.Sin(angle)
	//	where : 
	//	- t() denotes the transpose
	//	- S = 0  -az  ay
	//        az  0  -ax
	//        -ay ax  0
	matrix += uu;


	float zmin = FLT_MAX;
	float zmax = -FLT_MAX;
	float xmin = FLT_MAX;
	float xmax = -FLT_MAX;
	float ymin = FLT_MAX;
	float ymax = -FLT_MAX;

	for (int i=0;i<m_triangles.GetSize();i++)
	{
		axis.Set(m_triangles[i]->a.X(),m_triangles[i]->a.Y(),m_triangles[i]->a.Z(),1.0f);
		axis *= matrix;
		m_triangles[i]->a.Set(axis.X(),axis.Y(),axis.Z(),1.0f);
		if (axis.X()<xmin) xmin=axis.X(); else if (axis.X()>xmax) xmax=axis.X();
		if (axis.Y()<ymin) ymin=axis.Y(); else if (axis.Y()>ymax) ymax=axis.Y();
		if (axis.Z()<zmin) zmin=axis.Z(); else if (axis.Z()>zmax) zmax=axis.Z();

		axis.Set(m_triangles[i]->b.X(),m_triangles[i]->b.Y(),m_triangles[i]->b.Z(),1.0f);
		axis *= matrix;
		m_triangles[i]->b.Set(axis.X(),axis.Y(),axis.Z(),1.0f);
		if (axis.X()<xmin) xmin=axis.X(); else if (axis.X()>xmax) xmax=axis.X();
		if (axis.Y()<ymin) ymin=axis.Y(); else if (axis.Y()>ymax) ymax=axis.Y();
		if (axis.Z()<zmin) zmin=axis.Z(); else if (axis.Z()>zmax) zmax=axis.Z();

		axis.Set(m_triangles[i]->c.X(),m_triangles[i]->c.Y(),m_triangles[i]->c.Z(),1.0f);
		axis *= matrix;
		m_triangles[i]->c.Set(axis.X(),axis.Y(),axis.Z(),1.0f);
		if (axis.X()<xmin) xmin=axis.X(); else if (axis.X()>xmax) xmax=axis.X();
		if (axis.Y()<ymin) ymin=axis.Y(); else if (axis.Y()>ymax) ymax=axis.Y();
		if (axis.Z()<zmin) zmin=axis.Z(); else if (axis.Z()>zmax) zmax=axis.Z();

		m_triangles[i]->Init();
	}

	bBox->Set(xmin,ymin,zmin,xmax,ymax,zmax);
}