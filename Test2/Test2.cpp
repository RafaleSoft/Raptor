// Test2.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"

#include "Test2.h"
#include "Teapot.h"
#include "TestView.h"
#include "TestEngine.h"
#include "System/Raptor.h"
#include "System/RaptorConfig.h"
#include "System/RaptorErrorManager.h"
#include "Engine/Animator.h"
#include "Engine/TimeObject.h"
#include "MFCExtension/CWnd/GLWnd.h"
#include "MFCExtension/CWnd/GLFrameWnd.h"
#include "MFCExtension/Dialog/RaptorStatus.h"

#include "ToolBox/Imaging.h"
#include "ToolBox/Streaming.h"


/////////////////////////////////////////////////////////////////////////////
// CTest2App

BEGIN_MESSAGE_MAP(CTest2App, CWinApp)
	//{{AFX_MSG_MAP(CTest2App)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_RAPTOR_STATUS, OnRaptorStatus)
	ON_COMMAND(ID_OBJECT_STATUS, OnObjectStatus)
	ON_COMMAND(ID_VIDSETTINGS, OnVideoSettings)
	ON_COMMAND(ID_ABOUT_RAPTOR, OnAboutRaptor)
	ON_COMMAND(ID_RAPTORERRORS, OnRaptorerrors)
	ON_COMMAND_RANGE(ID_BUMPDEMO, ID_AMBIENTOCCLUSIONDEMO,OnSelectDemo)
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTest2App construction

CTest2App::CTest2App()
{
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CTest2App object

CTest2App theApp;

/////////////////////////////////////////////////////////////////////////////
// CTest2App initialization

BOOL CTest2App::InitInstance()
{
	SetRegistryKey(_T("Rafale Soft Inc. (c)"));

	LoadStdProfileSettings();  // Load standard INI file options (including MRU)

	//	initialize Raptor classes and settings
    CRaptorConfig config;
    config.m_bRelocation = true;
    config.m_uiPolygons = 300000;
    config.m_uiVertices = 1000000;
	config.m_uiTexels = 65536;
	config.m_logFile = "Test2_Raptor.log";
	bool init = Raptor::glInitRaptor(config);

	CTimeObject::setTimeFactor(1.0f);
    Animator = new CAnimator();
	CAnimator::SetAnimator(Animator);

    CImaging::installImagers(CTextureFactory::getDefaultFactory());
    CStreaming::installStreamers();

	char title[256];
	unsigned int v = Raptor::GetVersion();
	sprintf(title,"Your empty starting Raptor %d.%d.%d Release test",(v>>24)&0xFF,(v>>16)&0xFF,(v>>8)&0xFF);

	CRaptorDisplayConfig glcs;
	glcs.width = 640;
	glcs.height = 480;
	glcs.x = 10;
	glcs.y = 10;
	glcs.caption = title;
	glcs.display_mode = CGL_RGBA | CGL_DEPTH | CGL_DOUBLE | CGL_STENCIL | //CGL_ACCUM |*/
						CGL_HARDWARE | CGL_ANTIALIAS_4X; 
	glcs.frame_mode = CGL_DRAWLOGO;//| CGL_NOSTATUS;
	glcs.refresh_rate.fps = CGL_MAXREFRESHRATE;
	glcs.framebufferState.colorClearValue = CColor::RGBA(0.5f,0.6f,0.7f,1.0f);
	glcs.hintState.pointSmooth = GL_NICEST;

	CGLWnd::SetDefaultDisplayConfig(glcs);

	//if (!Animator->startRecord("test2.avi",400,300))
	//	AfxMessageBox("AVIStartRecord failed");
	Engine = new CTestEngine();
	C3DEngine::Set3DEngine(Engine);

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CTeapot),
		RUNTIME_CLASS(CGLFrameWnd),
		RUNTIME_CLASS(CTestView));
	AddDocTemplate(pDocTemplate);

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// Dispatch commands specified on the command line
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();

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
void CTest2App::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CTest2App message handlers

int CTest2App::ExitInstance() 
{
	return CWinApp::ExitInstance();
}

