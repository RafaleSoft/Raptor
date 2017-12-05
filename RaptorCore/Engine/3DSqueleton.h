// 3DSqueleton.h: interface for the C3DSqueleton class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_3DSQUELETON_H__3F4B9881_392F_4267_841C_3BA7929FCAC6__INCLUDED_)
#define AFX_3DSQUELETON_H__3F4B9881_392F_4267_841C_3BA7929FCAC6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#ifndef __SIMD_H__
	#include "Subsys/SimdLib/simd.h"
#endif
#if !defined(AFX_PERSISTENCE_H__5561BA28_831B_11D3_9142_EEB51CEBBDB0__INCLUDED_)
    #include "GLHierarchy/Persistence.h"
#endif

RAPTOR_NAMESPACE_BEGIN


class CObject3D;

class RAPTOR_API C3DSqueleton : public CPersistence
{
public:
	typedef enum
	{
		PIVOT,
		GUIDE,
		NB_TYPE
	} SQUELETON_LINK_TYPE;

    typedef struct bone bone_st;
    typedef struct object_link object_link_st;


public:
	C3DSqueleton(CObject3D *root = NULL,const std::string& name = "3D_SQUELETON");
	virtual ~C3DSqueleton();


	object_link	*addLink(	bone *parent,
							CVector4f &position,
							CVector4f &axis,
							SQUELETON_LINK_TYPE type = C3DSqueleton::PIVOT,
							CObject3D *obj = NULL);

	bone		*addBone(object_link *pivot,CVector4f &length, CObject3D *obj);

	bone		*getRootBone(void) const { return p_root; };

	//!	Implements CPersistence
	DECLARE_CLASS_ID(C3DSqueletonClassID,"Squeleton",CPersistence)


private:
	void deletePivots(bone *bone);
	void glRenderBone(bone *bone);
	void glRenderPivot(object_link *link);

	bone	*p_root;
	bool	m_renderState;
};


RAPTOR_NAMESPACE_END


#endif // !defined(AFX_3DSQUELETON_H__3F4B9881_392F_4267_841C_3BA7929FCAC6__INCLUDED_)

