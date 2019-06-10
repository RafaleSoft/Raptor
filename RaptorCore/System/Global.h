#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#include "Subsys/CodeGeneration.h"

#if !defined(AFX_RAPTORERRORMANAGER_H__FA5A36CD_56BC_4AA1_A5F4_451734AD395E__INCLUDED_)
    #include "RaptorErrorManager.h"
#endif
#if !defined(AFX_RAPTORDISPLAYCONFIG_H__DA0759DF_6CF9_44A7_9ADE_D404FEEC2DDF__INCLUDED_)
	#include "RaptorDisplayConfig.h"
#endif
#if !defined(AFX_RAPTORCONSOLE_H__27656611_2DF3_4416_8124_F608CFAC2122__INCLUDED_)
	#include "RaptorConsole.h"
#endif
#if !defined(AFX_RAPTORCONFIG_H__29B753B8_17DE_44DF_A4D2_9D19C5AC53D5__INCLUDED_)
	#include "System/RaptorConfig.h"
#endif



RAPTOR_NAMESPACE_BEGIN

#define SHAREWARE_RELEASE 1

class CAnimator;
class CRaptorMessages;
class CRaptorErrorManager;
class C3DEngineTaskManager;
class CRaptorDisplay;
class CRenderEntryPoint;

#ifdef WIN32
	#pragma warning(disable:4786)
#endif


class Global
{
public:
	//!	The global status structure of Raptor.
	typedef struct RAPTOR_CURRENT_STATUS_t
	{
		bool					initialised;			//	raptor has been initialised
		bool					forceSSE;
		bool					runAsShareware;
		bool					terminate;				//	the second pipeline has exited, raptor can be closed safely
        CRaptorConsole			*console;
		CAnimator				*currentAnimator;		//	current animator
        C3DEngineTaskManager	*engineTaskMgr;

		//!	default display creation structure + defaut initial state of the renderer.
		CRaptorDisplayConfig	defaultConfig;

        unsigned int			iRenderedObjects;
        unsigned int			iRenderedTriangles;
		
        CRaptorDisplay			*defaultDisplay;
		RAPTOR_HANDLE			defaultWindow;
		long					defaultContext;
		vector<CRaptorDisplay*>	displays;
		vector<CRenderEntryPoint*>	  renderEntryPoints;
		vector<CRaptorConsole::CInputCollectorBase*>	inputCollectors;
	} RAPTOR_CURRENT_STATUS;
	typedef RAPTOR_CURRENT_STATUS *LPRAPTOR_CURRENT_STATUS;


public:
    //! Returns the singleton
	static Global&	GetInstance();

    //! Initialise Raptor Global status.
	//! Must be called once only or after a call to destroy.
	bool	init(const CRaptorConfig& config);

    //! Delete Raptor status and any allocated resource.
    bool	destroy(void);

    //! Returns Raptor's current status
	RAPTOR_CURRENT_STATUS& getCurrentStatus(void)
	{ return raptorStatus; }

    //! Stores Display attributes for delayed creation.
    //! The physical display creation is delegated to the underlying API, 
    //! and might not be synchronous to the return of the creation method.
    //! Thus, attributes can be saved temporarily until the real creation happens.
    //! When a display is created directly (@see Raptor class ), this method is unnecessary
	void setDefaultConfig(const CRaptorDisplayConfig &pcs);

    //! Returns the defaut initial state of the renderer.
    //! Data is gathered from RaptorData, so different run state can be used with various RaptorData.
    const CRaptorDisplayConfig& getDefaultConfig(void) const { return raptorStatus.defaultConfig; }

    //! Returns the console if it has been created.
    CRaptorConsole* const getConsole(void) const { return raptorStatus.console; };

	//! Emulates CPersistence for global opengl classes IDs & global namespace
	DECLARE_CLASS_ID(COpenGLClassID,"OpenGL",CPersistence)

	//! Emulates CPersistence for global vulkan classes IDs & global namespace
	DECLARE_CLASS_ID(CVulkanClassID,"Vulkan",CPersistence)


private:
	Global();
	virtual ~Global();

	//! One single instance of global is allowed
	static Global			*pInstance;

	RAPTOR_CURRENT_STATUS	raptorStatus;
    
};

RAPTOR_NAMESPACE_END

#endif

