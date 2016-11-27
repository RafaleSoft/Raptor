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

#if defined(VK_VERSION_1_0)
	#if !defined(AFX_RAPTORVULKANMEMORY_H__72256FF7_DBB9_4B9C_9BF7_C36F425CF811__INCLUDED_)
		#include "Subsys/Vulkan/VulkanMemory.h"
	#endif
#endif


RAPTOR_NAMESPACE_BEGIN

	CContextManager *CContextManager::p_manager = NULL;

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
	:m_logo()
#if defined(VK_VERSION_1_0)
	,m_pExtensions(NULL)
	,m_pVkContext(NULL)
	,vulkanModule()
	,vkGetInstanceProcAddr(NULL)
	,vkEnumerateInstanceExtensionProperties(NULL)
	,vkEnumerateInstanceLayerProperties(NULL)
	,vkCreateInstance(NULL)
#endif
{
#if defined(VK_VERSION_1_0)
	m_pVkContext = new VK_CONTEXT[MAX_CONTEXT];
	for (unsigned int i=0;i<MAX_CONTEXT;i++)
	{
		VK_CONTEXT &ctxt = m_pVkContext[i];
		ctxt.instance = VK_NULL_HANDLE;

		ctxt.vkEnumeratePhysicalDevices = NULL;
		ctxt.vkEnumerateDeviceExtensionProperties = NULL;
		ctxt.vkEnumerateDeviceLayerProperties = NULL;
		ctxt.vkGetPhysicalDeviceProperties = NULL;
		ctxt.vkGetPhysicalDeviceFeatures = NULL;
		ctxt.vkGetPhysicalDeviceQueueFamilyProperties = NULL;
		ctxt.vkGetPhysicalDeviceMemoryProperties = NULL;
		ctxt.vkDestroyInstance = NULL;
		ctxt.vkCreateDevice = NULL;
		ctxt.vkGetDeviceProcAddr = NULL;

		ctxt.nbPhysicalDevices = 0;
		ctxt.pPhysicalDevices = NULL;
		ctxt.pProperties = NULL;
		ctxt.pFeatures = NULL;
		ctxt.physicalDevice = MAXUINT;

#ifdef VK_KHR_win32_surface
		ctxt.surface = VK_NULL_HANDLE;
		ctxt.pSurfaceFormatCount = 0;
		ctxt.pSurfaceFormats = NULL;
		ctxt.pPresentModeCount = 0;
		ctxt.pPresentModes = NULL;
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
	if (NULL != m_pExtensions)
		delete m_pExtensions;

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

    CRaptorDataManager  *dataManager = CRaptorDataManager::GetInstance();
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
bool CContextManager::vkInit(void)
{
	if (NULL == vkGetInstanceProcAddr)
		return false;
	
	vkEnumerateInstanceExtensionProperties = (PFN_vkEnumerateInstanceExtensionProperties)vkGetInstanceProcAddr(NULL,"vkEnumerateInstanceExtensionProperties");
	vkEnumerateInstanceLayerProperties = (PFN_vkEnumerateInstanceLayerProperties)vkGetInstanceProcAddr(NULL,"vkEnumerateInstanceLayerProperties");
	vkCreateInstance = (PFN_vkCreateInstance)vkGetInstanceProcAddr(NULL,"vkCreateInstance");

	if ((NULL != vkEnumerateInstanceExtensionProperties) &&
		(NULL != vkEnumerateInstanceLayerProperties) &&
		(NULL != vkCreateInstance))
	{
		VkResult res = VK_NOT_READY;

		instance_extensions = "";
		uint32_t pPropertyCount = 0;
		res = vkEnumerateInstanceExtensionProperties(NULL,&pPropertyCount,NULL);
		if ((VK_SUCCESS == res) && (pPropertyCount > 0))
		{
			VkExtensionProperties *pProperties = new VkExtensionProperties[pPropertyCount];
			res = vkEnumerateInstanceExtensionProperties(NULL,&pPropertyCount,pProperties);
			if (VK_SUCCESS == res)
			{
				for (uint32_t i=0;i<pPropertyCount;i++)
				{
					instance_extensions += " ";
					instance_extensions += pProperties[i].extensionName;
				}
			}
			delete [] pProperties;
		}

		instance_layers = "";
		uint32_t pLayerCount = 0;
		res = vkEnumerateInstanceLayerProperties(&pLayerCount,NULL);
		if ((VK_SUCCESS == res) && (pLayerCount > 0))
		{
			VkLayerProperties *pProperties = new VkLayerProperties[pLayerCount];
			uint32_t nbLayerCount = pLayerCount;
			res = vkEnumerateInstanceLayerProperties(&nbLayerCount,pProperties);
			if (VK_SUCCESS == res)
			{
				for (uint32_t i=0;i<pLayerCount;i++)
				{
					instance_layers += " ";
					instance_layers += pProperties[i].layerName;
				}
			}
			delete [] pProperties;
		}

		CRaptorErrorManager *pErrMgr = Raptor::GetErrorManager();
		if (VK_SUCCESS != res)
		{
			pErrMgr->vkGetError(res,__FILE__,__LINE__);
			return false;
		}
		else
			return true;
	}
	else
		return false;
}

bool CContextManager::vkInitInstance(CContextManager::RENDERING_CONTEXT_ID ctx)
{
	VK_CONTEXT &vk_ctx = m_pVkContext[ctx];
	CRaptorErrorManager *pErrMgr = Raptor::GetErrorManager();
	VkResult res = VK_NOT_READY;

	VkApplicationInfo applicationInfo = {	VK_STRUCTURE_TYPE_APPLICATION_INFO, NULL,
											"Raptor", RAPTOR_VERSION,
											"Raptor 3D Engine", RAPTOR_VERSION,
											VK_API_VERSION_1_0 };
	
	//	This is Win32 specific: use instance_extensions instead ?
	uint32_t nbExtensions = 2;
#if defined(RAPTOR_DEBUG_MODE_GENERATION) && defined(VK_EXT_debug_report)
	nbExtensions = 3;
	const char* extensions[3] = { "VK_KHR_surface", "VK_KHR_win32_surface", "VK_EXT_debug_report" };
#else
	const char* extensions[2] = { "VK_KHR_surface", "VK_KHR_win32_surface" };
#endif
	VkInstanceCreateInfo instanceCreateInfo = {	VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO, // VkStructureType sType;
												NULL,                                   // const void* pNext;
												0,                                      // VkInstanceCreateFlags flags;
												&applicationInfo,						// const VkApplicationInfo* pApplicationInfo;
												0,                                      // uint32_t enabledLayerNameCount;
												NULL,                                   // const char* const* ppEnabledLayerNames;
												nbExtensions,                           // uint32_t enabledExtensionNameCount;
												extensions};							// const char* const* ppEnabledExtensionNames;

	res = vkCreateInstance(	&instanceCreateInfo,
							CVulkanMemory::GetAllocator(), 
							&vk_ctx.instance);
	if (VK_SUCCESS != res)
		pErrMgr->vkGetError(res,__FILE__,__LINE__);

	if (VK_SUCCESS == res)
	{
		vk_ctx.vkEnumeratePhysicalDevices = (PFN_vkEnumeratePhysicalDevices)(vkGetInstanceProcAddr(vk_ctx.instance,"vkEnumeratePhysicalDevices"));
		vk_ctx.nbPhysicalDevices = 0;

		res = vk_ctx.vkEnumeratePhysicalDevices(vk_ctx.instance,&vk_ctx.nbPhysicalDevices,NULL);
		if ((VK_SUCCESS == res) && (vk_ctx.nbPhysicalDevices > 0))
		{
			vk_ctx.pPhysicalDevices = new VkPhysicalDevice[vk_ctx.nbPhysicalDevices];
			res = vk_ctx.vkEnumeratePhysicalDevices(vk_ctx.instance,&vk_ctx.nbPhysicalDevices,vk_ctx.pPhysicalDevices);
			if (VK_SUCCESS == res)
			{
				vk_ctx.vkEnumerateDeviceExtensionProperties = (PFN_vkEnumerateDeviceExtensionProperties)(vkGetInstanceProcAddr(vk_ctx.instance,"vkEnumerateDeviceExtensionProperties"));
				vk_ctx.vkEnumerateDeviceLayerProperties = (PFN_vkEnumerateDeviceLayerProperties)(vkGetInstanceProcAddr(vk_ctx.instance,"vkEnumerateDeviceLayerProperties"));
				vk_ctx.vkGetPhysicalDeviceProperties = (PFN_vkGetPhysicalDeviceProperties)(vkGetInstanceProcAddr(vk_ctx.instance,"vkGetPhysicalDeviceProperties"));
				vk_ctx.vkGetPhysicalDeviceFeatures = (PFN_vkGetPhysicalDeviceFeatures)(vkGetInstanceProcAddr(vk_ctx.instance,"vkGetPhysicalDeviceFeatures"));
				vk_ctx.vkGetPhysicalDeviceQueueFamilyProperties = (PFN_vkGetPhysicalDeviceQueueFamilyProperties)(vkGetInstanceProcAddr(vk_ctx.instance,"vkGetPhysicalDeviceQueueFamilyProperties"));
				vk_ctx.vkGetPhysicalDeviceMemoryProperties = (PFN_vkGetPhysicalDeviceMemoryProperties)(vkGetInstanceProcAddr(vk_ctx.instance,"vkGetPhysicalDeviceMemoryProperties"));

				vk_ctx.vkDestroyInstance = (PFN_vkDestroyInstance)(vkGetInstanceProcAddr(vk_ctx.instance,"vkDestroyInstance"));
				vk_ctx.vkCreateDevice = (PFN_vkCreateDevice)(vkGetInstanceProcAddr(vk_ctx.instance,"vkCreateDevice"));
				vk_ctx.vkGetDeviceProcAddr = (PFN_vkGetDeviceProcAddr)(vkGetInstanceProcAddr(vk_ctx.instance,"vkGetDeviceProcAddr"));

				vk_ctx.device.vkCreateDevice = vk_ctx.vkCreateDevice;
				vk_ctx.device.vkGetDeviceProcAddr = vk_ctx.vkGetDeviceProcAddr;

				CVulkanMemory::vkGetBufferMemoryRequirements = (PFN_vkGetBufferMemoryRequirements)(vkGetInstanceProcAddr(vk_ctx.instance,"vkGetBufferMemoryRequirements"));
				CVulkanMemory::vkCreateBuffer = (PFN_vkCreateBuffer)(vkGetInstanceProcAddr(vk_ctx.instance,"vkCreateBuffer"));
				CVulkanMemory::vkDestroyBuffer = (PFN_vkDestroyBuffer)(vkGetInstanceProcAddr(vk_ctx.instance,"vkDestroyBuffer"));
				CVulkanMemory::vkAllocateMemory = (PFN_vkAllocateMemory)(vkGetInstanceProcAddr(vk_ctx.instance,"vkAllocateMemory"));
				CVulkanMemory::vkBindBufferMemory = (PFN_vkBindBufferMemory)(vkGetInstanceProcAddr(vk_ctx.instance,"vkBindBufferMemory"));
				CVulkanMemory::vkFreeMemory = (PFN_vkFreeMemory)(vkGetInstanceProcAddr(vk_ctx.instance,"vkFreeMemory"));
				CVulkanMemory::vkMapMemory = (PFN_vkMapMemory)(vkGetInstanceProcAddr(vk_ctx.instance,"vkMapMemory"));
				CVulkanMemory::vkFlushMappedMemoryRanges = (PFN_vkFlushMappedMemoryRanges)(vkGetInstanceProcAddr(vk_ctx.instance,"vkFlushMappedMemoryRanges"));
				CVulkanMemory::vkUnmapMemory = (PFN_vkUnmapMemory)(vkGetInstanceProcAddr(vk_ctx.instance,"vkUnmapMemory"));

				
#if defined(VK_USE_PLATFORM_WIN32_KHR)
				vk_ctx.vkCreateWin32SurfaceKHR = (PFN_vkCreateWin32SurfaceKHR)(vkGetInstanceProcAddr(vk_ctx.instance,"vkCreateWin32SurfaceKHR"));
				vk_ctx.vkGetPhysicalDeviceWin32PresentationSupportKHR = (PFN_vkGetPhysicalDeviceWin32PresentationSupportKHR)(vkGetInstanceProcAddr(vk_ctx.instance,"vkGetPhysicalDeviceWin32PresentationSupportKHR"));
#elif defined(VK_USE_PLATFORM_XLIB_KHR)
				vk_ctx.vkCreateXlibSurfaceKHR = (PFN_vkCreateXlibSurfaceKHR)(vkGetInstanceProcAddr(vk_ctx.instance,"vkCreateXlibSurfaceKHR"));
				vk_ctx.vkGetPhysicalDeviceXlibPresentationSupportKHR = (PFN_vkGetPhysicalDeviceXlibPresentationSupportKHR)(vkGetInstanceProcAddr(vk_ctx.instance,"vkGetPhysicalDeviceXlibPresentationSupportKHR"));
#endif

#if defined(VK_KHR_surface)
				vk_ctx.vkDestroySurfaceKHR = (PFN_vkDestroySurfaceKHR)(vkGetInstanceProcAddr(vk_ctx.instance,"vkDestroySurfaceKHR"));
				vk_ctx.vkGetPhysicalDeviceSurfaceSupportKHR = (PFN_vkGetPhysicalDeviceSurfaceSupportKHR)(vkGetInstanceProcAddr(vk_ctx.instance,"vkGetPhysicalDeviceSurfaceSupportKHR"));
				vk_ctx.vkGetPhysicalDeviceSurfaceCapabilitiesKHR = (PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR)(vkGetInstanceProcAddr(vk_ctx.instance,"vkGetPhysicalDeviceSurfaceCapabilitiesKHR"));
				vk_ctx.vkGetPhysicalDeviceSurfaceFormatsKHR = (PFN_vkGetPhysicalDeviceSurfaceFormatsKHR)(vkGetInstanceProcAddr(vk_ctx.instance,"vkGetPhysicalDeviceSurfaceFormatsKHR"));
				vk_ctx.vkGetPhysicalDeviceSurfacePresentModesKHR = (PFN_vkGetPhysicalDeviceSurfacePresentModesKHR)(vkGetInstanceProcAddr(vk_ctx.instance,"vkGetPhysicalDeviceSurfacePresentModesKHR"));
#endif
				
				vk_ctx.pProperties = new VkPhysicalDeviceProperties[vk_ctx.nbPhysicalDevices];
				vk_ctx.pFeatures = new VkPhysicalDeviceFeatures[vk_ctx.nbPhysicalDevices];

				for (uint32_t i=0;i<vk_ctx.nbPhysicalDevices;i++)
				{
					VkPhysicalDevice device = vk_ctx.pPhysicalDevices[i];
					vk_ctx.vkGetPhysicalDeviceProperties(device,&vk_ctx.pProperties[i]);
					vk_ctx.vkGetPhysicalDeviceFeatures(device,&vk_ctx.pFeatures[i]);

					uint32_t pPropertyCount = 0;
					res = vk_ctx.vkEnumerateDeviceExtensionProperties(device,NULL,&pPropertyCount,NULL);
					if ((VK_SUCCESS == res) && (pPropertyCount > 0))
					{
						VkExtensionProperties* pProperties = new VkExtensionProperties[pPropertyCount];
						res = vk_ctx.vkEnumerateDeviceExtensionProperties(device,NULL,&pPropertyCount,pProperties);
						for (uint32_t j=0;j<pPropertyCount;j++)
						{
							vk_ctx.deviceExtensions += " ";
							vk_ctx.deviceExtensions += pProperties[j].extensionName;
						}
						delete [] pProperties;
					}

					uint32_t pLayerCount = 0;
					res = vk_ctx.vkEnumerateDeviceLayerProperties(device,&pLayerCount,NULL);
					if ((VK_SUCCESS == res) && (pLayerCount > 0))
					{
						VkLayerProperties* pProperties = new VkLayerProperties[pLayerCount];
						uint32_t nbLayerCount = pLayerCount;
						res = vk_ctx.vkEnumerateDeviceLayerProperties(device, &nbLayerCount, pProperties);
						for (uint32_t j=0;j<pLayerCount;j++)
						{
							vk_ctx.deviceLayers += " ";
							vk_ctx.deviceLayers += pProperties[j].layerName;
						}
						delete [] pProperties;
					}
				}
			}
			else
			{
				vk_ctx.nbPhysicalDevices = 0;
				delete [] vk_ctx.pPhysicalDevices;
			}
		}
		if (VK_SUCCESS != res)
			pErrMgr->vkGetError(res,__FILE__,__LINE__);
	}

	if (VK_SUCCESS != res)
	{
		pErrMgr->generateRaptorError(	Global::CVulkanClassID::GetClassId(),
										CRaptorErrorManager::RAPTOR_ERROR,
										"Unable to initialise a Vulkan Instance");
	}

	return ((VK_SUCCESS == res) && (vk_ctx.nbPhysicalDevices > 0));
}

bool CContextManager::vkInitDevice(CContextManager::RENDERING_CONTEXT_ID ctx,const CRaptorDisplayConfig& config)
{
	VK_CONTEXT &vk_ctx = m_pVkContext[ctx];
	CRaptorErrorManager *pErrMgr = Raptor::GetErrorManager();
	VkResult res = VK_NOT_READY;

	unsigned int maxd = vk_ctx.nbPhysicalDevices;
	uint32_t queueCount = 0;
	uint32_t graphicsQueueFamilyIndex = MAXUINT;
	uint32_t presentQueueFamilyIndex = MAXUINT;

	//!	Find a device with graphic & win32 presentation support
	for (unsigned int i=0; (i<maxd) && (vk_ctx.physicalDevice > maxd); i++)
	{
		VkPhysicalDevice physicalDevice = vk_ctx.pPhysicalDevices[i];
		//! Not confirmed if a CPU type is able to render graphics
		if ((vk_ctx.pProperties[i].deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU) ||
			(vk_ctx.pProperties[i].deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) ||
			(vk_ctx.pProperties[i].deviceType == VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU))
		{
			uint32_t pQueueFamilyPropertyCount = 0;
			vk_ctx.vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice,&pQueueFamilyPropertyCount,NULL);
			if (pQueueFamilyPropertyCount > 0)
			{
				VkQueueFamilyProperties *pQueueFamilyProperties = new VkQueueFamilyProperties[pQueueFamilyPropertyCount];
				uint32_t nbQueueFamilyProperties = pQueueFamilyPropertyCount;
				vk_ctx.vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &nbQueueFamilyProperties, pQueueFamilyProperties);
				
				for (uint32_t j=0;j<pQueueFamilyPropertyCount;j++)
				{
					//!	Check presentation support
					VkBool32 support = VK_FALSE;
#if defined(VK_USE_PLATFORM_WIN32_KHR)
					support = vk_ctx.vkGetPhysicalDeviceWin32PresentationSupportKHR(physicalDevice,j);
#elif defined(VK_USE_PLATFORM_XLIB_KHR)
					support = vk_ctx.PFN_vkGetPhysicalDeviceXlibPresentationSupportKHR(physicalDevice,j);
#else
					RAPTOR_WARNING( Global::CVulkanClassID::GetClassId(),"No support for Vulkan WSI");
					support = VK_TRUE;
#endif
					VkBool32 present_support = VK_FALSE;
#if defined(VK_KHR_surface)
					if (VK_NULL_HANDLE != vk_ctx.surface )
						res = vk_ctx.vkGetPhysicalDeviceSurfaceSupportKHR(	physicalDevice,
																			j,
																			vk_ctx.surface,
																			&present_support);
#endif

					if ((pQueueFamilyProperties[j].queueCount > 0) &&
						(VK_QUEUE_GRAPHICS_BIT == (VK_QUEUE_GRAPHICS_BIT & pQueueFamilyProperties[j].queueFlags)))
					// && (VK_QUEUE_TRANSFER_BIT == (VK_QUEUE_TRANSFER_BIT & pQueueFamilyProperties[j].queueFlags)))
					{
						if ((pQueueFamilyProperties[j].queueCount > queueCount) || (graphicsQueueFamilyIndex == MAXUINT))
						{
							queueCount = pQueueFamilyProperties[j].queueCount;
							graphicsQueueFamilyIndex = j;
						}

						//	If queue supports both graphics & presentation, select it.
						if ((VK_TRUE == support) && (VK_TRUE == present_support))
						{
							presentQueueFamilyIndex = j;
							graphicsQueueFamilyIndex = j;
						}
					}


					if ((pQueueFamilyProperties[j].queueCount > 0) && (VK_TRUE == support) && (VK_TRUE == present_support))
						presentQueueFamilyIndex = j;
			
					//!	Here, we always select a device with presentable queue.
					//!	For future : allow selection of device not for presentation if
					//!	specificaly required.
					if ((graphicsQueueFamilyIndex < MAXUINT) &&
						(presentQueueFamilyIndex < MAXUINT))
					{
						vk_ctx.physicalDevice = i;
						break;
					}
				}

				delete [] pQueueFamilyProperties;
			}
		}
	}

	//! Check Device properties to comply with requested config
	if (vk_ctx.physicalDevice < maxd)
	{
		VkPhysicalDeviceProperties props = vk_ctx.pProperties[vk_ctx.physicalDevice];
		VkPhysicalDeviceLimits limits = props.limits;
		if ((limits.maxViewports < 1) ||
			(limits.maxViewportDimensions[0] < config.width) ||
			(limits.maxViewportDimensions[1] < config.height))
			return false;

		VkPhysicalDevice physicalDevice = vk_ctx.pPhysicalDevices[vk_ctx.physicalDevice];
		VkPhysicalDeviceFeatures features = vk_ctx.pFeatures[vk_ctx.physicalDevice];

		VkPhysicalDeviceMemoryProperties memory_properties;
		vk_ctx.vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memory_properties );
		CVulkanMemory& memory = CVulkanMemory::GetInstance(	physicalDevice,
															memory_properties);

		return vk_ctx.device.vkCreateLogicalDevice(	physicalDevice,
													features,
													graphicsQueueFamilyIndex,
													queueCount,
													presentQueueFamilyIndex,1);
	}
	else
		return false;
}


