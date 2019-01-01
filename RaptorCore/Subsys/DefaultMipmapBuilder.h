// DefaultMipmapBuilder.h: interface for the CDefaultMipmapBuilder class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DEFAULTMIPMAPBUILDER_H__9C508D96_B614_4920_8816_B670295B6CE2__INCLUDED_)
#define AFX_DEFAULTMIPMAPBUILDER_H__9C508D96_B614_4920_8816_B670295B6CE2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if !defined(AFX_IMAGE_H__F545D0D5_5F10_4EFA_BE3B_3F3D34D4DBF3__INCLUDED_)
	#include "System/Image.h"
#endif

RAPTOR_NAMESPACE_BEGIN


class CDefaultMipmapBuilder : public CImage::IImageOP
{
public:
	CDefaultMipmapBuilder();
	virtual ~CDefaultMipmapBuilder();
  
    //! Implements CImageOP
    virtual OP_KIND getKind(void) const { return MIPMAP_BUILDER; };

    //! Implaments CImageOP
    virtual bool apply(	CTextureObject* const src,
						unsigned int innerFormat,
						unsigned int texelFormat,
						unsigned int texelType,
						const CTextureFactoryConfig& config) const;
};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_DEFAULTMIPMAPBUILDER_H__9C508D96_B614_4920_8816_B670295B6CE2__INCLUDED_)


