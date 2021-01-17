/***************************************************************************/
/*                                                                         */
/*  TextureAlpha.h                                                         */
/*                                                                         */
/*    Raptor OpenGL & Vulkan realtime 3D Engine SDK.                       */
/*                                                                         */
/*  Copyright 1998-2021 by                                                 */
/*  Fabrice FERRAND.                                                       */
/*                                                                         */
/*  This file is part of the Raptor project, and may only be used,         */
/*  modified, and distributed under the terms of the Raptor project        */
/*  license, LICENSE.  By continuing to use, modify, or distribute         */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/


#if !defined(AFX_TEXTUREALPHA_H__479A98C2_8277_4153_B96F_0706D8FB8A8A__INCLUDED_)
#define AFX_TEXTUREALPHA_H__479A98C2_8277_4153_B96F_0706D8FB8A8A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if !defined(AFX_IMAGE_H__F545D0D5_5F10_4EFA_BE3B_3F3D34D4DBF3__INCLUDED_)
#include "System/Image.h"
#endif


RAPTOR_NAMESPACE_BEGIN

class CTextureAlpha : public CImage::IImageOP
{
public:
	CTextureAlpha(uint32_t t);
	CTextureAlpha(const CTextureAlpha& alpha);
	CTextureAlpha& operator=(const CTextureAlpha& alpha);

	virtual ~CTextureAlpha();

	//! Implements CImageOP
	virtual OP_KIND getKind(void) const { return ALPHA_TRANSPARENCY; };

	//! Implements CImageOP
	virtual bool apply(CImage* const src) const;

private:
	CTextureAlpha();

	//!	Texture transparency for alpha channel.
	uint32_t m_transparency;
};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_TEXTUREALPHA_H__479A98C2_8277_4153_B96F_0706D8FB8A8A__INCLUDED_)


