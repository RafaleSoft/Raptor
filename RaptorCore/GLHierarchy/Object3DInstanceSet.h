// Object3DInstanceSet.h: interface for the CObject3DInstanceSet class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OBJECT3DINSTANCESET_H__4552BADA_3CE7_40A5_9FE3_C10D58360193__INCLUDED_)
#define AFX_OBJECT3DINSTANCESET_H__4552BADA_3CE7_40A5_9FE3_C10D58360193__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if !defined(AFX_OBJECT3DINSTANCE_H__A2627662_F5F9_11D3_9142_CFEB8E9F2745__INCLUDED_)
	#include "Object3DInstance.h"
#endif


RAPTOR_NAMESPACE_BEGIN


//!
//!	This class offers the possibility to render a set
//! of objets in the following form:
//!
//!	Apply(transform1)
//!	Render(object1)
//!	Apply(trasform2)
//!	Render(object2)
//! ...
//!	Each transform is relative to the previous, and the
//! first one is relative to the ObjectInstance transform.
class RAPTOR_API CObject3DInstanceSet : public CObject3DInstance
{
public:
	CObject3DInstanceSet(const std::string& name = "OBJECT_INSTANCE_SET");
	virtual ~CObject3DInstanceSet();

	//!	Adds a new objet to the instance set.
	//! The transform supplied is relative to the previous object
	void Instantiate(CObject3D * const instance,const GL_MATRIX &transform);

	virtual void glRender();

	virtual void glClipRender();
	virtual bool RAPTOR_FASTCALL fullClip(const CGenericMatrix<float> &transform);
	virtual void glRenderBBox(bool filled = false);

	//! Inherited from CPersistence
	DECLARE_CLASS_ID(CObject3DInstanceSetClassID,"Object3DInstanceSet",CObject3DInstance)

private:
	typedef struct ObjectInstance_t
	{
		GL_MATRIX	transform;
		CObject3D	*pObject;
	} ObjectInstance;
	vector<ObjectInstance>	m_pObjects;

    CGenericMatrix<float>   m_finalTransform;
};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_OBJECT3DINSTANCESET_H__4552BADA_3CE7_40A5_9FE3_C10D58360193__INCLUDED_)

