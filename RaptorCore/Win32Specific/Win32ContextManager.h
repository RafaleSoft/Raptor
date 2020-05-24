/***************************************************************************/
/*                                                                         */
/*  Win32ContextManager.h                                                  */
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


#if !defined(AFX_WIN32CONTEXTMANAGER_H__A1D82397_7E92_4D01_A04D_782BCFD17689__INCLUDED_)
#define AFX_WIN32CONTEXTMANAGER_H__A1D82397_7E92_4D01_A04D_782BCFD17689__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if !defined(AFX_CONTEXTMANAGER_H__F992F5F0_D8A5_475F_9777_B0EB30E7648E__INCLUDED_)
	#include "Subsys/ContextManager.h"
#endif


RAPTOR_NAMESPACE_BEGIN

class CRaptorGLExtensions;


//! This class implements openGL contexts management for WIN32 platform.
class CWin32ContextManager : public CContextManager  
{
public:
	CWin32ContextManager();
	virtual ~CWin32ContextManager();

	//! @see base class
	virtual const CRaptorGLExtensions *const glGetExtensions(void);

	//! @see base class
	virtual bool validateConfig(CRaptorDisplayConfig& rdc);

	//! @see base class
	virtual RAPTOR_HANDLE glCreateWindow(	const CRaptorDisplayConfig &pcs,
											CRaptorDisplay *&pDisplay,
											RENDERING_CONTEXT_ID &ctx);

    //! @see base class
    virtual bool glDestroyWindow(const RAPTOR_HANDLE& wnd);

	//! @see base class
	virtual RENDERING_CONTEXT_ID glCreateContext(	const RAPTOR_HANDLE& device,
													const CRaptorDisplayConfig& config);

	//! @see base class
	virtual RENDERING_CONTEXT_ID glCreateExtendedContext(	const RAPTOR_HANDLE& device,
															const CRaptorDisplayConfig& config);

    //! @see base class
    virtual RAPTOR_HANDLE getDevice(RENDERING_CONTEXT_ID ctx) const;

    //! @see base class
    virtual RENDERING_CONTEXT_ID getContext(RAPTOR_HANDLE device) const;

	//! @see base class
	virtual void glMakeCurrentContext(const RAPTOR_HANDLE& device,RENDERING_CONTEXT_ID ctx);

	//! @see base class
	virtual void glDestroyContext(RENDERING_CONTEXT_ID ctx);

	//! @see base class
	virtual void glSwapBuffers(RENDERING_CONTEXT_ID ctx);

	//! @see base class
	virtual bool glSwapVSync(unsigned int nbVSync) const;



	//! @see base class
	virtual PIXEL_BUFFER_ID glCreatePBuffer(const CRaptorDisplayConfig& pcs);

	//! @see base class
	virtual bool glDestroyPBuffer(PIXEL_BUFFER_ID pbuffer);

	//! @see base class
	virtual void glBindPBuffer(PIXEL_BUFFER_ID pbuffer,ITextureObject::CUBE_FACE selectBuffer);

	//! @see base class
	virtual bool glIsPBuffer(PIXEL_BUFFER_ID pbuffer) const;

	//! @see base class
	virtual bool glBindTexImageARB(PIXEL_BUFFER_ID pbuffer,int iBuffer);

	//! @see base class
	virtual bool glReleaseTexImageARB(PIXEL_BUFFER_ID pbuffer,int iBuffer);


#if defined(VK_VERSION_1_0)
	//! @see base class
	virtual bool vkInit(void);
	//! @see base class
	virtual bool vkRelease(void);
	//! @see base class
	virtual bool vkCreateSurface(const RAPTOR_HANDLE& handle,RENDERING_CONTEXT_ID ctx);
	//!	@see base class
	virtual uint32_t getPresentationSuppotQueueFamily(RENDERING_CONTEXT_ID ctx);
	//!	@see base class
	virtual void vkSwapVSync(unsigned int framerate);
#endif



private:
#if defined(WGL_ARB_pbuffer)
	typedef struct pbuffer_tag
	{
		HPBUFFERARB pbuffer;
		HDC			pbufferDC;
		HGLRC		pbufferGLRC;
	} pbuffer_t;
#endif

	typedef struct context_tag
	{
		HGLRC					OGLContext;
        HDC						WIN32Context;
        HWND					WIN32Window;
		PAINTSTRUCT				WIN32Paint;
	    CRaptorGLExtensions*	pExtensions;
	} context_t;

	unsigned int			nbPBuffers;
#if defined(WGL_ARB_pbuffer)
	pbuffer_t				*pBuffers;
#endif
	unsigned int			nbContext;
	context_t				*pContext;


#if defined RAPTOR_DEBUG_MODE_GENERATION
    void getLastError(const std::string& file,int line) const;
	#define	CATCH_WIN32_ERROR \
		getLastError(__FILE__,__LINE__);
#else
	#define	CATCH_WIN32_ERROR
#endif

};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_WIN32CONTEXTMANAGER_H__A1D82397_7E92_4D01_A04D_782BCFD17689__INCLUDED_)

