// RaptorExtensions.cpp: implementation of the RaptorExtensions class.
//
//////////////////////////////////////////////////////////////////////
#include "Subsys/CodeGeneration.h"

#if !defined(AFX_RAPTORVKEXTENSIONS_H__B17D6B7F_5AFC_4E34_9D49_8DC6CE9192D6__INCLUDED_)
	#include "RaptorVKExtensions.h"
#endif
#ifndef __vkext_macros_h_
	#include "VKEXTMacros.h"
#endif
#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
	#include "Raptor.h"
#endif
#if !defined(AFX_RAPTORERRORMANAGER_H__FA5A36CD_56BC_4AA1_A5F4_451734AD395E__INCLUDED_)
    #include "RaptorErrorManager.h"
#endif
#ifndef __GLOBAL_H__
	#include "Global.h"
#endif
#if defined(VK_VERSION_1_0)
	#if !defined(AFX_RAPTORVULKANMEMORY_H__72256FF7_DBB9_4B9C_9BF7_C36F425CF811__INCLUDED_)
		#include "Subsys/Vulkan/VulkanMemory.h"
	#endif
#endif

RAPTOR_NAMESPACE_BEGIN

	std::string CRaptorVKExtensions::instance_extensions = "";

#if defined(VK_VERSION_1_0)
	VkInstance CRaptorVKExtensions::instance = VK_NULL_HANDLE;

	PFN_vkGetInstanceProcAddr CRaptorVKExtensions::vkGetInstanceProcAddr = NULL;

	PFN_vkEnumerateInstanceExtensionProperties CRaptorVKExtensions::vkEnumerateInstanceExtensionProperties = NULL;
	PFN_vkEnumerateInstanceLayerProperties CRaptorVKExtensions::vkEnumerateInstanceLayerProperties = NULL;
	PFN_vkCreateInstance CRaptorVKExtensions::vkCreateInstance = NULL;

	IMPLEMENT_RAPTOR_VK_instance(CRaptorVKExtensions)
#endif

RAPTOR_NAMESPACE_END


RAPTOR_NAMESPACE


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CRaptorVKExtensions::CRaptorVKExtensions(const std::string &ext)
	:extensions(ext)
{
#if defined(VK_VERSION_1_0)
	IMPLEMENT_VK_win32(this, instance);
	IMPLEMENT_VK_xlib(this, instance);
	IMPLEMENT_VK_KHR_surface(this, instance);
#endif
}

CRaptorVKExtensions::~CRaptorVKExtensions()
{
}

bool CRaptorVKExtensions::vkInitExtensions(void)
{
	CRaptorErrorManager *pErrMgr = Raptor::GetErrorManager();
	if (NULL == vkGetInstanceProcAddr)
	{
		pErrMgr->generateRaptorError(Global::CVulkanClassID::GetClassId(),
										CRaptorErrorManager::RAPTOR_ERROR,
										"Unable to initialise a Vulkan entry points");
		return false;
	}
		
#if !defined(RAPTOR_DEBUG_MODE_GENERATION)
	if ((NULL != vkEnumerateInstanceExtensionProperties) ||
		(NULL != vkEnumerateInstanceLayerProperties) ||
		(NULL != vkCreateInstance))
	{
		pErrMgr->generateRaptorError(Global::CVulkanClassID::GetClassId(),
									 CRaptorErrorManager::RAPTOR_WARNING,
									 "Vulkan Extensions already initialised");
	}
#endif

	vkEnumerateInstanceExtensionProperties = (PFN_vkEnumerateInstanceExtensionProperties)vkGetInstanceProcAddr(NULL, "vkEnumerateInstanceExtensionProperties");
	vkEnumerateInstanceLayerProperties = (PFN_vkEnumerateInstanceLayerProperties)vkGetInstanceProcAddr(NULL, "vkEnumerateInstanceLayerProperties");
	vkCreateInstance = (PFN_vkCreateInstance)vkGetInstanceProcAddr(NULL, "vkCreateInstance");
	
	CATCH_VK_ERROR(VK_SUCCESS);

	if ((NULL == vkEnumerateInstanceExtensionProperties) ||
		(NULL == vkEnumerateInstanceLayerProperties) ||
		(NULL == vkCreateInstance))
		return false;

	instance_extensions = "";
	VkResult res = VK_NOT_READY;

	uint32_t pPropertyCount = 0;
	res = vkEnumerateInstanceExtensionProperties(NULL, &pPropertyCount, NULL);
	if ((VK_SUCCESS == res) && (pPropertyCount > 0))
	{
		VkExtensionProperties *pProperties = new VkExtensionProperties[pPropertyCount];
		res = vkEnumerateInstanceExtensionProperties(NULL, &pPropertyCount, pProperties);
		if (VK_SUCCESS == res)
		{
			for (uint32_t i = 0; i<pPropertyCount; i++)
			{
				instance_extensions += " ";
				instance_extensions += pProperties[i].extensionName;
			}
		}
		delete[] pProperties;
	}

	uint32_t pLayerCount = 0;
	res = vkEnumerateInstanceLayerProperties(&pLayerCount, NULL);
	if ((VK_SUCCESS == res) && (pLayerCount > 0))
	{
		VkLayerProperties *pProperties = new VkLayerProperties[pLayerCount];
		uint32_t nbLayerCount = pLayerCount;
		res = vkEnumerateInstanceLayerProperties(&nbLayerCount, pProperties);
		if (VK_SUCCESS == res)
		{
			for (uint32_t i = 0; i<pLayerCount; i++)
			{
				instance_extensions += " ";
				instance_extensions += pProperties[i].layerName;
			}
		}
		delete[] pProperties;
	}

	if (VK_SUCCESS != res)
	{
		pErrMgr->vkGetError(res, __FILE__, __LINE__);
		return false;
	}
	else
		return true;
}

