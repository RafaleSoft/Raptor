// TGAImaging.h: interface for the CTGAImaging class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TGAIMAGING_H__EF9F065C_C2BE_4C41_BD50_3DA107B5050B__INCLUDED_)
#define AFX_TGAIMAGING_H__EF9F065C_C2BE_4C41_BD50_3DA107B5050B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if !defined(AFX_IMAGE_H__F545D0D5_5F10_4EFA_BE3B_3F3D34D4DBF3__INCLUDED_)
	#include "System/Image.h"
#endif


RAPTOR_NAMESPACE

class CTGAImaging :	public CImage::IImageIO
{
public:
	CTGAImaging(void);
	virtual ~CTGAImaging(void);

	virtual bool isOfKind(const std::string &kind) const;

	virtual vector<std::string> getImageKind(void) const;

	virtual bool storeImageFile(const std::string& fname, CImage* const I) const;

	//! Method prototype for image loading 'from file'
	//!	@param fname : full filename, with path and file extensions
	//! @param I : a valid image object.
	//! @eturn true if loading is successfull.
	virtual bool loadImageFile(const std::string& fname, CImage* const I) const;
};

#endif	// !defined(AFX_TGAIMAGING_H__EF9F065C_C2BE_4C41_BD50_3DA107B5050B__INCLUDED_)



