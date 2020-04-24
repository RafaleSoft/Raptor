#include <core/Exception.h>
#include <core/Test.h>
#include <core/TestFailure.h>
#include <listener/TextTestResult.h>
#include <output/TextOutputter.h>
#include <portability/Stream.h>


CPPUNIT_NS_BEGIN


TextTestResult::TextTestResult()
{
  addListener( this );
}


void 
TextTestResult::addFailure( const TestFailure &failure )
{
  TestResultCollector::addFailure( failure );
  stdCOut() << ( failure.isError() ? "E" : "F" );
}


void 
TextTestResult::startTest( Test *test )
{
  TestResultCollector::startTest (test);
  stdCOut() << ".";
}


void 
TextTestResult::print( OStream &stream ) 
{
  TextOutputter outputter( this, stream );
  outputter.write();
}


OStream &
operator <<( OStream &stream, 
             TextTestResult &result )
{ 
  result.print (stream); return stream; 
}


CPPUNIT_NS_END
