/***************************************************************************/
/*                                                                         */
/*  RaysUtils.h                                                            */
/*                                                                         */
/*    Raptor OpenGL & Vulkan realtime 3D Engine SDK.                       */
/*                                                                         */
/*  Copyright 1998-2021 by                                                 */
/*  Fabrice FERRAND.                                                       */
/*                                                                         */
/*  This file is part of the Raptor project, and may only be used,         */
/*  modified, and distributed under the terms of the Raptor project        */
/*  license, LICENSE.  By continuing to use, modify, or distribute         */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/


#if !defined(AFX_RAYSUTILS_H__1CC878E3_B301_4A19_8211_F3B5977D3781__INCLUDED_)
#define AFX_RAYSUTILS_H__1CC878E3_B301_4A19_8211_F3B5977D3781__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "Subsys/CodeGeneration.h"

#if !defined(AFX_RAYSSETTINGS_H__40662BB9_6FC8_40CA_A8A0_F2A701AD70BD__INCLUDED_)
	#include "RaysSettings.h"
#endif


namespace Rays
{
	class RaysUtils
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
		static bool loadConfig(const std::string &config_file);

		//!	Save application config file
		static bool saveConfig(void);

		//!	Returns the application Settings.
		static CRaysSettings& getSettings(void);

		//!	Returns the current logger.
		static ILogger& getLog();

		//!	Defines the current logger.
		//!	Returns the previous logger if any.
		static ILogger* setLog(ILogger* pLogger);


	private:
		RaysUtils();
		RaysUtils(const RaysUtils&);
		~RaysUtils();
		RaysUtils& operator=(const RaysUtils&);

		//!	Initialise instance if necessary.
		static RaysUtils& getUtils();

		//!	The single instance.
		static RaysUtils *s_pUtils;

		//!	A unique logger for simplicity
		ILogger *m_pLogger;

		//! A global set of application settings.
		CRaysSettings m_settings;
	};
}

#endif // !defined(AFX_RAYSUTILS_H__1CC878E3_B301_4A19_8211_F3B5977D3781__INCLUDED_)
