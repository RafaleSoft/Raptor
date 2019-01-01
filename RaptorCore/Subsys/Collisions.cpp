#include "Subsys/CodeGeneration.h"


#if !defined(AFX_3DENGINE_H__DB24F018_80B9_11D3_97C1_FC2841000000__INCLUDED_)
	#include "../Engine/3DEngine.h"
#endif

#if !defined(AFX_PHYSICS_H__B42ABB89_80E8_11D3_97C2_DE5C28000000__INCLUDED_)
	#include "../Engine/Physics.h"
#endif

#if !defined(AFX_OBJECT3DINSTANCE_H__A2627662_F5F9_11D3_9142_CFEB8E9F2745__INCLUDED_)
	#include "../GLHierarchy/Object3DInstance.h"
#endif

#if !defined(__COLLISIONS_H__)
	#include "Collisions.h"
#endif

RAPTOR_NAMESPACE


CCubeBTree::~CCubeBTree()
{
	if (m_up) 
	{
		delete m_up;
		m_up = NULL;
	}
	if (m_down)
	{
		delete m_down;
		m_down = NULL;
	}
	if (m_left) 
	{
		delete m_left;
		m_left = NULL;
	}
	if (m_right) 
	{
		delete m_right;
		m_right = NULL;
	}
	if (m_front) 
	{
		delete m_front;
		m_front = NULL;
	}
	if (m_back) 
	{
		delete m_back;
		m_back = NULL;
	}
}


GL_MATRIX &CCubeBTree::SetTransform(GL_MATRIX &M)
{
	//	transpose rotation
	transform[1] = M[4];
	transform[4] = M[1];

	transform[2] = M[8];
	transform[8] = M[2];

	transform[6] = M[9];
	transform[9] = M[6];

	transform[0] = M[0];
	transform[5] = M[5];
	transform[10] = M[10];

	//	transform translation
	transform[3] = -(M[0]*M[3] + M[4]*M[7] + M[8]*M[11]);
	transform[7] = -(M[1]*M[3] + M[5]*M[7] + M[9]*M[11]);
	transform[11] = -(M[2]*M[3] + M[6]*M[7] + M[10]*M[11]);

	transform[12] = transform[13] = transform[14] = 0.0f;
	transform[15] = 1.0f;

	return transform;
}


//	T = M1 * M2
void MultMatrix(GL_MATRIX &M1,GL_MATRIX &M2,GL_MATRIX &T)
{

	for (int j=0; j<16 ; j+=4)
	{
		float x0 = M1[j];
		float y0 = M1[j+1];
		float z0 = M1[j+2];
		float h0 = M1[j+3];

		for (int i=0;i<4;i++)
		{
			T[i+j] = x0 * M2[i] + y0 * M2[i+4] + z0 * M2[i+8] + h0 * M2[i+12];
		}
	}
}



