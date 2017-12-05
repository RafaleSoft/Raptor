// stdafx.h : fichier Include pour les fichiers Include système standard,
// ou les fichiers Include spécifiques aux projets qui sont utilisés fréquemment,
// et sont rarement modifiés
//

#ifdef WIN32
    #if _MSC_VER > 1000
    #pragma once
    #endif // _MSC_VER > 1000

	#include "targetver.h"

	#define WIN32_LEAN_AND_MEAN             // Exclure les en-têtes Windows rarement utilisés
	// Fichiers d'en-tête Windows :
	#pragma warning(disable: 4275)  // deriving exported class from non-exported
	#pragma warning(disable: 4251)  // using non-exported as public in exported
	#include <windows.h>
#endif



// TODO : faites référence ici aux en-têtes supplémentaires nécessaires au programme
