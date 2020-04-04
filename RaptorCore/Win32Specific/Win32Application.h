/***************************************************************************/
/*                                                                         */
/*  Win32Application.h                                                     */
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


#if !defined(AFX_WIN32APPLICATION_H__3EADD210_ABF5_4CFD_A511_09047EDBB881__INCLUDED_)
#define AFX_WIN32APPLICATION_H__3EADD210_ABF5_4CFD_A511_09047EDBB881__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if !defined(AFX_RAPTORAPPLICATION_H__E102DE21_30F2_4478_8C5A_55F780A0B33E__INCLUDED_)
    #include "System/RaptorApplication.h"
#endif


RAPTOR_NAMESPACE_BEGIN


//! This class implements a basic RaptorApplication for Win32 platform
class CWin32Application : public CRaptorApplication
{
public:
	CWin32Application();
	virtual ~CWin32Application();

	//!	The prototype of the window based event handler
	typedef LRESULT (CALLBACK *windowProc)(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	//! @return the default application window event handler
	static windowProc getWindowProc(void);

	//!	Temporary
	//!	TODO: find something cleaner !!!
	static bool destroyWindow;

protected:
	//! Implements CRaptorApplication
    virtual bool run(void);

	//! Implements CRaptorApplication
	virtual void grabCursor(bool grab);


private:
	//!	Internal async keyboard input handler
	void asyncWindowProc(LONG xCenter,LONG yCenter);

	//!	Mouse capture.
	bool	m_bGrab;

	//!	Virtual mouse position for mouse capture
	LONG	m_VMouseXPos;
	LONG	m_VMouseYPos;
};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_WIN32APPLICATION_H__3EADD210_ABF5_4CFD_A511_09047EDBB881__INCLUDED_)