#ifdef VK_KHR_win32_surface
void CContextManager::vkResize(	RENDERING_CONTEXT_ID ctx,const CRaptorDisplayConfig& config)
{
	VK_CONTEXT &context = m_pVkContext[ctx];

	//!	 Collect new surface capabilities,
	//! presumably the only this modified for a resize.
	VkPhysicalDevice pPhysicalDevice = context.pPhysicalDevices[context.physicalDevice];
	VkResult res = context.vkGetPhysicalDeviceSurfaceCapabilitiesKHR(pPhysicalDevice,
																	context.surface,
																	&context.surfaceCapabilities);
	if (VK_SUCCESS != res)
	{
		RAPTOR_ERROR(	Global::CVulkanClassID::GetClassId(),
						"Failed to obtain Vulkan surface capabilities");
	}
	else
	{
		if (!vkCreateSwapChain(ctx,config.getNbSamples(),config.width,config.height))
		{
			RAPTOR_ERROR(	Global::CVulkanClassID::GetClassId(),
							"Failed to recreate Vulkan swap chain");
		}
	}
}

bool CContextManager::vkInitSurface(RENDERING_CONTEXT_ID ctx)
{
	VK_CONTEXT &context = m_pVkContext[ctx];
	context.pSurfaceFormatCount = 0;
	if (NULL != context.pSurfaceFormats)
		delete [] context.pSurfaceFormats;
	context.pSurfaceFormats = NULL;
	context.pPresentModeCount = 0;
	if (NULL != context.pPresentModes)
		delete [] context.pPresentModes;
	context.pPresentModes = NULL;

	//!	GetSurface Capabilities
	VkPhysicalDevice pPhysicalDevice = context.pPhysicalDevices[context.physicalDevice];
	VkResult res = context.vkGetPhysicalDeviceSurfaceCapabilitiesKHR(pPhysicalDevice,
																	context.surface,
																	&context.surfaceCapabilities);
	if (VK_SUCCESS != res)
	{
		RAPTOR_ERROR(	Global::CVulkanClassID::GetClassId(),
						"Failed to obtain Vulkan surface capabilities");
		return false;
	}

	//!	Get Surface formats
	res = context.vkGetPhysicalDeviceSurfaceFormatsKHR(	pPhysicalDevice,
														context.surface,
														&context.pSurfaceFormatCount,
														NULL);
	if ((VK_SUCCESS != res) || (0 == context.pSurfaceFormatCount))
	{
		RAPTOR_ERROR(	Global::CVulkanClassID::GetClassId(),
						"Failed to obtain Vulkan surface formats");
		res = VK_NOT_READY;
		return false;
	}
	else
	{
		context.pSurfaceFormats = new VkSurfaceFormatKHR[context.pSurfaceFormatCount];
		res = context.vkGetPhysicalDeviceSurfaceFormatsKHR(	pPhysicalDevice,
															context.surface,
															&context.pSurfaceFormatCount,
															context.pSurfaceFormats);
		if (VK_SUCCESS != res)
		{
			RAPTOR_ERROR(	Global::CVulkanClassID::GetClassId(),
							"Failed to obtain Vulkan surface formats");
		}
	}

	if (VK_SUCCESS == res)
		res = context.vkGetPhysicalDeviceSurfacePresentModesKHR(pPhysicalDevice,
																context.surface,
																&context.pPresentModeCount,
																NULL);
	if ((VK_SUCCESS != res) || (0 == context.pPresentModeCount))
	{
		RAPTOR_ERROR(	Global::CVulkanClassID::GetClassId(),
						"Failed to obtain Vulkan surface present modes or no present modes available");
		res = VK_NOT_READY;
	}
	else
	{
		context.pPresentModes = new VkPresentModeKHR[context.pPresentModeCount];
		res = context.vkGetPhysicalDeviceSurfacePresentModesKHR(pPhysicalDevice,
																context.surface,
																&context.pPresentModeCount,
																context.pPresentModes);
		if (VK_SUCCESS != res)
		{
			RAPTOR_ERROR(	Global::CVulkanClassID::GetClassId(),
							"Failed to obtain Vulkan surface formats");
		}
	}

	//	Clean Surface formats and present modes
	if (VK_SUCCESS != res)
	{
		if (NULL != context.pSurfaceFormats)
			delete [] context.pSurfaceFormats;
		context.pSurfaceFormats = NULL;
		context.pSurfaceFormatCount = 0;
		if (NULL != context.pPresentModes)
			delete [] context.pPresentModes;
		context.pPresentModes = NULL;
		context.pPresentModeCount = 0;
		return false;
	}

	return true;
}
#endif


