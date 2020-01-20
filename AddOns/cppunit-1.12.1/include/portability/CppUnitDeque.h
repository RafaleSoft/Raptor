#ifndef CPPUNIT_PORTABILITY_CPPUNITDEQUE_H
#define CPPUNIT_PORTABILITY_CPPUNITDEQUE_H

// The technic used is similar to the wrapper of STLPort.
 
#include <Portability.h>
#include <deque>


#if CPPUNIT_STD_NEED_ALLOCATOR

template<class T>
class CppUnitDeque : public std::deque<T,CPPUNIT_STD_ALLOCATOR>
{
public:
};

#else // CPPUNIT_STD_NEED_ALLOCATOR

#define CppUnitDeque std::deque

#endif

#ifdef WIN32
	#if _MSC_VER > 1800
		#define DLL_EXPORT_DEQUE(type) \
			EXPIMP_TEMPLATE template class CPPUNIT_API std::_Deque_val<std::_Deque_simple_types<CppUnit::type *>>; \
			EXPIMP_TEMPLATE template class CPPUNIT_API std::_Compressed_pair<std::_Wrap_alloc<std::allocator<CppUnit::type *>>, std::_Deque_val<std::_Deque_simple_types<CppUnit::type *>>, true>; \
			EXPIMP_TEMPLATE template class CPPUNIT_API std::deque<CppUnit::type *>;
	#else
		#define DLL_EXPORT_DEQUE(type) \
			EXPIMP_TEMPLATE template class CPPUNIT_API std::deque<CppUnit::type *>;
	#endif
#else
	#define DLL_EXPORT_DEQUE(type)
#endif

#endif // CPPUNIT_PORTABILITY_CPPUNITDEQUE_H

