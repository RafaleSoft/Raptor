// ByteVector.h: interface for the CByteVector class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BYTEVECTOR_H__A9932FAD_211E_4F03_BF5B_C0E608372FE8__INCLUDED_)
#define AFX_BYTEVECTOR_H__A9932FAD_211E_4F03_BF5B_C0E608372FE8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __GENERIC_VECTOR_H__
	#include "GenericVector.h"
#endif

class CBVector : public CGenericVector<char>  
{
public:
	CBVector(){};
	virtual ~CBVector(){};

	CBVector& operator= ( const CBVector& v )
	{ 	
		int t = *((int*)v.vector());
		*((int*)m_vector) = t;
		return *this;
	};
	CBVector& operator= ( const CGenericVector<char>& v )
	{ 
		int t = *((int*)v.vector());
		*((int*)m_vector) = t;
		return *this;
	};
	CBVector& operator= ( const char v[4] )
	{
		int t = *((int*)v);
		*((int*)m_vector) = t;
		return *this; 
	};	

	//operations
	bool operator== ( const CGenericVector<char>& v ) const
	{ 
		__asm
		{
			mov edi,this
			mov esi,v
			xor eax,eax
			mov ecx,[edi+4]
			cmp ecx,[esi+4]
			jne noteq
			mov eax,1
			noteq:
		}
// no return value
#pragma warning(disable:4035)
	};
#pragma warning(default:4035)

	bool operator== ( const CBVector& v ) const
	{ 
		__asm
		{
			mov edi,this
			mov esi,v
			xor eax,eax
			mov ecx,[edi+4]
			cmp ecx,[esi+4]
			jne noteq
			mov eax,1
			noteq:
		}
// no return value
#pragma warning(disable:4035)
	};
#pragma warning(default:4035)

	bool operator== ( const short v[4] ) const
	{ 
		__asm
		{
		mov edi,this
		mov esi,v
		xor eax,eax
		mov ecx,[edi+4]
		cmp ecx,[esi]
		jne noteq
		mov eax,1
		noteq:
		}	
// no return value
#pragma warning(disable:4035)
	};
#pragma warning(default:4035)

	void SIMD_CALL Zero() 
	{ 
		__asm 
		{
			mov edi,this
			xor eax,eax 
			mov [edi+8],eax
			mov [edi+4],eax 
		}
	};

	void SIMD_CALL One()
	{ 
		__asm 
		{
			mov edi,this
			mov eax,0x00010001
			mov [edi+8],eax
			mov [edi+4],eax
		}
	};

	// input/output for special case : chars as signed bytes
	std::istream& operator>> (std::istream& i)
	{ return i>>vector()[0]>>vector()[1]>>vector()[2]>>vector()[3];};
	std::ostream& operator<< (std::ostream& o)
	{ return o<<(short)vector()[0]<<" "<<(short)vector()[1]<<" "<<(short)vector()[2]<<" "<<(short)vector()[3];};
};

#endif // !defined(AFX_BYTEVECTOR_H__A9932FAD_211E_4F03_BF5B_C0E608372FE8__INCLUDED_)

