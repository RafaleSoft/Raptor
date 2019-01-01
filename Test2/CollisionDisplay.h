// CollisionDisplay.h: interface for the CCollisionDisplay class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COLLISIONDISPLAY_H__E50E70E0_7ED1_4D8A_936D_FC517D32673A__INCLUDED_)
#define AFX_COLLISIONDISPLAY_H__E50E70E0_7ED1_4D8A_936D_FC517D32673A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GenericDisplay.h"


class CCollisionDisplay  : public CGenericDisplay
{
public:
	CCollisionDisplay();
	virtual ~CCollisionDisplay();

	virtual void Init();

	virtual void Display();

	virtual const char* getTitle(void) const
	{ return "Collision demo"; };

private:

	float		dt;

	CTextureFactory	*texture;
	CGeometry	*ball;
	CGeometry	*ground;

	CObject3DInstance	*instGround;
	CObject3DInstance	*instBall;
	CPhysics			*physicsGround;
	CPhysics			*physicsBall;


	virtual void	ReInit();

    virtual void UnInit();
};

#endif // !defined(AFX_COLLISIONDISPLAY_H__E50E70E0_7ED1_4D8A_936D_FC517D32673A__INCLUDED_)
