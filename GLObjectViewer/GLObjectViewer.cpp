// GLObjectViewer.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "GLObjectViewer.h"

#include "MainFrm.h"
#include "ChildFrm.h"

#include "GLObjectViewerDoc.h"
#include "GLObjectViewerView.h"

#include "System/Raptor.h"
#include "System/RaptorConfig.h"
#include "System/RaptorErrorManager.h"
#include "ToolBox/Imaging.h"
#include <strstream>

#include "MFCExtension/CWnd/GLWnd.h"


RAPTOR_NAMESPACE


/////////////////////////////////////////////////////////////////////////////
// CGLObjectViewerApp

BEGIN_MESSAGE_MAP(CGLObjectViewerApp, CWinApp)
	//{{AFX_MSG_MAP(CGLObjectViewerApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	//ON_COMMAND(ID_FILE_SAVE, OnFileSave)
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGLObjectViewerApp construction

CGLObjectViewerApp::CGLObjectViewerApp()
{
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CGLObjectViewerApp object

CGLObjectViewerApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CGLObjectViewerApp initialization

BOOL CGLObjectViewerApp::InitInstance()
{
	SetRegistryKey(_T("Raptor GLObjectViewer"));

	LoadStdProfileSettings();  // Load standard INI file options (including MRU)

    CRaptorConfig cfg;
    cfg.m_bRelocation = true;
    cfg.m_uiPolygons = 1000000;
    cfg.m_uiVertices = 4000000;
    Raptor::glInitRaptor(cfg);

    Raptor::GetErrorManager()->logToFile("GLObjectViewer.log");

    unsigned long v = Raptor::GetVersion();
	ostrstream title;
    title << "Your empty starting Raptor ";
    title << ((v>>24)&0xFF) << "." << ((v>>16)&0xFF) << "." << ((v>>8)&0xFF);
    title << " Release test";
    title << ends;

    CRaptorDisplayConfig glcs;
	glcs.width = 1024;
	glcs.height = 768;
	glcs.x = 0;
	glcs.y = 0;
	glcs.caption = title.str();
	glcs.display_mode = CGL_RGBA | CGL_DEPTH | CGL_DOUBLE | CGL_HARDWARE | CGL_ANTIALIAS_16X;
	glcs.draw_logo = true;
    
	CGLWnd::SetDefaultDisplayConfig(glcs);

    CImaging::installImagers(CTextureFactory::getDefaultFactory());


	CMultiDocTemplate* pDocTemplate;
	pDocTemplate = new CMultiDocTemplate(
		IDR_GLOBJETYPE,
		RUNTIME_CLASS(CGLObjectViewerDoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CGLObjectViewerView));
	AddDocTemplate(pDocTemplate);

	// create main MDI Frame window
	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame->LoadFrame(IDR_MAINFRAME))
		return FALSE;
	m_pMainWnd = pMainFrame;

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// Dispatch commands specified on the command line
//	if (!ProcessShellCommand(cmdInfo))
//		return FALSE;

	// The main window has been initialized, so show and update it.
	pMainFrame->ShowWindow(m_nCmdShow);
	pMainFrame->UpdateWindow();

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CGLObjectViewerApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CGLObjectViewerApp message handlers


void CGLObjectViewerApp::OnFileOpen() 
{
	CWinApp::OnFileOpen();
	
	Raptor::glRender();
}

void CGLObjectViewerApp::OnFileSave() 
{
	int i = 0;
}

