// TextureFactoryConfig.cpp: implementation of the CTextureFactoryConfig class.
//
//////////////////////////////////////////////////////////////////////
#include "Subsys/CodeGeneration.h"

#ifndef __CGLTYPES_HPP__
	#include "System/CGLTypes.h"
#endif

#if !defined(AFX_TEXTUREFACTORYCONFIG_H__7A20D208_423F_4E02_AA4D_D736E0A7959F__INCLUDED_)
	#include "TextureFactoryConfig.h"
#endif

#ifndef __GLOBAL_H__
	#include "System/Global.h"
#endif

#if !defined(AFX_RAPTOREXTENSIONS_H__E5B5A1D9_60F8_4E20_B4E1_8E5A9CB7E0EB__INCLUDED_)
	#include "System/RaptorExtensions.h"
#endif

#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
	#include "System/Raptor.h"
#endif

#if !defined(AFX_PERSISTENCE_H__5561BA28_831B_11D3_9142_EEB51CEBBDB0__INCLUDED_)
	#include "Persistence.h"
#endif
#if !defined(AFX_TEXTUREFACTORY_H__1B470EC4_4B68_11D3_9142_9A502CBADC6B__INCLUDED_)
	#include "TextureFactory.h"
#endif

//! Default Imaging functionnalities
#if !defined(AFX_BUFFERIMAGE_H__B28C75CD_81D5_473F_A247_608FB6E02949__INCLUDED_)
    #include "Subsys/BufferImage.h"
#endif
#if !defined(AFX_DEFAULTBUMPMAPLOADER_H__3841D5F8_284B_4DC5_9E4B_56EF18AF80F4__INCLUDED_)
    #include "Subsys/DefaultBumpmapLoader.h"
#endif
#if !defined(AFX_DEFAULTIMAGESCALER_H__E3E63A13_79FC_4E46_A1D5_BCD41CF86360__INCLUDED_)
    #include "Subsys/DefaultImageScaler.h"
#endif
#if !defined(AFX_DEFAULTMIPMAPBUILDER_H__9C508D96_B614_4920_8816_B670295B6CE2__INCLUDED_)
    #include "Subsys/DefaultMipmapBuilder.h"
#endif

RAPTOR_NAMESPACE

class COpenGLCompressor : public CTextureFactoryConfig::ICompressor
{
public:
	COpenGLCompressor() {};
	virtual ~COpenGLCompressor() {};

	virtual std::string getName(void) const
	{ return "OpenGL"; };

	// TODO : check all compatible inner formats
	virtual unsigned int getFormat(unsigned int innerFormat) const
	{
#if defined(GL_ARB_texture_compression)
		if (innerFormat == GL_ALPHA8)
			return GL_COMPRESSED_ALPHA_ARB;
		else if (innerFormat == GL_INTENSITY8)
			return GL_COMPRESSED_INTENSITY_ARB;
		else if (innerFormat == GL_RGB8)
			return GL_COMPRESSED_RGB_ARB;
#if defined(GL_ARB_texture_rg)
		else if (innerFormat == GL_RED)
			return GL_COMPRESSED_RED_ARB;
		else if (innerFormat == GL_RG)
			return GL_COMPRESSED_RG_ARB;
#endif
#if defined(GL_VERSION_2_1)
		else if (innerFormat == GL_SRGB8)
			return GL_COMPRESSED_SRGB;
		else if (innerFormat == GL_SRGB8_ALPHA8)
			return GL_COMPRESSED_SRGB_ALPHA;
		else if (innerFormat == GL_SLUMINANCE8)
			return GL_COMPRESSED_SLUMINANCE;
		else if (innerFormat == GL_SLUMINANCE8_ALPHA8)
			return GL_COMPRESSED_SLUMINANCE_ALPHA;
#endif
		else if (innerFormat == GL_RGBA8)
			return GL_COMPRESSED_RGBA_ARB;
		else if (innerFormat == GL_LUMINANCE8)
			return GL_COMPRESSED_LUMINANCE_ARB;
		else if (innerFormat == GL_LUMINANCE8_ALPHA8)
			return GL_COMPRESSED_LUMINANCE_ALPHA_ARB;
		else
#endif
			return (unsigned int)-1;
	};

