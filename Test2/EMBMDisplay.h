// EMBMDisplay.h: interface for the CEMBMDisplay class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EMBMDISPLAY_H__6BC639D8_797C_4A8F_8B92_74C93E4634AE__INCLUDED_)
#define AFX_EMBMDISPLAY_H__6BC639D8_797C_4A8F_8B92_74C93E4634AE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GenericDisplay.h"


class CEMBMDisplay  : public CGenericDisplay
{
public:
	CEMBMDisplay();
	virtual ~CEMBMDisplay();

	virtual void Init();

	virtual void Display();

	virtual const char* getTitle(void) const
	{ return "Environment Map Bump Mapping demo ( Pixel & Vertex Shaders)"; };


private:
	virtual void ReInit();

    virtual void UnInit();

	CBumppedGeometry	*embm;
	C3DScene			*m_pScene;
};

#endif // !defined(AFX_EMBMDISPLAY_H__6BC639D8_797C_4A8F_8B92_74C93E4634AE__INCLUDED_)
