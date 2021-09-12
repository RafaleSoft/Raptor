// VideoSettings.cpp : implementation file
//

#include "StdAfx.h"

#include "Resource.h"


#ifndef __CGLTYPES_HPP__
	#include "System/CGLTypes.h"
#endif

#if !defined(AFX_VIDEOSETTINGS_H__CB2E9A06_6EFA_4545_B84D_A5DBCB6799F3__INCLUDED_)
	#include "VideoSettings.h"
#endif

#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
	#include "System/Raptor.h"
#endif
#if !defined(AFX_RAPTORERRORMANAGER_H__FA5A36CD_56BC_4AA1_A5F4_451734AD395E__INCLUDED_)
    #include "System/RaptorErrorManager.h"
#endif


RAPTOR_NAMESPACE_BEGIN

static bool setThread = true;

//	Desktop switcher
class DSwitch
{
public:
	DSwitch(HDESK newDesk,HDESK oldDesk)
	{
		if ((m_oldDesk == NULL) && (oldDesk != NULL) && (newDesk != NULL))
		{
			m_oldDesk = oldDesk;
			BOOL res = SwitchDesktop(newDesk);
			if (res == 0)
				m_oldDesk = NULL;
			else
			{
				res = SetThreadDesktop(newDesk);
				/*if (res == 0)
				{
					SwitchDesktop(m_oldDesk);
					m_oldDesk = NULL;
					setThread = false;
				}*/
			}
		}
	}
	~DSwitch()
	{
		if (m_oldDesk != NULL)
		{
			SwitchDesktop(m_oldDesk);
		}
	}

	static HDESK m_oldDesk;
};

HDESK DSwitch::m_oldDesk = NULL;
static DSwitch *__dswitch = NULL;


class CDSwitchMonitor
{
public:
	CDSwitchMonitor() {};
	~CDSwitchMonitor()
	{
		if (__dswitch != NULL)
			delete __dswitch;
		__dswitch = NULL;
	};
};

static CDSwitchMonitor __monitor;

RAPTOR_NAMESPACE_END


RAPTOR_NAMESPACE


/////////////////////////////////////////////////////////////////////////////
// CVideoSettings dialog


CVideoSettings::CVideoSettings(	unsigned int colorsSize, /*= CGL_32BITS*/
								unsigned int screenSize, /*= CGL_1280x1024*/
								unsigned int refreshRate, /*= CGL_75_HZ*/
								CWnd* pParent /*=NULL*/)
	: CDialog(CVideoSettings::IDD, pParent)
{
	//{{AFX_DATA_INIT(CVideoSettings)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_screenColorsSize = colorsSize;
	m_screenSize = screenSize;
	m_screenRefreshRate = refreshRate;

	InitVideoData();
}


void CVideoSettings::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CVideoSettings)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CVideoSettings, CDialog)
	//{{AFX_MSG_MAP(CVideoSettings)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_TEST, OnTest)
	ON_BN_CLICKED(IDC_APPLY, OnApply)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


