/***************************************************************************/
/*                                                                         */
/*  Raysettings.h                                                          */
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


#if !defined(AFX_RAYSSETTINGS_H__40662BB9_6FC8_40CA_A8A0_F2A701AD70BD__INCLUDED_)
#define AFX_RAYSSETTINGS_H__40662BB9_6FC8_40CA_A8A0_F2A701AD70BD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Subsys/CodeGeneration.h"

namespace raptor
{
	class CRaptorIO;
};

#if !defined(AFX_CMDLINEPARSER_H__D7D8768A_3D97_491F_8493_588972A3CF62__INCLUDED_)
	#include "ToolBox/CmdLineParser.h"
#endif

RAPTOR_NAMESPACE


namespace Rays
{
	class CRaysSettings
	{
	public:
		//!	Constructor.
		CRaysSettings(void);

		//!	Destructor.
		~CRaysSettings(void);

		//!	Add an option to request parsing.
		//! @param name : the option name
		//!	@param defaultValue : initial or default option value.
		//!	@return true if option successfully added, false if error (e.g. option already exists).
		template <class T>
		bool addSetting(const std::string &name, T defaultValue);

		//!	Set a settings value by name.
		bool setValue(const std::string& settingsName, const std::string& str_value);

		//!	Retrive a settings value by name.
		template <class T>
		bool setValue(const std::string& settingsName, T &t);

		//!	Retrive a settings value by name.
		template <class T>
		bool getValue(const std::string& settingsName, T &t) const;

		//!	Load Rays settings.
		bool importSettings(raptor::CRaptorIO *conf);

		//!	Load Rays startup configuration.
		bool importStartup(raptor::CRaptorIO *conf);

		//!	Save Rays settings.
		bool exportSettings(raptor::CRaptorIO *conf);

		//!	Update the server configuration data
		bool setSettings(const CCmdLineParser& parser);


	private:
		//! Usually not enough settings to justify a map.
		//!	Define a setting as a set of command line options to factorize handling.
		CCmdLineParser m_settings;
	};
}


template <class T>
bool Rays::CRaysSettings::addSetting(const std::string &name, T defaultValue)
{
	std::string shortname = name.substr(0,1);
	return m_settings.addOption<T>(name, shortname, defaultValue);
}

template <class T>
bool Rays::CRaysSettings::getValue(const std::string& settingsName, T& t) const
{
	return m_settings.getValue<T>(settingsName, t);
}

template <class T>
bool Rays::CRaysSettings::setValue(const std::string& settingsName, T& t)
{
	return m_settings.setValue(settingsName, t);
}

#endif // !defined(AFX_RAYSSETTINGS_H__40662BB9_6FC8_40CA_A8A0_F2A701AD70BD__INCLUDED_)