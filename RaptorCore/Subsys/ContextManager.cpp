/***************************************************************************/
/*                                                                         */
/*  ContextManager.cpp                                                     */
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


#include "Subsys/CodeGeneration.h"


#if !defined(AFX_CONTEXTMANAGER_H__F992F5F0_D8A5_475F_9777_B0EB30E7648E__INCLUDED_)
	#include "ContextManager.h"
#endif
#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
	#include "System/Raptor.h"
#endif
#if !defined(AFX_RAPTORGLEXTENSIONS_H__E5B5A1D9_60F8_4E20_B4E1_8E5A9CB7E0EB__INCLUDED_)
	#include "System/RaptorGLExtensions.h"
#endif
#if !defined(AFX_RAPTORVKEXTENSIONS_H__B17D6B7F_5AFC_4E34_9D49_8DC6CE9192D6__INCLUDED_)
	#include "System/RaptorVKExtensions.h"
#endif
#if !defined(AFX_TEXTUREFACTORY_H__1B470EC4_4B68_11D3_9142_9A502CBADC6B__INCLUDED_)
	#include "GLHierarchy/TextureFactory.h"
#endif
#if !defined(AFX_TEXTUREQUAD_H__1712AF34_6723_4E39_BC72_05ED6FA28418__INCLUDED_)
	#include "GLHierarchy/TextureQuad.h"
#endif
#if !defined(AFX_RAPTORERRORMANAGER_H__FA5A36CD_56BC_4AA1_A5F4_451734AD395E__INCLUDED_)
    #include "System/RaptorErrorManager.h"
#endif
#ifndef __GLOBAL_H__
	#include "System/Global.h"
#endif


#if defined(_WIN32)
    #if !defined(AFX_WIN32CONTEXTMANAGER_H__A1D82397_7E92_4D01_A04D_782BCFD17689__INCLUDED_)
	    #include "Win32Specific/Win32ContextManager.h"
    #endif
#endif

#if defined(LINUX)
    #if !defined(AFX_GLXCONTEXTMANAGER_H__B6CE3CDF_D7E4_4B9C_89BF_5E934062BC97__INCLUDED_)
        #include "GLXSpecific/GLXContextManager.h"
    #endif
#endif

#if !defined(AFX_RAPTORDATAMANAGER_H__114BFB19_FA00_4E3E_879E_C9130043668E__INCLUDED_)
    #include "DataManager/RaptorDataManager.h"
#endif

#if defined(VK_VERSION_1_0)
	#if !defined(AFX_RAPTORVULKANMEMORY_H__72256FF7_DBB9_4B9C_9BF7_C36F425CF811__INCLUDED_)
		#include "Subsys/Vulkan/VulkanMemory.h"
	#endif
	#if !defined(AFX_RAPTORVULKANSURFACE_H__C377C267_32A8_4963_BC2A_4694F4299A68__INCLUDED_)
		#include "Subsys/Vulkan/VulkanSurface.h"
	#endif
#endif


RAPTOR_NAMESPACE_BEGIN

	CContextManager *CContextManager::p_manager = NULL;
	const CContextManager::RENDERING_CONTEXT_ID CContextManager::INVALID_CONTEXT = -1;

RAPTOR_NAMESPACE_END
//
//////////////////////////////////////////////////////////////////////

RAPTOR_NAMESPACE

static const int MAX_CONTEXT = 16;
static CVulkanDevice defaultDevice;


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CContextManager::CContextManager()
	:pLogo(NULL),
	m_currentGLContext(CContextManager::INVALID_CONTEXT)
#if defined(VK_VERSION_1_0)
	,m_currentVKContext(CContextManager::INVALID_CONTEXT)
	,m_pVkContext(NULL)
	,vulkanModule()
#ifdef VK_KHR_swapchain
	,presentMode(VK_PRESENT_MODE_FIFO_KHR)
#endif
#endif
{

}

CContextManager::~CContextManager()
{
	p_manager = NULL;

#if defined(VK_VERSION_1_0)
	if (NULL != m_pVkContext)
		delete [] m_pVkContext;
#endif

	glRemoveLogo();
}

