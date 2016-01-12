// DefaultImageScaler.h: interface for the CDefaultImageScaler class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DEFAULTIMAGESCALER_H__E3E63A13_79FC_4E46_A1D5_BCD41CF86360__INCLUDED_)
#define AFX_DEFAULTIMAGESCALER_H__E3E63A13_79FC_4E46_A1D5_BCD41CF86360__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if !defined(AFX_TEXTUREFACTORYCONFIG_H__7A20D208_423F_4E02_AA4D_D736E0A7959F__INCLUDED_)
    #include "GLHierarchy/TextureFactoryConfig.h"
#endif

RAPTOR_NAMESPACE_BEGIN

class CTextureObject;

class CDefaultImageScaler : public CTextureFactoryConfig::IImageOP  
{
public:
	CDefaultImageScaler();
	virtual ~CDefaultImageScaler();

    //! Implements CImageOP
    virtual OP_KIND getKind(void) const { return IMAGE_SCALER; };

    //! Implaments CImageOP
    virtual bool apply(	CTextureObject* const src,
						unsigned int innerFormat,
						unsigned int texelFormat,
						unsigned int texelType,
						const CTextureFactoryConfig& config) const;
};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_DEFAULTIMAGESCALER_H__E3E63A13_79FC_4E46_A1D5_BCD41CF86360__INCLUDED_)


