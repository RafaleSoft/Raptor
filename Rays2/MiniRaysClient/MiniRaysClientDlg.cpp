// MiniRaysClientDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MiniRaysClient.h"
#include "MiniRaysClientDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define MAX_OUTPUT_STR	200

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
// CMiniRaysClientDlg dialog

CMiniRaysClientDlg::CMiniRaysClientDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMiniRaysClientDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMiniRaysClientDlg)
	m_Port = PORTBASE;
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDI_RAYSAPP);
	m_IP = "127.0.0.1";
}

void CMiniRaysClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMiniRaysClientDlg)
	DDX_Control(pDX, IDC_OUTPUT, m_Output);
	DDX_Control(pDX, IDC_IPADDRESS, m_IPAddress);
	DDX_Text(pDX, IDC_PORT, m_Port);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CMiniRaysClientDlg, CDialog)
	//{{AFX_MSG_MAP(CMiniRaysClientDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_CONNECT, OnConnect)
	ON_BN_CLICKED(IDC_DISCONNECT, OnDisconnect)
	ON_BN_CLICKED(IDC_CHOOSESCENE, OnChooseScene)
	ON_BN_CLICKED(IDC_RENDER, OnRender)
	ON_BN_CLICKED(IDC_STATUS, OnStatus)
	ON_BN_CLICKED(IDC_INSTALL, OnInstall)
	ON_BN_CLICKED(IDC_OPTIONS, OnOptions)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMiniRaysClientDlg message handlers

BOOL CMiniRaysClientDlg::OnInitDialog()
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
	
	m_Output.SetHorizontalExtent(500);
	//	Initialise default address to localhost

	//	address is reversed to be used
	//	with inet_ntoa()
	DWORD buf = inet_addr(LPCTSTR(m_IP));
	{
		__asm mov eax,buf
		__asm bswap eax
		__asm mov buf,eax
	}
	m_IPAddress.SetAddress( buf );
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CMiniRaysClientDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CMiniRaysClientDlg::OnPaint() 
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
HCURSOR CMiniRaysClientDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CMiniRaysClientDlg::Output(CString msg)
{
	m_Output.AddString( LPCTSTR(msg) );

	int nb = m_Output.GetCount();
	while (nb > MAX_OUTPUT_STR)
	{
		m_Output.DeleteString( 0 );
		nb = m_Output.GetCount();
	}

	m_Output.SetCurSel(nb-1);
}

void CMiniRaysClientDlg::OnConnect() 
{
	UpdateData(TRUE);

	BYTE nField0;
	BYTE nField1; 
	BYTE nField2;
	BYTE nField3;

	m_IPAddress.GetAddress( nField0, nField1, nField2, nField3 );

	CString IP;

	IP.Format("%d.%d.%d.%d",nField0,nField1,nField2,nField3);

	if (miniRaysClient.Connect(IP,m_Port))
	{
		GetDlgItem( IDC_CONNECT )->EnableWindow(FALSE);
		GetDlgItem( IDC_DISCONNECT )->EnableWindow(TRUE);
	}
}

void CMiniRaysClientDlg::OnDisconnect() 
{
	if (!miniRaysClient.Disconnect())
	{
		GetDlgItem( IDC_CONNECT )->EnableWindow(TRUE);
		GetDlgItem( IDC_DISCONNECT )->EnableWindow(FALSE);
	}
}

void CMiniRaysClientDlg::OnChooseScene() 
{
	CFileDialog *f;
	DWORD flags = OFN_EXPLORER | OFN_HIDEREADONLY | OFN_LONGNAMES | OFN_PATHMUSTEXIST;
	
	f = new CFileDialog(TRUE,"*.ray",NULL, flags, "Rays2 Scenes Files (*.ray)|*.ray|Text Files (*.txt)|*.txt|All Files (*.*)|*.*||",this );

	if (IDOK == f->DoModal())
	{
		CString path = f->GetPathName();
		CString file = f->GetFileName();

		int pos = path.ReverseFind('\\');
		miniRaysClient.scene_location = path.Left(pos+1);
		miniRaysClient.scene = file;
		SetDlgItemText(IDC_SCENE,file);
	}

	delete f;		 
}

void CMiniRaysClientDlg::OnRender() 
{
	miniRaysClient.Render();	
}

void CMiniRaysClientDlg::OnStatus() 
{
	miniRaysClient.Status();
}

void CMiniRaysClientDlg::OnInstall() 
{
	miniRaysClient.upload_location = "";
	miniRaysClient.upload = "";
	miniRaysClient.correct_texture_upload = false;
	miniRaysClient.correct_module_upload = false;
	miniRaysClient.Upload();
}

void CMiniRaysClientDlg::OnOptions() 
{
	
}
