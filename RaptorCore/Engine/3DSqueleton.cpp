// 3DSqueleton.cpp: implementation of the C3DSqueleton class.
//
//////////////////////////////////////////////////////////////////////
#include "Subsys/CodeGeneration.h"

#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
	#include "System/Raptor.h"
#endif

#if !defined(AFX_3DSQUELETON_H__3F4B9881_392F_4267_841C_3BA7929FCAC6__INCLUDED_)
	#include "3DSqueleton.h"
#endif

#ifndef __GLOBAL_H__
	#include "System/Global.h"
#endif
#if !defined(AFX_MEMORY_H__81A6CA9A_4ED9_4260_B6E4_C03276C38DBC__INCLUDED_)
	#include "System/Memory.h"
#endif
#if !defined(AFX_OBJECT3D_H__DB24F017_80B9_11D3_97C1_FC2841000000__INCLUDED_)
	#include "GLHierarchy/Object3D.h"
#endif


RAPTOR_NAMESPACE_BEGIN

typedef struct object_link
{
		bool		is_pivot;	//	true => pivot ; false => guide
		CVector4f	axis;
		CVector4f	position;
		float		value;
		bone		*p_parent;
		bone		*p_child;
		CObject3D	*p_pivot;	//	the pivot can be represented by an object
		bool		is_valid;
		bool		rendered;
} object_link_st;
typedef object_link *lp_object_link;

typedef struct bone
{
	CVector4f	length;
	CObject3D	*p_object;
	bool		is_locked;
	bool		rendered;
	vector<lp_object_link>	links;
} bone_st;
typedef bone *lp_bone;

static C3DSqueleton::C3DSqueletonClassID squeletonId;
const CPersistence::CPersistenceClassID& C3DSqueleton::C3DSqueletonClassID::GetClassId(void)
{
	return squeletonId;
}

RAPTOR_NAMESPACE_END


RAPTOR_NAMESPACE


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
C3DSqueleton::C3DSqueleton(CObject3D *root,const std::string& name)
    :CPersistence(squeletonId,name),
    p_root(NULL),m_renderState(false)
{
	if (root == NULL)
	{
		Raptor::GetErrorManager()->generateRaptorError(	C3DSqueleton::C3DSqueletonClassID::GetClassId(),
														CRaptorErrorManager::RAPTOR_WARNING,
														CRaptorMessages::ID_NULL_OBJECT);
        GL_COORD_VERTEX nullPosition(0,0,0,1);
	}
	else
	{
		void* mem = CHostMemoryManager::GetInstance()->allocate(sizeof(bone), 1, 16);
		lp_bone bone_root = new(mem) bone;

		bone_root->is_locked = false;
		bone_root->p_object = root;
		bone_root->length.Set(0,0,0,1);

		p_root = bone_root;
	}
}

C3DSqueleton::~C3DSqueleton()
{
	if (p_root != NULL)
	{
		deletePivots(p_root);
		delete p_root;
	}
}


void C3DSqueleton::deletePivots(lp_bone bone)
{
	int nb_pivots = bone->links.size();

	for (int i=0;i<nb_pivots;i++)
	{
		lp_object_link p_l = bone->links[i];

		if (NULL != p_l)
		{
			if (p_l->p_child != NULL)
			{
				deletePivots(p_l->p_child);
				delete p_l->p_child;
			}

			delete p_l;
		}
	}
}


void C3DSqueleton::glRenderBone(bone *bone)
{
	int nb_lnks = bone->links.size();

	if (nb_lnks > 1)
		glPushMatrix();

	glTranslatef(bone->length.X(),bone->length.Y(),bone->length.Z());

	bone->p_object->glRender();

	bone->rendered = m_renderState;

	for (int i=0;i<nb_lnks;i++)
	{
		if (bone->links[i]->rendered != m_renderState)
			glRenderPivot(bone->links[i]);
	}

	if (nb_lnks > 1)
		glPopMatrix();

	CATCH_GL_ERROR
}

void C3DSqueleton::glRenderPivot(object_link *link)
{
	glTranslatef(link->position.X(),link->position.Y(),link->position.Z());

	if (link->is_pivot)
		glRotatef(link->value,link->axis.X(),link->axis.Y(),link->axis.Z());
	else
		glTranslatef(link->axis.X(),link->axis.Y(),link->axis.Z());

	if (link->p_pivot != NULL)
		link->p_pivot->glRender();

	link->rendered = m_renderState;

	if (link->p_child->rendered != m_renderState)
		glRenderBone(link->p_child);

	CATCH_GL_ERROR
}


object_link	*C3DSqueleton::addLink(bone *parent,
								   CVector4f &position,
								   CVector4f &axis,
								   SQUELETON_LINK_TYPE type,
								   CObject3D *obj)
{
	if (parent == NULL)
		return NULL;

	void* mem = CHostMemoryManager::GetInstance()->allocate(sizeof(object_link), 1, 16);
	object_link *lnk = new(mem)object_link;


	parent->links.push_back(lnk);

	lnk->position = position;
	lnk->axis = axis;
	lnk->is_pivot = (type == PIVOT);
	lnk->is_valid = false;
	lnk->p_child = NULL;
	lnk->p_parent = parent;
	lnk->p_pivot = obj;
	lnk->value = 0;
	lnk->rendered = false;

	return lnk;
}

bone *C3DSqueleton::addBone(object_link *pivot,CVector4f & length,CObject3D *obj)
{
	if (pivot == NULL)
		return NULL;

	if (obj == NULL)
		return NULL;

	if (pivot->p_child != NULL)
	{
        Raptor::GetErrorManager()->generateRaptorError(	C3DSqueleton::C3DSqueletonClassID::GetClassId(),
														CRaptorErrorManager::RAPTOR_WARNING,
														CRaptorMessages::ID_LOST_LINK);
	}

	void* mem = CHostMemoryManager::GetInstance()->allocate(sizeof(bone), 1, 16);
	lp_bone b = new(mem)bone;

	b->is_locked = false;
	b->p_object = obj;
	b->length = length;
	b->rendered = false;

	pivot->p_child = b;

	return b;
}


