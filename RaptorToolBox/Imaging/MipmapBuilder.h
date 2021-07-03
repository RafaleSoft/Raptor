/***************************************************************************/
/*                                                                         */
/*  MipmapBuilder.h                                                        */
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


#if !defined(AFX_MIPMAPBUILDER_H__9C508D96_B614_4920_8816_B670295B6CE2__INCLUDED_)
#define AFX_MIPMAPBUILDER_H__9C508D96_B614_4920_8816_B670295B6CE2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if !defined(AFX_IMAGE_H__F545D0D5_5F10_4EFA_BE3B_3F3D34D4DBF3__INCLUDED_)
	#include "System/Image.h"
#endif

RAPTOR_NAMESPACE_BEGIN


class RAPTOR_API CMipmapBuilder : public CImage::IImageOP
{
public:
	CMipmapBuilder();
	virtual ~CMipmapBuilder();
  
    //! Implements CImageOP
	virtual OP_KIND getKind(void) const { return CImage::IImageOP::MIPMAP_BUILDER; };

    //! Implaments CImageOP
    virtual bool apply(	ITextureObject* const src,
						unsigned int innerFormat,
						unsigned int texelFormat,
						unsigned int texelType,
						const CTextureFactoryConfig& config) const;
};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_DEFAULTMIPMAPBUILDER_H__9C508D96_B614_4920_8816_B670295B6CE2__INCLUDED_)


