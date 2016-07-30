// RaptorExtensions.cpp: implementation of the RaptorExtensions class.
//
//////////////////////////////////////////////////////////////////////
#include "Subsys/CodeGeneration.h"

#if !defined(AFX_RAPTOREXTENSIONS_H__E5B5A1D9_60F8_4E20_B4E1_8E5A9CB7E0EB__INCLUDED_)
	#include "RaptorExtensions.h"
#endif
#ifndef __glext_macros_h_
	#include "GLEXTMacros.h"
#endif
#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
	#include "Raptor.h"
#endif
#if !defined(AFX_RAPTORERRORMANAGER_H__FA5A36CD_56BC_4AA1_A5F4_451734AD395E__INCLUDED_)
    #include "RaptorErrorManager.h"
#endif
#ifndef __GLOBAL_H__
	#include "System/Global.h"
#endif


RAPTOR_NAMESPACE_BEGIN
#if defined(WGL_ARB_extensions_string)
	PFN_WGL_GET_EXTENSIONS_STRING_ARB_PROC CRaptorExtensions::wglGetExtensionsStringARB = NULL;
#endif
    IMPLEMENT_RAPTOR_WGL_ARB_pixel_format(CRaptorExtensions)
	IMPLEMENT_RAPTOR_WGL_ARB_pbuffer(CRaptorExtensions)
	IMPLEMENT_RAPTOR_WGL_ARB_render_texture(CRaptorExtensions)
	IMPLEMENT_RAPTOR_WGL_EXT_swap_control(CRaptorExtensions)
RAPTOR_NAMESPACE_END

RAPTOR_NAMESPACE


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CRaptorExtensions::CRaptorExtensions(const std::string &ext)
{
	extensions = ext;
}

CRaptorExtensions::~CRaptorExtensions()
{

}

void CRaptorExtensions::glInitExtensions(void)
{
#if defined(WGL_ARB_extensions_string)
	{
        wglGetExtensionsStringARB = (PFN_WGL_GET_EXTENSIONS_STRING_ARB_PROC)wglGetProcAddress("wglGetExtensionsStringARB");
	}
#endif

#if defined(WGL_ARB_pixel_format)
    wglGetPixelFormatAttribivARB = (PFN_WGL_GET_PIXEL_FORMAT_ATTRIBIV_ARB_PROC)wglGetProcAddress("wglGetPixelFormatAttribivARB");
    wglGetPixelFormatAttribfvARB = (PFN_WGL_GET_PIXEL_FORMAT_ATTRIBFV_ARB_PROC)wglGetProcAddress("wglGetPixelFormatAttribfvARB");
    wglChoosePixelFormatARB = (PFN_WGL_CHOOSE_PIXEL_FORMAT_ARB_PROC)wglGetProcAddress("wglChoosePixelFormatARB");
#endif

	IMPLEMENT_GL_ARB_texture_compression(this)
	IMPLEMENT_WGL_ARB_render_texture(this)
	IMPLEMENT_WGL_ARB_pbuffer(this)
	IMPLEMENT_GL_NV_vertex_array_range(this)
	IMPLEMENT_GL_ARB_vertex_buffer_object(this)
	IMPLEMENT_GL_EXT_compiled_vertex_array(this)
	IMPLEMENT_GL_ARB_multitexture(this)
	IMPLEMENT_GL_ARB_transpose_matrix(this)
	IMPLEMENT_GL_EXT_vertex_weighting(this)
	IMPLEMENT_GL_NV_register_combiners(this)
	IMPLEMENT_GL_EXT_fog_coord(this)
	IMPLEMENT_GL_EXT_point_parameters(this)
	IMPLEMENT_GL_EXT_secondary_color(this)
	IMPLEMENT_WGL_EXT_swap_control(this)
	IMPLEMENT_GL_ARB_vertex_program(this)
	IMPLEMENT_GL_ARB_imaging(this)
	IMPLEMENT_GL_ARB_occlusion_query(this)
	IMPLEMENT_GL_NV_point_sprite(this)
	IMPLEMENT_GL_ARB_point_parameters(this)
	IMPLEMENT_GL_EXT_stencil_two_side(this)
	IMPLEMENT_GL_ARB_color_buffer_float(this)
	IMPLEMENT_GL_EXT_texture3D(this)
	IMPLEMENT_GL_ARB_shader_objects(this)
	IMPLEMENT_GL_ARB_vertex_shader(this)
	IMPLEMENT_GL_ARB_geometry_shader4(this)
	IMPLEMENT_GL_ARB_draw_buffers(this)
	IMPLEMENT_GL_EXT_blend_color(this)
	IMPLEMENT_GL_EXT_framebuffer_object(this)
	IMPLEMENT_GL_EXT_framebuffer_blit(this)
	IMPLEMENT_GL_EXT_framebuffer_multisample(this)

    CATCH_GL_ERROR
	
#if defined(VK_VERSION_1_0)
	initVk();
#endif
	
	defaultInit();
}

