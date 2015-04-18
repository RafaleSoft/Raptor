// LodDisplay.h: interface for the CLodDisplay class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LODDISPLAY_H__063F37A7_DDE1_4AAE_AECF_913465CEB60F__INCLUDED_)
#define AFX_LODDISPLAY_H__063F37A7_DDE1_4AAE_AECF_913465CEB60F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GenericDisplay.h"


class CLodDisplay : public CGenericDisplay
{
public:
	CLodDisplay();
	virtual ~CLodDisplay();

	virtual void Init();

	virtual void Display();

	virtual const char* getTitle(void) const
	{ return "Dynamic LOD demo"; };


private:
	float		dt;

	CGLLod		*lod;
	CMaterial	*material;
	CGLLayer	*layer;
	CGLFont		*font;

	virtual void	ReInit();

    virtual void UnInit();

	void	CountFaces(void);
};

#endif // !defined(AFX_LODDISPLAY_H__063F37A7_DDE1_4AAE_AECF_913465CEB60F__INCLUDED_)
