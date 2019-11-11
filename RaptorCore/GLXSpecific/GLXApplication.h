/***************************************************************************/
/*                                                                         */
/*  GLXApplication.h                                                       */
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


#if !defined(AFX_GLXAPPLICATION_H__7384D172_A179_4048_BC2A_6D2C628442E5__INCLUDED_)
#define AFX_GLXAPPLICATION_H__7384D172_A179_4048_BC2A_6D2C628442E5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if !defined(AFX_RAPTORAPPLICATION_H__E102DE21_30F2_4478_8C5A_55F780A0B33E__INCLUDED_)
    #include "System/RaptorApplication.h"
#endif

RAPTOR_NAMESPACE_BEGIN

//! This class implements a basic RaptorApplication for XWindow platform
class CGLXApplication : public CRaptorApplication  
{
public:
	CGLXApplication();
	virtual ~CGLXApplication();

protected:
	//! @see CRaptorApplication
	virtual bool initApplication(void);

	//! @see CRaptorApplication
    virtual bool run(void);

	//! @see CRaptorApplication
    virtual void setRootWindow(const RAPTOR_HANDLE& root);

	//! @see CRaptorApplication
	virtual void grabCursor(bool grab);
};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_GLXAPPLICATION_H__7384D172_A179_4048_BC2A_6D2C628442E5__INCLUDED_)
