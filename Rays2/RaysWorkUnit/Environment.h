// Environment.h: interface for the CEnvironment class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ENVIRONMENT_H__CFB09A15_7E14_40E3_819D_0AADE7F21498__INCLUDED_)
#define AFX_ENVIRONMENT_H__CFB09A15_7E14_40E3_819D_0AADE7F21498__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#if !defined(AFX_COLOR_H__3770AC59_0D0E_49EF_99C8_037268A33CE4__INCLUDED_)
	#include "System/Color.h"
#endif
#ifndef __GENERIC_VECTOR_H__
	#include "SimdLib/GenericVector.h"
#endif
RAPTOR_NAMESPACE

class CTexture;


class CEnvironment  
{
public:
	virtual ~CEnvironment();

	static CEnvironment	*GetInstance(void);

	void SetTexture(CTexture *t) { m_pEnvironMap = t; };
	CTexture*	GetTexture(void) const { return m_pEnvironMap; };
	CColor::RGBA&	GetLocalColor(const CGenericVector<float>	&direction);

private:
	CEnvironment();

	static CEnvironment	*m_pEnvironment;

	CTexture		*m_pEnvironMap;
	CColor::RGBA	tmpClr;
	CGenericVector<float> tmpVect;
};

#endif // !defined(AFX_ENVIRONMENT_H__CFB09A15_7E14_40E3_819D_0AADE7F21498__INCLUDED_)
