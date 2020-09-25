/***************************************************************************/
/*                                                                         */
/*  Raptor.cpp                                                             */
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
	#include "Subsys/ContextManager.h"
#endif
#if !defined(AFX_RAPTORBUFFERDISPLAY_H__DBB1E3EC_EAF3_4EBA_AAB4_C51FA49C5884__INCLUDED_)
	#include "RaptorBufferDisplay.h"
#endif
#if !defined(AFX_RAPTORSCREENDISPLAY_H__D3165157_E39B_4770_990F_26D44A7BD1A3__INCLUDED_)
	#include "RaptorScreenDisplay.h"
#endif
#if !defined(AFX_RAPTORFILTEREDDISPLAY_H__CF464057_8489_43FF_954F_52ECFD722D13__INCLUDED_)
	#include "RaptorFilteredDisplay.h"
#endif
#if !defined(AFX_RAPTORRENDERBUFFERDISPLAY_H__AC23F2BD_DAC5_4B9A_9F75_AD9A2CEEAE08__INCLUDED_)
	#include "RaptorRenderBufferDisplay.h"
#endif
#if !defined(AFX_RAPTORVULKANDISPLAY_H__1D39CB77_CA4D_4A8F_90FA_F9C76774CE6F__INCLUDED_)
	#include "RaptorVulkanDisplay.h"
#endif
#if !defined(AFX_3DENGINETASKMANAGER_H__04149C60_C594_4009_A2C9_F852497146A3__INCLUDED_)
    #include "Engine/3DEngineTaskManager.h"
#endif
#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
    #include "Raptor.h"
#endif
#if !defined(AFX_RENDERENTRYPOINT_H__0C53B135_BE95_4B42_AE37_0CBE00093DEB__INCLUDED_)
	#include "RenderEntryPoint.h"
#endif
#if !defined(AFX_TEXTUREFACTORY_H__1B470EC4_4B68_11D3_9142_9A502CBADC6B__INCLUDED_)
	#include "GLHierarchy/TextureFactory.h"
#endif
#if !defined(AFX_TIMEOBJECT_H__C06AC4B9_4DD7_49E2_9C5C_050EF5C39780__INCLUDED_)
	#include "Engine/TimeObject.h"
#endif
#if !defined(AFX_RAPTORGLEXTENSIONS_H__E5B5A1D9_60F8_4E20_B4E1_8E5A9CB7E0EB__INCLUDED_)
	#include "System/RaptorGLExtensions.h"
#endif
#if !defined(AFX_RAPTORVKEXTENSIONS_H__B17D6B7F_5AFC_4E34_9D49_8DC6CE9192D6__INCLUDED_)
	#include "System/RaptorVKExtensions.h"
#endif
#if !defined(AFX_RAPTORINSTANCE_H__90219068_202B_46C2_BFF0_73C24D048903__INCLUDED_)
	#include "Subsys/RaptorInstance.h"
#endif
#if !defined(AFX_VULKAN_H__625F6BC5_F386_44C2_85C1_EDBA23B16921__INCLUDED_)
	#include "Subsys/Vulkan/RaptorVulkan.h"
#endif
#if !defined(AFX_RAPTORERRORMANAGER_H__FA5A36CD_56BC_4AA1_A5F4_451734AD395E__INCLUDED_)
	#include "System/RaptorErrorManager.h"
#endif
#if !defined(AFX_OPENGL_H__6C8840CA_BEFA_41DE_9879_5777FBBA7147__INCLUDED_)
	#include "Subsys/OpenGL/RaptorOpenGL.h"
#endif


RAPTOR_NAMESPACE

static const char* RAPTOR_VERSION_STRING = RAPTOR_VERSION_STR;



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Raptor::Raptor()
{

}

Raptor::~Raptor()
{
    glQuitRaptor();
}

const CRaptorConfig& Raptor::GetConfig(void)
{
	CRaptorInstance &instance = CRaptorInstance::GetInstance();
	return instance.config;
};

CRaptorConsole *const Raptor::GetConsole(void)
{
	CRaptorInstance &instance = CRaptorInstance::GetInstance();
	if (instance.pConsole == NULL)
		instance.pConsole = new CRaptorConsole;

	return instance.pConsole;
}

bool Raptor::glIsExtensionSupported(const std::string &ext)
{
	const CRaptorGLExtensions *const extensions = CContextManager::GetInstance()->glGetExtensions();
	if (extensions != NULL)
		return extensions->glIsExtensionSupported(ext);
	else
		return false;
}

