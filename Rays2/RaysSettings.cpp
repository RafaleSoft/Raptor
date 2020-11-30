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
	addSetting<uint16_t>("deamon_port", (uint16_t)2049);
	addSetting<string>("host_addr", std::string("127.0.0.1"));
	addSetting<uint32_t>("nb_wu_per_job", (uint32_t)1);
}

CRaysSettings::~CRaysSettings(void)
{
}

bool CRaysSettings::importSettings(raptor::CRaptorIO *conf)
{
	size_t nb_deamon = 0;
	std::string value;
	std::string v_data;

	*conf >> v_data;
	std::string v_name = conf->getValueName();

	while (conf->hasMoreValues())
	{
		if (v_name == "add")
		{
			value = "";
			v_data = "";
			*conf >> v_name;
		}
		else if ("key" == v_name)
			*conf >> v_data;
		else if ("value" == v_name)
			*conf >> value;
		else
			*conf >> v_name;

		if (!value.empty() && !v_data.empty())
		{
			std::cout << "read settings value: " << v_data << " = " << value << std::endl;
			m_settings.setValue(v_data, value.c_str());
			value = "";
			v_data = "";

			v_name = conf->getValueName();
			*conf >> v_name;
		}

		v_name = conf->getValueName();
	}

	return true;
}

bool CRaysSettings::importStartup(raptor::CRaptorIO *conf)
{
	size_t nb_deamon = 0;
	std::string value;
	std::string v_data;

	*conf >> v_data;
	std::string v_name = conf->getValueName();

	while (conf->hasMoreValues())
	{
		if (v_name == "supportedRuntime")
		{
			while (conf->hasMoreValues())
			{
				*conf >> v_data;
				v_name = conf->getValueName();
			}
			*conf >> v_data;
		}
		else
			*conf >> v_data;

		v_name = conf->getValueName();
	}

	*conf >> v_data;
	v_name = conf->getValueName();

	return true;
}

bool CRaysSettings::exportSettings(raptor::CRaptorIO *conf)
{
	// TODO: implement
	return false;
}

bool CRaysSettings::setSettings(const CCmdLineParser& parser)
{
	uint16_t port = 0;
	if (parser.getValue<uint16_t>("port", port))
	{
		//std::cout << "set port settings from command line:" << port << std::endl;
		if (!setValue<uint16_t>("port", port))
			addSetting<uint16_t>("port", port);
	}

	uint16_t deamon_port = 0;
	if (parser.getValue<uint16_t>("deamon_port", deamon_port))
	{
		//std::cout << "set port settings from command line:" << port << std::endl;
		if (!setValue<uint16_t>("deamon_port", deamon_port))
			addSetting<uint16_t>("deamon_port", deamon_port);
	}

	std::string host_addr = "";
	if (parser.getValue<std::string>("host_addr", host_addr))
	{
		//std::cout << "set host settings from command line:" << host_addr << std::endl;
		if (!setValue<std::string>("host_addr", host_addr))
			addSetting<std::string>("host_addr", host_addr);
	}
	
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
	
	addSetting<vector<string>>("deamon", vector<string>());
	*/
	return true;
}


bool CRaysSettings::setValue(const std::string& settingsName, const std::string& str_value)
{
	return m_settings.setValue(settingsName, str_value.c_str());
}

