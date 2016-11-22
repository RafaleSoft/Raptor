// GeometryEditor.cpp: implementation of the CGeometryEditor class.
//
//////////////////////////////////////////////////////////////////////
#include "Subsys/CodeGeneration.h"

#if !defined(AFX_GEOMETRYEDITOR_H__2D77E428_ED3D_416B_8DE9_DABFD45A38A7__INCLUDED_)
    #include "GeometryEditor.h"
#endif
#if !defined(AFX_GEOMETRY_H__B42ABB87_80E8_11D3_97C2_DE5C28000000__INCLUDED_)
	#include "Geometry.h"
#endif
#if !defined(AFX_GEOMETRYALLOCATOR_H__802B3C7A_43F7_46B2_A79E_DDDC9012D371__INCLUDED_)
	#include "Subsys/GeometryAllocator.h"
#endif
#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
	#include "System/Raptor.h"
#endif
#if !defined(AFX_3DENGINEMATRIX_H__6CD1110E_1174_4f38_A452_30FB312022D0__INCLUDED_)
	#include "Engine/3DEngineMatrix.h"
#endif

RAPTOR_NAMESPACE

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGeometryEditor::CGeometryEditor(CGeometry *pGeometry)
	:m_bMinimized(false),m_bSurfaceElements(false),m_pGeometry(pGeometry)
{
	if (NULL != m_pGeometry->m_pEditor)
		m_pGeometry = NULL;
}

CGeometryEditor::~CGeometryEditor()
{
}

bool CGeometryEditor::destroy()
{
	if (NULL != m_pGeometry)
	{
		if (NULL != m_pGeometry->m_pEditor)
			return false;
		else
			delete this;
	}
	else
		delete this;

	return true;
}

#pragma warning (disable : 4711)	// automatic inline expansion for GenNormals

#define ADD_NORMAL(p1,p2,p3,normal) \
m_pGeometry->NORMAL(p1).x += normal.x; \
m_pGeometry->NORMAL(p2).x += normal.x; \
m_pGeometry->NORMAL(p3).x += normal.x; \
m_pGeometry->NORMAL(p1).y += normal.y; \
m_pGeometry->NORMAL(p2).y += normal.y; \
m_pGeometry->NORMAL(p3).y += normal.y; \
m_pGeometry->NORMAL(p1).z += normal.z; \
m_pGeometry->NORMAL(p2).z += normal.z; \
m_pGeometry->NORMAL(p3).z += normal.z; \
m_pGeometry->NORMAL(p1).h +=1.0f; \
m_pGeometry->NORMAL(p2).h +=1.0f; \
m_pGeometry->NORMAL(p3).h +=1.0f;


void CGeometryEditor::updateSurfaceElements(const vector<CGeometry*> &geometries,
											const vector<CGenericMatrix<float> > &transforms)
{
	if (geometries.size() < 2)
		return;

	for (unsigned int k=0;k<geometries.size();k++)
	{
		CGeometry *g = geometries[k];

		g->vertex = (GL_COORD_VERTEX*)(CGeometryAllocator::GetInstance()->glvkMapPointer((float*)g->vertex));
		g->normals = (GL_COORD_VERTEX*)(CGeometryAllocator::GetInstance()->glvkMapPointer((float*)g->normals));
		g->colors = (CColor::RGBA*)(CGeometryAllocator::GetInstance()->glvkMapPointer((float*)g->colors));
	}

	for (int k=0;k<1/*geometries.size()*/;k++)
	{
		CGeometry *receiver = geometries[k];
		int iMax = receiver->nbVertex();

		C3DEngineMatrix T1(transforms[0]);
		C3DEngineMatrix T2_inv(transforms[1]);
		T2_inv.Inverse();

		C3DEngineMatrix M = T2_inv * T1;
		C3DEngineMatrix M_inv(M);
		M_inv.Transpose().Inv();

		//!    Compute accessibility for each element of each Geometry
		//!		TODO: Use two passes to compensate for occluded emitters
		int i=0;
#pragma omp parallel for default(none) private (i) shared(iMax,receiver,geometries,k,M,M_inv)
		for (i=0;i<iMax;i++)
		{
#if defined (DATA_EXTENDED)
			if (NULL == m_pGeometry->geometry)
#elif defined (DATA_PACKED)
			if (NULL == receiver->colors)
#endif
				continue;

			CGenericVector<float> vi(receiver->VERTEX(i));
			CGenericVector<float> tr(receiver->NORMAL(i));

			vi *= M;
			tr *= M_inv;

			CColor::RGBA &ci = receiver->COLOR(i);
			float FF = ci.a;

			for (unsigned int l=0;l<geometries.size();l++)
			{
				if (l == k)
					continue;

				CGeometry *emiter = geometries[l];
				int jMax = emiter->nbVertex();
				CGenericVector<float> vj;
				CGenericVector<float> te;
				CGenericVector<float> r;

				// j is the emitter
				for (int j=0;j<jMax;j++)
				{
					vj = emiter->VERTEX(j);
					r = vj - vi;

					float cos_tr = (tr % r);
					if (cos_tr > 0)
					{
						te = emiter->NORMAL(j);
						float cos_te = -(te % r);
						if  (cos_te > 0)
						{
							float r2 = r % r;
							float ff = cos_te * cos_tr / r2;
							float A_r2 = te.H() / r2;
							ff *= (1 - 1.0f / sqrt(1 + A_r2));

							FF -= ff;
						}
					}
				}
			}

			ci.r = MAX(0.0f,FF);
		}
	}

// this form factor is not so good indeed :-(
//float ff = cos_te * MAX(1,4*cos_tr) / r2;
// form factor for transmitted light
//float tl = A * cos_te * cos_tr / (PI * r2 + A);
/*
float tl = ff * A_r2 / (1 + A_r2);	//ff * A / (r2 + A);
*/

	for (unsigned int k=0;k<geometries.size();k++)
	{
		CGeometry *g = geometries[k];

		g->vertex = (GL_COORD_VERTEX*)(CGeometryAllocator::GetInstance()->glDiscardPointer((float*)g->vertex));
		g->normals = (GL_COORD_VERTEX*)(CGeometryAllocator::GetInstance()->glDiscardPointer((float*)g->normals));
		g->colors = (CColor::RGBA*)(CGeometryAllocator::GetInstance()->glvkUnMapPointer((float*)g->colors));
	}
}

