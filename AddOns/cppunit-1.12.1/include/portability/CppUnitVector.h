#ifndef CPPUNIT_PORTABILITY_CPPUNITVECTOR_H
#define CPPUNIT_PORTABILITY_CPPUNITVECTOR_H

// The technic used is similar to the wrapper of STLPort.
 
#include <Portability.h>
#include <vector>


#if CPPUNIT_STD_NEED_ALLOCATOR

template<class T>
class CppUnitVector : public std::vector<T,CPPUNIT_STD_ALLOCATOR>
{
public:
};

#else // CPPUNIT_STD_NEED_ALLOCATOR

#define CppUnitVector std::vector

#endif


#ifdef WIN32
	#if _MSC_VER > 1800
		#define DLL_EXPORT_VECTOR(type) \
			EXPIMP_TEMPLATE template class CPPUNIT_API std::_Vector_val<std::_Simple_types<CppUnit::type *>>; \
			EXPIMP_TEMPLATE template class CPPUNIT_API std::_Compressed_pair<std::_Wrap_alloc<std::allocator<CppUnit::type *>>, std::_Vector_val<std::_Simple_types<CppUnit::type *>>, true>; \
			EXPIMP_TEMPLATE template class CPPUNIT_API std::vector<type *>;
	#else
		#define DLL_EXPORT_VECTOR(type) \
			EXPIMP_TEMPLATE template class CPPUNIT_API std::_Vector_val<std::_Simple_types<CppUnit::type *>>; \
			EXPIMP_TEMPLATE template class CPPUNIT_API std::vector<type *>;
	#endif
#endif


#endif // CPPUNIT_PORTABILITY_CPPUNITVECTOR_H

