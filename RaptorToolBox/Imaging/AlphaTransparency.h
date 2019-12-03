/***************************************************************************/
/*                                                                         */
/*  AlphaTransparency.h                                                    */
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


#if !defined(AFX_ALPHATRANSPARENCY_H__8EAD8C9F_DC40_4063_8E0A_0C2AB300AD16__INCLUDED_)
#define AFX_ALPHATRANSPARENCY_H__8EAD8C9F_DC40_4063_8E0A_0C2AB300AD16__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if !defined(AFX_IMAGE_H__F545D0D5_5F10_4EFA_BE3B_3F3D34D4DBF3__INCLUDED_)
#include "System/Image.h"
#endif


RAPTOR_NAMESPACE_BEGIN

class RAPTOR_API CAlphaTransparency : public CImage::IImageOP
{
public:
	CAlphaTransparency(uint32_t t);
	CAlphaTransparency(const CAlphaTransparency& alpha);
	CAlphaTransparency& operator=(const CAlphaTransparency& alpha);

	virtual ~CAlphaTransparency();

	//! Implements CImageOP
	virtual OP_KIND getKind(void) const { return ALPHA_TRANSPARENCY; };

	//! Implements CImageOP
	virtual bool apply(CImage* const src) const;

private:
	CAlphaTransparency();

	//!	Texture transparency for alpha channel.
	uint32_t m_transparency;
};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_DEFAULTALPHATRANSPARENCY_H__8EAD8C9F_DC40_4063_8E0A_0C2AB300AD16__INCLUDED_)


