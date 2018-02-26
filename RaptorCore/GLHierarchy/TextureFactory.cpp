// Texture.cpp: implementation of the CTextureFactory class.
//
//////////////////////////////////////////////////////////////////////
#include "Subsys/CodeGeneration.h"


#if !defined(AFX_TEXTUREFACTORY_H__1B470EC4_4B68_11D3_9142_9A502CBADC6B__INCLUDED_)
	#include "TextureFactory.h"
#endif
#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
	#include "System/Raptor.h"
#endif
#if !defined(AFX_RAPTORERRORMANAGER_H__FA5A36CD_56BC_4AA1_A5F4_451734AD395E__INCLUDED_)
    #include "System/RaptorErrorManager.h"
#endif
#if !defined(AFX_RAPTORGLEXTENSIONS_H__E5B5A1D9_60F8_4E20_B4E1_8E5A9CB7E0EB__INCLUDED_)
	#include "System/RaptorGLExtensions.h"
#endif
#if !defined(AFX_RAPTORIO_H__87D52C27_9117_4675_95DC_6AD2CCD2E78D__INCLUDED_)
	#include "System/RaptorIO.h"
#endif
#if !defined(AFX_OBJECTFACTORY_H__7F891C52_9E32_489C_B09C_5E5803522D91__INCLUDED_)
	#include "ObjectFactory.h"
#endif
#if !defined(AFX_MEMORY_H__81A6CA9A_4ED9_4260_B6E4_C03276C38DBC__INCLUDED_)
	#include "System/Memory.h"
#endif
#if !defined(AFX_VULKANTEXTUREOBJECT_H__5E3E26C2_441F_4051_986F_2207AF0B3F6D__INCLUDED_)
	#include "Subsys/Vulkan/VulkanTextureObject.h"
#endif
#if !defined(AFX_RAPTORVULKANDEVICE_H__2FDEDD40_444E_4CC2_96AA_CBF9E79C3ABE__INCLUDED_)
	#include "Subsys/Vulkan/VulkanDevice.h"
#endif

#ifdef WIN32
#pragma warning(disable:4786)
#endif


RAPTOR_NAMESPACE

static CTextureFactory::CTextureFactoryClassID factoryId;
static CPersistentType<CTextureFactory> textureFactory(factoryId);
const CPersistence::CPersistenceClassID& CTextureFactory::CTextureFactoryClassID::GetClassId(void)
{
	return factoryId;
}

CTextureFactory *CTextureFactory::m_pDefault = NULL;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CTextureFactory::CTextureFactory(const std::string& name)
	:CPersistence(factoryId,name),mConfig()
{
    if (mConfig.getNumCompressors() < 0)
        mConfig.glInit();
}

CTextureFactory::~CTextureFactory(void)
{
	if (m_pDefault == this)
		m_pDefault = NULL;
}

CTextureFactory& CTextureFactory::getDefaultFactory()
{
	if (m_pDefault == NULL)
	{
		m_pDefault = new CTextureFactory("DEFAULT_TEXTURE_FACTORY");
	}

	return *m_pDefault;
}


