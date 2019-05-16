/***************************************************************************/
/*                                                                         */
/*  main.cpp                                                               */
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


#include "GLBuilder.h"
#include <iostream>
#include <unistd.h>	// sleep
#include <string.h>

#ifndef __GLX_RAPTOR_GLEXT_H__
	#include "GLXSpecific/GLXGlext.h"
#endif
//#include "vulkan.h"

#define GLX_CONTEXT_MAJOR_VERSION_ARB       0x2091
#define GLX_CONTEXT_MINOR_VERSION_ARB       0x2092
typedef GLXContext (*glXCreateContextAttribsARBProc)(Display*, GLXFBConfig, GLXContext, Bool, const int*);

static bool isExtensionSupported(const char *extList, const char *extension)
{
	const char *start = NULL;
	const char *where = NULL;
	const char *terminator = NULL;

	// Extension names should not have spaces.
	where = strchr(extension, ' ');
	if (where || *extension == '\0')
		return false;

	// It takes a bit of care to be fool-proof about parsing the
	// OpenGL extensions string. Don't be fooled by sub-strings, etc.
	for (start=extList;;)
	{
		where = strstr(start, extension);

		if (!where)
			break;

		terminator = where + strlen(extension);

		if ( where == start || *(where - 1) == ' ' )
			if ( *terminator == ' ' || *terminator == '\0' )
				return true;

		start = terminator;
	}

	return false;
}

static bool ctxErrorOccurred = false;
static int ctxErrorHandler( Display *dpy, XErrorEvent *ev )
{
    ctxErrorOccurred = true;
    return 0;
}

static CGLBuilder builder;

