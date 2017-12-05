#include "StdAfx.h"
#include "Subsys/CodeGeneration.h"

#if !defined(AFX_CMDLINEPARSER_H__D7D8768A_3D97_491F_8493_588972A3CF62__INCLUDED_)
	#include "CmdLineParser.h"
#endif

CCmdLineParser::CCmdLineParser(void)
{
}

CCmdLineParser::~CCmdLineParser(void)
{
	for (unsigned int i=0;i<m_options.size();i++)
		delete m_options[i];
}

bool CCmdLineParser::parse(int argc, char *argv[])
{
	int i = 0;
	while (i+1 < argc)
	{
		const char *arg = argv[i];
		if (arg[0] == '-')
		{
			arg = arg + 1;

			if (arg[0] == '-')
			{
				arg = arg + 1;
				for (unsigned int o=0;o<m_options.size();o++)
				{
					if (m_options[o]->getName() == arg)
					{
						i++;
						m_options[o]->parse(argv[i]);
						break;
					}
				}
			}
			else for (unsigned int o=0;o<m_options.size();o++)
			{
				if (m_options[o]->getShort() == arg)
				{
					i++;
					m_options[o]->parse(argv[i]);
					break;
				}
			}
		}
		i++;
	}

	return true;
}