const CRaptorGLExtensions *const Raptor::glGetExtensions(void)
{
	return CContextManager::GetInstance()->glGetExtensions();
}

bool Raptor::vkIsExtensionSupported(const std::string &ext)
{
	const CRaptorVKExtensions *const extensions = CContextManager::GetInstance()->vkGetExtensions();
	if (extensions != NULL)
		return extensions->vkIsExtensionSupported(ext);
	else
		return false;
}

const CRaptorVKExtensions *const Raptor::vkGetExtensions(void)
{
	return CContextManager::GetInstance()->vkGetExtensions();
}

CRaptorMessages * const Raptor::GetMessages(void)
{
	CRaptorInstance &instance = CRaptorInstance::GetInstance();
    return instance.pMessages;
}

CRaptorErrorManager * const Raptor::GetErrorManager(void)
{
	CRaptorInstance &instance = CRaptorInstance::GetInstance();
    return instance.pErrorMgr;
}

unsigned long Raptor::GetVersion(void)
{
	return RAPTOR_VERSION;
}

const char* Raptor::GetVersionString(void)
{
	return RAPTOR_VERSION_STRING;
}

int Raptor::glPurgeRaptor(bool count)
{
	//	Clean user objects
	size_t nb = CPersistence::NbInstance();

	if (!count)
	{
		while (nb>0)
		{
			void *pos = NULL;
			CPersistence *obj = NULL;

			do
			{
				obj = CPersistence::Object((void*&)pos);
				if (obj!=NULL)
				{
#ifdef RAPTOR_DEBUG_MODE_GENERATION
					stringstream str;
					str << "Deleting: ";
					str << obj << " - ";
					str << obj->getName();
					str << ends;
					RAPTOR_NO_ERROR(COpenGL::COpenGLClassID::GetClassId(),str.str().c_str());
#endif
					pos = NULL;
					delete obj;
					//	We need to restart from first object because destruction of obj removed
					//	elements from global objects map and thus, pos is no longer valid.
					obj = CPersistence::Object((void*&)pos);
				}
			}
			while (pos != NULL);

			nb = CPersistence::NbInstance();
		}

		nb = 0;
	}

	if (NULL != GetErrorManager())
	{
		CATCH_GL_ERROR
	}

	return (int)nb;
}

bool Raptor::glCheckDisplayConfig(const CRaptorDisplayConfig &pcs)
{
	CRaptorInstance &instance = CRaptorInstance::GetInstance();
	if ((!instance.isInitialised()) || (instance.terminate()))
	{
#ifdef RAPTOR_DEBUG_MODE_GENERATION
		RAPTOR_ERROR(COpenGL::COpenGLClassID::GetClassId(), "Raptor is not Initialised !");
#endif
        return false;
	}


    bool valid = true;
    CContextManager *ctxMgr = CContextManager::GetInstance();

	if ((pcs.renderer == CRaptorDisplayConfig::PIXEL_BUFFER) ||
		(pcs.renderer == CRaptorDisplayConfig::PIXEL_BUFFER_FILTER_CHAIN))
    {
		//	Also check texture binding
		if (!Raptor::glIsExtensionSupported(GL_ARB_PIXEL_BUFFER_OBJECT_EXTENSION_NAME))
			valid = false;
    }
    else
    {
		if ((pcs.renderer == CRaptorDisplayConfig::RENDER_BUFFER) ||
			(pcs.renderer == CRaptorDisplayConfig::RENDER_BUFFER_FILTER_CHAIN))
		{
			if ((!Raptor::glIsExtensionSupported(GL_EXT_FRAMEBUFFER_OBJECT_EXTENSION_NAME)) ||
				(!Raptor::glIsExtensionSupported(GL_EXT_FRAMEBUFFER_BLIT_EXTENSION_NAME)))
				valid = false;
		}

		if (valid)
		{
			// copy config to create an invisible window
			CRaptorDisplayConfig pcs2 = pcs;
			pcs2.width = -1;
			pcs2.height = -1;

			CRaptorDisplay *pDisplay = NULL;
			CContextManager::RENDERING_CONTEXT_ID ctx = CContextManager::INVALID_CONTEXT;
			RAPTOR_HANDLE wnd = ctxMgr->glCreateWindow(pcs2, pDisplay, ctx);
			if (wnd.handle() != 0)
			{
				valid = true;
				pDisplay->glvkBindDisplay(wnd);
				pDisplay->glvkReleaseResources();
				pDisplay->glvkUnBindDisplay();
				glDestroyDisplay(pDisplay);
				ctxMgr->glDestroyWindow(wnd);
			}
			else
				return false;
		}
    }

    return valid;
}

