#if !defined(AFX_VIDEOSETTINGS_H__CB2E9A06_6EFA_4545_B84D_A5DBCB6799F3__INCLUDED_)
#define AFX_VIDEOSETTINGS_H__CB2E9A06_6EFA_4545_B84D_A5DBCB6799F3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VideoSettings.h : header file
//

#include "RaptorStatus.h"

//	DEVMODE should be defined in wingdi.h
/*
typedef struct _devicemode
{
	TCHAR  dmDeviceName[32];
	WORD   dmSpecVersion;
	WORD   dmDriverVersion;
	WORD   dmSize;
	WORD   dmDriverExtra;
	DWORD  dmFields;
	short  dmOrientation;
	short  dmPaperSize;
	short  dmPaperLength;
	short  dmPaperWidth;
	short  dmScale;
	short  dmCopies;
	short  dmDefaultSource;
	short  dmPrintQuality;
	short  dmColor;
	short  dmDuplex;
	short  dmYResolution;
	short  dmTTOption;
	short  dmCollate;
	TCHAR  dmFormName[32];
	WORD   dmUnusedPadding;
	USHORT dmBitsPerPel;
	DWORD  dmPelsWidth;
	DWORD  dmPelsHeight;
	DWORD  dmDisplayFlags;
	DWORD  dmDisplayFrequency;
} DEVMODE;
*/

RAPTOR_NAMESPACE_BEGIN


/////////////////////////////////////////////////////////////////////////////
// CVideoSettings dialog

class CVideoSettings : public CDialog
{
public:

// Construction
public:
	CVideoSettings(	unsigned int colorsSize = CGL_32BITS,
					unsigned int screenSize = CGL_1280x1024,
					unsigned int refreshRate = CGL_75_HZ,
					CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CVideoSettings)
	enum { IDD = IDD_DIALOG3 };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVideoSettings)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

public:
	LPDEVMODE	GetVideoModeInfo(unsigned int nVMode) const;
	bool		TestVideoMode(unsigned int nVMode) const;
	bool		SetVideoMode(unsigned int nVMode) const;
	unsigned int GetNbVideoMode(void) const { return videoModes.size(); };
	//	prepares the desktop before full screen switch
	bool		PrepareFullScreen(void);



protected:

	unsigned int		m_screenColorsSize;
	unsigned int		m_screenSize;
	unsigned int		m_screenRefreshRate;

	vector<LPDEVMODE>	videoModes;

	// Generated message map functions
	//{{AFX_MSG(CVideoSettings)
	afx_msg void OnDestroy();
	virtual BOOL OnInitDialog();
	afx_msg void OnTest();
	afx_msg void OnApply();
	//}}AFX_MSG

	bool	FilterVMode(LPDEVMODE lpDevMode);
	bool	InitVideoData();

	//	Default initial desktop on window station
	HDESK DEFAULT_DESKTOP;
	//	Full screen desktop
	HDESK FULL_SCRREN_DESKTOP;
	//	Default initial window station
	HWINSTA DEFAULT_WINSTA;
	
	DECLARE_MESSAGE_MAP()
};

RAPTOR_NAMESPACE_END

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VIDEOSETTINGS_H__CB2E9A06_6EFA_4545_B84D_A5DBCB6799F3__INCLUDED_)

