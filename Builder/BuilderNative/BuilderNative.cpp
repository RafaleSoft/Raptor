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


#include "stdafx.h"
#include <windows.h>
#include <GL\gl.h>
#include "vulkan.h"
#include <string>
//#include "BuilderNative.h"

#if !defined(AFX_GLBUILDER_H__0F2DA94E_7293_494C_B936_8CB72098E626__INCLUDED_)
	#include "GLBuilder.h"
#endif
#if !defined(__SIMDMACROS_H__)
	#include "SimdLib\simdMacros.h"
#endif

typedef const char * (APIENTRY * pfnc)(HDC hdc);

// tip to solve warning LNK4248: unresolved typeref token:
//	the types below are only predeclarations in vulkan.h
//	so here, define only the name ...
struct VkPhysicalDevice_T {};
struct VkInstance_T {};

//	The builder instance.
static CGLBuilder	*builder = NULL;

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

bool getExtensions(NATIVE_EXTENSION* ext, uint32_t *s)
{
	if (NULL == builder)
		return false;

	if (NULL == ext)
		if (NULL != s)
			*s = builder->getExtensions().size();
		else
			return false;
	else
	{
		if (NULL == s)
			return false;
		const std::vector<CGLBuilder::EXTENSION> &extensions = builder->getExtensions();

		size_t sz = max(*s, extensions.size());
		for (size_t i = 0; i < sz; i++)
		{
			const CGLBuilder::EXTENSION& extension = extensions[i];
			ext[i].kind = extension.kind;
			ext[i].active = extension.active;
			ext[i].extensionName = _strdup(extension.extensionName.c_str());

			size_t nb = extension.dependencies.size();
			ext[i].nb_dependencies = nb;
			ext[i].dependencies = (const char**)(malloc(nb));
			for (size_t j = 0; j < nb; j++)
				ext[i].dependencies[j] = _strdup(extension.dependencies[j].c_str());
		}
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