RAPTOR_HANDLE Raptor::glCreateWindow(const CRaptorDisplayConfig& pcs,CRaptorDisplay *&pDisplay)
{
	RAPTOR_HANDLE result;

	CRaptorInstance &instance = CRaptorInstance::GetInstance();
	if ((!instance.isInitialised()) || (instance.terminate()))
	{
#ifdef RAPTOR_DEBUG_MODE_GENERATION
		RAPTOR_ERROR(COpenGL::COpenGLClassID::GetClassId(), "Raptor is not Initialised !");
#endif
        return result;
	}

    CContextManager *ctxMgr = CContextManager::GetInstance();
    if (ctxMgr != NULL)
    {
		CContextManager::RENDERING_CONTEXT_ID ctx = CContextManager::INVALID_CONTEXT;
        result = ctxMgr->glCreateWindow(pcs,pDisplay,ctx);
    }
    return result;
}

CRaptorDisplay* Raptor::glCreateDisplay(const CRaptorDisplayConfig& pcs)
{
	CRaptorInstance &instance = CRaptorInstance::GetInstance();
	if ((!instance.isInitialised()) || (instance.terminate()))
	{
#ifdef RAPTOR_DEBUG_MODE_GENERATION
		RAPTOR_ERROR(COpenGL::COpenGLClassID::GetClassId(), "Raptor is not Initialised !");
#endif
        return NULL;
	}

	instance.setDefaultConfig(pcs);
	unsigned int display_mode = pcs.display_mode;

	CRaptorDisplay *pDisplay = NULL;
	
	if ((CRaptorDisplayConfig::RENDER_BUFFER_FILTER_CHAIN == pcs.renderer) ||
		(CRaptorDisplayConfig::PIXEL_BUFFER_FILTER_CHAIN == pcs.renderer))
		pDisplay = new CRaptorFilteredDisplay(pcs);
	else if (CRaptorDisplayConfig::RENDER_BUFFER == pcs.renderer)
		pDisplay = new CRaptorRenderBufferDisplay(pcs);
	else if (CRaptorDisplayConfig::PIXEL_BUFFER == pcs.renderer)
		pDisplay = new CRaptorBufferDisplay(pcs);
	else if (CRaptorDisplayConfig::VULKAN == pcs.renderer)
		pDisplay = new CRaptorVulkanDisplay(pcs);
	else	// Native renderer
		pDisplay = new CRaptorScreenDisplay(pcs);

	instance.displays.push_back(pDisplay);
	return pDisplay;
}

void Raptor::glDestroyDisplay(CRaptorDisplay* pDisplay)
{
	CRaptorInstance &instance = CRaptorInstance::GetInstance();
	if (!instance.isInitialised())
	{
#ifdef RAPTOR_DEBUG_MODE_GENERATION
		RAPTOR_ERROR(CRaptorDisplay::CRaptorDisplayClassID::GetClassId(), "Raptor is not Initialised !");
#endif
        return;
	}

	if (pDisplay != NULL)
	{
		pDisplay->glvkReleaseResources();

		if (pDisplay == CRaptorDisplay::GetCurrentDisplay())
		{
#ifdef RAPTOR_DEBUG_MODE_GENERATION
			RAPTOR_WARNING(CRaptorDisplay::CRaptorDisplayClassID::GetClassId(), "Cannot destroy a Display currently bound !");
#endif
			pDisplay->glvkUnBindDisplay();
		}

		vector<CRaptorDisplay*>::iterator itr = instance.displays.begin();
			
		while (itr != instance.displays.end())
		{
			if ((*itr) == pDisplay)
				itr = instance.displays.erase(itr);
			else
				itr++;
		}

		delete pDisplay;
	}
}

