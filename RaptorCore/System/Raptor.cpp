// Raptor.cpp: implementation of the Raptor class.
//
//////////////////////////////////////////////////////////////////////

#include "Subsys/CodeGeneration.h"

#ifndef __GLOBAL_H__
	#include "System/Global.h"
#endif
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
#if !defined(AFX_TEXTUREFACTORYCONFIG_H__7A20D208_423F_4E02_AA4D_D736E0A7959F__INCLUDED_)
	#include "GLHierarchy/TextureFactoryConfig.h"
#endif
#if !defined(AFX_TIMEOBJECT_H__C06AC4B9_4DD7_49E2_9C5C_050EF5C39780__INCLUDED_)
	#include "Engine/TimeObject.h"
#endif
#if !defined(AFX_RAPTOREXTENSIONS_H__E5B5A1D9_60F8_4E20_B4E1_8E5A9CB7E0EB__INCLUDED_)
	#include "System/RaptorExtensions.h"
#endif
#if !defined(AFX_RAPTORERRORMANAGER_H__FA5A36CD_56BC_4AA1_A5F4_451734AD395E__INCLUDED_)
    #include "RaptorErrorManager.h"
#endif


RAPTOR_NAMESPACE

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
	return Global::GetInstance().getConfig(); 
};

CRaptorConsole *const Raptor::GetConsole(void)
{
	Global::RAPTOR_CURRENT_STATUS& status = Global::GetInstance().getCurrentStatus();
	if (status.console == NULL)
		status.console = new CRaptorConsole;

	return status.console;
}

bool Raptor::glIsExtensionSupported(const std::string &ext)
{
	const CRaptorExtensions *const extensions = CContextManager::GetInstance()->glGetExtensions();
	if (extensions != NULL)
		return extensions->glIsExtensionSupported(ext);
	else
		return false;
}

const CRaptorExtensions *const Raptor::glGetExtensions(void)
{
	return CContextManager::GetInstance()->glGetExtensions();
}

CRaptorMessages * const Raptor::GetMessages(void)
{
    Global::RAPTOR_CURRENT_STATUS &st = Global::GetInstance().getCurrentStatus();
    return st.messages;
}

CRaptorErrorManager * const Raptor::GetErrorManager(void)
{
    Global::RAPTOR_CURRENT_STATUS &st = Global::GetInstance().getCurrentStatus();
    return st.errorMgr;
}

unsigned long Raptor::GetVersion(void)
{
	return RAPTOR_VERSION;
}

const char* Raptor::GetVersionString(void)
{
	return RAPTOR_VERSION_STR;
}

int Raptor::glPurgeRaptor(bool count)
{
	//	Clean user objects
	int nb = CPersistence::NbInstance();

	if (!count)
	{
        Global::RAPTOR_CURRENT_STATUS& raptorStatus = Global::GetInstance().getCurrentStatus();

        //! Use integer pos because glReleaseResources may destroy some displays
		unsigned int dPos = 0;
		while (dPos < raptorStatus.displays.size())
		{
            CRaptorDisplay *pDisplay = raptorStatus.displays[dPos++];
            pDisplay->glReleaseResources();
        }

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
        Raptor::GetErrorManager()->generateRaptorError(	Global::COpenGLClassID::GetClassId(),
														CRaptorErrorManager::RAPTOR_NO_ERROR,
														str.str().c_str());
#endif
					pos = NULL;
					delete obj;
					//	We need to restart from first object
					//	Because destruction of obj removed
					//	elements from static objects map and
					//	thus, pos is no longer valid.
					obj = CPersistence::Object((void*&)pos);
				}
			}
			while (pos != NULL);

			nb = CPersistence::NbInstance();
		}

		nb = 0;
	}

    CATCH_GL_ERROR

	return nb;
}

bool Raptor::glCheckDisplayConfig(const CRaptorDisplayConfig &pcs)
{
   	Global::RAPTOR_CURRENT_STATUS& status = Global::GetInstance().getCurrentStatus();
    if ((!status.initialised) || (status.terminate))
	{
#ifdef RAPTOR_DEBUG_MODE_GENERATION
		Raptor::GetErrorManager()->generateRaptorError(	Global::COpenGLClassID::GetClassId(),
														CRaptorErrorManager::RAPTOR_ERROR,
														"Raptor is not Initialised !");
#endif
        return false;
	}


    bool valid = false;
    CContextManager *ctxMgr = CContextManager::GetInstance();

	if (pcs.renderer == CRaptorDisplayConfig::PIXEL_BUFFER)
    {
        // TODO
		//	Also check texture binding ?
    }
    else
    {
        // copy config to create an invisible window
        CRaptorDisplayConfig pcs2 = pcs;
        pcs2.width = -1;
        pcs2.height = -1;

        CRaptorDisplay *pDisplay = NULL;
        CContextManager::RENDERING_CONTEXT_ID ctx = -1;
        RAPTOR_HANDLE wnd = ctxMgr->glCreateWindow(pcs2,pDisplay,ctx);
        if (wnd.handle != 0)
        {
            valid = true;
            glDestroyDisplay(pDisplay);
            ctxMgr->glDestroyWindow(wnd);
        }
    }

    return valid;
}

