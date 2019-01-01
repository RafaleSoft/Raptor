#include "stdafx.h"
#include <vcclr.h>
#include <windows.h>

#if !defined(AFX_RAYSCLIENTUTILS_H__D5EDE275_AF42_4BF5_80F7_2C408934AEFF__INCLUDED_)
	#include "RaysClientUtils.h"
#endif
#if !defined(AFX_NETWORKLOGGER_H__04F6649D_5560_45A7_8ED5_B5FC9354256C__INCLUDED_)
	#include "RaptorNetwork/NetworkLogger.h"
#endif
#if !defined(AFX_NETWORK_H__AC9D546D_A00A_4BFC_AC0C_288BE137CD20__INCLUDED_)
	#include "RaptorNetwork/Network.h"
#endif


using namespace RaysClient;
using namespace System::Configuration;


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

		System::String^ console_msg = gcnew System::String(log.c_str());

		RaysClientUtils::ILogger^ logger = RaysClientUtils::getLog();
		if (nullptr != logger)
			logger->Log(console_msg);

		System::Console::WriteLine(console_msg);
	}
};

static RaysClientUtils::RaysClientUtils()
{
	INetworkLogger *logger = new LoggerWrapper();
	Network::addLogger(logger);
	rays_config = gcnew RaysClientUtils::RAYS_CONFIG;
	m_logger = nullptr;
}

RaysClientUtils::ILogger^ RaysClientUtils::getLog()
{
	return m_logger;
}

RaysClientUtils::ILogger^ RaysClientUtils::setLog(ILogger^ logger)
{
	RaysClientUtils::ILogger^ l = m_logger;
	m_logger = logger;
	return l;
}

RaysClientUtils::RAYS_CONFIG::RAYS_CONFIG()
{
	host = gcnew System::String("127.0.0.1");
	port = 2048;
}

char* RaysClientUtils::convertSystemString(System::String^ str)
{
	pin_ptr<const wchar_t> wch = PtrToStringChars(str);

	size_t convertedChars = 0;
	size_t  sizeInBytes = ((str->Length + 1) * 2);
	char    *ch = (char *)malloc(sizeInBytes);

	wcstombs_s(&convertedChars, ch, sizeInBytes, wch, sizeInBytes);

	return ch;
}

bool RaysClientUtils::loadConfig(void)
{
	Configuration^ conf = ConfigurationManager::OpenExeConfiguration(System::Configuration::ConfigurationUserLevel::None);
	if (conf->HasFile)
	{
		AppSettingsSection^ settings = conf->AppSettings;
		KeyValueConfigurationElement^ item = settings->Settings["port"];
		if (nullptr != item)
			rays_config->port = System::Int32::Parse(item->Value);
		item = settings->Settings["host"];
		if (nullptr != item)
			rays_config->host = item->Value;

		return true;
	}
	else
		return false;
}


bool RaysClientUtils::saveConfig(void)
{
	Configuration^ conf = ConfigurationManager::OpenExeConfiguration(ConfigurationUserLevel::None);
	if (conf->HasFile)
	{
		AppSettingsSection^ settings = conf->AppSettings;
		KeyValueConfigurationElement^ item = settings->Settings["port"];
		if (nullptr != item)
			item->Value = rays_config->port.ToString();
		else
			settings->Settings->Add("port",rays_config->port.ToString());

		item = settings->Settings["host"];
		if (nullptr != item)
			item->Value = rays_config->host;
		else
			settings->Settings->Add("host",rays_config->host);

		conf->Save(ConfigurationSaveMode::Modified);
		return true;
	}

	return false;
}

RaysClientUtils::RAYS_CONFIG^ RaysClientUtils::getConfig(void)
{
	return rays_config;
}
