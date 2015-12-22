#include "stdafx.h"
#include <vcclr.h>
#include <windows.h>

#if !defined(AFX_RAYSSERVERUTILS_H__1CC878E3_B301_4A19_8211_F3B5977D3781__INCLUDED_)
	#include "RaysServerUtils.h"
#endif


using namespace RaysServer;
using namespace System::Configuration;

static RaysServerUtils::RaysServerUtils()
{
	rays_config = gcnew RaysServerUtils::RAYS_CONFIG;
	m_logger = nullptr;
}

RaysServerUtils::ILogger^ RaysServerUtils::getLog()
{
	return m_logger;
}

RaysServerUtils::ILogger^ RaysServerUtils::setLog(ILogger^ logger)
{
	RaysServerUtils::ILogger^ l = m_logger;
	m_logger = logger;
	return l;
}

RaysServerUtils::RAYS_CONFIG::RAYS_CONFIG()
{
	host = gcnew System::String("127.0.0.1");
	port = 2048;
	wu_priority = 1;
	deamon_delay = 10;
	nb_wu_per_job = 1;
	deamons = gcnew System::Collections::ArrayList;
}

char* RaysServerUtils::convertSystemString(System::String^ str)
{
	pin_ptr<const wchar_t> wch = PtrToStringChars(str);

	size_t convertedChars = 0;
	size_t  sizeInBytes = ((str->Length + 1) * 2);
	char    *ch = (char *)malloc(sizeInBytes);

	wcstombs_s(&convertedChars, ch, sizeInBytes, wch, sizeInBytes);

	return ch;
}

bool RaysServerUtils::loadConfig(void)
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
		item = settings->Settings["wu_priority"];
		if (nullptr != item)
			rays_config->wu_priority = System::Int32::Parse(item->Value);
		item = settings->Settings["deamon_delay"];
		if (nullptr != item)
			rays_config->deamon_delay = System::Int32::Parse(item->Value);
		item = settings->Settings["nb_wu_per_job"];
		if (nullptr != item)
			rays_config->nb_wu_per_job = System::Int32::Parse(item->Value);

		return true;
	}
	else
		return false;
}


bool RaysServerUtils::saveConfig(void)
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

		item = settings->Settings["wu_priority"];
		if (nullptr != item)
			item->Value = rays_config->wu_priority.ToString();
		else
			settings->Settings->Add("wu_priority",rays_config->wu_priority.ToString());

		item = settings->Settings["deamon_delay"];
		if (nullptr != item)
			item->Value = rays_config->deamon_delay.ToString();
		else
			settings->Settings->Add("deamon_delay",rays_config->deamon_delay.ToString());

		item = settings->Settings["nb_wu_per_job"];
		if (nullptr != item)
			item->Value = rays_config->nb_wu_per_job.ToString();
		else
			settings->Settings->Add("nb_wu_per_job",rays_config->nb_wu_per_job.ToString());

		conf->Save(ConfigurationSaveMode::Modified);
		return true;
	}

	return false;
}

RaysServerUtils::RAYS_CONFIG^ RaysServerUtils::getConfig(void)
{
	return rays_config;
}
