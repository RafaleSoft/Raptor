// RaptorErrors.cpp : implementation file
//
#include "StdAfx.h"

#include "Resource.h"


#if !defined(AFX_PERSISTENCE_H__5561BA28_831B_11D3_9142_EEB51CEBBDB0__INCLUDED_)
	#include "GLHierarchy/Persistence.h"
#endif

#if !defined(AFX_RAPTORERRORS_H__4B829705_7FC0_44DD_83A2_CAB01172F485__INCLUDED_)
	#include "RaptorErrors.h"
#endif

#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
	#include "System/Raptor.h"
#endif
#if !defined(AFX_RAPTORERRORMANAGER_H__FA5A36CD_56BC_4AA1_A5F4_451734AD395E__INCLUDED_)
    #include "System/RaptorErrorManager.h"
#endif


RAPTOR_NAMESPACE

/////////////////////////////////////////////////////////////////////////////
// CRaptorErrors dialog


CRaptorErrors::CRaptorErrors(CWnd* pParent /*=NULL*/)
	: CDialog(CRaptorErrors::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRaptorErrors)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CRaptorErrors::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRaptorErrors)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRaptorErrors, CDialog)
	//{{AFX_MSG_MAP(CRaptorErrors)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRaptorErrors message handlers

BOOL CRaptorErrors::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CString		error;
	CListBox	*l = (CListBox*)this->GetDlgItem(IDC_LIST3);

	l->SetHorizontalExtent( 500 );

    CRaptorErrorManager::GL_RAPTOR_ERROR err;

	int nbErr = IRaptor::GetErrorManager()->getLastRaptorError();

	for (int i=0; i<=nbErr; i++)
	{
		IRaptor::GetErrorManager()->getRaptorError(i,err);

		switch(err.type)
		{
			case CRaptorErrorManager::RAPTOR_NO_ERROR:
				error = "None     [";
				break;
			case CRaptorErrorManager::RAPTOR_WARNING:
				error = "Warning  [";
				break;
			case CRaptorErrorManager::RAPTOR_ERROR:
				error = "Error    [";
				break;
			case CRaptorErrorManager::RAPTOR_GL_ERROR:
				error = "GL Error [";
				break;
			case CRaptorErrorManager::RAPTOR_FATAL:
				error = "FATAL !  [";
				break;
		}

		error += err.className.data();
		error += "] ";

		error += err.error.data();

		l->AddString(LPCTSTR(error));
		
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
