#include "stdafx.h"

#include "RaptorImageTest.h"
#include "System/Image.h"
#include "ToolBox/Imaging.h"

#include "core/TestCase.h"
#include "core/TestAssert.h"
#include "core/TestSuite.h"
#include "helper/TestCaller.h"


CRaptorImageTest::CRaptorImageTest()
	:TestFixture()
{
}


CRaptorImageTest::~CRaptorImageTest()
{
}

void CRaptorImageTest::setUp()
{
	// Load Toolbox loaders
	CImaging::installImagers();
}

void CRaptorImageTest::tearDown()
{

}


void CRaptorImageTest::testLoadImage()
{
	CImage image;
	CImage::IImageIO *io = image.getImageKindIO("jpg");
	CPPUNIT_ASSERT( io->isOfKind("jpg") );
}

void CRaptorImageTest::testSaveImage()
{
	//CImage image;
	CPPUNIT_ASSERT(0 == 0);
}

void CRaptorImageTest::testOperators()
{
	//CImage image;
	CPPUNIT_ASSERT(0 == 0);
}

CppUnit::Test *CRaptorImageTest::suite()
{
	CppUnit::TestSuite *suiteOfTests = new CppUnit::TestSuite("RAPTOR_CImage_test");
	
	suiteOfTests->addTest(new CppUnit::TestCaller<CRaptorImageTest>("testLoadImage", &CRaptorImageTest::testLoadImage));
	suiteOfTests->addTest(new CppUnit::TestCaller<CRaptorImageTest>("testSaveImage", &CRaptorImageTest::testSaveImage));
	suiteOfTests->addTest(new CppUnit::TestCaller<CRaptorImageTest>("testOperators", &CRaptorImageTest::testOperators));
	
	return suiteOfTests;
}