void CGeometryEditor::computeVertexArea(unsigned short p1,
										unsigned short p2,
										unsigned short p3) const
{
	if ((p1!=p2)&&(p1!=p3)&&(p2!=p3))
	{
		GL_COORD_VERTEX &v1 = m_pGeometry->VERTEX(p1);
		GL_COORD_VERTEX &v2 = m_pGeometry->VERTEX(p2);
		GL_COORD_VERTEX &v3 = m_pGeometry->VERTEX(p3);

		//!	Compute lenght of each edge
		CGenericVector<float> V(v2.x - v1.x, v2.y - v1.y, v2.z - v1.z, 1.0f);
		float a = V.Norm();
		V.Set(v3.x - v2.x, v3.y - v2.y, v3.z - v2.z, 1.0f);
		float b = V.Norm();
		V.Set(v1.x - v3.x, v1.y - v3.y, v1.z - v3.z, 1.0f);
		float c = V.Norm();

		//!	Compute area.
        float s = 0.5f*(a+b+c);
		float A = sqrt(s*(s-a)*(s-b)*(s-c)) / (3*PI);

		//!	Accumulate area on each vertex forming the triangle.
		m_pGeometry->normals[p1].h += A;
		m_pGeometry->normals[p2].h += A;
		m_pGeometry->normals[p3].h += A;
	}
}

void CGeometryEditor::genSurfaceElements(void) const
{
#if defined (DATA_EXTENDED)
    if (m_pGeometry->geometry == NULL)
#elif defined (DATA_PACKED)
	if (m_pGeometry->normals == NULL)
#endif
		return;

	if (m_pGeometry->colors != NULL) CGeometryAllocator::GetInstance()->releaseVertices((float*)m_pGeometry->colors);
	int iMax = m_pGeometry->nbVertex();
	m_pGeometry->colors = (CColor::RGBA*)(CGeometryAllocator::GetInstance()->allocateVertices(4*iMax));

	if ((CGeometryAllocator::GetInstance()->isMemoryRelocated()) &&
		(!m_pGeometry->m_bDataLocked))
	{
		m_pGeometry->vertex = (GL_COORD_VERTEX*)(CGeometryAllocator::GetInstance()->glvkMapPointer((float*)m_pGeometry->vertex));
		m_pGeometry->polys = (unsigned short*)(CGeometryAllocator::GetInstance()->glvkMapPointer(m_pGeometry->polys));
		m_pGeometry->normals = (GL_COORD_VERTEX*)(CGeometryAllocator::GetInstance()->glvkMapPointer((float*)m_pGeometry->normals));
		m_pGeometry->colors = (CColor::RGBA*)(CGeometryAllocator::GetInstance()->glvkMapPointer((float*)m_pGeometry->colors));
	}

	//!    Reinitialize all weights before proceding
    int i=0;
#pragma omp parallel for default(none) private (i) shared(iMax)
	for (i=0;i<iMax;i++)
	{
		m_pGeometry->NORMAL(i).h = 0.0f;
		m_pGeometry->COLOR(i) = CColor::RGBA(1,1,1,1);
	}

	//!    First step, proceed with the area of each triangle
	i = 0;
	unsigned short p1,p2,p3 = 0;
	iMax = 3*m_pGeometry->m_nbPolys;
	GL_COORD_VERTEX* normals = m_pGeometry->normals;

// TODO check this pragma for concurrent access on weights !!!
#pragma omp parallel for default(none) private (i,p1,p2,p3) shared(iMax,normals)
	for (i=0;i<iMax;i+=3)
	{
		p1 = m_pGeometry->polys[i];
		p2 = m_pGeometry->polys[i+1];
		p3 = m_pGeometry->polys[i+2];
		computeVertexArea(p1,p2,p3);
	}

	//!    Second step, proceed with primitives.
	const vector<CGeometryPrimitive*>	primitives = m_pGeometry->getPrimitives();
    vector<CGeometryPrimitive*>::const_iterator	it = primitives.begin();
    while (it != primitives.end())
    {
        CGeometryPrimitive *p = *it++;
        int size = p->getSize();
        unsigned short *faces = new unsigned short[size];
        p->getIndexes(faces);

		switch(p->getKind())
        {
			case CGeometryPrimitive::TRIANGLE:
			{
				p1 = faces[i];
				p2 = faces[i+1];
		        p3 = faces[i+2];
				computeVertexArea(p1,p2,p3);
				break;
			}
			case CGeometryPrimitive::TRIANGLE_STRIP:
			{
				bool reverse = true;
                for (i=0 ; i < size - 2 ; i++)
		        {
                    if (reverse = !reverse)
                    {
                        p1 = faces[i+1];
			            p2 = faces[i];
                    }
                    else
                    {
                        p1 = faces[i];
			            p2 = faces[i+1];
                    }
			        p3 = faces[i+2];
					computeVertexArea(p1,p2,p3);
				}
				break;
			}
			case CGeometryPrimitive::TRIANGLE_FAN:
            {
				p1 = faces[0];
                for (i=0 ; i < size ; i+=2)
		        {
			        p2 = faces[i];
			        p3 = faces[i+1];
			        computeVertexArea(p1,p2,p3);
                }
				break;
            }
		}

		delete [] faces;
	}

	//!    Third step, compute accessibility for each element
	//!		Use two passes to compensate for occluded emitters
	iMax = m_pGeometry->nbVertex();
	for (int nPass=0;nPass<2;nPass++)
#pragma omp parallel for default(none) private (i) shared(iMax,nPass)
	for (i=0;i<iMax;i++)
	{
		bool compensate = (nPass > 0); 

		GL_COORD_VERTEX &vi = m_pGeometry->VERTEX(i);
		CGenericVector<float> tr(m_pGeometry->NORMAL(i));

		float FF = 1;
		CColor::RGBA radiance(0.0f,0.0f,0.0f,1.0f);

		// j is the emitter
		for (int j=0;j<iMax;j++)
		{
			if (i == j)
				continue;

			GL_COORD_VERTEX &vj = m_pGeometry->VERTEX(j);
			CGenericVector<float> r(vj.x - vi.x,vj.y - vi.y,vj.z - vi.z,1.0f);
			CGenericVector<float> te(m_pGeometry->NORMAL(j));

			float A = te.H();

			float cos_tr = (tr % r);
			float cos_te = -(te % r);

			if ((cos_tr > 0) && (cos_te > 0))
			{
				float r2 = r % r;
				// this form factor is not so good indeed :-(
				//float ff = cos_te * MAX(1,4*cos_tr) / r2;
				float ff = cos_te * cos_tr / r2;
				float A_r2 = A / r2;
				// form factor for transmitted light
				//float tl = A * cos_te * cos_tr / (PI * r2 + A);
				float tl = ff * A_r2 / (1 + A_r2);	//ff * A / (r2 + A);
				ff *= (1 - 1.0f / sqrt(1 + A_r2));
				if (compensate)
				{
					CColor::RGBA &c = m_pGeometry->COLOR(j);
					ff *= c.r;
					//radiance = radiance + (tl*c);
				}

				FF -= ff;
			}
		}

		CColor::RGBA &c = m_pGeometry->COLOR(i);
		if (compensate)
			c.a = FF;
		else
			c.r = MAX(0.0f,FF);
	}

	if ((CGeometryAllocator::GetInstance()->isMemoryRelocated()) &&
		(!m_pGeometry->m_bDataLocked))
	{
		m_pGeometry->vertex = (GL_COORD_VERTEX*)(CGeometryAllocator::GetInstance()->glDiscardPointer((float*)m_pGeometry->vertex));
		m_pGeometry->normals = (GL_COORD_VERTEX*)(CGeometryAllocator::GetInstance()->glvkUnMapPointer((float*)m_pGeometry->normals));
		m_pGeometry->polys = (unsigned short*)(CGeometryAllocator::GetInstance()->glDiscardPointer(m_pGeometry->polys));
		m_pGeometry->colors = (CColor::RGBA*)(CGeometryAllocator::GetInstance()->glvkUnMapPointer((float*)m_pGeometry->colors));
	}

	m_bSurfaceElements = true;
}

