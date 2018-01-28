// TextureFactoryConfig.h: interface for the CTextureFactoryConfig class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TEXTUREFACTORYCONFIG_H__7A20D208_423F_4E02_AA4D_D736E0A7959F__INCLUDED_)
#define AFX_TEXTUREFACTORYCONFIG_H__7A20D208_423F_4E02_AA4D_D736E0A7959F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


RAPTOR_NAMESPACE_BEGIN

class CTextureObject;

class RAPTOR_API CTextureFactoryConfig  
{
public:

	//! This class is an interface to Texture Compressor Services
	class ICompressor
	{
	public:
		virtual std::string getName(void) const = 0;

		//!	This methods shall return a compressed pixel formats matching 
		//! the pixel format passed as parameter.
		//! @param innerFormat: the 'uncompressed' texel format
		//! @return the 'compressed' equivalent texel format if available, (UINT)-1 if none available
		virtual unsigned int getFormat(unsigned int innerFormat) const = 0;

		//! This method checks supported compressed pixel formats.
		//! @param compressedFormat: a compressed pixel format ( that can be returned by the compressor with getFormat )
		//! @return true if the compression is supported
		virtual bool isCompressionSupported(unsigned int compressedFormat) const = 0;

	protected:
		ICompressor() {};
		virtual ~ICompressor() {};
		ICompressor(const ICompressor&) {};
		ICompressor& operator=(const ICompressor&) {};
	};

	//!	Defines the texel byte order format used by texture factories.
	//! The default is RGBA mode.
	typedef enum
	{
		BYTEORDER_RGBA,
		BYTEORDER_BGRA,
		BYTEORDER_ABGR
	} TEXEL_FORMAT;



public:
	CTextureFactoryConfig();
	virtual ~CTextureFactoryConfig();

	//!	Initialises implementation dependant datas.
	//! Raptor will call it once for a correct init
	bool glInit();

	//! Defines the texel format for pixel buffers ( textures and frame buffers )
	void setTexelFormat(TEXEL_FORMAT format) { m_texelFormat = format; };

	//!	Returns the texel file format currently used
	TEXEL_FORMAT getTexelFormat(void) const { return m_texelFormat; };

    //! Defines the bumpping amplitude : for each texel, the slope computed with the neighbours
    //! is scaled with this value to amplify bumpping when necessary.
    void setBumpAmplitude(float amplitude) { m_fBumpScale = amplitude; };

    //! Returns the value set here above.
    float getBumpAmplitude(void) const { return m_fBumpScale; };

	//! This method sets the capability to auto-generate mipmap if available from OpenGL.
	//!	@param generate : true (default) to request auto-generation.
	void setGenerateMipmap(bool generate);

	//! Returns the previous value.
	bool getGenerateMipmap(void) const { return m_bGenerateMipmap; };

	//! This method defines the priority for all subsequent newly
	//! created CTextureObject. The default value is max priority : 1.0f
	void setCurrentPriority(float priority);

	//! Returns the previous value.
	float getCurrentPriority(void) const { return m_fCurrentPriority; };

	//! Same behaviour as above, excepts that it sets anisotropy.
	//! 0.0 .. 1.0f => no anisotropy
	//! 1.0f .. max => set anisotropy ( limited by hardware caps )
	void setCurrentAnisotropy(float anisotropy);

	//! Returns the previous value.
	float getCurrentAnisotropy(void) const { return m_fCurrentAnisotropy; };

    //! Non power of two texture support
    //! Enables or Disables texture resizing to a power of two at loading time.
	//!	@param resize : true if automatic resizing requested ( to a power of 2 )
    void useTextureResize(bool resize);

	//! @return: the previous resizing state.
    bool useTextureResize(void) const { return m_bResizeTextures; };

	//! @return : true if GL driver supports hardware non power of 2 textures.
	bool supportTextureResize(void) const { return m_bSupportResize; };

    //! Returns the number of supported texture images to generate a fragment
    //! Rq : do not be confused with the number of texture units
    int getNumTextureImages(void) const { return m_nbTextureImages; };

	//!	Returns the number of available texture compressors.
    //!    Remark : if the returned value is -1, then compression is not initialised or not supported
    //!    ( it is a different case of compression supported, but no compressors available )
    int getNumCompressors(void) { return m_nbCompressors; };

    //! Returns the numCompressor texture compressor
    //! ( within the limit given above )
	const ICompressor* getCompressor(unsigned int numCompressor);
	const ICompressor* getCompressor(const std::string& name);

	//! Defines the currently selected compressor.
	void setCurrentCompressor(const ICompressor* compressor)
	{ m_pCurrentCompressor = compressor; };

	//! Returns the currently selected compressor.
	const ICompressor* getCurrentCompressor(void) const
	{ return m_pCurrentCompressor; };



private:
	TEXEL_FORMAT	m_texelFormat;
    float			m_fBumpScale;
	bool			m_bGenerateMipmap;
    bool			m_bResizeTextures;
	float			m_fCurrentPriority;
	float			m_fCurrentAnisotropy;

	//! Implementation dependant data : number of texture compressors
	int		m_nbCompressors;
	//! Implementation dependant data : usable compressors
	CTextureFactoryConfig::ICompressor **m_pCompressors;
	//! Current selected compressor
	const ICompressor	*m_pCurrentCompressor;
	//! Implementation dependant data : number of texture images per TMU
	int		m_nbTextureImages;
	//! Implementation dependant data : max level of anisotropy
	float	m_fMaxAnisotropy;
	//! Implementation dependant data : non power of two textures are supported without resize
	bool	m_bSupportResize;
};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_TEXTUREFACTORYCONFIG_H__7A20D208_423F_4E02_AA4D_D736E0A7959F__INCLUDED_)

