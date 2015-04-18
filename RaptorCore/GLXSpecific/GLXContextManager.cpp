// GLXContextManager.cpp: implementation of the CGLXContextManager class.
//
//////////////////////////////////////////////////////////////////////

#include "Subsys/CodeGeneration.h"


#if !defined(AFX_GLXCONTEXTMANAGER_H__B6CE3CDF_D7E4_4B9C_89BF_5E934062BC97__INCLUDED_)
    #include "GLXContextManager.h"
#endif

#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
	#include "System/Raptor.h"
#endif
#if !defined(AFX_PERSISTENCE_H__5561BA28_831B_11D3_9142_EEB51CEBBDB0__INCLUDED_)
	#include "GLHierarchy/Persistence.h"
#endif

#if !defined(AFX_RAPTOREXTENSIONS_H__E5B5A1D9_60F8_4E20_B4E1_8E5A9CB7E0EB__INCLUDED_)
	#include "System/RaptorExtensions.h"
#endif
#ifndef __GLOBAL_H__
	#include "System/Global.h"
#endif
#if !defined(AFX_GLXVIEW_H__4B65A453_8F4F_4F97_835F_23EE19B5657E__INCLUDED_)
    #include "GLXView.h"
#endif


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

static const int MAX_PBUFFERS = 16;
static const int MAX_CONTEXT = 16;

//////////////////////////////////////////////////////////////////////
//
#define DEFINE_ACCELERATION \
piAttribIList[attribIndex++] = WGL_ACCELERATION_ARB;\
if ((m_mode & 0x0700) == CGL_SOFTWARE)  piAttribIList[attribIndex++] = WGL_NO_ACCELERATION_ARB;\
else if ((m_mode & 0x0700) == CGL_GENERIC) piAttribIList[attribIndex++] = WGL_GENERIC_ACCELERATION_ARB;\
else if ((m_mode & 0x0700) == CGL_HARDWARE) piAttribIList[attribIndex++] = WGL_FULL_ACCELERATION_ARB;\
else piAttribIList[attribIndex++] = WGL_FULL_ACCELERATION_ARB;\
piAttribIList[attribIndex++] = WGL_SUPPORT_OPENGL_ARB; piAttribIList[attribIndex++] = TRUE;

#define DEFINE_PIXELS \
piAttribIList[attribIndex++] = WGL_PIXEL_TYPE_ARB;\
if ((m_mode & CGL_FLOAT) == CGL_FLOAT)  \
{ \
    piAttribIList[attribIndex++] = WGL_TYPE_RGBA_FLOAT_ARB;\
} \
else \
{ \
    piAttribIList[attribIndex++] = WGL_TYPE_RGBA_ARB;\
    piAttribIList[attribIndex++] = WGL_COLOR_BITS_ARB; piAttribIList[attribIndex++] = 24;\
    piAttribIList[attribIndex++] = WGL_RED_BITS_ARB; piAttribIList[attribIndex++] = 8;\
    piAttribIList[attribIndex++] = WGL_GREEN_BITS_ARB; piAttribIList[attribIndex++] = 8;\
    piAttribIList[attribIndex++] = WGL_BLUE_BITS_ARB; piAttribIList[attribIndex++] = 8;\
    piAttribIList[attribIndex++] = WGL_ALPHA_BITS_ARB;\
    if (m_mode & CGL_RGBA) piAttribIList[attribIndex++] = 8; else piAttribIList[attribIndex++] = 0; \
}

#define DEFINE_DOUBLE_BUFFER \
if ((m_mode & CGL_DOUBLE) == CGL_DOUBLE)\
{\
	piAttribIList[attribIndex++] = WGL_DOUBLE_BUFFER_ARB;\
	piAttribIList[attribIndex++] = TRUE;\
	piAttribIList[attribIndex++] = WGL_SWAP_METHOD_ARB;\
	piAttribIList[attribIndex++] = WGL_SWAP_COPY_ARB;\
}

#define DEFINE_DEPTH_BUFFER \
if ((m_mode & CGL_DEPTH_32) == CGL_DEPTH_32)\
{\
	piAttribIList[attribIndex++] = WGL_DEPTH_BITS_ARB;\
	piAttribIList[attribIndex++]= 32;\
}\
else if ((m_mode & CGL_DEPTH_16) == CGL_DEPTH_16)\
{\
	piAttribIList[attribIndex++] = WGL_DEPTH_BITS_ARB;\
	piAttribIList[attribIndex++] =  16;\
}\
else if ((m_mode & CGL_DEPTH_24) == CGL_DEPTH_24)\
{\
	piAttribIList[attribIndex++] = WGL_DEPTH_BITS_ARB;\
	piAttribIList[attribIndex++] = 24;\
}

