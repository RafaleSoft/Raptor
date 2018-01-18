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
#if !defined(AFX_OBJECTREFERENCE_H__0D47C721_2B2D_4163_AB88_BE1B4E08A84D__INCLUDED_)
    #include "ObjectReference.h"
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
class RAPTOR_API CTextureObject : public CObjectReference, public ITextureObject
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

	//! Texel sampler filter
	typedef enum
	{
		CGL_UNFILTERED,
		CGL_BILINEAR,
		CGL_BILINEAR_MIPMAPPED,
		CGL_TRILINEAR,
		CGL_ANISOTROPIC
	} TEXTURE_FILTER;

	//!	Server-side texel type
    typedef enum
    {
        CGL_COLOR24,
        CGL_COLOR24_ALPHA,
        CGL_LIGHTMAP,
        CGL_LIGHTMAP_ALPHA,
		CGL_LIGHTMAP16,
        CGL_LIGHTMAP16_ALPHA,
        CGL_COLOR_FLOAT16,
        CGL_COLOR_FLOAT16_ALPHA,
        CGL_COLOR_FLOAT32,
        CGL_COLOR_FLOAT32_ALPHA,
        CGL_DEPTH8,
        CGL_DEPTH16,
        CGL_DEPTH24,
        CGL_DEPTH32,
		CGL_DEPTH24_STENCIL8
    } TEXEL_TYPE;

	//! Texture sampler clampping model
	typedef enum
	{
		CGL_REPEAT,
		CGL_CLAMP,
		CGL_EDGECLAMP
	} CLAMP_MODE;

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

    //! @return texture name ( default is the source filename )
	const std::string & getName(void) const { return m_name; };

	//! @param name: the new texture name
	void setName(const std::string & name) { m_name = name; };

    //! Returns the selected environment function
    TEXTURE_FUNCTION getFunction(void) const;

	//! Updates texture filtering function
	//! @param F : the filter function
    void glUpdateFilter(TEXTURE_FILTER F);

    //! Returns the selected filtering method
	TEXTURE_FILTER getFilter(void) const { return m_filter; };

    //! Returns the texture texel type for internal storage
    TEXEL_TYPE getTexelType(void) const { return m_type; };

	//! Updates texture sampler clamping mode. This method
	//!	is a simple helper, clamping mode is applied equally 
	//!	to each dimension.
	//! @param C : the clamp mode
    void glUpdateClamping(CLAMP_MODE C);


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

    //! Return texture global transparency.
    //! @ return : the value set using the method glSetTransparency
    unsigned int	getTransparency(void) const { return m_alpha; };
	
    //! This method set texture alpha for transparency management.
    //! if 0, use color average, if > 255, saturate to 0 and 255, else use alpha
    void glSetTransparency(unsigned int	alpha);

    //!	Set a specific color as transparent. Any number of colors can be transparent,
	//!	they are changed through their alpha component
    //! This method only applyes to 2D textures ; future versions will handle more cases if needed.
	void glSetTransparentColor(  unsigned char r, unsigned char g, unsigned char b);




private:
	//!
	//!	Forbidden methods
	//!
	CTextureObject(TEXEL_TYPE type);
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
    
	//! The texture texel type is stored only to avoid server calls to query internal format
    TEXEL_TYPE      m_type;

    //! Aplha value. It is stored to be applyed before or after texture loading.
    unsigned int	m_alpha;

    //!	Object name ( default is filename )
	string			m_name;
    //!	OpenGL context based texture name
	unsigned int	texname;
    //!  Target : 
	//!		low word = 1D,2D,3D,CUBE, 
	//!		hi word = subtarget
	unsigned int	target;
    //!	rendering environment model
	unsigned int    env_mode;
    //	texture level ( mipmapping and anisotropy)
	unsigned int	level;

	TEXTURE_FILTER	m_filter;

    //! Anisotropy level
    float           aniso_level;

    //!	Render target used to provide texImage
	ITextureGenerator  *m_pTexelGenerator;

    //! generation source dimensions
    int source[4];

	//!	Handle to vulkan image	
	VkImage			vk_texname;
};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_TEXTUREOBJECT_H__D32B6294_B42B_4E6F_AB73_13B33C544AD0__INCLUDED_)