/*
RAPTOR_HANDLE CTextureFactory::glvkPreloadTexture(CTextureObject* const T,
												const std::string &fname,
												const CVaArray<CImage::IImageOP::OP_KIND>& ops)
{
	RAPTOR_HANDLE preload(0,NULL);

	//	ensure we can do something ...
#ifdef RAPTOR_DEBUG_MODE_GENERATION
	if ((T == NULL) || (!glIsTexture(T->texname) && (T->target != VK_IMAGE_TYPE_2D))
	{
        Raptor::GetErrorManager()->generateRaptorError(	CTextureFactory::CTextureFactoryClassID::GetClassId(),
														CRaptorErrorManager::RAPTOR_WARNING,
														CRaptorMessages::ID_NULL_OBJECT);
		return preload;
	}
#endif

	if ((T->target >> 16) == ITextureGenerator::BUFFERED)
	{
#ifdef RAPTOR_DEBUG_MODE_GENERATION
        Raptor::GetErrorManager()->generateRaptorError(	CTextureFactory::CTextureFactoryClassID::GetClassId(),
														CRaptorErrorManager::RAPTOR_WARNING,
														CRaptorMessages::ID_UPDATE_FAILED);
#endif
		return preload;
	}


	CImage::IImageIO *imager = CImage::getImageKindIO(fname);
	if (imager != NULL)
	{
		if (imager->loadImageFile(fname,0)) //T))
		{
			//
			//	Initialise texture attributes : main formats.
			//
			T->m_name = fname.data();
			TexturePreload P;
			P.target = 0;
			P.inner_format = T->getTexelFormat();
			P.format = T->getBufferFormat();
			P.src_format = T->getBufferType();
			P.createNormalMap = ops.hasValue(CImage::IImageOP::BUMPMAP_LOADER);
			P.reScale = ops.hasValue(CImage::IImageOP::IMAGE_SCALER);

			//	1)	Texture internal format
			//
			//	Handle normal maps for bumping
			//
			if (P.createNormalMap)
			{
				CImage::IImageOP* op = CImage::getImageKindOP(CImage::IImageOP::BUMPMAP_LOADER);
				//op->apply(T,P.format,P.src_format,mConfig);
			}

			//
			//	Handle support for texture compression.
			//	Rely on OpenGL 'generic' compression to choose
			//	appropriate compressor of hardware capabilities.
			//
#ifdef GL_ARB_texture_compression
			//	use texture compression if possible
			else if (NULL != mConfig.getCurrentCompressor())
			{
				GLuint compressor = mConfig.getCurrentCompressor()->getFormat(P.inner_format);
				if (compressor != (GLuint)-1)
					P.inner_format = compressor;
			}
#endif


			//	2)	Format of data to load
			//
			CTextureFactoryConfig::TEXEL_FORMAT texelFormat = mConfig.getTexelFormat();
			if (P.format == GL_RGBA)
			{
#if defined(GL_EXT_bgra)
				if (texelFormat == CTextureFactoryConfig::BYTEORDER_BGRA)
					P.format = GL_BGRA_EXT;
#endif
#if defined(GL_EXT_abgr)
				else if (texelFormat == CTextureFactoryConfig::BYTEORDER_ABGR)
					P.format = GL_ABGR_EXT;
#endif
			}
			else if (P.format == GL_RGB)
			{
#if defined(GL_EXT_bgra)
				if (texelFormat == CTextureFactoryConfig::BYTEORDER_BGRA)
					P.format = GL_BGR_EXT;
#endif
			}


			//
			//	Handle Image re-scaling
			//
			if (mConfig.useTextureResize() || P.reScale)
            {
				CImage::IImageOP* op = CImage::getImageKindOP(CImage::IImageOP::IMAGE_SCALER);
// OGL call		//op->apply(T,P.format,P.src_format,mConfig);
            }

			//
			//	Handle cubemap textures
			//
			P.target = T->target;
			preload.handle = (unsigned int)T;
			preload.hClass = 0;
			m_preloads[T] = P;

			//
			// Store texels in a relocated chunk.
			//
			if (VK_IMAGE_TYPE_2D == T->target)
			{
				T->vk_texname =
					CTexelAllocator::GetInstance()->vkAllocateTextureImage(	T->getWidth(),
																			T->getHeight(),
																			T->getDepth(),
																			T->getTexelType());
			}
			else
			{
				size_t size = 4 * T->getWidth() * T->getHeight() * MAX(1, T->getDepth());
				unsigned char *texPointer = NULL;
				if (T->getTexels() != NULL)
				{
					texPointer = CTexelAllocator::GetInstance()->allocateTexels(size);
					CTexelAllocator::GetInstance()->glvkCopyPointer(texPointer, T->getTexels(), size);
				}
				else if (T->getFloatTexels() != NULL)
				{
					texPointer = (unsigned char *)CTexelAllocator::GetInstance()->allocateFloatTexels(size);
					CTexelAllocator::GetInstance()->glvkCopyPointer(texPointer,
																	(unsigned char *)T->getFloatTexels(),
																	4 * size);
				}
			}
			
			T->releaseTexels();

			CATCH_GL_ERROR
		}
		else	// load from file failed
        {
            CRaptorMessages::MessageArgument arg;
            arg.arg_sz = fname.data();
            vector<CRaptorMessages::MessageArgument> args;
            args.push_back(arg);
            Raptor::GetErrorManager()->generateRaptorError(	CTextureFactory::CTextureFactoryClassID::GetClassId(),
			    											CRaptorErrorManager::RAPTOR_WARNING,
				    										CRaptorMessages::ID_TEXTURE_MISS,args);
        }
	}

	return preload;
}
*/
/*
bool CTextureFactory::glvkLoadTexture(	CTextureObject* const T,
										RAPTOR_HANDLE preload)
{
	if ((NULL == T) || (preload.handle != (unsigned int)T))
		return false;

	TexturePreload P;
	map<CTextureObject*, TexturePreload>::iterator it = m_preloads.find(T);
	if (it == m_preloads.end())
		return false;
	else
	{
		P = (*it).second;
		m_preloads.erase(it);
	}

	if (VK_IMAGE_TYPE_2D == T->target)
	{
	}
	else
	{
		//
		//	Final processing : 
		//	- mipmapping generation
		//	- image loading
#if defined(GL_EXT_texture3D)
		if (T->getDepth() > 1)   // Texture Volumes
		{
			const CRaptorExtensions * const extensions = Raptor::glGetExtensions();
			extensions->glTexImage3DEXT(P.target, 0,
										P.inner_format,
										T->getWidth(), T->getHeight(), T->getDepth(),
										0, P.format, P.src_format,
										T->texels);
		}
		else
#endif
		{
			bool generateMipmap = ((T->getFilter() == ITextureObject::CGL_TRILINEAR) ||
								   (T->getFilter() == ITextureObject::CGL_ANISOTROPIC)) &&
								   mConfig.getGenerateMipmap();
			//
			//  The default case, at least load the texture.
			//
#if (defined(GL_VERSION_1_4) && !defined(GL_VERSION_3_0) && !defined(GL_EXT_framebuffer_object))
			if (generateMipmap)
				glTexParameteri(target, GL_GENERATE_MIPMAP, GL_TRUE);
#endif
			glTexImage2D(P.target,
						 T->getCurrentMipMapLevel(), P.inner_format,
						 T->getWidth(), T->getHeight(),
						 0, P.format,
						 P.src_format,
						 T->texels);
#if (defined(GL_VERSION_3_0) || defined(GL_EXT_framebuffer_object))
			if (generateMipmap)
				Raptor::glGetExtensions()->glGenerateMipmapEXT(P.target);
#endif
		}
	}

	CATCH_GL_ERROR

	return true;
}
*/


