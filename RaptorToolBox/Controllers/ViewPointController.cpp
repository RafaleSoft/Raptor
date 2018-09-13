#include "Subsys/CodeGeneration.h"

#if !defined(AFX_CONTROLLERS_H__DCDB86E4_E50D_4B97_BBE2_AD0425D0D925__INCLUDED_)
	#include "Controllers/ViewPointController.h"
#endif



CViewPointController::CViewPointController(CViewPoint *pVP)
	:m_pViewPoint(pVP),
	oldxpos(-1),oldypos(-1)
{
}

CViewPointController::~CViewPointController(void)
{
}

void CViewPointController::handleCharacterInput(char c)
{
}

void CViewPointController::handleSpecialKeyInput(unsigned char k)
{
	switch (k)
	{
		case 0x26: //VK_UP
		{
			float reverse = (m_bReverseZAxis ? -m_fScaleZ : m_fScaleZ);
			m_pViewPoint->translate(0.0f,0.0f,-reverse);
			break;
		}
		case 0x28: //VK_DOWN:
		{
			float reverse = (m_bReverseZAxis ? -m_fScaleZ : m_fScaleZ);
			m_pViewPoint->translate(0.0f,0.0f,reverse);
			break;
		}
		case 0x25: //VK_LEFT:
		{
			float reverse = (m_bReverseXAxis ? -m_fScaleX : m_fScaleX);
			m_pViewPoint->translate(-reverse,0.0f,0.0f);
			break;
		}
		case 0x27: //VK_RIGHT:
		{
			float reverse = (m_bReverseXAxis ? -m_fScaleX : m_fScaleX);
			m_pViewPoint->translate(reverse,0.0f,0.0f);
			break;
		}
		case 0x21: //VK_PRIOR:
		{
			float reverse = (m_bReverseYAxis ? -m_fScaleY : m_fScaleY);
			m_pViewPoint->translate(0.0f,-reverse,0.0f);
			break;
		}
		case 0x22: //VK_NEXT:
		{
			float reverse = (m_bReverseYAxis ? -m_fScaleY : m_fScaleY);
			m_pViewPoint->translate(0.0f,reverse,0.0f);
			break;
		}
	}
}

void CViewPointController::handleMouseInput(int button, int xpos, int ypos)
{		
	if ((oldxpos != xpos) || (oldypos != ypos))
	{
		if (oldxpos > 0)
		{
			float deltax = (m_bReverseXAxis ? -m_fScaleX : m_fScaleX) * (xpos-oldxpos);
			float deltay = (m_bReverseXAxis ? -m_fScaleY : m_fScaleY) * (ypos-oldypos);
			//m_pViewPoint->translate(deltax,deltay,0.0f);
			m_pViewPoint->rotationY(deltax);
			m_pViewPoint->rotationX(deltay);
		}

		oldxpos = xpos;
		oldypos = ypos;
	}
}