#define DEFINE_STENCIL_BUFFER \
if (m_mode & CGL_STENCIL)\
{\
	piAttribIList[attribIndex++] = WGL_STENCIL_BITS_ARB;\
	piAttribIList[attribIndex++] = 8;\
}

#define DEFINE_ACCUM_BUFFER \
if ((m_mode & CGL_ACCUM) == CGL_ACCUM)\
{\
	piAttribIList[attribIndex++] = WGL_ACCUM_BITS_ARB;\
	piAttribIList[attribIndex++] = 64;\
}

#if defined(GL_ARB_multisample)
	#define DEFINE_SAMPLE_BUFFER \
	if (m_mode & CGL_ANTIALIAS)\
	{\
		piAttribIList[attribIndex++] = WGL_SAMPLE_BUFFERS_ARB;\
		piAttribIList[attribIndex++] = 1;\
		piAttribIList[attribIndex++] = WGL_SAMPLES_ARB;\
		piAttribIList[attribIndex++] = ((m_mode & 0xF0000000) >> 28) + 1;\
	}
#else
	#define DEFINE_SAMPLE_BUFFER
#endif


RAPTOR_NAMESPACE

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGLXContextManager::CGLXContextManager():
	pExtensions(NULL),pExtensionsTmp(NULL),pGlobalDisplay(NULL)
{
	pExtensions = new CRaptorExtensions();

	glGlobalRC = NULL;
	glGlobalExtendedRC = NULL;

	nbPBuffers = 0;
	nbContext = 0;

	m_currentContext = -1;
#if defined(WGL_ARB_pbuffer)
	pBuffers = new pbuffer_t[MAX_PBUFFERS];
#endif
	pContext = new context_t[MAX_CONTEXT];

    unsigned int i=0;
#if defined(WGL_ARB_pbuffer)
	for (i=0;i<MAX_PBUFFERS;i++)
	{
		pBuffers[i].pbuffer = 0;
		pBuffers[i].pbufferDC = 0;
		pBuffers[i].pbufferGLRC = 0;
	}
#endif
	for (i=0;i<MAX_CONTEXT;i++)
	{
		pContext[i].window = 0;
		pContext[i].display = 0;
		pContext[i].visual = 0;
		pContext[i].OGLContext = 0;
	}

    pGlobalDisplay = XOpenDisplay(NULL);
    
    // Check support for OpenGL
	int dummy;
	if ((pGlobalDisplay == NULL) || !glXQueryExtension(pGlobalDisplay, &dummy, &dummy))
	{
		Raptor::GetErrorManager()->generateRaptorError(	Global::COpenGLClassID::GetClassId(),
                                                            CRaptorErrorManager::RAPTOR_FATAL,
										                    "OpenGL GLX Extensions are not supported or display is invalid!");
	}
}

CGLXContextManager::~CGLXContextManager()
{
	delete pExtensions;
}

const CRaptorExtensions *const CGLXContextManager::glGetExtensions(void)
{
	return pExtensions;
}

void CGLXContextManager::glInitExtensions(void)
{
	pExtensions->glInitExtensions();
#if defined(WGL_ARB_extensions_string)
	HDC hdc = wglGetCurrentDC();
	if ((wgl_extensions.IsEmpty()) && (pExtensions->wglGetExtensionsStringARB))
		wgl_extensions = pExtensions->wglGetExtensionsStringARB(hdc);
#endif

    CATCH_GL_ERROR
}


RAPTOR_HANDLE CGLXContextManager::getDevice(RENDERING_CONTEXT_ID ctx) const
{
    RAPTOR_HANDLE device;

    if ((ctx >= 0) && (ctx < MAX_CONTEXT))
	{
		context_t& context = pContext[ctx];
        device.handle = (unsigned int)context.display;
        device.hClass = DEVICE_CONTEXT_CLASS;
    }

    return device;
}

