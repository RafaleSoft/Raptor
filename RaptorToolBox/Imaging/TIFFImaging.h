// TIFFImaging.h: interface for the CTIFFImaging class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TIFFIMAGING_H__3AD77410_776F_446A_860E_496C8D13CB0F__INCLUDED_)
#define AFX_TIFFIMAGING_H__3AD77410_776F_446A_860E_496C8D13CB0F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if !defined(AFX_IMAGE_H__F545D0D5_5F10_4EFA_BE3B_3F3D34D4DBF3__INCLUDED_)
	#include "System/Image.h"
#endif


RAPTOR_NAMESPACE

class CTIFFImaging : public CImage::IImageIO
{
public:
	CTIFFImaging(void);
	virtual ~CTIFFImaging(void);

	virtual bool isOfKind(const std::string &kind) const;

	virtual vector<std::string> getImageKind(void) const;

	virtual bool storeImageFile(const std::string& fname, CImage* const I) const;

	//! Method prototype for image loading 'from file'
	//!	@param fname : full filename, with path and file extensions
	//! @param I : a valid image object.
	//! @eturn true if loading is successfull.
	virtual bool loadImageFile(const std::string& fname, CImage* const I) const;
};

#endif	// !defined(AFX_TIFFIMAGING_H__3AD77410_776F_446A_860E_496C8D13CB0F__INCLUDED_)



