// ContextManager.cpp: implementation of the CContextManager class.
//
//////////////////////////////////////////////////////////////////////

#include "Subsys/CodeGeneration.h"


#if !defined(AFX_CONTEXTMANAGER_H__F992F5F0_D8A5_475F_9777_B0EB30E7648E__INCLUDED_)
	#include "ContextManager.h"
#endif
#if !defined(AFX_TEXTUREOBJECT_H__D32B6294_B42B_4E6F_AB73_13B33C544AD0__INCLUDED_)
	#include "GLHierarchy/TextureObject.h"
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

RAPTOR_NAMESPACE

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CContextManager::CContextManager()
	:m_logo()
#if defined(VK_VERSION_1_0)
	,m_pExtensions(NULL)
	,m_globalInstance(NULL)
	,m_pPhysicalDevices(NULL)
#endif
{
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
	//rdc.runAsShareware |= ((rdc.frame_mode & CGL_DRAWLOGO) == CGL_DRAWLOGO);
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
	VkResult res = VK_NOT_READY;
	VkApplicationInfo applicationInfo = {	VK_STRUCTURE_TYPE_APPLICATION_INFO, NULL,
											"Raptor", RAPTOR_VERSION,
											"Raptor 3D Engine", RAPTOR_VERSION,
											(1 << 22) + (0 && 12) + 3 };
	VkInstanceCreateInfo instanceCreateInfo = {	VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO, // VkStructureType sType;
												NULL,                                   // const void* pNext;
												0,                                      // VkInstanceCreateFlags flags;
												&applicationInfo,						// const VkApplicationInfo* pApplicationInfo;
												0,                                      // uint32_t enabledLayerNameCount;
												NULL,                                   // const char* const* ppEnabledLayerNames;
												0,                                      // uint32_t enabledExtensionNameCount;
												NULL};									// const char* const* ppEnabledExtensionNames;
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
		res = m_pExtensions->vkEnumeratePhysicalDevices(m_globalInstance,&pPhysicalDeviceCount,NULL);
		if ((VK_SUCCESS == res) && (pPhysicalDeviceCount > 0))
		{
			m_pPhysicalDevices = new VkPhysicalDevice[pPhysicalDeviceCount];
			res = m_pExtensions->vkEnumeratePhysicalDevices(m_globalInstance,&pPhysicalDeviceCount,m_pPhysicalDevices);
			if (VK_SUCCESS == res)
			{
				VkPhysicalDeviceProperties pProperties;
				for (uint32_t i=0;i<pPhysicalDeviceCount;i++)
				{
					m_pExtensions->vkGetPhysicalDeviceProperties(m_pPhysicalDevices[i],&pProperties);

					uint32_t pQueueFamilyPropertyCount = 0;
					m_pExtensions->vkGetPhysicalDeviceQueueFamilyProperties(m_pPhysicalDevices[i],&pQueueFamilyPropertyCount,NULL);
					if (pQueueFamilyPropertyCount > 0)
					{
						VkQueueFamilyProperties *pQueueFamilyProperties = new VkQueueFamilyProperties[pQueueFamilyPropertyCount];
						m_pExtensions->vkGetPhysicalDeviceQueueFamilyProperties(m_pPhysicalDevices[i],&pQueueFamilyPropertyCount,pQueueFamilyProperties);
						for (uint32_t i=0;i<pQueueFamilyPropertyCount;i++)
						{
							
						}
						delete [] pQueueFamilyProperties;
					}
				}
			}
		}
		if (VK_SUCCESS != res)
			pErrMgr->vkGetError(res,__FILE__,__LINE__);
	}

	if (VK_SUCCESS != res)
	{
		pErrMgr->generateRaptorError(	Global::CVulkanClassID::GetClassId(),
										CRaptorErrorManager::RAPTOR_ERROR,
										"Unable to initialise Vulkan API");
	}
	return (VK_SUCCESS == res);
}
#endif