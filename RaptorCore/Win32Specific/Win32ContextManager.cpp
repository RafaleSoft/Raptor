// Win32ContextManager.cpp: implementation of the CWin32ContextManager class.
//
//////////////////////////////////////////////////////////////////////
#include "Subsys/CodeGeneration.h"

#if !defined(AFX_WIN32CONTEXTMANAGER_H__A1D82397_7E92_4D01_A04D_782BCFD17689__INCLUDED_)
	#include "Win32ContextManager.h"
#endif
#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
	#include "System/Raptor.h"
#endif
#if !defined(AFX_RAPTORERRORMANAGER_H__FA5A36CD_56BC_4AA1_A5F4_451734AD395E__INCLUDED_)
	#include "System/RaptorErrorManager.h"
#endif
#if !defined(AFX_RAPTORGLEXTENSIONS_H__E5B5A1D9_60F8_4E20_B4E1_8E5A9CB7E0EB__INCLUDED_)
	#include "System/RaptorGLExtensions.h"
#endif
#if !defined(AFX_RAPTORVKEXTENSIONS_H__B17D6B7F_5AFC_4E34_9D49_8DC6CE9192D6__INCLUDED_)
	#include "System/RaptorVKExtensions.h"
#endif
#if !defined(AFX_RAPTORVULKANSURFACE_H__C377C267_32A8_4963_BC2A_4694F4299A68__INCLUDED_)
	#include "Subsys/Vulkan/VulkanSurface.h"
#endif
#if !defined(AFX_WIN32APPLICATION_H__3EADD210_ABF5_4CFD_A511_09047EDBB881__INCLUDED_)
    #include "Win32Application.h"
#endif
#if !defined(AFX_RAPTORINSTANCE_H__90219068_202B_46C2_BFF0_73C24D048903__INCLUDED_)
	#include "Subsys/RaptorInstance.h"
#endif
#if !defined(AFX_OPENGL_H__6C8840CA_BEFA_41DE_9879_5777FBBA7147__INCLUDED_)
	#include "Subsys/OpenGL/RaptorOpenGL.h"
#endif
#if !defined(AFX_VULKAN_H__625F6BC5_F386_44C2_85C1_EDBA23B16921__INCLUDED_)
	#include "Subsys/Vulkan/RaptorVulkan.h"
#endif


#include <strstream>


RAPTOR_NAMESPACE_BEGIN


static const int MAX_PBUFFERS = 32;
static const int MAX_CONTEXT = 16;

//////////////////////////////////////////////////////////////////////
//
unsigned int defineAcceleration(unsigned int index,
								CRaptorDisplayConfig::GL_ACCELERATION acceleration,
								int *attribs)
{
    unsigned int attribIndex = index;
#if defined(WGL_ARB_pixel_format)
    attribs[attribIndex++] = WGL_ACCELERATION_ARB;

	if (CRaptorDisplayConfig::SOFTWARE == acceleration)
        attribs[attribIndex++] = WGL_NO_ACCELERATION_ARB;
    else if (CRaptorDisplayConfig::GENERIC == acceleration)
        attribs[attribIndex++] = WGL_GENERIC_ACCELERATION_ARB;
    else if (CRaptorDisplayConfig::HARDWARE == acceleration) 
        attribs[attribIndex++] = WGL_FULL_ACCELERATION_ARB;
    else attribs[attribIndex++] = WGL_FULL_ACCELERATION_ARB;

    attribs[attribIndex++] = WGL_SUPPORT_OPENGL_ARB; 
    attribs[attribIndex++] = TRUE;
#endif
    return attribIndex;
}

//! It seems that float 32bits rgba rendering to texture is not properly supported ( veeeery slooow !?! )
//! Indeed half float pixels is enough for HDR, full float 32bits flitering is applyied and stored in a 16 bits target.
unsigned int definePixels(unsigned int index,int mode,int *attribs)
{
    unsigned int attribIndex = index;
#if defined(WGL_ARB_pixel_format)
    attribs[attribIndex++] = WGL_PIXEL_TYPE_ARB;
#if defined(GL_ARB_color_buffer_float) || defined(WGL_ATI_pixel_format_float)
    if ((mode & CGL_FLOAT) == CGL_FLOAT)
    {
		//!	Default to 32 bits, PBuffer pixel format is poorly supported
		//! with FLOAT_16 pixel type (since nVidia 375.86)
        unsigned int bits = 32;
        if ((mode & CGL_FLOAT_16) == CGL_FLOAT_16)
            bits = 16;
        else if ((mode & CGL_FLOAT_32) == CGL_FLOAT_32)
            bits = 32;

#if defined(GL_ARB_color_buffer_float)
		attribs[attribIndex++] = WGL_TYPE_RGBA_FLOAT_ARB;
#elif defined(WGL_ATI_pixel_format_float)
        attribs[attribIndex++] = WGL_TYPE_RGBA_FLOAT_ATI; // same value as above, but may be change
#endif
		attribs[attribIndex++] = WGL_COLOR_BITS_ARB;
		attribs[attribIndex++] = bits * 3;
        attribs[attribIndex++] = WGL_RED_BITS_ARB; 
        attribs[attribIndex++] = bits;
        attribs[attribIndex++] = WGL_GREEN_BITS_ARB; 
        attribs[attribIndex++] = bits;
        attribs[attribIndex++] = WGL_BLUE_BITS_ARB; 
        attribs[attribIndex++] = bits;
        attribs[attribIndex++] = WGL_ALPHA_BITS_ARB;
        if (mode & CGL_RGBA) 
            attribs[attribIndex++] = bits; 
        else 
            attribs[attribIndex++] = 0;
    }
    else
#endif
    {
        attribs[attribIndex++] = WGL_TYPE_RGBA_ARB;
        attribs[attribIndex++] = WGL_COLOR_BITS_ARB; 
        attribs[attribIndex++] = 24;
        attribs[attribIndex++] = WGL_RED_BITS_ARB; 
        attribs[attribIndex++] = 8;
        attribs[attribIndex++] = WGL_GREEN_BITS_ARB; 
        attribs[attribIndex++] = 8;
        attribs[attribIndex++] = WGL_BLUE_BITS_ARB; 
        attribs[attribIndex++] = 8;
        attribs[attribIndex++] = WGL_ALPHA_BITS_ARB;
        if (mode & CGL_RGBA) 
            attribs[attribIndex++] = 8; 
        else 
            attribs[attribIndex++] = 0; 
    }
#endif
    return attribIndex;
}