bool CRaptorVKExtensions::vkInitInstanceExtensions(void)
{
	if (!CRaptorVKExtensions::vkInitExtensions())
		return false;

	CRaptorErrorManager *pErrMgr = Raptor::GetErrorManager();
	if (NULL == vkCreateInstance)
	{
		pErrMgr->generateRaptorError(Global::CVulkanClassID::GetClassId(),
									 CRaptorErrorManager::RAPTOR_ERROR,
									 "Unable to initialise a Vulkan Instance");
		return false;
	}

	VkApplicationInfo applicationInfo = {	VK_STRUCTURE_TYPE_APPLICATION_INFO,
											NULL,
											"Raptor",
											RAPTOR_VERSION,
											"Raptor 3D Engine",
											RAPTOR_VERSION,
											VK_API_VERSION_1_0 };

	const char* extensions[3] = { NULL, NULL, NULL };
	uint32_t nbExtensions = 0;

#if defined(RAPTOR_DEBUG_MODE_GENERATION) && defined(VK_EXT_debug_report)
	if (string::npos != instance_extensions.find(VK_EXT_DEBUG_REPORT_EXTENSION_NAME))
	{
		extensions[nbExtensions] = VK_EXT_DEBUG_REPORT_EXTENSION_NAME;
		nbExtensions++;
	}
#endif
#if defined (VK_KHR_surface)
	if (string::npos != instance_extensions.find(VK_KHR_SURFACE_EXTENSION_NAME))
	{
		extensions[nbExtensions] = VK_KHR_SURFACE_EXTENSION_NAME;
		nbExtensions++;
	}
#endif
#if defined (VK_KHR_win32_surface)
	if (string::npos != instance_extensions.find(VK_KHR_WIN32_SURFACE_EXTENSION_NAME))
	{
		extensions[nbExtensions] = VK_KHR_WIN32_SURFACE_EXTENSION_NAME;
		nbExtensions++;
	}
#endif

	VkInstanceCreateInfo instanceCreateInfo = { VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO, // VkStructureType sType;
												NULL,                                   // const void* pNext;
												0,                                      // VkInstanceCreateFlags flags;
												&applicationInfo,						// const VkApplicationInfo* pApplicationInfo;
												0,                                      // uint32_t enabledLayerNameCount;
												NULL,                                   // const char* const* ppEnabledLayerNames;
												nbExtensions,                           // uint32_t enabledExtensionNameCount;
												extensions };							// const char* const* ppEnabledExtensionNames;

	VkResult res = VK_NOT_READY;
	res = CRaptorVKExtensions::vkCreateInstance(&instanceCreateInfo,
												CVulkanMemory::GetAllocator(),
												&instance);
	if (VK_SUCCESS != res)
		pErrMgr->vkGetError(res, __FILE__, __LINE__);

	IMPLEMENT_VK_instance(CRaptorVKExtensions, instance);

	return true;
}

bool CRaptorVKExtensions::vkInitDeviceExtensions(VkPhysicalDevice device)
{
	VkResult res = VK_SUCCESS;

	std::vector<std::string> layers;
	uint32_t pLayerCount = 0;
	res = vkEnumerateDeviceLayerProperties(device, &pLayerCount, NULL);
	if ((VK_SUCCESS == res) && (pLayerCount > 0))
	{
		VkLayerProperties* pProperties = new VkLayerProperties[pLayerCount];
		res = vkEnumerateDeviceLayerProperties(device, &pLayerCount, pProperties);
		for (uint32_t j = 0; j < pLayerCount; j++)
			layers.push_back(pProperties[j].layerName);
		delete[] pProperties;
	}
	else
		return false;

	uint32_t pPropertyCount = 0;
	res = vkEnumerateDeviceExtensionProperties(device, NULL, &pPropertyCount, NULL);
	if ((VK_SUCCESS == res) && (pPropertyCount > 0))
	{
		VkExtensionProperties* pProperties = new VkExtensionProperties[pPropertyCount];
		res = vkEnumerateDeviceExtensionProperties(device, NULL, &pPropertyCount, pProperties);
		for (uint32_t j = 0; j<pPropertyCount; j++)
		{
			extensions += " ";
			extensions += pProperties[j].extensionName;
		}
		delete[] pProperties;
	}
	else
		return false;

	for (size_t l = 0; l < layers.size(); l++)
	{
		res = vkEnumerateDeviceExtensionProperties(device, layers[l].c_str() , &pPropertyCount, NULL);
		if ((VK_SUCCESS == res) && (pPropertyCount > 0))
		{
			VkExtensionProperties* pProperties = new VkExtensionProperties[pPropertyCount];
			res = vkEnumerateDeviceExtensionProperties(device, layers[l].c_str(), &pPropertyCount, pProperties);
			for (uint32_t j = 0; j<pPropertyCount; j++)
			{
				extensions += " ";
				extensions += pProperties[j].extensionName;
			}
			delete[] pProperties;
		}
	}

	return true;
}

bool CRaptorVKExtensions::vkIsExtensionSupported(const std::string &ext) const
{	
	bool supported = false;

    supported = (string::npos != extensions.find(ext.data()));
	if (!supported)
		supported = (string::npos != instance_extensions.find(ext.data()));
    
	return supported;
}


