// stdafx.h�: fichier Include pour les fichiers Include syst�me standard,
// ou les fichiers Include sp�cifiques aux projets qui sont utilis�s fr�quemment,
// et sont rarement modifi�s
//

#ifdef WIN32
    #if _MSC_VER > 1000
    #pragma once
    #endif // _MSC_VER > 1000

	#include "targetver.h"

	#define WIN32_LEAN_AND_MEAN             // Exclure les en-t�tes Windows rarement utilis�s
	// Fichiers d'en-t�te Windows�:
	#pragma warning(disable: 4275)  // deriving exported class from non-exported
	#pragma warning(disable: 4251)  // using non-exported as public in exported
	#include <windows.h>
#endif



// TODO�: faites r�f�rence ici aux en-t�tes suppl�mentaires n�cessaires au programme