#if defined(_WIN32)
	#define GETPROCADDRESS(proc) GetProcAddress(module,proc)
#elif defined(LINUX)
	#define GETPROCADDRESS("proc") GetProcAddress(module,"vkCreateInstance")
#endif

#if defined(VK_VERSION_1_0)
void CRaptorExtensions::initVk(void)
{
	VkResult res = VK_NOT_READY;


	CRaptorErrorManager *pErrMgr = Raptor::GetErrorManager();
#if defined(_WIN32)
	HMODULE module = (HMODULE)Global::GetInstance().getCurrentStatus().vulkanModule.handle;
#elif defined(LINUX)
#endif
	if (module != NULL)
	{
		vkCreateInstance = (PFN_vkCreateInstance)(GETPROCADDRESS("vkCreateInstance"));
		vkDestroyInstance = (PFN_vkDestroyInstance)(GETPROCADDRESS("vkDestroyInstance"));
		vkCreateDevice = (PFN_vkCreateDevice)(GETPROCADDRESS("vkCreateDevice"));
		vkDestroyDevice = (PFN_vkDestroyDevice)(GETPROCADDRESS("vkDestroyDevice"));

		vkEnumeratePhysicalDevices = (PFN_vkEnumeratePhysicalDevices)(GETPROCADDRESS("vkEnumeratePhysicalDevices"));
		vkEnumerateInstanceExtensionProperties = (PFN_vkEnumerateInstanceExtensionProperties)(GETPROCADDRESS("vkEnumerateInstanceExtensionProperties"));
		vkEnumerateDeviceExtensionProperties = (PFN_vkEnumerateDeviceExtensionProperties)(GETPROCADDRESS("vkEnumerateDeviceExtensionProperties"));
		vkEnumerateInstanceLayerProperties = (PFN_vkEnumerateInstanceLayerProperties)(GETPROCADDRESS("vkEnumerateInstanceLayerProperties;"));
		vkEnumerateDeviceLayerProperties = (PFN_vkEnumerateDeviceLayerProperties)(GETPROCADDRESS("vkEnumerateDeviceLayerProperties;"));

		vkGetPhysicalDeviceFeatures = (PFN_vkGetPhysicalDeviceFeatures)(GETPROCADDRESS("vkGetPhysicalDeviceFeatures"));
		vkGetPhysicalDeviceProperties = (PFN_vkGetPhysicalDeviceProperties)(GETPROCADDRESS("vkGetPhysicalDeviceProperties"));
		vkGetPhysicalDeviceFormatProperties = (PFN_vkGetPhysicalDeviceFormatProperties)(GETPROCADDRESS("vkGetPhysicalDeviceFormatProperties"));
		vkGetPhysicalDeviceMemoryProperties = (PFN_vkGetPhysicalDeviceMemoryProperties)(GETPROCADDRESS("vkGetPhysicalDeviceMemoryProperties"));
		vkGetPhysicalDeviceQueueFamilyProperties = (PFN_vkGetPhysicalDeviceQueueFamilyProperties)(GETPROCADDRESS("vkGetPhysicalDeviceQueueFamilyProperties"));
		
		//	These methods are bound to en entry point, is the function pointer obtained valid ?
		vkGetInstanceProcAddr = (PFN_vkGetInstanceProcAddr)(GETPROCADDRESS("vkGetInstanceProcAddr"));
		vkCreateCommandPool = (PFN_vkCreateCommandPool)(GETPROCADDRESS("vkCreateCommandPool"));
		vkResetCommandPool = (PFN_vkResetCommandPool)(GETPROCADDRESS("vkResetCommandPool"));
		vkDestroyCommandPool = (PFN_vkDestroyCommandPool)(GETPROCADDRESS("vkDestroyCommandPool"));
		vkGetDeviceQueue =  (PFN_vkGetDeviceQueue)(GETPROCADDRESS("vkGetDeviceQueue"));

		vkCreateRenderPass = (PFN_vkCreateRenderPass)(GETPROCADDRESS("vkCreateRenderPass"));
		vkDestroyRenderPass = (PFN_vkDestroyRenderPass)(GETPROCADDRESS("vkDestroyRenderPass"));

		//	Surfaces and WSI
#if defined(VK_KHR_surface)
		vkDestroySurfaceKHR = (PFN_vkDestroySurfaceKHR)(GETPROCADDRESS("vkDestroySurfaceKHR"));
		vkGetPhysicalDeviceSurfaceSupportKHR = (PFN_vkGetPhysicalDeviceSurfaceSupportKHR)(GETPROCADDRESS("vkGetPhysicalDeviceSurfaceSupportKHR"));
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR = (PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR)(GETPROCADDRESS("vkGetPhysicalDeviceSurfaceCapabilitiesKHR"));
		vkGetPhysicalDeviceSurfaceFormatsKHR = (PFN_vkGetPhysicalDeviceSurfaceFormatsKHR)(GETPROCADDRESS("vkGetPhysicalDeviceSurfaceFormatsKHR"));
		vkGetPhysicalDeviceSurfacePresentModesKHR = (PFN_vkGetPhysicalDeviceSurfacePresentModesKHR)(GETPROCADDRESS("vkGetPhysicalDeviceSurfacePresentModesKHR"));
#endif

#if defined(VK_KHR_swapchain)
		vkCreateSwapchainKHR = (PFN_vkCreateSwapchainKHR)(GETPROCADDRESS("vkCreateSwapchainKHR"));
		vkDestroySwapchainKHR = (PFN_vkDestroySwapchainKHR)(GETPROCADDRESS("vkDestroySwapchainKHR"));
		vkGetSwapchainImagesKHR = (PFN_vkGetSwapchainImagesKHR)(GETPROCADDRESS("vkGetSwapchainImagesKHR"));
		vkAcquireNextImageKHR = (PFN_vkAcquireNextImageKHR)(GETPROCADDRESS("vkAcquireNextImageKHR"));
		vkQueuePresentKHR = (PFN_vkQueuePresentKHR)(GETPROCADDRESS("vkQueuePresentKHR"));
#endif

#if defined(VK_KHR_display)
		vkGetPhysicalDeviceDisplayPropertiesKHR = (PFN_vkGetPhysicalDeviceDisplayPropertiesKHR)(GETPROCADDRESS("vkGetPhysicalDeviceDisplayPropertiesKHR"));
		vkGetPhysicalDeviceDisplayPlanePropertiesKHR = (PFN_vkGetPhysicalDeviceDisplayPlanePropertiesKHR)(GETPROCADDRESS("vkGetPhysicalDeviceDisplayPlanePropertiesKHR"));
		vkGetDisplayPlaneSupportedDisplaysKHR = (PFN_vkGetDisplayPlaneSupportedDisplaysKHR)(GETPROCADDRESS("vkGetDisplayPlaneSupportedDisplaysKHR"));
		vkGetDisplayModePropertiesKHR = (PFN_vkGetDisplayModePropertiesKHR)(GETPROCADDRESS("vkGetDisplayModePropertiesKHR"));
		vkCreateDisplayModeKHR = (PFN_vkCreateDisplayModeKHR )(GETPROCADDRESS("vkCreateDisplayModeKHR"));
		vkGetDisplayPlaneCapabilitiesKHR = (PFN_vkGetDisplayPlaneCapabilitiesKHR )(GETPROCADDRESS("vkGetDisplayPlaneCapabilitiesKHR"));
		vkCreateDisplayPlaneSurfaceKHR = (PFN_vkCreateDisplayPlaneSurfaceKHR)(GETPROCADDRESS("vkCreateDisplayPlaneSurfaceKHR"));
#endif
		
#if defined(VK_USE_PLATFORM_WIN32_KHR)
		vkCreateWin32SurfaceKHR = (PFN_vkCreateWin32SurfaceKHR)(GETPROCADDRESS("vkCreateWin32SurfaceKHR"));
		vkGetPhysicalDeviceWin32PresentationSupportKHR = (PFN_vkGetPhysicalDeviceWin32PresentationSupportKHR)(GETPROCADDRESS("vkGetPhysicalDeviceWin32PresentationSupportKHR"));
#elif defined(VK_USE_PLATFORM_XLIB_KHR)
		vkCreateXlibSurfaceKHR = (PFN_vkCreateXlibSurfaceKHR)(GETPROCADDRESS("vkCreateXlibSurfaceKHR"));
		vkGetPhysicalDeviceXlibPresentationSupportKHR = (PFN_vkGetPhysicalDeviceXlibPresentationSupportKHR)(GETPROCADDRESS("vkGetPhysicalDeviceXlibPresentationSupportKHR"));
#endif

		res = ((NULL != vkCreateInstance) && (NULL != vkEnumerateInstanceExtensionProperties) ? VK_SUCCESS : VK_NOT_READY);
	}

	if (VK_SUCCESS != res)
	{
		pErrMgr->generateRaptorError(	Global::CVulkanClassID::GetClassId(),
										CRaptorErrorManager::RAPTOR_ERROR,
										"Unable to initialise Vulkan API");
	}
	else
	{
		VkInstanceCreateInfo instanceCreateInfo = {	VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO, // VkStructureType sType;
											NULL,                                   // const void* pNext;
											0,                                      // VkInstanceCreateFlags flags;
											NULL,									// const VkApplicationInfo* pApplicationInfo;
											0,                                      // uint32_t enabledLayerNameCount;
											NULL,                                   // const char* const* ppEnabledLayerNames;
											0,                                      // uint32_t enabledExtensionNameCount;
											NULL};									// const char* const* ppEnabledExtensionNames;
		VkInstance inst;
		res = vkCreateInstance(&instanceCreateInfo, NULL, &inst);
		if (VK_SUCCESS != res)
			pErrMgr->vkGetError(res,__FILE__,__LINE__);
		
		if (VK_SUCCESS == res)
		{
			extensions += " VK_VERSION_1_0";
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
						extensions += " ";
						extensions += pProperties[i].extensionName;
					}
				}
				delete [] pProperties;
			}
			if (VK_SUCCESS != res)
				pErrMgr->vkGetError(res,__FILE__,__LINE__);

			if (VK_SUCCESS == res)
			{
				uint32_t pPhysicalDeviceCount = 0;
				res = vkEnumeratePhysicalDevices(inst,&pPhysicalDeviceCount,NULL);
				if ((VK_SUCCESS == res) && (pPhysicalDeviceCount > 0))
				{
					VkPhysicalDevice *m_pPhysicalDevices = new VkPhysicalDevice[pPhysicalDeviceCount];
					res = vkEnumeratePhysicalDevices(inst,&pPhysicalDeviceCount,m_pPhysicalDevices);
					if (VK_SUCCESS == res)
					{
						for (uint32_t i=0;i<pPhysicalDeviceCount;i++)
						{
							pPropertyCount = 0;
							res = vkEnumerateDeviceExtensionProperties(m_pPhysicalDevices[i],NULL,&pPropertyCount,NULL);
							if ((VK_SUCCESS == res) && (pPropertyCount > 0))
							{
								VkExtensionProperties* pProperties = new VkExtensionProperties[pPropertyCount];
								res = vkEnumerateDeviceExtensionProperties(m_pPhysicalDevices[i],NULL,&pPropertyCount,pProperties);
								for (uint32_t i=0;i<pPropertyCount;i++)
								{
									extensions += " ";
									extensions += pProperties[i].extensionName;
								}
								delete [] pProperties;
							}
						}
					}
					delete [] m_pPhysicalDevices;
				}
			}

			vkDestroyInstance(inst,NULL);
		}
	}
}
#endif