	bool isCompressionSupported(unsigned int compressedFormat) const
	{
#if defined(GL_ARB_texture_compression)
		return ((compressedFormat == GL_COMPRESSED_RGB_ARB) ||
				(compressedFormat == GL_COMPRESSED_ALPHA_ARB) ||
				(compressedFormat == GL_COMPRESSED_INTENSITY_ARB) ||
#if defined(GL_ARB_texture_rg)
				(compressedFormat == GL_COMPRESSED_RED_ARB) ||
				(compressedFormat == GL_COMPRESSED_RG_ARB) ||
#endif
#if defined(GL_VERSION_2_1)
				(compressedFormat == GL_COMPRESSED_SRGB) ||
				(compressedFormat == GL_COMPRESSED_SRGB_ALPHA) ||
				(compressedFormat == GL_COMPRESSED_SLUMINANCE) ||
				(compressedFormat == GL_COMPRESSED_SLUMINANCE_ALPHA) ||
#endif
				(compressedFormat == GL_COMPRESSED_RGBA_ARB) ||
				(compressedFormat == GL_COMPRESSED_LUMINANCE_ARB) ||
				(compressedFormat == GL_COMPRESSED_LUMINANCE_ALPHA_ARB));
#else
		return false;
#endif
	}
};

class CNullCompressor : public CTextureFactoryConfig::ICompressor
{
public:
	CNullCompressor() {};
	virtual ~CNullCompressor() {};

	virtual std::string getName(void) const
	{ return ""; };

	virtual unsigned int getFormat(unsigned int ) const
	{	return (unsigned int)-1; };

	bool isCompressionSupported(unsigned int ) const
	{	return false; 	}
};

class CDXT1Compressor : public CTextureFactoryConfig::ICompressor
{
public:
	CDXT1Compressor() {};
	virtual ~CDXT1Compressor() {};

	virtual std::string getName(void) const
	{ return "DXT1"; };

	// TODO : check all compatible inner formats
	virtual unsigned int getFormat(unsigned int innerFormat) const
	{
#if (defined(GL_EXT_texture_compression_s3tc) || defined(GL_NV_texture_compression_vtc) || defined(GL_EXT_texture_compression_dxt1))
		if (innerFormat == GL_RGB8)
			return GL_COMPRESSED_RGB_S3TC_DXT1_EXT;
#endif
#if (defined(GL_EXT_texture_compression_s3tc) || defined(GL_NV_texture_compression_vtc))
		else if (innerFormat == GL_RGBA8)
			// Is this a best compressor than GL_COMPRESSED_RGBA_S3TC_DXT3_EXT?
			return GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
		else
#elif defined(GL_EXT_texture_compression_dxt1)
		else if (innerFormat == GL_RGBA8)
			return GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
		else
#endif
			return (unsigned int)-1;
	};

	bool isCompressionSupported(unsigned int compressedFormat) const
	{
#if defined(GL_ARB_texture_compression)
#if (defined(GL_EXT_texture_compression_s3tc) || defined(GL_NV_texture_compression_vtc) || defined(GL_EXT_texture_compression_dxt1))
		return ((compressedFormat == GL_COMPRESSED_RGB_S3TC_DXT1_EXT) ||
				(compressedFormat == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ||
#endif
#if (defined(GL_EXT_texture_compression_s3tc) || defined(GL_NV_texture_compression_vtc))
				(compressedFormat == GL_COMPRESSED_RGBA_S3TC_DXT3_EXT) ||
				(compressedFormat == GL_COMPRESSED_RGBA_S3TC_DXT5_EXT));
#else // dxt1 only
				false;
#endif
#else
		return false;
#endif
	}
};

class CRGTCCompressor : public CTextureFactoryConfig::ICompressor
{
public:
	CRGTCCompressor() {};
	virtual ~CRGTCCompressor() {};

	virtual std::string getName(void) const
	{ return "RGTC"; };

	// TODO : check all compatible inner formats
	virtual unsigned int getFormat(unsigned int innerFormat) const
	{
		// How to provide signed versions ?
#if defined(GL_ARB_texture_compression_rgtc)
		if (innerFormat == GL_LUMINANCE8)
			return GL_COMPRESSED_RED_RGTC1_ARB;
		else if (innerFormat == GL_LUMINANCE8_ALPHA8)
			return GL_COMPRESSED_RG_RGTC2_ARB;
		else
#elif defined(GL_EXT_texture_compression_rgtc)
		if (innerFormat == GL_LUMINANCE8)
			return GL_COMPRESSED_RED_RGTC1_EXT;
		else if (innerFormat == GL_LUMINANCE8_ALPHA8)
			return GL_COMPRESSED_RED_GREEN_RGTC2_EXT;
		else
#endif
			return (unsigned int)-1;
	};

