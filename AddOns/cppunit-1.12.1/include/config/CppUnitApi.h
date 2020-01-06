#ifndef CPPUNIT_CONFIG_CPPUNITAPI
#define CPPUNIT_CONFIG_CPPUNITAPI

#undef CPPUNIT_API

#ifdef WIN32

// define CPPUNIT_DLL_BUILD when building CppUnit dll.
#ifdef CPPUNIT_BUILD_DLL
	#define CPPUNIT_API __declspec(dllexport)
	#define EXPIMP_TEMPLATE	
#endif

// define CPPUNIT_DLL when linking to CppUnit dll.
#ifdef CPPUNIT_DLL
	#define CPPUNIT_API __declspec(dllimport)
	#define EXPIMP_TEMPLATE extern
#endif


// std::string needs to have dll-interface to be used by clients of cppunit_dll
#include <string>
#include <deque>
EXPIMP_TEMPLATE template union CPPUNIT_API std::_String_val<std::_Simple_types<char>>::_Bxty;
EXPIMP_TEMPLATE struct CPPUNIT_API std::_Container_base12;
EXPIMP_TEMPLATE template class CPPUNIT_API std::_Deque_val<std::_Deque_simple_types<std::basic_string<char, std::char_traits<char>, std::allocator<char>>>>;
EXPIMP_TEMPLATE template class CPPUNIT_API std::_Compressed_pair<std::_Wrap_alloc<std::allocator<std::string>>, std::_Deque_val<std::_Deque_simple_types<std::basic_string<char, std::char_traits<char>, std::allocator<char>>>>, true>;
EXPIMP_TEMPLATE template class CPPUNIT_API std::_String_val<std::_Simple_types<char>>;
EXPIMP_TEMPLATE template class CPPUNIT_API std::_Compressed_pair<std::_Wrap_alloc<std::allocator<char>>, std::_String_val<std::_Simple_types<char>>, true>;
EXPIMP_TEMPLATE template class CPPUNIT_API std::basic_string<char, std::char_traits<char>, std::allocator<char>>;
EXPIMP_TEMPLATE template class CPPUNIT_API std::deque<std::string, std::allocator<std::string>>;

#endif


#ifndef CPPUNIT_API
	#define CPPUNIT_API
#endif

 
#endif  // CPPUNIT_CONFIG_CPPUNITAPI
