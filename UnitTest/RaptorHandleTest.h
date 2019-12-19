#pragma once

#include "Raptordll.h"
#include "core/Test.h"
#include "core/TestFixture.h"

RAPTOR_NAMESPACE


class CRaptorHandleTest : public CppUnit::TestFixture
{
public:
	CRaptorHandleTest();
	virtual ~CRaptorHandleTest();

	//!	Specific test cases
	void testEquality();
	void testGetters();
	void testSetters();

	//!	Profide a full suite of tests for Raptor handle
	static CppUnit::Test *suite();

	//! Implement base class
	void setUp();
	void tearDown();
};

