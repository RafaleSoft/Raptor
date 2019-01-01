// MiniRaysClientUpload.cpp : implementation file
//

#include "stdafx.h"
#include "MiniRaysClient.h"
#include "MiniRaysClientUpload.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// MiniRaysClientUpload dialog


MiniRaysClientUpload::MiniRaysClientUpload(CWnd* pParent /*=NULL*/)
	: CDialog(MiniRaysClientUpload::IDD, pParent)
{
	//{{AFX_DATA_INIT(MiniRaysClientUpload)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void MiniRaysClientUpload::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(MiniRaysClientUpload)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(MiniRaysClientUpload, CDialog)
	//{{AFX_MSG_MAP(MiniRaysClientUpload)
	ON_BN_CLICKED(IDC_DOUPLOAD, OnUpload)
	ON_BN_CLICKED(IDC_CHOOSESCENE, OnChoosescene)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// MiniRaysClientUpload message handlers

void MiniRaysClientUpload::OnUpload() 
{
	miniRaysClient.QueryUpload();
}

void MiniRaysClientUpload::OnChoosescene() 
{
	CFileDialog *f;
	DWORD flags = OFN_EXPLORER | OFN_HIDEREADONLY | OFN_LONGNAMES | OFN_PATHMUSTEXIST;
	
	f = new CFileDialog(TRUE,NULL,NULL, flags, "Rays2 Texture Files (*.tga;*.jpg)|*.tga;*.jpg|Rays2 Plug-in Files (*.dll)|*.dll||",this );

	if (IDOK == f->DoModal())
	{
		CString path = f->GetPathName();
		CString file = f->GetFileName();

		int pos = path.ReverseFind('\\');
		miniRaysClient.upload_location = path.Left(pos+1);
		miniRaysClient.upload = file;
		SetDlgItemText(IDC_UPLOAD,file);

		SetDlgItemText(IDC_INFO,miniRaysClient.CheckUpload());
	}

	delete f;
}
