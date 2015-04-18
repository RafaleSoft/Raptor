// BumpDisplay.h: interface for the CBumpDisplay class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BUMPDISPLAY_H__4E542F6A_0175_4E6B_B43C_A000F4A33E63__INCLUDED_)
#define AFX_BUMPDISPLAY_H__4E542F6A_0175_4E6B_B43C_A000F4A33E63__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GenericDisplay.h"

class CBumpDisplay : public CGenericDisplay
{
public:
	CBumpDisplay();
	virtual ~CBumpDisplay();

	virtual void Init();

	virtual void Display();

	virtual const char* getTitle(void) const
	{ return "Bump mapping demo (Generic/Dot3/Pixel & Vertex Shaders)"; };

private:
	virtual void ReInit();

    virtual void UnInit();

	CBumppedGeometry	*teapot;
	C3DScene			*m_pScene;
    CLight				*m_pLight;
    CLight				*m_pLight2;
};

#endif // !defined(AFX_BUMPDISPLAY_H__4E542F6A_0175_4E6B_B43C_A000F4A33E63__INCLUDED_)
