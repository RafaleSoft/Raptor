// BMPImaging.h: interface for the CBMPImaging class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BMPIMAGING_H__C9C42555_D3BF_42BE_8CC2_FA35410B79AD__INCLUDED_)
#define AFX_BMPIMAGING_H__C9C42555_D3BF_42BE_8CC2_FA35410B79AD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if !defined(AFX_IMAGE_H__F545D0D5_5F10_4EFA_BE3B_3F3D34D4DBF3__INCLUDED_)
	#include "System/Image.h"
#endif

RAPTOR_NAMESPACE

class CBMPImaging :	public CImage::IImageIO
{
public:
	CBMPImaging(void);
	virtual ~CBMPImaging(void);

	virtual bool isOfKind(const std::string &kind) const;

	virtual vector<std::string> getImageKind(void) const;

    virtual bool loadImageFile(const std::string& fname,CTextureObject* const T);

    virtual bool storeImageFile(const std::string& fname,CTextureObject* const T);
};

#endif	// !defined(AFX_BMPIMAGING_H__C9C42555_D3BF_42BE_8CC2_FA35410B79AD__INCLUDED_)



