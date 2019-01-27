/***************************************************************************/
/*                                                                         */
/*  AssemblyInfo.cpp                                                       */
/*                                                                         */
/*    Raptor OpenGL & Vulkan realtime 3D Engine SDK.                       */
/*                                                                         */
/*  Copyright 1998-2019 by                                                 */
/*  Fabrice FERRAND.                                                       */
/*                                                                         */
/*  This file is part of the Raptor project, and may only be used,         */
/*  modified, and distributed under the terms of the Raptor project        */
/*  license, LICENSE.  By continuing to use, modify, or distribute         */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/


#include "stdafx.h"

using namespace System;
using namespace System::Reflection;
using namespace System::Runtime::CompilerServices;
using namespace System::Runtime::InteropServices;
using namespace System::Security::Permissions;

//
// Les informations g�n�rales relatives � un assembly d�pendent de
// l'ensemble d'attributs suivant. Changez les valeurs de ces attributs pour modifier les informations
// associ�es � un assembly.
//
[assembly:AssemblyTitleAttribute("BuilderNet")];
[assembly:AssemblyDescriptionAttribute("Raptor code generation Builder")];
[assembly:AssemblyConfigurationAttribute("")];
[assembly:AssemblyCompanyAttribute("Rafale Soft Inc.(c)")];
[assembly:AssemblyProductAttribute("BuilderNet")];
[assembly:AssemblyCopyrightAttribute("Copyright (c) Rafale Soft Inc.(c) 2019")];
[assembly:AssemblyTrademarkAttribute("")];
[assembly:AssemblyCultureAttribute("")];

//
// Les informations de version pour un assembly se composent des quatre valeurs suivantes�:
//
//      Version principale
//      Version secondaire
//      Num�ro de build
//      R�vision
//
// Vous pouvez sp�cifier toutes les valeurs ou indiquer les num�ros de r�vision et de build par d�faut
// en utilisant '*', comme indiqu� ci-dessous�:

#include "System/Version.h"

[assembly:AssemblyVersionAttribute(RAPTOR_VERSION_STR)];

[assembly:ComVisible(false)];

[assembly:CLSCompliantAttribute(true)];

//[assembly:SecurityPermission(SecurityAction::RequestMinimum, UnmanagedCode = true)];
