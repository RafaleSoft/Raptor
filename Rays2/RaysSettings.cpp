
#if !defined(AFX_RAYSSETTINGS_H__40662BB9_6FC8_40CA_A8A0_F2A701AD70BD__INCLUDED_)
	#include "RaysSettings.h"
#endif

CCRaysettings::CCRaysettings(void)
{
}

CCRaysettings::~CCRaysettings(void)
{
	for (unsigned int i = 0; i<m_settings.size(); i++)
		delete m_settings[i];
}

bool CCRaysettings::setValue(const string& settingsName, const char* value)
{
	for (unsigned int o = 0; o<m_settings.size(); o++)
	{
		CSettingsOption* option = m_settings[o];
		if (option->getName() == settingsName)
			return option->parse(value);
	}

	return false;
}