unsigned int defineDoubleBuffer(unsigned int index,
								bool double_buffer,
								CRaptorDisplayConfig::GL_SWAPBUFFER swap_buffer,
								int *attribs)
{
    unsigned int attribIndex = index;
#if defined(WGL_ARB_pixel_format)
    if (double_buffer)
    {
	    attribs[attribIndex++] = WGL_DOUBLE_BUFFER_ARB;
	    attribs[attribIndex++] = TRUE;
		
		if (CRaptorDisplayConfig::SWAP_COPY == swap_buffer)
        {
	        attribs[attribIndex++] = WGL_SWAP_METHOD_ARB;
	        attribs[attribIndex++] = WGL_SWAP_COPY_ARB;
        }
        else if (CRaptorDisplayConfig::SWAP_EXCHANGE == swap_buffer)
        {
	        attribs[attribIndex++] = WGL_SWAP_METHOD_ARB;
	        attribs[attribIndex++] = WGL_SWAP_EXCHANGE_ARB;
        }
        else if (CRaptorDisplayConfig::SWAP_UNDEFINED == swap_buffer)
        {
		//!	This swap model does not work (no pixel format available)
	    //   attribs[attribIndex++] = WGL_SWAP_METHOD_ARB;
	    //   attribs[attribIndex++] = WGL_SWAP_UNDEFINED_ARB;
        }
    }
#endif
    return attribIndex;
}

unsigned int defineDepthBuffer(unsigned int index,int mode,int *attribs)
{
    unsigned int attribIndex = index;
#if defined(WGL_ARB_pixel_format)
    if ((mode & CGL_DEPTH_32) == CGL_DEPTH_32)
    {
	    attribs[attribIndex++] = WGL_DEPTH_BITS_ARB;
	    attribs[attribIndex++]= 32;
    }
    else if ((mode & CGL_DEPTH_16) == CGL_DEPTH_16)
    {
	    attribs[attribIndex++] = WGL_DEPTH_BITS_ARB;
	    attribs[attribIndex++] =  16;
    }
    else if ((mode & CGL_DEPTH_24) == CGL_DEPTH_24)
    {
	    attribs[attribIndex++] = WGL_DEPTH_BITS_ARB;
	    attribs[attribIndex++] = 24;
    }
#endif
    return attribIndex;
}

unsigned int defineStencilBuffer(unsigned int index,bool stencil,int *attribs)
{
    unsigned int attribIndex = index;
#if defined(WGL_ARB_pixel_format)
    if (stencil)
    {
	    attribs[attribIndex++] = WGL_STENCIL_BITS_ARB;
	    attribs[attribIndex++] = 8;
    }
#endif
    return attribIndex;
}

unsigned int defineAccumBuffer(unsigned int index,bool accumulator_buffer,int *attribs)
{
    unsigned int attribIndex = index;
#if defined(WGL_ARB_pixel_format)
    if (accumulator_buffer)
    {
	    attribs[attribIndex++] = WGL_ACCUM_BITS_ARB;
	    attribs[attribIndex++] = 64;
    }
#endif
    return attribIndex;
}

unsigned int defineSampleBuffer(unsigned int index,unsigned int nbSamples,int *attribs)
{
    unsigned int attribIndex = index;
#if defined(WGL_ARB_pixel_format)
#if defined(GL_ARB_multisample)
	if (1 < nbSamples)
	{
		attribs[attribIndex++] = WGL_SAMPLE_BUFFERS_ARB;
		attribs[attribIndex++] = 1;
		attribs[attribIndex++] = WGL_SAMPLES_ARB;
		attribs[attribIndex++] = nbSamples;
	}
#endif
#endif
    return attribIndex;
}

RAPTOR_NAMESPACE_END


RAPTOR_NAMESPACE

#if defined RAPTOR_DEBUG_MODE_GENERATION
void CWin32ContextManager::getLastError(const std::string& file,int line) const
{
    DWORD err = ::GetLastError();
    if (err != ERROR_SUCCESS)
    {
        ostrstream r_line;
        r_line << " [line:" << line  << "] " << ends;
        string::size_type pos1 = file.rfind('\\');
        string::size_type pos2 = file.rfind('.');
	    string r_file = file.substr(pos1+1,pos2-pos1-1);

        string msgStr = r_line.str();
        switch(err)
        {
            case ERROR_INVALID_HANDLE:
                msgStr = "Invalid Handle ( class " + r_file + r_line.str() + ")";
                break;
            case ERROR_INVALID_DATA:
                msgStr = "Invalid Data ( class " + r_file + r_line.str() + ")";
                break;
            case ERROR_DC_NOT_FOUND:
                msgStr = "DC not found ( class " + r_file + r_line.str() + ")";
                break;
            case ERROR_INVALID_PIXEL_FORMAT:
                msgStr = "Invalid Pixel Format ( class " + r_file + r_line.str() + ")";
                break;
            case ERROR_NO_SYSTEM_RESOURCES:
                msgStr = "No System Resources ( class " + r_file + r_line.str() + ")";
                break;
            case ERROR_NOT_ENOUGH_MEMORY:
				msgStr = "Not enough memory ( class " + r_file + r_line.str() + ")";
                break;
            default:
                msgStr = "Unknown error ( class " + r_file + r_line.str() + ")";
                break;
        }

		r_line.rdbuf()->freeze(0);
		RAPTOR_WARNING(COpenGL::COpenGLClassID::GetClassId(),msgStr);
        SetLastError(0);
    }
}
#endif
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CWin32ContextManager::CWin32ContextManager()
{
	nbPBuffers = 0;
	nbContext = 0;

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
        pContext[i].WIN32Context = NULL;
		pContext[i].WIN32Window = NULL;
		pContext[i].OGLContext = 0;
        pContext[i].pExtensions = NULL;
	}
}

CWin32ContextManager::~CWin32ContextManager()
{
    //	delete pExtensions;
    for (unsigned int i=0;i<MAX_CONTEXT;i++)
	{
        if (pContext[i].pExtensions != NULL)
            delete pContext[i].pExtensions;
	}
    delete [] pContext;

#if defined(WGL_ARB_pbuffer)
    delete [] pBuffers;
#endif
}

const CRaptorGLExtensions *const CWin32ContextManager::glGetExtensions(void)
{
	if ((m_currentGLContext >= 0) && (m_currentGLContext < MAX_CONTEXT))
		return pContext[m_currentGLContext].pExtensions;
    else
   //! If the current context is not valid, try at least to return the first context,
   //!  which should be initialized by Raptor.  
	    return pContext[0].pExtensions;
}

bool CWin32ContextManager::validateConfig(CRaptorDisplayConfig& rdc)
{
	bool res = CContextManager::validateConfig(rdc);

	int xmax = GetSystemMetrics(SM_CXVIRTUALSCREEN);
	int ymax = GetSystemMetrics(SM_CYVIRTUALSCREEN);

	//	validates window position to be fully visible
	//	if window not fully visible, hardware very slow...
	if (rdc.height + rdc.y>ymax)
	{
		rdc.height = ymax - rdc.y;
		res = false;
	}

	if (rdc.width + rdc.x > xmax)
	{
		rdc.width = xmax - rdc.x;
		res = false;
	}

	return res;
}

RAPTOR_HANDLE CWin32ContextManager::getDevice(CContextManager::RENDERING_CONTEXT_ID ctx) const
{
    RAPTOR_HANDLE device;

    if ((ctx >= 0) && (ctx < MAX_CONTEXT))
	{
		context_t& context = pContext[ctx];
        device.ptr(context.WIN32Context);
        device.hClass(DEVICE_CONTEXT_CLASS);
    }

    return device;
}

