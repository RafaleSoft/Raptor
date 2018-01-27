// DefaultImageScaler.h: interface for the CDefaultImageScaler class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DEFAULTIMAGESCALER_H__E3E63A13_79FC_4E46_A1D5_BCD41CF86360__INCLUDED_)
#define AFX_DEFAULTIMAGESCALER_H__E3E63A13_79FC_4E46_A1D5_BCD41CF86360__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if !defined(AFX_IMAGE_H__F545D0D5_5F10_4EFA_BE3B_3F3D34D4DBF3__INCLUDED_)
	#include "System/Image.h"
#endif

RAPTOR_NAMESPACE_BEGIN


class CDefaultImageScaler : public CImage::IImageOP
{
public:
	CDefaultImageScaler(float sx, float sy);
	CDefaultImageScaler(const CDefaultImageScaler& scaler);
	CDefaultImageScaler& operator=(const CDefaultImageScaler& scaler);

	virtual ~CDefaultImageScaler();

    //! Implements CImageOP
    virtual OP_KIND getKind(void) const { return IMAGE_SCALER; };

	//! Implements CImageOP
	virtual bool apply(CImage* const src, const operation_param_t& param) const;

private:
	CDefaultImageScaler();

	//!	Texture resize factors.
	float scale_x;
	float scale_y;
};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_DEFAULTIMAGESCALER_H__E3E63A13_79FC_4E46_A1D5_BCD41CF86360__INCLUDED_)


