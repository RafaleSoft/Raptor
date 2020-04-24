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

/*
	JPEG testing: integrate here ?

IF EXIST testout* $(RM) testout*
.\djpeg - dct int - ppm - outfile testout.ppm  testorig.jpg
.\djpeg - dct int - bmp - colors 256 - outfile testout.bmp  testorig.jpg
.\cjpeg - dct int - outfile testout.jpg  testimg.ppm
.\djpeg - dct int - ppm - outfile testoutp.ppm testprog.jpg
.\cjpeg - dct int - progressive - opt - outfile testoutp.jpg testimg.ppm
.\jpegtran - outfile testoutt.jpg testprog.jpg
fc / b testimg.ppm testout.ppm
fc / b testimg.bmp testout.bmp
fc / b testimg.jpg testout.jpg
fc / b testimg.ppm testoutp.ppm
fc / b testimgp.jpg testoutp.jpg
fc / b testorig.jpg testoutt.jpg
*/

void CRaptorImageTest::testLoadImage()
{
	CImage image;
	CImage::IImageIO *io = image.getImageKindIO("jpg");
	CPPUNIT_ASSERT( io->isOfKind("jpg") );

	std::string input = datapath;
	input += "/testdata/testimg.jpg";
	CPPUNIT_ASSERT(io->loadImageFile(input,&image));

	io = image.getImageKindIO("png");
	CPPUNIT_ASSERT(io->isOfKind("PNG"));

	input = datapath;
	input += "/testdata/teapot.png";
	CPPUNIT_ASSERT(io->loadImageFile(input, &image));

	io = image.getImageKindIO("BMP");
	CPPUNIT_ASSERT(io->isOfKind("bmp"));

	input = datapath;
	input += "/testdata/Raptor.bmp";
	CPPUNIT_ASSERT(io->loadImageFile(input, &image));

	io = image.getImageKindIO("TIFF");
	CPPUNIT_ASSERT(io->isOfKind("TIF"));

	input = datapath;
	input += "/testdata/hdr.tif";
	CPPUNIT_ASSERT(io->loadImageFile(input, &image));
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
