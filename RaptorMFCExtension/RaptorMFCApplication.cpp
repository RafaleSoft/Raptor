// RaptorMFCApplication.cpp: implementation of the CRaptorMFCApplication class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"

#if !defined(AFX_RAPTORMFCAPPLICATION_H__0E7DFD68_2D1F_4837_B393_2D35DC18D55A__INCLUDED_)
    #include "RaptorMFCApplication.h"
#endif
#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
	#include "System/Raptor.h"
#endif
#if !defined(AFX_RAPTORCONFIG_H__29B753B8_17DE_44DF_A4D2_9D19C5AC53D5__INCLUDED_)
    #include "System/RaptorConfig.h"
#endif
#if !defined(AFX_RAPTORERRORMANAGER_H__FA5A36CD_56BC_4AA1_A5F4_451734AD395E__INCLUDED_)
    #include "System/RaptorErrorManager.h"
#endif
#if !defined(AFX_GLFRAMEWND_H__F8E95403_4608_11D3_9142_9135643D7AEC__INCLUDED_)
	#include "CWnd/GLFrameWnd.h"
#endif
#if !defined(AFX_GLWND_H__9DD83791_854B_11D2_9142_830BB0D20AF2__INCLUDED_)
	#include "CWnd/GLWnd.h"
#endif

RAPTOR_NAMESPACE_BEGIN

class APP : public CWinApp
{
public:
    APP() {};

    BOOL OnIdle(LONG lCount) 
    {
	    Raptor::glRender();
	    CWinThread::OnIdle(lCount);
	    return TRUE;	// we want animation, so need more idle time
    };

    int ExitInstance()
    {
        if (Raptor::GetConfig().m_bAutoDestroy)
            Raptor::glQuitRaptor();
        return CWinApp::ExitInstance();
    }

    BOOL ProcessMessageFilter(int code, LPMSG lpMsg)
    {
        if (lpMsg->message == WM_QUIT)
        {
            return Raptor::glQuitRaptor();
        }
        else return CWinThread::ProcessMessageFilter(code,lpMsg);
    }
};

//! static data will be initialized properly at MFC initialization.
static APP app;

RAPTOR_NAMESPACE_END


RAPTOR_NAMESPACE


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CRaptorMFCApplication::CRaptorMFCApplication()
{
    internal = &app;
}

CRaptorMFCApplication::~CRaptorMFCApplication()
{
}

void CRaptorMFCApplication::grabCursor(bool grab)
{
	//m_bGrab = grab;
}

bool CRaptorMFCApplication::initApplication(void)
{
    return CRaptorApplication::initApplication();
}

CFrameWnd *CRaptorMFCApplication::createRootWindow(const CRaptorDisplayConfig& glcs)
{
	CGLWnd::SetDefaultDisplayConfig(glcs);

    CFrameWnd *wnd = new CGLFrameWnd();

    RECT rect;
    rect.left = glcs.x;
    rect.top = glcs.y;
    rect.right = glcs.x + glcs.width;
    rect.bottom = glcs.y + glcs.height;

	BOOL res = wnd->CreateEx(	WS_EX_TOPMOST,
								NULL,
								CA2T(glcs.caption.data()),
								WS_VISIBLE|WS_OVERLAPPED|WS_SYSMENU,
								rect,
								NULL,
								NULL,
								NULL);
	if (TRUE == res)
    {
		//RECT rect2;
		//wnd->GetWindowRect(&rect);
		//wnd->GetClientRect(&rect2);

        internal->m_pMainWnd = wnd;

        RAPTOR_HANDLE hWnd(WINDOW_CLASS,wnd);
        CRaptorApplication::setRootWindow(hWnd);
    }
    else
    {
        wnd = NULL;
		Raptor::GetErrorManager()->generateRaptorError( CPersistence::CPersistenceClassID::GetClassId(),
														CRaptorErrorManager::RAPTOR_ERROR,
														"RaptorApplication cannot create root window !.");
    }

    return wnd;
}

void CRaptorMFCApplication::setRootWindow(const RAPTOR_HANDLE& root)
{
    if ((root.handle == 0) || (root.hClass != WINDOW_CLASS))
    {
        Raptor::GetErrorManager()->generateRaptorError( CPersistence::CPersistenceClassID::GetClassId(),
														CRaptorErrorManager::RAPTOR_ERROR,
														"RaptorApplication has no root window !.");
        return;
    }

    CFrameWnd *wnd = (CFrameWnd*)(root.handle);
    internal->m_pMainWnd = wnd;

    CRaptorApplication::setRootWindow(root);
}

bool CRaptorMFCApplication::run(void)
{
    if (internal->m_pMainWnd == NULL)
    {
        Raptor::GetErrorManager()->generateRaptorError(	CPersistence::CPersistenceClassID::GetClassId(),
														CRaptorErrorManager::RAPTOR_ERROR,
														"RaptorApplication has no root window !.");
        return false;
    }

    internal->Run();
 
    return true;
}