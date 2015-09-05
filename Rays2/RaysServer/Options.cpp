// Options.cpp : implementation file
//

#include "stdafx.h"
#include "RaysServer.h"
#include "Options.h"
#include "DeamonIP.h"
#include "DialogWUPath.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CRaysServerApp Server;

/////////////////////////////////////////////////////////////////////////////
// COptions dialog
COptions::COptions(CWnd* pParent /*=NULL*/)
	: CDialog(COptions::IDD, pParent)
{
	//{{AFX_DATA_INIT(COptions)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void COptions::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COptions)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COptions, CDialog)
	ON_COMMAND_RANGE(ID_Y_DELETE,ID_Y_DEAMONIP, OnOptionsPopup )
	//{{AFX_MSG_MAP(COptions)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_BUTTON1, OnButton1)
	ON_LBN_DBLCLK(IDC_LIST1, OnDblclkList1)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COptions message handlers

BOOL COptions::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CSliderCtrl *l = (CSliderCtrl*)(this->GetDlgItem(IDC_SLIDER1));
	CString text;

	l->SetRange(1,8,TRUE);
	l->SetPos(Server.m_nbWUperJOB);

	l = (CSliderCtrl*)(this->GetDlgItem(IDC_SLIDER2));
	l->SetRange(1,4,TRUE);

	CWnd *w = this->GetDlgItem(IDC_WUCPU);
	text.Format("Work Unit / Job : %d",Server.m_nbWUperJOB);
	w->SetWindowText(text);
	w = this->GetDlgItem(IDC_PRIORITY);
	switch(Server.m_wUnitPriority)
	{
	case NORMAL_PRIORITY_CLASS:
		text="Work Units Priority : Normal";
		l->SetPos(1);
		break;
	case IDLE_PRIORITY_CLASS:
		text="Work Units Priority : Idle";
		l->SetPos(2);
		break;
	case HIGH_PRIORITY_CLASS:
		text="Work Units Priority : High";
		l->SetPos(3);
		break;
	case REALTIME_PRIORITY_CLASS:
		text="Work Units Priority : Real Time";
		l->SetPos(4);
		break;
	}
	w->SetWindowText(text);

	l = (CSliderCtrl*)(this->GetDlgItem(IDC_SLIDER3));
	l->SetRange(1,11,TRUE);
	w = this->GetDlgItem(IDC_POLLINGDELAY);
	text = "Work Unit Deamon polling delay : ";
	switch(Server.m_deamonDelay)
	{
		case 10:text+="10 s";l->SetPos(1);break;
		case 20:text+="20 s";l->SetPos(2);break;
		case 30:text+="30 s";l->SetPos(3);break;
		case 60:text+="1 mn";l->SetPos(4);break;
		case 120:text+="2 mn";l->SetPos(5);break;
		case 180:text+="3 mn";l->SetPos(6);break;
		case 300:text+="5 mn";l->SetPos(7);break;
		case 600:text+="10 mn";l->SetPos(8);break;
		case 1200:text+="20 mn";l->SetPos(9);break;
		case 1800:text+="30 mn";l->SetPos(10);break;
		case 3600:text+="1 hour";l->SetPos(11);break;
	}
	w->SetWindowText(text);

	DisplayWUList();

	return TRUE;  
}

void COptions::DisplayWUList(void)
{
	CString text;

	CListBox *list = (CListBox*)(this->GetDlgItem(IDC_LIST1));
	list->ResetContent();

	const CDeamonManager * const pDeamon = Server.GetDeamonManager();

	for (int i=0;i<pDeamon->GetNbRegWorkUnits();i++)
	{
		const CDeamonManager::WUREGSTRUCT * const lpRegWU = pDeamon->GetRegWorkUnit(i);

		text.Format("CPU:%d/%d - %s - ",lpRegWU->available,lpRegWU->nbProcs,LPCTSTR(lpRegWU->deamonIP));
		text+=lpRegWU->source;
		list->AddString(text);
	}

	list->SetHorizontalExtent(1000);
}

