// ModuleNoise.h : main header file for the MODULENOISE DLL
//

#if !defined(AFX_MODULENOISE_H__5AE60879_BD67_4A7E_84DA_7CD6EFBDE836__INCLUDED_)
#define AFX_MODULENOISE_H__5AE60879_BD67_4A7E_84DA_7CD6EFBDE836__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols
#include "..\simdlib\simd.h"

/////////////////////////////////////////////////////////////////////////////
// CModuleNoiseApp
// See ModuleNoise.cpp for the implementation of this class
//

class CModuleNoiseApp : public CWinApp
{
public:
	CModuleNoiseApp();

	void InitNoiseGeneration();
	void V_Noise_P( float x, float y, float z );
	CGenericVector<float> &GammaHalf( int i, int j, int k );
	CGenericVector<float> &GammaFull( int i, int j, int k );
	CGenericVector<float> &V_Turb( CGenericVector<float> &pos, float freq_base, int nb_octaves );

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CModuleNoiseApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CModuleNoiseApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CGenericVector<float>	tmpVect;
	CGenericVector<float>	v1;
	CGenericVector<float>	v2;
	CGenericVector<float>	turbulence;
};

extern CModuleNoiseApp theApp;

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MODULENOISE_H__5AE60879_BD67_4A7E_84DA_7CD6EFBDE836__INCLUDED_)