void RAPTOR_APICALL glActiveTextureARB__default(GLenum target)
{
#ifdef RAPTOR_DEBUG_MODE_GENERATION
    Raptor::GetErrorManager()->generateRaptorError(	Global::COpenGLClassID::GetClassId(),
													CRaptorErrorManager::RAPTOR_ERROR,
													"Using unavailable extension glActiveTextureARB");
#endif
}

void RAPTOR_APICALL glMultiTexCoord4fvARB__default(GLenum target, const GLfloat *v)
{
#ifdef RAPTOR_DEBUG_MODE_GENERATION
    Raptor::GetErrorManager()->generateRaptorError(	Global::COpenGLClassID::GetClassId(),
													CRaptorErrorManager::RAPTOR_ERROR,
													"Using unavailable extension glMultiTexCoord4fvARB");
#endif
	glTexCoord4fv(v);
}

void CRaptorExtensions::defaultInit(void)
{
#if defined(GL_ARB_multitexture)
	if (glMultiTexCoord4fvARB == NULL)
		glMultiTexCoord4fvARB = glMultiTexCoord4fvARB__default;
	if (glActiveTextureARB == NULL)
		glActiveTextureARB = glActiveTextureARB__default;
#endif
}

bool CRaptorExtensions::glIsExtensionSupported(const std::string &ext) const
{	
	bool supported = false;

    supported = (string::npos != extensions.find(ext.data()));
    
	return supported;
}


