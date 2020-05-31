// Texture.h: interface for the CTexture class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TEXTUREFACTORY_H__1B470EC4_4B68_11D3_9142_9A502CBADC6B__INCLUDED_)
#define AFX_TEXTUREFACTORY_H__1B470EC4_4B68_11D3_9142_9A502CBADC6B__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#if defined(_WIN32)
	#pragma warning(disable:4786)
#endif


#ifndef __CGLTYPES_HPP__
	#include "System/CGLTypes.h"
#endif
#if !defined(AFX_PERSISTENCE_H__5561BA28_831B_11D3_9142_EEB51CEBBDB0__INCLUDED_)
	#include "GLHierarchy/Persistence.h"
#endif
#if !defined(AFX_ITEXTUREOBJECT_H__3AA8C89E_BB23_483C_A547_C8A4CC53E551__INCLUDED_)
#include "GLHierarchy/ITextureObject.h"
#endif
#if !defined(AFX_TEXTUREFACTORYCONFIG_H__7A20D208_423F_4E02_AA4D_D736E0A7959F__INCLUDED_)
	#include "GLHierarchy/TextureFactoryConfig.h"
#endif
#if !defined(AFX_IMAGE_H__F545D0D5_5F10_4EFA_BE3B_3F3D34D4DBF3__INCLUDED_)
	#include "System/Image.h"
#endif


RAPTOR_NAMESPACE_BEGIN

class ITextureGenerator;


class RAPTOR_API CTextureFactory : public CPersistence
{
public:
	CTextureFactory(const std::string& name = "GL_TEXTURE_FACTORY");
	virtual ~CTextureFactory(void);

	//!	Returns a default factory, suited far most cases.
	static CTextureFactory& getDefaultFactory();

	//! Returns the common factory configuration
	CTextureFactoryConfig& getConfig(void) { return mConfig; };


	//!	Creates a 2D texture object with power of two dimensions ( much faster ! )
    //! @param type : the type of texels of the texture ( server memory model )
	//! @param env_mode : is one of the TEXTURE_FUNCTION enum
    //! @param filter : specifies the kind of texel sampler of the texture object
    //! @return : the texture created or NULL if there has been an error
    ITextureObject*		const glCreateTexture(  ITextureObject::TEXEL_TYPE type,
												ITextureObject::TEXTURE_FUNCTION env_mode = ITextureObject::CGL_ALPHA_TRANSPARENT,
												ITextureObject::TEXTURE_FILTER filter = ITextureObject::CGL_UNFILTERED);

    //!	Creates a 2D rectangular texture object
	//! @param type : the type of texels of the texture ( server memory model )
	//! @param env_mode : is one of the TEXTURE_FUNCTION enum
    //! @param filter : specifies the kind of texel sampler of the texture object
    //! @return : the texture created or NULL if there has been an error
    ITextureObject*		const glCreateRectangleTexture(	ITextureObject::TEXEL_TYPE type,
														ITextureObject::TEXTURE_FUNCTION env_mode = ITextureObject::CGL_ALPHA_TRANSPARENT,
														ITextureObject::TEXTURE_FILTER filter = ITextureObject::CGL_UNFILTERED);
	
	//!	Creates a texture object
    //! @param type : the type of texels of the texture ( server memory model )
	//! @param env_mode : is one of the TEXTURE_FUNCTION enum
	//! @param pGenerator : is a texture generator that produces texels to dynamically 'reload' the texture.
    //! @param filter : specifies the kind of texel sampler of the texture object
    //! @return : the texture created or NULL if there has been an error
    ITextureObject*		const glCreateDynamicTexture(	ITextureObject::TEXEL_TYPE type,
														ITextureObject::TEXTURE_FUNCTION env_mode = ITextureObject::CGL_ALPHA_TRANSPARENT,
														ITextureObject::TEXTURE_FILTER filter = ITextureObject::CGL_UNFILTERED,
														ITextureGenerator* pGenerator = NULL);

	//!	Creates a cubemap object
	//! @param type : the type of texels of the texture ( server memory model )
	//! @param env_mode : is one of the TEXTURE_FUNCTION enum
    //! @param filter : specifies the kind of texel sampler of the texture object
    //! @return : the texture created or NULL if there has been an error
	ITextureObject*		const glCreateCubemap(	ITextureObject::TEXEL_TYPE type,
												ITextureObject::TEXTURE_FUNCTION env_mode = ITextureObject::CGL_ALPHA_TRANSPARENT,
												ITextureObject::TEXTURE_FILTER filter = ITextureObject::CGL_UNFILTERED);

    //!	Creates a 3D texture object. Might return NULL if volumic textures are not supported.
	//! @param type : the type of texels of the texture ( server memory model )
	//! @param env_mode : is one of the TEXTURE_FUNCTION enum
    //! @param filter : specifies the kind of texel sampler of the texture object
    //! @return : the texture created or NULL if there has been an error
	ITextureObject*		const glCreateVolumeTexture(ITextureObject::TEXEL_TYPE type,
													ITextureObject::TEXTURE_FUNCTION env_mode = ITextureObject::CGL_ALPHA_TRANSPARENT,
													ITextureObject::TEXTURE_FILTER filter = ITextureObject::CGL_UNFILTERED);

	//!	Creates a sprite object. A sprite is a small texture with
	//!	pixel replacement mode ( CGL_OPAQUE ) and no filtering ( thus no anisotropy )
	//!	See CreateTexture.
	ITextureObject*		const glCreateSprite(ITextureObject::TEXEL_TYPE type);


