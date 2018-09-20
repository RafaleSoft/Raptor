// DefaultBumpmapLoader.h: interface for the CDefaultBumpmapLoader class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DEFAULTBUMPMAPLOADER_H__3841D5F8_284B_4DC5_9E4B_56EF18AF80F4__INCLUDED_)
#define AFX_DEFAULTBUMPMAPLOADER_H__3841D5F8_284B_4DC5_9E4B_56EF18AF80F4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if !defined(AFX_IMAGE_H__F545D0D5_5F10_4EFA_BE3B_3F3D34D4DBF3__INCLUDED_)
	#include "System/Image.h"
#endif


RAPTOR_NAMESPACE_BEGIN


class CDefaultBumpmapLoader : public CImage::IImageOP
{
public:
    CDefaultBumpmapLoader(float scale);
	CDefaultBumpmapLoader(const CDefaultBumpmapLoader& loader);
	CDefaultBumpmapLoader& operator=(const CDefaultBumpmapLoader& loader);

	virtual ~CDefaultBumpmapLoader();

    //! Implements CImageOP
    virtual OP_KIND getKind(void) const { return BUMPMAP_LOADER; };

	//! Implements CImageOP
	virtual bool apply(CImage* const src, const operation_param_t& param) const;

private:
	CDefaultBumpmapLoader();

	//!	Bumping amplitude.
	float bump_scale;
};


RAPTOR_NAMESPACE_END

#endif // !defined(AFX_DEFAULTBUMPMAPLOADER_H__3841D5F8_284B_4DC5_9E4B_56EF18AF80F4__INCLUDED_)


