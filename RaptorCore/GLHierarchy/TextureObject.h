// TextureObject.h: interface for the CTextureObject class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TEXTUREOBJECT_H__D32B6294_B42B_4E6F_AB73_13B33C544AD0__INCLUDED_)
#define AFX_TEXTUREOBJECT_H__D32B6294_B42B_4E6F_AB73_13B33C544AD0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __CGLTYPES_HPP__
	#include "System/CGLTypes.h"
#endif
#if !defined(AFX_ITEXTUREOBJECT_H__3AA8C89E_BB23_483C_A547_C8A4CC53E551__INCLUDED_)
	#include "ITextureObject.h"
#endif


RAPTOR_NAMESPACE_BEGIN

class CTextureFactory;
class CTextureUnitSetup;
class ITextureGenerator;


//	Base structure for CTexture class
//	this structure defines a texture object
//	and its degenerate form : a sprite
class RAPTOR_API CTextureObject : public ITextureObject
{
public:
	//! Texel transfer function ( combines input fragment with texel extracted from sampler )
	typedef enum
	{
		CGL_OPAQUE,
		CGL_MULTIPLY,
		CGL_ALPHA_TRANSPARENT,
		CGL_CONSTANT_BLENDED
	} TEXTURE_FUNCTION;

	typedef enum
	{
		CGL_CUBEMAP_PX,
		CGL_CUBEMAP_NX,
		CGL_CUBEMAP_PY,
		CGL_CUBEMAP_NY,
		CGL_CUBEMAP_PZ,
		CGL_CUBEMAP_NZ,
		CGL_CUBEMAP_NONE
	} CUBE_FACE;


public:
    //!	Renders the textures : it is bound to the current active Texture Unit.
	virtual void glvkRender(void);

	//!	Returns the proper implementation
	virtual CTextureObject* getGLTextureObject(void) { return this; };

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
    unsigned int    getCurrentMipMapLevel(void) const { return level; }

    //! Selects the current mipmap level for image access ( loading, reading, ... )
    void selectMipMapLevel(unsigned int l) { level = l; };
    
	//! Returns the actual selected cubemap face.
    //! ( not the number of faces already loaded )
    CUBE_FACE    getCurrentCubeFace(void) const;

    //! Selects the current cubemap face for image access ( loading, reading, ... )
    void selectCubeFace(CUBE_FACE face);

    //! Define the dimensions ( of the generator ) for texture generation.
    //! Actual parameters are modified if requested generation size is too large for the texture
    //! or if position is not valid within the generator. Texture must have a valid size.
    //! @return : false is this texture is not generated.
    bool setGenerationSize(   int posx, int posy, unsigned int width, unsigned int height);

    //! Returns the generation dimensions set here above ( or default )
    void getGenerationSize(   int &posx, int &posy, int &width, int &height) const;

	//!	Returns the sized format of the texels stored in device memory (texture)
	unsigned int getTexelFormat(void) const;

    
    //! This method set texture alpha for transparency management.
    //! if 0, use color average, if > 255, saturate to 0 and 255, else use alpha
    void glSetTransparency(unsigned int	alpha);

    //!	Set a specific color as transparent. Any number of colors can be transparent,
	//!	they are changed through their alpha component
    //! This method only applyes to 2D textures ; future versions will handle more cases if needed.
	void glSetTransparentColor(  unsigned char r, unsigned char g, unsigned char b);

	//!	Returns the texture generator, if any. NULL otherwise.
	ITextureGenerator * getTexelGenerator(void) const { return m_pTexelGenerator; };


private:
	//!
	//!	Forbidden methods
	//!
	CTextureObject(ITextureObject::TEXEL_TYPE type);
	CTextureObject(const CTextureObject& rsh);
	virtual ~CTextureObject();

	//! texture objects cannot be assigned.
	const CTextureObject& operator=(const CTextureObject &rsh) { return *this; };

    //! This call is restricted to the factory or the TMUSetup
    void setFunction(TEXTURE_FUNCTION F);
	

	friend class CTextureFactory;
	friend class CTextureUnitSetup;
	friend class CRaptorRenderBufferDisplay;
 
	//!
	//!	Attributes
	//!

    
    //!	OpenGL context based texture name
	unsigned int	texname;
    //!  Target : 1D,2D,3D,CUBE, 
	unsigned int	target;
    //!	rendering environment model
	unsigned int    env_mode;
    //	texture level ( mipmapping and anisotropy)
	unsigned int	level;

    //!	Render target used to provide texImage
	ITextureGenerator  *m_pTexelGenerator;

    //! generation source dimensions
    int source[4];
};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_TEXTUREOBJECT_H__D32B6294_B42B_4E6F_AB73_13B33C544AD0__INCLUDED_)