	bool isCompressionSupported(unsigned int compressedFormat) const
	{
#if defined(GL_ARB_texture_compression_rgtc)
		return ((compressedFormat == GL_COMPRESSED_RED_RGTC1_ARB) ||
				(compressedFormat == GL_COMPRESSED_SIGNED_RED_RGTC1_ARB) ||
				(compressedFormat == GL_COMPRESSED_RG_RGTC2_ARB) ||
				(compressedFormat == GL_COMPRESSED_SIGNED_RG_RGTC2_ARB));
#elif defined(GL_EXT_texture_compression_rgtc)
		return ((compressedFormat == GL_COMPRESSED_RED_RGTC1_EXT) ||
				(compressedFormat == GL_COMPRESSED_SIGNED_RED_RGTC1_EXT) ||
				(compressedFormat == GL_COMPRESSED_RED_GREEN_RGTC2_EXT) ||
				(compressedFormat == GL_COMPRESSED_SIGNED_RED_GREEN_RGTC2_EXT));
#else
		return false;
#endif
	}
};

class CFXT1Compressor : public CTextureFactoryConfig::ICompressor
{
public:
	CFXT1Compressor() {};
	virtual ~CFXT1Compressor() {};

	virtual std::string getName(void) const
	{ return "FXT1"; };

	// TODO : check all compatible inner formats
	virtual unsigned int getFormat(unsigned int innerFormat) const
	{
#if defined(GL_3DFX_texture_compression_FXT1)
		if (innerFormat == GL_RGB8)
			return GL_COMPRESSED_RGB_FXT1_3DFX;
		else if (innerFormat == GL_RGBA8)
			return GL_COMPRESSED_RGBA_FXT1_3DFX;
		else
#endif
			return (unsigned int)-1;
	};

	bool isCompressionSupported(unsigned int compressedFormat) const
	{
#if defined(GL_3DFX_texture_compression_FXT1)
		return ((compressedFormat == GL_COMPRESSED_RGB_FXT1_3DFX) ||
				(compressedFormat == GL_COMPRESSED_RGBA_FXT1_3DFX));
#else
		return false;
#endif
	}
};

class CLATCCompressor : public CTextureFactoryConfig::ICompressor
{
public:
	CLATCCompressor() {};
	virtual ~CLATCCompressor() {};

	virtual std::string getName(void) const
	{ return "LATC"; };

	// TODO : check all compatible inner formats
	virtual unsigned int getFormat(unsigned int innerFormat) const
	{
		// How to provide signed versions ?
#if defined(GL_EXT_texture_compression_latc)
		if (innerFormat == GL_LUMINANCE8)
			return GL_COMPRESSED_LUMINANCE_LATC1_EXT;
		else if (innerFormat == GL_LUMINANCE8_ALPHA8)
			return GL_COMPRESSED_LUMINANCE_ALPHA_LATC2_EXT;
		else if (innerFormat == GL_SLUMINANCE8)
			return GL_COMPRESSED_SIGNED_LUMINANCE_LATC1_EXT;
		else if (innerFormat == GL_SLUMINANCE8_ALPHA8)
			return GL_COMPRESSED_SIGNED_LUMINANCE_ALPHA_LATC2_EXT;
		else
#endif
			return (unsigned int)-1;
	};

	bool isCompressionSupported(unsigned int compressedFormat) const
	{
#if defined(GL_EXT_texture_compression_latc)
		return ((compressedFormat == GL_COMPRESSED_LUMINANCE_LATC1_EXT) ||
				(compressedFormat == GL_COMPRESSED_SIGNED_LUMINANCE_LATC1_EXT) ||
				(compressedFormat == GL_COMPRESSED_LUMINANCE_ALPHA_LATC2_EXT) ||
				(compressedFormat == GL_COMPRESSED_SIGNED_LUMINANCE_ALPHA_LATC2_EXT));
#else
		return false;
#endif
	}
};

class CBPTCCompressor : public CTextureFactoryConfig::ICompressor
{
public:
	CBPTCCompressor() {};
	virtual ~CBPTCCompressor() {};

	virtual std::string getName(void) const
	{ return "BPTC"; };

