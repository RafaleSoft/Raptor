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

    //! Initialises the application and then Raptor. Must be derived to add custom behaviour,
    //! don't forget to call back this method
    virtual bool initApplication(void);

    virtual bool run(void) = 0;

    //! Set the root window of the application for event processing.
    //! This method is mandotory before Run can be called.
    virtual void setRootWindow(const RAPTOR_HANDLE& root) { m_root = root; };

    //! Returns the application's root window.
    virtual RAPTOR_HANDLE getRootWindow(void) const { return m_root; };

    //! Configure cursor handling, default is grabbed.
    virtual void grabCursor(bool grab) = 0;


protected:
    CRaptorApplication();

    static CRaptorApplication* m_pInstance;

    RAPTOR_HANDLE   m_root;
};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_RAPTORAPPLICATION_H__E102DE21_30F2_4478_8C5A_55F780A0B33E__INCLUDED_)