CContextManager *CContextManager::GetInstance(void)
{
	if (p_manager == NULL)
	{
#if defined(_WIN32)
		p_manager = new CWin32ContextManager;
#elif defined(LINUX)
        p_manager = new CGLXContextManager;
#endif
	}

	return p_manager;
}

void CContextManager::glDrawLogo(void)
{
	if (pLogo == NULL)
		pLogo = glBuildLogo();

	glPushAttrib(GL_ENABLE_BIT | GL_TEXTURE_BIT | GL_POLYGON_BIT);
	glPushMatrix();
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();

	glLoadIdentity();
	glOrtho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	pLogo->glRender();

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glPopAttrib();


	CATCH_GL_ERROR
}

CTextureQuad* CContextManager::glBuildLogo(void)
{
    CRaptorDataManager  *dataManager = CRaptorDataManager::GetInstance();
    if (NULL == dataManager)
        return NULL;

	/*
	p_Logo->glSetTransparency(256);
	if (Txt.glLoadTexture(p_Logo,"Raptor_logo_sml.tga",CGL_USER_MIPMAPPED))
		Txt.glExportCompressedTexture("Raptor_logo_sml.txt",p_Logo);
	*/

	string filepath = dataManager->ExportFile("Raptor_logo_sml.txt");
	pLogo = new CTextureQuad();
	pLogo->glLoadTexture(filepath,true);
	pLogo->glSetQuadAttributes(GL_COORD_VERTEX(0.85f, -0.925f, 1.0f, 1.0f),
							   CColor::RGBA(0.6f, 0.85f, 1.0f, 0.5f),
							   GL_COORD_VERTEX(0.15f, 0.075f, -0.15f, 0.0f));

	CATCH_GL_ERROR

    return pLogo;
}


void CContextManager::glRemoveLogo(void)
{
	//if (NULL != pLogo)
	//	delete pLogo;
	pLogo = NULL;

	CATCH_GL_ERROR
}

bool CContextManager::validateConfig(CRaptorDisplayConfig& rdc)
{
	bool res = true;

	//	validates window position to be fully visible
	//	if window not fully visible, hardware very slow...
	if (rdc.x < 0)
	{
		rdc.x = 0;
		res = false;
	}

	if (rdc.y < 0)
	{
		rdc.y = 0;
		res = false;
	}

	if (rdc.height < 1)
	{
		rdc.height = 1;
		res = false;
    }

	if (rdc.width < 1)
	{
		rdc.width = 1;
		res = false;
    }

	//	validates refresh rate
	if (rdc.refresh_rate.fps<CGL_MINREFRESHRATE)
	{
		rdc.refresh_rate.fps = CGL_MINREFRESHRATE;
		res = false;
	}
	else if (rdc.refresh_rate.fps>CGL_MAXREFRESHRATE)
	{
		rdc.refresh_rate.fps = CGL_MAXREFRESHRATE;
		res = false;
	}

	return res;
}

const CRaptorVKExtensions *const CContextManager::vkGetExtensions(void)
{
#if defined(VK_VERSION_1_0)
	if ((m_currentVKContext >= 0) && (m_currentVKContext < MAX_CONTEXT))
		return m_pVkContext[m_currentVKContext].pExtensions;
	else
#endif
		return NULL;
}

#if defined(VK_VERSION_1_0)
bool CContextManager::vkInit(void)
{
	if (!CRaptorVKExtensions::vkInitInstanceExtensions())
		return false;

	if (NULL != m_pVkContext)
	{
		for (RENDERING_CONTEXT_ID ctx = 0; ctx < MAX_CONTEXT; ctx++)
			vkDestroyContext(ctx);
		delete[] m_pVkContext;
	}

	m_pVkContext = new VK_CONTEXT[MAX_CONTEXT];
	for (RENDERING_CONTEXT_ID ctx = 0; ctx < MAX_CONTEXT; ctx++)
	{
		VK_CONTEXT &ctxt = m_pVkContext[ctx];
		
		ctxt.pExtensions = NULL;
		ctxt.pSurface = NULL;
		ctxt.physicalDevice = VK_NULL_HANDLE;
	}

#ifdef VK_KHR_swapchain
	presentMode = VK_PRESENT_MODE_FIFO_KHR;
#endif

	return true;
}

