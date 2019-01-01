// ModuleTemplate.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "ModuleTemplate.h"
#include "..\PlugIn.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CModuleTemplateApp

BEGIN_MESSAGE_MAP(CModuleTemplateApp, CWinApp)
	//{{AFX_MSG_MAP(CModuleTemplateApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CModuleTemplateApp construction

CModuleTemplateApp::CModuleTemplateApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CModuleTemplateApp object

CModuleTemplateApp theApp;


#include <iostream.h>	// cout

extern "C" DLLEXPORT void InitModule()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	//	Init datas...
}


extern "C" DLLEXPORT char *GetModuleName()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return "ModuleTemplate";
}

extern "C" DLLEXPORT char *GetModuleAuthor()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return "Rafale Soft Inc.(c)";
}

extern "C" DLLEXPORT char *GetModuleDescription()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return "Module plug-in template for Rays2";
}

extern "C" DLLEXPORT PLUGIN_TYPE GetModuleType(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return UNKNOWN_TYPE;
}

extern "C" DLLEXPORT unsigned int GetModuleVersion(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return 0x01000001;	//	1.0.0.1
}

extern "C" DLLEXPORT PARAMETER_DATA *GetParameters(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return NULL;
}

extern "C" DLLEXPORT void SetParameters(PARAMETER_DATA* params)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

}

extern "C" DLLEXPORT void AfterEffect(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	//	do processing here
}