bool CVideoSettings::FilterVMode(LPDEVMODE lpDevMode)
{
	bool acceptColorDepth = false;
	bool acceptScreenSize = false;
	bool acceptScreenFreq = false;

	// testing colordepth
	if ((lpDevMode->dmBitsPerPel==4) && (m_screenColorsSize&CGL_4BITS))
		acceptColorDepth = true;
	else if ((lpDevMode->dmBitsPerPel==8) && (m_screenColorsSize&CGL_8BITS))
		acceptColorDepth = true;
	else if ((lpDevMode->dmBitsPerPel==16) && (m_screenColorsSize&CGL_16BITS))
		acceptColorDepth = true;
	else if ((lpDevMode->dmBitsPerPel==24) && (m_screenColorsSize&CGL_24BITS))
		acceptColorDepth = true;
	else if ((lpDevMode->dmBitsPerPel==32) && (m_screenColorsSize&CGL_32BITS))
		acceptColorDepth = true;

	// testing screenSize
	if ((lpDevMode->dmPelsWidth==320) && (lpDevMode->dmPelsHeight==200) && (m_screenSize&CGL_320x200))
		acceptScreenSize = true;
	else if ((lpDevMode->dmPelsWidth==320) && (lpDevMode->dmPelsHeight==240) && (m_screenSize&CGL_320x240))
		acceptScreenSize = true;
	else if ((lpDevMode->dmPelsWidth==400) && (lpDevMode->dmPelsHeight==300) && (m_screenSize&CGL_400x300))
		acceptScreenSize = true;
	else if ((lpDevMode->dmPelsWidth==480) && (lpDevMode->dmPelsHeight==360) && (m_screenSize&CGL_480x360))
		acceptScreenSize = true;
	else if ((lpDevMode->dmPelsWidth==512) && (lpDevMode->dmPelsHeight==384) && (m_screenSize&CGL_512x384))
		acceptScreenSize = true;
	else if ((lpDevMode->dmPelsWidth==640) && (lpDevMode->dmPelsHeight==400) && (m_screenSize&CGL_640x400))
		acceptScreenSize = true;
	else if ((lpDevMode->dmPelsWidth==640) && (lpDevMode->dmPelsHeight==480) && (m_screenSize&CGL_640x480))
		acceptScreenSize = true;
	else if ((lpDevMode->dmPelsWidth==800) && (lpDevMode->dmPelsHeight==600) && (m_screenSize&CGL_800x600))
		acceptScreenSize = true;
	else if ((lpDevMode->dmPelsWidth==1024) && (lpDevMode->dmPelsHeight==768) && (m_screenSize&CGL_1024x768))
		acceptScreenSize = true;
	else if ((lpDevMode->dmPelsWidth==1152) && (lpDevMode->dmPelsHeight==864) && (m_screenSize&CGL_1152x864))
		acceptScreenSize = true;
	else if ((lpDevMode->dmPelsWidth==1280) && (lpDevMode->dmPelsHeight==960) && (m_screenSize&CGL_1280x960))
		acceptScreenSize = true;
	else if ((lpDevMode->dmPelsWidth==1280) && (lpDevMode->dmPelsHeight==1024) && (m_screenSize&CGL_1280x1024))
		acceptScreenSize = true;
	else if ((lpDevMode->dmPelsWidth==1600) && (lpDevMode->dmPelsHeight==1200) && (m_screenSize&CGL_1600x1200))
		acceptScreenSize = true;

	// testing screenFreq
	if ((lpDevMode->dmDisplayFrequency==60) && (m_screenRefreshRate&CGL_60_HZ))
		acceptScreenFreq = true;
	else if ((lpDevMode->dmDisplayFrequency==70) && (m_screenRefreshRate&CGL_70_HZ))
		acceptScreenFreq = true;
	else if ((lpDevMode->dmDisplayFrequency==72) && (m_screenRefreshRate&CGL_72_HZ))
		acceptScreenFreq = true;
	else if ((lpDevMode->dmDisplayFrequency==75) && (m_screenRefreshRate&CGL_75_HZ))
		acceptScreenFreq = true;
	else if ((lpDevMode->dmDisplayFrequency==85) && (m_screenRefreshRate&CGL_85_HZ))
		acceptScreenFreq = true;
	else if ((lpDevMode->dmDisplayFrequency==100) && (m_screenRefreshRate&CGL_100_HZ))
		acceptScreenFreq = true;
	else if ((lpDevMode->dmDisplayFrequency==120) && (m_screenRefreshRate&CGL_120_HZ))
		acceptScreenFreq = true;

	return (acceptColorDepth && acceptScreenSize && acceptScreenFreq);
}

