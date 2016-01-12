// DefaultBumpmapLoader.h: interface for the CDefaultBumpmapLoader class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DEFAULTBUMPMAPLOADER_H__3841D5F8_284B_4DC5_9E4B_56EF18AF80F4__INCLUDED_)
#define AFX_DEFAULTBUMPMAPLOADER_H__3841D5F8_284B_4DC5_9E4B_56EF18AF80F4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if !defined(AFX_TEXTUREFACTORYCONFIG_H__7A20D208_423F_4E02_AA4D_D736E0A7959F__INCLUDED_)
    #include "GLHierarchy/TextureFactoryConfig.h"
#endif


RAPTOR_NAMESPACE_BEGIN

class CTextureObject;

class CDefaultBumpmapLoader : public CTextureFactoryConfig::IImageOP  
{
public:
    CDefaultBumpmapLoader();
	virtual ~CDefaultBumpmapLoader();

    //! Implements CImageOP
    virtual OP_KIND getKind(void) const { return BUMPMAP_LOADER; };

    //! Implaments CImageOP
    virtual bool apply(	CTextureObject* const src,
						unsigned int innerFormat,
						unsigned int texelFormat,
						unsigned int texelType,
						const CTextureFactoryConfig& config) const;
};


RAPTOR_NAMESPACE_END

#endif // !defined(AFX_DEFAULTBUMPMAPLOADER_H__3841D5F8_284B_4DC5_9E4B_56EF18AF80F4__INCLUDED_)


