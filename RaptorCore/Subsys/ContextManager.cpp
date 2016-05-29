// ContextManager.cpp: implementation of the CContextManager class.
//
//////////////////////////////////////////////////////////////////////

#include "Subsys/CodeGeneration.h"


#if !defined(AFX_CONTEXTMANAGER_H__F992F5F0_D8A5_475F_9777_B0EB30E7648E__INCLUDED_)
	#include "ContextManager.h"
#endif
#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
	#include "System/Raptor.h"
#endif
#if !defined(AFX_RAPTOREXTENSIONS_H__E5B5A1D9_60F8_4E20_B4E1_8E5A9CB7E0EB__INCLUDED_)
	#include "System/RaptorExtensions.h"
#endif
#if !defined(AFX_TEXTUREFACTORY_H__1B470EC4_4B68_11D3_9142_9A502CBADC6B__INCLUDED_)
	#include "GLHierarchy/TextureFactory.h"
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


RAPTOR_NAMESPACE_BEGIN

	CContextManager *CContextManager::p_manager = NULL;

RAPTOR_NAMESPACE_END
//
//////////////////////////////////////////////////////////////////////

static const int MAX_CONTEXT = 16;

RAPTOR_NAMESPACE

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CContextManager::CContextManager()
	:m_logo()
#if defined(VK_VERSION_1_0)
	,m_pExtensions(NULL)
	,m_globalInstance(NULL)
	,m_nbPhysicalDevices(0)
	,m_pPhysicalDevices(NULL)
	,m_pProperties(NULL)
	,m_pFeatures(NULL)
	,m_pVkContext(NULL)
#endif
{
#if defined(VK_VERSION_1_0)
	m_pVkContext = new VK_CONTEXT[MAX_CONTEXT];
	for (unsigned int i=0;i<MAX_CONTEXT;i++)
	{
		VK_CONTEXT &ctxt = m_pVkContext[i];
		ctxt.device = NULL;
		ctxt.commandPool = NULL;
#ifdef VK_KHR_win32_surface
		ctxt.surface = NULL;
	#ifdef VK_KHR_swapchain
		ctxt.swapChain = NULL;
	#endif
		ctxt.pImages = NULL;
		ctxt.currentImage = MAXUINT;
#endif
	}

#ifdef VK_KHR_swapchain
	presentMode = VK_PRESENT_MODE_FIFO_KHR;
#endif

#endif
}

CContextManager::~CContextManager()
{
	p_manager = NULL;

#if defined(VK_VERSION_1_0)
	if (NULL != m_globalInstance)
		m_pExtensions->vkDestroyInstance(m_globalInstance,NULL);

	if (NULL != m_pExtensions)
		delete m_pExtensions;

	if (NULL != m_pPhysicalDevices)
		delete [] m_pPhysicalDevices;
	if (NULL != m_pProperties)
		delete [] m_pProperties;
	if (NULL != m_pFeatures)
		delete [] m_pFeatures;

	for (unsigned int i=0;i<MAX_CONTEXT;i++)
		vkDestroyContext(i);

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
	if (m_logo.handle != NULL)
		glCallList(m_logo.handle);
	else
	{
		if (m_pLogo == NULL)
			m_pLogo = glBuildLogo();

		m_logo.handle = glGenLists(1);
		glNewList(m_logo.handle,GL_COMPILE_AND_EXECUTE);

		glPushAttrib(GL_ENABLE_BIT|GL_TEXTURE_BIT|GL_POLYGON_BIT);
		glPushMatrix();
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();

		glLoadIdentity();
		glOrtho(-1.0f,1.0f,-1.0f,1.0f,-1.0,1.0);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_LIGHTING);
		glDisable(GL_CULL_FACE);
		glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	#if defined(GL_ARB_multitexture)
		const CRaptorExtensions *const pExtensions = Raptor::glGetExtensions();
		if (pExtensions->glActiveTextureARB != NULL)
		{
			pExtensions->glActiveTextureARB(GL_TEXTURE3_ARB);
			glDisable(GL_TEXTURE_2D);
			pExtensions->glActiveTextureARB(GL_TEXTURE2_ARB);
			glDisable(GL_TEXTURE_2D);
			pExtensions->glActiveTextureARB(GL_TEXTURE1_ARB);
			glDisable(GL_TEXTURE_2D);
			pExtensions->glActiveTextureARB(GL_TEXTURE0_ARB);
		}
	#endif

	#if defined(GL_ARB_fragment_program)
		glDisable(GL_FRAGMENT_PROGRAM_ARB);
	#endif
	#if defined(GL_NV_texture_shader)
		glDisable(GL_TEXTURE_SHADER_NV);
	#endif

		glEnable(GL_TEXTURE_2D);
		glColor4f(1.0f,1.0f,1.0f,0.75f);
		if (!(m_pLogo == NULL))
			m_pLogo->glRender();
		glBegin(GL_QUADS);
			glTexCoord2f(0.0f,1.0f);		glVertex3f(0.7f,-0.85f,1.0f);
			glTexCoord2f(0.0f,0.0f);		glVertex3f(0.7f,-1.0f,1.0f);
			glTexCoord2f(1.0f,0.0f);		glVertex3f(1.0f,-1.0f,1.0f);
			glTexCoord2f(1.0f,1.0f);		glVertex3f(1.0f,-0.85f,1.0f);
		glEnd();

		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();
		glPopAttrib();

		glEndList();
	}

	CATCH_GL_ERROR
}