bool CTextureFactory::vkLoadTexture(CVulkanTextureObject* const T,
									const CImage &image)
{
	//	Initialise main texture attributes.
	if ((image.getWidth() > 0) && (image.getHeight() > 0))
		T->setSize(image.getWidth(), image.getHeight(), image.getLayers());
	T->setName(image.getName());

	//	1)	Texture internal format
	VkFormat GL_INNER_FORMAT = T->getTexelFormat();
	GLuint GL_FORMAT = image.getBufferFormat();
	GLuint GL_SRC_FORMAT = image.getBufferType();
	void *pixels = image.getPixels();
	if (NULL == pixels)
		pixels = image.getFloatPixels();


	//	2)	Format of data to load
	//
	CTextureFactoryConfig::TEXEL_FORMAT texelFormat = mConfig.getTexelFormat();
	if (GL_FORMAT == GL_RGBA)
	{
#if defined(GL_EXT_bgra)
		if (texelFormat == CTextureFactoryConfig::BYTEORDER_BGRA)
			GL_FORMAT = GL_BGRA_EXT;
#endif
#if defined(GL_EXT_abgr)
		else if (texelFormat == CTextureFactoryConfig::BYTEORDER_ABGR)
			GL_FORMAT = GL_ABGR_EXT;
#endif
	}
	else if (GL_FORMAT == GL_RGB)
	{
#if defined(GL_EXT_bgra)
		if (texelFormat == CTextureFactoryConfig::BYTEORDER_BGRA)
			GL_FORMAT = GL_BGR_EXT;
#endif
	}

	//
	//	Handle cubemap textures
	//
	bool result = false;

	//	Final processing :
	//	- mipmapping generation
	//	- image loading
	T->vkLoadTexture(GL_INNER_FORMAT,
					GL_FORMAT,
					GL_SRC_FORMAT,
					(uint8_t*)pixels);
	result = true;

	CATCH_GL_ERROR
		
	return result;
}

bool CTextureFactory::glLoadTexture(CTextureObject* const T,
									const CImage &image)
{
	//	ensure we can do something ...
#ifdef RAPTOR_DEBUG_MODE_GENERATION
	if (!glIsTexture(T->texname))
	{
		Raptor::GetErrorManager()->generateRaptorError(CTextureFactory::CTextureFactoryClassID::GetClassId(),
													   CRaptorErrorManager::RAPTOR_WARNING,
													   CRaptorMessages::ID_NULL_OBJECT);
		return false;
	}
#endif

	ITextureGenerator::GENERATOR_KIND kind = ITextureGenerator::NONE;
	if (NULL != T->getTexelGenerator())
		kind = T->getTexelGenerator()->getKind();
	if (kind == ITextureGenerator::BUFFERED)
	{
#ifdef RAPTOR_DEBUG_MODE_GENERATION
		Raptor::GetErrorManager()->generateRaptorError(CTextureFactory::CTextureFactoryClassID::GetClassId(),
													   CRaptorErrorManager::RAPTOR_WARNING,
													   CRaptorMessages::ID_UPDATE_FAILED);
#endif
		return false;
	}

	//	Initialise main texture attributes.
	if ((image.getWidth() > 0) && (image.getHeight() > 0))
		T->setSize(image.getWidth(), image.getHeight(), image.getLayers());
	T->setName(image.getName());

	//	1)	Texture internal format
	GLuint GL_INNER_FORMAT = T->getTexelFormat();
	GLuint GL_FORMAT = image.getBufferFormat();
	GLuint GL_SRC_FORMAT = image.getBufferType();
	void *pixels = image.getPixels();
	if (NULL == pixels)
		pixels = image.getFloatPixels();

#ifdef GL_ARB_texture_compression
	//	use texture compression if possible
	if (NULL != mConfig.getCurrentCompressor())
	{
		GLuint compressor = mConfig.getCurrentCompressor()->getFormat(GL_INNER_FORMAT);
		if (compressor != (GLuint)-1)
			GL_INNER_FORMAT = compressor;
	}
#endif


	//	2)	Format of data to load
	//
	CTextureFactoryConfig::TEXEL_FORMAT texelFormat = mConfig.getTexelFormat();
	if (GL_FORMAT == GL_RGBA)
	{
#if defined(GL_EXT_bgra)
		if (texelFormat == CTextureFactoryConfig::BYTEORDER_BGRA)
			GL_FORMAT = GL_BGRA_EXT;
#endif
#if defined(GL_EXT_abgr)
		else if (texelFormat == CTextureFactoryConfig::BYTEORDER_ABGR)
			GL_FORMAT = GL_ABGR_EXT;
#endif
	}
	else if (GL_FORMAT == GL_RGB)
	{
#if defined(GL_EXT_bgra)
		if (texelFormat == CTextureFactoryConfig::BYTEORDER_BGRA)
			GL_FORMAT = GL_BGR_EXT;
#endif
	}

	//
	//	Handle cubemap textures
	//
	bool result = false;
	GLuint target = T->target;

	//
	//	Final processing :
	//	- mipmapping generation
	//	- image loading
#if defined(GL_EXT_texture3D)
	if (T->getDepth() > 1)   // Texture Volumes
	{
		const CRaptorGLExtensions * const extensions = Raptor::glGetExtensions();
		extensions->glTexImage3DEXT(target, 0,
									GL_INNER_FORMAT,
									T->getWidth(),
									T->getHeight(),
									T->getDepth(),
									0, GL_FORMAT, GL_SRC_FORMAT,
									pixels);
		result = true;
	}
	else
#endif
	{
		bool generateMipmap = ((T->getFilter() == ITextureObject::CGL_TRILINEAR) ||
							   (T->getFilter() == ITextureObject::CGL_ANISOTROPIC)) &&
							   mConfig.getGenerateMipmap();

#if (defined(GL_VERSION_1_4) && !defined(GL_VERSION_3_0) && !defined(GL_EXT_framebuffer_object))
		if (generateMipmap)
			glTexParameteri(target, GL_GENERATE_MIPMAP, GL_TRUE);
#endif
		glTexImage2D(target,
						T->getCurrentMipMapLevel(),
						GL_INNER_FORMAT,
						T->getWidth(), 
						T->getHeight(),
						0, GL_FORMAT,
						GL_SRC_FORMAT,
						pixels);
#if (defined(GL_VERSION_3_0) || defined(GL_EXT_framebuffer_object))
		if (generateMipmap)
			Raptor::glGetExtensions()->glGenerateMipmapEXT(target);
#endif
		result = true;
	}

	CATCH_GL_ERROR

	return result;
}