void CTest2App::OnRaptorStatus() 
{
	CRaptorStatus Status;
	Status.InitDlg();
}

void CTest2App::OnRaptorerrors() 
{
	CRaptorStatus Status;
	Status.ErrorsDlg();
}

void CTest2App::OnObjectStatus() 
{
	CRaptorStatus Status;
	Status.ObjectsDlg();
}

void CTest2App::OnAboutRaptor() 
{
	CRaptorStatus Status;
	Status.AboutDlg();
	//Status.TMUSetupDialog();
}

void CTest2App::OnVideoSettings() 
{
	CRaptorStatus Status;
	
	unsigned int color_depth = CGL_16BITS | CGL_32BITS;
	unsigned int screen_size = CGL_640x480 | CGL_800x600 | CGL_1024x768 | CGL_1280x1024;
	unsigned int screen_freq = 0xFFFF;
	Status.VideoSettingsDlg(color_depth,screen_size,screen_freq);
}

BOOL CTest2App::OnIdle(LONG lCount) 
{
	Raptor::glRender();
//	CWinApp::OnIdle(lCount);
	CWinThread::OnIdle(lCount);
	return TRUE;	// we want animation, so need more idle time
}

void CTest2App::OnSelectDemo(UINT nID ) 
{
	CTest2App *pApp = (CTest2App*)AfxGetApp();

	POSITION pos = pApp->GetFirstDocTemplatePosition( );
	while (pos != NULL)
	{
		CDocTemplate* pDocTemplate = pApp->GetNextDocTemplate(pos);
		POSITION pos2 = pDocTemplate->GetFirstDocPosition( );
		while (pos2 != NULL)
		{
			CDocument* pDoc = pDocTemplate->GetNextDoc( pos2 );
			//	redisplay only GLframes. 
			//	to avoid RTTI, GLDocuments could be retrieved
			//	through persistence or stored in an array
			if (pDoc->IsKindOf( RUNTIME_CLASS(CTeapot)))
				//	better because manages frame refresh too
				switch(nID)
				{
					case ID_BUMPDEMO:
						((CTeapot*)pDoc)->SwitchDemo(BUMPDEMO);
						break;
					case ID_COLLISIONDEMO:
						((CTeapot*)pDoc)->SwitchDemo(COLLISIONDEMO);
						break;
					case ID_SKINNINGDEMO:
						((CTeapot*)pDoc)->SwitchDemo(SKINNINGDEMO);
						break;
					case ID_SPLINEDEMO:
						((CTeapot*)pDoc)->SwitchDemo(SPLINEDEMO);
						break;
					case ID_SHADOWDEMO:
						((CTeapot*)pDoc)->SwitchDemo(SHADOWDEMO);
						break;
					case ID_EDITDEMO:
						((CTeapot*)pDoc)->SwitchDemo(EDITDEMO);
						break;
					case ID_LODDEMO:
						((CTeapot*)pDoc)->SwitchDemo(LODDEMO);
						break;
					case ID_PARTICLEDEMO:
						((CTeapot*)pDoc)->SwitchDemo(PARTICLEDEMO);
						break;
					case ID_WARPINGDEMO:
						((CTeapot*)pDoc)->SwitchDemo(WARPINGDEMO);
						break;
					case ID_PROJECTIONDEMO:
						((CTeapot*)pDoc)->SwitchDemo(PROJECTIONDEMO);
						break;
					case ID_VERTEXSHADERSDEMO:
						((CTeapot*)pDoc)->SwitchDemo(VRTXSHADERSDEMO);
						break;
					case ID_SHADOWMAPDEMO:
						((CTeapot*)pDoc)->SwitchDemo(SHADOWMAPDEMO);
						break;
					case ID_PERPIXELEMBMDEMO:
						((CTeapot*)pDoc)->SwitchDemo(EMBMDEMO);
						break;
					case ID_AMBIENTOCCLUSIONDEMO:
						((CTeapot*)pDoc)->SwitchDemo(AMBIENTOCCLUSIONDEMO);
						break;
				}
		}
	}
}