void CGeometryEditor::genNormals(bool rebuild) const
{
#if defined (DATA_EXTENDED)
    if (m_pGeometry->geometry == NULL)
#elif defined (DATA_PACKED)
	if (m_pGeometry->normals == NULL)
#endif
		return;

	if ((CGeometryAllocator::GetInstance()->isMemoryRelocated()) &&
		(!m_pGeometry->m_bDataLocked))
	{
		m_pGeometry->normals = (GL_COORD_VERTEX*)(CGeometryAllocator::GetInstance()->glvkMapPointer((float*)m_pGeometry->normals));
		m_pGeometry->vertex = (GL_COORD_VERTEX*)(CGeometryAllocator::GetInstance()->glvkMapPointer((float*)m_pGeometry->vertex));
		m_pGeometry->polys = (unsigned short*)(CGeometryAllocator::GetInstance()->glvkMapPointer(m_pGeometry->polys));
	}

	int i=0;
	int iMax = m_pGeometry->nbVertex();

	if (rebuild)
	{
		GL_COORD_VERTEX normal;
		int p1,p2,p3;

        //!    Reinitialize all normals before proceding    
#pragma omp parallel for default(none) private (i) shared(iMax)
		for (i=0;i<iMax;i++)
		{
			m_pGeometry->NORMAL(i).x = 0.0f;
			m_pGeometry->NORMAL(i).y = 0.0f;
			m_pGeometry->NORMAL(i).z = 0.0f;
			m_pGeometry->NORMAL(i).h = 0.0f;
		}

        //!    First step, proceed with first level of polygons
		iMax = 3*m_pGeometry->m_nbPolys;
#pragma omp parallel for default(none) private (i,p1,p2,p3,normal) shared(iMax)
		for (i=0;i<iMax;i+=3)
		{
			p1 = m_pGeometry->polys[i];
			p2 = m_pGeometry->polys[i+1];
			p3 = m_pGeometry->polys[i+2];
			if ((p1!=p2)&&(p1!=p3)&&(p2!=p3))
			{
				C3DEngine::TriangleNormal(	m_pGeometry->VERTEX(p1),
											m_pGeometry->VERTEX(p2),
											m_pGeometry->VERTEX(p3),
											normal);

				//!	Smooth shading normal. Is it necessary to customize it ?
                ADD_NORMAL(p1,p2,p3,normal);
			}
		}

        //!    Second step, proceed with primitives.
		const vector<CGeometryPrimitive*>	primitives = m_pGeometry->getPrimitives();
        vector<CGeometryPrimitive*>::const_iterator	it = primitives.begin();
        while (it != primitives.end())
        {
            CGeometryPrimitive *p = *it++;
            int size = p->getSize();
            unsigned short *faces = new unsigned short[size];
            p->getIndexes(faces);

            //!    Future versions will complete normal computations
            if (p->getKind() == CGeometryPrimitive::TRIANGLE)
            {
                for (i=0 ; i < size ; i+=3)
		        {
                    p1 = faces[i];
			        p2 = faces[i+1];
			        p3 = faces[i+2];
			        if ((p1!=p2)&&(p1!=p3)&&(p2!=p3))
			        {
				        C3DEngine::TriangleNormal(	m_pGeometry->VERTEX(p1),
													m_pGeometry->VERTEX(p2),
													m_pGeometry->VERTEX(p3),
													normal);

				        //!	Smooth shading normal. Is it necessary to customize it ?
                        ADD_NORMAL(p1,p2,p3,normal);
			        }
                }
            }
			//!    Future versions will complete normal computations
            else if (p->getKind() == CGeometryPrimitive::TRIANGLE_FAN)
            {
				p1 = faces[0];
                for (i=0 ; i < size ; i+=2)
		        {
			        p2 = faces[i];
			        p3 = faces[i+1];
			        if ((p1!=p2)&&(p1!=p3)&&(p2!=p3))
			        {
				        C3DEngine::TriangleNormal(	m_pGeometry->VERTEX(p1),
													m_pGeometry->VERTEX(p2),
													m_pGeometry->VERTEX(p3),
													normal);

				        //!	Smooth shading normal. Is it necessary to customize it ?
                        ADD_NORMAL(p1,p2,p3,normal);
			        }
                }
            }
            else if (p->getKind() == CGeometryPrimitive::QUAD)
            {
                for (i=0 ; i < size ; i+=4)
		        {
                    p1 = faces[i];
			        p2 = faces[i+1];
			        p3 = faces[i+2];
			        if ((p1!=p2)&&(p1!=p3)&&(p2!=p3))
			        {
				        C3DEngine::TriangleNormal(	m_pGeometry->VERTEX(p1),
													m_pGeometry->VERTEX(p2),
													m_pGeometry->VERTEX(p3),
													normal);

				        //!	Smooth shading normal. Is it necessary to customize it ?
                        ADD_NORMAL(p1,p2,p3,normal);
                        m_pGeometry->NORMAL(faces[i+3]).x += normal.x;
                        m_pGeometry->NORMAL(faces[i+3]).y += normal.y;
                        m_pGeometry->NORMAL(faces[i+3]).z += normal.z;
                        m_pGeometry->NORMAL(faces[i+3]).h += 1.0f;
			        }
                }
            }
            else if (p->getKind() == CGeometryPrimitive::TRIANGLE_STRIP)
            {
                bool reverse = true;
                for (i=0 ; i < size - 2 ; i++)
		        {
                    if (reverse = !reverse)
                    {
                        p1 = faces[i+1];
			            p2 = faces[i];
                    }
                    else
                    {
                        p1 = faces[i];
			            p2 = faces[i+1];
                    }
			        p3 = faces[i+2];
			        if ((p1!=p2)&&(p1!=p3)&&(p2!=p3))
			        {
				        C3DEngine::TriangleNormal(	m_pGeometry->VERTEX(p1),
													m_pGeometry->VERTEX(p2),
													m_pGeometry->VERTEX(p3),
													normal);

				        //!	Smooth shading normal. Is it necessary to customize it ?
                         ADD_NORMAL(p1,p2,p3,normal);
			        }
                }
            }

            delete [] faces;
        }
	}

    //!    Final step, homogeneous coordinate
	iMax = m_pGeometry->nbVertex();
#pragma omp parallel for default(none) private (i) shared(iMax)
	for (i=0;i<iMax;i++)
	{
		CGenericVector<float> v(m_pGeometry->NORMAL(i));
		if (v.H() != 0.0f)
			v *= (1.0f/v.H());
		else
		{
			v.X() = v.Y() = 0.0f;
			v.Z() = 1.0f;
		}
		//v.H(1.0);

		//	normalizing normal vectors
		v.Normalize();
		m_pGeometry->NORMAL(i).x = v.X();
        m_pGeometry->NORMAL(i).y = v.Y();
        m_pGeometry->NORMAL(i).z = v.Z();
        m_pGeometry->NORMAL(i).h = 1.0f; //v.H();
	}

    if (m_pGeometry->m_bUpdateBBox)
		m_pGeometry->updateBBox();

	if ((CGeometryAllocator::GetInstance()->isMemoryRelocated()) &&
		(!m_pGeometry->m_bDataLocked))
	{
		m_pGeometry->normals = (GL_COORD_VERTEX*)(CGeometryAllocator::GetInstance()->glvkUnMapPointer((float*)m_pGeometry->normals));
		m_pGeometry->vertex = (GL_COORD_VERTEX*)(CGeometryAllocator::GetInstance()->glDiscardPointer((float*)m_pGeometry->vertex));
		m_pGeometry->polys = (unsigned short*)(CGeometryAllocator::GetInstance()->glDiscardPointer(m_pGeometry->polys));
	}
}

