// RaptorStatus.h: interface for the CRaptorStatus class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RAPTORSTATUS_H__353B53E2_3A33_11D3_9142_B236E7C6A77C__INCLUDED_)
#define AFX_RAPTORSTATUS_H__353B53E2_3A33_11D3_9142_B236E7C6A77C__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __CGLTYPES_HPP__
	#include "System/CGLTypes.h"
#endif


RAPTOR_NAMESPACE_BEGIN

//////////////////////////////////////////////////////////////////////
//	Video Mode Configuration
#define		CGL_4BITS		0x0001
#define		CGL_8BITS		0x0002
#define		CGL_16BITS		0x0004
#define		CGL_24BITS		0x0008
#define		CGL_32BITS		0x0010

#define		CGL_320x200		0x0001
#define		CGL_320x240		0x0002
#define		CGL_400x300		0x0004
#define		CGL_480x360		0x0008
#define		CGL_512x384		0x0010
#define		CGL_640x400		0X0020
#define		CGL_640x480		0x0040
#define		CGL_800x600		0x0080
#define		CGL_1024x768	0x0100
#define		CGL_1152x864	0x0200
#define		CGL_1280x960	0x0400
#define		CGL_1280x1024	0x0800
#define		CGL_1600x1200	0x1000

#define		CGL_60_HZ		0x0001
#define		CGL_70_HZ		0x0002
#define		CGL_72_HZ		0x0004
#define		CGL_75_HZ		0x0008
#define		CGL_85_HZ		0x0010
#define		CGL_100_HZ		0x0020
#define		CGL_120_HZ		0x0040


class CGLView;
class CVideoSettings;
class CPersistence;
class CRaptorErrors;

class RAPTOR_API CRaptorStatus  
{
public:
	CRaptorStatus();
	virtual ~CRaptorStatus();


	//	Raptor dialogs
	int InitDlg(CGLView *Context = NULL);
	int ObjectsDlg(CGLView *Context = NULL);
	int VideoSettingsDlg(	unsigned int colorsSize ,
							unsigned int screenSize ,
							unsigned int refreshRate );
	int AboutDlg();
	int ErrorsDlg();

	//	Functions re-routing
	bool SwitchFullScreen();
	int InitVideoSettings(	unsigned int colorsSize ,
							unsigned int screenSize ,
							unsigned int refreshRate );
	bool SelectVideoMode(unsigned int nVMode,bool testOnly = true) const;


private:
	CPersistence			*persistence;
	CVideoSettings			*vs;
};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_RAPTORSTATUS_H__353B53E2_3A33_11D3_9142_B236E7C6A77C__INCLUDED_)

