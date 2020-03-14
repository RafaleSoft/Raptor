// OpenEXRImaging.h: interface for the COpenEXRImaging class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OPENEXRIMAGING_H__A2B2F9B8_4DA0_4D06_AD64_40805C2A14C3__INCLUDED_)
#define AFX_OPENEXRIMAGING_H__A2B2F9B8_4DA0_4D06_AD64_40805C2A14C3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if !defined(AFX_IMAGE_H__F545D0D5_5F10_4EFA_BE3B_3F3D34D4DBF3__INCLUDED_)
	#include "System/Image.h"
#endif


RAPTOR_NAMESPACE

class COpenEXRImaging :	public CImage::IImageIO
{
public:
	COpenEXRImaging(void);
	virtual ~COpenEXRImaging(void);

	virtual bool isOfKind(const std::string &kind) const;

	virtual std::vector<std::string> getImageKind(void) const;

	virtual bool storeImageFile(const std::string& fname, CImage* const I) const;

	//! Method prototype for image loading 'from file'
	//!	@param fname : full filename, with path and file extensions
	//! @param I : a valid image object.
	//! @eturn true if loading is successfull.
	virtual bool loadImageFile(const std::string& fname, CImage* const I) const;
};

#endif	// !defined(AFX_OPENEXRIMAGING_H__A2B2F9B8_4DA0_4D06_AD64_40805C2A14C3__INCLUDED_)