bool CTextureFactory::glLoadTexture(ITextureObject* const T,
									const std::string& fname,
									const CVaArray<CImage::IImageOP::OP_KIND>& ops)
{
	//	ensure we can do something ...
#ifndef RAPTOR_DEBUG_MODE_GENERATION
	if ((T == NULL) || fname.empty())
	{
		Raptor::GetErrorManager()->generateRaptorError(CTextureFactory::CTextureFactoryClassID::GetClassId(),
													   CRaptorErrorManager::RAPTOR_WARNING,
													   CRaptorMessages::ID_NULL_OBJECT);
		return false;
	}
#endif

	CImage::IImageOP::operation_param_t param;
	param.bump_scale = mConfig.getBumpAmplitude();
	param.transparency = T->getTransparency();
	CVaArray<CImage::IImageOP::OP_KIND> iops = ops;
	if (mConfig.useTextureResize() && !ops.hasValue(CImage::IImageOP::IMAGE_SCALER))
		iops.addValue(CImage::IImageOP::IMAGE_SCALER);
	if (!ops.hasValue(CImage::IImageOP::ALPHA_TRANSPARENCY))
		iops.addValue(CImage::IImageOP::ALPHA_TRANSPARENCY);

	CImage loadImage;
	if (loadImage.loadImage(fname, iops, param))
	{
		CTextureObject *t = T->getGLTextureObject();
		if (NULL != t)
			return glLoadTexture(t, loadImage);
		else
		{
			CVulkanTextureObject *v = T->getVulkanTextureObject();
			if (NULL != v)
				return vkLoadTexture(v, loadImage);
			else
				return false;
		}
	}
	else
		return false;
}

