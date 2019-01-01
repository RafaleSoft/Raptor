// WVector.h: interface for the CWVector class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WVector_H__7493DD94_5C3A_4025_9FA6_C8F0EC4BE053__INCLUDED_)
#define AFX_WVector_H__7493DD94_5C3A_4025_9FA6_C8F0EC4BE053__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __GENERIC_VECTOR_H__
	#include "GenericVector.h"
#endif

class CWVector : public CGenericVector<unsigned short>
{
public:
	// Construction/Destruction
	CWVector(){};
	virtual ~CWVector(){};

#ifndef SIMD_NO_ASSEMBLY
	CWVector& operator= ( const CWVector& v )
	{
		__asm
		{
			mov esi,v
			mov edi,this
			mov eax,[esi+4]
			mov ebx,[esi+8]
			mov [edi+4],eax
			mov [edi+8],ebx
		}
		return *this;
	};
	CWVector& operator= ( const CGenericVector<unsigned short>& v )
	{
		__asm
		{
			mov esi,v
			mov edi,this
			mov eax,[esi+4]
			mov ebx,[esi+8]
			mov [edi+4],eax
			mov [edi+8],ebx
		}
		return *this;
	};
	CWVector& operator= ( const unsigned short v[4] )
	{
		__asm
		{
			mov esi,v
			mov edi,this
			mov eax,[esi+4]
			mov ebx,[esi]
			mov [edi+8],eax
			mov [edi+4],ebx
		}
		return *this;
	};	

	//operations
	bool operator== ( const CGenericVector<unsigned short>& v ) const
	{
		__asm
		{
			mov edi,this
			mov esi,v
			xor eax,eax
			mov ecx,[edi+4]
			cmp ecx,[esi+4]
			jne noteq
			mov ecx,[edi+8]
			cmp ecx,[esi+8]
			jne noteq
			mov eax,1
			noteq:
		}
// no return value
#pragma warning(disable:4035)
	};
#pragma warning(default:4035)

	bool operator== ( const CWVector& v ) const
	{
		__asm
		{
			mov edi,this
			mov esi,v
			xor eax,eax
			mov ecx,[edi+4]
			cmp ecx,[esi+4]
			jne noteq
			mov ecx,[edi+8]
			cmp ecx,[esi+8]
			jne noteq
			mov eax,1
			noteq:
		}
// no return value
#pragma warning(disable:4035)
	};
#pragma warning(default:4035)

	bool operator== ( const unsigned short v[4] ) const
	{
		__asm
		{
		mov edi,this
		mov esi,v
		xor eax,eax
		mov ecx,[edi+4]
		cmp ecx,[esi]
		jne noteq
		mov ecx,[edi+8]
		cmp ecx,[esi+4]
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

#endif
};

CWVector SIMD_CALL operator*(const CWVector&, const float&);
CWVector SIMD_CALL operator*(const float&, const CWVector&);

#endif // !defined(AFX_WVector_H__7493DD94_5C3A_4025_9FA6_C8F0EC4BE053__INCLUDED_)

