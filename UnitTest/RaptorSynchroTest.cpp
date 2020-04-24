#include "stdafx.h"

#include "RaptorSynchroTest.h"
#include "System/Image.h"
#include "ToolBox/Imaging.h"

#include "core/TestCase.h"
#include "core/TestAssert.h"
#include "core/TestSuite.h"
#include "helper/TestCaller.h"


static CRaptorMutex *mutex;
static const size_t ARRAY_SIZE = 1000;

CRaptorSynchroTest::CRaptorSynchroTest()
	:TestFixture()
{
}


CRaptorSynchroTest::~CRaptorSynchroTest()
{
}

void CRaptorSynchroTest::setUp()
{
	mutex = new CRaptorMutex();
}

void CRaptorSynchroTest::tearDown()
{
	delete mutex;
}

DWORD WINAPI Thread(void* pParam)
{
	for (size_t j=0; j<5; j++)
	for (size_t i=0; i<ARRAY_SIZE;i++)
	{
		CRaptorLock lock(*mutex);
		uint32_t *array = (uint32_t*)pParam;
		array[i] = array[i] + 1;
	}
	
	ExitThread(0);
	return 0;
}

void CRaptorSynchroTest::testMutex()
{
	uint32_t *array = new uint32_t[1000];
	memset(array, 0, ARRAY_SIZE * sizeof(uint32_t));

	HANDLE thread1 = CreateThread(NULL, 0, Thread, array, CREATE_SUSPENDED, NULL);
	HANDLE thread2 = CreateThread(NULL, 0, Thread, array, CREATE_SUSPENDED, NULL);
	HANDLE thread3 = CreateThread(NULL, 0, Thread, array, CREATE_SUSPENDED, NULL);

	CPPUNIT_ASSERT(0 != thread1);
	CPPUNIT_ASSERT(0 != thread2);
	CPPUNIT_ASSERT(0 != thread3);

	SetThreadPriority(thread1, THREAD_PRIORITY_NORMAL);
	SetThreadPriority(thread2, THREAD_PRIORITY_NORMAL);
	SetThreadPriority(thread3, THREAD_PRIORITY_NORMAL);

	ResumeThread(thread1);
	ResumeThread(thread2);
	ResumeThread(thread3);

	WaitForSingleObject(thread1, INFINITE);
	WaitForSingleObject(thread2, INFINITE);
	WaitForSingleObject(thread3, INFINITE);

	CPPUNIT_ASSERT(0 != CloseHandle(thread1));
	CPPUNIT_ASSERT(0 != CloseHandle(thread2));
	CPPUNIT_ASSERT(0 != CloseHandle(thread3));

	bool res = true;
	for (size_t i = 0; i < ARRAY_SIZE; i++)
	{
		res = res && (array[i] == 15);
	}

	CPPUNIT_ASSERT(res);
}

CppUnit::Test *CRaptorSynchroTest::suite()
{
	CppUnit::TestSuite *suiteOfTests = new CppUnit::TestSuite("RAPTOR_CRaptorMutex_test");
	
	suiteOfTests->addTest(new CppUnit::TestCaller<CRaptorSynchroTest>("testMutex", &CRaptorSynchroTest::testMutex));
	
	return suiteOfTests;
}
