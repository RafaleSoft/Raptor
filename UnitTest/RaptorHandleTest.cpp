#include "stdafx.h"

#include "RaptorHandleTest.h"
#include "System/CGLTypes.h"

#include "core/TestCase.h"
#include "core/TestAssert.h"
#include "core/TestSuite.h"
#include "helper/TestCaller.h"


CRaptorHandleTest::CRaptorHandleTest()
	:TestFixture()
{
}


CRaptorHandleTest::~CRaptorHandleTest()
{
}

void CRaptorHandleTest::setUp()
{

}

void CRaptorHandleTest::tearDown()
{

}


void CRaptorHandleTest::testEquality()
{
	RAPTOR_HANDLE h1;
	RAPTOR_HANDLE h2;
	CPPUNIT_ASSERT( h1 == h2);

	uint64_t p = (uint64_t)this;
	RAPTOR_HANDLE h3(42, this);
	RAPTOR_HANDLE h4(42, p);
	CPPUNIT_ASSERT(h3 == h4);

	GLhandleARB h = 1234;
	GLuint hh = 1234;
	RAPTOR_HANDLE h5(42, h);
	RAPTOR_HANDLE h6(42, hh);
	CPPUNIT_ASSERT(h5 == h6);
}

void CRaptorHandleTest::testGetters()
{
	uint64_t p = (uint64_t)this;
	RAPTOR_HANDLE h3(42, this);
	RAPTOR_HANDLE h4(42, p);

	CPPUNIT_ASSERT(h3.hClass() == h4.hClass());
	CPPUNIT_ASSERT((void*)h3.handle() == h4.ptr<CRaptorHandleTest>());
}

void CRaptorHandleTest::testSetters()
{
	RAPTOR_HANDLE h1;
	h1.hClass(42);
	CPPUNIT_ASSERT(h1.hClass() == 42);

	h1.ptr(this);
	CPPUNIT_ASSERT(h1.ptr<CRaptorHandleTest>() == this);

	GLhandleARB h = 1234;
	h1.glhandle(h);
	CPPUNIT_ASSERT(h1.glhandle() == h);

	uint64_t hh = 0xfedcba9876543210;
	h1.handle(hh);
	CPPUNIT_ASSERT(h1.handle() == hh);
}

CppUnit::Test *CRaptorHandleTest::suite()
{
	CppUnit::TestSuite *suiteOfTests = new CppUnit::TestSuite("RAPTOR_HANDLE_test");
	
	suiteOfTests->addTest(new CppUnit::TestCaller<CRaptorHandleTest>("testEquality", &CRaptorHandleTest::testEquality));
	suiteOfTests->addTest(new CppUnit::TestCaller<CRaptorHandleTest>("testGetters", &CRaptorHandleTest::testGetters));
	suiteOfTests->addTest(new CppUnit::TestCaller<CRaptorHandleTest>("testSetters", &CRaptorHandleTest::testSetters));
	
	return suiteOfTests;
}
