#include "Subsys/CodeGeneration.h"

#if !defined(AFX_CONTROLLERS_H__CC9BBAE1_0481_4CC0_A421_7BB690656844__INCLUDED_)
	#include "Controllers.h"
#endif

#if !defined(AFX_CONTROLLERS_H__DCDB86E4_E50D_4B97_BBE2_AD0425D0D925__INCLUDED_)
	#include "Controllers/ViewPointController.h"
#endif


CControllers::CController::CController()
{
	m_fScaleX = m_fScaleY = m_fScaleZ = 1.0f;
	m_bReverseXAxis = false;
	m_bReverseYAxis = true;
	m_bReverseZAxis = false;
	m_pInput = new CRaptorConsole::CInputCollector<CController>(*this);
}

CControllers::CController::~CController()
{
	delete m_pInput;
}

CControllers::CControllers(void)
{
}

CControllers::~CControllers(void)
{
}

CControllers::CController *CControllers::createViewpointController(CViewPoint* pVp)
{
	CViewPointController *pController = new CViewPointController(pVp);

	return pController;
}

bool CControllers::CController::reverseXAxis(void)
{
	bool res = m_bReverseXAxis;
	m_bReverseXAxis = !m_bReverseXAxis;
	return res;
}

bool CControllers::CController::reverseYAxis(void)
{
	bool res = m_bReverseYAxis;
	m_bReverseYAxis = !m_bReverseYAxis;
	return res;
}

bool CControllers::CController::reverseZAxis(void)
{
	bool res = m_bReverseZAxis;
	m_bReverseZAxis = !m_bReverseZAxis;
	return res;
}

void CControllers::CController::scale(float sx,float sy,float sz)
{
	if (sx != 0)
		m_fScaleX = sx;
	if (sy != 0)
		m_fScaleY = sy;
	if (sz != 0)
		m_fScaleZ = sz;
}
