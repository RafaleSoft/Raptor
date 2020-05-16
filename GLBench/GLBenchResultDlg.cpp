/***************************************************************************/
/*                                                                         */
/*  GLBenchResultDlg.cpp                                                   */
/*                                                                         */
/*    Raptor OpenGL & Vulkan realtime 3D Engine SDK.                       */
/*                                                                         */
/*  Copyright 1998-2019 by                                                 */
/*  Fabrice FERRAND.                                                       */
/*                                                                         */
/*  This file is part of the Raptor project, and may only be used,         */
/*  modified, and distributed under the terms of the Raptor project        */
/*  license, LICENSE.  By continuing to use, modify, or distribute         */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/


#include "stdafx.h"
#include "GLBench.h"
#include "GLBenchResultDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGLBenchResultDlg dialog


CGLBenchResultDlg::CGLBenchResultDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGLBenchResultDlg::IDD, pParent),
	m_pResults(NULL)
{
	//{{AFX_DATA_INIT(CGLBenchResultDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CGLBenchResultDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGLBenchResultDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGLBenchResultDlg, CDialog)
	//{{AFX_MSG_MAP(CGLBenchResultDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGLBenchResultDlg message handlers

void CGLBenchResultDlg::SetData(lp_result_bench_t results)
{
	m_pResults = results;
}

BOOL CGLBenchResultDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CListBox *lb = (CListBox *)(GetDlgItem(IDC_LIST));

	lb->SetHorizontalExtent( 500 );

	lb->AddString(CA2T(m_pResults->name));
	lb->AddString(CA2T(m_pResults->author));
	lb->AddString(CA2T(m_pResults->description));
	lb->AddString(TEXT(""));

	CString format;
	format.Format(TEXT("Score: %u"),m_pResults->score);
	lb->AddString(format);
	format.Format(TEXT("Avg. frame rate: %.2f fps"),m_pResults->rate);
	lb->AddString(format);
	lb->AddString(TEXT(""));
	lb->AddString(TEXT("Results:"));
	lb->AddString(TEXT(""));

	for (unsigned int i=0;i<m_pResults->nbItems;i++)
	{
		lb->AddString(CA2T(m_pResults->result_items[i].description));
		format.Format(TEXT("Score: %u"),m_pResults->result_items[i].score);
		lb->AddString(format);
		format.Format(TEXT("Frame rate: %.2f fps"),m_pResults->result_items[i].fps_rate);
		lb->AddString(format);
		format.Format(TEXT("Fragment rate: %.2f Mpixels/sec"), m_pResults->result_items[i].fragment_rate);
		lb->AddString(format);
		if (m_pResults->result_items[i].driver_overhead > 0)
			format.Format(TEXT("Driver overhead: %.2f %%"), m_pResults->result_items[i].driver_overhead);
		else
			format.Format(TEXT("Driver overlap: %.2f %%"), -m_pResults->result_items[i].driver_overhead);
		lb->AddString(format);
		lb->AddString(TEXT(""));
	}

	return TRUE;
}
