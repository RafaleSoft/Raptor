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
#if !defined(AFX_TEXTUREOBJECT_H__D32B6294_B42B_4E6F_AB73_13B33C544AD0__INCLUDED_)
	#include "GLHierarchy/TextureObject.h"
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
    CTextureObject*		const glCreateTexture(  ITextureObject::TEXEL_TYPE type,
												CTextureObject::TEXTURE_FUNCTION env_mode = CTextureObject::CGL_ALPHA_TRANSPARENT,
												ITextureObject::TEXTURE_FILTER filter = ITextureObject::CGL_UNFILTERED);

    //!	Creates a 2D rectangular texture object
	//! @param type : the type of texels of the texture ( server memory model )
	//! @param env_mode : is one of the TEXTURE_FUNCTION enum
    //! @param filter : specifies the kind of texel sampler of the texture object
    //! @return : the texture created or NULL if there has been an error
    CTextureObject*		const glCreateRectangleTexture(	ITextureObject::TEXEL_TYPE type,
														CTextureObject::TEXTURE_FUNCTION env_mode = CTextureObject::CGL_ALPHA_TRANSPARENT,
														ITextureObject::TEXTURE_FILTER filter = ITextureObject::CGL_UNFILTERED);
	
	//!	Creates a texture object
    //! @param type : the type of texels of the texture ( server memory model )
	//! @param env_mode : is one of the TEXTURE_FUNCTION enum
	//! @param pGenerator : is a texture generator that produces texels to dynamically 'reload' the texture.
    //! @param filter : specifies the kind of texel sampler of the texture object
    //! @return : the texture created or NULL if there has been an error
    CTextureObject*		const glCreateDynamicTexture(	ITextureObject::TEXEL_TYPE type,
														CTextureObject::TEXTURE_FUNCTION env_mode = CTextureObject::CGL_ALPHA_TRANSPARENT,
														ITextureObject::TEXTURE_FILTER filter = ITextureObject::CGL_UNFILTERED,
														ITextureGenerator* pGenerator = NULL);

	//!	Creates a cubemap object
	//! @param type : the type of texels of the texture ( server memory model )
	//! @param env_mode : is one of the TEXTURE_FUNCTION enum
    //! @param filter : specifies the kind of texel sampler of the texture object
    //! @return : the texture created or NULL if there has been an error
	CTextureObject*		const glCreateCubemap(	ITextureObject::TEXEL_TYPE type,
												CTextureObject::TEXTURE_FUNCTION env_mode = CTextureObject::CGL_ALPHA_TRANSPARENT,
												ITextureObject::TEXTURE_FILTER filter = ITextureObject::CGL_UNFILTERED);

    //!	Creates a 3D texture object. Might return NULL if volumic textures are not supported.
	//! @param type : the type of texels of the texture ( server memory model )
	//! @param env_mode : is one of the TEXTURE_FUNCTION enum
    //! @param filter : specifies the kind of texel sampler of the texture object
    //! @return : the texture created or NULL if there has been an error
	CTextureObject*		const glCreateVolumeTexture(ITextureObject::TEXEL_TYPE type,
													CTextureObject::TEXTURE_FUNCTION env_mode = CTextureObject::CGL_ALPHA_TRANSPARENT,
													ITextureObject::TEXTURE_FILTER filter = ITextureObject::CGL_UNFILTERED);

	//!	Creates a sprite object. A sprite is a small texture with
	//!	pixel replacement mode ( CGL_OPAQUE ) and no filtering ( thus no anisotropy )
	//!	See CreateTexture.
	CTextureObject*		const glCreateSprite(ITextureObject::TEXEL_TYPE type);


	//!	Creates a Vulkan image texture object.
	//! @param type : the type of texels of the texture ( server memory model )
	//! @param env_mode : is one of the TEXTURE_FUNCTION enum
	//! @param filter : specifies the kind of texel sampler of the texture object
	//! @return : the texture created or NULL if there has been an error
	ITextureObject*	const vkCreateTexture(	ITextureObject::TEXEL_TYPE type,
											CTextureObject::TEXTURE_FUNCTION env_mode = CTextureObject::CGL_ALPHA_TRANSPARENT,
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
						const CVaArray<CImage::IImageOP::OP_KIND>& ops = CVaArray<CImage::IImageOP::OP_KIND>());

	bool glLoadTexture(CTextureObject* const T,
					   const CImage &image);
	bool vkLoadTexture(CVulkanTextureObject* const T,
					   const CImage &image);

	//! Same as above excepts that the texteure texels are only loaded in client memory,
	//!	and not yet transfered to GL server memory.
	//! A subsequent call to glLoadTexture will finalize texture loading.
	RAPTOR_HANDLE glvkPreloadTexture(	CTextureObject* const T,
										const std::string &fname,
										const CVaArray<CImage::IImageOP::OP_KIND>& ops = CVaArray<CImage::IImageOP::OP_KIND>());

	//!	@param preload : the result of a texture preload
	//!	@param mode : @see glLoadTexture
	//!	@return : @see glLoadTexture
	bool glvkLoadTexture(	CTextureObject* const T,
							RAPTOR_HANDLE preload);



    //! Resize a texture object if it possible :
    //! - width & height & depth must be a power of two if not supported otherwise
    //! - image is rescaled if actually loaded
    //! - allocated texels are discarded.
    //! @return false if resizing is not possible
    bool glResizeTexture( CTextureObject *T, unsigned int width, unsigned int height, unsigned int depth=1) const;

    //! Exports a texture image to a file, type is selected from fname extension
	bool glExportTexture(CTextureObject *T,const std::string &fname);

	//! This methods loads a compressed texture that has been created 
	//! using the export method here under.
	bool glLoadCompressedTexture(	CTextureObject* const T, const std::string& fname );

    //! This method exports a given texture 
	bool glExportCompressedTexture(const std::string& fname, const CTextureObject *T);

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
	map<CTextureObject*, TexturePreload>	m_preloads;
};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_TEXTURE_H__1B470EC4_4B68_11D3_9142_9A502CBADC6B__INCLUDED_)

