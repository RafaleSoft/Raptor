// UnitTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "RaptorHandleTest.h"
#include "RaptorImageTest.h"
#include "RaptorSynchroTest.h"

#include "core/TestResult.h"
#include "core/TestSuite.h"
#include "ui/text/TestRunner.h"

#include "System/RaptorConfig.h"
#include "System/Raptor.h"

RAPTOR_NAMESPACE


char *datapath = NULL;

int _tmain(int argc, _TCHAR* argv[])
{
	printf("Welcome to Raptor UnitTests.\n\n");

	CppUnit::TextUi::TestRunner runner;

	if (argc > 1)
		datapath = _strdup(argv[1]);
	else
	{
		printf("WARNING: Default data path may be incorrect ! \n");
		datapath = _strdup("");
	}

	printf("Using test data from path: [%s]\n\n", datapath);

	printf("Initializing Raptor ... ");
	CRaptorConfig config;
	config.m_bRelocation = false;
	config.m_uiPolygons = 10000;
	config.m_uiVertices = 40000;
	config.m_uiTexels = 1000000;
	config.m_logFile = "UnitTest_Raptor.log";
	IRaptor::glInitRaptor(config);
	printf("Done.\n\n");

	//	Create the test suite for Raptor Handles
	CppUnit::Test *suite = CRaptorHandleTest::suite();
	runner.addTest(suite);

	//	Create the test suite for Raptor CImage
	suite = CRaptorImageTest::suite();
	runner.addTest(suite);

	//	Create the test suite for Raptor CRaptorMutex
	suite = CRaptorSynchroTest::suite();
	runner.addTest(suite);

	bool res = runner.run();

	printf("Releasing Raptor ... ");
	IRaptor::glQuitRaptor();
	printf("Done.\n\n");

	return (res ? 0 : 1);
}

