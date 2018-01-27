// SSE_COctree.cpp: implementation of the SSE_COctree class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RaysWorkUnit.h"
#include "SSE_COctree.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
//
	CGenericVector<float> SSE_COctree::tmpVect;

	unsigned int SSE_COctree::CALIBRATE_POLYGONS = 16;

	float SSE_COctree::m_intersected = HUGE_REAL;

//
//////////////////////////////////////////////////////////////////////

SSE_COctree::SSE_COctree(unsigned int nbPolygons,const CSSE_BoundingBox &size):
m_idxIntersected(-1)
{
	add(size);

	if (nbPolygons > CALIBRATE_POLYGONS)
	{
		int nbP = nbPolygons >> 3;
		float xmin = size.xMin();
		float ymin = size.yMin();
		float zmin = size.zMin();
		float xmax = size.xMax();
		float ymax = size.yMax();
		float zmax = size.zMax();
		
		CSSE_BoundingBox bbox;
		float half_x = 0.5f * (xmin + xmax);
		float half_y = 0.5f * (ymin + ymax);
		float half_z = 0.5f * (zmin + zmax);

		bbox.set(xmin,ymin,zmin,half_x,half_y,half_z);
		m_childs.push_back(new SSE_COctree(nbP,bbox));

		bbox.set(half_x,ymin,zmin,xmax,half_y,half_z);
		m_childs.push_back(new SSE_COctree(nbP,bbox));

		bbox.set(xmin,ymin,half_z,half_x,half_y,zmax);
		m_childs.push_back(new SSE_COctree(nbP,bbox));

		bbox.set(half_x,ymin,half_z,xmax,half_y,zmax);
		m_childs.push_back(new SSE_COctree(nbP,bbox));

		bbox.set(xmin,half_y,zmin,half_x,ymax,half_z);
		m_childs.push_back(new SSE_COctree(nbP,bbox));

		bbox.set(half_x,half_y,zmin,xmax,ymax,half_z);
		m_childs.push_back(new SSE_COctree(nbP,bbox));

		bbox.set(xmin,half_y,half_z,half_x,ymax,zmax);
		m_childs.push_back(new SSE_COctree(nbP,bbox));

		bbox.set(half_x,half_y,half_z,xmax,ymax,zmax);
		m_childs.push_back(new SSE_COctree(nbP,bbox));
	}
}

SSE_COctree::~SSE_COctree()
{
	unsigned int nbChilds = m_childs.size();

	for (unsigned int i=0 ; i<nbChilds ; i++)
		delete m_childs[i];
}

