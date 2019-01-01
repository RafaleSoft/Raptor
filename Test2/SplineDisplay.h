// SplineDisplay.h: interface for the CSplineDisplay class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SPLINEDISPLAY_H__47349156_EBDB_4366_ACE4_CACA1514286E__INCLUDED_)
#define AFX_SPLINEDISPLAY_H__47349156_EBDB_4366_ACE4_CACA1514286E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GenericDisplay.h"

class CSplineDisplay : public CGenericDisplay
{
public:
	CSplineDisplay();
	virtual ~CSplineDisplay();

	virtual void Init();

	virtual void Display();

	virtual const char* getTitle(void) const
	{ return "Spline demo"; };


private:
	C3DPath			*path;
	CShadedGeometry	*bspline;

	unsigned int	text;
	CGLFont			*font;
	CTextureSet		*texture;

	IViewPoint		*vp;
	CLight			*m_pLight;

	virtual void ReInit();

    virtual void UnInit();
};

#endif // !defined(AFX_SPLINEDISPLAY_H__47349156_EBDB_4366_ACE4_CACA1514286E__INCLUDED_)
