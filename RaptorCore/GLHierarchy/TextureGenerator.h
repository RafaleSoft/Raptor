/***************************************************************************/
/*                                                                         */
/*  ITextureGenerator.h                                                    */
/*                                                                         */
/*    Raptor OpenGL & Vulkan realtime 3D Engine SDK.                       */
/*                                                                         */
/*  Copyright 1998-2021 by                                                 */
/*  Fabrice FERRAND.                                                       */
/*                                                                         */
/*  This file is part of the Raptor project, and may only be used,         */
/*  modified, and distributed under the terms of the Raptor project        */
/*  license, LICENSE.  By continuing to use, modify, or distribute         */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/


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
    virtual GENERATOR_KIND	getKind(void) const = 0;

    //! This method must be implemented in subclasses to produce 
    //! an array of texels and load the currently bound texture object.
    //! @param I: the texture into which the generator will update texels.
	//! @param x: the horizontal offset of top left corner for the bounding rectangle to update.
	//! @param y: the vertical offset of top left corner for the bounding rectangle to update.
	//! @param w: the width of the region to update, starting from top left corner.
	//! @param h: the height of the region to update, starting from top left corner.
    virtual void glGenerate(ITextureObject* I, uint32_t x, uint32_t y, uint32_t w, uint32_t h) = 0;

    //! This method returns the maximum width of the generator
    virtual unsigned int getGenerateWidth(void) const = 0;

    //! This method returns the maximum height of the generator
    virtual unsigned int getGenerateHeight(void) const = 0;


protected:
    bool    m_bEnabled;
};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_TEXTUREGENERATOR_H__214CB62C_FE2E_4737_9EA8_CE5D97F16093__INCLUDED_)

