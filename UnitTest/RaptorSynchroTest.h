#pragma once

#include "Raptordll.h"
#include "core/Test.h"
#include "core/TestFixture.h"

RAPTOR_NAMESPACE


class CRaptorSynchroTest : public CppUnit::TestFixture
{
public:
	CRaptorSynchroTest();
	virtual ~CRaptorSynchroTest();

	//!	Specific test cases
	void testMutex();

	//!	Profide a full suite of tests for Raptor handle
	static CppUnit::Test *suite();

	//! Implement base class
	void setUp();
	void tearDown();
};

