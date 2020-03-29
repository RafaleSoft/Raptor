// RaptorApplication.cpp: implementation of the CRaptorApplication class.
//
//////////////////////////////////////////////////////////////////////

#include "Subsys/CodeGeneration.h"

#if !defined(AFX_RAPTORCONFIG_H__29B753B8_17DE_44DF_A4D2_9D19C5AC53D5__INCLUDED_)
    #include "RaptorConfig.h"
#endif
#if !defined(AFX_CONTEXTMANAGER_H__F992F5F0_D8A5_475F_9777_B0EB30E7648E__INCLUDED_)
	#include "Subsys/ContextManager.h"
#endif
#if !defined(AFX_RAPTORAPPLICATION_H__E102DE21_30F2_4478_8C5A_55F780A0B33E__INCLUDED_)
    #include "RaptorApplication.h"
#endif
#if defined(WIN32)
    #if !defined(AFX_WIN32APPLICATION_H__3EADD210_ABF5_4CFD_A511_09047EDBB881__INCLUDED_)
        #include "Win32Specific/Win32Application.h"
    #endif
#elif defined(LINUX)
    #if !defined(AFX_GLXAPPLICATION_H__7384D172_A179_4048_BC2A_6D2C628442E5__INCLUDED_)
        #include "GLXSpecific/GLXApplication.h"
    #endif
#endif

#if !defined(AFX_RAPTORERRORMANAGER_H__FA5A36CD_56BC_4AA1_A5F4_451734AD395E__INCLUDED_)
	#include "System/RaptorErrorManager.h"
#endif
#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
	#include "System/Raptor.h"
#endif


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
RAPTOR_NAMESPACE_BEGIN

CRaptorApplication* CRaptorApplication::m_pInstance = NULL;

RAPTOR_NAMESPACE_END


RAPTOR_NAMESPACE

CRaptorApplication::CRaptorApplication()
	:m_pDisplay(NULL), m_bRunning(false)
{

}

CRaptorApplication::~CRaptorApplication()
{
    m_pInstance = NULL;
}

CRaptorApplication   *CRaptorApplication ::GetInstance(void)
{ 
    return m_pInstance; 
}

CRaptorApplication  *CRaptorApplication ::CreateApplication(void)
{
    if (m_pInstance != NULL)
        return m_pInstance;

#if defined(WIN32)
    m_pInstance = new CWin32Application();
#elif defined(LINUX)
    m_pInstance = new CGLXApplication();
#endif

    return m_pInstance;
}


void CRaptorApplication::setRootWindow(const RAPTOR_HANDLE& root)
{
	if ((root.handle() == 0) || (root.hClass() != WINDOW_CLASS))
	{
		RAPTOR_ERROR(CPersistence::CPersistenceClassID::GetClassId(),
					 "RaptorApplication has no root window !.");
		return;
	}

	m_root = root;
}

bool CRaptorApplication::initApplication(CRaptorDisplayConfig &config)
{
	//! A defaut config if Raptor has not been initialised.
    CRaptorConfig cfg;
    bool res = Raptor::glInitRaptor(cfg);

	if (res)
	{
		RAPTOR_HANDLE wnd = Raptor::glCreateWindow(config, m_pDisplay);
		m_root = wnd;

		res = (NULL != m_root.ptr<HWND__>());
	}

	return res;
}

bool CRaptorApplication::quitApplication(void)
{
	bool res = true;

	if (NULL != m_root.ptr<HWND__>())
	{
		if (NULL != m_pDisplay)
		{
			res = res && m_pDisplay->glvkBindDisplay(m_root);
			m_pDisplay->glvkReleaseResources();
			res = res && m_pDisplay->glvkUnBindDisplay();
			Raptor::glDestroyDisplay(m_pDisplay);
			m_pDisplay = NULL;
		}

		CContextManager *ctx = CContextManager::GetInstance();
		res = res && ctx->glDestroyWindow(m_root);

		RAPTOR_HANDLE noHandle;
		m_root = noHandle;
	}

	if (Raptor::GetConfig().m_bAutoDestroy)
		res = Raptor::glQuitRaptor();

	return res;
}
