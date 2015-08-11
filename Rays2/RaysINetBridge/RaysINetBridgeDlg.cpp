// RaysINetBridgeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "RaysINetBridge.h"
#include "RaysINetBridgeDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRaysINetBridgeDlg dialog

CRaysINetBridgeDlg::CRaysINetBridgeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CRaysINetBridgeDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRaysINetBridgeDlg)
	m_lPort1 = 0;
	m_lPort2 = 0;
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_pInetSocket = NULL;
}

void CRaysINetBridgeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRaysINetBridgeDlg)
	DDX_Control(pDX, IDC_IPADDRESS2, m_ipAddress2Ctrl);
	DDX_Control(pDX, IDC_IPADDRESS1, m_ipAddress1Ctrl);
	DDX_Text(pDX, IDC_PORT1, m_lPort1);
	DDV_MinMaxLong(pDX, m_lPort1, 2048, 32767);
	DDX_Text(pDX, IDC_PORT2, m_lPort2);
	DDV_MinMaxLong(pDX, m_lPort2, 2048, 32767);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CRaysINetBridgeDlg, CDialog)
	//{{AFX_MSG_MAP(CRaysINetBridgeDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, OnConnect)
	ON_BN_CLICKED(IDC_SEND, OnSend)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRaysINetBridgeDlg message handlers

BOOL CRaysINetBridgeDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CRaysINetBridgeDlg::OnPaint() 
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
HCURSOR CRaysINetBridgeDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CRaysINetBridgeDlg::OnConnect() 
{
	UpdateData(TRUE);

	CString IP;
	BYTE nField0;
	BYTE nField1;
	BYTE nField2;
	BYTE nField3;

	m_ipAddress2Ctrl.GetAddress( nField0, nField1, nField2, nField3 );

	IP.Format("%d.%d.%d.%d",nField0,nField1,nField2,nField3);

	m_pInetSocket = new CInetSocket(this);

	if (m_pInetSocket->Create(m_lPort2,SOCK_DGRAM,LPCTSTR(IP)))
	{
		Output(CString("Sockect locale cree: mode SOCK_DGRAM"));
	}
	else
	{
		Output(CString("Failed to create local SOCK_DGRAM Socket !"));
	}

	if (m_pInetSocket->Bind(m_lPort2,LPCTSTR(IP)))
	{
		CString msg = "Sockect locale bindee." + IP;
		Output(msg);
	}
	else
	{
		Output(CString("Failed to bind local SOCK_DGRAM Socket !"));
	}
	
	m_ipAddress1Ctrl.GetAddress( nField0, nField1, nField2, nField3 );

	IP.Format("%d.%d.%d.%d",nField0,nField1,nField2,nField3);

	if (m_pInetSocket->Connect( LPCTSTR(IP),m_lPort1 ))
	{
		Output(CString("Connected : awaiting data"));
	}
	else
	{
		Output(CString("Failed to establish connection to remote Bridge !"));
	}
}


void CRaysINetBridgeDlg::Output(CString &msg)
{
	CListBox *lb = (CListBox*)(GetDlgItem(IDC_OUTPUT));

	if (lb != NULL)
		lb->AddString(LPCTSTR(msg));
}

void CRaysINetBridgeDlg::OnSend() 
{
	CWnd *input = GetDlgItem(IDC_INPUT);
	
	CString txt;

	input->GetWindowText(txt);
	m_pInetSocket->Send(LPCTSTR(txt),txt.GetLength());
}
