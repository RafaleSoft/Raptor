// ModuleTemplate.h : main header file for the MODULETEMPLATE DLL
//

#if !defined(AFX_MODULETEMPLATE_H__352171E5_0FC3_11D3_9142_E63213FDA737__INCLUDED_)
#define AFX_MODULETEMPLATE_H__352171E5_0FC3_11D3_9142_E63213FDA737__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols


/////////////////////////////////////////////////////////////////////////////
// CModuleTemplateApp
// See ModuleTemplate.cpp for the implementation of this class
//

class EXPORT CModuleTemplateApp : public CWinApp
{
public:
	CModuleTemplateApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CModuleTemplateApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CModuleTemplateApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MODULETEMPLATE_H__352171E5_0FC3_11D3_9142_E63213FDA737__INCLUDED_)