void CGeometryEditor::genBinormals(void) const
{
#if defined (DATA_EXTENDED)
    if (m_pGeometry->geometry != NULL)
    {
        for (unsigned int i=0;i<m_pGeometry->nbVertex();i++)
		{
			GL_COORD_VERTEX &v = m_pGeometry->NORMAL(i);
			Raptor::gl3DEngine()->Normals(v,m_pGeometry->TANGENT(i),m_pGeometry->BINORMAL(i));
		}
	}
#elif defined (DATA_PACKED)
	if (m_pGeometry->normals != NULL)
	{
        if (m_pGeometry->tangents != NULL) CGeometryAllocator::GetInstance()->releaseVertices((float*)m_pGeometry->tangents);
        if (m_pGeometry->binormals != NULL) CGeometryAllocator::GetInstance()->releaseVertices((float*)m_pGeometry->binormals);
		m_pGeometry->tangents = (GL_COORD_VERTEX*)(CGeometryAllocator::GetInstance()->allocateVertices(m_pGeometry->nbVertex()*4));
        m_pGeometry->binormals = (GL_COORD_VERTEX*)(CGeometryAllocator::GetInstance()->allocateVertices(m_pGeometry->nbVertex()*4));

        if ((CGeometryAllocator::GetInstance()->isMemoryRelocated()) &&
			(!m_pGeometry->m_bDataLocked))
        {
            m_pGeometry->normals = (GL_COORD_VERTEX*)(CGeometryAllocator::GetInstance()->glvkMapPointer((float*)m_pGeometry->normals));
            m_pGeometry->tangents = (GL_COORD_VERTEX*)(CGeometryAllocator::GetInstance()->glvkMapPointer((float*)m_pGeometry->tangents));
            m_pGeometry->binormals = (GL_COORD_VERTEX*)(CGeometryAllocator::GetInstance()->glvkMapPointer((float*)m_pGeometry->binormals));
        }

		C3DEngine *pEngine = C3DEngine::Get3DEngine();
		int i = 0;
		int iMax = m_pGeometry->nbVertex();
#pragma omp parallel for default(none) private (i) shared(pEngine,iMax)
		for (i=0;i<iMax;i++)
		{
			GL_COORD_VERTEX &v = m_pGeometry->normals[i];
			pEngine->normals(v,m_pGeometry->tangents[i],m_pGeometry->binormals[i]);
		}

        if ((CGeometryAllocator::GetInstance()->isMemoryRelocated()) &&
			(!m_pGeometry->m_bDataLocked))
        {
            m_pGeometry->normals = (GL_COORD_VERTEX*)(CGeometryAllocator::GetInstance()->glDiscardPointer((float*)m_pGeometry->normals));
            m_pGeometry->tangents = (GL_COORD_VERTEX*)(CGeometryAllocator::GetInstance()->glvkUnMapPointer((float*)m_pGeometry->tangents));
            m_pGeometry->binormals = (GL_COORD_VERTEX*)(CGeometryAllocator::GetInstance()->glvkUnMapPointer((float*)m_pGeometry->binormals));
        }
	}
#endif
}


