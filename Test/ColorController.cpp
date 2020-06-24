/***************************************************************************/
/*                                                                         */
/*  ColorController.cpp                                                    */
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


#include "StdAfx.h"
#include <sstream>

#include "ColorController.h"

#include "System/Raptor.h"
#include "GLHierarchy/FragmentProgram.h"
#include "GLHierarchy/VertexProgram.h"

#include "ToolBox/Filters/ColorControlFilter.h"
#include "ToolBox/Filters/HDRFilter.h"
#include "ToolBox/Filters/MagnifierFilter.h"
#include "ToolBox/Filters/MBFilter.h"
#include "ToolBox/Filters/BlurFilter.h"
#include "ToolBox/Filters/DOFFilter.h"

ColorController::ColorController(CColorControlFilter* ccf,
								 CHDRFilter* hdr,
								 CMBFilter* mb,
								 CBlurFilter* bf,
								 CMagnifierFilter *mf)
	:pCCF(ccf),pHDR(hdr),pMB(mb),pBF(bf),pMag(mf),lastInput(0.0f)
{
	CRaptorConsole *pConsole = Raptor::GetConsole();
	
	CGLFont::FONT_TEXT_ITEM item;
	{
		float b = 0.0f;
		float c = 0.0f;
		if (ccf != NULL)
			ccf->getCorrection(b,c);
		stringstream str;
		str << "ColorControl: brightness=" << b << " saturation=" << c << "    ";
		if (ccf != NULL)
			str << (ccf->isEnabled() ? "[enabled]" : "[disabled]");
		else
			str << " n/a";
		item.text = str.str();
	}
	pConsole->addItem(item);

	{
		float r=0,g=0,b=0,a=0;
		if (mb != NULL)
			mb->getPercentage(r,b,g,a);
		stringstream str;
		str << "MotionBlur: percentage=" << r << " " << g << " " << b << " " << a << "    ";
		if (mb != NULL)
			str << (mb->isEnabled() ? "[enabled]" : "[disabled]");
		else
			str << " n/a";
		item.text = str.str();
	}
	pConsole->addItem(item);

	{
		unsigned int s = 0;
		CBlurFilter::BLUR_MODEL m = CBlurFilter::BLUR_BOX;
		if (bf != NULL)
		{
			bf->getBlurSize(s);
			bf->getBlurModel(m);
		}
		stringstream str;
		str << "Gaussian Blur: size=" << s << " model=";
		str << (m == CBlurFilter::BLUR_BOX ? "box" : (m == CBlurFilter::BLUR_GAUSSIAN ? "gaussian" : "gaussian + linear"));
		str << "    ";
		if (bf != NULL)
			str << (bf->isEnabled() ? "[enabled]" : "[disabled]");
		else
			str << " n/a";
		item.text = str.str();
	}
	pConsole->addItem(item);

	{
		stringstream str;
		str << "Magnifier: ";
		if (mf != NULL)
			str << (mf->isEnabled() ? "[enabled]" : "[disabled]");
		else
			str << " n/a";
		item.text = str.str();
	}
	pConsole->addItem(item);
}

ColorController::~ColorController(void)
{
}

void ColorController::handleCharacterInput(char c)
{
	if ((pCCF == NULL) || (pHDR == NULL) || (pMB == NULL) || (pBF == NULL) || (pMag == NULL))
		return;

	float dt = CTimeObject::GetGlobalTime();
	if ((dt - lastInput) < 0.2f)
		return;
	lastInput = dt;

	float br,ct;
	pCCF->getCorrection(br,ct);
	float r,g,b,a;
	pMB->getPercentage(r,b,g,a);
	unsigned int s;
	CBlurFilter::BLUR_MODEL m;
	pBF->getBlurSize(s);
	pBF->getBlurModel(m);

	switch(c)
	{
		case 'B':
		{
			br = MIN(br+0.01f,2.0f);
			break;
		}
		case 'b':
		{
			br = MAX(br-0.01f,0.0f);
			break;
		}
		case 'c':
		{
			ct = MAX(ct-0.01f,0.0f);
			break;
		}
		case 'C':
		{
			ct = MIN(ct+0.01f,1.0f);
			break;
		}
		case '&':
		{
			pCCF->enableFilter(!pCCF->isEnabled());
			break;
		}
		case 'é':
		{
			pMB->enableFilter(!pMB->isEnabled());
			break;
		}
		case '"':
		{
			pBF->enableFilter(!pBF->isEnabled());
			break;
		}
		case '\'':
		{
			pMag->enableFilter(!pMag->isEnabled());
			break;
		}
		case 'p':
		{
			b = g = r = MAX(r-0.01f,0.0f);
			break;
		}
		case 'P':
		{
			b = g = r = MIN(r+0.01f,1.0f);
			break;
		}
		case 'm':
		{
			if (m == CBlurFilter::BLUR_BOX)
				m = CBlurFilter::BLUR_GAUSSIAN;
			else
				m = CBlurFilter::BLUR_BOX;
		}
		default:
			break;
	}
	pCCF->setCorrection(br,ct);
	pMB->setPercentage(r,g,b,a);
	pBF->setBlurModel(m);

	CRaptorConsole *pConsole = Raptor::GetConsole();
	CGLFont::FONT_TEXT_ITEM item;
	{
		stringstream str;
		str << "ColorControl: brightness=" << br << " saturation=" << ct << "    ";
		str << (pCCF->isEnabled() ? "[enabled]" : "[disabled]");
		item.text = str.str();
	}
	pConsole->updateItem(item,0);

	{
		stringstream str;
		str << "MotionBlur: percentage=" << r << " " << g << " " << b << " " << a << "    ";
		str << (pMB->isEnabled() ? "[enabled]" : "[disabled]");
		item.text = str.str();
	}
	pConsole->updateItem(item,1);

	{
		stringstream str;
		str << "Gaussian Blur: size=" << s << " model=";
		str << (m == CBlurFilter::BLUR_BOX ? "box" : (m == CBlurFilter::BLUR_GAUSSIAN ? "gaussian" : "gaussian + linear"));
		str << "    ";
		str << (pBF->isEnabled() ? "[enabled]" : "[disabled]");
		item.text = str.str();
	}
	pConsole->updateItem(item,2);

	{
		stringstream str;
		str << "Magnifier: ";
		str << (pMag->isEnabled() ? "[enabled]" : "[disabled]");
		item.text = str.str();
	}
	pConsole->updateItem(item,3);
}


void ColorController::handleSpecialKeyInput(unsigned char k)
{ 
}

void ColorController::handleMouseInput(int button, int xpos, int ypos)
{ 
}