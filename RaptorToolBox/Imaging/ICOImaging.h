// ICOImaging.h: interface for the CICOImaging class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ICOIMAGING_H__6E2B8113_3111_4770_97A8_A786982CF05F__INCLUDED_)
#define AFX_ICOIMAGING_H__6E2B8113_3111_4770_97A8_A786982CF05F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if !defined(AFX_TEXTUREFACTORYCONFIG_H__7A20D208_423F_4E02_AA4D_D736E0A7959F__INCLUDED_)
	#include "GLHierarchy/TextureFactoryConfig.h"
#endif


RAPTOR_NAMESPACE

class CICOImaging :	public CTextureFactoryConfig::CImageIO
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