bool CContextManager::vkRelease(void)
{
	for (RENDERING_CONTEXT_ID ctx = 0; ctx < MAX_CONTEXT; ctx++)
		vkDestroyContext(ctx);

	VkInstance instance = CRaptorVKExtensions::getInstance();
	if ((VK_NULL_HANDLE != instance) && (NULL != CRaptorVKExtensions::vkDestroyInstance))
	{
		CRaptorVKExtensions::vkDestroyInstance(instance, CVulkanMemory::GetAllocator());
		return true;
	}
	else
		return false;
}

bool CContextManager::vkInitDevice(CContextManager::RENDERING_CONTEXT_ID ctx,const CRaptorDisplayConfig& config)
{
	VK_CONTEXT &vk_ctx = m_pVkContext[ctx];
	CRaptorErrorManager *pErrMgr = Raptor::GetErrorManager();
	VkResult res = VK_NOT_READY;

	unsigned int nbPhysicalDevices = 0;
	VkPhysicalDevice *pPhysicalDevices = NULL;
	VkPhysicalDeviceProperties *pProperties = NULL;
	
	VkInstance instance = CRaptorVKExtensions::getInstance();
	res = CRaptorVKExtensions::vkEnumeratePhysicalDevices(instance, &nbPhysicalDevices, NULL);
	if ((VK_SUCCESS == res) && (nbPhysicalDevices > 0))
	{
		pPhysicalDevices = new VkPhysicalDevice[nbPhysicalDevices];
		res = CRaptorVKExtensions::vkEnumeratePhysicalDevices(instance, &nbPhysicalDevices, pPhysicalDevices);
		if (VK_SUCCESS == res)
		{
			pProperties = new VkPhysicalDeviceProperties[nbPhysicalDevices];

			for (uint32_t i = 0; i<nbPhysicalDevices; i++)
			{
				VkPhysicalDevice device = pPhysicalDevices[i];
				CRaptorVKExtensions::vkGetPhysicalDeviceProperties(device, &pProperties[i]);
			}
		}
		else
		{
			nbPhysicalDevices = 0;
			delete[] pPhysicalDevices;
		}
	}

	if ((NULL == pPhysicalDevices) || (NULL == pProperties))
		return false;

	uint32_t numDevice = MAXUINT;
	uint32_t queueCount = 0;
	uint32_t graphicsQueueFamilyIndex = MAXUINT;
	uint32_t presentQueueFamilyIndex = MAXUINT;
	uint32_t transferQueueFamilyIndex = MAXUINT;

	//!	Find a device with graphic & win32 presentation support
	for (unsigned int i = 0; (i<nbPhysicalDevices) && (numDevice > nbPhysicalDevices); i++)
	{
		VkPhysicalDevice physical = pPhysicalDevices[i];
		//! Not confirmed if a CPU type is able to render graphics
		if ((pProperties[i].deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU) ||
			(pProperties[i].deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) ||
			(pProperties[i].deviceType == VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU))
		{
			vk_ctx.physicalDevice = physical;
			uint32_t numQueue = getPresentationSuppotQueueFamily(ctx);
			if (MAXUINT == numQueue)
				continue;

			uint32_t pQueueFamilyPropertyCount = numQueue + 1;
				
			VkQueueFamilyProperties *pQueueFamilyProperties = new VkQueueFamilyProperties[pQueueFamilyPropertyCount];
			CRaptorVKExtensions::vkGetPhysicalDeviceQueueFamilyProperties(physical, &pQueueFamilyPropertyCount, pQueueFamilyProperties);
			
			const VkQueueFamilyProperties& props = pQueueFamilyProperties[numQueue];
			if (0 == props.queueCount)
				continue;

			VkBool32 support = (MAXUINT != numQueue);
			VkBool32 present_support = vk_ctx.pSurface->supportPresentation(physical, numQueue,vk_ctx.pExtensions);

			if (VK_QUEUE_GRAPHICS_BIT == (VK_QUEUE_GRAPHICS_BIT & props.queueFlags))
			{
				if ((props.queueCount > queueCount) || (graphicsQueueFamilyIndex == MAXUINT))
				{
					queueCount = props.queueCount;
					graphicsQueueFamilyIndex = numQueue;
				}

				//	If queue supports both graphics & presentation, select it.
				if ((VK_TRUE == support) && (VK_TRUE == present_support))
				{
					presentQueueFamilyIndex = numQueue;
					graphicsQueueFamilyIndex = numQueue;
				}
			}

			if (VK_QUEUE_TRANSFER_BIT == (VK_QUEUE_TRANSFER_BIT & props.queueFlags))
				transferQueueFamilyIndex = numQueue;

			if ((VK_TRUE == support) && (VK_TRUE == present_support))
				presentQueueFamilyIndex = numQueue;
			
			//!	Here, we always select a device with presentable queue.
			//!	For future : allow selection of device not for presentation if
			//!	specificaly required.
			if ((graphicsQueueFamilyIndex < MAXUINT) &&
				(presentQueueFamilyIndex < MAXUINT) &&
				(transferQueueFamilyIndex < MAXUINT))
			{
				numDevice = i;
				vk_ctx.physicalDevice = physical;
				break;
			}

			delete [] pQueueFamilyProperties;
		}
	}

	//! Check Device properties to comply with requested config
	if (numDevice < nbPhysicalDevices)
	{
		VkPhysicalDeviceProperties props = pProperties[numDevice];
		VkPhysicalDeviceLimits limits = props.limits;
		if ((limits.maxViewports < 1) ||
			(limits.maxViewportDimensions[0] < config.width) ||
			(limits.maxViewportDimensions[1] < config.height))
			return false;
		
		vk_ctx.pExtensions->vkInitDeviceExtensions(vk_ctx.physicalDevice);

		//make common initialisation in CTextureFactoryConfig

		return vk_ctx.device.vkCreateLogicalDevice(	vk_ctx.physicalDevice,
													props,
													graphicsQueueFamilyIndex,
													queueCount,
													presentQueueFamilyIndex,1,
													transferQueueFamilyIndex,1);
	}
	else
		return false;
}


