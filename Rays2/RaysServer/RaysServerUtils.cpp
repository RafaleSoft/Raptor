/***************************************************************************/
/*                                                                         */
/*  RaysServerUtils.cpp                                                    */
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


#if defined(WIN32)
	#define VC_EXTRALEAN
	#include <Windows.h>
#endif

#if !defined(AFX_RAYSSERVERUTILS_H__1CC878E3_B301_4A19_8211_F3B5977D3781__INCLUDED_)
	#include "RaysServerUtils.h"
#endif
#if !defined(AFX_NETWORKLOGGER_H__04F6649D_5560_45A7_8ED5_B5FC9354256C__INCLUDED_)
	#include "RaptorNetwork/NetworkLogger.h"
#endif
#if !defined(AFX_NETWORK_H__AC9D546D_A00A_4BFC_AC0C_288BE137CD20__INCLUDED_)
	#include "RaptorNetwork/Network.h"
#endif
#if !defined(AFX_RAPTORERRORMANAGER_H__FA5A36CD_56BC_4AA1_A5F4_451734AD395E__INCLUDED_)
	#include "System/RaptorErrorManager.h"
#endif
#if !defined(AFX_RAPTORIO_H__87D52C27_9117_4675_95DC_6AD2CCD2E78D__INCLUDED_)
	#include "System/RaptorIO.h"
#endif
#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
	#include "System/Raptor.h"
#endif
#if !defined(AFX_RAYSSETTINGS_H__40662BB9_6FC8_40CA_A8A0_F2A701AD70BD__INCLUDED_)
	#include "RaysSettings.h"
#endif


RAPTOR_NAMESPACE
using namespace RaysServer;

//!	Static utils
RaysServerUtils *RaysServerUtils::s_pUtils = NULL;


class LoggerWrapper : public INetworkLogger
{
public:
	LoggerWrapper() {};
	virtual ~LoggerWrapper() {};

	virtual void Log(INetworkLogger::SEVERITY level, const std::string& msg) const
	{
		std::string log;
		switch (level)
		{
		case INetworkLogger::NETWORK_DEBUG:
			log += "DEBUG ";
			break;
		case INetworkLogger::NETWORK_INFO:
			log += "INFO ";
			break;
		case INetworkLogger::NETWORK_WARNING:
			log += "WARNING ";
			break;
		case INetworkLogger::NETWORK_ERROR:
			log += "ERROR ";
			break;
		case INetworkLogger::NETWORK_FATAL:
			log += "FATAL ";
			break;
		}
		log += msg;

		RaysServerUtils::ILogger& logger = RaysServerUtils::getLog();
		logger.Log(log);

		std::cout << log << std::endl;
	}
};

RaysServerUtils::RaysServerUtils()
{
	INetworkLogger *logger = new LoggerWrapper();
	Network::addLogger(logger);
	m_pLogger = NULL;

	m_settings.addSetting<uint32_t>("port", (uint32_t)2048);
	m_settings.addSetting<uint32_t>("wu_priority", (uint32_t)1);
	m_settings.addSetting<uint32_t>("deamon_delay", (uint32_t)10);
	m_settings.addSetting<uint32_t>("nb_wu_per_job", (uint32_t)1);
	m_settings.addSetting<string>("host", string("127.0.0.1"));
}

RaysServerUtils::~RaysServerUtils()
{
}

RaysServerUtils& RaysServerUtils::getUtils()
{
	if (NULL == s_pUtils)
		s_pUtils = new RaysServerUtils();

	return *s_pUtils;
}

RaysServerUtils::ILogger& RaysServerUtils::getLog()
{
	return *(s_pUtils->m_pLogger);
}

RaysServerUtils::ILogger* RaysServerUtils::setLog(ILogger* pLogger)
{
	RaysServerUtils::ILogger* l = getUtils().m_pLogger;
	getUtils().m_pLogger = pLogger;
	return l;
}


bool RaysServerUtils::importSettings(raptor::CRaptorIO *conf)
{
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
		else if ("wu_priority" == data)
			*conf >> value;
		else if ("deamon_delay" == data)
			*conf >> value;
		else if ("nb_wu_per_job" == data)
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

bool RaysServerUtils::loadConfig(void)
{
	CRaptorIO *conf = CRaptorIO::Create("RaysServer.config",
										CRaptorIO::IO_KIND::DISK_READ,
										CRaptorIO::ASCII_XML);

	if (CRaptorIO::IO_OK == conf->getStatus())
	{
		conf->parse("RaysServer.config", 0);

		string name;
		*conf >> name;
		string data = conf->getValueName();
		if ("configuration" != data)
		{
			RAPTOR_ERROR(CPersistence::CPersistenceClassID::GetClassId(), 
						 "Invalid Rays Server configuration file.");
			return false;
		}

		//  skip data intro
		*conf >> name;
		data = conf->getValueName();

		while (!data.empty())
		{
			if ("appSettings" == data)
				getUtils().importSettings(conf);
			else
				*conf >> name;

			data = conf->getValueName();
		}

		bool res = (CRaptorIO::IO_OK == conf->getStatus());
		delete conf;
		return res;
	}
	else
		return false;
}


bool RaysServerUtils::saveConfig(void)
{
	CRaptorIO *conf = CRaptorIO::Create("RaysServer.config",
										CRaptorIO::IO_KIND::DISK_WRITE,
										CRaptorIO::ASCII_XML);

	const CRaysettings &settings = RaysServerUtils::getSettings();
	if (CRaptorIO::IO_OK == conf->getStatus())
	{
		getUtils().importSettings(conf);

		return true;
	}
	else
		return false;
}

const CRaysettings& RaysServerUtils::getSettings(void)
{
	return getUtils().m_settings;
}