CContextManager::RENDERING_CONTEXT_ID CGLXContextManager::getContext(RAPTOR_HANDLE device) const
{
    RENDERING_CONTEXT_ID ctx = -1;
    if (device.handle == 0)
        return ctx;

    for (unsigned int i=0;i<MAX_CONTEXT;i++)
    {
        context_t& context = pContext[i];
        if ((device.hClass == WINDOW_CLASS) && (context.window == (Window)(device.handle)))
            ctx = i;
        else if (((unsigned int)context.OGLContext) == device.handle)
            ctx = i;
    }

    return ctx;
}

void CGLXContextManager::glMakeCurrentContext(const RAPTOR_HANDLE& device,RENDERING_CONTEXT_ID ctx)
{
	if ((ctx >= 0) && (ctx < MAX_CONTEXT))
	{
		context_t& context = pContext[ctx];
        if (context.window != 0)
        {
            if (device.handle != 0)
            {
                if (device.hClass == WINDOW_CLASS)
                {
                    CGLXView* wnd = (CGLXView*)(device.handle);
                    Display *display = context.display;
                    Window window = (Window)(wnd->getWindow().handle);
                    glXMakeCurrent(display, window, context.OGLContext);
                }
                else
                {
                    Display* display = (Display*)(device.handle);
                    glXMakeCurrent(display, context.window, context.OGLContext);
                }
            }
            else
                glXMakeCurrent(context.display, context.window, context.OGLContext);
        }
        else
        {
            cout << "No drawable for device" << endl;
        }

        m_currentContext = ctx;
	}
	else
	{
        //glXMakeCurrent(context.display, context.window, NULL);
		m_currentContext = -1;
	}
}

void CGLXContextManager::glDestroyContext(RENDERING_CONTEXT_ID ctx)
{
	if ((ctx >= 0) && (ctx < MAX_CONTEXT))
	{
		context_t& context = pContext[ctx];
		
        glXDestroyContext( context.display, context.OGLContext );

		context.OGLContext = NULL;
	}
}


void CGLXContextManager::glSwapBuffers(RENDERING_CONTEXT_ID ctx)
{
	if ((ctx >= 0) && (ctx < MAX_CONTEXT))
	{
		context_t& context = pContext[ctx];

#ifdef SHAREWARE_RELEASE
		if (Global::GetInstance().getCurrentStatus().runAsShareware)
		{
			//drawLogo(RAPTOR_LOGO);
		}
#endif
        glFlush();
		glXSwapBuffers( context.display, context.window );
	}
}

bool CGLXContextManager::glSwapVSync(unsigned int nbVSync) const
{
	bool swapControl = false;

#if defined(WGL_EXT_swap_control)
	if ((Raptor::glIsExtensionSupported("WGL_EXT_swap_control")) && 
				(pExtensions->wglSwapIntervalEXT != NULL))
	{
		swapControl = true;

		DEVMODE mode;
		memset(&mode,sizeof(DEVMODE),0);
		mode.dmSize = sizeof(DEVMODE);
		mode.dmDriverExtra = 0;
		mode.dmFields = DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT|DM_DISPLAYFLAGS|DM_DISPLAYFREQUENCY;

		if (0!=EnumDisplaySettings(NULL,ENUM_CURRENT_SETTINGS,&mode))
		{
			DWORD freq = mode.dmDisplayFrequency;

			if ( nbVSync > freq)
				pExtensions->wglSwapIntervalEXT(0);
			else
				pExtensions->wglSwapIntervalEXT( freq / nbVSync);
		}
		else
			swapControl = false;
	}
#endif

	return swapControl;
}

bool CGLXContextManager::glDestroyWindow(const RAPTOR_HANDLE& wnd)
{
    return false;
}