	// TODO : check all compatible inner formats
	virtual unsigned int getFormat(unsigned int innerFormat) const
	{
		// How to select compression sub-options ?
#if defined(GL_ARB_texture_compression_bptc)
		if (innerFormat == GL_RGBA8)
			return GL_COMPRESSED_RGBA_BPTC_UNORM_ARB;
		else if (innerFormat == GL_SRGB8_ALPHA8)
			return GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM_ARB;
		else if (innerFormat == GL_RGB32F_ARB)
			return GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT_ARB;
		/*else if (innerFormat == GL_SRGB8_ALPHA8)
			return GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT_ARB;*/
		else
#endif
			return (unsigned int)-1;
	};

	bool isCompressionSupported(unsigned int compressedFormat) const
	{
#if defined(GL_ARB_texture_compression_bptc)
		return ((compressedFormat == GL_COMPRESSED_RGBA_BPTC_UNORM_ARB) ||
				(compressedFormat == GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM_ARB) ||
				(compressedFormat == GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT_ARB) ||
				(compressedFormat == GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT_ARB));
#else
		return false;
#endif
	}
};

class CASTCCompressor : public CTextureFactoryConfig::ICompressor
{
public:
	CASTCCompressor() {};
	virtual ~CASTCCompressor() {};

	virtual std::string getName(void) const
	{ return "ASTC"; };

	// TODO : check all compatible inner formats
	virtual unsigned int getFormat(unsigned int innerFormat) const
	{
#if (defined(GL_KHR_texture_compression_astc_hdr) || defined(GL_KHR_texture_compression_astc_ldr))
		if (innerFormat == GL_RGBA8)
			return GL_COMPRESSED_RGBA_ASTC_4x4_KHR;
		else if (innerFormat == GL_SRGB8_ALPHA8)
			return GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR;
		else
#endif
			return (unsigned int)-1;
	}

	bool isCompressionSupported(unsigned int compressedFormat) const
	{
#if (defined(GL_KHR_texture_compression_astc_hdr) || defined(GL_KHR_texture_compression_astc_ldr))
		return ((compressedFormat == GL_COMPRESSED_RGBA_ASTC_4x4_KHR) ||
				(compressedFormat == GL_COMPRESSED_RGBA_ASTC_5x4_KHR) ||
				(compressedFormat == GL_COMPRESSED_RGBA_ASTC_5x5_KHR) ||
				(compressedFormat == GL_COMPRESSED_RGBA_ASTC_6x5_KHR) ||
				(compressedFormat == GL_COMPRESSED_RGBA_ASTC_6x6_KHR) ||
				(compressedFormat == GL_COMPRESSED_RGBA_ASTC_8x5_KHR) ||
				(compressedFormat == GL_COMPRESSED_RGBA_ASTC_8x6_KHR) ||
				(compressedFormat == GL_COMPRESSED_RGBA_ASTC_8x8_KHR) ||
				(compressedFormat == GL_COMPRESSED_RGBA_ASTC_10x5_KHR) ||
				(compressedFormat == GL_COMPRESSED_RGBA_ASTC_10x6_KHR) ||
				(compressedFormat == GL_COMPRESSED_RGBA_ASTC_10x8_KHR) ||
				(compressedFormat == GL_COMPRESSED_RGBA_ASTC_10x10_KHR) ||
				(compressedFormat == GL_COMPRESSED_RGBA_ASTC_12x10_KHR) ||
				(compressedFormat == GL_COMPRESSED_RGBA_ASTC_12x12_KHR) ||
				(compressedFormat == GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR) ||
				(compressedFormat == GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x4_KHR) ||
				(compressedFormat == GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5_KHR) ||
				(compressedFormat == GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x5_KHR) ||
				(compressedFormat == GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR) ||
				(compressedFormat == GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x5_KHR) ||
				(compressedFormat == GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x6_KHR) ||
				(compressedFormat == GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR) ||
				(compressedFormat == GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x5_KHR) ||
				(compressedFormat == GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x6_KHR) ||
				(compressedFormat == GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x8_KHR) ||
				(compressedFormat == GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR) ||
				(compressedFormat == GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x10_KHR) ||
				(compressedFormat == GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR));
#else
		return false;
#endif
	}
};

static const int MAX_COMPRESSORS = 8;
static CTextureFactoryConfig::ICompressor *knownCompressors[MAX_COMPRESSORS] = 
{
	new CNullCompressor(),
	new COpenGLCompressor(),
	new CDXT1Compressor(),
	new CRGTCCompressor(),
	new CFXT1Compressor(),
	new CLATCCompressor(),
	new CBPTCCompressor(),
	new CASTCCompressor()
	// new COESPalettedCompressor()
};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTextureFactoryConfig::CTextureFactoryConfig()
{
	m_nbCompressors = -1;
	m_nbTextureImages = 0;
	m_pCompressors = NULL;
	m_pCurrentCompressor = NULL;
	m_fMaxAnisotropy = 1.0f;
	m_bSupportResize = false;

    m_fBumpScale = 1.0f;
	m_fCurrentPriority = 1.0f;
	m_fCurrentAnisotropy = 1.0f;
	m_fMaxAnisotropy = 1.0f;
	m_bResizeTextures = true;
	m_bSupportResize = false;
    m_nbTextureImages = 0;
	m_texelFormat = BYTEORDER_RGBA;

	Global::RAPTOR_CURRENT_STATUS &status = Global::GetInstance().getCurrentStatus();

	IImageIO *pIO = new CBufferImage();
	vector<std::string> exts = pIO->getImageKind();
	for (size_t j=0;j<exts.size();j++)
		IMAGE_KIND_IO.insert(map<std::string,IImageIO*>::value_type(exts[j],pIO));

	IMAGE_KIND_OP.insert(map<IImageOP::OP_KIND,IImageOP*>::value_type(IImageOP::BUMPMAP_LOADER,status.pDefaultBumpmapLoader));
	IMAGE_KIND_OP.insert(map<IImageOP::OP_KIND,IImageOP*>::value_type(IImageOP::IMAGE_SCALER,status.pDefaultImageScaler));
	IMAGE_KIND_OP.insert(map<IImageOP::OP_KIND,IImageOP*>::value_type(IImageOP::MIPMAP_BUILDER,status.pDefaultMipmapBuilder));
}

CTextureFactoryConfig::~CTextureFactoryConfig()
{
#if defined(GL_ARB_texture_compression)
    if (m_pCompressors != NULL)
		delete [] m_pCompressors;
#endif
}


bool CTextureFactoryConfig::glInit()
{
#if defined(GL_ARB_multitexture)
    //! Initialise a default value to the number of TMU
	glGetIntegerv(GL_MAX_TEXTURE_UNITS_ARB,&m_nbTextureImages);
#endif
#if defined(GL_ARB_fragment_program) || defined(GL_ARB_vertex_shader) || defined(GL_ARB_fragment_shader)
    glGetIntegerv(GL_MAX_TEXTURE_COORDS_ARB,&m_nbTextureImages);
    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS_ARB,&m_nbTextureImages);
#endif

