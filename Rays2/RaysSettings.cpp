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
#if !defined(AFX_RAPTORIO_H__87D52C27_9117_4675_95DC_6AD2CCD2E78D__INCLUDED_)
	#include "System/RaptorIO.h"
#endif


using namespace Rays;

CRaysSettings::CRaysSettings(void)
{
	addSetting<uint16_t>("port", (uint16_t)2048);
	addSetting<string>("host_addr", std::string("127.0.0.1"));
}

CRaysSettings::~CRaysSettings(void)
{
}

bool CRaysSettings::importSettings(raptor::CRaptorIO *conf)
{
	size_t nb_deamon = 0;
	string value;
	string name;

	*conf >> name;
	string data = conf->getValueName();

	while (!data.empty())
	{
		if (data == "add")
		{
			value = "";
			name = "";
			*conf >> data;
		}
		else if ("key" == data)
			*conf >> name;
		else if ("value" == data)
			*conf >> value;
		else
			*conf >> data;

		if (!value.empty() && !name.empty())
		{
			m_settings.setValue(name, value.c_str());
			value = "";
			name = "";

			data = conf->getValueName();
			*conf >> data;
		}

		data = conf->getValueName();
	}

	return true;
}

bool CRaysSettings::exportSettings(raptor::CRaptorIO *conf)
{
	// TODO: implement
	return false;
}

bool CRaysSettings::setSettings(const CCmdLineParser& parser)
{
	uint16_t port;
	if (parser.getValue<uint16_t>("port", port))
		if (!setValue<uint16_t>("port", port))
			addSetting<uint16_t>("port", port);

	std::string host_addr;
	if (parser.getValue<std::string>("host_addr", host_addr))
		if (!setValue<std::string>("host_addr", host_addr))
			addSetting<std::string>("host_addr", host_addr);
	
	/*
	uint32_t deamon_delay;
	if (parser.getValue<uint32_t>("deamon_delay", deamon_delay))
		if (!setValue<uint32_t>("deamon_delay", deamon_delay))
			addSetting<uint32_t>("deamon_delay", deamon_delay);

	
	uint32_t wu_priority;
	if (parser.getValue<uint32_t>("wu_priority", wu_priority))
		if (!setValue<uint32_t>("wu_priority", wu_priority))
			addSetting<uint32_t>("wu_priority", wu_priority);

	addSetting<uint32_t>("deamon_delay", (uint32_t)10);
	addSetting<uint32_t>("nb_wu_per_job", (uint32_t)1);
	addSetting<vector<string>>("deamon", vector<string>());
	*/
	return true;
}

