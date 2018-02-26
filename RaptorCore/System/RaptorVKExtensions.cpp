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
{
	extensions = ext;
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
	if (vkIsExtensionSupported("VK_EXT_debug_report"))
	{
		extensions[nbExtensions] = "VK_EXT_debug_report";
		nbExtensions++;
	}
#endif
#if defined (VK_KHR_surface)
	if (string::npos != instance_extensions.find("VK_KHR_surface"))
	{
		extensions[nbExtensions] = "VK_KHR_surface";
		nbExtensions++;
	}
#endif
#if defined (VK_KHR_win32_surface)
	if (string::npos != instance_extensions.find("VK_KHR_win32_surface"))
	{
		extensions[nbExtensions] = "VK_KHR_win32_surface";
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

	IMPLEMENT_VK_instance(CRaptorVKExtensions::, instance);

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

