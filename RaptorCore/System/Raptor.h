// Raptor.h: interface for the Raptor class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
#define AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Subsys/CodeGeneration.h"


#if !defined(AFX_RAPTORDISPLAY_H__9637BC66_3734_43A8_A130_87553D4379BC__INCLUDED_)
	#include "RaptorDisplay.h"
#endif


RAPTOR_NAMESPACE_BEGIN

class CRaptorGLExtensions;
class CRaptorVKExtensions;
class CRaptorDisplay;
class CRaptorMessages;
class CRaptorErrorManager;
class CRaptorConsole;
class CRaptorConfig;


class RAPTOR_API Raptor  
{
public:
	static unsigned long GetVersion(void);
	static const char* GetVersionString(void);

	//!	Initialize the entire Raptor SDK. 
    //! This method must be called before any other method. It returns false
    //! if Raptor cannot start initialisation. Otherwise, init errors can be retrieved
    //! using error manager.
	static bool glInitRaptor(const CRaptorConfig& config);

	//! Returns the global config used at initialisation
    static const CRaptorConfig& GetConfig(void);


    //! Quit raptor engine, releasing any allocated resource.
    //! Must be called before init can be called again.
    static bool glQuitRaptor(void);

	//!	This method will destroy all persistant objects currently allocated.
	//! This method is also called by glQuitRaptor.
	//! User can then avoid to delete objects manually in destructors. 
	//!	This method can alos be usefull to make a clean switch between several rendering contexts.
	static int glPurgeRaptor(bool count);

	//! Create a new display
	static CRaptorDisplay* glCreateDisplay(const CRaptorDisplayConfig &pcs);

    //! Create a window to use with CRaptorDisplays.
    //! This method is only a helper when no windowing advanced features are needed, 
    //! it only creates a simple window. 
    //! The parameter display is used to receive the display created with the window.
    //! If window creation failed, the handle is invalid.
    //! If display is not NULL, it is assumed to be a valid display to be bound to the window,
	//! otherwise a new display is created.
    static RAPTOR_HANDLE glCreateWindow(const CRaptorDisplayConfig &pcs,
										CRaptorDisplay *&pDisplay);

	//! Deletes a valid display.
	//! The Display must not be bound to a device. On return, the
	//! pointer is no longer valid.
	static void glDestroyDisplay(CRaptorDisplay* pDisplay);

    //! Check if the config can be supported.
    //! @return : true if config is supported, false otherwise
    static bool glCheckDisplayConfig(const CRaptorDisplayConfig &pcs);

	//!	Main Render entry point : all CRenderEntryPoints are rendered ( such as displays )
	static void glRender(void);

	//!	Return extensions state.
	static bool glIsExtensionSupported(const std::string &ext);
	static const CRaptorGLExtensions *const glGetExtensions(void);
	static bool vkIsExtensionSupported(const std::string &ext);
	static const CRaptorVKExtensions *const vkGetExtensions(void);
    
	//!	Access to RaptorConsole, if initialised at startup
	static CRaptorConsole *const GetConsole(void);

    //! Error management use internal messages.
    static CRaptorErrorManager * const GetErrorManager(void);
    static CRaptorMessages * const GetMessages(void);

private:
    Raptor();
	virtual ~Raptor();
};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)


