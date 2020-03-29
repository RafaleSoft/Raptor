// RaptorMFCApplication.h: interface for the CRaptorMFCApplication class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RAPTORMFCAPPLICATION_H__0E7DFD68_2D1F_4837_B393_2D35DC18D55A__INCLUDED_)
#define AFX_RAPTORMFCAPPLICATION_H__0E7DFD68_2D1F_4837_B393_2D35DC18D55A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if !defined(AFX_RAPTORAPPLICATION_H__E102DE21_30F2_4478_8C5A_55F780A0B33E__INCLUDED_)
    #include "System/RaptorApplication.h"
#endif
#if !defined(AFX_RAPTORDISPLAYCONFIG_H__DA0759DF_6CF9_44A7_9ADE_D404FEEC2DDF__INCLUDED_)
#include "System/RaptorDisplayConfig.h"
#endif


class CWinApp;
class CFrameWnd;


RAPTOR_NAMESPACE_BEGIN


class RAPTOR_API CRaptorMFCApplication : public CRaptorApplication  
{
public:
	CRaptorMFCApplication();
	virtual ~CRaptorMFCApplication();

    //!    Creates a new CGLFrameWnd using Raptor Defaut Display Config and then 
    //!    sets this window as the application's root window.
    CFrameWnd *createRootWindow(const CRaptorDisplayConfig& glcs);


protected:
	//! Implements CRaptorApplication
	virtual bool initApplication(CRaptorDisplayConfig &config);

	//! Implements CRaptorApplication
    virtual bool run(void);

	//! Implements CRaptorApplication
    virtual void grabCursor(bool grab);


private:
	//!	We need a real CWinApp to handle window messages.
	CWinApp	*internal;
};

RAPTOR_NAMESPACE_END


#endif // !defined(AFX_RAPTORMFCAPPLICATION_H__0E7DFD68_2D1F_4837_B393_2D35DC18D55A__INCLUDED_)
