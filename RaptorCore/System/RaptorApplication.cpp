// RaptorApplication.cpp: implementation of the CRaptorApplication class.
//
//////////////////////////////////////////////////////////////////////

#include "Subsys/CodeGeneration.h"

#if !defined(AFX_RAPTORCONFIG_H__29B753B8_17DE_44DF_A4D2_9D19C5AC53D5__INCLUDED_)
    #include "RaptorConfig.h"
#endif

#if !defined(AFX_RAPTORAPPLICATION_H__E102DE21_30F2_4478_8C5A_55F780A0B33E__INCLUDED_)
    #include "RaptorApplication.h"
#endif
#if defined(WIN32)
    #if !defined(AFX_WIN32APPLICATION_H__3EADD210_ABF5_4CFD_A511_09047EDBB881__INCLUDED_)
        #include "Win32Specific/Win32Application.h"
    #endif
#elif defined(LINUX)
    #if !defined(AFX_GLXAPPLICATION_H__7384D172_A179_4048_BC2A_6D2C628442E5__INCLUDED_)
        #include "GLXSpecific/GLXApplication.h"
    #endif
#endif

#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
	#include "System/Raptor.h"
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
RAPTOR_NAMESPACE_BEGIN

CRaptorApplication* CRaptorApplication::m_pInstance = NULL;

RAPTOR_NAMESPACE_END


RAPTOR_NAMESPACE

CRaptorApplication::CRaptorApplication()
{

}

CRaptorApplication::~CRaptorApplication()
{
    m_pInstance = NULL;
}

CRaptorApplication   *CRaptorApplication ::GetInstance(void)
{ 
    return m_pInstance; 
}

CRaptorApplication  *CRaptorApplication ::CreateApplication(void)
{
    if (m_pInstance != NULL)
        return m_pInstance;

#if defined(WIN32)
    m_pInstance = new CWin32Application();
#elif defined(LINUX)
    m_pInstance = new CGLXApplication();
#endif

    return m_pInstance;
}

bool CRaptorApplication::initApplication(void)
{
    CRaptorConfig config;
    return Raptor::glInitRaptor(config);
}