void CContextManager::vkResize(RENDERING_CONTEXT_ID ctx,const CRaptorDisplayConfig& config)
{
	VK_CONTEXT &context = m_pVkContext[ctx];
#if defined(VK_KHR_display)
	if (!context.pSurface->vkResize(context.physicalDevice))
	{
		RAPTOR_ERROR(	Global::CVulkanClassID::GetClassId(),
						"Failed to obtain Vulkan surface capabilities");
	}
	else
	{
		if (!context.device.vkCreateSwapChain(context.pSurface, config))
		{
			RAPTOR_ERROR(	Global::CVulkanClassID::GetClassId(),
							"Failed to recreate Vulkan swap chain");
		}
	}
#endif
}

CContextManager::RENDERING_CONTEXT_ID CContextManager::vkCreateContext(const RAPTOR_HANDLE& handle,
																	   const CRaptorDisplayConfig& config)
{
	RENDERING_CONTEXT_ID ctx = 0;
	while ((ctx < MAX_CONTEXT) && (NULL != m_pVkContext[ctx].pExtensions))
		ctx++;
	if (ctx >= MAX_CONTEXT)
	{
		RAPTOR_ERROR(Global::CVulkanClassID::GetClassId(), "Too many Vulkan Context created");
		return CContextManager::INVALID_CONTEXT;
	}

	VK_CONTEXT &context = m_pVkContext[ctx];
	context.pExtensions = new CRaptorVKExtensions("");
	
	if (WINDOW_CLASS == handle.hClass)
	{
		if (!vkCreateSurface(handle, ctx))
		{
			vkDestroyContext(ctx);
			return CContextManager::INVALID_CONTEXT;
		}
	}

	if (!vkInitDevice(ctx,config))
	{
		vkDestroyContext(ctx);
		return CContextManager::INVALID_CONTEXT;
	}

#ifdef VK_KHR_surface
	if (!context.pSurface->vkInitSurface(context.physicalDevice, context.pExtensions))
	{
		vkDestroyContext(ctx);
		return CContextManager::INVALID_CONTEXT;
	}

	if (!context.device.vkCreateSwapChain(context.pSurface, config))
	{
		vkDestroyContext(ctx);
		return CContextManager::INVALID_CONTEXT;
	}
#endif
	return ctx;
}