	m_pCurrentCompressor = NULL;
	if (NULL != m_pCompressors)
		delete [] m_pCompressors;

#if defined(GL_ARB_texture_compression)
	if (Raptor::glIsExtensionSupported("GL_ARB_texture_compression"))
	{
		m_nbCompressors = 0;
		glHint(GL_TEXTURE_COMPRESSION_HINT_ARB,GL_NICEST);

		// if compression is supported, add 2 default compressors
		bool compressors[MAX_COMPRESSORS] = { true, true, false, false, false, false, false };
		int nbCompressors = 0;
		glGetIntegerv(GL_NUM_COMPRESSED_TEXTURE_FORMATS_ARB,&nbCompressors);
		if (nbCompressors > 0)
		{
			int *pCompressors = new int[nbCompressors];
			glGetIntegerv(GL_COMPRESSED_TEXTURE_FORMATS_ARB,pCompressors);

			for (int i=0;i<nbCompressors;i++)
			{
				for (int j=2;j<MAX_COMPRESSORS;j++)
				{
					if (knownCompressors[j]->isCompressionSupported(pCompressors[i]))
					{
						compressors[j] = true;
						break;
					}
				}
			}

			delete [] pCompressors;

			for (int j=0;j<MAX_COMPRESSORS;j++)
				if (compressors[j])
					m_nbCompressors++;

			m_pCompressors = new ICompressor*[m_nbCompressors];
			nbCompressors = 0;
			for (int j=0;j<MAX_COMPRESSORS;j++)
				if (compressors[j])
					m_pCompressors[nbCompressors++] = knownCompressors[j];
		}
	}
	else
		m_nbCompressors = 0;
#else
	m_nbCompressors = 0;
#endif

#ifdef GL_EXT_texture_filter_anisotropic
	if (Raptor::glIsExtensionSupported("GL_EXT_texture_filter_anisotropic"))
		glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT,&m_fMaxAnisotropy);
	else
		m_fMaxAnisotropy = 1.0f;
#endif

#ifdef GL_ARB_texture_non_power_of_two
	m_bSupportResize = Raptor::glIsExtensionSupported("GL_ARB_texture_non_power_of_two");