bool CTextureFactory::glResizeTexture( CTextureObject *T, unsigned int width, unsigned int height, unsigned int depth) const
{
    if ((T == NULL) || (width == 0) || (height == 0) || (depth == 0))
        return false;

    if ((T->getWidth() == width) && (T->getHeight() == height) && (T->getDepth() == depth))
        return true;

    GLint currentWidth = 0;
    GLint currentHeight = 0;
	GLint currentDepth = 0;

    GLenum target = T->target;

    //! For a cube map, the target must identify a specific cube face
    //! otherwise, getTexLevelParameter will raise an error.
    //! As a cube map must have six equal sized 2D textures, we can query only one
    //! no matter what is the current, the error will be raised at loading time.
#if defined(GL_ARB_texture_cube_map)
	if (target == GL_TEXTURE_CUBE_MAP_ARB)
	    target = GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB;
#endif
    
    glBindTexture(target,T->texname);

	glGetTexLevelParameteriv(target, T->getCurrentMipMapLevel(), GL_TEXTURE_WIDTH, &currentWidth);
	glGetTexLevelParameteriv(target, T->getCurrentMipMapLevel(), GL_TEXTURE_HEIGHT, &currentHeight);
	glGetTexLevelParameteriv(target, T->getCurrentMipMapLevel(), GL_TEXTURE_DEPTH, &currentDepth);


	//!	Handle the case with unsupported rectangle texture
	if (!mConfig.supportTextureResize())
	{
		unsigned int w2 = width;
		unsigned int h2 = height;
		unsigned int d2 = depth;
		if (w2 > 0)
			w2 = 1 << (unsigned int)(log((float)width) / log(2.0f));
		if (h2 > 0)
			h2 = 1 << (unsigned int)(log((float)height) / log(2.0f));
		if (d2 > 0)
			d2 = 1 << (unsigned int)(log((float)depth) / log(2.0f));

		if ((w2 != width) || (h2 != height) || (d2 != depth))
		{
			if (mConfig.useTextureResize())
			{
				// It seems better to select the biggest size
				//	because a too small texture may not satify
				//	the user ( e.g. texel buffer too small for a video )
				if (w2 < width)
					width = 2 * w2;
				else
					width = w2;
				if (h2 < height)
					height = 2 * h2;
				else
					height = h2;
				if (d2 < depth)
					depth = 2 * d2;
				else
					depth = d2;
			}
		}
	}

    {
		T->setSize(width, height, depth);
		int posx = 0;
		int posy = 0;
		int W = 0;
		int H = 0;
		T->getGenerationSize(posx, posy, W, H);
		T->setGenerationSize(posx, posy, W, H);
		
		GLuint GL_FORMAT = GL_RGBA;
		GLuint GL_TYPE = GL_UNSIGNED_BYTE;
		if ((T->getTexelFormat() == GL_DEPTH_COMPONENT) ||
			(T->getTexelFormat() == GL_DEPTH_COMPONENT16_ARB) ||
			(T->getTexelFormat() == GL_DEPTH_COMPONENT24_ARB) ||
			(T->getTexelFormat() == GL_DEPTH_COMPONENT32_ARB))
		{
			GL_FORMAT = GL_DEPTH_COMPONENT;
			GL_TYPE = GL_FLOAT;
		}
		else if (T->getTexelFormat() == GL_DEPTH24_STENCIL8_EXT)
		{
			GL_FORMAT = GL_DEPTH_STENCIL_EXT;
			GL_TYPE = GL_UNSIGNED_INT_24_8_EXT;
		}

        if (target == GL_TEXTURE_1D)
			glTexImage1D(	GL_TEXTURE_1D, 
							T->getCurrentMipMapLevel(), 
							T->getTexelFormat(),
							width, 
							0, 
							GL_FORMAT,
							GL_TYPE,
							NULL);
        else if (target == GL_TEXTURE_2D)
            glTexImage2D(	GL_TEXTURE_2D, 
							T->getCurrentMipMapLevel(), 
							T->getTexelFormat(),
							width, height, 
							0, 
							GL_FORMAT,
							GL_TYPE,
							NULL);
#if defined(GL_EXT_texture3D)
        else if (target == GL_TEXTURE_3D_EXT)
        {
            const CRaptorGLExtensions * const extensions = Raptor::glGetExtensions();
            extensions->glTexImage3DEXT(	GL_TEXTURE_3D_EXT, 
											T->getCurrentMipMapLevel(), 
											T->getTexelFormat(),
											width, 
											height, 
											depth, 
											0, 
											GL_FORMAT,
											GL_TYPE,
											NULL);
        }
#endif
    }
  // Temporary : need to resample texture data with proper interpolator
  //  else
  //  {
  //      
  //  }

    CATCH_GL_ERROR

    return true;
}

bool CTextureFactory::glExportTexture(CTextureObject *T,const std::string &fname)
{
	if ((T == NULL) || (fname.empty()))
        return false;

    if (T->texname == 0)
        return false;

    if ((T->getWidth() == 0) || (T->getHeight() == 0))
        return false;

	CImage::IImageIO *imager = CImage::getImageKindIO(fname);
    bool res = true;
	if (imager != NULL)
	{
		T->glvkRender();

		CImage exp;
		exp.allocatePixels(T->getWidth(), T->getHeight());
		glGetTexImage(GL_TEXTURE_2D, T->getCurrentMipMapLevel(), GL_RGBA, GL_UNSIGNED_BYTE, exp.getPixels());

        res = imager->storeImageFile(fname,&exp);
    }

    CATCH_GL_ERROR

    return res;
}

CTextureObject* const CTextureFactory::glCreateSprite(CTextureObject::TEXEL_TYPE type)
{
    //! type checking will be donne at loading
	CTextureObject* T = new CTextureObject(type);

	T->target = GL_TEXTURE_2D;
	glGenTextures(1,&(T->texname));

    T->setFunction(CTextureObject::CGL_OPAQUE);

	glBindTexture(GL_TEXTURE_2D,T->texname);

	T->glvkUpdateFilter(ITextureObject::CGL_UNFILTERED);
	T->glvkUpdateClamping(ITextureObject::CGL_CLAMP);
	
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_PRIORITY,mConfig.getCurrentPriority());

	CATCH_GL_ERROR

	return T;
}

CTextureObject* const CTextureFactory::glCreateCubemap(  ITextureObject::TEXEL_TYPE type,
                                                         CTextureObject::TEXTURE_FUNCTION env_mode,
														 ITextureObject::TEXTURE_FILTER filter)
{
#if defined(GL_ARB_texture_cube_map)
	if (!Raptor::glIsExtensionSupported("GL_ARB_texture_cube_map"))
    {
#ifdef RAPTOR_DEBUG_MODE_GENERATION
		vector<CRaptorMessages::MessageArgument> args;
		CRaptorMessages::MessageArgument arg;
		arg.arg_sz = "Cube Texture";
		args.push_back(arg);
        Raptor::GetErrorManager()->generateRaptorError(	CTextureFactory::CTextureFactoryClassID::GetClassId(),
														CRaptorErrorManager::RAPTOR_ERROR,
														CRaptorMessages::ID_FORMAT_NOT_SUPPORTED,args);
#endif
        return NULL;
    }

	CTextureObject* T = new CTextureObject(type);

	glGenTextures(1,&(T->texname));

    T->setFunction(env_mode);
	T->target = GL_TEXTURE_CUBE_MAP_ARB;

	glBindTexture(GL_TEXTURE_CUBE_MAP_ARB, T->texname);

	T->glvkUpdateFilter(filter);
	T->glvkUpdateClamping(CTextureObject::CGL_REPEAT);

	glTexParameterf(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_PRIORITY, mConfig.getCurrentPriority());

	// Is this usefull on a cube map ?
#ifdef GL_EXT_texture_filter_anisotropic
	if ((mConfig.getCurrentAnisotropy() > 1.0f) && (filter == ITextureObject::CGL_ANISOTROPIC))
    {
		glTexParameterf(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MAX_ANISOTROPY_EXT, mConfig.getCurrentAnisotropy());
        T->aniso_level = mConfig.getCurrentAnisotropy();
    }
#endif

	CATCH_GL_ERROR

	return T;
#else
    return NULL;
#endif
}


