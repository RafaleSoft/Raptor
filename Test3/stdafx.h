// stdafx.h : fichier Include pour les fichiers Include système standard,
// ou les fichiers Include spécifiques aux projets qui sont utilisés fréquemment,
// et sont rarement modifiés
//


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef WIN32
    #define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers
	#include <windows.h>
	#include <tchar.h>
#endif