CCubeBTree *CCubeBTree::Insert(CPhysics *obj,GL_MATRIX &M,CCollisionStatus &clip)
{
	if (obj == m_node)
		return this;

	// Object is inserted at the first free location
	// where clip.* == 8, i.e. opposite clip.~* == 0
	CCollisionStatus	st;
	GL_MATRIX			transform;

	if (clip.m_leftX>0)
	{
		if (m_left == NULL) 
		{
			if (clip.m_leftX == 8)
			{
				m_left = new CCubeBTree(obj,this->m_engine);
				m_left->SetTransform(M);
				return(m_left);
			}
		}
		else
		{
			//transform = m_left->transform * M;
			MultMatrix(m_left->transform,M,transform);
			C3DEngine::IntersectBBox(	m_left->m_node->getObject()->getObject()->boundingBox(),
										obj->getObject()->getObject()->boundingBox(),
										transform,
										st);
			if (st.m_intersect > 0) 
			{ 
				m_engine->collideObjects(obj,m_left->m_node); 
				m_engine->onObjectCollision(obj,m_left->m_node); 
			} 
			return (m_left->Insert(obj,M,st));
		}
	}

	if (clip.m_rightX>0)
	{
		if (m_right == NULL) 
		{
			if (clip.m_rightX == 8)
			{
				m_right = new CCubeBTree(obj,this->m_engine);
				m_right->SetTransform(M);
				return(m_right);
			}
		}
		else
		{
			//transform = m_right->transform * M;
			MultMatrix(m_right->transform,M,transform);
			C3DEngine::IntersectBBox(	m_right->m_node->getObject()->getObject()->boundingBox(),
										obj->getObject()->getObject()->boundingBox(),
										transform,
										st);
			if (st.m_intersect > 0) 
			{ 
				m_engine->collideObjects(obj,m_right->m_node); 
				m_engine->onObjectCollision(obj,m_right->m_node); 
			} 
			return (m_right->Insert(obj,M,st));
		}
	}

	if (clip.m_upY>0)
	{
		if (m_up == NULL) 
		{
			if (clip.m_upY == 8)
			{
				m_up = new CCubeBTree(obj,this->m_engine);
				m_up->SetTransform(M);
				return(m_up);
			}
		}
		else
		{
			//transform = m_up->transform * M;
			MultMatrix(m_up->transform,M,transform);
			C3DEngine::IntersectBBox(m_up->m_node->getObject()->getObject()->boundingBox(),
										obj->getObject()->getObject()->boundingBox(),
										transform,
										st);
			if (st.m_intersect > 0) 
			{ 
				m_engine->collideObjects(obj,m_up->m_node); 
				m_engine->onObjectCollision(obj,m_up->m_node); 
			} 
			return (m_up->Insert(obj,M,st));
		}
	}

	if (clip.m_downY>0)
	{
		if (m_down == NULL) 
		{
			if (clip.m_downY == 8)
			{
				m_down = new CCubeBTree(obj,this->m_engine);
				m_down->SetTransform(M);
				return(m_down);
			}
		}
		else
		{
			//transform = m_down->transform * M;
			MultMatrix(m_down->transform,M,transform);
			C3DEngine::IntersectBBox(	m_down->m_node->getObject()->getObject()->boundingBox(),
										obj->getObject()->getObject()->boundingBox(),
										transform,
										st);
			if (st.m_intersect > 0)
			{ 
				m_engine->collideObjects(obj,m_down->m_node);
				m_engine->onObjectCollision(obj,m_down->m_node);
			}
			return (m_down->Insert(obj,M,st));
		}
	}

	if (clip.m_frontZ>0)
	{
		if (m_front == NULL)
		{
			if (clip.m_frontZ == 8)
			{
				m_front = new CCubeBTree(obj,this->m_engine);
				m_front->SetTransform(M);
				return(m_front);
			}
		}
		else
		{
			//transform = m_front->transform * M;
			MultMatrix(m_front->transform,M,transform);
			C3DEngine::IntersectBBox(	m_front->m_node->getObject()->getObject()->boundingBox(),
										obj->getObject()->getObject()->boundingBox(),
										transform,
										st);
			if (st.m_intersect > 0)
			{
				m_engine->collideObjects(obj,m_front->m_node);
				m_engine->onObjectCollision(obj,m_front->m_node);
			}
			return (m_front->Insert(obj,M,st));
		}
	}
	
	if (clip.m_backZ>0)
	{
		if (m_back == NULL) 
		{
			if (clip.m_backZ == 8)
			{
				m_back = new CCubeBTree(obj,this->m_engine);
				m_back->SetTransform(M);
				return(m_back);
			}
		}
		else
		{
			//transform = m_back->transform * M;
			MultMatrix(m_back->transform,M,transform);
			C3DEngine::IntersectBBox(	m_back->m_node->getObject()->getObject()->boundingBox(),
										obj->getObject()->getObject()->boundingBox(),
										transform,
										st);
			if (st.m_intersect > 0) 
			{ 
				m_engine->collideObjects(obj,m_back->m_node); 
				m_engine->onObjectCollision(obj,m_back->m_node); 
			} 
			return (m_back->Insert(obj,M,st));
		}
	}
	
	return NULL;
}



