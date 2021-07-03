/***************************************************************************/
/*                                                                         */
/*  Config.cpp                                                             */
/*                                                                         */
/*    Raptor OpenGL & Vulkan realtime 3D Engine SDK.                       */
/*                                                                         */
/*  Copyright 1998-2021 by                                                 */
/*  Fabrice FERRAND.                                                       */
/*                                                                         */
/*  This file is part of the Raptor project, and may only be used,         */
/*  modified, and distributed under the terms of the Raptor project        */
/*  license, LICENSE.  By continuing to use, modify, or distribute         */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/



#include "StdAfx.h"
#include "MicroLex.h"

#include <string.h>	// strcmp


MicroLexConfig* MicroLexConfig::m_pConfig = NULL;

MicroLexConfig::MicroLexConfig()
{
	verbose = false;
	lexclass = "CLex";
	yaccclass = "CYacc";
	renameClasses = false;
	output = "lexyacc";
	input = "";
}

MicroLexConfig* MicroLexConfig::GetConfig(void)
{
	if (m_pConfig == NULL)
	{
		m_pConfig = new MicroLexConfig();
	}

	return m_pConfig;
}

void MicroLexConfig::Usage(void)
{
	cout << "Usage : MicroLex <-file filename> <-out outfile> [-mfc] [-verbose] [-lexclass <class name>] [-yaccclass <class name>]" << endl;
	cout << "        <filename> is the micro lex file to parse" << endl;
	cout <<	"        <outfile> is the name of the .h and .cpp " << endl;
	cout << "            files generated to compile with your application" << endl;
	cout << endl;
	cout << "        -file|-f specifies the lex/yacc file to parse (language definition)" << endl;
	cout << endl;
	cout << "        -out|-o specifies the base file names for the generated files" << endl;
	cout << endl;
	cout << "        -mfc|-m switch prepares files to be included in an MFC application, even a console Win32App might need this flag" << endl;
	cout << endl;
	cout << "        -verbose|-v switch displays detailed parsing steps" << endl;
	cout << endl;
	cout << "        -lexclass|-l specifies the lex class to be <class name>" << endl;
	cout << endl;
	cout << "        -yaccclass|-y specifies the yacc class to be <class name>" << endl;
	cout << endl;
}


int MicroLexConfig::InitConfig(int argc,char* argv[])
{
	if (argc < 2)
		return -1;

	for (int i=1;i<argc;i++)
	{
		if ((!strcmp(argv[i],"-verbose")) || (!strcmp(argv[i],"-v")))
			verbose = true;
		else if ((!strcmp(argv[i],"-lexclass")) || (!strcmp(argv[i],"-l")))
		{
			i++;
			if (i == argc)
			{
				cout << "Error : missing class name argument for -lexclass specifier" << endl;
				return -1;
			}
			else
			{
				lexclass = argv[i];
				renameClasses = true;
			}
		}
		else if ((!strcmp(argv[i],"-yaccclass")) || (!strcmp(argv[i],"-y")))
		{
			i++;
			if (i == argc)
			{
				cout << "Error : missing class name argument for -yaccclass specifier" << endl;
				return -1;
			}
			else
			{
				yaccclass = argv[i];
				renameClasses = true;
			}
		}
		else if ((!strcmp(argv[i],"-out")) || (!strcmp(argv[i],"-o")))
		{
			i++;
			if (i == argc)
			{
				cout << "Error : missing class name argument for -out specifier" << endl;
				return -1;
			}
			else
			{
				output = argv[i];
			}
		}
		else if ((!strcmp(argv[i],"-file")) || (!strcmp(argv[i],"-f")))
		{
			i++;
			if (i == argc)
			{
				cout << "Error : missing class name argument for -out specifier" << endl;
				return -1;
			}
			else
			{
				input = argv[i];
			}
		}
		else
			cout << "Unknown parameter " << argv[i] << endl;
	}

	return 1;
}

