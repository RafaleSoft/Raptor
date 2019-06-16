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


class Global
{
public:
	//!	The global status structure of Raptor.
	typedef struct RAPTOR_CURRENT_STATUS_t
	{
        //CRaptorDisplay			*defaultDisplay;
		//RAPTOR_HANDLE			defaultWindow;
		//long					defaultContext;
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


private:
	Global();
	virtual ~Global();

	//! One single instance of global is allowed
	static Global			*pInstance;

	RAPTOR_CURRENT_STATUS	raptorStatus;
    
};

RAPTOR_NAMESPACE_END

#endif