void CGeometryEditor::genTexCoords(	TEXTURE_MODEL model, 
									float xFactor, float xOffset,
									float yFactor, float yOffset, 
									float zFactor, float zOffset) const
{
	if (model >= NB_TEXTURE_MODELS)
		return;

#if defined (DATA_EXTENDED)
    if (m_pGeometry->geometry != NULL)
#elif defined (DATA_PACKED)
	if (m_pGeometry->texcoords != NULL)
#endif
	{
		const CBoundingBox* const bbox = m_pGeometry->boundingBox();
        if ((CGeometryAllocator::GetInstance()->isMemoryRelocated()) &&
			(!m_pGeometry->m_bDataLocked))
		{
            m_pGeometry->texcoords = (GL_TEX_VERTEX*)(CGeometryAllocator::GetInstance()->glvkMapPointer((float*)m_pGeometry->texcoords));
			m_pGeometry->vertex = (GL_COORD_VERTEX*)(CGeometryAllocator::GetInstance()->glvkMapPointer((float*)m_pGeometry->vertex));
		}

		float width = bbox->xMax() - bbox->xMin();
		if (width != 0.0f)
			width = 1.0f / width;
		else
			width = 0.0f;
		float height = bbox->yMax() - bbox->yMin();
		if (height != 0.0f)
			height = 1.0f / height;
		else
			height = 0.0f;
		float depth = bbox->zMax() - bbox->zMin();
		if (depth != 0.0f)
			depth = 1.0f / depth;
		else
			depth = 0.0f;

		for (unsigned int i=0;i<m_pGeometry->nbVertex();i++)
		{
            GL_TEX_VERTEX &t = m_pGeometry->TEXCOORD0(i);

			switch (model)
			{
				case XY_PLANE:
				{
					const GL_COORD_VERTEX &v = m_pGeometry->VERTEX(i);
					t.u = xFactor * v.x + xOffset;
					t.v = yFactor * v.y + yOffset;
					break;
				}
				case XY_BOX:
				{
					const GL_COORD_VERTEX &v = m_pGeometry->VERTEX(i);
					t.u = ((xFactor * v.x + xOffset) - bbox->xMin()) * width;
					t.v = ((yFactor * v.y + yOffset) - bbox->yMin()) * height;
					break;
				}
				case XZ_PLANE:
				{
					const GL_COORD_VERTEX &v = m_pGeometry->VERTEX(i);
					t.u = ((xFactor * v.x + xOffset) - bbox->xMin()) * width;
					t.v = ((zFactor * v.z + zOffset) - bbox->zMin()) * depth;
					break;
				}
				case YZ_PLANE:
				{
					const GL_COORD_VERTEX &v = m_pGeometry->VERTEX(i);
					t.u = ((yFactor * v.y + yOffset) - bbox->yMin()) * height;
					t.v = ((zFactor * v.z + zOffset) - bbox->zMin()) * depth;
					break;
				}
				default:
					break;
			}
        }

        if ((CGeometryAllocator::GetInstance()->isMemoryRelocated()) &&
			(!m_pGeometry->m_bDataLocked))
		{
            m_pGeometry->texcoords = (GL_TEX_VERTEX*)(CGeometryAllocator::GetInstance()->glvkUnMapPointer((float*)m_pGeometry->texcoords));
			m_pGeometry->vertex = (GL_COORD_VERTEX*)(CGeometryAllocator::GetInstance()->glDiscardPointer((float*)m_pGeometry->vertex));
		}
    }
}


void CGeometryEditor::scaleTexCoords(float sx, float sy) const
{
#if defined (DATA_EXTENDED)
    if (m_pGeometry->geometry != NULL)
#elif defined (DATA_PACKED)
	if (m_pGeometry->texcoords != NULL)
#endif
	{
        if ((CGeometryAllocator::GetInstance()->isMemoryRelocated()) &&
			(!m_pGeometry->m_bDataLocked))
            m_pGeometry->texcoords = (GL_TEX_VERTEX*)(CGeometryAllocator::GetInstance()->glvkMapPointer((float*)m_pGeometry->texcoords));

		for (unsigned int i=0;i<m_pGeometry->nbVertex();i++)
		{
            GL_TEX_VERTEX &t = m_pGeometry->TEXCOORD0(i);

            t.u *= sx;
            t.v *= sy;
        }

        if ((CGeometryAllocator::GetInstance()->isMemoryRelocated()) &&
			(!m_pGeometry->m_bDataLocked))
            m_pGeometry->texcoords = (GL_TEX_VERTEX*)(CGeometryAllocator::GetInstance()->glvkUnMapPointer((float*)m_pGeometry->texcoords));
    }
}