CRaptorInstance* Raptor::glvkCreateInstance(const CRaptorConfig& config)
{
	CRaptorInstance* previous = CRaptorInstance::createNewInstance();
	
	CRaptorInstance &new_instance = CRaptorInstance::GetInstance();
	new_instance.config = config;
	new_instance.initInstance();

	//	Create a dummy window to initialize GL
	CRaptorDisplayConfig glCS;
	glCS.width = -1;
	glCS.height = -1;
	glCS.caption = "Raptor Default Display";
	glCS.display_mode = CGL_RGBA;
	glCS.acceleration = CRaptorDisplayConfig::GENERIC;
	glCS.refresh_rate.fps = CGL_MAXREFRESHRATE;

	CRaptorDisplay *pDisplay = NULL;
	CContextManager *ctxMgr = CContextManager::GetInstance();
	CContextManager::RENDERING_CONTEXT_ID ctx = CContextManager::INVALID_CONTEXT;
	RAPTOR_HANDLE wnd = ctxMgr->glCreateWindow(glCS, pDisplay, ctx);

	new_instance.defaultWindow = wnd;
	new_instance.defaultContext = ctx;
	new_instance.defaultDisplay = pDisplay;

	//	Initialize platform dependant datas.
	CContextManager::GetInstance()->glMakeCurrentContext(new_instance.defaultWindow, 
														 new_instance.defaultContext);
	CTextureFactory::glGetDefaultFactory();
	CATCH_GL_ERROR

	//! Release context and return init state.
	RAPTOR_HANDLE noDevice(0, (void*)0);
	CContextManager::GetInstance()->glMakeCurrentContext(noDevice, 
														 new_instance.defaultContext);

	return previous;
}

CRaptorInstance* Raptor::switchInstance(CRaptorInstance* pInstance)
{
	if (NULL == pInstance)
		return NULL;

	//	A first instance needs to be initialised.
	CRaptorInstance &instance = CRaptorInstance::GetInstance();
	if (!instance.isInitialised())
		return NULL;
}

bool Raptor::glInitRaptor(const CRaptorConfig& config)
{ 
    bool res = false;

	//	Initialise global Raptor data
	CRaptorInstance &instance = CRaptorInstance::GetInstance();
	if (instance.isInitialised())
        return true;
	
	//  store configuration and initialize platform dependant datas. 
	//	Some configurations can only be applyied per screen display : 
    //  e.g.: memory allocation is dependant of a real context.
	instance.config = config;
	instance.initInstance();
	
	//	Create a dummy window to initialize GL
	CRaptorDisplayConfig glCS;
	glCS.width = -1;
	glCS.height = -1;
	glCS.caption = "Raptor Default Display";
	glCS.display_mode = CGL_RGBA;
	glCS.acceleration = CRaptorDisplayConfig::GENERIC;
	glCS.refresh_rate.fps = CGL_MAXREFRESHRATE;

    CRaptorDisplay *pDisplay = NULL;
    CContextManager *ctxMgr = CContextManager::GetInstance();
	CContextManager::RENDERING_CONTEXT_ID ctx = CContextManager::INVALID_CONTEXT;
	RAPTOR_HANDLE wnd = ctxMgr->glCreateWindow(glCS,pDisplay,ctx);

	instance.defaultWindow = wnd;
	instance.defaultContext = ctx;
    instance.defaultDisplay = pDisplay;

	//	Initialize platform dependant datas.
	CContextManager::GetInstance()->glMakeCurrentContext(instance.defaultWindow, instance.defaultContext);
	CTextureFactory::glGetDefaultFactory();
	CATCH_GL_ERROR

	//! Release context and return init state.
	RAPTOR_HANDLE noDevice;
	CContextManager::GetInstance()->glMakeCurrentContext(noDevice, instance.defaultContext);

	return (wnd.handle() != 0);
}

void Raptor::glRender(void)
{
	//! Do not display debug information,
	//!	because this method is likely to be called in a loop!
	CRaptorInstance &instance = CRaptorInstance::GetInstance();
	if ((!instance.isInitialised()) || (instance.terminate()))
		return;

	//!	Update global time reference.
	CTimeObject::deltaTime();

	vector<CRenderEntryPoint*>::iterator itr = instance.renderEntryPoints.begin();
	vector<CRenderEntryPoint*>::iterator end = instance.renderEntryPoints.end();

	while (itr != end)
	{
		CRenderEntryPoint* rep = (*itr);
		rep->glRender();
		itr++;
	}

    //! buffer swapping and context refresh are very long,
    //! we can use this time more to update dynamic objects before next rendering
    //! ( it enables more objects to be updated before a render is asked ).
    C3DEngineTaskManager *engine = instance.engineTaskMgr;
	if (engine != NULL)
		engine->run();

	CHostMemoryManager *memory = CHostMemoryManager::GetInstance();
	memory->pack();
}

bool Raptor::glQuitRaptor(void)
{
	CRaptorInstance &instance = CRaptorInstance::GetInstance();
	if ((!instance.isInitialised()) || (instance.terminate()))
        return true;

    bool res = instance.destroy();

	glPurgeRaptor(false);

	return res;
}
