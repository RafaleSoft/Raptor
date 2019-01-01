// WarpingDisplay.h: interface for the CWarpingDisplay class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WARPINGDISPLAY_H__8672D431_8AB4_4045_A068_1F609107F091__INCLUDED_)
#define AFX_WARPINGDISPLAY_H__8672D431_8AB4_4045_A068_1F609107F091__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GenericDisplay.h"

class CWarpObject;
class CGlassObject;
class CLargeObject;

class CWarpingDisplay : public CGenericDisplay
{
public:
	CWarpingDisplay();
	virtual ~CWarpingDisplay();

	virtual void Init();

	virtual void Display();

	virtual const char* getTitle(void) const
	{ return "Warping demo"; };

private:
	float	dt;
	virtual void ReInit();
    virtual void UnInit();

	CObject3DInstance	*m_pBall;
	CObject3DInstance	*m_pPlane;
	CObject3DInstance	*m_pWarp;
	CObject3DInstance	*m_pGlass;

	// Warping scene
	C3DScene *m_pScene;
};

#endif // !defined(AFX_WARPINGDISPLAY_H__8672D431_8AB4_4045_A068_1F609107F091__INCLUDED_)
