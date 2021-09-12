/***************************************************************************/
/*                                                                         */
/*  GLBench.cpp                                                            */
/*                                                                         */
/*    Raptor OpenGL & Vulkan realtime 3D Engine SDK.                       */
/*                                                                         */
/*  Copyright 1998-2021 by                                                 */
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
	IRaptor::glInitRaptor(config);

    CImaging::installImagers();

	CGLBenchDlg dlg;
	m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();
	
	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
