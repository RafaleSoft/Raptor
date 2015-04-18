// ParticleDisplay.h: interface for the CParticleDisplay class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PARTICLEDISPLAY_H__1A93AE75_C51C_4A81_9EB2_66ADC362AF47__INCLUDED_)
#define AFX_PARTICLEDISPLAY_H__1A93AE75_C51C_4A81_9EB2_66ADC362AF47__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GenericDisplay.h"


class CParticleDisplay : public CGenericDisplay
{
public:
	// Construction/Destruction
	CParticleDisplay();
	virtual ~CParticleDisplay() {};

	// Initialization
	virtual void Init();

	// Implements CGenericDisplay
	virtual void Display();

	virtual const char* getTitle(void) const
	{ return "Particles demo"; };


private:
	// Reinitialiazation
	virtual void ReInit();

	virtual void UnInit();


	//	Particle generator
	CImageModifier *m_pModifier;

	// Particules clusters
    CParticleManager    *m_pParticleManager;
	CObject3DInstance *m_pParticle;
	CObject3DInstance* m_pFire;
	CObject3DInstance *m_pSmoke;
	CObject3DInstance *m_pSmoke2;

	// Particle scene
	C3DScene *m_pScene;

	// animations
	float dt;
	int numframe;
	int repeat;
};

#endif // !defined(AFX_PARTICLEDISPLAY_H__1A93AE75_C51C_4A81_9EB2_66ADC362AF47__INCLUDED_)