RAPTOR_HANDLE CGLXContextManager::glCreateWindow(const CRaptorDisplayConfig& pcs,CRaptorDisplay *& pDisplay,RENDERING_CONTEXT_ID &ctx)
{
	RAPTOR_HANDLE wnd;
	int width = pcs.width;
	int height = pcs.height;
    int top = pcs.y;
    int left = pcs.x;

	Raptor::SetDefaultDisplayConfig(pcs);

    if ((width <0) || (height <0))
    {
        width = 1;
        height = 1;
        left = 0;
        top = 0;
    }

	RAPTOR_HANDLE device;
	device.handle = (unsigned int)(pGlobalDisplay);

	RENDERING_CONTEXT_ID id;
    bool destroyDisplay = (pDisplay == NULL);
    
    if ((pcs.display_mode & CGL_GENERIC) == CGL_GENERIC)
    {
        id = glCreateContext(device,pcs.display_mode);
        pDisplay = NULL;
		ctx = id;
    }
    else
    {
        if (destroyDisplay)
            pDisplay = Raptor::glCreateDisplay(pcs);
        id = 0;
        if (!pDisplay->glBindDisplay(device))
            id = -1;
        else
            id = m_currentContext;
        if (!pDisplay->glUnBindDisplay())
            id = -1;
    }

	if (id > -1)
	{
        int screen = DefaultScreen(pGlobalDisplay);
        XVisualInfo *visual =  pContext[id].visual;

        unsigned long valuemask = CWBorderPixel | CWBackPixel | CWColormap;
        XSetWindowAttributes attributes;
        attributes.border_pixel = 0;
        attributes.background_pixel = 0;
        attributes.colormap = XCreateColormap(  pGlobalDisplay, 
                                                RootWindow(pGlobalDisplay, screen), 
                                                visual->visual, AllocNone);

        Window window = XCreateWindow(pGlobalDisplay, RootWindow(pGlobalDisplay, screen), 
                                      left, top, width, height, 
                                      0, visual->depth, CopyFromParent, visual->visual, 
                                      valuemask, &attributes);

        XSelectInput(pGlobalDisplay, window, 0xFFFF); 
        XStoreName(pGlobalDisplay, window, pcs.caption.data());
        XMapWindow(pGlobalDisplay, window);

        pContext[id].window = window;
        glMakeCurrentContext(device, id);
	    glInitExtensions();
	    glMakeCurrentContext(device, -1);
    
        CGLXView *w = new CGLXView();
        w->m_wnd.handle =  (unsigned int)(window);
        w->m_wnd.hClass = WINDOW_CLASS;
        w->m_pDisplay = pDisplay;

        wnd.handle = (unsigned int)(w);
        wnd.hClass = WINDOW_CLASS;
	}
    else
    {
        if (destroyDisplay)
		{
            Raptor::glDestroyDisplay(pDisplay);
            pDisplay = NULL;
        }

        wnd.handle = (unsigned int)(0);
        wnd.hClass = WINDOW_CLASS;
    }

	return wnd;
}

//	
//	Standard OpenGL Rendering Context creation method
//
CContextManager::RENDERING_CONTEXT_ID CGLXContextManager::glCreateContext(const RAPTOR_HANDLE& device,int displayMode,bool global)
{
	if (nbContext >= MAX_CONTEXT)
	{
		Raptor::GetErrorManager()->generateRaptorError(	Global::COpenGLClassID::GetClassId(),
                                                            CRaptorErrorManager::RAPTOR_ERROR,
										                    "Too many Rendering Context created");
		return -1;
	}

	int pixelformat;
	
	GLXContext	    defaultGLRC = NULL;
	GLXContext	    glhrc = NULL;
		
    int flags[256];
    int numFlags = 0;
	int m_mode = displayMode;
    
    flags[numFlags++] = GLX_RGBA;
    flags[numFlags++] = GLX_USE_GL;
    flags[numFlags++] = GLX_RED_SIZE;
    flags[numFlags++] = 8;
    flags[numFlags++] = GLX_GREEN_SIZE;
    flags[numFlags++] = 8;
    flags[numFlags++] = GLX_BLUE_SIZE;
    flags[numFlags++] = 8;
    if (m_mode & CGL_RGBA)
    {
        flags[numFlags++] = GLX_ALPHA_SIZE;
        flags[numFlags++] = 8;
    }

    
	if (m_mode & CGL_DOUBLE) flags[numFlags++] = GLX_DOUBLEBUFFER;

    flags[numFlags++] = GLX_DEPTH_SIZE;
	if ((m_mode & CGL_DEPTH_32) == CGL_DEPTH_32) flags[numFlags++] = 32;
	else if (m_mode & CGL_DEPTH_16) flags[numFlags++] = 16;
	else if (m_mode & CGL_DEPTH_24)	flags[numFlags++] = 24;
	else flags[numFlags++] = 16;

	if (m_mode & CGL_STENCIL)
    {
        flags[numFlags++] = GLX_STENCIL_SIZE;
        flags[numFlags++] = 8;
    }
	if (m_mode & CGL_ACCUM)
    {
        flags[numFlags++] = GLX_ACCUM_RED_SIZE;
        flags[numFlags++] = 8;
        flags[numFlags++] = GLX_ACCUM_GREEN_SIZE;
        flags[numFlags++] = 8;
        flags[numFlags++] = GLX_ACCUM_BLUE_SIZE;
        flags[numFlags++] = 8;
        if (m_mode & CGL_RGBA)
        {
            flags[numFlags++] = GLX_ACCUM_ALPHA_SIZE;
            flags[numFlags++] = 8;
        }
    }
    

    flags[numFlags++] = None;

    int screen = DefaultScreen(pGlobalDisplay);
    Display *display = (Display*)(device.handle);
    XVisualInfo *visual = glXChooseVisual(display, screen, flags);

	if ( visual == 0 ) 
	{
		Raptor::GetErrorManager()->generateRaptorError(Global::COpenGLClassID::GetClassId(),
                                                           CRaptorErrorManager::RAPTOR_FATAL,
                                                           "Raptor Context Manager failed to choose pixel format");
		return -1; 
	}

	defaultGLRC = glXCreateContext(display,visual,0,True);
	if (!defaultGLRC)
	{
		Raptor::GetErrorManager()->generateRaptorError(Global::COpenGLClassID::GetClassId(),
                                                           CRaptorErrorManager::RAPTOR_FATAL,
                                                           "Raptor Context Manager failed to create OpenGL context");
		return -1;
	}

	if (global)
	{
		if (glGlobalRC == NULL)
			glGlobalRC = defaultGLRC;
		glhrc = glGlobalRC;
	}
	else
	{	
		glhrc = defaultGLRC;
	}

	//glXMakeCurrent(pGlobalDisplay, glhrc);
	//glInitExtensions();
	//glXMakeCurrent(pGlobalDisplay, NULL);

	unsigned int pos = 0;
	while ((pos < nbContext) && (pContext[pos].OGLContext != NULL))
		pos++;

	pContext[pos].OGLContext = glhrc;
	pContext[pos].display = display;
	pContext[pos].window = (Window)0;
    pContext[pos].visual = visual;
	nbContext++;

	return pos;
}



