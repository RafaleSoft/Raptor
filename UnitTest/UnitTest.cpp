// UnitTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "RaptorHandleTest.h"
#include "RaptorImageTest.h"

#include "core/TestResult.h"
#include "core/TestSuite.h"
#include "ui/text/TestRunner.h"


int _tmain(int argc, _TCHAR* argv[])
{
	CppUnit::TextUi::TestRunner runner;

	runner.addTest(CRaptorHandleTest::suite());
	runner.addTest(CRaptorImageTest::suite());
	bool res = runner.run();

	return (res ? 0 : 1);
}