CContextManager::RENDERING_CONTEXT_ID CWin32ContextManager::getContext(RAPTOR_HANDLE device) const
{
	RENDERING_CONTEXT_ID ctx = CContextManager::INVALID_CONTEXT;
    if (device.handle() == 0)
        return ctx;

    for (unsigned int i=0;i<MAX_CONTEXT;i++)
    {
        context_t& context = pContext[i];
        if ((device.hClass() == WINDOW_CLASS) && (context.WIN32Window == device.ptr<HWND__>()))
            ctx = i;
        else if (context.WIN32Context == device.ptr<HDC__>())
            ctx = i;
    }

    return ctx;
}

void CWin32ContextManager::glMakeCurrentContext(const RAPTOR_HANDLE& device,RENDERING_CONTEXT_ID ctx)
{
	if ((ctx >= 0) && (ctx < MAX_CONTEXT))
	{
		context_t& context = pContext[ctx];
		if (device.hClass() != NULL)
		{
			if ((device.hClass() == WINDOW_CLASS) && (device.handle() != NULL))
			{
				context.WIN32Window = device.ptr<HWND__>();
                
				// Is this better ? yes in case the context does not exist (e.g. windows 7 or iGPU)
				context.WIN32Context = GetDC(context.WIN32Window);
				//context.WIN32Context = BeginPaint(context.WIN32Window,&context.WIN32Paint);
			}
			else if ((device.hClass() == DEVICE_CONTEXT_CLASS) && (device.handle() != NULL))
			{
                HDC hDevice = device.ptr<HDC__>();
                context.WIN32Context = hDevice;
			}
            else if ((device.hClass() == DIB_CLASS) && (device.handle() != NULL))
			{
                HDC hDevice = device.ptr<HDC__>();
                context.WIN32Context = hDevice;
			}
            if (context.WIN32Context != NULL)
                wglMakeCurrent(context.WIN32Context, context.OGLContext);
			m_currentGLContext = ctx;
		}
		else
		{
            if (context.WIN32Context != NULL)
			{
                wglMakeCurrent(context.WIN32Context, NULL);
				if (context.WIN32Window != NULL)
				// Is this better ? yes in case the context does not exist (e.g. windows 7 or iGPU)
					ReleaseDC(context.WIN32Window,context.WIN32Context);
				//EndPaint(context.WIN32Window,&context.WIN32Paint);
			}
			context.WIN32Window = NULL;
            context.WIN32Context = NULL;
			m_currentGLContext = CContextManager::INVALID_CONTEXT;
		}
	}
	else
	{
		wglMakeCurrent(NULL,NULL);
		m_currentGLContext = CContextManager::INVALID_CONTEXT;
	}
    CATCH_WIN32_ERROR
}

void CWin32ContextManager::glDestroyContext(RENDERING_CONTEXT_ID ctx)
{
	if ((ctx >= 0) && (ctx < MAX_CONTEXT))
	{
		context_t& context = pContext[ctx];
		
		wglDeleteContext(context.OGLContext);

        if (context.WIN32Window != NULL)
            ReleaseDC(context.WIN32Window,context.WIN32Context);
		context.WIN32Window = NULL;
        context.WIN32Context = NULL;
		context.OGLContext = NULL;
		
        if (context.pExtensions != NULL)
		    delete context.pExtensions;
		context.pExtensions = NULL;

		nbContext--;

        CATCH_WIN32_ERROR
	}
}

void CWin32ContextManager::glSwapBuffers(RENDERING_CONTEXT_ID ctx)
{
	if ((ctx >= 0) && (ctx < MAX_CONTEXT))
	{
		context_t& context = pContext[ctx];
        wglSwapLayerBuffers(context.WIN32Context,WGL_SWAP_MAIN_PLANE);

        CATCH_WIN32_ERROR
	}
}

bool CWin32ContextManager::glSwapVSync(unsigned int nbVSync) const
{
	bool swapControl = false;

	if ((m_currentGLContext >= 0) && (m_currentGLContext < MAX_CONTEXT))
	{ 
		context_t &context = pContext[m_currentGLContext];
        
#if defined(WGL_EXT_swap_control)
		if ((context.pExtensions->glIsExtensionSupported(WGL_EXT_SWAP_CONTROL_EXTENSION_NAME)) &&
			(context.pExtensions->wglSwapIntervalEXT != NULL))
	    {
		    swapControl = true;

		    DEVMODE mode;
		    memset(&mode,sizeof(DEVMODE),0);
		    mode.dmSize = sizeof(DEVMODE);
		    mode.dmDriverExtra = 0;
		    mode.dmFields = DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT|DM_DISPLAYFLAGS|DM_DISPLAYFREQUENCY;

		    if (0 != EnumDisplaySettings(NULL,ENUM_CURRENT_SETTINGS,&mode))
		    {
			    DWORD freq = mode.dmDisplayFrequency;

			    if ( nbVSync > freq)
				    context.pExtensions->wglSwapIntervalEXT(0);
#if defined(WGL_EXT_swap_control_tear)
			    else if ((nbVSync == 0) &&
						 (context.pExtensions->glIsExtensionSupported(WGL_EXT_SWAP_CONTROL_TEAR_EXTENSION_NAME)))
					context.pExtensions->wglSwapIntervalEXT(-1);
#endif
				else if (nbVSync > 0)
				    context.pExtensions->wglSwapIntervalEXT(freq / nbVSync);
				else
					context.pExtensions->wglSwapIntervalEXT(0);
		    }
		    else
			    swapControl = false;

            CATCH_WIN32_ERROR
	    }
#endif
    }

	return swapControl;
}


