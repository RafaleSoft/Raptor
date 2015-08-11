// WorkUnitDlg.cpp : implementation file
//

#include "stdafx.h"
#include "WorkUnit.h"
#include "WorkUnitDlg.h"
#include "Raytracer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

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
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWorkUnitDlg dialog

CWorkUnitDlg::CWorkUnitDlg(CString address,unsigned int port,int ID,CWnd* pParent /*=NULL*/)
	: CDialog(CWorkUnitDlg::IDD, pParent),sname(address),sport(port),sID(ID)
{
	//{{AFX_DATA_INIT(CWorkUnitDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CWorkUnitDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWorkUnitDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CWorkUnitDlg, CDialog)
	//{{AFX_MSG_MAP(CWorkUnitDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWorkUnitDlg message handlers

BOOL CWorkUnitDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	BOOL res = ModifyStyle( WS_MAXIMIZEBOX, 0);

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	SetDlgItemText(IDC_ADDRESS,"Address: "+sname);
	CString port;
	port.Format("Port: %d",sport);
	SetDlgItemText(IDC_PORT,port);
		
	pCtrl = (CProgressCtrl*)this->GetDlgItem(IDC_WORKPROGRESS);
	lb = (CListBox*)this->GetDlgItem(IDC_LIST1);
	port.Format("My ID is : %d",sID);

	lb->AddString(port);

	CPU_INFO info = getCPUINFO();

	if (info.MMX)
		((CWnd*)this->GetDlgItem(IDC_MMX))->EnableWindow(TRUE);
	if (info.SSE)
		((CWnd*)this->GetDlgItem(IDC_SSE))->EnableWindow(TRUE);
	if (info.CMOV)
		((CWnd*)this->GetDlgItem(IDC_CMOV))->EnableWindow(TRUE);

	data->use_mmx = info.MMX;
	data->use_sse = info.SSE;
	data->use_cmov = info.CMOV;

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CWorkUnitDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CWorkUnitDlg::OnPaint() 
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
HCURSOR CWorkUnitDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CWorkUnitDlg::SetProgress(int progress)
{
	pCtrl->SetPos(progress);
}

void CWorkUnitDlg::OnTimer(UINT nIDEvent) 
{
	CDialog::OnTimer(nIDEvent);
	((CWorkUnitApp*)AfxGetApp())->Notify();		
}

void CWorkUnitDlg::OnOK() 
{
	// CDialog::OnOK();
}
