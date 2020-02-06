/***************************************************************************/
/*                                                                         */
/*  RaysUtils.cpp                                                          */
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

#if !defined(AFX_RAYSUTILS_H__1CC878E3_B301_4A19_8211_F3B5977D3781__INCLUDED_)
	#include "RaysUtils.h"
#endif
#if !defined(AFX_NETWORKLOGGER_H__04F6649D_5560_45A7_8ED5_B5FC9354256C__INCLUDED_)
	#include "RaptorNetwork/NetworkLogger.h"
#endif
#if !defined(AFX_NETWORK_H__AC9D546D_A00A_4BFC_AC0C_288BE137CD20__INCLUDED_)
	#include "RaptorNetwork/Network.h"
#endif
#if !defined(AFX_RAPTORIO_H__87D52C27_9117_4675_95DC_6AD2CCD2E78D__INCLUDED_)
	#include "System/RaptorIO.h"
#endif



RAPTOR_NAMESPACE
using namespace Rays;

//!	Static utils
RaysUtils *RaysUtils::s_pUtils = NULL;


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

		RaysUtils::ILogger& logger = RaysUtils::getLog();
		logger.Log(log);

		std::cout << log << std::endl;
	}
};

RaysUtils::RaysUtils()
{
	INetworkLogger *logger = new LoggerWrapper();
	Network::addLogger(logger);
	m_pLogger = NULL;

	m_settings.addSetting<uint32_t>("wu_priority", (uint32_t)1);
	m_settings.addSetting<uint32_t>("deamon_delay", (uint32_t)10);
	m_settings.addSetting<uint32_t>("nb_wu_per_job", (uint32_t)1);
	m_settings.addSetting<vector<string>>("deamon", vector<string>());
}

RaysUtils::~RaysUtils()
{
}

RaysUtils& RaysUtils::getUtils()
{
	if (NULL == s_pUtils)
		s_pUtils = new RaysUtils();

	return *s_pUtils;
}

RaysUtils::ILogger& RaysUtils::getLog()
{
	return *(s_pUtils->m_pLogger);
}

RaysUtils::ILogger* RaysUtils::setLog(ILogger* pLogger)
{
	RaysUtils::ILogger* l = getUtils().m_pLogger;
	getUtils().m_pLogger = pLogger;
	return l;
}


bool RaysUtils::loadConfig(const std::string &config_file)
{
	CRaptorIO *conf = CRaptorIO::Create(config_file,
										CRaptorIO::IO_KIND::DISK_READ,
										CRaptorIO::ASCII_XML);

	if (CRaptorIO::IO_OK == conf->getStatus())
	{
		conf->parse(config_file.c_str(), 0);

		string name;
		*conf >> name;
		string data = conf->getValueName();
		if ("configuration" != data)
		{
			getLog().Log("Invalid Rays Server configuration file.");
			return false;
		}

		//  skip data intro
		*conf >> name;
		data = conf->getValueName();

		while (!data.empty())
		{
			if ("appSettings" == data)
				getUtils().getSettings().importSettings(conf);
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


bool RaysUtils::saveConfig(void)
{
	CRaptorIO *conf = CRaptorIO::Create("RaysServer.config",
										CRaptorIO::IO_KIND::DISK_WRITE,
										CRaptorIO::ASCII_XML);

	if (CRaptorIO::IO_OK == conf->getStatus())
	{
		getUtils().getSettings().exportSettings(conf);

		return true;
	}
	else
		return false;
}

CRaysSettings& RaysUtils::getSettings()
{
	return getUtils().m_settings;
}