// TextureObject.h: interface for the CTextureObject class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TEXTUREOBJECT_H__D32B6294_B42B_4E6F_AB73_13B33C544AD0__INCLUDED_)
#define AFX_TEXTUREOBJECT_H__D32B6294_B42B_4E6F_AB73_13B33C544AD0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if !defined(AFX_OBJECTREFERENCE_H__0D47C721_2B2D_4163_AB88_BE1B4E08A84D__INCLUDED_)
    #include "ObjectReference.h"
#endif


RAPTOR_NAMESPACE_BEGIN

class CTextureFactory;
class CTextureUnitSetup;
class CTextureGenerator;


//	Base structure for CTexture class
//	this structure defines a texture object
//	and its degenerate form : a sprite
class RAPTOR_API CTextureObject : public CObjectReference
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
	void glRender(void);

    //! texture name ( default is the source filename )
	const std::string & getName(void) const { return m_name; };

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


    //! Defines the size of the texel array ( texel wrapper ) for texture loading/updates/reading.
    //! Note that the server texture object will not be modified, only the factory is allowed
    //! to proceed a GL resize. If texels have been allocated and the new size do not match,
    //! texels are lost and the wrapper is reallocated.
    //! This method must be used with allocateTexels, any other usage is not supported.
    void setSize(unsigned int width, unsigned int height, unsigned int depth=0);

    //! Allocates a bloc of texels to pass data to server. Memory allocator must be in library.
    //! The size is dependent on texture attributes, it might be reallocated or not.
    void allocateTexels(TEXEL_TYPE type = CGL_COLOR24_ALPHA);

	//! Releases any allocated texel blocs.
    void releaseTexels(void);

    //! Define the dimensions ( of the generator ) for texture generation.
    //! Actual parameters are modified if requested generation size is too large for the texture
    //! or if position is not valid within the generator. Texture must have a valid size.
    //! @return : false is this texture is not generated.
    bool setGenerationSize(   int posx, int posy, unsigned int width, unsigned int height);

    //! Returns the generation dimensions set here above ( or default )
    void getGenerationSize(   int &posx, int &posy, int &width, int &height) const;

    //! Request texture object texel data access.
    //! The pointer returned can be used to manually upload texels.
    //! After the texture has been uploaded to server-side, the returned pointer
    //! will always be NULL except if a reallocation has been issued.
    //! @return : the returned pointer can be NULL because the type do not match even is there is data to request.
	//! ( i.e. data has been allocated using allocateTexels )
    unsigned char* getTexels(void);

    //!	Same as above, except that the requested type of texels is different.
    float* getFloatTexels(void);

    //! Return texture width
    unsigned int	getWidth(void) const { return m_width; };

    //! Return texture height
    unsigned int	getHeight(void) const { return m_height; };

    //! Return texture depth : assume it is a volumetric texture.
    unsigned int	getDepth(void) const { return m_depth; };

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

	//! A helper to select texel format from source buffer ( allocated for user's needs )
	unsigned int getBufferTexelType(void) const;
	unsigned int getBufferType(void) const;
	unsigned int getSizedTexelType(void) const;


	friend class CTextureFactory;
	friend class CTextureUnitSetup;
	friend class CRaptorRenderBufferDisplay;
 
	//!
	//!	Attributes
	//!

    //! Texture dimensions
    unsigned int	m_width;
	unsigned int	m_height;
    unsigned int	m_depth;

    //! The texel type is stored only to avoid server calls to query internal format
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
	CTextureGenerator  *m_pTexelGenerator;

    //! generation source dimensions
    int source[4];

    //  Temporary data : might be null after texture loading to release client memory
    TEXEL_TYPE  m_bufferType;
    void *texels;
};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_TEXTUREOBJECT_H__D32B6294_B42B_4E6F_AB73_13B33C544AD0__INCLUDED_)