	//!	Creates a Vulkan image texture object.
	//! @param type : the type of texels of the texture ( server memory model )
	//! @param env_mode : is one of the TEXTURE_FUNCTION enum
	//! @param filter : specifies the kind of texel sampler of the texture object
	//! @return : the texture created or NULL if there has been an error
	ITextureObject*	const vkCreateTexture(	ITextureObject::TEXEL_TYPE type,
											ITextureObject::TEXTURE_FUNCTION env_mode = ITextureObject::CGL_ALPHA_TRANSPARENT,
											ITextureObject::TEXTURE_FILTER filter = ITextureObject::CGL_UNFILTERED);

	//!	If .buffer extension is used,
	//!	loadBUFFER does nothing, but assumes the user provides a buffer
	//!	of pixels, pointed by T->pixels and that T->width & T->height are
	//!	properly set to the width and height of the image.
	//!	Datas in the buffer must be GL_RGBA unsigned bytes format
	//!	T must have been created with BindTexture
	//!
	//!	Some special processing is done with alpha component when default loaders are used:
	//!	- if T->alpha is > 255 : if RGB components are all 0, alpha is set to 0, 255 otherwise.
	//! - if T->alpha is > 0 : alpha component is set to T->alpha
	//!	- otherwise, alpha component is set to an average of RGB components values.
	//!
	//!	mode is an 'oring' of the parameters:
	//!	- mipmapping model : USER_MIPMAPPED or AUTO_MIPMAPPED
	//!	- texture format : COLOR24, COLOR24_ALPHA, LIGHTMAP, LIGHTMAP_ALPHA a DEPTHMAP:
	//!	the depthmap can be of unsigned bytes, shorts, ints of floats using CGL_DEPTH_MAP8,
	//! CGL_DEPTH_MAP16, CGL_DEPTH_MAP32 or CGL_DEPTH_MAPZ
	//!	- the cubemap face index : CUBEMAP_PX,PY,PZ,NX,NY,or NZ
	//! - the numeric format
	bool glLoadTexture(	ITextureObject* const T,
						const std::string &fname,
						const CVaArray<CImage::IImageOP*>& ops = CVaArray<CImage::IImageOP*>());

	bool glLoadTexture(ITextureObject* const T,
					   const CImage &image);
	bool vkLoadTexture(CVulkanTextureObject* const T,
					   const CImage &image);

	//! Same as above excepts that the texteure texels are only loaded in client memory,
	//!	and not yet transfered to GL server memory.
	//! A subsequent call to glLoadTexture will finalize texture loading.
	RAPTOR_HANDLE glvkPreloadTexture(	ITextureObject* const T,
										const std::string &fname,
										const CVaArray<CImage::IImageOP*>& ops = CVaArray<CImage::IImageOP*>());

	//!	@param preload : the result of a texture preload
	//!	@param mode : @see glLoadTexture
	//!	@return : @see glLoadTexture
	bool glvkLoadTexture(	ITextureObject* const T,
							RAPTOR_HANDLE preload);



    //! Resize a texture object if it possible :
    //! @param width: desired width, must be a power of two if not supported otherwise.
	//! @param height: desired height, must be a power of two if not supported otherwise.
	//! @param depth: desired depth, must be a power of two if not supported otherwise.
    //! @param I: texture is rescaled if actually loaded, allocated texels are discarded.
    //! @return false if resizing is not possible.
    bool glResizeTexture(ITextureObject *I, uint32_t width, uint32_t height, uint32_t depth=1) const;

	//! This method set texture alpha for transparency management.
	//! @param alpha: if 0, use color average, if > 255, saturate to 0 and 255, else use alpha
	//! @return false if color quantizing is not possible.
	bool glSetTransparency(ITextureObject *I, uint32_t	alpha);

	//!	Set a specific color as transparent. Any number of colors can be transparent,
	//!	they are changed through their alpha component
	//! This method only applyes to 2D textures ; future versions will handle more cases if needed.
	bool glSetTransparentColor(ITextureObject *I, uint8_t r, uint8_t g, uint8_t b);
		
    //! Exports a texture image to a file, type is selected from fname extension
	bool glExportTexture(ITextureObject *T,const std::string &fname);

	//! This methods loads a compressed texture that has been created 
	//! using the export method here under.
	bool glLoadCompressedTexture(ITextureObject* const T, const std::string& fname );

    //! This method exports a given texture 
	bool glExportCompressedTexture(const std::string& fname, const ITextureObject *T);

	//! Inherited from CPersistence
	DECLARE_CLASS_ID(CTextureFactoryClassID,"TextureFactory",CPersistence)



private:
	//! Forbidden copy constructor
	CTextureFactory(const CTextureFactory& rsh);

	//! Forbidden assignment operator
	CTextureFactory& operator=(const CTextureFactory& rsh) { return *this; };

	//!	This factory configuration
	CTextureFactoryConfig	mConfig;

	//!	A pointer to the default factory, initialized only if required.
	static CTextureFactory *m_pDefault;

	typedef struct TexturePreload_t
	{
		unsigned int target;
		unsigned int inner_format;
		unsigned int format;
		unsigned int src_format;
		bool createNormalMap;
		bool reScale;
	} TexturePreload;
	map<ITextureObject*, TexturePreload>	m_preloads;
};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_TEXTURE_H__1B470EC4_4B68_11D3_9142_9A502CBADC6B__INCLUDED_)