ITextureObject* const CTextureFactory::vkCreateTexture(ITextureObject::TEXEL_TYPE type,
													   CTextureObject::TEXTURE_FUNCTION env_mode,
													   ITextureObject::TEXTURE_FILTER filter)
{
#ifdef RAPTOR_DEBUG_MODE_GENERATION
	if ((type == ITextureObject::CGL_COLOR_FLOAT32) || (type == ITextureObject::CGL_COLOR_FLOAT32_ALPHA))
	{
		vector<CRaptorMessages::MessageArgument> args;
		CRaptorMessages::MessageArgument arg;
		arg.arg_sz = "2D Float32 Texture, use a Texture Rectangle instead.";
		args.push_back(arg);
		Raptor::GetErrorManager()->generateRaptorError(CTextureFactory::CTextureFactoryClassID::GetClassId(),
													   CRaptorErrorManager::RAPTOR_WARNING,
													   CRaptorMessages::ID_FORMAT_NOT_SUPPORTED, args);
	}
#endif

	CVulkanTextureObject* T = CVulkanDevice::getCurrentDevice().createTextureObject(type);
	if (NULL != T)
	{
		T->glvkUpdateFilter(filter);
		T->glvkUpdateClamping(ITextureObject::CGL_REPEAT);
		//T->setFunction(env_mode);

#ifdef GL_EXT_texture_filter_anisotropic
		if ((mConfig.getCurrentAnisotropy() > 1.0f) && (filter == ITextureObject::CGL_ANISOTROPIC))
		{
			//T->aniso_level = mConfig.getCurrentAnisotropy();
		}
#endif
	}

	return T;
}

CTextureObject* const CTextureFactory::glCreateTexture( ITextureObject::TEXEL_TYPE type,
                                                        CTextureObject::TEXTURE_FUNCTION env_mode,
														ITextureObject::TEXTURE_FILTER filter)
{
#ifdef RAPTOR_DEBUG_MODE_GENERATION
    if ((type == CTextureObject::CGL_COLOR_FLOAT32) || (type == CTextureObject::CGL_COLOR_FLOAT32_ALPHA))
    {
		vector<CRaptorMessages::MessageArgument> args;
		CRaptorMessages::MessageArgument arg;
		arg.arg_sz = "2D Float32 Texture, use a Texture Rectangle instead.";
		args.push_back(arg);
        Raptor::GetErrorManager()->generateRaptorError(	CTextureFactory::CTextureFactoryClassID::GetClassId(),
														CRaptorErrorManager::RAPTOR_WARNING,
														CRaptorMessages::ID_FORMAT_NOT_SUPPORTED,args);
    }
#endif
	if ((type == ITextureObject::CGL_DEPTH24_STENCIL8) &&
		(!Raptor::glIsExtensionSupported("GL_EXT_packed_depth_stencil")))
    {
#ifdef RAPTOR_DEBUG_MODE_GENERATION
		vector<CRaptorMessages::MessageArgument> args;
		CRaptorMessages::MessageArgument arg;
		arg.arg_sz = "Packed Depth Stencil Texture Format";
		args.push_back(arg);
        Raptor::GetErrorManager()->generateRaptorError(	CTextureFactory::CTextureFactoryClassID::GetClassId(),
														CRaptorErrorManager::RAPTOR_ERROR,
														CRaptorMessages::ID_FORMAT_NOT_SUPPORTED,args);
#endif
		return NULL;
	}

	CTextureObject* T = new CTextureObject(type);

	glGenTextures(1,&(T->texname));

    T->setFunction(env_mode);
	T->target = GL_TEXTURE_2D;

	glBindTexture(GL_TEXTURE_2D, T->texname);

	T->glvkUpdateFilter(filter);
	T->glvkUpdateClamping(CTextureObject::CGL_REPEAT);
	
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_PRIORITY, mConfig.getCurrentPriority());

#ifdef GL_EXT_texture_filter_anisotropic
	if ((mConfig.getCurrentAnisotropy() > 1.0f) && (filter == ITextureObject::CGL_ANISOTROPIC))
    {
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, mConfig.getCurrentAnisotropy());
        T->aniso_level = mConfig.getCurrentAnisotropy();
    }
#endif

	CATCH_GL_ERROR

	return T;
}

