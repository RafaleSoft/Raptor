// CCmdLineParser.h: interface for the CmdLineParser class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CMDLINEPARSER_H__D7D8768A_3D97_491F_8493_588972A3CF62__INCLUDED_)
#define AFX_CMDLINEPARSER_H__D7D8768A_3D97_491F_8493_588972A3CF62__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


RAPTOR_NAMESPACE


class RAPTOR_API CCmdLineParser
{
public:
	//! Defines a command line option
	class CCommandLineOption
	{
	public:
		CCommandLineOption(const std::string &name, const std::string &shortname)
			:m_name(name),m_short(shortname) {};
		virtual ~CCommandLineOption() {};

		virtual const std::string& getName(void) const
		{ return m_name; };

		virtual const std::string& getShort(void) const
		{ return m_short; };

		virtual bool parse(const char* argv)
		{ return false; };

	private:
		std::string	m_name;
		std::string	m_short;
	};
	
	template <class T>
	class CCommandLineOptionValue : public CCommandLineOption
	{
	public:
		CCommandLineOptionValue(const std::string &name,
								const std::string &shortname,
								T defaultValue)
			:CCommandLineOption(name,shortname),
			m_value(defaultValue) {};

		virtual ~CCommandLineOptionValue() {};

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
	CCmdLineParser(void);

	//!	Destructor.
	virtual ~CCmdLineParser(void);

	//!	Add an option to request parsing.
	//! @param name : the option name
	//! @param shortname : the option shortname (can be empty)
	//!	@param defaultValue : initial or default option value.
	//!	@return true if option successfully added, false if error (e.g. option already exists).
	template <class T>
	bool addOption(const std::string &name, const std::string &shortname, T defaultValue);

	//!	Parse the command line to retrive values as specified.
	//!	@param argc : argument count.
	//! @param argv : argument values.
	bool parse(int argc, char *argv[]);

	//!	Retrive an option value by name.
	template <class T>
	bool getValue(const std::string& optionName, T &t) const;

private:
	vector<CCommandLineOption*> m_options;
};


template <class T>
bool CCmdLineParser::addOption(const std::string &name,
							   const std::string &shortname,
								T defaultValue)
{
	bool exist = false;
	for (unsigned int i=0;!exist && i<m_options.size();i++)
		exist = (m_options[i]->getName() == name);

	if (exist)
		return false;
	else
	{
		CCommandLineOptionValue<T> *option = new CCommandLineOptionValue<T>(name,shortname,defaultValue);
		m_options.push_back(option);
		return true;
	}
}

template <class T>
bool CCmdLineParser::getValue(const std::string& optionName, T& t) const
{
	for (unsigned int o=0;o<m_options.size();o++)
	{
		CCommandLineOption* cmdline = m_options[o];
		if ((cmdline->getName() == optionName) ||
			(cmdline->getShort() == optionName))
		{
			//!	Need to avoid cast to check improper type.
			CCommandLineOptionValue<T>* option = (CCommandLineOptionValue<T>*)cmdline;
			t = option->getValue((T*)0);
			return true;
		}
	}

	return false;
}


#endif // !defined(AFX_CMDLINEPARSER_H__D7D8768A_3D97_491F_8493_588972A3CF62__INCLUDED_)