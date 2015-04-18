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

#if !defined(AFX_RAPTOREXTENSIONS_H__E5B5A1D9_60F8_4E20_B4E1_8E5A9CB7E0EB__INCLUDED_)
	#include "System/RaptorExtensions.h"
#endif

#if !defined(AFX_RAPTORIO_H__87D52C27_9117_4675_95DC_6AD2CCD2E78D__INCLUDED_)
	#include "System/RaptorIO.h"
#endif

#if !defined(AFX_TEXTUREFACTORYCONFIG_H__7A20D208_423F_4E02_AA4D_D736E0A7959F__INCLUDED_)
	#include "TextureFactoryConfig.h"
#endif

#ifndef __INTERNAL_PROCS_H__
	#include "Subsys/InternalProcs.h"
#endif

#if !defined(AFX_OBJECTFACTORY_H__7F891C52_9E32_489C_B09C_5E5803522D91__INCLUDED_)
	#include "ObjectFactory.h"
#endif

#if !defined(AFX_MEMORY_H__81A6CA9A_4ED9_4260_B6E4_C03276C38DBC__INCLUDED_)
	#include "System/Memory.h"
#endif

#if !defined(AFX_TEXELALLOCATOR_H__7C48808C_E838_4BE3_8B0E_286428BB7CF8__INCLUDED_)
	#include "Subsys/TexelAllocator.h"
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
	:CPersistence(factoryId,name),
	m_pConfig(NULL)
{
	m_pConfig = new CTextureFactoryConfig();

    if (m_pConfig->getNumCompressors() < 0)
        m_pConfig->glInit();
}

