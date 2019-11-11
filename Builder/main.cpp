/***************************************************************************/
/*                                                                         */
/*  main.cpp                                                               */
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


#include <iostream>
#include <unistd.h>	// sleep
#include <string.h>
#include "BuilderNative.h"

#include <vector>

void print_help(void)
{
	std::cout << "Raptor Builder copyright 2019." << std::endl;
	std::cout << "Usage:" << std::endl;
	std::cout << "Builder [-v|--verbose] [-e|--enable <extension name>] [-d|--disable <extension name>]" << std::endl;
	std::cout << "  -v, --verbose: print extended information messages" << std::endl;
	std::cout << "  -e, --enable: activate an OpenGL or Vulkan extension given by name" << std::endl;
	std::cout << "  -d, --disable: deactivate an OpenGL or Vulkan extension given by name" << std::endl;
	std::cout << "Exiting." << std::endl;
}

int main(int argc, char *argv[])
{
	std::vector<const char*> active;
	std::vector<const char*> deactive;

	bool verbose = false;
	bool help = false;
	for (int i = 1; i<argc; i++)
	{
		const char* param = argv[i];
		if (NULL == param)
			continue;
		else if (!strcmp("-v", param))
			verbose = true;
		else if (!strcmp("--verbose", param))
			verbose = true;
		else if (!strcmp("-h", param))
			help = true;
		else if (!strcmp("--help", param))
			help = true;
		else if (!strcmp("-e", param) && (i + 1 < argc))
			active.push_back(argv[i + 1]);
		else if (!strcmp("--enable", param) && (i + 1 < argc))
			active.push_back(argv[i + 1]);
		else if (!strcmp("-d", param) && (i + 1 < argc))
			deactive.push_back(argv[i + 1]);
		else if (!strcmp("--disable", param) && (i + 1 < argc))
			deactive.push_back(argv[i + 1]);
	}

	if (help)
	{
		print_help();
		return(0);
	}

	if (!glvkInitBuilder(NULL))
	{
		std::cout << "Unable to initialise an OpenGL&Vulkan display." << std::endl;
		return(-1);
	}

	for (size_t i = 0; i < active.size();i++)
		activateExtension(active[i], true);
	for (size_t i = 0; i < deactive.size(); i++)
		activateExtension(deactive[i], false);

	NATIVE_EXTENSION *extensions = NULL;
	uint32_t s = 0;
	if (getExtensions(extensions,&s))
	{
		extensions = new NATIVE_EXTENSION[s];
		getExtensions(extensions, &s);
	}
	else
	{
		std::cout << "Unable to query OpenGL&Vulkan extensions." << std::endl;
		return(-1);
	}

	if (verbose) for (uint32_t i=0;i<s;i++)
	{
		const NATIVE_EXTENSION& extension = extensions[i];
		std::cout << "extension: " << extension.extensionName;
		std::cout << ((extension.active)?" O ":" x ") << std::endl;
	}

	if (!checkConsistency(false))
	{
		std::cout << "Inconsistency found in extension profile. Header file not generated." << std::endl;
		return(-1);
	}
	else
	{
		if (!writeHeader("CodeGeneration.h"))
		{
			std::cout << "Failed to generate header file." << std::endl;
			return(-1);
		}
	}

	return(0);
}
