// Environment.h: interface for the CEnvironment class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ENVIRONMENT_H__CFB09A15_7E14_40E3_819D_0AADE7F21498__INCLUDED_)
#define AFX_ENVIRONMENT_H__CFB09A15_7E14_40E3_819D_0AADE7F21498__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "..\simdlib\simd.h"	//CWVector

class CTexture;

class CEnvironment  
{
public:
	virtual ~CEnvironment();

	static CEnvironment	*GetInstance(void);

	void SetTexture(CTexture *t) { m_pEnvironMap = t; };
	CTexture*	GetTexture(void) const { return m_pEnvironMap; };
	CWVector&		GetLocalColor(const CGenericVector<float>	&direction);

private:
	CEnvironment();

	static CEnvironment	*m_pEnvironment;

	CTexture	*m_pEnvironMap;
	CWVector	tmpClr;
	CGenericVector<float> tmpVect;
};

#endif // !defined(AFX_ENVIRONMENT_H__CFB09A15_7E14_40E3_819D_0AADE7F21498__INCLUDED_)