RAPTOR_HANDLE CWin32ContextManager::glCreateWindow(const CRaptorDisplayConfig& c_pda,CRaptorDisplay *&pDisplay,RENDERING_CONTEXT_ID &ctx)
{
	RAPTOR_HANDLE wnd;
	
    //  Invalid values are checked here, RaptorDisplay creation will succeed 
    // with invalid rect dimensions because they are set to minimum rect (0,0,1,1)
    // The trick for invisible windows can be used but is subject to change in future versions.
	CRaptorDisplayConfig pda = c_pda;
	validateConfig(pda);

    WNDCLASSEX   winclass;	// this will hold the class we create
    HWND	     hwnd;		// generic window handle

	winclass.cbSize = sizeof(WNDCLASSEX);
    if (FALSE == GetClassInfoEx(GetModuleHandle(NULL),"RaptorWindow",&winclass))
    {
        // first fill in the window class stucture
        winclass.cbSize = sizeof(WNDCLASSEX);
        winclass.style	= CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
        winclass.lpfnWndProc	= CWin32Application::getWindowProc();//set event handler
        winclass.cbClsExtra	= 0;									//reserve data space
        winclass.cbWndExtra	= 0;									//
        winclass.hInstance	= GetModuleHandle(NULL);				//set instance of application
        winclass.hIcon		= LoadIcon(NULL, IDI_APPLICATION);		//load program icon
        winclass.hCursor	= LoadCursor(NULL, IDC_ARROW);			//load cursor type
        winclass.hbrBackground	= NULL;								//set background brush
        winclass.lpszMenuName	= NULL;								//arrach resource menu
        winclass.lpszClassName	= "RaptorWindow";                   //set Windows class name
        winclass.hIconSm        = LoadIcon(NULL, IDI_APPLICATION);

        // register the window class
        if (!RegisterClassEx(&winclass))
	        return wnd;
    }


	int width = pda.width;
	int height = pda.height;

    RECT rect;
    UINT mode = 0;
    if ((width <0) || (height <0))
    {
        rect.right = 1;
        rect.bottom = 1;
        rect.left = 0;
        rect.top = 0;
        mode = WS_POPUP;
    }
    else
    {
		int padding = 2 * GetSystemMetrics(SM_CXPADDEDBORDER);
		if (padding > 0) padding += 2;	// no justification: a bug in window size between VS2010/VS2013
        rect.left = pda.x;
        rect.right = pda.x + pda.width + 2 * GetSystemMetrics(SM_CXBORDER) + padding;
        rect.top = pda.y;
        rect.bottom = pda.y + pda.height + 2 * GetSystemMetrics(SM_CYBORDER) + padding;
        mode = WS_VISIBLE|WS_BORDER|WS_POPUP|WS_CLIPSIBLINGS|WS_CLIPCHILDREN;
        if (pda.caption.size() > 0)
        {
		    mode |= WS_CAPTION|WS_SYSMENU;
            rect.right += 2 * GetSystemMetrics(SM_CXEDGE);
            rect.bottom += 2 * GetSystemMetrics(SM_CYEDGE) + GetSystemMetrics(SM_CYCAPTION);
        }
    }

    // create the window
    if (!(hwnd = CreateWindowEx(0,                  // extended style
                                "RaptorWindow",     // class
                                pda.caption.data(), // title
                                mode,
                                rect.left,rect.top,	  // initial x,y
                                (rect.right - rect.left),(rect.bottom - rect.top),  // initial width, height
                                NULL,	  // handle to parent 
                                NULL,	  // handle to menu
                                GetModuleHandle(NULL),// instance of this application
                                NULL)))	// extra creation parms
        return wnd;

	SetForegroundWindow(hwnd);	// Slightly Higher Priority
	SetFocus(hwnd);

	HDC dc = GetDC(hwnd);
	RAPTOR_HANDLE device(DEVICE_CONTEXT_CLASS,dc);

	RENDERING_CONTEXT_ID id = CContextManager::INVALID_CONTEXT;
	if (CRaptorDisplayConfig::GENERIC == pda.acceleration)
    {
	    id = glCreateContext(device,pda);
        pDisplay = NULL;
		if (CContextManager::INVALID_CONTEXT != id)
	    {
		    wnd.ptr(hwnd);
		    wnd.hClass(WINDOW_CLASS);
            ctx = id;
	    }
	    else
	    {
		    DestroyWindow(hwnd);
	    }
    }
    else
    {
        bool destroyDisplay = (pDisplay == NULL);
        if (destroyDisplay)
            pDisplay = Raptor::glCreateDisplay(pda);
        id = 0;
		if (!pDisplay->glvkBindDisplay(device))
			id = CContextManager::INVALID_CONTEXT;
        else
		{
			RECT rect2;
			GetWindowRect(hwnd,&rect);
			GetClientRect(hwnd,&rect2);
			if (pda.status_bar)
            {
                int menuHeight = GetSystemMetrics(SM_CYMENU);
				pDisplay->glResize(MAX(1,pda.width),pda.height+menuHeight,0,menuHeight);
            }
			else
				pDisplay->glResize(MAX(1,pda.width),MAX(1,pda.height),0,0);

			if (!pDisplay->glvkUnBindDisplay())
				id = CContextManager::INVALID_CONTEXT;
		}
		if (CContextManager::INVALID_CONTEXT != id)
	    {
		    wnd.ptr(hwnd);
		    wnd.hClass(WINDOW_CLASS);
	    }
	    else
	    {
            if (destroyDisplay)
			{
                Raptor::glDestroyDisplay(pDisplay);
                pDisplay = NULL;
            }
		    DestroyWindow(hwnd);
	    }
    }
    
    ReleaseDC(hwnd,dc);
    CATCH_WIN32_ERROR

	return wnd;
}


bool CWin32ContextManager::glDestroyWindow(const RAPTOR_HANDLE& wnd)
{
    if ((wnd.hClass() != WINDOW_CLASS) || (wnd.handle() == 0))
        return false;

    HWND hwnd = wnd.ptr<HWND__>();
    return (0 != DestroyWindow(hwnd));
}

