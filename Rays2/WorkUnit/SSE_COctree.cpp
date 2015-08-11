// SSE_COctree.cpp: implementation of the SSE_COctree class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "WorkUnit.h"
#include "SSE_COctree.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
//
	CGenericVector<float> SSE_COctree::tmpVect;

	unsigned int SSE_COctree::CALIBRATE_POLYGONS = 16;

	float SSE_COctree::m_intersected = HUGE_REAL;

//
//////////////////////////////////////////////////////////////////////

SSE_COctree::SSE_COctree(unsigned int nbPolygons,SSE_CBoundingBox *size):
m_idxIntersected(-1)
{
	Add(size);

	if (nbPolygons > CALIBRATE_POLYGONS)
	{
		int nbP = nbPolygons >> 3;
		float xmin,ymin,zmin,xmax,ymax,zmax;
		size->Get(xmin,ymin,zmin,xmax,ymax,zmax);
		
		SSE_CBoundingBox bbox;
		float half_x = 0.5f * (xmin + xmax);
		float half_y = 0.5f * (ymin + ymax);
		float half_z = 0.5f * (zmin + zmax);

		bbox.Set(xmin,ymin,zmin,half_x,half_y,half_z);
		m_childs.Add(new SSE_COctree(nbP,&bbox));

		bbox.Set(half_x,ymin,zmin,xmax,half_y,half_z);
		m_childs.Add(new SSE_COctree(nbP,&bbox));

		bbox.Set(xmin,ymin,half_z,half_x,half_y,zmax);
		m_childs.Add(new SSE_COctree(nbP,&bbox));

		bbox.Set(half_x,ymin,half_z,xmax,half_y,zmax);
		m_childs.Add(new SSE_COctree(nbP,&bbox));

		bbox.Set(xmin,half_y,zmin,half_x,ymax,half_z);
		m_childs.Add(new SSE_COctree(nbP,&bbox));

		bbox.Set(half_x,half_y,zmin,xmax,ymax,half_z);
		m_childs.Add(new SSE_COctree(nbP,&bbox));

		bbox.Set(xmin,half_y,half_z,half_x,ymax,zmax);
		m_childs.Add(new SSE_COctree(nbP,&bbox));

		bbox.Set(half_x,half_y,half_z,xmax,ymax,zmax);
		m_childs.Add(new SSE_COctree(nbP,&bbox));
	}
}

SSE_COctree::~SSE_COctree()
{
	int nbChilds = m_childs.GetSize();

	for (int i=0 ; i<nbChilds ; i++)
		delete m_childs[i];
}

float SSE_COctree::Intersect( CGenericRay &ray,CArray<SSE_CTriangle*,SSE_CTriangle*&> &triangles)
{
	float	hit = HUGE_REAL;
	float	t;

	m_idxIntersected = -1;

	//	Intersect childs if there are any
	int nbChilds = m_childs.GetSize();

	if (nbChilds > 0)
	{
		int pos = 0;

		for (int i=0 ; i<nbChilds ; i++)
		{
			t = m_childs[i]->SSE_CBoundingBox::Intersect(ray);

			if (t < m_intersected)
			{
				m_childIntersections[pos] = t;
				m_childInterected[pos++] = m_childs[i];
			}
		}

		if (pos > 0)
		{
			m_childInterected[pos] = NULL;

			bool ordered = false;
			SSE_COctree*	ot = NULL;

			while (!ordered)
			{
				ordered = true;
				for (int i=0;i<pos-1;i++)
				{
					if (m_childIntersections[i+1]<m_childIntersections[i])
					{
						t = m_childIntersections[i];
						m_childIntersections[i] = m_childIntersections[i+1];
						m_childIntersections[i+1] = t;

						ot = m_childInterected[i];
						m_childInterected[i] = m_childInterected[i+1];
						m_childInterected[i+1] = ot;

						ordered = false;
					}
				}
			}

			int	pos3 = 0;
			
			while ((pos3 < pos) && (hit == HUGE_REAL))
			{
				t = m_childInterected[pos3]->Intersect(ray,triangles);
				if ((1.0e-3 < t) && (t < hit))
				{
					hit = t;
					m_idxIntersected = m_childInterected[pos3]->m_idxIntersected;
				}
				pos3++;
			}

			if (hit < m_intersected)
			{
				m_intersected = hit;
			}
		}

		return hit;
	}


	//	Otherwise, intersect triangles
	else
	{
		int		j;

		for (int i=0;i<m_subMesh.GetSize();i++)
		{
			j = m_subMesh[i];
			t = triangles[j]->Intersect(ray);
			if ((1.0e-3 < t) && (t < hit))
			{
				hit = t;
				m_idxIntersected = j;
			}
		}

		return hit;
	}
}

bool SSE_COctree::Intersect( SSE_CTriangle *&t )
{
	//	First case : a vertex lies in the cube
	if (IsInBox(t->a))
		return true;
	if (IsInBox(t->b))
		return true;
	if (IsInBox(t->c))
		return true;

	//	Second case : an edge of the triangle intersects
	//	the cube
	CGenericRay	r;
	float intersected=HUGE_REAL;

	r.origin = t->a;
	r.direction = t->b;
	r.direction -= t->a;
	intersected = SSE_CBoundingBox::Intersect(r);

	if ((intersected >= 0.0f) && (intersected <= 1.0))
		return true;

	r.origin = t->b;
	r.direction = t->c;
	r.direction -= t->b;
	intersected = SSE_CBoundingBox::Intersect(r);

	if ((intersected >= 0.0f) && (intersected <= 1.0))
		return true;

	r.origin = t->c;
	r.direction = t->a;
	r.direction -= t->c;
	intersected = SSE_CBoundingBox::Intersect(r);

	if ((intersected >= 0.0f) && (intersected <= 1.0))
		return true;

	//	Third case : plane intersection


	return false;
}