bool CContextManager::vkCreateSwapChain(RENDERING_CONTEXT_ID ctx,uint32_t nbSamples,uint32_t width,uint32_t height)
{
	VK_CONTEXT &context = m_pVkContext[ctx];

	VkSurfaceFormatKHR format = { VK_FORMAT_R8G8B8A8_UNORM, VK_COLORSPACE_SRGB_NONLINEAR_KHR };
	for (unsigned int i=0;i<context.pSurfaceFormatCount;i++)
		if ((VK_FORMAT_R8G8B8A8_UNORM == context.pSurfaceFormats[i].format) ||
			(VK_FORMAT_B8G8R8A8_UNORM == context.pSurfaceFormats[i].format))
			format = context.pSurfaceFormats[i];

#ifdef VK_KHR_swapchain
	// Find a supported present mode and update requested/current present mode.
	VkPresentModeKHR pMode = VK_PRESENT_MODE_MAX_ENUM_KHR;
	for (unsigned int i=0;i<context.pPresentModeCount;i++)
		if (context.pPresentModes[i] == presentMode)
			pMode = presentMode;
	if (pMode == VK_PRESENT_MODE_MAX_ENUM_KHR)
		for (unsigned int i=0;i<context.pPresentModeCount;i++)
			if (context.pPresentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR)
				pMode = VK_PRESENT_MODE_MAILBOX_KHR;
	if (pMode == VK_PRESENT_MODE_MAX_ENUM_KHR)
		for (unsigned int i=0;i<context.pPresentModeCount;i++)
			if (context.pPresentModes[i] == VK_PRESENT_MODE_FIFO_KHR)
				pMode = VK_PRESENT_MODE_FIFO_KHR;
	presentMode = pMode;

	if (!context.device.vkCreateSwapChain(	context.surface,
											format,
											context.surfaceCapabilities,
											presentMode,
											width,
											height))
		return false;
#endif

	if (!context.device.vkCreateRenderPassResources(format,
													nbSamples,
													width,
													height))
		return false;
#endif

	if (!context.device.vkCreateRenderingResources())
		return false;

	return true;
}

