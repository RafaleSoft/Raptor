// AmbientOcclusionDisplay.h: interface for the CAmbientOcclusionDisplay class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AMBIENTOCCLUSIONDISPLAY_H__2E407164_6C58_4d08_B1AD_D1CD86D56E61__INCLUDED_)
#define AFX_AMBIENTOCCLUSIONDISPLAY_H__2E407164_6C58_4d08_B1AD_D1CD86D56E61__INCLUDED_


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "genericdisplay.h"


class CAmbientOcclusionDisplay : public CGenericDisplay
{
public:
	CAmbientOcclusionDisplay(void);
	virtual ~CAmbientOcclusionDisplay(void);

	virtual void Init();

	virtual void Display();

	virtual const char* getTitle(void) const
	{ return "Ambient Occlusion demo"; };

private:
	virtual void	ReInit();
    virtual void	UnInit();

	float dt;
	CShadedGeometry	*m_pTeapot;
	CShadedGeometry *m_pMesh;
	CViewModifier	*m_pVM;
	CLight			*m_pLight;
};

#endif // !defined(AFX_AMBIENTOCCLUSIONDISPLAY_H__2E407164_6C58_4d08_B1AD_D1CD86D56E61__INCLUDED_)