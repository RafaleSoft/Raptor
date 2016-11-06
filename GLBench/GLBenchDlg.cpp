// GLBenchDlg.cpp : implementation file
//

#include "StdAfx.h"
#include "resource.h"
#include "Interface.h"
#include "GLBenchDlg.h"
#include "GLBenchResultDlg.h"

#include <afx.h>				// CFileFind
#include "MFCExtension/Dialog/RaptorStatus.h"	// Sys info

RAPTOR_NAMESPACE

static BENCH fnc_bench = NULL;
static INITMODULE fnc_init = NULL;
static RESULT fnc_result = NULL;
static HINSTANCE module = NULL;


/////////////////////////////////////////////////////////////////////////////
// CGLBenchDlg dialog

CGLBenchDlg::CGLBenchDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGLBenchDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CGLBenchDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CGLBenchDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGLBenchDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CGLBenchDlg, CDialog)
	//{{AFX_MSG_MAP(CGLBenchDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_LBN_SELCHANGE(IDC_LIST, OnSelchangeList)
	ON_BN_CLICKED(IDC_BENCH, OnBench)
	ON_BN_CLICKED(IDC_SYSINFO, OnSysinfo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGLBenchDlg message handlers

BOOL CGLBenchDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	LoadModules(0);
	TCHAR buffer[MAX_PATH];
	GetEnvironmentVariable(TEXT("RAPTOR_ROOT"),buffer,MAX_PATH);
	StrCat(buffer, TEXT("\\Redist\\bin"));
	LoadModules(CT2A(buffer));

	CButton *bt = (CButton *)(GetDlgItem(IDC_RADIO1));
	bt->SetCheck(1);
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CGLBenchDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CGLBenchDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}


void CGLBenchDlg::LoadModules(const char* path)
{
	CListBox *lb = (CListBox *)(GetDlgItem(IDC_LIST));
	CFileFind findFile;

	CString filepath = "*.dll";
	if (NULL != path)
		filepath = CString(path) + "\\*.dll";
	BOOL res = findFile.FindFile( filepath );
	
	if (res != 0)
	{
		do
		{
			res = findFile.FindNextFile();
			CString moduleName = findFile.GetFileName();

			module = AfxLoadLibrary(moduleName);
			if (module != NULL)
			{
				INITMODULE fnc_init = (INITMODULE)(GetProcAddress(module,"InitModule"));
				BENCH fnc_bench = (BENCH)(GetProcAddress(module,"Bench"));
				RESULT fnc_result = (RESULT)(GetProcAddress(module,"GetBenchResult"));

				if ((fnc_result != NULL) &&
					(fnc_init != NULL) &&
					(fnc_bench != NULL))
				{
					lb->AddString(moduleName.Left(moduleName.GetLength()-4));
				}
				AfxFreeLibrary(module);
			}
		}
		while (0 != res);
	}
}


UINT Bench(LPVOID pParam);

void CGLBenchDlg::OnSelchangeList() 
{
	CListBox *lb = (CListBox *)(GetDlgItem(IDC_LIST));

	int moduleIndex = lb->GetCurSel();

	CString moduleName;

	lb->GetText( moduleIndex, moduleName );

	if (module != NULL)
	{
		fnc_bench = NULL;
		fnc_init = NULL;
		fnc_result = NULL;

		if (FALSE == AfxFreeLibrary(module))
			AfxMessageBox(TEXT("Failed to release module"));

		module = NULL;
	}

	module = AfxLoadLibrary(moduleName+".dll");

	if (module == NULL)
	{
		AfxMessageBox(TEXT("Invalid bench module"));
	}
	else
	{
		GETMODULEINFO fnc_info1 = (GETMODULEINFO)(GetProcAddress(module,"GetModuleName"));
		GETMODULEINFO fnc_info2 = (GETMODULEINFO)(GetProcAddress(module,"GetModuleAuthor"));
		GETMODULEINFO fnc_info3 = (GETMODULEINFO)(GetProcAddress(module,"GetModuleDescription"));

		fnc_init = (INITMODULE)(GetProcAddress(module,"InitModule"));
		fnc_bench = (BENCH)(GetProcAddress(module,"Bench"));
		fnc_result = (RESULT)(GetProcAddress(module,"GetBenchResult"));

		if ((fnc_info1 != NULL) && 
			(fnc_info2 != NULL) &&
			(fnc_info3 != NULL) &&
			(fnc_result != NULL) &&
			(fnc_init != NULL) &&
			(fnc_bench != NULL))
		{
			CWnd *wnd = GetDlgItem(IDC_AUTHOR);
			wnd->SetWindowText(CA2T(fnc_info2()));
			wnd = GetDlgItem(IDC_NAME);
			wnd->SetWindowText(CA2T(fnc_info1()));
			wnd = GetDlgItem(IDC_DESCRIPTION);
			wnd->SetWindowText(CA2T(fnc_info3()));
		}
		else
		{
			AfxMessageBox(TEXT("Invalid bench module"));
		}
	}
}

