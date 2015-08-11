// WorkUnit.h : main header file for the WORKUNIT application
//

#if !defined(AFX_WORKUNIT_H__0D5F8B77_B031_4EB8_9AE5_F80401D99195__INCLUDED_)
#define AFX_WORKUNIT_H__0D5F8B77_B031_4EB8_9AE5_F80401D99195__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols
#include "..\messages.h"

/////////////////////////////////////////////////////////////////////////////
// CWorkUnitApp:
// See WorkUnit.cpp for the implementation of this class
//
class CGenericRenderObject;
class CWorkUnitDlg;
class CTexture;

class CWorkUnitApp : public CWinApp
{
private:
	CWinThread		*raytracer;
	CWorkUnitDlg	*dlg;

public:
	CWorkUnitApp();

	void	WriteMessage(CString &msg);
	void	ProcessMsg(MSGSTRUCT& msg,unsigned char raw_data[]);
	void	SendPercentage(UINT percent) const;
	void	Notify(void);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWorkUnitApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation
private:
	bool RunRaytrace(MSGSTRUCT& msg,unsigned char raw_data[]);
	bool BuildSpheres(MSGSTRUCT& msg,unsigned char raw_data[]);
	bool BuildLights(MSGSTRUCT& msg,unsigned char raw_data[]);
	bool BuildCamera(MSGSTRUCT& msg,unsigned char raw_data[]);
	bool BuildGeometries(MSGSTRUCT& msg,unsigned char raw_data[]);
	bool BuildTextures(MSGSTRUCT& msg,unsigned char raw_data[]);
	bool BuildPlugins(MSGSTRUCT& msg,unsigned char raw_data[]);
	bool BuildFrames(MSGSTRUCT& msg,unsigned char raw_data[]);

	CTexture *GetTexture(CString tname);
	void SetTextures(CGenericRenderObject* obj,rays_objectbase_t& base);
	bool GetRaytraceData();
	void SaveZBuffer(void);
	bool LoadModule(CString name);

	//{{AFX_MSG(CWorkUnitApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WORKUNIT_H__0D5F8B77_B031_4EB8_9AE5_F80401D99195__INCLUDED_)
