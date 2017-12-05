// EditDisplay.h: interface for the CEditDisplay class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EDITDISPLAY_H__9B61B938_52BB_48C0_ADAD_A5C7FD07F1C7__INCLUDED_)
#define AFX_EDITDISPLAY_H__9B61B938_52BB_48C0_ADAD_A5C7FD07F1C7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GenericDisplay.h"
#include "Engine/Physics.h"


class CEditDisplay  : public CGenericDisplay
{
public:
	CEditDisplay();
	virtual ~CEditDisplay();

	virtual void Init();

	virtual void Display();

	unsigned int	editVertex;
	CGeometry		*editObject;
	unsigned char	editColor[4];

private:
	float		dt;

	C3DSet				*set;
	CPhysics			*physicsEdit;
	CObject3DInstance	*instEdit;
	
	C3DSqueleton		*squeleton;
    CPhysics::CForce*			editForce;

	CGeometry	*strip;
	CGLRunner			*runner;

	virtual void		ReInit();

    virtual void UnInit();
};

#endif // !defined(AFX_EDITDISPLAY_H__9B61B938_52BB_48C0_ADAD_A5C7FD07F1C7__INCLUDED_)