#endif

	CATCH_GL_ERROR

	return true;
}

void CTextureFactoryConfig::setCurrentPriority(float priority)
{
	m_fCurrentPriority = priority;
	if (m_fCurrentPriority > 1.0)
		m_fCurrentPriority = 1.0f;
	else if (m_fCurrentPriority < 0.0f)
		m_fCurrentPriority = 0.0f;
}

void CTextureFactoryConfig::setCurrentAnisotropy(float priority)
{
#ifdef GL_EXT_texture_filter_anisotropic
	m_fCurrentAnisotropy = priority;
	if (m_fCurrentAnisotropy < 1.0f)
		m_fCurrentAnisotropy = 1.0f;
	else if (m_fCurrentAnisotropy > m_fMaxAnisotropy)
		m_fCurrentAnisotropy = m_fMaxAnisotropy;
#else
	m_fCurrentAnisotropy = 1.0f;
#endif
}

void CTextureFactoryConfig::useTextureResize(bool resize)
{
#ifdef GL_ARB_texture_non_power_of_two
	if (m_bSupportResize)
		m_bResizeTextures = resize;
	else
		m_bResizeTextures = true;
#else
	m_bResizeTextures = true;
#endif
}


void CTextureFactoryConfig::setImageKindIO(IImageIO *imager)
{
#ifdef RAPTOR_DEBUG_MODE_GENERATION
    if ((imager == NULL) || (imager->getImageKind().empty()))
	{
		Raptor::GetErrorManager()->generateRaptorError(	CTextureFactory::CTextureFactoryClassID::GetClassId(),
														CRaptorErrorManager::RAPTOR_WARNING,
														CRaptorMessages::ID_PROCEDURE_FAILED);
		return;
	}
#endif

	vector<std::string> extensionKind = imager->getImageKind();
	for (size_t j=0;j<extensionKind.size();j++)
	{
		string ext;
		for (unsigned int i=0;i<extensionKind[j].size();i++)
			ext += toupper(extensionKind[j][i]);

		IMAGE_KIND_IO.insert(map<std::string,IImageIO*>::value_type(ext,imager));
	}
}

CTextureFactoryConfig::IImageIO* const CTextureFactoryConfig::getImageKindIO(const std::string &extension) const
{
	//	extract the right image loader
    string ext = extension;
	std::string::size_type pos = ext.rfind('.');
    if (pos < ext.size())
        ext = ext.substr(pos+1);

    pos = 0;
	for (pos=0;pos<ext.size();pos++)
        ext[pos] = toupper(ext[pos]);

	map<std::string,IImageIO*>::const_iterator itr = IMAGE_KIND_IO.find(ext);
	if (IMAGE_KIND_IO.end() != itr)
		return (*itr).second;
	else 
		return NULL;
}


void CTextureFactoryConfig::setImageKindOP(IImageOP *op)
{
#ifdef RAPTOR_DEBUG_MODE_GENERATION
    if (op == NULL)
	{
        Raptor::GetErrorManager()->generateRaptorError(	CTextureFactory::CTextureFactoryClassID::GetClassId(),
														CRaptorErrorManager::RAPTOR_WARNING,
														CRaptorMessages::ID_PROCEDURE_FAILED);
		return;
	}
#endif

	IMAGE_KIND_OP.insert(map<IImageOP::OP_KIND,IImageOP*>::value_type(op->getKind(),op));
}


CTextureFactoryConfig::IImageOP* const CTextureFactoryConfig::getImageKindOP(IImageOP::OP_KIND kind) const
{
    map<IImageOP::OP_KIND,IImageOP*>::const_iterator itr = IMAGE_KIND_OP.find(kind);
	if (IMAGE_KIND_OP.end() != itr)
		return (*itr).second;
	else 
		return NULL;
}


const CTextureFactoryConfig::ICompressor* CTextureFactoryConfig::getCompressor(unsigned int numCompressor)
{
	
    if (m_nbCompressors < 0)
        return NULL;
	else if ((int)numCompressor < m_nbCompressors)
		return m_pCompressors[numCompressor];
	else
		return NULL;
}

const CTextureFactoryConfig::ICompressor* CTextureFactoryConfig::getCompressor(const std::string& name)
{
	for (int i=0;i<m_nbCompressors;i++)
	{
		if (m_pCompressors[i]->getName() == name)
			return m_pCompressors[i];
	}

	return NULL;
}


