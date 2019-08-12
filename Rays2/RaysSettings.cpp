/***************************************************************************/
/*                                                                         */
/*  Raysettings.cpp                                                        */
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
	#include "RaysSettings.h"
#endif

using namespace Rays;

CRaysSettings::CRaysSettings(void)
{
}

CRaysSettings::~CRaysSettings(void)
{
	for (unsigned int i = 0; i<m_settings.size(); i++)
		delete m_settings[i];
}

bool CRaysSettings::setValue(const std::string& settingsName, const char* value)
{
	for (unsigned int o = 0; o<m_settings.size(); o++)
	{
		CSettingsOption* option = m_settings[o];
		if (option->getName() == settingsName)
			return option->parse(value);
	}

	return false;
}
