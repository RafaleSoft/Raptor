/***************************************************************************/
/*                                                                         */
/*  RaysServerUtils.h                                                      */
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


#if !defined(AFX_RAYSSERVERUTILS_H__1CC878E3_B301_4A19_8211_F3B5977D3781__INCLUDED_)
#define AFX_RAYSSERVERUTILS_H__1CC878E3_B301_4A19_8211_F3B5977D3781__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Subsys/CodeGeneration.h"
namespace raptor
{
	class CRaptorIO;
};

#if !defined(AFX_RAYSSETTINGS_H__40662BB9_6FC8_40CA_A8A0_F2A701AD70BD__INCLUDED_)
	#include "../RaysSettings.h"
#endif


namespace RaysServer
{
	class CDeamonManager;

	class RaysServerUtils
	{
	public:
		class ILogger
		{
		public:
			virtual void Log(const std::string& msg) = 0;
		protected:
			ILogger() {};
			virtual ~ILogger() {};
		};

		//!	Load application config file
		static bool loadConfig(void);

		//!	Save application config file
		static bool saveConfig(void);

		//!	Returns the server configuration structure (from file)
		//static RaysServerUtils::RAYS_CONFIG& getConfig(void);

		//!	Returns the application Settings.
		static const CRaysettings& getSettings(void);

		//!	Returns the current logger.
		static ILogger& getLog();

		//!	Defines the current logger.
		//!	Returns the previous logger if any.
		static ILogger* setLog(ILogger* pLogger);


	private:
		RaysServerUtils();
		RaysServerUtils(const RaysServerUtils&);
		~RaysServerUtils();
		RaysServerUtils& operator=(const RaysServerUtils&);

		//!	Initialise instance if necessary.
		static RaysServerUtils& getUtils();

		//!	Load server settings section.
		bool importSettings(raptor::CRaptorIO *conf);

		static RaysServerUtils *s_pUtils;

		//!	A unique logger for simplicity
		ILogger *m_pLogger;

		//! A global set of application settings.
		CRaysettings m_settings;
	};
}

#endif // !defined(AFX_RAYSSERVERUTILS_H__1CC878E3_B301_4A19_8211_F3B5977D3781__INCLUDED_)
