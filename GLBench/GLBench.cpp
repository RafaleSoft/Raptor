// GLBench.cpp : Defines the class behaviors for the application.
//

#include "StdAfx.h"
#include "GLBench.h"
#include "GLBenchDlg.h"

#include "System/Raptor.h"
#include "System/RaptorConfig.h"

#include "../RaptorToolBox/Imaging.h"

RAPTOR_NAMESPACE

/////////////////////////////////////////////////////////////////////////////
// CGLBenchApp

BEGIN_MESSAGE_MAP(CGLBenchApp, CWinApp)
	//{{AFX_MSG_MAP(CGLBenchApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGLBenchApp construction

CGLBenchApp::CGLBenchApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CGLBenchApp object

CGLBenchApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CGLBenchApp initialization

BOOL CGLBenchApp::InitInstance()
{
	//	initialize Raptor classes and settings
    CRaptorConfig config;
	Raptor::glInitRaptor(config);

    CImaging::installImagers(CTextureFactory::getDefaultFactory());

	CGLBenchDlg dlg;
	m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();
	
	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