CTextureObject* const CTextureFactory::glCreateRectangleTexture( ITextureObject::TEXEL_TYPE type,
                                                                 CTextureObject::TEXTURE_FUNCTION env_mode,
														         ITextureObject::TEXTURE_FILTER filter)
{
#if defined(GL_ARB_texture_rectangle)
    if (!Raptor::glIsExtensionSupported("GL_ARB_texture_rectangle"))
    {
#ifdef RAPTOR_DEBUG_MODE_GENERATION
		vector<CRaptorMessages::MessageArgument> args;
		CRaptorMessages::MessageArgument arg;
		arg.arg_sz = "Texture Rectangle";
		args.push_back(arg);
        Raptor::GetErrorManager()->generateRaptorError(	CTextureFactory::CTextureFactoryClassID::GetClassId(),
														CRaptorErrorManager::RAPTOR_ERROR,
														CRaptorMessages::ID_FORMAT_NOT_SUPPORTED,args);
#endif
        return NULL;
    }

	CTextureObject* T = new CTextureObject(type);

	glGenTextures(1,&(T->texname));

    T->setFunction(env_mode);
	T->target = GL_TEXTURE_RECTANGLE_ARB;

	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, T->texname);

	if ((filter == ITextureObject::CGL_TRILINEAR) || (filter == ITextureObject::CGL_ANISOTROPIC))
		T->glvkUpdateFilter(ITextureObject::CGL_BILINEAR);
    else
		T->glvkUpdateFilter(filter);

	T->glvkUpdateClamping(CTextureObject::CGL_EDGECLAMP);

	glTexParameterf(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_PRIORITY, mConfig.getCurrentPriority());

#ifdef GL_EXT_texture_filter_anisotropic
	if ((mConfig.getCurrentAnisotropy() > 1.0f) && (filter == ITextureObject::CGL_ANISOTROPIC))
    {
		glTexParameterf(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAX_ANISOTROPY_EXT, mConfig.getCurrentAnisotropy());
        T->aniso_level = mConfig.getCurrentAnisotropy();
    }
#endif

	CATCH_GL_ERROR

	return T;
#else
    return NULL;
#endif
}


CTextureObject* const CTextureFactory::glCreateDynamicTexture(ITextureObject::TEXEL_TYPE type,
                                                              CTextureObject::TEXTURE_FUNCTION env_mode,
                                                              ITextureObject::TEXTURE_FILTER filter,
														      ITextureGenerator* pGenerator)
{
	if (pGenerator == NULL)
	{
#ifdef RAPTOR_DEBUG_MODE_GENERATION
        Raptor::GetErrorManager()->generateRaptorError(	CTextureFactory::CTextureFactoryClassID::GetClassId(),
														CRaptorErrorManager::RAPTOR_ERROR,
														CRaptorMessages::ID_NULL_OBJECT);
#endif
        return NULL;
	}

    if ((pGenerator->getKind() != ITextureGenerator::BUFFERED) && 
        (pGenerator->getKind() != ITextureGenerator::ANIMATED) && 
        (pGenerator->getKind() != ITextureGenerator::STATIC))
        return NULL;

	CTextureObject* T = new CTextureObject(type);

	glGenTextures(1,&(T->texname));

    T->setFunction(env_mode);
	T->target = GL_TEXTURE_2D;
	T->m_pTexelGenerator = pGenerator;

	glBindTexture(GL_TEXTURE_2D,T->texname);

	T->glvkUpdateFilter(filter);

	//	Dynamic textures are mostly used for render-to-texture buffers,
	//	frequent usage of these textures is clamped mode.
	//	Until there exist an interface to customize it, it is hard coded here.
	T->glvkUpdateClamping(CTextureObject::CGL_EDGECLAMP);

	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_PRIORITY,mConfig.getCurrentPriority());

	glResizeTexture(T,pGenerator->getGenerateWidth(),pGenerator->getGenerateHeight());
	T->setGenerationSize(0,0,T->getWidth(),T->getHeight());

	CATCH_GL_ERROR

	return T;
}

CTextureObject* const CTextureFactory::glCreateVolumeTexture(ITextureObject::TEXEL_TYPE type,
                                                             CTextureObject::TEXTURE_FUNCTION env_mode,
														     ITextureObject::TEXTURE_FILTER filter)
{
#if defined(GL_EXT_texture3D)
    if (!Raptor::glIsExtensionSupported("GL_EXT_texture3D"))
    {
#ifdef RAPTOR_DEBUG_MODE_GENERATION
		vector<CRaptorMessages::MessageArgument> args;
		CRaptorMessages::MessageArgument arg;
		arg.arg_sz = "Texture 3D";
		args.push_back(arg);
		Raptor::GetErrorManager()->generateRaptorError(	CTextureFactory::CTextureFactoryClassID::GetClassId(),
														CRaptorErrorManager::RAPTOR_ERROR,
														CRaptorMessages::ID_FORMAT_NOT_SUPPORTED,args);
#endif
        return NULL;
    }
    
	CTextureObject* T = new CTextureObject(type);

	glGenTextures(1,&(T->texname));

    T->setFunction(env_mode);	
	T->target = GL_TEXTURE_3D_EXT;

	glBindTexture(GL_TEXTURE_3D_EXT,T->texname);

	T->glvkUpdateFilter(filter);

	glTexParameteri(GL_TEXTURE_3D_EXT,GL_TEXTURE_WRAP_S,GL_REPEAT);
	glTexParameteri(GL_TEXTURE_3D_EXT,GL_TEXTURE_WRAP_T,GL_REPEAT);
    glTexParameteri(GL_TEXTURE_3D_EXT,GL_TEXTURE_WRAP_R_EXT,GL_REPEAT);
	glTexParameterf(GL_TEXTURE_3D_EXT,GL_TEXTURE_PRIORITY,mConfig.getCurrentPriority());

	CATCH_GL_ERROR

	return T;
#else
    return NULL;
#endif
}


