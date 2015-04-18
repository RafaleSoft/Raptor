#pragma once
#include "ToolBox/Controllers.h"

RAPTOR_NAMESPACE

class CColorControlFilter;
class CHDRFilter;
class CMBFilter;
class CBlurFilter;
class CMagnifierFilter;

class ColorController :	public CControllers::CController
{
public:
	ColorController(CColorControlFilter* ccf,
					CHDRFilter* hdr,
					CMBFilter* mb,
					CBlurFilter* bf,
					CMagnifierFilter *mf);

	virtual ~ColorController(void);

	virtual void handleCharacterInput(char c);

	virtual void handleSpecialKeyInput(unsigned char k);

	virtual void handleMouseInput(int button, int xpos, int ypos);
	
private:
	float lastInput;
	CColorControlFilter* pCCF;
	CHDRFilter* pHDR;
	CMBFilter*	pMB;
	CBlurFilter* pBF;
	CMagnifierFilter *pMag;
};
