/***************************************************************************/
/*                                                                         */
/*  BuilderNative.cpp                                                      */
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

#ifdef WIN32
	#define WIN32_LEAN_AND_MEAN
	#include <Windows.h>
#endif

#include <GL/gl.h>
#include "vulkan.h"
#include <string>
#include <vector>
#include "BuilderNative.h"

#if !defined(AFX_GLBUILDER_H__0F2DA94E_7293_494C_B936_8CB72098E626__INCLUDED_)
	#include "GLBuilder.h"
#endif
#if !defined(__SIMDMACROS_H__)
	#include "SimdLib/simdMacros.h"
#endif


// tip to solve warning LNK4248: unresolved typeref token:
//	the types below are only predeclarations in vulkan.h
//	so here, define only the name ...
struct VkPhysicalDevice_T {};
struct VkInstance_T {};

//	The builder instance.
static CGLBuilder	*builder = NULL;

#ifdef WIN32
	#define STRDUP(s) _strdup(s)
	typedef const char * (APIENTRY * pfnc)(HDC hdc);

	int glCreateContext(HDC& hdc, HGLRC& glhrc)
	{
		DWORD	flags;
		BYTE	alphabits = 8;
		BYTE	depthbits = 24;
		BYTE	stencilbits = 8;

		flags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_SWAP_EXCHANGE | PFD_DOUBLEBUFFER;

		PIXELFORMATDESCRIPTOR pfd = { sizeof(PIXELFORMATDESCRIPTOR),  //  size of this pfd 
			1,                     // version number 
			flags,
			PFD_TYPE_RGBA,         // RGBA type 
			24,                    // 24-bit color depth 
			0, 0, 0, 0, 0, 0,      // color bits ignored 
			alphabits,             // 8 bits alpha buffer 
			0,                     // alpha shift bit
			0,                     // no accumulation buffer 
			0, 0, 0, 0,            // accum bits ignored 
			depthbits,             // 16/24/32-bit z-buffer	 
			stencilbits,           // 8 bits stencil buffer 
			0,                     // no auxiliary buffer 
			PFD_MAIN_PLANE,        // main layer 
			0,                     // reserved 
			0, 0, 0                // layer masks ignored 
		};

		int pixelformat = 0;
		if ((pixelformat = ChoosePixelFormat(hdc, &pfd)) == 0)
		{
			return -1;
		}

		if (SetPixelFormat(hdc, pixelformat, &pfd) == FALSE)
		{
			return -1;
		}

		glhrc = wglCreateContext(hdc);
		if (!glhrc)
		{
			return -1;
		}
		else
		{
			return 0;
		}
	}

	bool glvkInitBuilder(HDC dc)
	{
		if (NULL == builder)
			builder = new CGLBuilder();

		HGLRC glhrc;
		int res = glCreateContext(dc, glhrc);

		if (res == 0)
		{
			BOOL bres = wglMakeCurrent(dc, glhrc);

			std::string glextensions = (const char*)glGetString(GL_EXTENSIONS);
			std::string version = (const char*)glGetString(GL_VERSION);

			pfnc _glGetExtensionsStringARB;
			if (NULL != (_glGetExtensionsStringARB = (pfnc)wglGetProcAddress("wglGetExtensionsStringARB")))
			{
				std::string wgl_extensions = (const char*)_glGetExtensionsStringARB(dc);
				glextensions += wgl_extensions;
			}
			glextensions += " ";
			glextensions += CGLBuilder::full_profile;

			char buffer[MAX_PATH];
			GetEnvironmentVariable("VULKAN_BIN_PATH", (LPTSTR)buffer, MAX_PATH);
			std::string vkpath = buffer;
			vkpath += "\\VULKAN-1.DLL";

			HMODULE module = LoadLibrary(vkpath.c_str());
			if (module != NULL)
			{
				PFN_vkCreateInstance vkCreateInstance = (PFN_vkCreateInstance)(GetProcAddress(module, "vkCreateInstance"));
				VkInstanceCreateInfo instanceCreateInfo = { VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO, // VkStructureType sType;
					NULL,                                   // const void* pNext;
					0,                                      // VkInstanceCreateFlags flags;
					NULL,                                   // const VkApplicationInfo* pApplicationInfo;
					0,                                      // uint32_t enabledLayerNameCount;
					NULL,                                   // const char* const* ppEnabledLayerNames;
					0,                                      // uint32_t enabledExtensionNameCount;
					NULL };									// const char* const* ppEnabledExtensionNames;
				VkInstance inst = NULL;
				if (VK_SUCCESS == vkCreateInstance(&instanceCreateInfo, NULL, &inst))
				{
					glextensions += " VK_VERSION_1_0";

					PFN_vkEnumerateInstanceExtensionProperties vkEnumerateInstanceExtensionProperties =
						(PFN_vkEnumerateInstanceExtensionProperties)(GetProcAddress(module, "vkEnumerateInstanceExtensionProperties"));
					uint32_t pPropertyCount = 0;
					if (VK_SUCCESS == vkEnumerateInstanceExtensionProperties(NULL, &pPropertyCount, NULL))
					{
						VkExtensionProperties *pProperties = new VkExtensionProperties[pPropertyCount];
						vkEnumerateInstanceExtensionProperties(NULL, &pPropertyCount, pProperties);
						for (uint32_t i = 0; i<pPropertyCount; i++)
						{
							glextensions += " ";
							glextensions += pProperties[i].extensionName;
						}
						delete[] pProperties;
					}

					if (VK_SUCCESS == res)
					{
						PFN_vkEnumeratePhysicalDevices vkEnumeratePhysicalDevices =
							(PFN_vkEnumeratePhysicalDevices)(GetProcAddress(module, "vkEnumeratePhysicalDevices"));

						uint32_t pPhysicalDeviceCount = 0;
						res = vkEnumeratePhysicalDevices(inst, &pPhysicalDeviceCount, NULL);
						if ((VK_SUCCESS == res) && (pPhysicalDeviceCount > 0))
						{
							VkPhysicalDevice *m_pPhysicalDevices = new VkPhysicalDevice[pPhysicalDeviceCount];
							res = vkEnumeratePhysicalDevices(inst, &pPhysicalDeviceCount, m_pPhysicalDevices);
							if (VK_SUCCESS == res)
							{
								PFN_vkEnumerateDeviceExtensionProperties vkEnumerateDeviceExtensionProperties =
									(PFN_vkEnumerateDeviceExtensionProperties)(GetProcAddress(module, "vkEnumerateDeviceExtensionProperties"));
								for (uint32_t i = 0; i<pPhysicalDeviceCount; i++)
								{
									pPropertyCount = 0;
									res = vkEnumerateDeviceExtensionProperties(m_pPhysicalDevices[i], NULL, &pPropertyCount, NULL);
									if ((VK_SUCCESS == res) && (pPropertyCount > 0))
									{
										VkExtensionProperties* pProperties = new VkExtensionProperties[pPropertyCount];
										res = vkEnumerateDeviceExtensionProperties(m_pPhysicalDevices[i], NULL, &pPropertyCount, pProperties);
										for (uint32_t j = 0; j<pPropertyCount; j++)
										{
											glextensions += " ";
											glextensions += pProperties[j].extensionName;
										}
										delete[] pProperties;
									}
								}
							}

							delete[] m_pPhysicalDevices;
						}
					}

					PFN_vkDestroyInstance vkDestroyInstance = (PFN_vkDestroyInstance)(GetProcAddress(module, "vkDestroyInstance"));
					vkDestroyInstance(inst, NULL);
				}
				FreeLibrary(module);
			}

			bres = wglMakeCurrent(NULL, NULL);
			bres = wglDeleteContext(glhrc);

			glextensions += builder->setVersion(version);
			builder->glQueryExtensions(glextensions);
		}
		else
			return false;

		const CPU_INFO& cpu = getCPUINFO();

		builder->activateExtension("RAPTOR_SMP_CODE_GENERATION", cpu.CPUID);
		builder->activateExtension("RAPTOR_SSE_CODE_GENERATION", cpu.hasFeature(CPU_INFO::SSE));
		builder->activateExtension("RAPTOR_SSE2_CODE_GENERATION", cpu.hasFeature(CPU_INFO::SSE2));
		builder->activateExtension("RAPTOR_SSE3_CODE_GENERATION", cpu.hasFeature(CPU_INFO::SSE3));
		builder->activateExtension("RAPTOR_SSSE3_CODE_GENERATION", cpu.hasFeature(CPU_INFO::SSSE3));
		builder->activateExtension("RAPTOR_SSE41_CODE_GENERATION", cpu.hasFeature(CPU_INFO::SSE41));
		builder->activateExtension("RAPTOR_SSE42_CODE_GENERATION", cpu.hasFeature(CPU_INFO::SSE42));
		builder->activateExtension("RAPTOR_AES_CODE_GENERATION", cpu.hasFeature(CPU_INFO::AES));
		builder->activateExtension("RAPTOR_AVX_CODE_GENERATION", cpu.hasFeature(CPU_INFO::AVX));
		builder->activateExtension("RAPTOR_FMA_CODE_GENERATION", cpu.hasFeature(CPU_INFO::FMA));

		return true;
	}

