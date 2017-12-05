// RootInstance.h: interface for the CRootInstance class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ROOTINSTANCE_H__31DB14EE_F01D_4D13_8F51_B9920514B7DE__INCLUDED_)
#define AFX_ROOTINSTANCE_H__31DB14EE_F01D_4D13_8F51_B9920514B7DE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "GLHierarchy/Object3DInstance.h"

RAPTOR_NAMESPACE


class CRootInstance : public CObject3DInstance  
{
public:
	CRootInstance();
	virtual ~CRootInstance();

	virtual void glClipRender();

	void renderBoxes(bool renderboxes) 
	{ m_bRenderBoxes = renderboxes; };

	void setSelection(CObject3D* pSelection)
	{ m_pSelection = pSelection; };

private:
	bool	m_bRenderBoxes;

	CObject3D*	m_pSelection;
};

#endif // !defined(AFX_ROOTINSTANCE_H__31DB14EE_F01D_4D13_8F51_B9920514B7DE__INCLUDED_)