CContextManager::RENDERING_CONTEXT_ID CContextManager::vkCreateContext(const RAPTOR_HANDLE& handle,
																	   const CRaptorDisplayConfig& config)
{
	size_t nbInstances = 0;
	for (size_t i=0;i<MAX_CONTEXT;i++)
		nbInstances += (VK_NULL_HANDLE != m_pVkContext[i].instance ? 1 : 0);

	if (nbInstances >= MAX_CONTEXT)
	{
		RAPTOR_ERROR( Global::CVulkanClassID::GetClassId(),"Too many Vulkan Context created");
		return -1;
	}

	//	Here, at least one instance slot is available, so no further check
	unsigned int ctx = 0;
	while ((ctx < MAX_CONTEXT) && (VK_NULL_HANDLE != m_pVkContext[ctx].instance))
		ctx++;
	VK_CONTEXT &context = m_pVkContext[ctx];

	if (!vkInitInstance(ctx))
	{
		vkDestroyContext(ctx);
		return -1;
	}

	if (WINDOW_CLASS == handle.hClass)
	{
		if (!vkCreateSurface(handle,ctx))
		{
			vkDestroyContext(ctx);
			return -1;
		}
	}

	if (!vkInitDevice(ctx,config))
	{
		vkDestroyContext(ctx);
		return -1;
	}


#ifdef VK_KHR_win32_surface
	if (!vkInitSurface(ctx))
	{
		vkDestroyContext(ctx);
		return -1;
	}

	if (!vkCreateSwapChain(ctx,config.getNbSamples(),config.width,config.height))
	{
		vkDestroyContext(ctx);
		return -1;
	}

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
			}
		}
	}
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