void CContextManager::vkMakeCurrentContext(const RAPTOR_HANDLE& device,RENDERING_CONTEXT_ID ctx)
{
	if ((ctx >= 0) && (ctx < MAX_CONTEXT))
	{
		//	TODO
		//	Use a timeout coherent with vkSwapVSync
		uint64_t timeout = MAXUINT64;	// infinite wait
		VK_CONTEXT& context = m_pVkContext[ctx];
		if (NULL != device.hClass)
		{
			if (!context.device.acquireSwapChainImage(timeout))
			{
				CRaptorErrorManager *pErrMgr = Raptor::GetErrorManager();
				pErrMgr->generateRaptorError(	Global::CVulkanClassID::GetClassId(),
												CRaptorErrorManager::RAPTOR_WARNING,
												"Vulkan Device failed to provide rendering Image!");

				// onWindowSizeChanged()
				m_currentVKContext = CContextManager::INVALID_CONTEXT;
			}
			else
				m_currentVKContext = ctx;
		}
	}
	else
		m_currentVKContext = CContextManager::INVALID_CONTEXT;
}

void CContextManager::vkSwapBuffers(RENDERING_CONTEXT_ID ctx)
{
	if ((ctx >= 0) && (ctx < MAX_CONTEXT))
	{
		VK_CONTEXT& context = m_pVkContext[ctx];
		if (!context.device.presentSwapChainImage())
		{
			CRaptorErrorManager *pErrMgr = Raptor::GetErrorManager();
			pErrMgr->generateRaptorError(	Global::CVulkanClassID::GetClassId(),
											CRaptorErrorManager::RAPTOR_WARNING,
											"Vulkan Device failed to submit and present rendering queue!");

			// onWindowSizeChanged()
		}
	}
}



void CContextManager::vkDestroyContext(RENDERING_CONTEXT_ID ctx)
{
	if ((ctx >= 0) && (ctx < MAX_CONTEXT))
	{
		VK_CONTEXT& context = m_pVkContext[ctx];
		
		if (context.device.vkDestroyLogicalDevice())
		{
			if (NULL != context.pSurface)
				delete context.pSurface;
			context.pSurface = NULL;
		}

		if (NULL != context.pExtensions)
			delete context.pExtensions;
		context.pExtensions = NULL;
	}
	else
	{
		CRaptorErrorManager *pErrMgr = Raptor::GetErrorManager();
		pErrMgr->generateRaptorError(	Global::CVulkanClassID::GetClassId(),
										CRaptorErrorManager::RAPTOR_ERROR,
										"Unable to destroy unknown Vulkan context");
	}
}

CVulkanDevice& CContextManager::vkGetDevice(RENDERING_CONTEXT_ID ctx) const
{
	if ((ctx >= 0) && (ctx < MAX_CONTEXT))
	{
		VK_CONTEXT& context = m_pVkContext[ctx];
		return context.device;
	}
	else
		return defaultDevice;
}

RAPTOR_HANDLE CContextManager::getDevice(RENDERING_CONTEXT_ID ctx) const
{
	RAPTOR_HANDLE device;
	if ((ctx >= 0) && (ctx < MAX_CONTEXT))
	{
		device.handle = 0;
		device.hClass = DEVICE_CONTEXT_CLASS;
	}

	return device;
}
#endif