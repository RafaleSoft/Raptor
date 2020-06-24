/***************************************************************************/
/*                                                                         */
/*  ColorController.h                                                      */
/*                                                                         */
/*    Raptor OpenGL & Vulkan realtime 3D Engine SDK.                       */
/*                                                                         */
/*  Copyright 1998-2019 by                                                 */
/*  Fabrice FERRAND.                                                       */
/*                                                                         */
/*  This file is part of the Raptor project, and may only be used,         */
/*  modified, and distributed under the terms of the Raptor project        */
/*  license, LICENSE.  By continuing to use, modify, or distribute         */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/


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
