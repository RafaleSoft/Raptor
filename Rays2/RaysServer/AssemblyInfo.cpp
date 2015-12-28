#include "stdafx.h"

using namespace System;
using namespace System::Reflection;
using namespace System::Runtime::CompilerServices;
using namespace System::Runtime::InteropServices;
using namespace System::Security::Permissions;

//
// Les informations générales relatives à un assembly dépendent de
// l'ensemble d'attributs suivant. Changez les valeurs de ces attributs pour modifier les informations
// associées à un assembly.
//
[assembly:AssemblyTitleAttribute("RaysServer")];
[assembly:AssemblyDescriptionAttribute("Rays raytracer Server")];
[assembly:AssemblyConfigurationAttribute("")];
[assembly:AssemblyCompanyAttribute("Rafale Soft Inc. (c)")];
[assembly:AssemblyProductAttribute("RaysServer")];
[assembly:AssemblyCopyrightAttribute("Copyright (c)  2015")];
[assembly:AssemblyTrademarkAttribute("")];
[assembly:AssemblyCultureAttribute("")];

//
// Les informations de version pour un assembly se composent des quatre valeurs suivantes :
//
//      Version principale
//      Version secondaire
//      Numéro de build
//      Révision
//
// Vous pouvez spécifier toutes les valeurs ou indiquer les numéros de révision et de build par défaut
// en utilisant '*', comme indiqué ci-dessous :

#include "System/Version.h"

[assembly:AssemblyVersionAttribute(RAPTOR_VERSION_STR)];

[assembly:ComVisible(false)];

[assembly:CLSCompliantAttribute(true)];

//[assembly:SecurityPermission(SecurityAction::RequestMinimum, UnmanagedCode = true)];
