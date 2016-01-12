// OpenEXRImaging.h: interface for the COpenEXRImaging class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OPENEXRIMAGING_H__A2B2F9B8_4DA0_4D06_AD64_40805C2A14C3__INCLUDED_)
#define AFX_OPENEXRIMAGING_H__A2B2F9B8_4DA0_4D06_AD64_40805C2A14C3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if !defined(AFX_TEXTUREFACTORYCONFIG_H__7A20D208_423F_4E02_AA4D_D736E0A7959F__INCLUDED_)
	#include "GLHierarchy/TextureFactoryConfig.h"
#endif


RAPTOR_NAMESPACE

class COpenEXRImaging :	public CTextureFactoryConfig::IImageIO
{
public:
	COpenEXRImaging(void);
	virtual ~COpenEXRImaging(void);

	virtual bool isOfKind(const std::string &kind) const;

	virtual vector<std::string> getImageKind(void) const;

    virtual bool loadImageFile(const std::string& fname,CTextureObject* const T);

    virtual bool storeImageFile(const std::string& fname,CTextureObject* const T);
};

#endif	// !defined(AFX_OPENEXRIMAGING_H__A2B2F9B8_4DA0_4D06_AD64_40805C2A14C3__INCLUDED_)