//	
//	Standard OpenGL Rendering Context creation method
//
CContextManager::RENDERING_CONTEXT_ID CWin32ContextManager::glCreateContext(const RAPTOR_HANDLE& device,
																			const CRaptorDisplayConfig& config)
{
	if (device.handle() == 0)
	{
		RAPTOR_ERROR( COpenGL::COpenGLClassID::GetClassId(),"Raptor cannot create a valid context on an Invalid device");
		return CContextManager::INVALID_CONTEXT;
	}
	if (nbContext >= MAX_CONTEXT)
	{
		RAPTOR_ERROR(COpenGL::COpenGLClassID::GetClassId(), "Too many Rendering Context created");
		return CContextManager::INVALID_CONTEXT;
	}

    HDC hDC = NULL;
    int pixelformat = 0;
	
	HGLRC	glhrc = NULL;
	DWORD	flags = 0;
	BYTE	alphabits = 0;
	BYTE	depthbits = 0;
	BYTE	stencilbits = 0;
	BYTE	overlaybits = 0;
	BYTE	accumbits = 0;

    if (device.hClass() == DIB_CLASS)
    {
        hDC = device.ptr<HDC__>();
        flags = PFD_DRAW_TO_BITMAP | PFD_SUPPORT_OPENGL | PFD_GENERIC_ACCELERATED;
    }
    else if (device.hClass() == DEVICE_CONTEXT_CLASS)
    {
        hDC = device.ptr<HDC__>();
        flags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_SWAP_EXCHANGE;
    }
	if (0 == hDC)
		return CContextManager::INVALID_CONTEXT;
	
	int m_mode = config.display_mode;

	if (config.double_buffer) flags = flags|PFD_DOUBLEBUFFER;
	else flags = flags|PFD_DOUBLEBUFFER_DONTCARE;

	if ((m_mode & CGL_DEPTH_32) == CGL_DEPTH_32) depthbits = 32;
	else if (m_mode & CGL_DEPTH_16) depthbits = 16;
	else if (m_mode & CGL_DEPTH_24)	depthbits = 24;
	else flags = flags|PFD_DEPTH_DONTCARE;

	if (m_mode & CGL_RGBA) alphabits=8;
	if (config.stencil_buffer) stencilbits=8;
	if (config.overlay) overlaybits = 1;
	if (config.accumulator_buffer) accumbits = 64;

	PIXELFORMATDESCRIPTOR
	pfd = { 
			sizeof(PIXELFORMATDESCRIPTOR),  //  size of this pfd 
			1,                     // version number 
			flags,
			PFD_TYPE_RGBA,         // RGBA type 
			24,                    // 24-bit color depth 
			0, 0, 0, 0, 0, 0,      // color bits ignored 
			alphabits,             // 8 bits alpha buffer 
			0,                     // alpha shift bit
			accumbits,             // accumulation buffer bits
			0, 0, 0, 0,            // accum bits ignored 
			depthbits,             // 16/24/32-bit z-buffer	 
			stencilbits,           // 8 bits stencil buffer 
			0,                     // no auxiliary buffer 
			PFD_MAIN_PLANE,        // main layer 
			overlaybits,           // reserved 
			0, 0, 0                // layer masks ignored 
		};

	if ( (pixelformat = ChoosePixelFormat(hDC, &pfd)) == 0 ) 
	{
		RAPTOR_FATAL(COpenGL::COpenGLClassID::GetClassId(), "Raptor Context Manager failed to choose pixel format");
		return CContextManager::INVALID_CONTEXT;
	}

	if (SetPixelFormat(hDC, pixelformat, &pfd) == FALSE) 
	{ 
		RAPTOR_FATAL(COpenGL::COpenGLClassID::GetClassId(), "Raptor Context Manager failed to set pixel format");
		return CContextManager::INVALID_CONTEXT;
	}

	glhrc = wglCreateContext(hDC);
	if (!glhrc)
	{
		RAPTOR_FATAL(COpenGL::COpenGLClassID::GetClassId(), "Raptor Context Manager failed to create OpenGL context");
		return CContextManager::INVALID_CONTEXT;
	}

    unsigned int pos = 0;
	while ((pos < MAX_CONTEXT) && (pContext[pos].OGLContext != NULL))
		pos++;

	// pos value is always valid because buffer overflow
	//	is checked on method entry.
	context_t &context = pContext[pos];
	context.OGLContext = glhrc;
	context.WIN32Window = NULL;
	context.WIN32Context = NULL;
    
	wglMakeCurrent(hDC, glhrc);
	RENDERING_CONTEXT_ID	oldContext = m_currentGLContext;
	m_currentGLContext = pos;

	PFN_WGL_GET_EXTENSIONS_STRING_ARB_PROC wglGetExtensionsStringARB = (PFN_WGL_GET_EXTENSIONS_STRING_ARB_PROC)wglGetProcAddress("wglGetExtensionsStringARB");
	std::string extensions = (const char*)glGetString(GL_EXTENSIONS);
	extensions += wglGetExtensionsStringARB(hDC);
	context.pExtensions = new CRaptorGLExtensions(extensions);
	context.pExtensions->glInitExtensions();

	m_currentGLContext = oldContext;
	wglMakeCurrent(hDC,NULL);
	 
	nbContext++;

    CATCH_WIN32_ERROR

	return pos;
}


//	
//	Extended OpenGL Rendering Context creation method
//
CContextManager::RENDERING_CONTEXT_ID  CWin32ContextManager::glCreateExtendedContext(const RAPTOR_HANDLE& device,
																					 const CRaptorDisplayConfig& config)
{
    if (device.handle() == 0)
	{
		RAPTOR_ERROR(COpenGL::COpenGLClassID::GetClassId(), "Raptor cannot create a valid context on an Invalid device");
		return CContextManager::INVALID_CONTEXT;
	}

	if (nbContext >= MAX_CONTEXT)
	{
		RAPTOR_ERROR(COpenGL::COpenGLClassID::GetClassId(), "Too many Rendering Context created");
		return CContextManager::INVALID_CONTEXT;
	}

	HGLRC glhrc = NULL;
		
	//	Now, we are sure that the rendering context can be created,
	//	try to create the extended rendering context
#if defined(WGL_ARB_pixel_format)
	if ((CRaptorGLExtensions::wglChoosePixelFormatARB != NULL) && 
		(CRaptorGLExtensions::wglGetExtensionsStringARB != NULL))
	{
		// alloc pixel format descriptor
        int piAttribIList[2*30];
		memset(piAttribIList,0,2*30*sizeof(int));
		unsigned int attribIndex = 0;

        HDC hDC = device.ptr<HDC__>();
        if (device.hClass() == DIB_CLASS)
        {
            piAttribIList[attribIndex++] = WGL_DRAW_TO_BITMAP_ARB; 
            piAttribIList[attribIndex++] = TRUE;
			hDC = device.ptr<HDC__>();
        }
        else if (device.hClass() == DEVICE_CONTEXT_CLASS)
        {
            piAttribIList[attribIndex++] = WGL_DRAW_TO_WINDOW_ARB; 
            piAttribIList[attribIndex++] = TRUE;
			hDC = device.ptr<HDC__>();
        }
		else if (device.hClass() == WINDOW_CLASS)
		{
			piAttribIList[attribIndex++] = WGL_DRAW_TO_WINDOW_ARB; 
            piAttribIList[attribIndex++] = TRUE;
			hDC = GetDC(device.ptr<HWND__>());
		}

		//	All this stuff .... is for OpenGL of course !!!
		//	OGL acceleration
		attribIndex = defineAcceleration(attribIndex, config.acceleration, piAttribIList);

		//	Draw with double buffer
		attribIndex = defineDoubleBuffer(attribIndex,config.double_buffer,config.swap_buffer, piAttribIList);

		//	RGB or RGBA are the only supported modes, because they are fun.
		//	Paletted mode are so boring and slow and old fashioned ...
        attribIndex = definePixels(attribIndex, config.display_mode, piAttribIList);

		//	Use depth if requested
        attribIndex = defineDepthBuffer(attribIndex, config.display_mode, piAttribIList);
		
		//	Use stencil if requested
        attribIndex = defineStencilBuffer(attribIndex, config.stencil_buffer, piAttribIList);

		//	Use accum buffer if requested
        attribIndex = defineAccumBuffer(attribIndex, config.accumulator_buffer, piAttribIList);

		//	Use antialiasing if requested
        attribIndex = defineSampleBuffer(attribIndex, config.getNbSamples(), piAttribIList);

		//	Terminate the list and continue with the settings
		UINT nNumFormats = 0;
		int pixelformat;
		if (( CRaptorGLExtensions::wglChoosePixelFormatARB(hDC, piAttribIList,NULL,1,&pixelformat,&nNumFormats) == 0 ) || (nNumFormats == 0))
		{
			RAPTOR_FATAL(COpenGL::COpenGLClassID::GetClassId(), "Raptor Context Manager failed to choose EXT pixel format");
			return CContextManager::INVALID_CONTEXT;
		}

		PIXELFORMATDESCRIPTOR pfd;
		DescribePixelFormat(hDC, pixelformat,sizeof(PIXELFORMATDESCRIPTOR), &pfd);
		if (SetPixelFormat(hDC, pixelformat, &pfd) == FALSE) 
		{ 
			RAPTOR_FATAL(COpenGL::COpenGLClassID::GetClassId(), "Raptor Context Manager failed to set pixel format");
			return CContextManager::INVALID_CONTEXT;
		}

		glhrc = wglCreateContext(hDC);
		if (!glhrc)
		{
			RAPTOR_FATAL(COpenGL::COpenGLClassID::GetClassId(), "Raptor Context Manager failed to create OpenGL EXT context");
			return CContextManager::INVALID_CONTEXT;
		}

		unsigned int pos = 0;
		while ((pos < MAX_CONTEXT) && (pContext[pos].OGLContext != NULL))
			pos++;

		// pos value is always valid because buffer overflow
		//	is checked on method entry.
		context_t &context = pContext[pos];
		context.OGLContext = NULL;
		context.WIN32Window = NULL;
		context.WIN32Context = NULL;
		context.pExtensions = NULL; 
		nbContext++;

		wglMakeCurrent(hDC, glhrc);
		RENDERING_CONTEXT_ID	oldContext = m_currentGLContext;
		m_currentGLContext = pos;
        context.OGLContext = glhrc; 

		PFN_WGL_GET_EXTENSIONS_STRING_ARB_PROC wglGetExtensionsStringARB = (PFN_WGL_GET_EXTENSIONS_STRING_ARB_PROC)wglGetProcAddress("wglGetExtensionsStringARB");
		std::string extensions = (const char*)glGetString(GL_EXTENSIONS);
		extensions += wglGetExtensionsStringARB(hDC);
		context.pExtensions = new CRaptorGLExtensions(extensions);
		context.pExtensions->glInitExtensions();

		m_currentGLContext = oldContext;
		wglMakeCurrent(hDC, NULL);

		if (device.hClass() == WINDOW_CLASS)
		{
			ReleaseDC(device.ptr<HWND__>(),hDC);
		}

        CATCH_WIN32_ERROR

		return pos;
	}
#endif

	return CContextManager::INVALID_CONTEXT;
}



