// TGAImaging.h: interface for the CTGAImaging class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TGAIMAGING_H__EF9F065C_C2BE_4C41_BD50_3DA107B5050B__INCLUDED_)
#define AFX_TGAIMAGING_H__EF9F065C_C2BE_4C41_BD50_3DA107B5050B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if !defined(AFX_TEXTUREFACTORYCONFIG_H__7A20D208_423F_4E02_AA4D_D736E0A7959F__INCLUDED_)
	#include "GLHierarchy/TextureFactoryConfig.h"
#endif


RAPTOR_NAMESPACE

class CTGAImaging :	public CTextureFactoryConfig::CImageIO
{
public:
	CTGAImaging(void);
	virtual ~CTGAImaging(void);

	virtual bool isOfKind(const std::string &kind) const;

	virtual vector<std::string> getImageKind(void) const;

    virtual bool loadImageFile(const std::string& fname,CTextureObject* const T);

    virtual bool storeImageFile(const std::string& fname,CTextureObject* const T);
};

#endif	// !defined(AFX_TGAIMAGING_H__EF9F065C_C2BE_4C41_BD50_3DA107B5050B__INCLUDED_)



