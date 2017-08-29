// JPGImaging.h: interface for the CJPGImaging class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_JPGIMAGING_H__E268D39E_46E2_4CE0_B8C7_D5C3DE2B5F91__INCLUDED_)
#define AFX_JPGIMAGING_H__E268D39E_46E2_4CE0_B8C7_D5C3DE2B5F91__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if !defined(AFX_IMAGE_H__F545D0D5_5F10_4EFA_BE3B_3F3D34D4DBF3__INCLUDED_)
	#include "System/Image.h"
#endif


RAPTOR_NAMESPACE

class CJPGImaging :	public CImage::IImageIO
{
public:
	CJPGImaging(void);
	virtual ~CJPGImaging(void);

	virtual bool isOfKind(const std::string &kind) const;

	virtual vector<std::string> getImageKind(void) const;

    virtual bool loadImageFile(const std::string& fname,CTextureObject* const T);

    virtual bool storeImageFile(const std::string& fname,CTextureObject* const T);
};

#endif	// !defined(AFX_JPGIMAGING_H__E268D39E_46E2_4CE0_B8C7_D5C3DE2B5F91__INCLUDED_)