CTextureFactory::~CTextureFactory(void)
{
	if (m_pConfig != NULL)
		delete m_pConfig;
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



RAPTOR_HANDLE CTextureFactory::glPreloadTexture(CTextureObject* const T,
												const std::string &fname,
												unsigned int mode)
{
	RAPTOR_HANDLE preload(0,NULL);

	//	ensure we can do something ...
#ifdef RAPTOR_DEBUG_MODE_GENERATION
	if ((T == NULL) || (!glIsTexture(T->texname)))
	{
        Raptor::GetErrorManager()->generateRaptorError(	CTextureFactory::CTextureFactoryClassID::GetClassId(),
														CRaptorErrorManager::RAPTOR_WARNING,
														CRaptorMessages::ID_NULL_OBJECT);
		return preload;
	}
#endif

	if ((T->target >> 16) == CTextureGenerator::BUFFERED)
	{
#ifdef RAPTOR_DEBUG_MODE_GENERATION
        Raptor::GetErrorManager()->generateRaptorError(	CTextureFactory::CTextureFactoryClassID::GetClassId(),
														CRaptorErrorManager::RAPTOR_WARNING,
														CRaptorMessages::ID_UPDATE_FAILED);
#endif
		return preload;
	}

	CTextureFactoryConfig::CImageIO *imager = m_pConfig->getImageKindIO(fname);
	if (imager != NULL)
	{
		if (imager->loadImageFile(fname,T))
		{
			//
			//	Initialise texture attributes : main formats.
			//
			T->m_name = fname.data();
			TexturePreload P;
			P.inner_format = T->getSizedTexelType();
			P.format = T->getBufferType();
			P.src_format = T->getBufferTexelType();
			P.mode = mode;

			//	1)	Texture internal format
			//
			//	Handle normal maps for bumping
			//
			if ((mode & CGL_NORMAL_MAP) == CGL_NORMAL_MAP)
			{
				if ((mode & CGL_CREATE_NORMAL_MAP) == CGL_CREATE_NORMAL_MAP)
				{
                    CTextureFactoryConfig::CImageOP* op = m_pConfig->getImageKindOP(CTextureFactoryConfig::CImageOP::BUMPMAP_LOADER);
					op->apply(T,P.inner_format,P.format,P.src_format,*m_pConfig);
				}
			}

			//
			//	Handle support for texture compression.
			//	Rely on OpenGL 'generic' compression to choose
			//	appropriate compressor of hardware capabilities.
			//
#ifdef GL_ARB_texture_compression
			//	use texture compression if possible
			else if (NULL != m_pConfig->getCurrentCompressor())
			{
				GLuint compressor = m_pConfig->getCurrentCompressor()->getFormat(P.inner_format);
				if (compressor != (GLuint)-1)
					P.inner_format = compressor;
			}
#endif


			//	2)	Format of data to load
			//
			CTextureFactoryConfig::TEXEL_FORMAT texelFormat = m_pConfig->getTexelFormat();
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
            if (m_pConfig->useTextureResize())
            {
                CTextureFactoryConfig::CImageOP* op = m_pConfig->getImageKindOP(CTextureFactoryConfig::CImageOP::IMAGE_SCALER);
/* OGL call*/	op->apply(T,P.inner_format,P.format,P.src_format,*m_pConfig);
            }

			//
			//	Handle cubemap textures
			//
			P.target = T->target & 0xFFFF;
#if defined(GL_ARB_texture_cube_map)
			if (P.target == GL_TEXTURE_CUBE_MAP_ARB)
			{
				P.target = (mode & 0x0000C070);
				if ((P.target < CGL_CUBEMAP_PX) || (P.target > CGL_CUBEMAP_NZ))
					P.target = GL_TEXTURE_2D;
				else
					P.target = GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB + ((P.target - CGL_CUBEMAP_PX) >> 4);
			}
#endif
			preload.handle = T->texname;
			preload.hClass = 0;
			m_preloads[T->texname] = P;

			//
			// Store texels in a relocated chunk.
			//
			size_t size = 4 * T->getWidth() * T->getHeight() * MAX(1,T->getDepth());
			if (T->getTexels() != NULL)
			{
				unsigned char *const texPointer = CTexelAllocator::GetInstance()->allocateTexels(size);
				CTexelAllocator::GetInstance()->glCopyPointer(texPointer,T->getTexels(),size);
			}
			else if (T->getFloatTexels() != NULL)
			{
				float* const texPointer = CTexelAllocator::GetInstance()->allocateFloatTexels(size);
				CTexelAllocator::GetInstance()->glCopyPointer(	(unsigned char *)texPointer,
																(unsigned char *)T->getFloatTexels(),
																4*size);
			}
			T->releaseTexels();

			CATCH_GL_ERROR
		}
		else	/* load from file failed */
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

bool CTextureFactory::glLoadTexture(CTextureObject* const T, 
									RAPTOR_HANDLE preload)
{
	if (preload.handle == 0)
		return false;

	TexturePreload P;
	map<unsigned int,TexturePreload>::iterator it = m_preloads.find(preload.handle);
	if (it == m_preloads.end())
		return false;
	else
	{
		P = (*it).second;
		m_preloads.erase(it);
	}

	//
	//	Final processing : 
	//	- mipmapping generation
	//	- image loading
#if defined(GL_EXT_texture3D)
    if (T->m_depth > 0)   // Texture Volumes
    {
        const CRaptorExtensions * const extensions = Raptor::glGetExtensions();
        extensions->glTexImage3DEXT(P.target,0,
									P.inner_format,
                                    T->m_width,T->m_height,T->m_depth,
									0,P.format,P.src_format,
                                    T->texels);
    }
    else
#endif
	if (((P.mode & CGL_AUTO_MIPMAPPED) == CGL_AUTO_MIPMAPPED) &&
		(T->getFilter() == CTextureObject::CGL_TRILINEAR))
	{
		CTextureFactoryConfig::CImageOP* op = m_pConfig->getImageKindOP(CTextureFactoryConfig::CImageOP::MIPMAP_BUILDER);
		op->apply(T,P.inner_format,P.format,P.src_format,*m_pConfig);
	}
    //  The default case, at least load the texture.
	else /*if ((mode&CGL_USER_MIPMAPPED) == CGL_USER_MIPMAPPED) */
	{
		glTexImage2D(	P.target,
						T->level,P.inner_format,
						T->m_width,T->m_height,
						0,P.format,
						P.src_format,
						T->texels);
	}

	CATCH_GL_ERROR

	return true;
}




bool CTextureFactory::glLoadTexture(CTextureObject* const T,
									const std::string& fname,
									unsigned int mode)
{
	//	ensure we can do something ...
#ifdef RAPTOR_DEBUG_MODE_GENERATION
	if ((T == NULL) || (!glIsTexture(T->texname)))
	{
        Raptor::GetErrorManager()->generateRaptorError(	CTextureFactory::CTextureFactoryClassID::GetClassId(),
														CRaptorErrorManager::RAPTOR_WARNING,
														CRaptorMessages::ID_NULL_OBJECT);
		return false;
	}
#endif

	if ((T->target >> 16) == CTextureGenerator::BUFFERED)
	{
#ifdef RAPTOR_DEBUG_MODE_GENERATION
        Raptor::GetErrorManager()->generateRaptorError(	CTextureFactory::CTextureFactoryClassID::GetClassId(),
														CRaptorErrorManager::RAPTOR_WARNING,
														CRaptorMessages::ID_UPDATE_FAILED);
#endif
		return false;
	}


	CTextureFactoryConfig::CImageIO *imager = m_pConfig->getImageKindIO(fname);
	if (imager != NULL)
	{
		if (imager->loadImageFile(fname,T))
		{
			//
			//	Initialise texture attributes : main formats.
			//
			T->m_name = fname.data();
			GLuint GL_INNER_FORMAT = T->getSizedTexelType();
			GLuint GL_FORMAT = T->getBufferType();
			GLuint GL_SRC_FORMAT = T->getBufferTexelType();


			//	1)	Texture internal format
			//
			//	Handle normal maps for bumping
			//
			if ((mode & CGL_NORMAL_MAP) == CGL_NORMAL_MAP)
			{
				if ((mode & CGL_CREATE_NORMAL_MAP) == CGL_CREATE_NORMAL_MAP)
				{
                    CTextureFactoryConfig::CImageOP* op = m_pConfig->getImageKindOP(CTextureFactoryConfig::CImageOP::BUMPMAP_LOADER);
                    op->apply(T,GL_INNER_FORMAT,GL_FORMAT,GL_SRC_FORMAT,*m_pConfig);
				}
			}
			//
			//	Handle support for texture compression.
			//	Rely on OpenGL 'generic' compression to choose
			//	appropriate compressor of hardware capabilities.
			//
#ifdef GL_ARB_texture_compression
			//	use texture compression if possible
			else if (NULL != m_pConfig->getCurrentCompressor())
			{
				GLuint compressor = m_pConfig->getCurrentCompressor()->getFormat(GL_INNER_FORMAT);
				if (compressor != (GLuint)-1)
					GL_INNER_FORMAT = compressor;
			}
#endif


			//	2)	Format of data to load
			//
			CTextureFactoryConfig::TEXEL_FORMAT texelFormat = m_pConfig->getTexelFormat();
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
			//	Handle Image re-scaling
			//
            if (m_pConfig->useTextureResize() && (T->getDepth() == 0))
            {
                CTextureFactoryConfig::CImageOP* op = m_pConfig->getImageKindOP(CTextureFactoryConfig::CImageOP::IMAGE_SCALER);
                op->apply(T,GL_INNER_FORMAT,GL_FORMAT,GL_SRC_FORMAT,*m_pConfig);
            }

			//
			//	Handle cubemap textures
			//
			bool result = false;
			GLuint target = T->target & 0xFFFF;
#if defined(GL_ARB_texture_cube_map)
			if (target == GL_TEXTURE_CUBE_MAP_ARB)
			{
				target = (mode & 0x0000C070);
				if ((target < CGL_CUBEMAP_PX) || (target > CGL_CUBEMAP_NZ))
					target = GL_TEXTURE_2D;
				else
					target = GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB + ((target - CGL_CUBEMAP_PX) >> 4);
			}
#endif

			//
			//	Final processing :
			//	- mipmapping generation
			//	- image loading
#if defined(GL_EXT_texture3D)
            if (T->m_depth > 0)   // Texture Volumes
            {
                const CRaptorExtensions * const extensions = Raptor::glGetExtensions();
                extensions->glTexImage3DEXT( target,0,
                                             GL_INNER_FORMAT,
                                             T->m_width,T->m_height,T->m_depth,
                                             0,GL_FORMAT,GL_SRC_FORMAT,
                                             T->texels);
            }
            else
#endif
			if (((mode & CGL_AUTO_MIPMAPPED) == CGL_AUTO_MIPMAPPED) &&
				(T->getFilter() == CTextureObject::CGL_TRILINEAR))
			{
				CTextureFactoryConfig::CImageOP* op = m_pConfig->getImageKindOP(CTextureFactoryConfig::CImageOP::MIPMAP_BUILDER);
				result = op->apply(T,GL_INNER_FORMAT,GL_FORMAT,GL_SRC_FORMAT,*m_pConfig);
			}
            //  The default case, at least load the texture.
			else /*if ((mode&CGL_USER_MIPMAPPED) == CGL_USER_MIPMAPPED) */
			{
				glTexImage2D(	target,
								T->level,GL_INNER_FORMAT,
								T->m_width,T->m_height,
								0,GL_FORMAT,
								GL_SRC_FORMAT,
								T->texels);
				result = true;
			}
            T->releaseTexels();

			CATCH_GL_ERROR

			return result;
		}
		else	/* load from file failed */
        {
            CRaptorMessages::MessageArgument arg;
            arg.arg_sz = fname.data();
            vector<CRaptorMessages::MessageArgument> args;
            args.push_back(arg);
            Raptor::GetErrorManager()->generateRaptorError(	CTextureFactory::CTextureFactoryClassID::GetClassId(),
			    											CRaptorErrorManager::RAPTOR_WARNING,
				    										CRaptorMessages::ID_TEXTURE_MISS,args);
			return false;
        }
	}
	else	/*	Loader not found */
		return false;
}

bool CTextureFactory::glResizeTexture( CTextureObject *T, unsigned int width, unsigned int height, unsigned int depth) const
{
    if ((T == NULL) || (width == 0) || (height == 0))
        return false;

    if ((T->m_width == width) && (T->m_height == height) && (T->m_depth == depth))
        return true;

    GLint currentWidth = 0;
    GLint currentHeight = 0;

    GLenum target = T->target & 0xFFFF;

    //! For a cube map, the target must identify a specific cube face
    //! otherwise, getTexLevelParameter will raise an error.
    //! As a cube map must have six equal sized 2D textures, we can query only one
    //! no matter what is the current, the error will be raised at loading time.
#if defined(GL_ARB_texture_cube_map)
	if (target == GL_TEXTURE_CUBE_MAP_ARB)
	    target = GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB;
#endif
    
    glBindTexture((T->target & 0xFFFF),T->texname);

    glGetTexLevelParameteriv(target, T->level, GL_TEXTURE_WIDTH, &currentWidth);
    glGetTexLevelParameteriv(target, T->level, GL_TEXTURE_HEIGHT, &currentHeight);


	//!	Handle the case with unsupported rectangle texture
	if (!m_pConfig->supportTextureResize())
	{
		unsigned int w2 = width;
		unsigned int h2 = height;
		if (w2 > 0)
			w2 = 1 << (unsigned int)(log((float)width) / log(2.0f));
		if (h2 > 0)
			h2 = 1 << (unsigned int)(log((float)height) / log(2.0f));

		if ((w2 != width) || (h2 != height))
		{
			if (m_pConfig->useTextureResize())
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
			}
		}
	}

    //! Current depth is not needed, because it is considered 0 and valid for a 2D texture.
    //! Here under, the texture is non existent, there has been no loading.
//    if ((currentWidth == 0) || (currentHeight == 0))
    {
        T->m_width = width;
        T->m_height = height;
        T->m_depth = depth;

        if (T->m_pTexelGenerator != NULL)
            T->setGenerationSize(T->source[0],T->source[1],T->source[2],T->source[3]);

        if ((T->target & 0xFFFF) == GL_TEXTURE_1D)
			glTexImage1D(	GL_TEXTURE_1D, 
							T->getCurrentMipMapLevel(), 
							T->getSizedTexelType(), 
							width, 
							0, 
							T->getBufferType(), 
							T->getBufferTexelType(),
							NULL);
        else if ((T->target & 0xFFFF) == GL_TEXTURE_2D)
            glTexImage2D(	GL_TEXTURE_2D, 
							T->getCurrentMipMapLevel(), 
							T->getSizedTexelType(), 
							width, height, 
							0, 
							T->getBufferType(), 
							T->getBufferTexelType(),
							NULL);
#if defined(GL_EXT_texture3D)
        else if ((T->target & 0xFFFF) == GL_TEXTURE_3D_EXT)
        {
            const CRaptorExtensions * const extensions = Raptor::glGetExtensions();
            extensions->glTexImage3DEXT(	GL_TEXTURE_3D_EXT, 
											T->getCurrentMipMapLevel(), 
											T->getSizedTexelType(), 
											width, 
											height, 
											depth, 
											0, 
											T->getBufferType(), 
											T->getBufferTexelType(),
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
    if (T == NULL)
        return false;

    if (T->texname == 0)
        return false;

    if ((T->m_width == 0) || (T->m_height == 0))
        return false;

	CTextureFactoryConfig::CImageIO *imager = m_pConfig->getImageKindIO(fname);
    bool res = true;
	if (imager != NULL)
	{
        T->glRender();

		unsigned char *texels = T->getTexels();
        if (texels == NULL)
        {
            T->allocateTexels();
			texels = T->getTexels();
        }

        glGetTexImage(GL_TEXTURE_2D,T->level,GL_RGBA,GL_UNSIGNED_BYTE,texels);

        res = imager->storeImageFile(fname,T);
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

	T->glUpdateFilter(CTextureObject::CGL_UNFILTERED);
	T->glUpdateClamping(CTextureObject::CGL_CLAMP);
	
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_PRIORITY,m_pConfig->getCurrentPriority());

	CATCH_GL_ERROR

	return T;
}

CTextureObject* const CTextureFactory::glCreateCubemap(  CTextureObject::TEXEL_TYPE type,
                                                         CTextureObject::TEXTURE_FUNCTION env_mode,
														 CTextureObject::TEXTURE_FILTER filter)
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

	glBindTexture(T->target,T->texname);

	T->glUpdateFilter(filter);
	T->glUpdateClamping(CTextureObject::CGL_REPEAT);

	glTexParameterf(T->target,GL_TEXTURE_PRIORITY,m_pConfig->getCurrentPriority());

	// Is this usefull on a cube map ?
#ifdef GL_EXT_texture_filter_anisotropic
	if ((m_pConfig->getCurrentAnisotropy() > 1.0f) && (filter == CTextureObject::CGL_ANISOTROPIC))
    {
		glTexParameterf(T->target,GL_TEXTURE_MAX_ANISOTROPY_EXT,m_pConfig->getCurrentAnisotropy());
        T->aniso_level = m_pConfig->getCurrentAnisotropy();
    }
#endif

	CATCH_GL_ERROR

	return T;
#else
    return NULL;
#endif
}

CTextureObject* const CTextureFactory::glCreateTexture( CTextureObject::TEXEL_TYPE type,
                                                        CTextureObject::TEXTURE_FUNCTION env_mode,
														CTextureObject::TEXTURE_FILTER filter)
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
	if ((type == CTextureObject::CGL_DEPTH24_STENCIL8) &&
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

	glBindTexture(T->target,T->texname);

	T->glUpdateFilter(filter);
	T->glUpdateClamping(CTextureObject::CGL_REPEAT);
	
	glTexParameterf(T->target,GL_TEXTURE_PRIORITY,m_pConfig->getCurrentPriority());

#ifdef GL_EXT_texture_filter_anisotropic
	if ((m_pConfig->getCurrentAnisotropy() > 1.0f) && (filter == CTextureObject::CGL_ANISOTROPIC))
    {
		glTexParameterf(T->target,GL_TEXTURE_MAX_ANISOTROPY_EXT,m_pConfig->getCurrentAnisotropy());
        T->aniso_level = m_pConfig->getCurrentAnisotropy();
    }
#endif

	CATCH_GL_ERROR

	return T;
}

CTextureObject* const CTextureFactory::glCreateRectangleTexture( CTextureObject::TEXEL_TYPE type,
                                                                 CTextureObject::TEXTURE_FUNCTION env_mode,
														         CTextureObject::TEXTURE_FILTER filter)
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
	glBindTexture(T->target,T->texname);

	if ((filter == CTextureObject::CGL_TRILINEAR) || (filter == CTextureObject::CGL_ANISOTROPIC))
        T->glUpdateFilter(CTextureObject::CGL_BILINEAR);
    else
        T->glUpdateFilter(filter);

	T->glUpdateClamping(CTextureObject::CGL_EDGECLAMP);

	glTexParameterf(T->target,GL_TEXTURE_PRIORITY,m_pConfig->getCurrentPriority());

#ifdef GL_EXT_texture_filter_anisotropic
	if ((m_pConfig->getCurrentAnisotropy() > 1.0f) && (filter == CTextureObject::CGL_ANISOTROPIC))
    {
		glTexParameterf(T->target,GL_TEXTURE_MAX_ANISOTROPY_EXT,m_pConfig->getCurrentAnisotropy());
        T->aniso_level = m_pConfig->getCurrentAnisotropy();
    }
#endif

	CATCH_GL_ERROR

	return T;
#else
    return NULL;
#endif
}


CTextureObject* const CTextureFactory::glCreateDynamicTexture(CTextureObject::TEXEL_TYPE type,
                                                              CTextureObject::TEXTURE_FUNCTION env_mode,
                                                              CTextureObject::TEXTURE_FILTER filter,
														      CTextureGenerator* pGenerator)
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

    if ((pGenerator->getKind() != CTextureGenerator::BUFFERED) && 
        (pGenerator->getKind() != CTextureGenerator::ANIMATED) && 
        (pGenerator->getKind() != CTextureGenerator::STATIC))
        return NULL;

	CTextureObject* T = new CTextureObject(type);

	glGenTextures(1,&(T->texname));

    T->setFunction(env_mode);
		
    T->target = GL_TEXTURE_2D | (pGenerator->getKind() << 16);
	T->m_pTexelGenerator = pGenerator;

	glBindTexture(GL_TEXTURE_2D,T->texname);

	T->glUpdateFilter(filter);

	//	Dynamic textures are mostly used for render-to-texture buffers,
	//	frequent usage of these textures is clamped mode.
	//	Until there exist an interface to customize it, it is hard coded here.
	//T->glUpdateClamping(CTextureObject::CGL_CLAMP);
	T->glUpdateClamping(CTextureObject::CGL_EDGECLAMP);

	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_PRIORITY,m_pConfig->getCurrentPriority());

    //  Initialise texture object with default value to store width & height
    //glTexImage2D(GL_TEXTURE_2D, 0, 4, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE,NULL);

	this->glResizeTexture(T,pGenerator->getGenerateWidth(),pGenerator->getGenerateHeight());
	T->setGenerationSize(0,0,T->getWidth(),T->getHeight());

	CATCH_GL_ERROR

	return T;
}

CTextureObject* const CTextureFactory::glCreateVolumeTexture(CTextureObject::TEXEL_TYPE type,
                                                             CTextureObject::TEXTURE_FUNCTION env_mode,
														     CTextureObject::TEXTURE_FILTER filter)
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
	T->m_pTexelGenerator = NULL;

	glBindTexture(GL_TEXTURE_3D_EXT,T->texname);

	T->glUpdateFilter(filter);

	glTexParameteri(GL_TEXTURE_3D_EXT,GL_TEXTURE_WRAP_S,GL_REPEAT);
	glTexParameteri(GL_TEXTURE_3D_EXT,GL_TEXTURE_WRAP_T,GL_REPEAT);
    glTexParameteri(GL_TEXTURE_3D_EXT,GL_TEXTURE_WRAP_R_EXT,GL_REPEAT);
	glTexParameterf(GL_TEXTURE_3D_EXT,GL_TEXTURE_PRIORITY,m_pConfig->getCurrentPriority());

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
	while ((pos < m_pConfig->getNumCompressors()) && 
		   (!m_pConfig->getCompressor(pos)->isCompressionSupported(params)))
		pos++;
	if (ret && (pos < m_pConfig->getNumCompressors()))
	{
		glBindTexture(GL_TEXTURE_2D,T->texname);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_PRIORITY,m_pConfig->getCurrentPriority());

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




