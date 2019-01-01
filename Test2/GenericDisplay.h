// GenericDisplay.h: interface for the CGenericDisplay class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GENERICDISPLAY_H__7589102B_1C0E_4CFD_978D_0F0D6B3E7683__INCLUDED_)
#define AFX_GENERICDISPLAY_H__7589102B_1C0E_4CFD_978D_0F0D6B3E7683__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Raptordll.h"

RAPTOR_NAMESPACE


class CGenericDisplay  
{
public:
	CGenericDisplay();
	virtual ~CGenericDisplay();

public:
	virtual void Display() {};

	virtual void Init() = 0;

	virtual void ReInit() = 0;

    virtual void UnInit() = 0;

	void DoInit() { reinit = true; };

	virtual const char* getTitle(void) const = 0;

protected:
	bool	reinit;

private:
	static RAPTOR_HANDLE reinitTMU;
};

#endif // !defined(AFX_GENERICDISPLAY_H__7589102B_1C0E_4CFD_978D_0F0D6B3E7683__INCLUDED_)