int glCreateContext()
{
    Display* pGlobalDisplay = XOpenDisplay(NULL);
    int dummy = 0;
    if ((NULL == pGlobalDisplay) || !glXQueryExtension(pGlobalDisplay, &dummy, &dummy))
    {
        std::cout << "OpenGL GLX Extensions are not supported or display is invalid!" << std::endl;
        return 0;
    }

	int glx_major, glx_minor;

	// FBConfigs were added in GLX version 1.3.
	if ( !glXQueryVersion( pGlobalDisplay, &glx_major, &glx_minor ) || 
		( ( glx_major == 1 ) && ( glx_minor < 3 ) ) || ( glx_major < 1 ) )
	{
		std::cout << "Invalid GLX version!" << std::endl;
		return 0;
	}

	// Get a matching FB config
	static int visual_attribs[] =
	{
		GLX_X_RENDERABLE    , True,
		GLX_DRAWABLE_TYPE   , GLX_WINDOW_BIT,
		GLX_RENDER_TYPE     , GLX_RGBA_BIT,
		GLX_X_VISUAL_TYPE   , GLX_TRUE_COLOR,
		GLX_RED_SIZE        , 8,
		GLX_GREEN_SIZE      , 8,
		GLX_BLUE_SIZE       , 8,
		GLX_ALPHA_SIZE      , 8,
		GLX_DEPTH_SIZE      , 24,
		GLX_STENCIL_SIZE    , 8,
		GLX_DOUBLEBUFFER    , True,
		//GLX_SAMPLE_BUFFERS  , 1,
		//GLX_SAMPLES         , 4,
		None
	};

	std::cout << "Getting matching framebuffer configs." << std::endl;
	int fbcount;
	GLXFBConfig* fbc = glXChooseFBConfig(pGlobalDisplay, DefaultScreen(pGlobalDisplay), visual_attribs, &fbcount);
	if (!fbc)
	{
		std::cout << "Failed to retrieve a framebuffer config!" << std::endl;
		return 0;
	}
	std::cout << "Found " << fbcount << " matching FB configs." << std::endl;

	// Pick the FB config/visual with the most samples per pixel
	std::cout << "Getting XVisualInfos." << std::endl;
	int best_fbc = -1, worst_fbc = -1, best_num_samp = -1, worst_num_samp = 999;

	for (int i=0; i<fbcount; ++i)
	{
		XVisualInfo *vi = glXGetVisualFromFBConfig( pGlobalDisplay, fbc[i] );
		if ( vi )
		{
			int samp_buf, samples;
			glXGetFBConfigAttrib( pGlobalDisplay, fbc[i], GLX_SAMPLE_BUFFERS, &samp_buf );
			glXGetFBConfigAttrib( pGlobalDisplay, fbc[i], GLX_SAMPLES       , &samples  );

			std::cout 	<< "  Matching fbconfig " << i
						<< ", visual ID 0x" << vi->visualid
						<< ": SAMPLE_BUFFERS = " << samp_buf
						<< " SAMPLES = " << samples << std::endl; 

			if ( best_fbc < 0 || samp_buf && samples > best_num_samp )
				best_fbc = i, best_num_samp = samples;
			if ( worst_fbc < 0 || !samp_buf || samples < worst_num_samp )
				worst_fbc = i, worst_num_samp = samples;
		}
		XFree( vi );
	}

	GLXFBConfig bestFbc = fbc[best_fbc];

	// Be sure to free the FBConfig list allocated by glXChooseFBConfig()
	XFree( fbc );


	// Get a visual
	XVisualInfo *vi = glXGetVisualFromFBConfig( pGlobalDisplay, bestFbc );
	std::cout << "Chosen visual ID = " << vi->visualid << std::endl;

	std::cout << "Creating colormap." << std::endl;
	XSetWindowAttributes swa;
	Colormap cmap;
	swa.colormap = cmap = XCreateColormap(	pGlobalDisplay,
											RootWindow( pGlobalDisplay, vi->screen ), 
											vi->visual, AllocNone );
	swa.background_pixmap = None ;
	swa.border_pixel      = 0;
	swa.event_mask        = StructureNotifyMask;

	std::cout << "Creating window." << std::endl;
	Window win = XCreateWindow( pGlobalDisplay, RootWindow( pGlobalDisplay, vi->screen ), 
								0, 0, 100, 100, 0, vi->depth, InputOutput, 
								vi->visual, 
								CWBorderPixel|CWColormap|CWEventMask, &swa );
	if ( !win )
	{
		std::cout << "Failed to create window!" << std::endl;
		return 0;
	}

	  // Done with the visual info data
	XFree( vi );

	XStoreName( pGlobalDisplay, win, "GL 3.0 Window" );

	std::cout << "Mapping window." << std::endl;
	XMapWindow( pGlobalDisplay, win );

	// Get the default screen's GLX extension list
	const char *glxExts = glXQueryExtensionsString(pGlobalDisplay, DefaultScreen(pGlobalDisplay));

	// NOTE: It is not necessary to create or make current to a context before
	// calling glXGetProcAddressARB
	glXCreateContextAttribsARBProc glXCreateContextAttribsARB = 0;
	glXCreateContextAttribsARB = (glXCreateContextAttribsARBProc)glXGetProcAddressARB( (const GLubyte *) "glXCreateContextAttribsARB" );

	GLXContext ctx = 0;

	// Install an X error handler so the application won't exit if GL 3.0
	// context allocation fails.
	//
	// Note this error handler is global.  All display connections in all threads
	// of a process use the same error handler, so be sure to guard against other
	// threads issuing X commands while this code is running.
	ctxErrorOccurred = false;
	int (*oldHandler)(Display*, XErrorEvent*) = XSetErrorHandler(&ctxErrorHandler);

	// Check for the GLX_ARB_create_context extension string and the function.
	// If either is not present, use GLX 1.3 context creation method.
	if (!isExtensionSupported( glxExts, "GLX_ARB_create_context" ) ||
		!glXCreateContextAttribsARB )
	{
		std::cout << "glXCreateContextAttribsARB() not found ... using old-style GLX context!" << std::endl;
		ctx = glXCreateNewContext(pGlobalDisplay, bestFbc, GLX_RGBA_TYPE, 0, True );
	}
	// If it does, try to get a GL 3.0 context!
	else
	{
		int context_attribs[] = {
									GLX_CONTEXT_MAJOR_VERSION_ARB, 3,
									GLX_CONTEXT_MINOR_VERSION_ARB, 0,
									//GLX_CONTEXT_FLAGS_ARB        ,
									// GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
									None
								};

		std::cout << "Creating context" << std::endl;
		ctx = glXCreateContextAttribsARB(pGlobalDisplay, bestFbc, 0, True, context_attribs);

		// Sync to ensure any errors generated are processed.
		XSync( pGlobalDisplay, False );
		if ( !ctxErrorOccurred && ctx )
			std::cout << "Created GL 3.0 context." << std::endl;
		else
		{
			// Couldn't create GL 3.0 context.  Fall back to old-style 2.x context.
			// When a context version below 3.0 is requested, implementations will
			// return the newest context version compatible with OpenGL versions less
			// than version 3.0.
			// GLX_CONTEXT_MAJOR_VERSION_ARB = 1
			context_attribs[1] = 1;
			// GLX_CONTEXT_MINOR_VERSION_ARB = 0
			context_attribs[3] = 0;

			ctxErrorOccurred = false;

			std::cout << "Failed to create GL 3.0 context ... using old-style GLX context!" << std::endl;
			ctx = glXCreateContextAttribsARB(pGlobalDisplay, bestFbc, 0, True, context_attribs );
		}
	}

	// Sync to ensure any errors generated are processed.
	XSync( pGlobalDisplay, False );

	// Restore the original error handler
	XSetErrorHandler( oldHandler );

	if ( ctxErrorOccurred || !ctx )
	{
		std::cout << "Failed to create an OpenGL context!" << std::endl;
		return 0;
	}

	// Verifying that context is a direct context
	if ( ! glXIsDirect ( pGlobalDisplay, ctx ) )
	{
		std::cout << "Indirect GLX rendering context obtained." << std::endl;
	}
	else
	{
		std::cout << "Direct GLX rendering context obtained." << std::endl;
	}

	std::cout << "Making context current." << std::endl;
	glXMakeCurrent( pGlobalDisplay, win, ctx );

	glClearColor( 0, 0.5, 1, 1 );
	glClear( GL_COLOR_BUFFER_BIT );
	glXSwapBuffers ( pGlobalDisplay, win );

	sleep( 1 );

	glClearColor ( 1, 0.5, 0, 1 );
	glClear ( GL_COLOR_BUFFER_BIT );
	glXSwapBuffers ( pGlobalDisplay, win );

	sleep( 1 );


	std::cout << "Collecting OpenGL extensions." << std::endl;
	std::string glextensions = (const char*)glGetString(GL_EXTENSIONS);
	std::cout << "glextensions:" << glextensions << std::endl;
	string version = (const char*)glGetString(GL_VERSION);
	std::cout << "version:" << version << std::endl;
	glextensions += " ";
	glextensions += CGLBuilder::full_profile;
	glextensions += builder.setVersion(version);
	builder.glQueryExtensions(glextensions);

	glXMakeCurrent( pGlobalDisplay, 0, 0 );
	glXDestroyContext( pGlobalDisplay, ctx );

	XDestroyWindow( pGlobalDisplay, win );
	XFreeColormap( pGlobalDisplay, cmap );
	XCloseDisplay( pGlobalDisplay );

	std::cout << "glCreateContext done." << std::endl;
    return 1;
}

