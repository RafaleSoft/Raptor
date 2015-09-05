// DialogWUPath.cpp : implementation file
//

#include "stdafx.h"
#include "RaysServer.h"
#include "DialogWUPath.h"
#include <Afxdlgs.h>	// Open file dialog

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialogWUPath dialog


CDialogWUPath::CDialogWUPath(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogWUPath::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogWUPath)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDialogWUPath::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogWUPath)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogWUPath, CDialog)
	//{{AFX_MSG_MAP(CDialogWUPath)
	ON_BN_CLICKED(IDC_BUTTON1, OnBrowse)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogWUPath message handlers

void CDialogWUPath::OnBrowse() 
{
	CFileDialog *f;
	DWORD flags = OFN_EXPLORER | OFN_HIDEREADONLY | OFN_LONGNAMES | OFN_PATHMUSTEXIST;
	
	f = new CFileDialog(TRUE,"*.exe",NULL, flags, "Work Units Files (*.exe)|*.exe|All Files (*.*)|*.*||",NULL );

	if (IDOK == f->DoModal())
	{
		CString text = f->GetPathName();

		m_cstrWUPath = text;
		SetDlgItemText(IDC_WORKUNIT,text);
	}

	delete f;	
}

void CDialogWUPath::OnOK() 
{
	GetDlgItemText(IDC_WORKUNIT,m_cstrWUPath);

	CDialog::OnOK();
}
