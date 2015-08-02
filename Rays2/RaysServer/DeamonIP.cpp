// DeamonIP.cpp : implementation file
//

#include "stdafx.h"
#include "RaysServer.h"
#include "DeamonIP.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDeamonIP dialog


CDeamonIP::CDeamonIP(CWnd* pParent /*=NULL*/)
	: CDialog(CDeamonIP::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDeamonIP)
	//}}AFX_DATA_INIT
}


void CDeamonIP::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDeamonIP)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDeamonIP, CDialog)
	//{{AFX_MSG_MAP(CDeamonIP)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDeamonIP message handlers

BOOL CDeamonIP::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CIPAddressCtrl *IPCtrl = (CIPAddressCtrl *)(this->GetDlgItem(IDC_IPADDRESS));
	IPCtrl->SetAddress(m_address);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDeamonIP::OnOK() 
{
	CIPAddressCtrl *IPCtrl = (CIPAddressCtrl *)(this->GetDlgItem(IDC_IPADDRESS));
	IPCtrl->GetAddress(m_b0,m_b1,m_b2,m_b3);

	CDialog::OnOK();
}