void CGeometryEditor::flipNormals(void) const 
{
	//	change normal signs to give the opposite direction
#if defined (DATA_EXTENDED)
    if (m_pGeometry->geometry != NULL)
#elif defined (DATA_PACKED)
	if (m_pGeometry->normals != NULL)
#endif
	{
		for (unsigned int i=0;i<m_pGeometry->nbVertex();i++)
		{
			GL_COORD_VERTEX &v = m_pGeometry->NORMAL(i);

			v.x = -v.x;
			v.y = -v.y;
			v.z = -v.z;
			v.h = 1.0f;
		}
	}

	//	polygons index must be reversed too, because if there
	//	is lighting, the normals won't be correct
	if (m_pGeometry->polys)
	{
		int pos = 0;
		for (unsigned int i=0;i<m_pGeometry->m_nbPolys;i++)
		{
			unsigned int p = m_pGeometry->polys[pos+1];
			m_pGeometry->polys[pos+1] = m_pGeometry->polys[pos+2];
			m_pGeometry->polys[pos+2] = p;

			pos +=3;
		}
	}

    // TODO : handle CGeometryPrimitive case
}


void CGeometryEditor::shareVertex(unsigned int own_vrtx,unsigned int other_vrtx,CGeometry &geo) const
{
	if ((own_vrtx < m_pGeometry->nbVertex()) && (other_vrtx < geo.nbVertex()))
#if defined (DATA_EXTENDED)
        if ((m_pGeometry->geometry != NULL) && (geo.geometry != NULL))
#elif defined (DATA_PACKED)
		if ((m_pGeometry->normals != NULL) && (geo.normals != NULL))
#endif
		{
			const GL_COORD_VERTEX& n = m_pGeometry->NORMAL(own_vrtx);
			CGenericVector<float> normal(n.x,n.y,n.z,n.h);

			const GL_COORD_VERTEX& n2 = geo.NORMAL(other_vrtx);
			CGenericVector<float> normal2(n2.x,n2.y,n2.z,n2.h);

			normal += normal2;
			normal.Normalize();

			m_pGeometry->NORMAL(own_vrtx).x = normal.X();
            m_pGeometry->NORMAL(own_vrtx).y = normal.Y();
            m_pGeometry->NORMAL(own_vrtx).z = normal.Z();
            m_pGeometry->NORMAL(own_vrtx).h = 1.0f;
			geo.NORMAL(other_vrtx).x = normal.X();
            geo.NORMAL(other_vrtx).y = normal.Y();
            geo.NORMAL(other_vrtx).z = normal.Z();
            geo.NORMAL(other_vrtx).h = 1.0f;
		}
}


void CGeometryEditor::minimize(void) const
{
	int i=0;
	int maxVertex = m_pGeometry->nbVertex();

    if (maxVertex < 1)
        return;

	int *replacements = new int[maxVertex];
#pragma omp parallel for default(none) private (i) shared(replacements,maxVertex)
	for (i=0;i<maxVertex;i++)
		replacements[i] = -1;


#if defined (DATA_PACKED)
    if (CGeometryAllocator::GetInstance()->isMemoryRelocated())
        m_pGeometry->vertex = (GL_COORD_VERTEX*)(CGeometryAllocator::GetInstance()->glvkMapPointer((float*)m_pGeometry->vertex));
#endif

	map<float,unsigned int> mapx;
	map<float,unsigned int> mapy;
	map<float,unsigned int> mapz;
	for (i=0;i<maxVertex;i++)
	{
		GL_COORD_VERTEX &v = m_pGeometry->VERTEX(i);

		map<float,unsigned int>::const_iterator itx = mapx.find(v.x);
		if (itx != mapx.end())
		{
			GL_COORD_VERTEX &v2 = m_pGeometry->VERTEX((*itx).second);
			if (v2 == v)
			{
                unsigned int newidx = (*itx).second;
				while (replacements[newidx] >= 0)
					newidx = replacements[newidx];

                replacements[i] = newidx;
			}
			else
			{
				// Vertices are different here! : same x but v2 != v !
				map<float,unsigned int>::const_iterator ity = mapy.find(v.y);
				if (ity != mapy.end())
				{
					GL_COORD_VERTEX &v3 = m_pGeometry->VERTEX((*ity).second);
					if (v3 == v)
					{
						unsigned int newidy = (*ity).second;
						while (replacements[newidy] >= 0)
							newidy = replacements[newidy];

						replacements[i] = newidy;
					}
					else
					{
						//! Same x, same y, only z is different.
						map<float,unsigned int>::const_iterator itz = mapz.find(v.z);
						if (itz != mapz.end())
						{
							GL_COORD_VERTEX &v4 = m_pGeometry->VERTEX((*itz).second);
							if (v4 == v)
							{
								unsigned int newidz = (*itz).second;
								while (replacements[newidz] >= 0)
									newidz = replacements[newidz];

								replacements[i] = newidz;
							}
							else for (int j=0;j<i;j++)
							{
								GL_COORD_VERTEX &v5 = m_pGeometry->VERTEX(j);

								if (v5 == v)
								{
									unsigned int newid = j;
									while (replacements[newid] >= 0)
										newid = replacements[newid];

									replacements[i] = newid;
									break;
								}
							}
						}
						else
							mapz[v.z] = i;
					}
				}
				else
					mapy[v.y] = i;
			}
		}
		else
			mapx[v.x] = i;
	}

	/*
	//	suppress indexes in polygons
	for (i=0;i<maxVertex-1;i++)
	{
		GL_COORD_VERTEX &v = m_pGeometry->VERTEX(i);

		for (int j=i+1;j<maxVertex;j++)
		{
			GL_COORD_VERTEX &v2 = m_pGeometry->VERTEX(j);

			if (v2 == v)
			{
                unsigned int newidx = i;
				while (replacements[newidx] >= 0)
					newidx = replacements[newidx];

                replacements[j] = newidx;
				unique--;
			}
		}
	}
	*/

#if defined (DATA_PACKED)
    if (CGeometryAllocator::GetInstance()->isMemoryRelocated())
        m_pGeometry->polys = (unsigned short*)(CGeometryAllocator::GetInstance()->glvkMapPointer(m_pGeometry->polys));
#endif

	// TODO : handle the case of CGeometryPrimitives
	int iMax = m_pGeometry->m_nbPolys*3;
#pragma omp parallel for default(none) private (i) shared(replacements,iMax)
    for (i=0;i<iMax;i++)
    {
        unsigned int idx = m_pGeometry->polys[i];

		int newidx = replacements[idx];
		if (newidx >= 0)
            m_pGeometry->polys[i] = newidx;
    }

	//	suppress unused vertex
	int pos = 0;
	while (maxVertex > pos)
	{
		// select a replaced vertex
		if (replacements[pos] >= 0)
		{
			// search a non replaced vertex from the end
			maxVertex--;
			while ((maxVertex > pos) && (replacements[maxVertex] >= 0))
				maxVertex--;

			// if valid, record replacements
			if ((maxVertex > pos) && (replacements[maxVertex] < 0))
			{
				GL_COORD_VERTEX &v = m_pGeometry->VERTEX(pos);
				GL_COORD_VERTEX &v2 = m_pGeometry->VERTEX(maxVertex);
				v = v2;
				replacements[maxVertex] = pos;
			}
			replacements[pos] = -1;
		}

		pos++;
	}

#pragma omp parallel for default(none) private (i) shared(replacements,iMax)
    for (i=0;i<iMax;i++)
    {
        unsigned int idx = m_pGeometry->polys[i];

		int newidx = replacements[idx];
		if (newidx >= 0)
            m_pGeometry->polys[i] = newidx;
    }

	delete [] replacements;



	// Reallocate data
#if defined (DATA_PACKED)
    if (CGeometryAllocator::GetInstance()->isMemoryRelocated())
        m_pGeometry->polys = (unsigned short*)(CGeometryAllocator::GetInstance()->glvkUnMapPointer(m_pGeometry->polys));
#endif

#if defined (DATA_EXTENDED)
    GL_VERTEX_DATA *vrtx = m_pGeometry->geometry;
    m_pGeometry->geometry = new GL_VERTEX_DATA[maxVertex];
    for (i=0;i<maxVertex;i++)
		m_pGeometry->geometry[i] = vrtx[i];
    delete [] vrtx;
#elif defined (DATA_PACKED)
	GL_COORD_VERTEX *vrtx = m_pGeometry->vertex;

	m_pGeometry->vertex = (GL_COORD_VERTEX*)(CGeometryAllocator::GetInstance()->allocateVertices(maxVertex*4));
    if (CGeometryAllocator::GetInstance()->isMemoryRelocated())
        m_pGeometry->vertex = (GL_COORD_VERTEX*)(CGeometryAllocator::GetInstance()->glvkMapPointer((float*)m_pGeometry->vertex));
	
	for (i=0;i<maxVertex;i++)
		m_pGeometry->vertex[i] = vrtx[i];

    if (CGeometryAllocator::GetInstance()->isMemoryRelocated())
    {
        m_pGeometry->vertex = (GL_COORD_VERTEX*)(CGeometryAllocator::GetInstance()->glvkUnMapPointer((float*)m_pGeometry->vertex));
        vrtx = (GL_COORD_VERTEX*)(CGeometryAllocator::GetInstance()->glDiscardPointer((float*)vrtx));
    }
	CGeometryAllocator::GetInstance()->releaseVertices((float*)vrtx);
#endif

	m_pGeometry->m_nbVertex = maxVertex;
	m_bMinimized = true;
}



