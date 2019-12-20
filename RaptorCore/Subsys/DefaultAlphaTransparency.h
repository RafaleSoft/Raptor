// DefaultAlphaTransparency.h: interface for the CDefaultAlphaTransparency class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DEFAULTALPHATRANSPARENCY_H__8EAD8C9F_DC40_4063_8E0A_0C2AB300AD16__INCLUDED_)
#define AFX_DEFAULTALPHATRANSPARENCY_H__8EAD8C9F_DC40_4063_8E0A_0C2AB300AD16__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if !defined(AFX_IMAGE_H__F545D0D5_5F10_4EFA_BE3B_3F3D34D4DBF3__INCLUDED_)
#include "System/Image.h"
#endif

RAPTOR_NAMESPACE_BEGIN

class CDefaultAlphaTransparency : public CImage::IImageOP
{
public:
	CDefaultAlphaTransparency(uint32_t t);
	CDefaultAlphaTransparency(const CDefaultAlphaTransparency& alpha);
	CDefaultAlphaTransparency& operator=(const CDefaultAlphaTransparency& alpha);

	virtual ~CDefaultAlphaTransparency();

	//! Implements CImageOP
	virtual OP_KIND getKind(void) const { return ALPHA_TRANSPARENCY; };

	//! Implements CImageOP
	virtual bool apply(CImage* const src) const; // , const operation_param_t& param) const;

private:
	CDefaultAlphaTransparency();

	//!	Texture resize factors.
	uint32_t transparency;
};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_DEFAULTALPHATRANSPARENCY_H__8EAD8C9F_DC40_4063_8E0A_0C2AB300AD16__INCLUDED_)


