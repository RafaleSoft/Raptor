
#if !defined(AFX_CONTROLLERS_H__DCDB86E4_E50D_4B97_BBE2_AD0425D0D925__INCLUDED_)
#define AFX_CONTROLLERS_H__DCDB86E4_E50D_4B97_BBE2_AD0425D0D925__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if !defined(AFX_CONTROLLERS_H__CC9BBAE1_0481_4CC0_A421_7BB690656844__INCLUDED_)
	#include "Controllers.h"
#endif

RAPTOR_NAMESPACE

class CViewPointController : public CControllers::CController
{
public:
	CViewPointController(IViewPoint	*pVP);
	virtual ~CViewPointController(void);

private:
	//@see RaptorConsole
	void handleCharacterInput(char c);

	//@see RaptorConsole
	void handleSpecialKeyInput(unsigned char k);

	//@see RaptorConsole
	void handleMouseInput(int button, int xpos, int ypos);

	//! The controlled view point
	IViewPoint	*m_pViewPoint;

	int oldxpos;
	int oldypos;
};

#endif //!defined(AFX_CONTROLLERS_H__DCDB86E4_E50D_4B97_BBE2_AD0425D0D925__INCLUDED_)

