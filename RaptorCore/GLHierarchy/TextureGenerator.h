// TextureGenerator.h: interface for the CTextureGenerator class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TEXTUREGENERATOR_H__214CB62C_FE2E_4737_9EA8_CE5D97F16093__INCLUDED_)
#define AFX_TEXTUREGENERATOR_H__214CB62C_FE2E_4737_9EA8_CE5D97F16093__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


RAPTOR_NAMESPACE_BEGIN


class ITextureObject;

//! This class defines the base class for all classes of texture generators:
//! buffered, animated, static ... texture generators
class RAPTOR_API ITextureGenerator  
{
public:
    typedef enum
    {
        NONE,
        BUFFERED,
        ANIMATED,
        STATIC
    } GENERATOR_KIND;

    ITextureGenerator():m_bEnabled(true) {};

    virtual ~ITextureGenerator() {};

    //! Enable or disable texel generation
    void enableGenerator(bool enable) { m_bEnabled = enable; };

    //! Returns the kind of render target texture generator.
    virtual GENERATOR_KIND    getKind(void) const = 0;

    //! This method must be implemented in subclasses to produce 
    //! an array of texels and load the currently bound texture object.
    //! To get texture parameters, the currently bound texture object is passed in
    virtual void glGenerate(ITextureObject* ) = 0;

    //! This method returns the width of the generator
    virtual unsigned int getGenerateWidth(void) const = 0;

    //! This method returns the height of the generator
    virtual unsigned int getGenerateHeight(void) const = 0;


protected:
    bool    m_bEnabled;
};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_TEXTUREGENERATOR_H__214CB62C_FE2E_4737_9EA8_CE5D97F16093__INCLUDED_)

