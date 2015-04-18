// DefaultMipmapBuilder.h: interface for the CDefaultMipmapBuilder class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DEFAULTMIPMAPBUILDER_H__9C508D96_B614_4920_8816_B670295B6CE2__INCLUDED_)
#define AFX_DEFAULTMIPMAPBUILDER_H__9C508D96_B614_4920_8816_B670295B6CE2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if !defined(AFX_TEXTUREFACTORYCONFIG_H__7A20D208_423F_4E02_AA4D_D736E0A7959F__INCLUDED_)
    #include "GLHierarchy/TextureFactoryConfig.h"
#endif

RAPTOR_NAMESPACE_BEGIN

class CTextureObject;

class CDefaultMipmapBuilder : public CTextureFactoryConfig::CImageOP  
{
public:
	CDefaultMipmapBuilder();
	virtual ~CDefaultMipmapBuilder();
  
    //! Implements CImageOP
    virtual OP_KIND getKind(void) const { return MIPMAP_BUILDER; };

    //! Implaments CImageOP
    virtual bool apply(	CTextureObject* const src,
						unsigned int innerFormat,
						unsigned int texelFormat,
						unsigned int texelType,
						const CTextureFactoryConfig& config) const;
};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_DEFAULTMIPMAPBUILDER_H__9C508D96_B614_4920_8816_B670295B6CE2__INCLUDED_)