#elif defined(LINUX)

#ifdef GLX_EXTENSIONS
	#include <malloc.h>
	#include <string.h>
	#include <unistd.h>

	typedef ptrdiff_t GLintptr;
	typedef ptrdiff_t GLsizeiptr;

	#include <GL/glx.h>
#endif
	#define STRDUP(s) strdup(s)
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

	GLXContext ctx = 0;

	int glCreateContext(Display *pGlobalDisplay, XVisualInfo *&vi)
	{
		int dummy = 0;
		if ((NULL == pGlobalDisplay) || !glXQueryExtension(pGlobalDisplay, &dummy, &dummy))
		{
			//std::cout << "OpenGL GLX Extensions are not supported or display is invalid!" << std::endl;
			return 0;
		}

		int glx_major, glx_minor;

		// FBConfigs were added in GLX version 1.3.
		if ( !glXQueryVersion( pGlobalDisplay, &glx_major, &glx_minor ) || 
			( ( glx_major == 1 ) && ( glx_minor < 3 ) ) || ( glx_major < 1 ) )
		{
			//std::cout << "Invalid GLX version!" << std::endl;
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

		//std::cout << "Getting matching framebuffer configs." << std::endl;
		int fbcount;
		GLXFBConfig* fbc = glXChooseFBConfig(pGlobalDisplay, DefaultScreen(pGlobalDisplay), visual_attribs, &fbcount);
		if (!fbc)
		{
			//std::cout << "Failed to retrieve a framebuffer config!" << std::endl;
			return 0;
		}
		//std::cout << "Found " << fbcount << " matching FB configs." << std::endl;

		// Pick the FB config/visual with the most samples per pixel
		//std::cout << "Getting XVisualInfos." << std::endl;
		int best_fbc = -1, worst_fbc = -1, best_num_samp = -1, worst_num_samp = 999;

		for (int i=0; i<fbcount; ++i)
		{
			XVisualInfo *vi = glXGetVisualFromFBConfig( pGlobalDisplay, fbc[i] );
			if ( vi )
			{
				int samp_buf, samples;
				glXGetFBConfigAttrib( pGlobalDisplay, fbc[i], GLX_SAMPLE_BUFFERS, &samp_buf );
				glXGetFBConfigAttrib( pGlobalDisplay, fbc[i], GLX_SAMPLES       , &samples  );
				/*
				std::cout 	<< "  Matching fbconfig " << i
					<< ", visual ID 0x" << vi->visualid
					<< ": SAMPLE_BUFFERS = " << samp_buf
					<< " SAMPLES = " << samples << std::endl; 
					*/
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
		vi = glXGetVisualFromFBConfig(pGlobalDisplay, bestFbc);
		//std::cout << "Chosen visual ID = " << vi->visualid << std::endl;

		// Get the default screen's GLX extension list
		const char *glxExts = glXQueryExtensionsString(pGlobalDisplay, DefaultScreen(pGlobalDisplay));

		// NOTE: It is not necessary to create or make current to a context before
		// calling glXGetProcAddressARB
		glXCreateContextAttribsARBProc glXCreateContextAttribsARB = 0;
		glXCreateContextAttribsARB = (glXCreateContextAttribsARBProc)glXGetProcAddressARB( (const GLubyte *) "glXCreateContextAttribsARB" );


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
			//std::cout << "glXCreateContextAttribsARB() not found ... using old-style GLX context!" << std::endl;
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

			//std::cout << "Creating context" << std::endl;
			ctx = glXCreateContextAttribsARB(pGlobalDisplay, bestFbc, 0, True, context_attribs);

			// Sync to ensure any errors generated are processed.
			XSync( pGlobalDisplay, False );
			if ( !ctxErrorOccurred && ctx )
				;//std::cout << "Created GL 3.0 context." << std::endl;
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

				//std::cout << "Failed to create GL 3.0 context ... using old-style GLX context!" << std::endl;
				ctx = glXCreateContextAttribsARB(pGlobalDisplay, bestFbc, 0, True, context_attribs );
			}
		}

		// Sync to ensure any errors generated are processed.
		XSync( pGlobalDisplay, False );

		// Restore the original error handler
		XSetErrorHandler( oldHandler );

		if ( ctxErrorOccurred || !ctx )
		{
			//std::cout << "Failed to create an OpenGL context!" << std::endl;
			return 0;
		}

		// Verifying that context is a direct context
		if ( ! glXIsDirect ( pGlobalDisplay, ctx ) )
		{
			//std::cout << "Indirect GLX rendering context obtained." << std::endl;
		}
		else
		{
			//std::cout << "Direct GLX rendering context obtained." << std::endl;
		}

		//std::cout << "glCreateContext done." << std::endl;
		return 1;
	}

	Colormap cmap;

	Window glCreateWindow(Display* pGlobalDisplay,XVisualInfo *vi)
	{
		//std::cout << "Creating colormap." << std::endl;
		XSetWindowAttributes swa;
		swa.colormap = cmap = XCreateColormap(pGlobalDisplay,
											  RootWindow(pGlobalDisplay, vi->screen),
											  vi->visual, AllocNone);
		swa.background_pixmap = None;
		swa.border_pixel = 0;
		swa.event_mask = StructureNotifyMask;

		//std::cout << "Creating window." << std::endl;
		Window win = XCreateWindow(pGlobalDisplay, RootWindow(pGlobalDisplay, vi->screen),
								   0, 0, 100, 100, 0, vi->depth, InputOutput,
								   vi->visual,
								   CWBorderPixel | CWColormap | CWEventMask, &swa);
		if (!win)
		{
			//std::cout << "Failed to create window!" << std::endl;
			return 0;
		}

		XStoreName(pGlobalDisplay, win, "GL 3.0 Window");

		return win;
	}

	bool glvkInitBuilder(void*)
	{
		if (NULL == builder)
			builder = new CGLBuilder();

		Display* pGlobalDisplay = XOpenDisplay(NULL);
		XVisualInfo *vi = NULL;

		if (!glCreateContext(pGlobalDisplay,vi))
		{
			//std::cout << "Failed to query OpenGL context. " << std::endl;
			return false;
		}

		Window win = glCreateWindow(pGlobalDisplay,vi);
		if (0 == win)
		{
			//std::cout << "Failed to create Opengl Window for context mapping. " << std::endl;
			return false;
		}
		else
		{
			//std::cout << "Mapping window." << std::endl;
			XMapWindow(pGlobalDisplay, win);

			//std::cout << "Making context current." << std::endl;
			glXMakeCurrent(pGlobalDisplay, win, ctx);

			glClearColor(0, 0.5, 1, 1);
			glClear(GL_COLOR_BUFFER_BIT);
			glXSwapBuffers(pGlobalDisplay, win);

			sleep(1);

			glClearColor(1, 0.5, 0, 1);
			glClear(GL_COLOR_BUFFER_BIT);
			glXSwapBuffers(pGlobalDisplay, win);

			sleep(1);


			//std::cout << "Collecting OpenGL extensions." << std::endl;
			std::string glextensions = (const char*)glGetString(GL_EXTENSIONS);
			//std::cout << "glextensions:" << glextensions << std::endl;
			string version = (const char*)glGetString(GL_VERSION);
			//std::cout << "version:" << version << std::endl;
			
			glextensions += " ";
			glextensions += CGLBuilder::full_profile;
			glextensions += builder->setVersion(version);
			builder->glQueryExtensions(glextensions);

			glXMakeCurrent(pGlobalDisplay, 0, 0);
			glXDestroyContext(pGlobalDisplay, ctx);

			XDestroyWindow(pGlobalDisplay, win);
			XFreeColormap(pGlobalDisplay, cmap);
			XCloseDisplay(pGlobalDisplay);
			// Done with the visual info data
			XFree(vi);
		}

		const CPU_INFO& cpu = getCPUINFO();

		builder->activateExtension("RAPTOR_SMP_CODE_GENERATION", cpu.CPUID);
		builder->activateExtension("RAPTOR_SSE_CODE_GENERATION", cpu.hasFeature(CPU_INFO::SSE));
		builder->activateExtension("RAPTOR_SSE2_CODE_GENERATION", cpu.hasFeature(CPU_INFO::SSE2));
		builder->activateExtension("RAPTOR_SSE3_CODE_GENERATION", cpu.hasFeature(CPU_INFO::SSE3));
		builder->activateExtension("RAPTOR_SSSE3_CODE_GENERATION", cpu.hasFeature(CPU_INFO::SSSE3));
		builder->activateExtension("RAPTOR_SSE41_CODE_GENERATION", cpu.hasFeature(CPU_INFO::SSE41));
		builder->activateExtension("RAPTOR_SSE42_CODE_GENERATION", cpu.hasFeature(CPU_INFO::SSE42));
		builder->activateExtension("RAPTOR_AES_CODE_GENERATION", cpu.hasFeature(CPU_INFO::AES));
		builder->activateExtension("RAPTOR_AVX_CODE_GENERATION", cpu.hasFeature(CPU_INFO::AVX));
		builder->activateExtension("RAPTOR_FMA_CODE_GENERATION", cpu.hasFeature(CPU_INFO::FMA));

		return true;
	}
#else
	bool glvkInitBuilder(HDC dc)
	{
		return false;
	}
#endif

bool getExtensions(NATIVE_EXTENSION* ext, uint32_t *s)
{
	if (NULL == builder)
		return false;

	if (NULL == ext)
		if (NULL != s)
			*s = (uint32_t)builder->getExtensions().size();
		else
			return false;
	else
	{
		if (NULL == s)
			return false;
		const std::vector<CGLBuilder::EXTENSION> &extensions = builder->getExtensions();

		size_t sz = (*s > extensions.size() ? extensions.size() : *s);
		for (size_t i = 0; i < sz; i++)
		{
			const CGLBuilder::EXTENSION& extension = extensions[i];
			ext[i].kind = extension.kind;
			ext[i].active = extension.active;
			ext[i].extensionName = STRDUP(extension.extensionName.c_str());

			uint32_t nb = (uint32_t)extension.dependencies.size();
			ext[i].nb_dependencies = nb;
			ext[i].dependencies = (const char**)(calloc(nb+1,sizeof(char*)));
			if (NULL != ext[i].dependencies)
			{
				if (nb == 0)
					ext[i].dependencies[0] = STRDUP("");
				else for (size_t j = 0; j < nb; j++)
					ext[i].dependencies[j] = STRDUP(extension.dependencies[j].c_str());
			}
		}
	}

	return true;
}

bool freeExtensions(NATIVE_EXTENSION* extensions, uint32_t s)
{
	if (NULL == extensions)
		return false;

	for (uint32_t i = 0; i < s; i++)
	{
		NATIVE_EXTENSION& extension = extensions[i];
		free((char*)extension.extensionName);
		for (size_t j = 0; j < extension.nb_dependencies; j++)
			free((char*)(extension.dependencies[j]));
		free(extension.dependencies);
	}

	return true;
}

bool checkConsistency(bool force)
{
	if (NULL == builder)
		return false;

	return builder->checkConsistency(force);
}

bool writeHeader(const char* filename)
{
	if (NULL == builder)
		return false;

	if (NULL == filename)
		return false;

	return builder->writeHeader(filename);
}

bool activateExtension(const char* extension, bool activate)
{
	if (NULL == builder)
		return false;

	if (NULL == extension)
		return false;

	bool res = true;
	res = builder->activateExtension(extension, activate);

	return res;
}

bool setBuildSupplement(BUILD_SUPPLEMENT *bld)
{
	if (NULL == builder)
		return false;

	builder->activateDebug(bld->DEBUG);
	builder->activateRedist(bld->REDIST);
	builder->activateCompute(bld->COMPUTE);
	builder->activatePhysX(bld->PHYSX);

	return true;
}

bool activateAllOrNone(bool all)
{
	if (NULL == builder)
		return false;

	if (all)
		builder->activateAll();
	else
		builder->activateNone();

	return true;
}

bool isExtensionActive(const char* extension)
{
	if (NULL == builder)
		return false;

	if (NULL == extension)
		return false;

	bool res = true;
	res = builder->isExtensionActive(extension);

	return res;
}
