// MiniRaysClientStatus.cpp : implementation file
//

#include "stdafx.h"
#include "MiniRaysClient.h"
#include "MiniRaysClientStatus.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// MiniRaysClientStatus dialog


MiniRaysClientStatus::MiniRaysClientStatus(CWnd* pParent /*=NULL*/)
	: CDialog(MiniRaysClientStatus::IDD, pParent)
{
	//{{AFX_DATA_INIT(MiniRaysClientStatus)
	m_jobID = _T("");
	m_startTime = _T("");
	m_endTime = _T("");
	m_jobDone = _T("");
	//}}AFX_DATA_INIT
}


void MiniRaysClientStatus::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(MiniRaysClientStatus)
	DDX_Control(pDX, IDC_LISTPLUGINS, m_plugins);
	DDX_Control(pDX, IDC_LISTTEXTURES, m_textures);
	DDX_Control(pDX, IDC_JOBSDONE, m_jobsAvailable);
	DDX_Text(pDX, IDC_JOBID, m_jobID);
	DDX_Text(pDX, IDC_STARTTIME, m_startTime);
	DDX_Text(pDX, IDC_ENDTIME, m_endTime);
	DDX_Text(pDX, IDC_JOBDONE, m_jobDone);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(MiniRaysClientStatus, CDialog)
	//{{AFX_MSG_MAP(MiniRaysClientStatus)
	ON_BN_CLICKED(IDC_GETJOB, OnGetjob)
	ON_BN_CLICKED(IDC_DELJOB, OnDeljob)
	ON_BN_CLICKED(IDC_REFRESH, OnRefresh)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// MiniRaysClientStatus message handlers

void MiniRaysClientStatus::OnGetjob() 
{
	CString jobStr;
	m_jobsAvailable.GetWindowText(jobStr);

	miniRaysClient.Getjob(atoi(LPCTSTR(jobStr)));
}

void MiniRaysClientStatus::OnDeljob() 
{
	CString jobStr;
	m_jobsAvailable.GetWindowText(jobStr);

	miniRaysClient.Deljob(atoi(LPCTSTR(jobStr)));
}

void MiniRaysClientStatus::OnRefresh() 
{
	miniRaysClient.QueryStatus();
}

BOOL MiniRaysClientStatus::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	miniRaysClient.QueryStatus();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void MiniRaysClientStatus::SetCurrentJobInfo(CString &id, CString &st, CString &et, CString p)
{
	m_jobID = id;
	m_startTime = st;
	m_endTime = et;
	m_jobDone = p;

	UpdateData(FALSE);
}

void MiniRaysClientStatus::SetAvailableJobs(unsigned int *jobs)
{
	m_jobsAvailable.ResetContent();

	if (jobs != NULL)
	{
		unsigned int nb = jobs[0];

		CString id;

		for (unsigned int i=nb ; i>0 ; i--)
		{
			id.Format("%d",jobs[i]);
			m_jobsAvailable.AddString( id );
		}

		m_jobsAvailable.SetCurSel(0);
	}
}

void MiniRaysClientStatus::SetAvailableTextures(CStringArray &strs)
{
	m_textures.ResetContent();

	for (int i=0;i<strs.GetSize();i++)
	{
		m_textures.AddString(strs[i]);
	}
}

void MiniRaysClientStatus::SetAvailablePlugins(CStringArray &strs)
{
	m_plugins.ResetContent();

	for (int i=0;i<strs.GetSize();i++)
	{
		m_plugins.AddString(strs[i]);
	}
}
