// RaptorStatus.cpp: implementation of the CRaptorStatus class.
//
//////////////////////////////////////////////////////////////////////
#include "StdAfx.h"

#include "Resource.h"


#if !defined(AFX_RAPTORSTATUS_H__353B53E2_3A33_11D3_9142_B236E7C6A77C__INCLUDED_)
	#include "RaptorStatus.h"
#endif

#if !defined(AFX_INITDIALOG_H__57A7FAB0_3A1F_11D3_9142_B236E7C6A77C__INCLUDED_)
	#include "InitDialog.h"
#endif

#if !defined(AFX_OBJECTDIALOG_H__5561BA29_831B_11D3_9142_EEB51CEBBDB0__INCLUDED_)
	#include "ObjectDialog.h"
#endif

#if !defined(AFX_VIDEOSETTINGS_H__CB2E9A06_6EFA_4545_B84D_A5DBCB6799F3__INCLUDED_)
	#include "VideoSettings.h"
#endif

#if !defined(AFX_ABOUTDIALOG_H__23A6FAE2_FE89_466A_B177_290ACC171E05__INCLUDED_)
	#include "AboutDialog.h"
#endif

#if !defined(AFX_RAPTORERRORS_H__4B829705_7FC0_44DD_83A2_CAB01172F485__INCLUDED_)
	#include "RaptorErrors.h"
#endif
#if !defined(AFX_PERSISTENCE_H__5561BA28_831B_11D3_9142_EEB51CEBBDB0__INCLUDED_)
	#include "GLHierarchy/Persistence.h"
#endif


RAPTOR_NAMESPACE

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRaptorStatus::CRaptorStatus():
	vs(NULL)
{
	//persistence = new CPersistence(	CPersistence::CPersistenceClassID::GetClassId(),
									//"RAPTOR_STATUS");
}

CRaptorStatus::~CRaptorStatus()
{
	//delete persistence;

	if (vs != NULL)
		delete vs;
}

int CRaptorStatus::InitDlg(CGLView *Context)
{
	CInitDialog dlg;
	dlg.GLContext = Context;
	return dlg.DoModal();
}

int CRaptorStatus::ObjectsDlg(CGLView *Context)
{
	CObjectDialog dlg;
	dlg.GLContext = Context;
	return dlg.DoModal();
}

int CRaptorStatus::VideoSettingsDlg(unsigned int colorsSize,
									unsigned int screenSize,
									unsigned int refreshRate)
{
	CVideoSettings dlg(colorsSize,screenSize,refreshRate);
	return dlg.DoModal();
}

int CRaptorStatus::AboutDlg()
{
	CAboutDialog dlg;
	return dlg.DoModal();
}

int CRaptorStatus::ErrorsDlg()
{
	CRaptorErrors dlg;
	return dlg.DoModal();
}


int CRaptorStatus::InitVideoSettings(	unsigned int colorsSize,
										unsigned int screenSize,
										unsigned int refreshRate)
{
	if (vs != NULL)
		delete vs;

	vs = new CVideoSettings(colorsSize,screenSize,refreshRate);

	if (vs == NULL)
		return -1;
	else
		return vs->GetNbVideoMode();
}

bool CRaptorStatus::SelectVideoMode(unsigned int nVMode,bool testOnly) const
{
	if (vs == NULL)
		return false;

	unsigned int nv = nVMode;

	if (nv < 0)
		nv = 0;

	if (nv > vs->GetNbVideoMode())
		nv = vs->GetNbVideoMode()-1;

	if (nv < 0)
		return false;

	if (testOnly)
		return vs->TestVideoMode(nv);
	else
		return vs->SetVideoMode(nv);
}

bool CRaptorStatus::SwitchFullScreen()
{
	if (vs == NULL)
		return false;

	return vs->PrepareFullScreen();
}