//	
//	Extended OpenGL Rendering Context creation method
//	This rendering context cannot be created during
//	window creation because the rendering context
//	cannot be set ( dc not fully initialised ? )
//
CContextManager::RENDERING_CONTEXT_ID  CGLXContextManager::glCreateExtendedContext(const RAPTOR_HANDLE& device,int displayMode,bool global)
{
	if (nbContext >= MAX_CONTEXT)
	{
		Raptor::GetErrorManager()->generateRaptorError(Global::COpenGLClassID::GetClassId(),
                                                            CRaptorErrorManager::RAPTOR_ERROR,
										                    "Too many Rendering Context created");
		return -1;
	}

    return glCreateContext(device,displayMode,global);
}



#ifdef WGL_ARB_pbuffer
	CContextManager::PIXEL_BUFFER_ID CGLXContextManager::glCreatePBuffer(const CRaptorDisplayConfig& pcs)
	{
		if (nbPBuffers >= MAX_PBUFFERS)
		{
			Raptor::GetErrorManager()->generateRaptorError(Global::COpenGLClassID::GetClassId(),
                                                                                           CRaptorErrorManager::RAPTOR_ERROR,
                                                                                           "Too many Pixel Buffers created");
			return 0;
		}

		if ((pExtensions->wglCreatePBufferARB == NULL) || 
			(pExtensions->wglChoosePixelFormatARB == NULL) || 
			(pExtensions->wglGetPBufferDCARB == NULL))
		{
			Raptor::GetErrorManager()->generateRaptorError(Global::COpenGLClassID::GetClassId(),
                                                                                           CRaptorErrorManager::RAPTOR_FATAL,
                                                                                           "Your driver or GPU does not support Pixel Buffers");
			return 0;
		}

		HDC hdc = wglGetCurrentDC();

		// alloc pixel format descriptor
		int *piAttribIList = new int[2 * 20];
		memset(piAttribIList,0,2*20*sizeof(int));
		int m_mode = pcs.display_mode;
		unsigned int attribIndex = 0;

		//	draw to P Buffer
		piAttribIList[attribIndex++] = WGL_DRAW_TO_PBUFFER_ARB; piAttribIList[attribIndex++] = TRUE;

		//	All this stuff .... is for OpenGL of course !!!
		//	OGL acceleration
		DEFINE_ACCELERATION
		
		// P Buffer do not support double buffering. Maybe future versions will do so.
		//	Draw with double buffer
		DEFINE_DOUBLE_BUFFER

		//	RGB or RGBA are the only supported modes, because they are fun.
		//	Paletted mode are so boring and slow and old fashioned ...
		DEFINE_PIXELS
		
		//	Use depth if requested
		DEFINE_DEPTH_BUFFER

		//	Use stencil if requested
		DEFINE_STENCIL_BUFFER

		//	Use accum buffer if requested
		DEFINE_ACCUM_BUFFER

		//	Use antialiasing if requested
		DEFINE_SAMPLE_BUFFER

#ifdef WGL_ARB_render_texture
		if ((m_mode & CGL_RENDER_TEXTURE) == CGL_RENDER_TEXTURE)
		{
			piAttribIList[attribIndex++] = WGL_BIND_TO_TEXTURE_RGBA_ARB;
			piAttribIList[attribIndex++] = TRUE;

		#ifdef WGL_NV_render_depth_texture
			if ((m_mode & CGL_RENDER_DEPTHTEXTURE) == CGL_RENDER_DEPTHTEXTURE)
			{
				piAttribIList[attribIndex++] = WGL_BIND_TO_TEXTURE_DEPTH_NV;
				piAttribIList[attribIndex++] = TRUE;
			}
		#endif

			if ((pExtensions->wglBindTexImageARB == NULL) || 
				(pExtensions->wglReleaseTexImageARB == NULL) || 
				(pExtensions->wglSetPbufferAttribARB == NULL))
			{
				Raptor::GetErrorManager()->generateRaptorError(Global::COpenGLClassID::GetClassId(),
                                                                                               CRaptorErrorManager::RAPTOR_FATAL,
                                                                                               "Your driver or GPU does not support texture rendering");
				return 0;
			}
		}
#endif

		//	Terminate the list and continue with the settings
		UINT nNumFormats = 0;
		int pixelformat;
		if (( pExtensions->wglChoosePixelFormatARB(hdc, piAttribIList,NULL,1,&pixelformat,&nNumFormats) == 0 ) || (nNumFormats == 0))
		{
			Raptor::GetErrorManager()->generateRaptorError(Global::COpenGLClassID::GetClassId(),
                                                                                           CRaptorErrorManager::RAPTOR_FATAL,
                                                                                           "Raptor failed to choose pixel format for PBuffer");

			delete [] piAttribIList;
			return 0; 
		}

		int pBufferAttribs[10];
		memset(pBufferAttribs,0,10*sizeof(int));
#ifdef WGL_ARB_render_texture
		if ((m_mode & CGL_RENDER_TEXTURE) == CGL_RENDER_TEXTURE)
		{
			int idx = 0;
			pBufferAttribs[idx++] = WGL_TEXTURE_FORMAT_ARB;
			pBufferAttribs[idx++] = WGL_TEXTURE_RGBA_ARB;
			pBufferAttribs[idx++] = WGL_TEXTURE_TARGET_ARB;
			pBufferAttribs[idx++] = WGL_TEXTURE_2D_ARB;
			//pBufferAttribs[idx++] = WGL_MIPMAP_TEXTURE_ARB;
			//pBufferAttribs[idx++] = TRUE;

		#ifdef WGL_NV_render_depth_texture
			if ((m_mode & CGL_RENDER_DEPTHTEXTURE) == CGL_RENDER_DEPTHTEXTURE)
			{
				pBufferAttribs[idx++] = WGL_DEPTH_TEXTURE_FORMAT_NV;
				pBufferAttribs[idx++] = WGL_TEXTURE_DEPTH_COMPONENT_NV;
			}
		#endif
		}
#endif

		HPBUFFERARB hpbuf = pExtensions->wglCreatePBufferARB(hdc,pixelformat,pcs.width,pcs.height,pBufferAttribs);
		if (hpbuf == NULL)
		{
			Raptor::GetErrorManager()->generateRaptorError(Global::COpenGLClassID::GetClassId(),
                                                                                           CRaptorErrorManager::RAPTOR_FATAL,
                                                                                           "Raptor failed to create PBuffer");
			return 0;
		}

		HDC hpbufdc = pExtensions->wglGetPBufferDCARB(hpbuf);
		if (!hpbufdc)
		{
			Raptor::GetErrorManager()->generateRaptorError(Global::COpenGLClassID::GetClassId(),
                                                                                           CRaptorErrorManager::RAPTOR_FATAL,
                                                                                           "Raptor failed to create PBuffer device context");
			return 0;
		}

		HGLRC hpbufglhrc = wglCreateContext(hpbufdc);
		if (!hpbufglhrc)
		{
			Raptor::GetErrorManager()->generateRaptorError(Global::COpenGLClassID::GetClassId(),
                                                                                           CRaptorErrorManager::RAPTOR_FATAL,
                                                                                           "Raptor failed to create PBuffer OpenGL context");
			return 0;
		}
		
		delete [] piAttribIList;

		unsigned int pos = 0;
		while ((pos < nbPBuffers) && (pBuffers[pos].pbuffer != NULL))
			pos++;

		pBuffers[pos].pbuffer = hpbuf;
		pBuffers[pos].pbufferDC = hpbufdc;
		pBuffers[pos].pbufferGLRC = hpbufglhrc;
		nbPBuffers++;

		
		BOOL res = wglShareLists(wglGetCurrentContext(),hpbufglhrc);
#ifdef RAPTOR_DEBUG_MODE_GENERATION
		if (res == FALSE)
		{
			Raptor::GetErrorManager()->generateRaptorError(	Global::COpenGLClassID::GetClassId(),
                                                                                            CRaptorErrorManager::RAPTOR_WARNING,
											                                                "Unable to share PBuffer display lists");
		}
#endif

		return pos;
	}

	bool CGLXContextManager::glDestroyPBuffer(PIXEL_BUFFER_ID pbuffer)
	{
		if (pbuffer > MAX_PBUFFERS)
		{
			Raptor::GetErrorManager()->generateRaptorError(Global::COpenGLClassID::GetClassId(),
                                                                                           CRaptorErrorManager::RAPTOR_ERROR,
                                                                                           "Exceeded max number of pBuffers");
			return false;
		}

		if (pBuffers[pbuffer].pbuffer == NULL)
			return false;

		if (FALSE == wglDeleteContext(pBuffers[pbuffer].pbufferGLRC))
		{
			Raptor::GetErrorManager()->generateRaptorError(Global::COpenGLClassID::GetClassId(),
                                                                                           CRaptorErrorManager::RAPTOR_FATAL,
                                                                                           "Raptor failed to destroy PBuffer OpenGL context");
			return false;
		}

		if (0 == pExtensions->wglReleasePBufferDCARB(pBuffers[pbuffer].pbuffer,pBuffers[pbuffer].pbufferDC))
		{
			Raptor::GetErrorManager()->generateRaptorError(Global::COpenGLClassID::GetClassId(),
                                                                                           CRaptorErrorManager::RAPTOR_FATAL,
                                                                                           "Raptor failed to release PBuffer device context");
			return false;
		}

		if (0 == pExtensions->wglDestroyPBufferARB(pBuffers[pbuffer].pbuffer))
		{
			Raptor::GetErrorManager()->generateRaptorError(Global::COpenGLClassID::GetClassId(),
                                                                                           CRaptorErrorManager::RAPTOR_FATAL,
                                                                                           "Raptor failed to destroy PBuffer");
			return false;
		}

		pBuffers[pbuffer].pbuffer = NULL;
		pBuffers[pbuffer].pbufferDC = NULL;
		pBuffers[pbuffer].pbufferGLRC = NULL;
		nbPBuffers--;

		return true;
	}

	void CGLXContextManager::glBindPBuffer(PIXEL_BUFFER_ID pbuffer)
	{
		if (pBuffers[pbuffer].pbuffer == NULL)
		{
			string str = "Raptor pBuffer ";
            char numbuffer[8];
            sprintf(numbuffer,"%d",pbuffer+1);
            str += numbuffer;
            str += " does not exist";
			Raptor::GetErrorManager()->generateRaptorError(Global::COpenGLClassID::GetClassId(),
                                                                                           CRaptorErrorManager::RAPTOR_ERROR,str);
			return;
		}
  
		wglMakeCurrent(pBuffers[pbuffer].pbufferDC,pBuffers[pbuffer].pbufferGLRC);
	}

	bool CGLXContextManager::glIsPBuffer(PIXEL_BUFFER_ID pbuffer) const
	{
		return (pBuffers[pbuffer].pbuffer != NULL);
	}

	bool CGLXContextManager::glBindTexImageARB(PIXEL_BUFFER_ID pbuffer,int iBuffer)
	{
		if (pbuffer > MAX_PBUFFERS)
		{
			Raptor::GetErrorManager()->generateRaptorError(Global::COpenGLClassID::GetClassId(),
                                                                                           CRaptorErrorManager::RAPTOR_ERROR,
                                                                                           "Exceeded max number of pBuffers");
			return false;
		}

		if ((pBuffers[pbuffer].pbuffer == NULL) ||
			(pExtensions->wglBindTexImageARB == NULL))
		{
			string str ="Raptor pBuffer ";
            char numbuffer[8];
            sprintf(numbuffer,"%d",pbuffer+1);
            str += numbuffer;
            str += " does not exist or texture rendering not supported";
			Raptor::GetErrorManager()->generateRaptorError(Global::COpenGLClassID::GetClassId(),
                                                                                           CRaptorErrorManager::RAPTOR_ERROR,str);
			return false;
		}
/*
		int iValue = 0;
		BOOL res = wglQueryPBufferARB(pBuffers[pbuffer].pbuffer,WGL_PBUFFER_WIDTH_ARB,&iValue);
		res = wglQueryPBufferARB(pBuffers[pbuffer].pbuffer,WGL_PBUFFER_HEIGHT_ARB,&iValue);
		res = wglQueryPBufferARB(pBuffers[pbuffer].pbuffer,WGL_PBUFFER_LOST_ARB,&iValue);
		res = wglQueryPBufferARB(pBuffers[pbuffer].pbuffer,WGL_TEXTURE_TARGET_ARB,&iValue);
		res = wglQueryPBufferARB(pBuffers[pbuffer].pbuffer,WGL_MIPMAP_TEXTURE_ARB,&iValue);
		res = wglQueryPBufferARB(pBuffers[pbuffer].pbuffer,WGL_MIPMAP_LEVEL_ARB,&iValue);
		res = wglQueryPBufferARB(pBuffers[pbuffer].pbuffer,WGL_CUBE_MAP_FACE_ARB,&iValue);
		res = wglQueryPBufferARB(pBuffers[pbuffer].pbuffer,WGL_TEXTURE_FORMAT_ARB,&iValue);

		int attribs[3];
		attribs[0] = WGL_MIPMAP_LEVEL_ARB;
		attribs[1] = 0;
		attribs[2] = 0;
		wglSetPbufferAttribARB(pBuffers[pbuffer].pbuffer,attribs);
*/
		return (FALSE != pExtensions->wglBindTexImageARB(pBuffers[pbuffer].pbuffer,iBuffer));
	}

	bool CGLXContextManager::glReleaseTexImageARB(PIXEL_BUFFER_ID pbuffer,int iBuffer)
	{
		if (pbuffer > MAX_PBUFFERS)
		{
			Raptor::GetErrorManager()->generateRaptorError(Global::COpenGLClassID::GetClassId(),
                                                                                           CRaptorErrorManager::RAPTOR_ERROR,
                                                                                           "Exceeded max number of pBuffers");
			return false;
		}

		if ((pBuffers[pbuffer].pbuffer == NULL) ||
			(pExtensions->wglReleaseTexImageARB == NULL))
		{
			string str = "Raptor pBuffer ";
            char numbuffer[8];
            sprintf(numbuffer,"%d",pbuffer+1);
            str += numbuffer;
            str += " does not exist or texture rendering not supported";
			Raptor::GetErrorManager()->generateRaptorError(Global::COpenGLClassID::GetClassId(),
                                                                                           CRaptorErrorManager::RAPTOR_ERROR,str);
			return false;
		}

		return (FALSE != pExtensions->wglReleaseTexImageARB(pBuffers[pbuffer].pbuffer,iBuffer));
	}

#else
	CContextManager::PIXEL_BUFFER_ID CGLXContextManager::glCreatePBuffer(const CRaptorDisplayConfig& ) { return 0; }
	bool CGLXContextManager::glDestroyPBuffer(PIXEL_BUFFER_ID ) { return false; };
	void CGLXContextManager::glBindPBuffer(PIXEL_BUFFER_ID, unsigned int ) { };
	bool CGLXContextManager::glIsPBuffer(PIXEL_BUFFER_ID ) const { return false; };
	bool CGLXContextManager::glBindTexImageARB(PIXEL_BUFFER_ID ,int ) { return false; };
	bool CGLXContextManager::glReleaseTexImageARB(PIXEL_BUFFER_ID ,int ) { return false; };
#endif


