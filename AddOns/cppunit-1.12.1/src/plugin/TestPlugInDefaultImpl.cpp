#include <config/SourcePrefix.h>

#if !defined(CPPUNIT_NO_TESTPLUGIN)

#include <core/TestSuite.h>
#include <helper/TestFactoryRegistry.h>
#include <plugin/TestPlugInDefaultImpl.h>


CPPUNIT_NS_BEGIN


TestPlugInDefaultImpl::TestPlugInDefaultImpl() 
{
}


TestPlugInDefaultImpl::~TestPlugInDefaultImpl()
{
}


void 
TestPlugInDefaultImpl::initialize( TestFactoryRegistry *registry,
                                   const PlugInParameters &parameters )
{
}


void 
TestPlugInDefaultImpl::addListener( TestResult *eventManager )
{
}


void 
TestPlugInDefaultImpl::removeListener( TestResult *eventManager )
{
}


void 
TestPlugInDefaultImpl::addXmlOutputterHooks( XmlOutputter *outputter )
{
}


void 
TestPlugInDefaultImpl::removeXmlOutputterHooks()
{
}


void 
TestPlugInDefaultImpl::uninitialize( TestFactoryRegistry *registry )
{
}


CPPUNIT_NS_END


#endif // !defined(CPPUNIT_NO_TESTPLUGIN)
