// ICOImaging.h: interface for the CICOImaging class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ICOIMAGING_H__6E2B8113_3111_4770_97A8_A786982CF05F__INCLUDED_)
#define AFX_ICOIMAGING_H__6E2B8113_3111_4770_97A8_A786982CF05F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if !defined(AFX_IMAGE_H__F545D0D5_5F10_4EFA_BE3B_3F3D34D4DBF3__INCLUDED_)
	#include "System/Image.h"
#endif


RAPTOR_NAMESPACE

class CICOImaging :	public CImage::IImageIO
{
public:
	CICOImaging(void);
	virtual ~CICOImaging(void);

	virtual bool isOfKind(const std::string &kind) const;

	virtual vector<std::string> getImageKind(void) const;

    virtual bool loadImageFile(const std::string& fname,CTextureObject* const T);

    virtual bool storeImageFile(const std::string& fname,CTextureObject* const T);
};

#endif	// !defined(AFX_ICOIMAGING_H__6E2B8113_3111_4770_97A8_A786982CF05F__INCLUDED_)