#ifdef WGL_ARB_pbuffer
	CContextManager::PIXEL_BUFFER_ID CWin32ContextManager::glCreatePBuffer(const CRaptorDisplayConfig &pcs)
	{
		if (nbPBuffers >= MAX_PBUFFERS)
		{
			RAPTOR_ERROR(COpenGL::COpenGLClassID::GetClassId(), "Too many Pixel Buffers created");
			return 0;
		}

		if ((m_currentGLContext < 0) || (m_currentGLContext >= MAX_CONTEXT))
		{
			RAPTOR_FATAL(COpenGL::COpenGLClassID::GetClassId(), "No active rendering context to create a pbuffer");
            return 0;
		} 
        
		context_t &context = pContext[m_currentGLContext];
        
		if ((context.pExtensions->wglCreatePBufferARB == NULL) || 
			(context.pExtensions->wglChoosePixelFormatARB == NULL) || 
			(context.pExtensions->wglGetPBufferDCARB == NULL))
		{
			RAPTOR_FATAL(COpenGL::COpenGLClassID::GetClassId(), "Your driver or GPU does not support Pixel Buffers");
			return 0;
		}

		HDC hdc = wglGetCurrentDC();

		// alloc pixel format descriptor
        int piAttribIList[2*20];
		memset(piAttribIList,0,2*20*sizeof(int));
		int m_mode = pcs.display_mode;
		unsigned int attribIndex = 0;

		//	draw to P Buffer
		piAttribIList[attribIndex++] = WGL_DRAW_TO_PBUFFER_ARB; 
        piAttribIList[attribIndex++] = TRUE;

		//	All this stuff .... is for OpenGL of course !!!
		//	OGL acceleration
        attribIndex  = defineAcceleration(attribIndex, pcs.acceleration, piAttribIList);
		
		// P Buffer do not support double buffering. Maybe future versions will do so.
		//	Draw with double buffer
		attribIndex = defineDoubleBuffer(attribIndex,pcs.double_buffer,pcs.swap_buffer, piAttribIList);

		//	RGB or RGBA are the only supported modes, because they are fun.
		//	Paletted mode are so boring and slow and old fashioned ...
        attribIndex = definePixels(attribIndex, pcs.display_mode, piAttribIList);
		
		//	Use depth if requested
        attribIndex = defineDepthBuffer(attribIndex, pcs.display_mode, piAttribIList);

		//	Use stencil if requested
		attribIndex = defineStencilBuffer(attribIndex, pcs.stencil_buffer, piAttribIList);

		//	Use accum buffer if requested
        attribIndex = defineAccumBuffer(attribIndex, pcs.accumulator_buffer, piAttribIList);

		//	Use antialiasing if requested
        attribIndex = defineSampleBuffer(attribIndex, pcs.getNbSamples(), piAttribIList);

#ifdef WGL_ARB_render_texture
		if (pcs.bind_to_texture)
		{
			piAttribIList[attribIndex++] = WGL_BIND_TO_TEXTURE_RGBA_ARB;
            //piAttribIList[attribIndex++] = 0x20B4; //WGL_BIND_TO_TEXTURE_RECTANGLE_FLOAT_RGBA_NV
			piAttribIList[attribIndex++] = TRUE;

		#ifdef WGL_NV_render_depth_texture
			if ((m_mode & CGL_DEPTH_32) != 0)
			{
				piAttribIList[attribIndex++] = WGL_BIND_TO_TEXTURE_DEPTH_NV;
				piAttribIList[attribIndex++] = TRUE;
			}
		#endif

			if ((context.pExtensions->wglBindTexImageARB == NULL) || 
				(context.pExtensions->wglReleaseTexImageARB == NULL) || 
				(context.pExtensions->wglSetPbufferAttribARB == NULL))
			{
				RAPTOR_FATAL( COpenGL::COpenGLClassID::GetClassId(),"Your driver or GPU does not support texture rendering");
				return 0;
			}
		}
#endif

		//	Terminate the list and continue with the settings
		UINT nNumFormats = 0;
		int pixelformat;
		if ((context.pExtensions->wglChoosePixelFormatARB(hdc, piAttribIList,NULL,1,&pixelformat,&nNumFormats) == 0 ) || (nNumFormats == 0))
		{
			RAPTOR_FATAL(COpenGL::COpenGLClassID::GetClassId(), "Raptor failed to choose pixel format for PBuffer");
			return 0; 
		}

		int pBufferAttribs[10];
		memset(pBufferAttribs,0,10*sizeof(int));
#ifdef WGL_ARB_render_texture
		if (pcs.bind_to_texture)
		{
			int idx = 0;
			pBufferAttribs[idx++] = WGL_TEXTURE_FORMAT_ARB;
			pBufferAttribs[idx++] = WGL_TEXTURE_RGBA_ARB;
            //pBufferAttribs[idx++] = 0x20B8; //WGL_TEXTURE_FLOAT_RGBA_NV
			pBufferAttribs[idx++] = WGL_TEXTURE_TARGET_ARB;
            if ((m_mode & CGL_RENDER_CUBETEXTURE) == CGL_RENDER_CUBETEXTURE)
                pBufferAttribs[idx++] = WGL_TEXTURE_CUBE_MAP_ARB;
            else
                pBufferAttribs[idx++] = WGL_TEXTURE_2D_ARB;
			//pBufferAttribs[idx++] = 0x20A2; //WGL_TEXTURE_RECTANGLE_NV
			//pBufferAttribs[idx++] = WGL_MIPMAP_TEXTURE_ARB;
			//pBufferAttribs[idx++] = TRUE;

		#ifdef WGL_NV_render_depth_texture
			if ((m_mode & CGL_DEPTH_32) != 0)
			{
				pBufferAttribs[idx++] = WGL_DEPTH_TEXTURE_FORMAT_NV;
				pBufferAttribs[idx++] = WGL_TEXTURE_DEPTH_COMPONENT_NV;
			}
		#endif
		}
#endif

		HPBUFFERARB hpbuf = context.pExtensions->wglCreatePBufferARB(hdc,pixelformat,pcs.width,pcs.height,pBufferAttribs);
		if (hpbuf == NULL)
		{
			RAPTOR_FATAL(COpenGL::COpenGLClassID::GetClassId(), "Raptor failed to create PBuffer");
			return 0;
		}

		HDC hpbufdc = context.pExtensions->wglGetPBufferDCARB(hpbuf);
		if (!hpbufdc)
		{
			RAPTOR_FATAL(COpenGL::COpenGLClassID::GetClassId(), "Raptor failed to create PBuffer device context");
			return 0;
		}

		HGLRC hpbufglhrc = wglCreateContext(hpbufdc);
		if (!hpbufglhrc)
		{
			RAPTOR_FATAL(COpenGL::COpenGLClassID::GetClassId(), "Raptor failed to create PBuffer OpenGL context");
			return 0;
		}
		

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
			RAPTOR_WARNING( COpenGL::COpenGLClassID::GetClassId(),"Unable to share PBuffer display lists");
		}
