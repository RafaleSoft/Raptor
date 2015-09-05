// RaysClient.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "RaysClient.h"

#include "MainFrm.h"
#include "ChildFrm.h"
#include "RaysClientDoc.h"
#include "RaysClientView.h"
#include "ObjectView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define NUM_IMG 7
int ObjectsImg[NUM_IMG] =
{
	IDR_RAYSTYPE,
	IDI_CAMERA,
	IDI_SURFACE,
	IDI_PLUGIN,
	IDI_EXTERN,
	IDI_OBJDATA,
	IDI_CAMAXES,
};
/////////////////////////////////////////////////////////////////////////////
// CRaysClientApp

BEGIN_MESSAGE_MAP(CRaysClientApp, CWinApp)
	//{{AFX_MSG_MAP(CRaysClientApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_FILE_NEW, OnFileNew)
	//}}AFX_MSG_MAP
	// Standard file based document commands
//	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()


__inline void Cout(CString msg)
{
	CRaysClientApp *pApp = (CRaysClientApp*)AfxGetApp();
	CListBox *list = (CListBox*)(pApp->Output->GetDlgItem(IDC_OUTPUT));

	if (list->GetCount()==100)
		list->DeleteString(99);
	list->InsertString(0,msg);
}
/////////////////////////////////////////////////////////////////////////////
// CRaysClientApp construction

CRaysClientApp::CRaysClientApp()
{
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CRaysClientApp object

CRaysClientApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CRaysClientApp initialization

BOOL CRaysClientApp::InitInstance()
{
	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();
	Enable3dControls();			// Call this when using MFC in a shared DLL

	SetRegistryKey(_T("Rafale Soft Inc(c)"));
	LoadStdProfileSettings();  // Load standard INI file options (including MRU)

	// build icon image list
	ObjectsData = new CImageList();
	ObjectsData->Create(16,16,ILC_COLOR32,0,1);
	for (int i=0;i<NUM_IMG;i++)
		ObjectsData->Add(LoadIcon(ObjectsImg[i]));

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.

	CMultiDocTemplate* pDocTemplate;
	pDocTemplate = new CMultiDocTemplate(
		IDR_RAYSTYPE,
		RUNTIME_CLASS(CRaysClientDoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CRaysClientView));
	AddDocTemplate(pDocTemplate);

	// create main MDI Frame window
	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame->LoadFrame(IDR_MAINFRAME))
		return FALSE;
	m_pMainWnd = pMainFrame;

	// Enable drag/drop open
	m_pMainWnd->DragAcceptFiles();

	// Enable DDE Execute open
	EnableShellOpen();
	RegisterShellFileTypes(TRUE);

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// Dispatch commands specified on the command line
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// Creating tool windows
	pMainFrame->EnableDocking(CBRS_ALIGN_ANY);
	Objects = new CDialogBar();
	Output = new CDialogBar();
	CRect rect;

	//	Transformer rect en screen coordinates !!!

	rect.left = rect.top = 0;
	rect.bottom = rect.right = 300;
	Objects->Create(pMainFrame,IDD_FORMVIEW,CBRS_ALIGN_LEFT|CBRS_FLOATING|CBRS_BORDER_ANY|CBRS_GRIPPER|CBRS_BORDER_3D,AFX_IDW_DIALOGBAR);
	Objects->SetWindowText("Rays Objects");
	Objects->EnableDocking(CBRS_ALIGN_ANY);
	Output->Create(pMainFrame,IDD_DIALOGBAR,CBRS_ALIGN_LEFT|CBRS_FLOATING|CBRS_BORDER_ANY|CBRS_GRIPPER|CBRS_BORDER_3D,AFX_IDW_DIALOGBAR);
	Output->SetWindowText("Rays Output");
	Output->EnableDocking(CBRS_ALIGN_ANY);
	
	pMainFrame->DockControlBar(Output,AFX_IDW_DOCKBAR_LEFT,rect);
	rect.top+=300;rect.bottom+=300;
	pMainFrame->DockControlBar(Objects,AFX_IDW_DOCKBAR_LEFT,rect);
	
	
	// The main window has been initialized, so show and update it.
	pMainFrame->ShowWindow(m_nCmdShow);
	//pMainFrame->ShowWindow(SW_SHOWMAXIMIZED);
	pMainFrame->UpdateWindow();

	// display first messages
	CListBox *output = (CListBox*)Output->GetDlgItem(IDC_OUTPUT);
	output->InitStorage(100,40);
	output->SetHorizontalExtent(300);
	output->InsertString(0,"Rays Client ready");
	output->InsertString(0,"Loading standard rays objects");
	
	// display tool objects
	BuildToolTree();

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
		// No message handlers
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
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CRaysClientApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CRaysClientApp message handlers

bool CRaysClientApp::ConnectToServer(CString name,int port)
{
	Cout("Trying server connection...");
	Client.DisconnectServer();
	CString text;
	text.Format("port:%d",port);
	if (Client.ConnectToServer(name,port))
	{
		Cout("Connected to server:" + name + text);
		return true;
	}
	else
	{
		Cout("Server not found:" + name + text);
		return false;
	}
}

int CRaysClientApp::ExitInstance() 
{
	Objects->DestroyWindow();
	Output->DestroyWindow();
	Client.DisconnectServer();	
	return CWinApp::ExitInstance();
}

void CRaysClientApp::OnFileNew() 
{
	CWinApp::OnFileNew();
}

void CRaysClientApp::BuildToolTree()
{
	CTreeCtrl *tree = (CTreeCtrl*)Objects->GetDlgItem(IDC_TREE);
	tree->SetImageList(ObjectsData,TVSIL_NORMAL);
	HTREEITEM std = tree->InsertItem("Std Rays Objects",0,0);
	tree->SetItemData(std,IDR_RAYSTYPE);
	HTREEITEM obj = tree->InsertItem("Cameras",1,1,std);
	tree->SetItemData(obj,IDI_CAMERA);
	obj = tree->InsertItem("Aligned on axes",6,6,obj);
	tree->SetItemData(obj,IDI_CAMAXES);
	obj = tree->InsertItem("Surfaces",2,2,std);
	tree->SetItemData(obj,IDI_SURFACE);
	obj = tree->InsertItem("Pluggins",3,3,std);
	tree->SetItemData(obj,IDI_PLUGIN);
	obj = tree->InsertItem("Externs",4,4,std);
	tree->SetItemData(obj,IDI_EXTERN);
}