void print_help(void)
{
	std::cout << "Raptor Builder copyright 2019." << std::endl;
	std::cout << "Usage:" << std::endl;
	std::cout << "Builder [-v|--verbose] [-e|--enable <extension name>] [-d|--disable <extension name>]" << std::endl;
	std::cout << "  -v, --verbose: print extended information messages" << std::endl;
	std::cout << "  -e, --enable: activate an OpenGL or Vulkan extension given by name" << std::endl;
	std::cout << "  -d, --disable: deactivate an OpenGL or Vulkan extension given by name" << std::endl;
	std::cout << "Exiting." << std::endl;
}

int main(int argc, char *argv[])
{
	if (!glCreateContext())
	{
		std::cout << "Failed to query OpenGL context. " << std::endl;
		return -1;
	}

	bool verbose = false;
	bool help = false;
	for (int i=1;i<argc;i++)
	{
		const char* param = argv[i];
		if (NULL == param)
			continue;
		else if (!strcmp("-v",param))
			verbose = true;
		else if (!strcmp("--verbose",param))
			verbose = true;
		else if (!strcmp("-h",param))
			help = true;
		else if (!strcmp("--help",param))
			help = true;
		else if (!strcmp("-e",param) && (i+1 < argc))
			builder.activateExtension(argv[i+1], true);
		else if (!strcmp("--enable",param) && (i+1 < argc))
			builder.activateExtension(argv[i+1], true);
		else if (!strcmp("-d",param) && (i+1 < argc))
			builder.activateExtension(argv[i+1], false);
		else if (!strcmp("--disable",param) && (i+1 < argc))
			builder.activateExtension(argv[i+1], false);
	}
	
	if (help)
	{
		print_help();
		return(0);
	}
	
	const vector<CGLBuilder::EXTENSION>& extensions = builder.getExtensions();
	if (verbose) for (size_t i=0;i<extensions.size();i++)
	{
		const CGLBuilder::EXTENSION& extension = extensions[i];
		std::cout << "extension: " << extension.extensionName;
		std::cout << ((extension.active)?" O ":" x ") << std::endl;
	}

	if (!builder.checkConsistency(false))
	{
		std::cout << "Inconsistency found in extension profile. Header file not generated." << std::endl;
		return(-1);
	}
	else
	{
		if (!builder.writeHeader("CodeGeneration.h"))
		{
			std::cout << "Failed to generate header file." << std::endl;
			return(-1);
		}
	}

	return(0);
}
