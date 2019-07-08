// CCmdLineParser.h: interface for the CmdLineParser class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RAYSSETTINGS_H__40662BB9_6FC8_40CA_A8A0_F2A701AD70BD__INCLUDED_)
#define AFX_RAYSSETTINGS_H__40662BB9_6FC8_40CA_A8A0_F2A701AD70BD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include <vector>
#include <stdint.h>
using std::string;
using std::vector;


class CCRaysettings
{
public:
	//! Defines a command line option
	class CSettingsOption
	{
	public:
		CSettingsOption(const string &name):m_name(name) {};
		virtual ~CSettingsOption() {};

		virtual const string& getName(void) const
		{ return m_name; };

		virtual bool parse(const char* argv)
		{ return false; };

	private:
		string	m_name;
	};
	
	template <class T>
	class CSettingsOptionValue : public CSettingsOption
	{
	public:
		CSettingsOptionValue(const string &name,T defaultValue)
			:CSettingsOption(name),
			m_value(defaultValue) {};

		virtual ~CSettingsOptionValue() {};

		T getValue(T* t) const
		{ return m_value; };

		void setValue(T t)
		{ m_value = t; };

		virtual bool parse(const char* argv)
		{ return false; };

	private:
		T	m_value;
	};


	//!	Constructor.
	CCRaysettings(void);

	//!	Destructor.
	~CCRaysettings(void);

	//!	Add an option to request parsing.
	//! @param name : the option name
	//!	@param defaultValue : initial or default option value.
	//!	@return true if option successfully added, false if error (e.g. option already exists).
	template <class T>
	bool addSetting(const string &name,T defaultValue);

	//!	Retrive a settings value by name.
	bool setValue(const string& settingsName, const char* value);

	//!	Retrive a settings value by name.
	template <class T>
	bool getValue(const string& settingsName,T &t) const;


private:
	//! Usually not enough settings to justify a map.
	vector<CSettingsOption*> m_settings;
};


template <class T>
bool CCRaysettings::addSetting(const string &name, T defaultValue)
{
	bool exist = false;
	for (unsigned int i = 0; !exist && i<m_settings.size(); i++)
		exist = (m_settings[i]->getName() == name);

	if (exist)
		return false;
	else
	{
		CSettingsOptionValue<T> *setting = new CSettingsOptionValue<T>(name, defaultValue);
		m_settings.push_back(setting);
		return true;
	}
}

template <class T>
bool CCRaysettings::getValue(const string& settingsName, T& t) const
{
	for (unsigned int o = 0; o<m_settings.size(); o++)
	{
		CSettingsOption* option = m_settings[o];
		if (option->getName() == settingsName)
		{
			//!	Need to avoid cast to check improper type.
			CSettingsOptionValue<T>* setting = (CSettingsOptionValue<T>*)option;
			t = setting->getValue((T*)0);
			return true;
		}
	}

	return false;
}

template <>
CCRaysettings::CSettingsOptionValue<const char*>::CSettingsOptionValue(	const string &name,
																		const char* defaultValue)
	:CSettingsOption(name), m_value(NULL)
{
	char *option = (char*)m_value;
	option = _strdup(defaultValue);
}

template <>
CCRaysettings::CSettingsOptionValue<const char*>::~CSettingsOptionValue()
{
	if (m_value != NULL)
		free((void*)m_value);
}

template <>
bool CCRaysettings::CSettingsOptionValue<uint32_t>::parse(const char* argv)
{
	m_value = atoi(argv);
	return true;
}

template <>
bool CCRaysettings::CSettingsOptionValue<uint16_t>::parse(const char* argv)
{
	m_value = (unsigned short)(0xffff & atoi(argv));
	return true;
}

template <>
bool CCRaysettings::CSettingsOptionValue<const char*>::parse(const char* argv)
{
	if (m_value != NULL)
		free((void*)m_value);

	m_value = _strdup(argv);

	return true;
}


template <>
bool CCRaysettings::CSettingsOptionValue<vector<uint32_t>>::parse(const char* argv)
{
	m_value.push_back((unsigned int)(0xffff & atoi(argv)));
	return true;
}

template <>
bool CCRaysettings::CSettingsOptionValue<vector<string>>::parse(const char* argv)
{
	if (argv != NULL)
		m_value.push_back(string(argv));

	return true;
}

#endif // !defined(AFX_RAYSSETTINGS_H__40662BB9_6FC8_40CA_A8A0_F2A701AD70BD__INCLUDED_)