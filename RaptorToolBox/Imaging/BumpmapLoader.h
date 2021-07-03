/***************************************************************************/
/*                                                                         */
/*  BumpmapLoader.h                                                        */
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


#if !defined(AFX_BUMPMAPLOADER_H__3841D5F8_284B_4DC5_9E4B_56EF18AF80F4__INCLUDED_)
#define AFX_BUMPMAPLOADER_H__3841D5F8_284B_4DC5_9E4B_56EF18AF80F4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if !defined(AFX_IMAGE_H__F545D0D5_5F10_4EFA_BE3B_3F3D34D4DBF3__INCLUDED_)
	#include "System/Image.h"
#endif


RAPTOR_NAMESPACE_BEGIN


class RAPTOR_API CBumpmapLoader : public CImage::IImageOP
{
public:
	CBumpmapLoader(float scale);
	CBumpmapLoader(const CBumpmapLoader& loader);
	CBumpmapLoader& operator=(const CBumpmapLoader& loader);

	//!	Destructor.
	virtual ~CBumpmapLoader();

    //! Implements CImageOP
    virtual OP_KIND getKind(void) const { return BUMPMAP_LOADER; };

	//! Implements CImageOP
	virtual bool apply(CImage* const src) const;


private:
	//!	Default constructor not available: bump_scale must be provided.
	CBumpmapLoader();

	//!	Bumping amplitude.
	float bump_scale;
};


RAPTOR_NAMESPACE_END

#endif // !defined(AFX_DEFAULTBUMPMAPLOADER_H__3841D5F8_284B_4DC5_9E4B_56EF18AF80F4__INCLUDED_)