#endif

        CATCH_WIN32_ERROR
		return pos;
	}

	bool CWin32ContextManager::glDestroyPBuffer(PIXEL_BUFFER_ID pbuffer)
	{
		if (pbuffer > MAX_PBUFFERS)
		{
			RAPTOR_ERROR( COpenGL::COpenGLClassID::GetClassId(),"Exceeded max number of pBuffers");
			return false;
		}

		if (pBuffers[pbuffer].pbuffer == NULL)
			return false;

		if (FALSE == wglDeleteContext(pBuffers[pbuffer].pbufferGLRC))
		{
			RAPTOR_FATAL(COpenGL::COpenGLClassID::GetClassId(), "Raptor failed to destroy PBuffer OpenGL context");
			return false;
		}
		
       const CRaptorGLExtensions *const pExtensions = this->glGetExtensions();
		if (0 == pExtensions->wglReleasePBufferDCARB(pBuffers[pbuffer].pbuffer,pBuffers[pbuffer].pbufferDC))
		{
			RAPTOR_FATAL(COpenGL::COpenGLClassID::GetClassId(), "Raptor failed to release PBuffer device context");
			return false;
		}

		if (0 == pExtensions->wglDestroyPBufferARB(pBuffers[pbuffer].pbuffer))
		{
			RAPTOR_FATAL(COpenGL::COpenGLClassID::GetClassId(), "Raptor failed to destroy PBuffer");
			return false;
		}

		pBuffers[pbuffer].pbuffer = NULL;
		pBuffers[pbuffer].pbufferDC = NULL;
		pBuffers[pbuffer].pbufferGLRC = NULL;
		nbPBuffers--;

        CATCH_WIN32_ERROR
		return true;
	}

	void CWin32ContextManager::glBindPBuffer(PIXEL_BUFFER_ID pbuffer,CTextureObject::CUBE_FACE selectBuffer)
	{
		if (pBuffers[pbuffer].pbuffer == NULL)
		{
			stringstream str;
			str << "Raptor pBuffer ";
			str << (pbuffer+1);
            str << " does not exist";
			RAPTOR_ERROR(COpenGL::COpenGLClassID::GetClassId(), str.str());
			return;
		}
  
        if (pBuffers[pbuffer].pbufferGLRC != wglGetCurrentContext())
		    wglMakeCurrent(pBuffers[pbuffer].pbufferDC,pBuffers[pbuffer].pbufferGLRC);
       
#if defined(WGL_ARB_render_texture)
		if ((selectBuffer >= CTextureObject::CGL_CUBEMAP_PX) && 
			(selectBuffer <= CTextureObject::CGL_CUBEMAP_NZ))
        {
            const CRaptorGLExtensions *const pExtensions = this->glGetExtensions();
        
            int piAttribList[3];
            piAttribList[0] = WGL_CUBE_MAP_FACE_ARB;
			piAttribList[1] = (selectBuffer - CTextureObject::CGL_CUBEMAP_PX) + WGL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB;
            piAttribList[2] = 0;

            pExtensions->wglSetPbufferAttribARB (pBuffers[pbuffer].pbuffer,piAttribList);
        }
#endif

        CATCH_WIN32_ERROR
	}

	bool CWin32ContextManager::glIsPBuffer(PIXEL_BUFFER_ID pbuffer) const
	{
		return (pBuffers[pbuffer].pbuffer != NULL);
	}

	bool CWin32ContextManager::glBindTexImageARB(PIXEL_BUFFER_ID pbuffer,int iBuffer)
	{
#if defined(WGL_ARB_render_texture)
		if (pbuffer > MAX_PBUFFERS)
		{
			RAPTOR_ERROR( COpenGL::COpenGLClassID::GetClassId(),"Exceeded max number of pBuffers");
			return false;
		}
		
        const CRaptorGLExtensions *const pExtensions = this->glGetExtensions();
        
		if ((pBuffers[pbuffer].pbuffer == NULL) || (pExtensions->wglBindTexImageARB == NULL))
		{
			stringstream str;
			str << "Raptor pBuffer ";
			str << (pbuffer+1);
            str << " does not exist or texture rendering not supported";
			RAPTOR_ERROR(COpenGL::COpenGLClassID::GetClassId(), str.str());
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
        BOOL res = pExtensions->wglBindTexImageARB(pBuffers[pbuffer].pbuffer,iBuffer);

        CATCH_WIN32_ERROR

		return (FALSE != res);
#else
        return false;
#endif //#if defined(WGL_ARB_render_texture)
	}

	bool CWin32ContextManager::glReleaseTexImageARB(PIXEL_BUFFER_ID pbuffer,int iBuffer)
	{
#if defined(WGL_ARB_render_texture)
		if (pbuffer > MAX_PBUFFERS)
		{
			RAPTOR_ERROR(COpenGL::COpenGLClassID::GetClassId(), "Exceeded max number of pBuffers");
			return false;
		}

        const CRaptorGLExtensions *const pExtensions = this->glGetExtensions();
       
		if ((pBuffers[pbuffer].pbuffer == NULL) || (pExtensions->wglReleaseTexImageARB == NULL))
		{
			stringstream str;
			str << "Raptor pBuffer ";
            str << (pbuffer+1);
            str << " does not exist or texture rendering not supported";
			RAPTOR_ERROR(COpenGL::COpenGLClassID::GetClassId(), str.str());
			return false;
		}

        BOOL res = pExtensions->wglReleaseTexImageARB(pBuffers[pbuffer].pbuffer,iBuffer);

        CATCH_WIN32_ERROR

		return (FALSE != res);
#else
        return false;
#endif //#if defined(WGL_ARB_render_texture)
	}

#else
	CContextManager::PIXEL_BUFFER_ID CWin32ContextManager::glCreatePBuffer(const CRaptorDisplayConfig& ) { return 0; }
	bool CWin32ContextManager::glDestroyPBuffer(PIXEL_BUFFER_ID ) { return false; };
	void CWin32ContextManager::glBindPBuffer(PIXEL_BUFFER_ID,unsigned int) { };
	bool CWin32ContextManager::glIsPBuffer(PIXEL_BUFFER_ID ) const { return false; };
	bool CWin32ContextManager::glBindTexImageARB(PIXEL_BUFFER_ID ,int ) { return false; };
	bool CWin32ContextManager::glReleaseTexImageARB(PIXEL_BUFFER_ID ,int ) { return false; };
#endif  // #ifdef WGL_ARB_pbuffer


#if defined(VK_VERSION_1_0)
bool CWin32ContextManager::vkInit(void)
{
	char buffer[MAX_PATH];
	GetEnvironmentVariable("VULKAN_BIN_PATH",buffer,MAX_PATH);
	std::string vkpath = buffer;
	vkpath += "\\VULKAN-1.DLL";
	HMODULE module = LoadLibrary(vkpath.c_str());
	vulkanModule.ptr(module);

	if (NULL != module)
	{
		CRaptorVKExtensions::vkGetInstanceProcAddr = (PFN_vkGetInstanceProcAddr)(GetProcAddress(module, "vkGetInstanceProcAddr"));
	
		if (CContextManager::vkInit())
		{
			CRaptorVKExtensions instance_extensions("");
			bool surface_rendering_supported =	instance_extensions.vkIsExtensionSupported(VK_KHR_SURFACE_EXTENSION_NAME) &&
												instance_extensions.vkIsExtensionSupported(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
			if (!surface_rendering_supported)
			{
				RAPTOR_ERROR(CVulkan::CVulkanClassID::GetClassId(),
								"Vulkan API does not support rendering to surface");
			}
			return surface_rendering_supported;
		}
		return false;
	}
	return false;
}

bool CWin32ContextManager::vkRelease(void)
{
	if (CContextManager::vkRelease())
	{
		if (0 != vulkanModule.handle())
		{
			HMODULE module = vulkanModule.ptr<HINSTANCE__>();
			BOOL res = FreeLibrary(module);
			return (res == TRUE);
		}
	}
		
	return false;
}

uint32_t CWin32ContextManager::getPresentationSuppotQueueFamily(RENDERING_CONTEXT_ID ctx)
{
	uint32_t presentation_queue = MAXUINT;

#if defined(VK_USE_PLATFORM_WIN32_KHR)
	VK_CONTEXT &vk_ctx = m_pVkContext[ctx];

	uint32_t pQueueFamilyPropertyCount = 0;
	CRaptorVKExtensions::vkGetPhysicalDeviceQueueFamilyProperties(vk_ctx.physicalDevice, &pQueueFamilyPropertyCount, NULL);

	if (pQueueFamilyPropertyCount > 0)
	{
		//!	Check presentation support
		for (uint32_t j = 0; j < pQueueFamilyPropertyCount; j++)
		{
			VkBool32 support = vk_ctx.pExtensions->vkGetPhysicalDeviceWin32PresentationSupportKHR(vk_ctx.physicalDevice, j);
			if (support)
				presentation_queue = j;
		}
	}

	if (MAXUINT == presentation_queue)
	{
		RAPTOR_ERROR(CVulkan::CVulkanClassID::GetClassId(),
					 "No queue family supports WSI rendering");
	}

#else
	RAPTOR_ERROR(Global::CVulkanClassID::GetClassId(),"No support for Vulkan WSI");
#endif

	return presentation_queue;
}

bool CWin32ContextManager::vkCreateSurface(const RAPTOR_HANDLE& handle,RENDERING_CONTEXT_ID ctx)
{
	if (WINDOW_CLASS == handle.hClass())
	{
		HWND hWnd = handle.ptr<HWND__>();
		HINSTANCE hInstance = (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE);
		VkWin32SurfaceCreateInfoKHR createInfo = {	VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
													NULL,0, //flags,
													hInstance,hWnd };
		VK_CONTEXT &context = m_pVkContext[ctx];
		if (NULL != context.pExtensions)
		{
			//!	Create the Surface
			VkResult res = VK_NOT_READY;
			VkSurfaceKHR surface = VK_NULL_HANDLE;
			if (context.pExtensions->vkIsExtensionSupported(VK_KHR_WIN32_SURFACE_EXTENSION_NAME))
				res = context.pExtensions->vkCreateWin32SurfaceKHR(CRaptorVKExtensions::getInstance(), &createInfo, NULL, &surface);
			if (VK_SUCCESS != res)
			{
				RAPTOR_ERROR(CVulkan::CVulkanClassID::GetClassId(),
							 "Failed to create Vulkan rendering surface");
				return false;
			}
			else
				context.pSurface = new CVulkanSurface(surface);
			return true;
		}
		else
			return false;
	}
	else
		return false;
}

void CWin32ContextManager::vkSwapVSync(unsigned int framerate)
{
#ifdef VK_KHR_swapchain
	if (CGL_MAXREFRESHRATE == framerate)
		presentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;	// no-vblank + no-queue (immediate)
	else if (0 == framerate)
		presentMode = VK_PRESENT_MODE_FIFO_RELAXED_KHR; // single v-blank + internal queue
	else // (swapinterval == 1)
	{
		DEVMODE mode;
		memset(&mode, sizeof(DEVMODE), 0);
		mode.dmSize = sizeof(DEVMODE);
		mode.dmDriverExtra = 0;
		mode.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT | DM_DISPLAYFLAGS | DM_DISPLAYFREQUENCY;

		if (0 != EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &mode))
		{
			if (mode.dmDisplayFrequency < framerate)
				presentMode = VK_PRESENT_MODE_MAILBOX_KHR;		// v-blank + no-queue (latest image rendered)
			else
				presentMode = VK_PRESENT_MODE_FIFO_KHR;			// v-blank + internal queue
		}
		else	// default mode.
			presentMode = VK_PRESENT_MODE_FIFO_KHR;			// v-blank + internal queue
	}
#endif
}
#endif