//
//	TODO : migrate to Win32 specifics ?
//
void CContextManager::vkSwapVSync(unsigned int framerate)
{
#ifdef VK_KHR_swapchain
	if (CGL_MAXREFRESHRATE == framerate)
		presentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;	// no-vblank + no-queue (immediate)
	else if (0 == framerate)
		presentMode = VK_PRESENT_MODE_FIFO_RELAXED_KHR; // single v-blank + internal queue
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
		
		if (context.device.vkDestroyLogicalDevice())
		{
#ifdef VK_KHR_win32_surface
			VkSurfaceKHR surface = context.surface;
			if (VK_NULL_HANDLE != surface)
				context.vkDestroySurfaceKHR(context.instance,surface,NULL);
			if (NULL != context.pSurfaceFormats)
				delete [] context.pSurfaceFormats;
			if (NULL != context.pPresentModes)
				delete [] context.pPresentModes;
#endif
		}

		if (NULL != context.instance)
			context.vkDestroyInstance(context.instance,CVulkanMemory::GetAllocator());

		context.surface = VK_NULL_HANDLE;
		context.instance = VK_NULL_HANDLE;

		if (NULL != context.pPhysicalDevices)
			delete [] context.pPhysicalDevices;
		context.pPhysicalDevices = NULL;
		if (NULL != context.pProperties)
			delete [] context.pProperties;
		context.pProperties = NULL;
		if (NULL != context.pFeatures)
			delete [] context.pFeatures;
		context.pFeatures = NULL;
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