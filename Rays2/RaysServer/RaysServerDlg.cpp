// RaysServerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "RaysServer.h"
#include "RaysServerDlg.h"
#include "Options.h"

#include <dos.h>			// about box info
#include <direct.h>			// about box info


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CRaysServerApp Server;

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
	virtual BOOL OnInitDialog();
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
// CRaysServerDlg dialog

CRaysServerDlg::CRaysServerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CRaysServerDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRaysServerDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDI_RAYSAPP);
	redraw = false;
}

void CRaysServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRaysServerDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CRaysServerDlg, CDialog)
	//{{AFX_MSG_MAP(CRaysServerDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOPTIONS, OnOptions)
	ON_WM_TIMER()
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN1, OnDeltaposSpin1)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRaysServerDlg message handlers

BOOL CRaysServerDlg::OnInitDialog()
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
	
	// Extra initializations
	pServer = Server.m_Server;
	SetDlgItemInt(IDC_BASEPORT, pServer->GetPort());
	SetDlgItemInt(IDC_NBJOBS, pServer->GetNumClients());
	SetDlgItemText(IDC_HOSTNAME, pServer->GetHostName());
	SetDlgItemText(IDC_SOCKNAME, pServer->GetSockName());

	black_pen = new CPen(PS_SOLID,1,RGB(0,0,0));
	blue_pen = new CPen(PS_SOLID,1,RGB(0,0,255));
	green_pen = new CPen(PS_SOLID,1,RGB(0,255,0));
	red_pen = new CPen(PS_SOLID,1,RGB(255,0,0));
	blue_brush = new CBrush(RGB(0,0,255));
	red_brush = new CBrush(RGB(255,0,0));
	green_brush = new CBrush(RGB(0,255,0));

	font.CreateFont(10,6,0,0,FW_NORMAL,0,0,0,0,OUT_TT_PRECIS,CLIP_TT_ALWAYS,PROOF_QUALITY,VARIABLE_PITCH,"Small Fonts");

	wJobID = this->GetDlgItem(IDC_JOBID);
	wJobPercent = this->GetDlgItem(IDC_JOBPERCENT);
	wProgress = (CProgressCtrl*)this->GetDlgItem(IDC_PROGRESS);
	wSpin = (CSpinButtonCtrl*)this->GetDlgItem(IDC_SPIN1);

	CListBox* l = (CListBox*)(this->GetDlgItem(IDC_LIST1));
	if (Server.GetDeamonManager()->GetNbRegWorkUnits()>0)
		l->AddString("SRVR msg : Server ready");
	else
		l->AddString("SRVR msg : No Work Units registered !");

	wProgress->SetRange(0,10000);
	wSpin->SetBuddy(wJobID);
	wSpin->SetRange(0,0);
	this->SetTimer(1,2000,NULL);
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CRaysServerDlg::OnSysCommand(UINT nID, LPARAM lParam)
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


void CRaysServerDlg::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	if (IsIconic())
	{

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
		JobDraw(&dc,true);
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CRaysServerDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

BOOL CAboutDlg::OnInitDialog()
{
	CDialog::OnInitDialog();	// CG:  This was added by System Info Component.

	// CG: Following block was added by System Info Component.
	{
		CString strFreeDiskSpace;
		CString strFreeMemory;
		CString strFrequency;
		CString strFmt;

		// Fill available memory
		MEMORYSTATUS MemStat;
		MemStat.dwLength = sizeof(MEMORYSTATUS);
		GlobalMemoryStatus(&MemStat);
		strFmt.LoadString(CG_IDS_PHYSICAL_MEM);
		strFreeMemory.Format(strFmt, MemStat.dwTotalPhys / 1024L);
		SetDlgItemText(IDC_PHYSICAL_MEM, strFreeMemory);

		// Fill disk free information
		struct _diskfree_t diskfree;
		int nDrive = _getdrive(); // use current default drive
		if (_getdiskfree(nDrive, &diskfree) == 0)
		{
			strFmt.LoadString(CG_IDS_DISK_SPACE);
			strFreeDiskSpace.Format(strFmt,
				(DWORD)diskfree.avail_clusters *
				(DWORD)diskfree.sectors_per_cluster *
				(DWORD)diskfree.bytes_per_sector / (DWORD)1024L,
				nDrive-1 + _T('A'));
		}
		else
			strFreeDiskSpace.LoadString(CG_IDS_DISK_SPACE_UNAVAIL);
		SetDlgItemText(IDC_DISK_SPACE, strFreeDiskSpace);

		SetDlgItemInt(IDC_NUM_PROCS, Server.m_nbProcs);
		strFmt.LoadString(IDS_FREQUENCY);
		strFrequency.Format(strFmt,Server.m_baseFrequency.QuadPart);
		SetDlgItemText(IDC_FREQUENCY,strFrequency);
	}

	return TRUE;	// CG:  This was added by System Info Component.
}

void CRaysServerDlg::OnOK() 
{
	if (Server.GetDeamonManager()->GetNbWorkUnits() > 0)
	{
		AfxMessageBox("Unable to stop server : work units running");
		return;
	}
	delete black_pen;
	delete blue_pen;
	delete green_pen;
	delete red_pen;
	delete blue_brush;
	delete red_brush;
	delete green_brush;

	this->KillTimer(1);

	CDialog::OnOK();
}

void CRaysServerDlg::OnOptions() 
{
	COptions options;
	options.DoModal();
}

void CRaysServerDlg::OnTimer(UINT nIDEvent) 
{
	CRect rect;
	rect.left = 100;
	rect.top = 80;
	rect.right = 460;
	rect.bottom = 180;
	this->InvalidateRect(&rect,FALSE);
	
	CDialog::OnTimer(nIDEvent);
}

void CRaysServerDlg::OnDeltaposSpin1(NMHDR* pNMHDR, LRESULT* pResult) 
{
	UNREFERENCED_PARAMETER(pNMHDR);
//	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	redraw = true;
	*pResult = 0;
}

void CRaysServerDlg::SetNbJobs(int n) 
{ 
	SetDlgItemInt(IDC_NBJOBS,n); 
	int njob = GetDlgItemInt(IDC_JOBID);

	if (n>0)
		wSpin->SetRange(0,n-1);
	else
		wSpin->SetRange(0,0);

	if (njob>=n)
		if (n>0)
			SetDlgItemInt(IDC_JOBID,n-1); 
		else
			SetDlgItemInt(IDC_JOBID,0); 

	wJobPercent->SetWindowText("100.00%");
}