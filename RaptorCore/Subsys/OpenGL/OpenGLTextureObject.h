/***************************************************************************/
/*                                                                         */
/*  OpenGLTextureObject.h                                                  */
/*                                                                         */
/*    Raptor OpenGL & Vulkan realtime 3D Engine SDK.                       */
/*                                                                         */
/*  Copyright 1998-2019 by                                                 */
/*  Fabrice FERRAND.                                                       */
/*                                                                         */
/*  This file is part of the Raptor project, and may only be used,         */
/*  modified, and distributed under the terms of the Raptor project        */
/*  license, LICENSE.  By continuing to use, modify, or distribute         */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/


#if !defined(AFX_OPENGLTEXTUREOBJECT_H__D32B6294_B42B_4E6F_AB73_13B33C544AD0__INCLUDED_)
#define AFX_OPENGLTEXTUREOBJECT_H__D32B6294_B42B_4E6F_AB73_13B33C544AD0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __CGLTYPES_HPP__
	#include "System/CGLTypes.h"
#endif
#if !defined(AFX_ITEXTUREOBJECT_H__3AA8C89E_BB23_483C_A547_C8A4CC53E551__INCLUDED_)
	#include "GLHierarchy/ITextureObject.h"
#endif


RAPTOR_NAMESPACE_BEGIN

class CTextureFactory;
class CTextureUnitSetup;
class ITextureGenerator;


//	Base structure for OpenGL texture class
//	this structure defines a texture object
//	and its degenerate form : a sprite
class RAPTOR_API COpenGLTextureObject : public ITextureObject
{
public:
    //!	Renders the textures : it is bound to the current active Texture Unit.
	virtual void glvkRender(void);

	//!	Returns the proper implementation
	virtual COpenGLTextureObject* getGLTextureObject(void) { return this; };

    //! Returns the selected environment function
    TEXTURE_FUNCTION getFunction(void) const;

	//! Updates texture filtering function
	//! @param F : the filter function
	virtual void glvkUpdateFilter(ITextureObject::TEXTURE_FILTER F);

	//! Updates texture sampler clamping mode. This method
	//!	is a simple helper, clamping mode is applied equally 
	//!	to each dimension.
	//! @param C : the clamp mode
	virtual void glvkUpdateClamping(ITextureObject::CLAMP_MODE C);


    //! Returns the actual selected mipmap.
    //! ( not the number of mipmaps )
    virtual uint32_t getCurrentMipMapLevel(void) const { return level; }

    //! Selects the current mipmap level for image access ( loading, reading, ... )
    virtual void selectMipMapLevel(unsigned int l) { level = l; };
    
	//! Returns the actual selected cubemap face.
    //! ( not the number of faces already loaded )
    virtual CUBE_FACE getCurrentCubeFace(void) const;

    //! Selects the current cubemap face for image access ( loading, reading, ... )
    virtual void selectCubeFace(CUBE_FACE face);

    //! Define the dimensions ( of the generator ) for texture generation.
    //! Actual parameters are modified if requested generation size is too large for the texture
    //! or if position is not valid within the generator. Texture must have a valid size.
    //! @return : false is this texture is not generated.
    virtual bool setGenerationSize(uint32_t posx, uint32_t posy, uint32_t width, uint32_t height);

	//!	Returns the sized format of the texels stored in device memory (texture)
	unsigned int getTexelFormat(void) const;
	
	//!	Returns the texture generator, if any. NULL otherwise.
	ITextureGenerator * getTexelGenerator(void) const { return m_pTexelGenerator; };


private:
	//!
	//!	Forbidden methods
	//!
	COpenGLTextureObject(ITextureObject::TEXEL_TYPE type);
	COpenGLTextureObject(const COpenGLTextureObject& rsh);
	virtual ~COpenGLTextureObject();

	//! texture objects cannot be assigned.
	const COpenGLTextureObject& operator=(const COpenGLTextureObject &rsh) { return *this; };

    //! This call is restricted to the factory or the TMUSetup
    void setFunction(ITextureObject::TEXTURE_FUNCTION F);
	

	friend class CTextureFactory;
	friend class CTextureUnitSetup;
	friend class CRaptorRenderBufferDisplay;
 
	//!
	//!	Attributes
	//!
    
    //!	OpenGL context based texture name
	uint32_t	texname;
    //!  Target : 1D,2D,3D,CUBE, 
	uint32_t	target;
    //!	rendering environment model
	uint32_t    env_mode;
    //	texture level ( mipmapping and anisotropy)
	uint32_t	level;

    //!	Render target used to provide texImage
	ITextureGenerator  *m_pTexelGenerator;

    //! generation source dimensions
    uint32_t source[4];
};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_OPENGLTEXTUREOBJECT_H__D32B6294_B42B_4E6F_AB73_13B33C544AD0__INCLUDED_)