float SSE_COctree::Intersect( CGenericRay &ray,std::vector<SSE_CTriangle*> &triangles)
{
	float	hit = HUGE_REAL;
	float	t;

	m_idxIntersected = -1;

	//	Intersect childs if there are any
	unsigned int nbChilds = m_childs.size();

	if (nbChilds > 0)
	{
		int pos = 0;

		for (unsigned int i=0 ; i<nbChilds ; i++)
		{
			GL_COORD_VERTEX O(ray.origin.X(), ray.origin.Y(), ray.origin.Z());
			GL_COORD_VERTEX D(ray.direction.X(), ray.direction.Y(), ray.direction.Z());
			t = m_childs[i]->CSSE_BoundingBox::intersect(O,D);

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
		for (unsigned int i=0;i<m_subMesh.size();i++)
		{
			int j = m_subMesh[i];
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
	if (isInBox(t->a))
		return true;
	if (isInBox(t->b))
		return true;
	if (isInBox(t->c))
		return true;

	//	Second case : an edge of the triangle intersects
	//	the cube
	float intersected=HUGE_REAL;

	{
		GL_COORD_VERTEX O(t->a.X(), t->a.Y(), t->a.Z());
		GL_COORD_VERTEX D(t->b.X() - t->a.X(), t->b.Y() - t->a.Y(), t->b.Z() - t->a.Z());
		intersected = CSSE_BoundingBox::intersect(O, D);
	}

	if ((intersected >= 0.0f) && (intersected <= 1.0))
		return true;

	{
		GL_COORD_VERTEX O(t->b.X(), t->b.Y(), t->b.Z());
		GL_COORD_VERTEX D(t->c.X() - t->b.X(), t->c.Y() - t->b.Y(), t->c.Z() - t->b.Z());
		intersected = CSSE_BoundingBox::intersect(O, D);
	}

	if ((intersected >= 0.0f) && (intersected <= 1.0))
		return true;

	{
		GL_COORD_VERTEX O(t->c.X(), t->c.Y(), t->c.Z());
		GL_COORD_VERTEX D(t->a.X() - t->c.X(), t->a.Y() - t->c.Y(), t->a.Z() - t->c.Z());
		intersected = CSSE_BoundingBox::intersect(O, D);
	}

	if ((intersected >= 0.0f) && (intersected <= 1.0))
		return true;

	//	Third case : plane intersection


	return false;
}

void SSE_COctree::AddTriangle(std::vector<SSE_CTriangle*> &triangles,unsigned int i)
{
	//	Add triangle to each child if they exist
	SSE_CTriangle *&t = triangles[i];

	unsigned int nbChilds = m_childs.size();

	if (nbChilds > 0)
	{
		bool added = false;

		for (unsigned int j=0 ; j<nbChilds ; j++)
		{
			if (m_childs[j]->Intersect(t))
			{
				m_childs[j]->AddTriangle(triangles,i);
				added = true;
			}
		}

		if (!added)
			std::cout << "Triangle lost in Octree ! " << std::endl;
	}
	//	Add triangle to this
	else
	{
		m_subMesh.push_back(i);
	}
}


bool SSE_COctree::Subdivide(std::vector<SSE_CTriangle*> &triangles)
{
	unsigned int nbChilds = m_childs.size();

	if (nbChilds > 0)
	{
		bool res = false;

		for (unsigned int i=0; i<nbChilds ; i++)
			res |= m_childs[i]->Subdivide(triangles);

		return res;
	}
	else
	{
		//	Subdivide if necessary
		if (m_subMesh.size() > CALIBRATE_POLYGONS)
		{
//			COUT << "Subdivide octree: " << m_subMesh.GetSize() << " polygons" << endl;

			float xmin = xMin();
			float ymin = yMin();
			float zmin = zMin();
			float xmax = xMax();
			float ymax = yMax();
			float zmax = zMax();

			CSSE_BoundingBox bbox;
			float half_x = 0.5f * (xmin + xmax);
			float half_y = 0.5f * (ymin + ymax);
			float half_z = 0.5f * (zmin + zmax);

			bbox.set(xmin,ymin,zmin,half_x,half_y,half_z);
			m_childs.push_back(new SSE_COctree(0,bbox));

			bbox.set(half_x,ymin,zmin,xmax,half_y,half_z);
			m_childs.push_back(new SSE_COctree(0,bbox));

			bbox.set(xmin,ymin,half_z,half_x,half_y,zmax);
			m_childs.push_back(new SSE_COctree(0,bbox));

			bbox.set(half_x,ymin,half_z,xmax,half_y,zmax);
			m_childs.push_back(new SSE_COctree(0,bbox));

			bbox.set(xmin,half_y,zmin,half_x,ymax,half_z);
			m_childs.push_back(new SSE_COctree(0,bbox));

			bbox.set(half_x,half_y,zmin,xmax,ymax,half_z);
			m_childs.push_back(new SSE_COctree(0,bbox));

			bbox.set(xmin,half_y,half_z,half_x,ymax,zmax);
			m_childs.push_back(new SSE_COctree(0,bbox));

			bbox.set(half_x,half_y,half_z,xmax,ymax,zmax);
			m_childs.push_back(new SSE_COctree(0,bbox));

			//	Reinsert triangles after subdivision
			for (unsigned int j=0; j<m_subMesh.size();j++)
				AddTriangle(triangles,m_subMesh[j]);
			
			m_subMesh.clear();

			return true;
		}
		else
			return false;
	}
}


int SSE_COctree::GetNbChild(void)
{
	unsigned int nbChilds = m_childs.size();

	if (nbChilds > 0)
	{
		int res = 0;

		for (unsigned int i=0;i<nbChilds;i++)
			res += m_childs[i]->GetNbChild();

		return nbChilds + res;
	}
	else
		return 1;
}

int SSE_COctree::GetNbEmpty(void)
{
	unsigned int nbChilds = m_childs.size();

	if (nbChilds > 0)
	{
		int res = 0;

		for (unsigned int i=0;i<nbChilds;i++)
			res += m_childs[i]->GetNbEmpty();

		return res;
	}
	else
	{
		if (m_subMesh.size() > 0)
			return 0;
		else
			return 1;
	}
}

void SSE_COctree::RemoveEmpty(void)
{
	unsigned int nbChilds = m_childs.size();
	bool to_delete = false;

	if (nbChilds > 0)
	{
		for (unsigned int i=0;i<nbChilds;i++)
		{
			unsigned int nbSubChilds = m_childs[i]->m_childs.size();

			if (nbSubChilds > 0)
				m_childs[i]->RemoveEmpty();
			else
			{
				if (m_childs[i]->m_subMesh.size() == 0)
				{
					to_delete = true;
					delete m_childs[i];
					m_childs[i] = NULL;
				}
			}
		}

		if (to_delete)
		{
			unsigned int pos = nbChilds - 1;
			while (pos >= 0)
			{
				if (m_childs[pos] == NULL)
					m_childs.erase(m_childs.begin()+pos);

				pos--;
			}
		}
	}
}