CTextureObject* CContextManager::glBuildLogo(void)
{
    glPushAttrib(GL_TEXTURE_BIT);

	CTextureFactory &Txt = CTextureFactory::getDefaultFactory();
	CTextureObject *p_Logo = Txt.glCreateTexture(	CTextureObject::CGL_COLOR24_ALPHA,
													CTextureObject::CGL_MULTIPLY,
													CTextureObject::CGL_BILINEAR);

    CRaptorDataManager  *dataManager = CRaptorDataManager::getInstance();
    if (dataManager == NULL)
        return NULL;

	string filepath = dataManager->ExportFile("Raptor_logo_sml.txt");
	if (!filepath.empty())
    {
        p_Logo->glSetTransparency(256);
		Txt.glLoadCompressedTexture(p_Logo,filepath);
    }
	/*
	p_Logo->glSetTransparency(256);
	if (Txt.glLoadTexture(p_Logo,"Raptor_logo_sml.tga",CGL_USER_MIPMAPPED))
		Txt.glExportCompressedTexture("Raptor_logo_sml.txt",p_Logo);
	*/

    glPopAttrib();

	CATCH_GL_ERROR

    return p_Logo;
}


void CContextManager::glRemoveLogo(void)
{
    m_pLogo = NULL;

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

#if defined(VK_VERSION_1_0)
bool CContextManager::vkInitContext(void)
{
	if (NULL == m_pExtensions)
		m_pExtensions = new CRaptorExtensions("");
	m_pExtensions->glInitExtensions();

	CRaptorErrorManager *pErrMgr = Raptor::GetErrorManager();
	if (!m_pExtensions->glIsExtensionSupported("VK_KHR_surface") ||
		!m_pExtensions->glIsExtensionSupported("VK_KHR_win32_surface"))
	{
		pErrMgr->generateRaptorError(	Global::CVulkanClassID::GetClassId(),
										CRaptorErrorManager::RAPTOR_ERROR,
										"Vulkan API does not support rendering to surface");
		return false;
	}

	m_nbPhysicalDevices = 0;
	if (NULL != m_pPhysicalDevices)
		delete [] m_pPhysicalDevices;
	if (NULL != m_pProperties)
		delete [] m_pProperties;
	if (NULL != m_pFeatures)
		delete [] m_pFeatures;

	VkResult res = VK_NOT_READY;
	VkApplicationInfo applicationInfo = {	VK_STRUCTURE_TYPE_APPLICATION_INFO, NULL,
											"Raptor", RAPTOR_VERSION,
											"Raptor 3D Engine", RAPTOR_VERSION,
											VK_API_VERSION };
	const char* extensions[2] = { "VK_KHR_surface", "VK_KHR_win32_surface" };
	VkInstanceCreateInfo instanceCreateInfo = {	VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO, // VkStructureType sType;
												NULL,                                   // const void* pNext;
												0,                                      // VkInstanceCreateFlags flags;
												&applicationInfo,						// const VkApplicationInfo* pApplicationInfo;
												0,                                      // uint32_t enabledLayerNameCount;
												NULL,                                   // const char* const* ppEnabledLayerNames;
												2,                                      // uint32_t enabledExtensionNameCount;
												extensions};							// const char* const* ppEnabledExtensionNames;
	res = m_pExtensions->vkCreateInstance(&instanceCreateInfo, NULL, &m_globalInstance);
	if (VK_SUCCESS != res)
		pErrMgr->vkGetError(res,__FILE__,__LINE__);
	
	//	Instance properties are already collected by CRaptorExtensions, skip it here.
	/*
	if (VK_SUCCESS == res)
	{
		uint32_t pPropertyCount = 0;
		res = m_pExtensions->vkEnumerateInstanceExtensionProperties(NULL,&pPropertyCount,NULL);
		if ((VK_SUCCESS == res) && (pPropertyCount > 0))
		{
			VkExtensionProperties *pProperties = new VkExtensionProperties[pPropertyCount];
			res = m_pExtensions->vkEnumerateInstanceExtensionProperties(NULL,&pPropertyCount,pProperties);
			if (VK_SUCCESS == res)
			{
				for (uint32_t i=0;i<pPropertyCount;i++)
				{
				}
			}
			delete [] pProperties;
		}
		if (VK_SUCCESS != res)
			pErrMgr->vkGetError(res,__FILE__,__LINE__);
	}*/

	if (VK_SUCCESS == res)
	{
		uint32_t pPhysicalDeviceCount = 0;
		res = m_pExtensions->vkEnumeratePhysicalDevices(m_globalInstance,
														&pPhysicalDeviceCount,NULL);
		if ((VK_SUCCESS == res) && (pPhysicalDeviceCount > 0))
		{
			m_pPhysicalDevices = new VkPhysicalDevice[pPhysicalDeviceCount];
			m_pProperties = new VkPhysicalDeviceProperties[pPhysicalDeviceCount];
			m_pFeatures = new VkPhysicalDeviceFeatures[pPhysicalDeviceCount];
			
			res = m_pExtensions->vkEnumeratePhysicalDevices(m_globalInstance,
															&pPhysicalDeviceCount,m_pPhysicalDevices);
			if (VK_SUCCESS == res)
			{
				for (uint32_t i=0;i<pPhysicalDeviceCount;i++)
				{
					m_pExtensions->vkGetPhysicalDeviceProperties(m_pPhysicalDevices[i],
																 &m_pProperties[i]);
					m_pExtensions->vkGetPhysicalDeviceFeatures(m_pPhysicalDevices[i],
															   &m_pFeatures[i]);
				}
			}
		}
		if (VK_SUCCESS != res)
			pErrMgr->vkGetError(res,__FILE__,__LINE__);
		else
			m_nbPhysicalDevices = pPhysicalDeviceCount;
	}

	if (VK_SUCCESS != res)
	{
		pErrMgr->generateRaptorError(	Global::CVulkanClassID::GetClassId(),
										CRaptorErrorManager::RAPTOR_ERROR,
										"Unable to initialise Vulkan API");
		m_nbPhysicalDevices = 0;
		if (NULL != m_pPhysicalDevices)
			delete [] m_pPhysicalDevices;
		if (NULL != m_pProperties)
			delete [] m_pProperties;
		if (NULL != m_pFeatures)
			delete [] m_pFeatures;
	}
	return (VK_SUCCESS == res);
}

CContextManager::RENDERING_CONTEXT_ID CContextManager::vkCreateContext(const RAPTOR_HANDLE& handle,int displayMode)
{
	if (m_nbPhysicalDevices < 1)
		return -1;

	size_t nbDevices = 0;
	for (size_t i=0;i<MAX_CONTEXT;i++)
		nbDevices += (VK_NULL_HANDLE != m_pVkContext[i].device ? 1 : 0);

	if (nbDevices >= MAX_CONTEXT)
	{
		RAPTOR_ERROR( Global::CVulkanClassID::GetClassId(),"Too many Vulkan Context created");
		return -1;
	}

	unsigned int physicalDevice = m_nbPhysicalDevices;
	uint32_t queueFamilyIndex = 0;
	uint32_t queueCount = 0;

	for (unsigned int i=0; (i<m_nbPhysicalDevices) && (physicalDevice == m_nbPhysicalDevices); i++)
	{
		//! Not confirmed if a CPU type is able to render graphics
		if ((m_pProperties[i].deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU) ||
			(m_pProperties[i].deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) ||
			(m_pProperties[i].deviceType == VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU))
		{
			uint32_t pQueueFamilyPropertyCount = 0;
			m_pExtensions->vkGetPhysicalDeviceQueueFamilyProperties(m_pPhysicalDevices[i],
																	&pQueueFamilyPropertyCount,NULL);
			if (pQueueFamilyPropertyCount > 0)
			{
				VkQueueFamilyProperties *pQueueFamilyProperties = new VkQueueFamilyProperties[pQueueFamilyPropertyCount];
				m_pExtensions->vkGetPhysicalDeviceQueueFamilyProperties(m_pPhysicalDevices[i],
																		&pQueueFamilyPropertyCount,
																		pQueueFamilyProperties);
				for (uint32_t j=0;j<pQueueFamilyPropertyCount;j++)
				{
					if ((VK_QUEUE_GRAPHICS_BIT == (VK_QUEUE_GRAPHICS_BIT & pQueueFamilyProperties[j].queueFlags)) &&
						(VK_QUEUE_TRANSFER_BIT == (VK_QUEUE_TRANSFER_BIT & pQueueFamilyProperties[j].queueFlags)))
					{
						VkBool32 support = VK_FALSE;
#if defined(VK_USE_PLATFORM_WIN32_KHR)
						support = m_pExtensions->vkGetPhysicalDeviceWin32PresentationSupportKHR(m_pPhysicalDevices[i],j);
#elif defined(VK_USE_PLATFORM_XLIB_KHR)
						support = m_pExtensions->PFN_vkGetPhysicalDeviceXlibPresentationSupportKHR(m_pPhysicalDevices[i],j);
#else
						RAPTOR_WARNING( Global::CVulkanClassID::GetClassId(),"No support for Vulkan WSI");
						support = VK_TRUE;
#endif
						if (VK_TRUE == support)
						{
							physicalDevice = i;
							queueFamilyIndex = j;
							queueCount = pQueueFamilyProperties[j].queueCount;
						}
					}
				}

				delete [] pQueueFamilyProperties;
			}
		}

#ifdef VK_KHR_display
//		uint32_t pPropertyCount = 0;
//		VkResult res = m_pExtensions->vkGetPhysicalDeviceDisplayPropertiesKHR(m_pPhysicalDevices[i],&pPropertyCount,NULL);
//		if ((VK_SUCCESS == res) && (pPropertyCount > 0))
//		{
//			VkDisplayPropertiesKHR *pProperties = new VkDisplayPropertiesKHR[pPropertyCount];
//			res = m_pExtensions->vkGetPhysicalDeviceDisplayPropertiesKHR(m_pPhysicalDevices[i],&pPropertyCount,pProperties);
//			delete [] pProperties;
//		}
#endif
	}

	if (physicalDevice < m_nbPhysicalDevices)
	{
		float *queuePriorities = new float[queueCount];
		for (unsigned int i=0;i<queueCount;i++)
			queuePriorities[i] = 1.0f;

		VkDeviceQueueCreateInfo queueCreateInfo = {	VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
													NULL,
													0,
													queueFamilyIndex,
													queueCount,
													queuePriorities };
		VkDeviceCreateInfo deviceCreateInfo = {	VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
												NULL, 0,
												1, &queueCreateInfo,
												0, NULL,	//	layers
												0, NULL,	//	extensions
												&m_pFeatures[physicalDevice] };

		unsigned int pos = 0;
		while ((pos < MAX_CONTEXT) && (m_pVkContext[pos].device != NULL))
			pos++;

		VkDevice device = NULL;
		VkResult res = m_pExtensions->vkCreateDevice(m_pPhysicalDevices[physicalDevice],
													 &deviceCreateInfo,NULL,&device);

		delete [] queuePriorities;
		CRaptorErrorManager *pErrMgr = Raptor::GetErrorManager();
		if (VK_SUCCESS != res)
		{
			pErrMgr->vkGetError(res,__FILE__,__LINE__);
			return -1;
		}
		

		VkCommandPoolCreateInfo commandPoolCreateInfo = {	VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
															NULL,
															VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
															queueFamilyIndex };

		VkCommandPool commandPool = NULL;
		res = m_pExtensions->vkCreateCommandPool(	device, &commandPoolCreateInfo,
													NULL, &commandPool);

		if (VK_SUCCESS != res)
		{
			pErrMgr->vkGetError(res,__FILE__,__LINE__);
			m_pExtensions->vkDestroyDevice(device,NULL);
			return -1;
		}
		else if (VK_SUCCESS != m_pExtensions->vkResetCommandPool(device,
																commandPool,
																VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT))
			{
				pErrMgr->vkGetError(res,__FILE__,__LINE__);
				m_pExtensions->vkDestroyCommandPool(device,commandPool,NULL);
				m_pExtensions->vkDestroyDevice(device,NULL);
				return -1;
			}

#ifdef VK_KHR_win32_surface
		VkSurfaceKHR surface;	// otherwise undefined ?
		VkSurfaceCapabilitiesKHR pSurfaceCapabilities;
		VkSurfaceFormatKHR* formats = NULL;
#ifdef VK_KHR_swapchain
		VkSwapchainKHR pSwapChain;
#endif
		VkImage *images = NULL;

		if (WINDOW_CLASS == handle.hClass)
		{
			HWND hWnd = (HWND)handle.handle;
			HINSTANCE hInstance = (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE);
			VkWin32SurfaceCreateInfoKHR createInfo = {	VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
														NULL,0, //flags,
														hInstance,hWnd };
			res = m_pExtensions->vkCreateWin32SurfaceKHR(m_globalInstance,&createInfo,NULL,&surface);
			if (VK_SUCCESS != res)
			{
				pErrMgr->vkGetError(res,__FILE__,__LINE__);
				m_pExtensions->vkDestroyCommandPool(device,commandPool,NULL);
				m_pExtensions->vkDestroyDevice(device,NULL);
				return -1;
			}

			VkBool32 pSupported;
			res = m_pExtensions->vkGetPhysicalDeviceSurfaceSupportKHR(	m_pPhysicalDevices[physicalDevice],
																		queueFamilyIndex,
																		surface,
																		&pSupported);
			if ((VK_SUCCESS != res) || (VK_FALSE == pSupported))
			{
				pErrMgr->vkGetError(res,__FILE__,__LINE__);
				m_pExtensions->vkDestroyCommandPool(device,commandPool,NULL);
				m_pExtensions->vkDestroyDevice(device,NULL);
				return -1;
			}

			res = m_pExtensions->vkGetPhysicalDeviceSurfaceCapabilitiesKHR(	m_pPhysicalDevices[physicalDevice],
																			surface,
																			&pSurfaceCapabilities);
			if (VK_SUCCESS != res)
				pErrMgr->vkGetError(res,__FILE__,__LINE__);

			uint32_t pSurfaceFormatCount = 0;
			res = m_pExtensions->vkGetPhysicalDeviceSurfaceFormatsKHR(	m_pPhysicalDevices[physicalDevice],
																		surface,
																		&pSurfaceFormatCount,
																		NULL);
			if ((VK_SUCCESS != res) || (pSurfaceFormatCount < 1))
				pErrMgr->vkGetError(res,__FILE__,__LINE__);
			else
			{
				formats = new VkSurfaceFormatKHR[pSurfaceFormatCount];
				res = m_pExtensions->vkGetPhysicalDeviceSurfaceFormatsKHR(	m_pPhysicalDevices[physicalDevice],
																			surface,
																			&pSurfaceFormatCount,
																			formats);
				if (VK_SUCCESS != res)
					pErrMgr->vkGetError(res,__FILE__,__LINE__);
			}

			uint32_t pPresentModeCount = 0;
			res = m_pExtensions->vkGetPhysicalDeviceSurfacePresentModesKHR(	m_pPhysicalDevices[physicalDevice],
																			surface,
																			&pPresentModeCount,
																			NULL);
			if (VK_SUCCESS != res)
				pErrMgr->vkGetError(res,__FILE__,__LINE__);
			else
			{
				VkPresentModeKHR* pPresentModes = new VkPresentModeKHR[pPresentModeCount];
				res = m_pExtensions->vkGetPhysicalDeviceSurfacePresentModesKHR(	m_pPhysicalDevices[physicalDevice],
																				surface,
																				&pPresentModeCount,
																				pPresentModes);
				if (VK_SUCCESS != res)
					pErrMgr->vkGetError(res,__FILE__,__LINE__);
			}

		}

	#ifdef VK_KHR_swapchain
		VkSwapchainCreateInfoKHR pCreateInfo = {VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
												NULL, 0, surface,
												pSurfaceCapabilities.maxImageCount,	// for swapChain
												formats[0].format, formats[0].colorSpace,
												pSurfaceCapabilities.currentExtent,
												1,
												VK_IMAGE_USAGE_TRANSFER_SRC_BIT, // ???
												VK_SHARING_MODE_EXCLUSIVE,
												0, NULL,
												pSurfaceCapabilities.currentTransform, // VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR ?
												VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
												presentMode,
												VK_TRUE,	// Hard clipping, no read back
												VK_NULL_HANDLE };
		res = m_pExtensions->vkCreateSwapchainKHR(device,&pCreateInfo,NULL,&pSwapChain);
		if (VK_SUCCESS != res)
		{
			pErrMgr->vkGetError(res,__FILE__,__LINE__);
			m_pExtensions->vkDestroySurfaceKHR(m_globalInstance,surface,NULL);
			m_pExtensions->vkDestroyCommandPool(device,commandPool,NULL);
			m_pExtensions->vkDestroyDevice(device,NULL);
			return -1;
		}

		uint32_t pSwapchainImageCount = 0;
		res = m_pExtensions->vkGetSwapchainImagesKHR(	device,
														pSwapChain,
														&pSwapchainImageCount,
														NULL);
		if ((VK_SUCCESS != res) || (pSwapchainImageCount == 0))
			pErrMgr->vkGetError(res,__FILE__,__LINE__);
		else
		{
			images = new VkImage[pSwapchainImageCount];
			res = m_pExtensions->vkGetSwapchainImagesKHR(	device,
															pSwapChain,
															&pSwapchainImageCount,
															images);
			if (VK_SUCCESS != res)
			{
				pErrMgr->vkGetError(res,__FILE__,__LINE__);
				delete [] images;
				images = NULL;
			}
		}

		if (VK_SUCCESS != res)
		{
			pErrMgr->vkGetError(res,__FILE__,__LINE__);
			m_pExtensions->vkDestroySurfaceKHR(m_globalInstance,surface,NULL);
			m_pExtensions->vkDestroyCommandPool(device,commandPool,NULL);
			m_pExtensions->vkDestroyDevice(device,NULL);
			return -1;
		}
	#endif

		if (NULL != formats)
			delete [] formats;
#endif

		VK_CONTEXT &context = m_pVkContext[pos];
#ifdef VK_KHR_win32_surface
		context.surface = surface;
	#ifdef VK_KHR_swapchain
		context.swapChain = pSwapChain;
	#endif
		context.currentImage = MAXUINT;
		context.pImages = images;
#endif
		context.device = device;
		context.commandPool = commandPool;
		return pos;
	}
	else
		return -1;
}

void CContextManager::vkMakeCurrentContext(const RAPTOR_HANDLE& device,RENDERING_CONTEXT_ID ctx)
{
	if ((ctx >= 0) && (ctx < MAX_CONTEXT))
	{
		//	TODO
		//	Use a timeout coherent with vkSwapVSync
		uint64_t timeout = 0;
		VK_CONTEXT& context = m_pVkContext[ctx];
		if (NULL != device.hClass)
		{
			CRaptorErrorManager *pErrMgr = Raptor::GetErrorManager();
			if (context.currentImage == MAXUINT)
			{
#ifdef VK_KHR_swapchain
				uint32_t imageIndex = 0;
				VkResult res = m_pExtensions->vkAcquireNextImageKHR(context.device,
																	context.swapChain,
																	timeout,
																	VK_NULL_HANDLE,
																	VK_NULL_HANDLE,
																	&imageIndex);
				if (VK_SUCCESS == res)
					context.currentImage = imageIndex;
				else
					pErrMgr->vkGetError(res,__FILE__,__LINE__);
#endif
			}
			else
				pErrMgr->generateRaptorError(	Global::CVulkanClassID::GetClassId(),
												CRaptorErrorManager::RAPTOR_WARNING,
												"Vulkan Context Image is being reused !");
		}
		else
		{
			context.currentImage = MAXUINT;
		}
	}
}

void CContextManager::vkSwapBuffers(RENDERING_CONTEXT_ID ctx)
{
	if ((ctx >= 0) && (ctx < MAX_CONTEXT))
	{
		const VK_CONTEXT& context = m_pVkContext[ctx];

		// Should store the queue in context struct ?
		VkQueue queue;
		uint32_t queueFamilyIndex = 0;
		uint32_t queueIndex = 0;
		m_pExtensions->vkGetDeviceQueue(context.device, queueFamilyIndex, queueIndex, &queue);

		//	TRansition image to present layout
		//	VK_IMAGE_LAYOUT_PRESENT_SRC_KHR

		VkPresentInfoKHR presentInfo = {VK_STRUCTURE_TYPE_PRESENT_INFO_KHR, NULL,
										0, NULL,
										1, &context.swapChain,
										&context.currentImage,
										NULL};

		CRaptorErrorManager *pErrMgr = Raptor::GetErrorManager();
		VkResult res = m_pExtensions->vkQueuePresentKHR(queue,&presentInfo);
		if (VK_SUCCESS != res)
			pErrMgr->vkGetError(res,__FILE__,__LINE__);
	}
}

//
//	TODO : migrate to Win32 specifics ?
//
void CContextManager::vkSwapVSync(unsigned int framerate)
{
#ifdef VK_KHR_swapchain
	if (CGL_MAXREFRESHRATE == framerate)
		presentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;	// no-vblank + no-queue (immediate)
	else if (0 == framerate)
		presentMode = VK_PRESENT_MODE_FIFO_RELAXED_KHR; // no v-blank + internal queue
	else // (swapinterval == 1)
	{
		DEVMODE mode;
	    memset(&mode,sizeof(DEVMODE),0);
	    mode.dmSize = sizeof(DEVMODE);
	    mode.dmDriverExtra = 0;
	    mode.dmFields = DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT|DM_DISPLAYFLAGS|DM_DISPLAYFREQUENCY;

	    if (0!=EnumDisplaySettings(NULL,ENUM_CURRENT_SETTINGS,&mode))
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

void CContextManager::vkDestroyContext(RENDERING_CONTEXT_ID ctx)
{
	if ((ctx >= 0) && (ctx < MAX_CONTEXT))
	{
		VK_CONTEXT& context = m_pVkContext[ctx];
		VkDevice device = context.device;
		VkCommandPool commandPool = context.commandPool;

		if (NULL != context.pImages)
			delete [] context.pImages;

		if (VK_NULL_HANDLE != device)
		{
#ifdef VK_KHR_win32_surface
	#ifdef VK_KHR_swapchain
			VkSwapchainKHR swapChain = context.swapChain;
			if (VK_NULL_HANDLE != swapChain)
				m_pExtensions->vkDestroySwapchainKHR(device,swapChain,NULL);
	#endif
			VkSurfaceKHR surface = context.surface;
			if (VK_NULL_HANDLE != surface)
				m_pExtensions->vkDestroySurfaceKHR(m_globalInstance,surface,NULL);
#endif
			if (VK_NULL_HANDLE != commandPool)
			{
				m_pExtensions->vkDestroyCommandPool(device,commandPool,NULL);
				m_pExtensions->vkDestroyDevice(device,NULL);
			}
		}

		context.surface = NULL;
		context.device = NULL;
		context.commandPool = NULL;
	}
	else
	{
		CRaptorErrorManager *pErrMgr = Raptor::GetErrorManager();
		pErrMgr->generateRaptorError(	Global::CVulkanClassID::GetClassId(),
										CRaptorErrorManager::RAPTOR_ERROR,
										"Unable to destroy unknown Vulkan context");
	}
}

RAPTOR_HANDLE CContextManager::getDevice(RENDERING_CONTEXT_ID ctx) const
{
	RAPTOR_HANDLE device;
	if ((ctx >= 0) && (ctx < MAX_CONTEXT))
	{
		device.handle = (unsigned int)m_pVkContext[ctx].device;
		device.hClass = DEVICE_CONTEXT_CLASS;
	}

	return device;
}
#endif