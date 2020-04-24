// RaptorApplication.h: interface for the CRaptorApplication class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RAPTORAPPLICATION_H__E102DE21_30F2_4478_8C5A_55F780A0B33E__INCLUDED_)
#define AFX_RAPTORAPPLICATION_H__E102DE21_30F2_4478_8C5A_55F780A0B33E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Subsys/CodeGeneration.h"

#ifndef __CGLTYPES_HPP__
    #include "CGLTypes.h"
#endif


RAPTOR_NAMESPACE_BEGIN

class CRaptorDisplay;
class CRaptorDisplayConfig;


//!
//! This class implements basic application functionalities when no application
//! class exists in a context of Raptor usage;
//! Feel free to use it or to write your own application, this class is only a helper
//!
class RAPTOR_API CRaptorApplication
{
public:
	virtual ~CRaptorApplication();

    //! This method creates an instance of an application
    static CRaptorApplication  *CreateApplication(void);

    //! Returns the unique instance of application in the process
    static CRaptorApplication   *GetInstance(void);

	//! Set the root window of the application for event processing.
	//! This method is mandotory before Run can be called.
	void setRootWindow(const RAPTOR_HANDLE& root);

	//! Returns the application's root window.
	RAPTOR_HANDLE getRootWindow(void) const { return m_root; };

	//!	Returns the display of the root window.
	CRaptorDisplay* getRootDisplay(void) const { return m_pDisplay; };


	//!
	//!	Application execution.
	//!

    //! Initialises the application and then Raptor. Must be derived to add custom behaviour,
    //! don't forget to call back this method
	virtual bool initApplication(CRaptorDisplayConfig &config);

	//!	Run Host specific application, which manages Raptor Rendering.
    virtual bool run(void) = 0;

	//!	Return true is application is running.
	bool isRunning() const { return m_bRunning; };

	//! Release the application resources and then Raptor 
	//! (calling glQuitRaptor is managed automatically and is not necessary at user level).
	//! Must be derived to add custom behaviour, don't forget to call back this method.
	//! @return true if application resource are all released, false if nothing to do or in case of error.
	virtual bool quitApplication(void);
	
    //! Configure cursor handling, default is grabbed.
    virtual void grabCursor(bool grab) = 0;


protected:
    CRaptorApplication();

	//! Sets the unique instance of application in the process.
	//!	Helper for Raptor extensions, e.g. RaptorMFCApplication.
	static void SetInstance(CRaptorApplication *application);

    static CRaptorApplication* m_pInstance;

	//!	Application is in a running state.
	bool	m_bRunning;

	//!	The root window of the application.
    RAPTOR_HANDLE   m_root;

	//!	The display associated to the root window.
	CRaptorDisplay	*m_pDisplay;
};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_RAPTORAPPLICATION_H__E102DE21_30F2_4478_8C5A_55F780A0B33E__INCLUDED_)

