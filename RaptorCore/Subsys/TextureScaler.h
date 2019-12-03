/***************************************************************************/
/*                                                                         */
/*  TextureScaler.h                                                        */
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


#if !defined(AFX_TEXTURESCALER_H__6EB6436B_D35C_41F6_96EA_B079626A43A0__INCLUDED_)
#define AFX_TEXTURESCALER_H__6EB6436B_D35C_41F6_96EA_B079626A43A0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if !defined(AFX_IMAGE_H__F545D0D5_5F10_4EFA_BE3B_3F3D34D4DBF3__INCLUDED_)
	#include "System/Image.h"
#endif

RAPTOR_NAMESPACE_BEGIN


class CTextureScaler : public CImage::IImageOP
{
public:
	CTextureScaler(float sx, float sy);
	CTextureScaler(const CTextureScaler& scaler);
	CTextureScaler& operator=(const CTextureScaler& scaler);
	

	virtual ~CTextureScaler();

    //! Implements CImageOP
    virtual OP_KIND getKind(void) const { return IMAGE_SCALER; };

	//! Implements CImageOP
	virtual bool apply(CImage* const src) const;


private:
	CTextureScaler();

	//!	Texture resize factors.
	float scale_x;
	float scale_y;
};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_TEXTURESCALER_H__6EB6436B_D35C_41F6_96EA_B079626A43A0__INCLUDED_)