void COptions::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	CString text;
	UINT pos = nPos;

	if (pos == 0) pos = ((CSliderCtrl*)pScrollBar)->GetPos();

	switch(nSBCode)
	{
	case TB_THUMBTRACK:
	case TB_PAGEUP:
	case TB_PAGEDOWN:
	case TB_LINEUP:
	case TB_LINEDOWN:
	case TB_BOTTOM:
	case TB_TOP:
		if ((CSliderCtrl*)pScrollBar == (CSliderCtrl*)(this->GetDlgItem(IDC_SLIDER1)))
		{
			CWnd *w = this->GetDlgItem(IDC_WUCPU);	
			text.Format("Work Unit / Job : %d",pos);
			w->SetWindowText(text);
		}
		else if ((CSliderCtrl*)pScrollBar == (CSliderCtrl*)(this->GetDlgItem(IDC_SLIDER2)))
		{
			CWnd *w = this->GetDlgItem(IDC_PRIORITY);		
			switch(pos)
			{
			case 1:	//NORMAL_PRIORITY_CLASS:
				text="Work Units Priority : Normal";
				break;
			case 2: //IDLE_PRIORITY_CLASS:
				text="Work Units Priority : Idle";
				break;
			case 3: //HIGH_PRIORITY_CLASS:
				text="Work Units Priority : High";
				break;
			case 4: //REALTIME_PRIORITY_CLASS:
				text="Work Units Priority : Real Time";
				break;
			}
			w->SetWindowText(text);
		}
		else if ((CSliderCtrl*)pScrollBar == (CSliderCtrl*)(this->GetDlgItem(IDC_SLIDER3)))
		{
			CWnd *w = this->GetDlgItem(IDC_POLLINGDELAY);
			text = "Work Unit Deamon polling delay : ";
			switch(pos)
			{
				case 1:text+="10 s";break;
				case 2:text+="20 s";break;
				case 3:text+="30 s";break;
				case 4:text+="1 mn";break;
				case 5:text+="2 mn";break;
				case 6:text+="3 mn";break;
				case 7:text+="5 mn";break;
				case 8:text+="10 mn";break;
				case 9:text+="20 mn";break;
				case 10:text+="30 mn";break;
				case 11:text+="1 hour";break;
			}
			w->SetWindowText(text);
		}
		break;
	}

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void COptions::OnOK() 
{
	CSliderCtrl *l = (CSliderCtrl*)(this->GetDlgItem(IDC_SLIDER1));	

	Server.m_nbWUperJOB = (unsigned char)(l->GetPos());
	Server.WriteProfileInt("Settings","NbWUperJob",Server.m_nbWUperJOB);

	l = (CSliderCtrl*)(this->GetDlgItem(IDC_SLIDER2));	
	Server.m_wUnitPriority = (int)(0x00000010<<l->GetPos());
	Server.WriteProfileInt("Settings","WUnitPriority",Server.m_wUnitPriority);

	l = (CSliderCtrl*)(this->GetDlgItem(IDC_SLIDER3));
	int pos = l->GetPos();
	switch(pos)
	{
		case 1:Server.m_deamonDelay = 10;break;
		case 2:Server.m_deamonDelay = 20;break;
		case 3:Server.m_deamonDelay = 30;break;
		case 4:Server.m_deamonDelay = 60;break;
		case 5:Server.m_deamonDelay = 120;break;
		case 6:Server.m_deamonDelay = 180;break;
		case 7:Server.m_deamonDelay = 300;break;
		case 8:Server.m_deamonDelay = 600;break;
		case 9:Server.m_deamonDelay = 1200;break;
		case 10:Server.m_deamonDelay = 1800;break;
		case 11:Server.m_deamonDelay = 3600;break;
	}
	Server.WriteProfileInt("Settings","DeamonDelay",Server.m_deamonDelay);

	CDialog::OnOK();
}

void COptions::OnButton1() 
{
	CDialogWUPath	wuPath;
	if (IDOK == wuPath.DoModal())
	{
		CString text;

		text = wuPath.m_cstrWUPath;

		Server.RegisterWorkUnit(text,"127.0.0.1");

		DisplayWUList();
	}
}

void COptions::OnDblclkList1() 
{
	POINT p;
	CMenu m;
	CMenu *popup;

	m.LoadMenu(IDR_OPTIONSPOPUP);
	popup = m.GetSubMenu(0);
	GetCursorPos(&p);

	int n = ((CListBox*)(this->GetDlgItem(IDC_LIST1)))->GetCurSel();

	const CDeamonManager * const pDeamon = Server.GetDeamonManager();
	const CDeamonManager::WUREGSTRUCT * const pWU = pDeamon->GetRegWorkUnit(n);
	
	if (pWU != NULL)
	{
		int nb = pWU->nbProcs;

		popup->CheckMenuItem(ID_Y_DELETE+nb,MF_CHECKED);
		popup->TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON,p.x,p.y,this);
	}
}

void COptions::OnOptionsPopup(UINT nID)
{
	CString text;
	CRaysServerApp *pApp = ((CRaysServerApp*)AfxGetApp());
	CListBox *list = (CListBox*)(this->GetDlgItem(IDC_LIST1));

	if (nID == ID_Y_DELETE)
		pApp->UnregisterWorkUnit(list->GetCurSel());
	else if (nID == ID_Y_DEAMONIP)
	{
		unsigned int nbWU = list->GetCurSel();
		const CDeamonManager::WUREGSTRUCT * const lpRegWU = 
			Server.GetDeamonManager()->GetRegWorkUnit(nbWU);
		CDeamonIP dlg;

		if (lpRegWU->deamonIP == "localhost")
		{
			Server.GetDeamonManager()->SetRegWUDeamon(nbWU,"127.0.0.1");
			dlg.m_address = 0x7F000001;
		}
		else
			dlg.m_address = CSystem::SockNameToAddr(lpRegWU->deamonIP);

		if (IDOK == dlg.DoModal())
		{
			text.Format("%d.%d.%d.%d",dlg.m_b0,dlg.m_b1,dlg.m_b2,dlg.m_b3);
			Server.GetDeamonManager()->SetRegWUDeamon(nbWU,text);
			Server.UpdateWorkUnitDeamon(lpRegWU);
		}
	}
	else
		pApp->SetRegWUNbCPU(list->GetCurSel(),(unsigned char)(nID-ID_Y_DELETE));
	
	DisplayWUList();
}