/////////////////////////////////////////////////////////////////////////////
// CVideoSettings message handlers
BOOL CALLBACK EnumDesktopProc(  LPTSTR lpszDesktop,LPARAM  )
{
	DWORD access =	DESKTOP_CREATEMENU | DESKTOP_CREATEWINDOW |
					DESKTOP_ENUMERATE | DESKTOP_HOOKCONTROL |
					DESKTOP_JOURNALPLAYBACK | DESKTOP_JOURNALRECORD |
					DESKTOP_READOBJECTS | DESKTOP_SWITCHDESKTOP |
					DESKTOP_WRITEOBJECTS ;

	HDESK hh = OpenDesktop(lpszDesktop,DF_ALLOWOTHERACCOUNTHOOK,TRUE,access);

	if (hh!=0)
	{
		string name = "desktop available: ";
		name += CT2A(lpszDesktop);

#ifdef RAPTOR_DEBUG_MODE_GENERATION
		Raptor::GetErrorManager()->generateRaptorError(	CPersistence::CPersistenceClassID::GetClassId(),
														CRaptorErrorManager::RAPTOR_NO_ERROR,
														name);
#endif
		CloseDesktop(hh);
  	}
	return TRUE;
}

bool CVideoSettings::InitVideoData()
{
	int			numDevice = 0;
	LPDEVMODE	lpDevMode = new DEVMODE;

	memset(lpDevMode,sizeof(DEVMODE),0);
	lpDevMode->dmSize = sizeof(DEVMODE);
	lpDevMode->dmDriverExtra = 0;
	lpDevMode->dmFields = DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT|DM_DISPLAYFLAGS|DM_DISPLAYFREQUENCY;

	while (0!=EnumDisplaySettings(NULL,numDevice++,lpDevMode))
	{
		if (FilterVMode(lpDevMode))
		{
			videoModes.push_back(lpDevMode);
			lpDevMode = new DEVMODE;
			memset(lpDevMode,sizeof(DEVMODE),0);
			lpDevMode->dmSize = sizeof(DEVMODE);
			lpDevMode->dmDriverExtra = 0;
			lpDevMode->dmFields = DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT|DM_DISPLAYFLAGS|DM_DISPLAYFREQUENCY;
		}
	}

	delete lpDevMode;

	DEFAULT_WINSTA = GetProcessWindowStation(); 
	if (DEFAULT_WINSTA==NULL)
	{
		RAPTOR_WARNING(CPersistence::CPersistenceClassID::GetClassId(),
					   "Unable to open process window station");
	}

	EnumDesktops( DEFAULT_WINSTA, EnumDesktopProc, 0 );
	//	Start on current desktop, this allows multiple instances
	DEFAULT_DESKTOP = GetThreadDesktop(GetCurrentThreadId());
	if (DEFAULT_DESKTOP==NULL)
	{
		RAPTOR_WARNING(CPersistence::CPersistenceClassID::GetClassId(),
					   "Unable to open process desktop");
	}

	DWORD access =	DESKTOP_CREATEMENU | DESKTOP_CREATEWINDOW |
					DESKTOP_ENUMERATE | DESKTOP_HOOKCONTROL |
					DESKTOP_JOURNALPLAYBACK | DESKTOP_JOURNALRECORD |
					DESKTOP_READOBJECTS | DESKTOP_SWITCHDESKTOP |
					DESKTOP_WRITEOBJECTS ;
	FULL_SCRREN_DESKTOP = CreateDesktop(TEXT("Raptor"), NULL, NULL, DF_ALLOWOTHERACCOUNTHOOK, access, NULL);

	if (FULL_SCRREN_DESKTOP==NULL)
	{
		RAPTOR_ERROR(CPersistence::CPersistenceClassID::GetClassId(),
					 "Unable to create full screen Raptor desktop");
	}
  
	return (videoModes.size() > 0 );
}