RAPTOR_HANDLE Raptor::glCreateWindow(const CRaptorDisplayConfig& pcs,CRaptorDisplay *&pDisplay)
{
	RAPTOR_HANDLE result;

	Global::RAPTOR_CURRENT_STATUS& status = Global::GetInstance().getCurrentStatus();
    if ((!status.initialised) || (status.terminate))
	{
#ifdef RAPTOR_DEBUG_MODE_GENERATION
		Raptor::GetErrorManager()->generateRaptorError(	Global::COpenGLClassID::GetClassId(),
														CRaptorErrorManager::RAPTOR_ERROR,
														"Raptor is not Initialised !");
#endif
        return result;
	}

    CContextManager *ctxMgr = CContextManager::GetInstance();
    if (ctxMgr != NULL)
    {
        CContextManager::RENDERING_CONTEXT_ID ctx = -1;
        result = ctxMgr->glCreateWindow(pcs,pDisplay,ctx);
    }
    return result;
}

CRaptorDisplay* Raptor::glCreateDisplay(const CRaptorDisplayConfig& pcs)
{
	Global::RAPTOR_CURRENT_STATUS& status = Global::GetInstance().getCurrentStatus();
    if ((!status.initialised) || (status.terminate))
	{
#ifdef RAPTOR_DEBUG_MODE_GENERATION
		Raptor::GetErrorManager()->generateRaptorError(	Global::COpenGLClassID::GetClassId(),
														CRaptorErrorManager::RAPTOR_ERROR,
														"Raptor is not Initialised !");
#endif
        return NULL;
	}

	Global::GetInstance().setDefaultConfig(pcs);
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

	Global::GetInstance().getCurrentStatus().displays.push_back(pDisplay);
	return pDisplay;
}

void Raptor::glDestroyDisplay(CRaptorDisplay* pDisplay)
{
	Global::RAPTOR_CURRENT_STATUS& status = Global::GetInstance().getCurrentStatus();
    if (!status.initialised)
	{
#ifdef RAPTOR_DEBUG_MODE_GENERATION
		Raptor::GetErrorManager()->generateRaptorError(	CRaptorDisplay::CRaptorDisplayClassID::GetClassId(),
														CRaptorErrorManager::RAPTOR_ERROR,
														"Raptor is not Initialised !");
#endif
        return;
	}

	if (pDisplay != NULL)
	{
		if (pDisplay == CRaptorDisplay::GetCurrentDisplay())
		{
#ifdef RAPTOR_DEBUG_MODE_GENERATION
			Raptor::GetErrorManager()->generateRaptorError(CRaptorDisplay::CRaptorDisplayClassID::GetClassId(),
                                                           CRaptorErrorManager::RAPTOR_WARNING,
											               "Cannot destroy a Display currently bound !");
#endif
			pDisplay->glUnBindDisplay();
		}
	//	else
		{
			Global::RAPTOR_CURRENT_STATUS& raptorStatus = Global::GetInstance().getCurrentStatus();
			vector<CRaptorDisplay*>::iterator itr = raptorStatus.displays.begin();
			
			while (itr != raptorStatus.displays.end())
			{
				if ((*itr) == pDisplay)
					itr = raptorStatus.displays.erase(itr);
				else
					itr++;
			}

			delete pDisplay;
		}
	}
}

bool Raptor::glInitRaptor(const CRaptorConfig& config)
{ 
    bool res = false;

	//	Initialise global Raptor data
    Global &rGlobal = Global::GetInstance();
    if (rGlobal.getCurrentStatus().initialised)
        return true;

	//  store configuration and initialize platform dependant datas. 
	//	Some configurations can only be applyied per screen display : 
    //  e.g.: memory allocation is dependant of a real context.
	res = rGlobal.init(config);

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
    CContextManager::RENDERING_CONTEXT_ID ctx = -1;
	RAPTOR_HANDLE wnd = ctxMgr->glCreateWindow(glCS,pDisplay,ctx);

	Global::RAPTOR_CURRENT_STATUS& status = rGlobal.getCurrentStatus();
	status.defaultWindow = wnd;
	status.defaultContext = ctx;
    status.defaultDisplay = pDisplay;

	//	Initialize platform dependant datas.
	CContextManager::GetInstance()->glMakeCurrentContext(status.defaultWindow,status.defaultContext);
	CTextureFactory::getDefaultFactory().getConfig();
	CATCH_GL_ERROR

	//! Release context and return init state.
	RAPTOR_HANDLE noDevice;
	CContextManager::GetInstance()->glMakeCurrentContext(noDevice,status.defaultContext);

	return (res && (wnd.handle != 0));
}

void Raptor::glRender(void)
{
	//!	Update global time reference.
	CTimeObject::deltaTime();

	//! Do not display debug information,
	//!	because this method is likely to be called in a loop!
	Global::RAPTOR_CURRENT_STATUS& raptorStatus = Global::GetInstance().getCurrentStatus();
	if ((!raptorStatus.initialised) || (raptorStatus.terminate))
		return;

	vector<CRenderEntryPoint*>::iterator itr = raptorStatus.renderEntryPoints.begin();
	vector<CRenderEntryPoint*>::iterator end = raptorStatus.renderEntryPoints.end();

	while (itr != end)
	{
		CRenderEntryPoint* rep = (*itr);
		rep->glRender();
		itr++;
	}

    //! buffer swapping and context refresh are very long,
    //! we can use this time more to update dynamic objects before next rendering
    //! ( it enables more objects to be updated before a render is asked ).
    C3DEngineTaskManager *engine = raptorStatus.engineTaskMgr;
	if (engine != NULL)
		engine->run();
}

bool Raptor::glQuitRaptor(void)
{
    Global::RAPTOR_CURRENT_STATUS& status = Global::GetInstance().getCurrentStatus();
    if ((!status.initialised) || (status.terminate))
        return false;

    return Global::GetInstance().destroy();
}
