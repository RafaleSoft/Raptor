// stdafx.h : fichier Include pour les fichiers Include système standard,
// ou les fichiers Include spécifiques aux projets qui sont utilisés fréquemment,
// et sont rarement modifiés
//

#if !defined(AFX_STDAFX_H__6BB9B418_367C_4759_B3EC_6C2C87E46118__INCLUDED_)
#define AFX_STDAFX_H__6BB9B418_367C_4759_B3EC_6C2C87E46118__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef WIN32
    #define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

	#include <winsock2.h>
	#include <windows.h>
#endif

#include <iostream>

#endif // AFX_STDAFX_H__6BB9B418_367C_4759_B3EC_6C2C87E46118__INCLUDED_