void CGLBenchDlg::OnBench() 
{
	AfxBeginThread( Bench,this,THREAD_PRIORITY_ABOVE_NORMAL,0,0,NULL );
}

void CGLBenchDlg::OnSysinfo() 
{
	CRaptorStatus status;
	status.InitDlg();
}


UINT Bench(LPVOID pParam)
{
	CGLBenchDlg* dlg = (CGLBenchDlg*) pParam;

	if ((fnc_bench != NULL) && (module != NULL) && (fnc_init != NULL))
	{
		RECT rect;
		CFrameWnd benchWindow;

		int screenWidth = GetSystemMetrics(SM_CXSCREEN);
		int screenHeight = GetSystemMetrics(SM_CYSCREEN);
		int borderWidth = GetSystemMetrics(SM_CXEDGE);
		int borderHeight = GetSystemMetrics(SM_CYEDGE);

		int width = 640;
		int height = 480;

		CButton *bt = (CButton *)(dlg->GetDlgItem(IDC_RADIO2));
		if (bt->GetCheck() == 1)
		{	width = 800; height = 600; 	}
		else
		{
			bt = (CButton *)(dlg->GetDlgItem(IDC_RADIO3));
			if (bt->GetCheck() == 1)
			{	width = 1024; height = 768;	}
			else
			{
				bt = (CButton *)(dlg->GetDlgItem(IDC_RADIO4));
				if (bt->GetCheck() == 1)
				{	width = 1280; height = 1024;	}
				else
				{
					bt = (CButton *)(dlg->GetDlgItem(IDC_RADIO5));
					if (bt->GetCheck() == 1)
					{	width = screenWidth; height = screenHeight;	}
				}
			}
		}

		rect.left = (screenWidth - width) / 2 - borderWidth;
		rect.right = rect.left + width + borderWidth + borderWidth;
		rect.top = (screenHeight - height) / 2 - borderHeight;
		rect.bottom = rect.top + height + borderHeight + borderHeight;

		benchWindow.Create(NULL,TEXT("GL Bench"),WS_VISIBLE|WS_POPUP|WS_OVERLAPPED,rect,dlg,NULL,WS_EX_TOPMOST,NULL);

		CDC *dc=benchWindow.GetDC();
		dc->SetBkMode(TRANSPARENT);								//	Tansparent background for cool text display
		CFont font;
		font.CreateFont(10,6,0,0,FW_NORMAL,0,0,0,0,OUT_TT_PRECIS,CLIP_TT_ALWAYS,PROOF_QUALITY,VARIABLE_PITCH,TEXT("Small Fonts"));
		dc->SelectObject(font);	

		ShowCursor(FALSE);

		dc->TextOut(50,40,"Loading Raptor...");
		benchWindow.SendMessage(WM_PAINT);
		fnc_init();

		dc->TextOut(50,60,"Running benchmark...");
		benchWindow.SendMessage(WM_PAINT);

		Sleep(1000);
		fnc_bench(&benchWindow);

		ShowCursor(TRUE);
	}

	if (fnc_result != NULL)
	{
		lp_result_bench_t results = fnc_result();

		CGLBenchResultDlg resultsDlg;
		resultsDlg.SetData(results);
		resultsDlg.DoModal();
	}

	return 0;
}