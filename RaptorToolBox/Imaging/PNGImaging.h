// PNGImaging.h: interface for the CPNGImaging class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PNGIMAGING_H__73E7FD82_0C63_4CB0_BFEC_33475DAABE35__INCLUDED_)
#define AFX_PNGIMAGING_H__73E7FD82_0C63_4CB0_BFEC_33475DAABE35__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if !defined(AFX_IMAGE_H__F545D0D5_5F10_4EFA_BE3B_3F3D34D4DBF3__INCLUDED_)
	#include "System/Image.h"
#endif


RAPTOR_NAMESPACE

class CPNGImaging :	public CImage::IImageIO
{
public:
	CPNGImaging(void);
	virtual ~CPNGImaging(void);

	virtual bool isOfKind(const std::string &kind) const;

	virtual vector<std::string> getImageKind(void) const;

    virtual bool loadImageFile(const std::string& fname,CTextureObject* const T);

    virtual bool storeImageFile(const std::string& fname,CTextureObject* const T);
};

#endif	// !defined(AFX_PNGIMAGING_H__73E7FD82_0C63_4CB0_BFEC_33475DAABE35__INCLUDED_)