void CGeometryEditor::strip(void) const
{
	vector<unsigned int> Polys;
	vector<vector<unsigned int> >	strips;

#if defined (DATA_PACKED)
    if ((CGeometryAllocator::GetInstance()->isMemoryRelocated()) &&
		(!m_pGeometry->m_bDataLocked))
        m_pGeometry->polys = (unsigned short*)(CGeometryAllocator::GetInstance()->glvkMapPointer(m_pGeometry->polys));
#endif
	Polys.resize(3*m_pGeometry->m_nbPolys);
	for (unsigned int i=0;i<3*m_pGeometry->m_nbPolys;i++)
		Polys[i] = m_pGeometry->polys[i];
#if defined (DATA_PACKED)
    if ((CGeometryAllocator::GetInstance()->isMemoryRelocated()) &&
		(!m_pGeometry->m_bDataLocked))
        m_pGeometry->polys = (unsigned short*)(CGeometryAllocator::GetInstance()->glDiscardPointer(m_pGeometry->polys));
#endif

	//	search for next strip elt
	while (Polys.size() > 0)
	{
		vector<unsigned int> currentStrip;

		currentStrip.push_back(Polys[0]);
		currentStrip.push_back(Polys[1]);
		currentStrip.push_back(Polys[2]);

		unsigned int idx1 = Polys[2];
		unsigned int idx2 = Polys[1];

		Polys.erase(Polys.begin());
		Polys.erase(Polys.begin());
		Polys.erase(Polys.begin());

		unsigned int index = 0;
		vector<unsigned int>::iterator index_itr = Polys.begin();
		bool found = false;
		bool strip_done = false;
		bool odd = true;

		while (!strip_done)
		{
			while ((!found) && (index < Polys.size()))
			{
				if (Polys[index] == idx1)
				{
					if (Polys[index+1] == idx2)
					{
						if (odd)
						{
							idx2 = Polys[index+2];
							currentStrip.push_back(idx2);
						}
						else
						{
							idx1 = Polys[index+2];
							currentStrip.push_back(idx1);
						}

						found = true;
					}
				}
				else if (Polys[index+1] == idx1)
				{
					if (Polys[index+2] == idx2)
					{
						if (odd)
						{
							idx2 = Polys[index];
							currentStrip.push_back(idx2);
						}
						else
						{
							idx1 = Polys[index];
							currentStrip.push_back(idx1);
						}

						found = true;
					}
				}
				else if (Polys[index+2] == idx1)
				{
					if (Polys[index] == idx2)
					{
						if (odd)
						{
							idx2 = Polys[index+1];
							currentStrip.push_back(idx2);
						}
						else
						{
							idx1 = Polys[index+1];
							currentStrip.push_back(idx1);
						}

						found = true;
					}
				}
			
				if (found)
				{
					index_itr = Polys.erase(index_itr);
					index_itr = Polys.erase(index_itr);
					index_itr = Polys.erase(index_itr);

					index = 0;
					index_itr = Polys.begin();
					odd = !odd;
				}
				else
				{
					index += 3;
					index_itr+=3;
				}
			}

			if (!found)
				strip_done = (index >= Polys.size());

			found = false;
		}

		unsigned int size = currentStrip.size();
		strips.push_back(currentStrip);
	}

	//	Optimise by merging strips


	//	Order the strip
	vector<vector<unsigned int> >	ordered_strips;

	int strip_pos = 0;
	while (strips.size() > 0)
	{
		unsigned int max = 0;
		vector<vector<unsigned int> >::iterator max_index = strips.begin();
		vector<vector<unsigned int> >::iterator itr = strips.begin();

		while (itr != strips.end())
		{
			if ((*itr).size() > max)
			{
				max = (*itr).size();
				max_index = itr;
			}
			itr++;
		}

		if (max > 3)
			strip_pos ++;

		ordered_strips.push_back(*max_index);
		strips.erase(max_index);
	}

	//	Create the final array of strips
	strips.clear();
	Polys.clear();

	strip_pos=0;

    unsigned int nb_indexes = 0;
	for (unsigned int j=0;j<ordered_strips.size();j++)
	{
        vector<unsigned int>& strip = ordered_strips[j];
		int nb = strip.size();

		if (nb > 3)
		{
            CGeometryPrimitive *p = m_pGeometry->createPrimitive(CGeometryPrimitive::TRIANGLE_STRIP);
            unsigned short *indexes = new unsigned short[nb];

			for (int i=0;i<nb;i++)
                indexes[i] = strip[i];

            p->setIndexes(nb,indexes);

            delete [] indexes;
		}
		else
            nb_indexes += nb;
	}


    if (nb_indexes > 0)
	{
		int offset = 0;

        CGeometryPrimitive *p = m_pGeometry->createPrimitive(CGeometryPrimitive::TRIANGLE);
        unsigned short *indexes = new unsigned short[nb_indexes];

		while (ordered_strips.size() > 0)
		{
            vector<unsigned int>& strip = ordered_strips[0];
			if (strip.size() <= 3)
			{
				for (unsigned int j=0;j<strip.size();j++)
					indexes[offset++] = strip[j];
			}
			ordered_strips.erase(ordered_strips.begin());
		}
	
        p->setIndexes(offset,indexes);
        delete [] indexes;
	}

	ordered_strips.clear();
}