void SSE_COctree::AddTriangle(CArray<SSE_CTriangle*,SSE_CTriangle*&> &triangles,unsigned int i)
{
	//	Add triangle to each child if they exist
	SSE_CTriangle *&t = triangles[i];

	int nbChilds = m_childs.GetSize();

	if (nbChilds > 0)
	{
		bool added = false;

		for (int j=0 ; j<nbChilds ; j++)
		{
			if (m_childs[j]->Intersect(t))
			{
				m_childs[j]->AddTriangle(triangles,i);
				added = true;
			}
		}

		if (!added)
			COUT << "Triangle lost in Octree ! " << endl;
	}
	//	Add triangle to this
	else
	{
		m_subMesh.Add(i);
	}
}


bool SSE_COctree::Subdivide(CArray<SSE_CTriangle*,SSE_CTriangle*&> &triangles)
{
	int nbChilds = m_childs.GetSize();

	if (nbChilds > 0)
	{
		bool res;

		for (int i=0; i<nbChilds ; i++)
			res |= m_childs[i]->Subdivide(triangles);

		return res;
	}
	else
	{
		//	Subdivide if necessary
		if ((unsigned int)m_subMesh.GetSize() > CALIBRATE_POLYGONS)
		{
//			COUT << "Subdivide octree: " << m_subMesh.GetSize() << " polygons" << endl;

			float xmin,ymin,zmin,xmax,ymax,zmax;
			this->Get(xmin,ymin,zmin,xmax,ymax,zmax);
		
			SSE_CBoundingBox bbox;
			float half_x = 0.5f * (xmin + xmax);
			float half_y = 0.5f * (ymin + ymax);
			float half_z = 0.5f * (zmin + zmax);

			bbox.Set(xmin,ymin,zmin,half_x,half_y,half_z);
			m_childs.Add(new SSE_COctree(0,&bbox));

			bbox.Set(half_x,ymin,zmin,xmax,half_y,half_z);
			m_childs.Add(new SSE_COctree(0,&bbox));

			bbox.Set(xmin,ymin,half_z,half_x,half_y,zmax);
			m_childs.Add(new SSE_COctree(0,&bbox));

			bbox.Set(half_x,ymin,half_z,xmax,half_y,zmax);
			m_childs.Add(new SSE_COctree(0,&bbox));

			bbox.Set(xmin,half_y,zmin,half_x,ymax,half_z);
			m_childs.Add(new SSE_COctree(0,&bbox));

			bbox.Set(half_x,half_y,zmin,xmax,ymax,half_z);
			m_childs.Add(new SSE_COctree(0,&bbox));

			bbox.Set(xmin,half_y,half_z,half_x,ymax,zmax);
			m_childs.Add(new SSE_COctree(0,&bbox));

			bbox.Set(half_x,half_y,half_z,xmax,ymax,zmax);
			m_childs.Add(new SSE_COctree(0,&bbox));

			//	Reinsert triangles after subdivision
			for (int j=0; j<m_subMesh.GetSize();j++)
				AddTriangle(triangles,m_subMesh[j]);
			
			m_subMesh.RemoveAll();

			return true;
		}
		else
			return false;
	}
}


int SSE_COctree::GetNbChild(void)
{
	int nbChilds = m_childs.GetSize();

	if (nbChilds > 0)
	{
		int res = 0;

		for (int i=0;i<nbChilds;i++)
			res += m_childs[i]->GetNbChild();

		return nbChilds + res;
	}
	else
		return 1;
}

int SSE_COctree::GetNbEmpty(void)
{
	int nbChilds = m_childs.GetSize();

	if (nbChilds > 0)
	{
		int res = 0;

		for (int i=0;i<nbChilds;i++)
			res += m_childs[i]->GetNbEmpty();

		return res;
	}
	else
	{
		if (m_subMesh.GetSize() > 0)
			return 0;
		else
			return 1;
	}
}

void SSE_COctree::RemoveEmpty(void)
{
	int nbChilds = m_childs.GetSize();
	bool to_delete = false;

	if (nbChilds > 0)
	{
		for (int i=0;i<nbChilds;i++)
		{
			int nbSubChilds = m_childs[i]->m_childs.GetSize();

			if (nbSubChilds > 0)
				m_childs[i]->RemoveEmpty();
			else
			{
				if (m_childs[i]->m_subMesh.GetSize() == 0)
				{
					to_delete = true;
					delete m_childs[i];
					m_childs[i] = NULL;
				}
			}
		}

		if (to_delete)
		{
			int pos = nbChilds - 1;
			while (pos >= 0)
			{
				if (m_childs[pos] == NULL)
					m_childs.RemoveAt(pos);

				pos--;
			}
		}
	}
}

ostream& operator<< (ostream &out, const SSE_COctree &surf)
{

	out << "pere: nbPolygons = "  << surf.m_subMesh.GetSize() << endl;

	out << "fils:" << endl << "{" << endl;

	int nbChilds = surf.m_childs.GetSize();

	for (int i=0 ; i<nbChilds ; i++)
	{
		out <<"\t"<< *(surf.m_childs[i]);
	}

	out << "}" << endl;

	return out;
}