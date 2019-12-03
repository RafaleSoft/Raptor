/***************************************************************************/
/*                                                                         */
/*  ImageScaler.h                                                          */
/*                                                                         */
/*    Raptor OpenGL & Vulkan realtime 3D Engine SDK.                       */
/*                                                                         */
/*  Copyright 1998-2019 by                                                 */
/*  Fabrice FERRAND.                                                       */
/*                                                                         */
/*  This file is part of the Raptor project, and may only be used,         */
/*  modified, and distributed under the terms of the Raptor project        */
/*  license, LICENSE.  By continuing to use, modify, or distribute         */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/


#if !defined(AFX_IMAGESCALER_H__E3E63A13_79FC_4E46_A1D5_BCD41CF86360__INCLUDED_)
#define AFX_IMAGESCALER_H__E3E63A13_79FC_4E46_A1D5_BCD41CF86360__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if !defined(AFX_IMAGE_H__F545D0D5_5F10_4EFA_BE3B_3F3D34D4DBF3__INCLUDED_)
	#include "System/Image.h"
#endif

RAPTOR_NAMESPACE_BEGIN


class RAPTOR_API CImageScaler : public CImage::IImageOP
{
public:
	CImageScaler(float sx, float sy);
	CImageScaler(const CImageScaler& scaler);
	CImageScaler& operator=(const CImageScaler& scaler);

	virtual ~CImageScaler();

    //! Implements CImageOP
    virtual OP_KIND getKind(void) const { return IMAGE_SCALER; };

	//! Implements CImageOP
	virtual bool apply(CImage* const src) const; // , const operation_param_t& param) const;


private:
	CImageScaler();

	void scaleFloats(size_t srcw, size_t srch, size_t dstw, size_t dsth, float *srcpx, float *dstpx);

	void scaleUints(size_t srcw, size_t srch, size_t dstw, size_t dsth, uint32_t *srcpx, uint32_t *dstpx);

	//!	Texture resize factors.
	float scale_x;
	float scale_y;
};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_DEFAULTIMAGESCALER_H__E3E63A13_79FC_4E46_A1D5_BCD41CF86360__INCLUDED_)