bool CTextureFactory::glLoadCompressedTexture(CTextureObject* const T,const std::string& fname)
{
	bool ret = false;

#ifdef GL_ARB_texture_compression

#ifdef RAPTOR_DEBUG_MODE_GENERATION
    if ((T == NULL) || (!glIsTexture(T->texname)))
	{
        Raptor::GetErrorManager()->generateRaptorError(	CTextureFactory::CTextureFactoryClassID::GetClassId(),
														CRaptorErrorManager::RAPTOR_WARNING,
														CRaptorMessages::ID_NULL_OBJECT);
		return false;
	}
#endif

	unsigned char *img = NULL;


	GLint params = 0;
	GLint size = 0;
	GLuint width = 0;
	GLuint height = 0;
    
    CRaptorIO* IO = CRaptorIO::Create(fname.data(),CRaptorIO::DISK_READ);
    ret = ((IO != NULL) && (IO->getStatus() == CRaptorIO::IO_OK));

    if (ret)
    {
        IO->read((unsigned char*)&width,sizeof(GLuint));
        ret = (IO->getStatus() == CRaptorIO::IO_OK);
    }

	if (ret)
	{
		IO->read((unsigned char*)&height, sizeof(GLuint));
		ret = (IO->getStatus() == CRaptorIO::IO_OK);
	}

    if (ret)
    {
        IO->read((unsigned char*)&params,sizeof(GLint));
        ret = (IO->getStatus() == CRaptorIO::IO_OK);
    }

    if (ret)
    {
        IO->read((unsigned char*)&size,sizeof(GLint));
        ret = (IO->getStatus() == CRaptorIO::IO_OK);
    }

    if (ret)
    {
		img = new unsigned char[size];
        IO->read(img,size);
        ret = (IO->getStatus() == CRaptorIO::IO_OK);
    }

    delete IO;

	//	If compressor available,
	// load texture to GL proxy
	int pos = 0;
	while ((pos < mConfig.getNumCompressors()) && 
		   (!mConfig.getCompressor(pos)->isCompressionSupported(params)))
		pos++;
	if (ret && (pos < mConfig.getNumCompressors()))
	{
		glBindTexture(GL_TEXTURE_2D,T->texname);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_PRIORITY,mConfig.getCurrentPriority());

		Raptor::glGetExtensions()->glCompressedTexImage2DARB(GL_TEXTURE_2D,
															 0,params,
															 width,
                                                             height,
															 0,size,img);
	}
    else
    {
		ret = false;
#ifdef RAPTOR_DEBUG_MODE_GENERATION
        Raptor::GetErrorManager()->generateRaptorError(CTextureFactory::CTextureFactoryClassID::GetClassId(),
                                                       CRaptorErrorManager::RAPTOR_WARNING,
                                                       CRaptorMessages::ID_NO_RESOURCE);
#endif
    }

	delete [] img;

	CATCH_GL_ERROR;
#endif

	return ret;
}

bool CTextureFactory::glExportCompressedTexture(const std::string& fname,const CTextureObject *T)
{
	bool ret = false;
#ifdef GL_ARB_texture_compression
	if (T != NULL)
	{
		glBindTexture(GL_TEXTURE_2D,T->texname);

		GLint params = 0;
		glGetTexLevelParameteriv(GL_TEXTURE_2D,0,GL_TEXTURE_COMPRESSED_ARB,&params);
		if (params != 0)
		{
			GLint size = 0;
			GLuint width = T->getWidth();
			GLuint height = T->getHeight();
			glGetTexLevelParameteriv(GL_TEXTURE_2D,0,GL_TEXTURE_INTERNAL_FORMAT,&params);
			glGetTexLevelParameteriv(GL_TEXTURE_2D,0,GL_TEXTURE_COMPRESSED_IMAGE_SIZE_ARB,&size);

			unsigned char *img = NULL;

            CRaptorIO* IO = CRaptorIO::Create(fname.data(),CRaptorIO::DISK_WRITE);
            ret = ((IO != NULL) && (IO->getStatus() == CRaptorIO::IO_OK));

            if (ret)
            {
				IO->write((unsigned char*)(&width), sizeof(GLuint));
                ret = (IO->getStatus() == CRaptorIO::IO_OK);
            }

			if (ret)
			{
				IO->write((unsigned char*)(&height), sizeof(GLuint));
				ret = (IO->getStatus() == CRaptorIO::IO_OK);
			}

            if (ret)
            {
                IO->write((unsigned char*)(&params),sizeof(GLint));
                ret = (IO->getStatus() == CRaptorIO::IO_OK);
            }

			img = new unsigned char[size];
			memset(img,0,size);
			Raptor::glGetExtensions()->glGetCompressedTexImageARB(GL_TEXTURE_2D,0,img);

            if (ret)
            {
			    IO->write((unsigned char*)(&size),sizeof(GLint));
                ret = (IO->getStatus() == CRaptorIO::IO_OK);
            }

            if (ret)
            {
			    IO->write(img,size);
                ret = (IO->getStatus() == CRaptorIO::IO_OK);
            }

            delete IO;
    		delete [] img;
		}
	}

	CATCH_GL_ERROR;
#endif
	return ret;
}




