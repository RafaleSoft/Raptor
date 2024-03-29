// GLXContextManager.h: interface for the CGLXContextManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GLXCONTEXTMANAGER_H__B6CE3CDF_D7E4_4B9C_89BF_5E934062BC97__INCLUDED_)
#define AFX_GLXCONTEXTMANAGER_H__B6CE3CDF_D7E4_4B9C_89BF_5E934062BC97__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#ifndef __GLX_RAPTOR_GLEXT_H__
	#include "GLXGlext.h"
#endif
#if !defined(AFX_CONTEXTMANAGER_H__F992F5F0_D8A5_475F_9777_B0EB30E7648E__INCLUDED_)
	#include "Subsys/ContextManager.h"
#endif


RAPTOR_NAMESPACE_BEGIN

class CRaptorGLExtensions;
class CRaptorDisplayConfig;


class CGLXContextManager : public CContextManager  
{
public:
	CGLXContextManager();
	virtual ~CGLXContextManager();

	//! See base class
	virtual const CRaptorGLExtensions *const glGetExtensions(void);

	//! See base class
	virtual RAPTOR_HANDLE glCreateWindow(const CRaptorDisplayConfig& pcs, CRaptorDisplay *& pdisplay,RENDERING_CONTEXT_ID &ctx);

    //! See base class
    virtual bool glDestroyWindow(const RAPTOR_HANDLE& wnd);

	//! See base class
	virtual RENDERING_CONTEXT_ID glCreateContext(const RAPTOR_HANDLE& device,
												 const CRaptorDisplayConfig& config);

	//! See base class
	virtual RENDERING_CONTEXT_ID glCreateExtendedContext(const RAPTOR_HANDLE& device,
														 const CRaptorDisplayConfig& config);

	//! See base class
	virtual RENDERING_CONTEXT_ID glGetCurrentContext(void) const { return m_currentContext; };

	//! See base class
	virtual RAPTOR_HANDLE getDevice(RENDERING_CONTEXT_ID ctx) const ;

	//! See base class
	virtual RENDERING_CONTEXT_ID getContext(RAPTOR_HANDLE device) const;

	//! See base class
	virtual void glMakeCurrentContext(const RAPTOR_HANDLE& device,RENDERING_CONTEXT_ID ctx);

	//! See base class
	virtual void glDestroyContext(RENDERING_CONTEXT_ID ctx);

	//! See base class
	virtual void glSwapBuffers(RENDERING_CONTEXT_ID ctx);

	//! See base class
	virtual bool glSwapVSync(unsigned int nbVSync) const;

	//! See base class
	virtual void vkSwapVSync(unsigned int framerate);


	//! See base class
	virtual PIXEL_BUFFER_ID glCreatePBuffer(const CRaptorDisplayConfig& pcs);

	//! See base class
	virtual bool glDestroyPBuffer(PIXEL_BUFFER_ID pbuffer);

	//! See base class
	virtual void glBindPBuffer(PIXEL_BUFFER_ID pbuffer, CTextureObject::CUBE_FACE selectBuffer = CTextureObject::CGL_CUBEMAP_NONE);

	//! See base class
	virtual bool glIsPBuffer(PIXEL_BUFFER_ID pbuffer) const;

	//! See base class
	virtual bool glBindTexImageARB(PIXEL_BUFFER_ID pbuffer,int iBuffer);

	//! See base class
	virtual bool glReleaseTexImageARB(PIXEL_BUFFER_ID pbuffer,int iBuffer);


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
		GLXContext	OGLContext;
        Display     *display;
        Window      window;
        XVisualInfo *visual;
		CRaptorGLExtensions*	pExtensions;
	} context_t;

	string					extensions;
	string					glx_extensions;
	

    Display*                pGlobalDisplay;                 
	GLXContext				glGlobalRC;
	GLXContext				glGlobalExtendedRC;

	unsigned int			nbPBuffers;
#if defined(WGL_ARB_pbuffer)
	pbuffer_t				*pBuffers;
#endif
	unsigned int			nbContext;
	context_t				*pContext;

	RENDERING_CONTEXT_ID	m_currentContext;

	
#if defined(VK_VERSION_1_0)
	//! See base class
	virtual uint32_t getPresentationSuppotQueueFamily(RENDERING_CONTEXT_ID ctx);

	//! See base class
	virtual bool vkCreateSurface(const RAPTOR_HANDLE& handle, RENDERING_CONTEXT_ID ctx);
#endif
};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_GLXCONTEXTMANAGER_H__B6CE3CDF_D7E4_4B9C_89BF_5E934062BC97__INCLUDED_)