void CVideoSettings::OnDestroy() 
{
	CDialog::OnDestroy();
	
	vector<LPDEVMODE>::iterator pos = videoModes.begin();

	while (pos != videoModes.end())
	{
		LPDEVMODE lpDevMode = *pos;
		delete lpDevMode;
		pos++;
	}
	videoModes.clear();
}

BOOL CVideoSettings::OnInitDialog() 
{
	CDialog::OnInitDialog();
	CComboBox *box = (CComboBox*)this->GetDlgItem(IDC_CONFIG);

	for (unsigned int i=0;i<videoModes.size();i++)
	{
		LPDEVMODE lpDevMode = videoModes.at(i);
		stringstream s;
		s << lpDevMode->dmPelsWidth << "x" << lpDevMode->dmPelsHeight;
		s << " " << lpDevMode->dmBitsPerPel << " bits at " << lpDevMode->dmDisplayFrequency << " Htz";
		box->AddString(CA2T(s.str().c_str()));
	}
	box->SetCurSel( 0 );
	

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

LPDEVMODE CVideoSettings::GetVideoModeInfo(unsigned int nVMode) const
{
	if (nVMode<videoModes.size())
		return videoModes.at(nVMode);
	else
		return NULL;
}

bool CVideoSettings::TestVideoMode(unsigned int nVMode) const
{
	if (nVMode>=videoModes.size())
		return false;

	return (DISP_CHANGE_SUCCESSFUL==ChangeDisplaySettings(	videoModes.at(nVMode),
															CDS_TEST|CDS_FULLSCREEN));
}

bool CVideoSettings::SetVideoMode(unsigned int nVMode) const
{
	if (nVMode>=videoModes.size())
		return false;

	return (DISP_CHANGE_SUCCESSFUL==ChangeDisplaySettings(	videoModes.at(nVMode),
															CDS_SET_PRIMARY|CDS_FULLSCREEN));
}

#pragma warning (disable : 4711)	//	hide warning 4711 : TestVideoMode
void CVideoSettings::OnTest() 
{
	CComboBox *box = (CComboBox*)this->GetDlgItem(IDC_CONFIG);
	int selection = box->GetCurSel( );

	if (false == TestVideoMode(selection))
		SetDlgItemText(IDC_STATUS, TEXT("Failed to test video mode"));
	else
		SetDlgItemText(IDC_STATUS, TEXT("Video mode is valid and can be selected"));
}
#pragma warning (default:4711)

#pragma warning (disable : 4711)	//	hide warning 4711 : SetVideoMode
void CVideoSettings::OnApply() 
{
	CComboBox *box = (CComboBox*)this->GetDlgItem(IDC_CONFIG);
	int selection = box->GetCurSel( );

	if (false == SetVideoMode(selection))
		SetDlgItemText(IDC_STATUS, TEXT("Failed to change video mode"));
	else
		SetDlgItemText(IDC_STATUS, TEXT("Video mode changed successfully"));
}
#pragma warning (default:4711)

bool CVideoSettings::PrepareFullScreen(void)
{
	if (FULL_SCRREN_DESKTOP==NULL)
	{
		RAPTOR_ERROR(CPersistence::CPersistenceClassID::GetClassId(),
					 "Unable to create full screen Raptor desktop");
		return false;
	}

	if (DEFAULT_DESKTOP==NULL)
	{
		RAPTOR_ERROR(CPersistence::CPersistenceClassID::GetClassId(),
					 "Unable to open process desktop");
		return false;
	}

	if (__dswitch != NULL)
		delete __dswitch;

	__dswitch = new DSwitch(FULL_SCRREN_DESKTOP,DEFAULT_DESKTOP);

	if (DSwitch::m_oldDesk == NULL)
	{
		stringstream msg;
		msg << "Unable to switch to Raptor desktop: ";
		msg << setThread;
		RAPTOR_ERROR(CPersistence::CPersistenceClassID::GetClassId(), msg.str());
		return false;
	}

	return true;
}