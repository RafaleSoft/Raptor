// ModuleDOF.h : main header file for the MODULEDOF DLL
//

#if !defined(AFX_MODULEDOF_H__A1ED968E_6A20_4CCD_B7CF_2E0E5B0DCFF6__INCLUDED_)
#define AFX_MODULEDOF_H__A1ED968E_6A20_4CCD_B7CF_2E0E5B0DCFF6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CModuleDOFApp
// See ModuleDOF.cpp for the implementation of this class
//

class CModuleDOFApp : public CWinApp
{
public:
	CModuleDOFApp();

	unsigned int	width;
	unsigned int	height;
	unsigned char	*image;
	float			*zbuffer;
	float			focale;
	float			aperture;
	float			objectplane;

	unsigned short	*workImage;

	void			AfterEffect(void);
	void			proceedPixel(float diameter,unsigned int index);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CModuleDOFApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CModuleDOFApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

extern CModuleDOFApp theApp;
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MODULEDOF_H__A1ED968E_6A20_4CCD_B7CF_2E0E5B0DCFF6__INCLUDED_)
