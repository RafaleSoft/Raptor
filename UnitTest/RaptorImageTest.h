#pragma once

#include "Raptordll.h"
#include "core/Test.h"
#include "core/TestFixture.h"

RAPTOR_NAMESPACE


class CRaptorImageTest : public CppUnit::TestFixture
{
public:
	CRaptorImageTest();
	virtual ~CRaptorImageTest();

	//!	Specific test cases
	void testLoadImage();
	void testSaveImage();
	void testOperators();

	//!	Profide a full suite of tests for Raptor handle
	static CppUnit::Test *suite();

	//! Implement base class
	void setUp();
	void tearDown();
};