void CGeometryEditor::moveVertex(	unsigned int numVertex,
									GL_COORD_VERTEX moveTo,
									unsigned int tension)
{
	if (numVertex >= m_pGeometry->nbVertex())
		return;

#if defined (DATA_PACKED)
    if ((CGeometryAllocator::GetInstance()->isMemoryRelocated()) &&
		(!m_pGeometry->m_bDataLocked))
	{
        m_pGeometry->vertex = (GL_COORD_VERTEX*)(CGeometryAllocator::GetInstance()->glvkMapPointer((float*)m_pGeometry->vertex));
		m_pGeometry->polys = (unsigned short*)(CGeometryAllocator::GetInstance()->glvkMapPointer(m_pGeometry->polys));
	}
#endif
	
	m_pGeometry->setCoord(numVertex,moveTo.x,moveTo.y,moveTo.z,1.0f);

	float tMax = tension;

	while (tension > 0)
	{
		tMax *= 0.5f;

		//	In this array, we will store all the vertex indices that we will move.
		map<unsigned int,unsigned int> previous;
		map<unsigned int,unsigned int> vertices;
		previous[numVertex] = numVertex;

		bool swap = false;
		map<unsigned int,unsigned int>* pprevious = &previous;
		map<unsigned int,unsigned int>* pvertices = &vertices;

		unsigned int p1,p2,p3;
		unsigned int prev;

		//	Search all surrounding vertices according to tension
		for (unsigned int i=0;i<m_pGeometry->nbFace();i++)
		{
			m_pGeometry->getFace(i,p1,p2,p3);

			if (pprevious->find(p1) != pprevious->end())
			{
				pvertices->operator[](p2) = p2;
				pvertices->operator[](p3) = p3;
			}

			if (pprevious->find(p2) != pprevious->end())
			{
				pvertices->operator[](p1) = p1;
				pvertices->operator[](p3) = p3;
			}
			
			if (pprevious->find(p3) != pprevious->end())
			{
				pvertices->operator[](p1) = p1;
				pvertices->operator[](p2) = p2;
			}
		}

		//	extend geometry with a very easy algorithm
		map<unsigned int,unsigned int>::const_iterator pos = pvertices->begin();
		GL_VERTEX_DATA v;
		while (pos != pvertices->end())
		{
			prev = (*pos++).second;

			m_pGeometry->getVertex(prev,v);
						
			v.vertex.x += tMax * (moveTo.x - v.vertex.x);
			v.vertex.y += tMax * (moveTo.y - v.vertex.y);
			v.vertex.z += tMax * (moveTo.z - v.vertex.z);

			m_pGeometry->setCoord(prev,v.vertex.x,v.vertex.y,v.vertex.z,1.0f);
		}

		//	swap arrays to go to next iteration.
		swap = !swap;
		if (pprevious == &previous)
		{
			pprevious = &vertices;
			pvertices = &previous;
		}
		else
		{
			pprevious = &previous;
			pvertices = &vertices;
		}

		// go to next iteration
		tension --;
	}

#if defined (DATA_PACKED)
    if ((CGeometryAllocator::GetInstance()->isMemoryRelocated()) &&
		(!m_pGeometry->m_bDataLocked))
	{
        m_pGeometry->vertex = (GL_COORD_VERTEX*)(CGeometryAllocator::GetInstance()->glvkUnMapPointer((float*)m_pGeometry->vertex));
		m_pGeometry->polys = (unsigned short*)(CGeometryAllocator::GetInstance()->glvkUnMapPointer(m_pGeometry->polys));
	}
#endif

}
