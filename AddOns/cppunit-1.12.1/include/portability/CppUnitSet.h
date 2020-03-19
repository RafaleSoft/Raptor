#ifndef CPPUNIT_PORTABILITY_CPPUNITSET_H
#define CPPUNIT_PORTABILITY_CPPUNITSET_H

// The technic used is similar to the wrapper of STLPort.
 
#include <Portability.h>
#include <functional>
#include <set>


#if CPPUNIT_STD_NEED_ALLOCATOR

template<class T>
class CppUnitSet : public std::set<T
                                  ,std::less<T>
                                  ,CPPUNIT_STD_ALLOCATOR>
{
public:
};

#else // CPPUNIT_STD_NEED_ALLOCATOR

#define CppUnitSet std::set

#endif

#ifdef WIN32
	#if _MSC_VER > 1900
		#define DLL_EXPORT_SET(type) \
			EXPIMP_TEMPLATE template class CPPUNIT_API std::_Tree_val<std::_Tree_simple_types<CppUnit::type *>>; \
			EXPIMP_TEMPLATE template class CPPUNIT_API std::_Compressed_pair<std::allocator<std::_Tree_node<CppUnit::type *, void *>>, std::_Tree_val<std::_Tree_simple_types<CppUnit::type *>>, true>; \
			EXPIMP_TEMPLATE template class CPPUNIT_API std::_Compressed_pair<std::less<CppUnit::type *>, std::_Compressed_pair<std::allocator<std::_Tree_node<CppUnit::type *, void *>>, std::_Tree_val<std::_Tree_simple_types<CppUnit::type *>>, true>, true>; \
			EXPIMP_TEMPLATE template class CPPUNIT_API std::set<CppUnit::type *>;
	#elif _MSC_VER > 1800
	#define DLL_EXPORT_SET(type) \
			EXPIMP_TEMPLATE template class CPPUNIT_API std::_Tree_val<std::_Tree_simple_types<CppUnit::type *>>; \
			EXPIMP_TEMPLATE template class CPPUNIT_API std::_Compressed_pair<std::_Wrap_alloc<std::allocator<std::_Tree_node<CppUnit::type *, void *>>>, std::_Tree_val<std::_Tree_simple_types<CppUnit::type *>>, true>; \
			EXPIMP_TEMPLATE template class CPPUNIT_API std::_Compressed_pair<std::less<CppUnit::type *>, std::_Compressed_pair<std::_Wrap_alloc<std::allocator<std::_Tree_node<CppUnit::type *, void *>>>, std::_Tree_val<std::_Tree_simple_types<CppUnit::type *>>, true>, true>; \
			EXPIMP_TEMPLATE template class CPPUNIT_API std::set<CppUnit::type *>;
	#else
		#define DLL_EXPORT_SET(type) \
			EXPIMP_TEMPLATE template class CPPUNIT_API std::_Tree_val<std::_Tree_simple_types<CppUnit::type *>>; \
			EXPIMP_TEMPLATE template class CPPUNIT_API std::set<CppUnit::type *>;
	#endif
#endif

#endif // CPPUNIT_PORTABILITY_CPPUNITSET_H

