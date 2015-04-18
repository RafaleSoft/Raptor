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
    //! This class defines an Image I/O service. The user can extend Raptor texture loading capabilities
    //! by implementing and registering a custom IO class into the TextureFactoryConfig.
    //! TextureFactories will dynamically request for an available ImageIO to load a texture.
    class CImageIO
    {
    public:
		//!	Returns the kind of image managed ( common extension used for image type: e.g. jpg)
		virtual bool isOfKind(const std::string &kind) const = 0;

		//! Returns the list of extension kind handled by this imageIO.
		virtual vector<std::string> getImageKind(void) const = 0;

        //! Method prototype for texture loading 'from file'
	    //!	@param fname : full filename, with path and file extensions
	    //! @param T : a valid texture object created from a factory.
	    //! @eturn true if loading is successfull.
        virtual bool loadImageFile(const std::string& fname,CTextureObject* const T) = 0;

        //! Method prototype for texture storing 'to file'
	    //!	@param fname : full filename, with path and file extensions
	    //! @param T : a valid texture object created from a factory, the image texels must be accesible through T->getTexels.
	    //! @return true if storing is successfull.
        virtual bool storeImageFile(const std::string& fname,CTextureObject* const T) = 0;

    protected:
        CImageIO() {};
        virtual ~CImageIO() {};
        CImageIO(const CImageIO&) {};
        CImageIO& operator=(const CImageIO&) {};
    };

    //! This class defines an Image Operation service. The user can extend Raptor texture loading capabilities
    //! by implementing and registering a custom Operator class into the TextureFactoryConfig. 
    //! Such an operator will be used during texture loading or/and generation phases.
    class CImageOP
    {
    public:
        //! This enum defines the standard operations available
        typedef enum
        {
            BUMPMAP_LOADER,
            IMAGE_SCALER,
            MIPMAP_BUILDER
        } OP_KIND;

        //!	Returns the kind of operation managed ( extension )
        virtual OP_KIND getKind(void) const = 0;

        //! Apply the specific operator to the textureobject ( it must be valid and have been loaded )
        //! @param src : a valid texture object, defined as the source of texels
        //! @param texelFormat : the format of the texels that will be holded by src->getTexels
        //! @return true if no error, false otherwise.
        virtual bool apply(	CTextureObject* const src,
							unsigned int innerFormat,
							unsigned int texelFormat,
							unsigned int texelType,
							const CTextureFactoryConfig& config) const = 0;

    protected:
        CImageOP() {};
        virtual ~CImageOP() {}
        CImageOP(const CImageOP&) {}
        CImageOP& operator=(const CImageOP&) {};
    };

	//! This class is an interface to Texture Compressor Services
	class CCompressor
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
		CCompressor() {};
		virtual ~CCompressor() {};
		CCompressor(const CCompressor&) {};
		CCompressor& operator=(const CCompressor&) {};
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
	const CCompressor* getCompressor(unsigned int numCompressor);
	const CCompressor* getCompressor(const std::string& name);

	//! Defines the currently selected compressor.
	void setCurrentCompressor(const CCompressor* compressor)
	{ m_pCurrentCompressor = compressor; };

	//! Returns the currently selected compressor.
	const CCompressor* getCurrentCompressor(void) const
	{ return m_pCurrentCompressor; };


    //!
	//!	Callbacks management :
    //!
	//!	Adds a loader/storer class for a specific kind of image (used mainly by LoadTexture ).
	//!	The different imagers are choosen by the file extension when LoadTexture is called. 
    //! If there is already an imager for an extension, the one given here replaces the existing one.
	//!	By default, there is only a buffer loader set. Some basics are provided by CRaptorToolBox
	void setImageKindIO(CImageIO *imager);

	//!	Returns a loader given a file name or file extension.
	//! The loader is one defined above.
	CImageIO* const getImageKindIO(const std::string &extension) const;

	//!	set an operator class for a specific kind of image (used mainly by LoadTexture ).
    //! If there is already an operator for a kind, the one given here replaces the existing one.
	//!	By default, there is an operator defined for each kind. Some basics are provided by CRaptorToolBox
	void setImageKindOP(CImageOP *op);

	//!	Returns a loader given a file extension.
	//! The loader is one defined above.
    CImageOP* const getImageKindOP(CImageOP::OP_KIND kind) const;





private:
	TEXEL_FORMAT	m_texelFormat;
    float			m_fBumpScale;
    bool			m_bResizeTextures;
	float			m_fCurrentPriority;
	float			m_fCurrentAnisotropy;

	//! Implementation dependant data : number of texture compressors
	int		m_nbCompressors;
	//! Implementation dependant data : usable compressors
	//int		*m_pCompressors;
	CTextureFactoryConfig::CCompressor **m_pCompressors;
	//! Current selected compressor
	const CCompressor	*m_pCurrentCompressor;
	//! Implementation dependant data : number of texture images per TMU
	int		m_nbTextureImages;
	//! Implementation dependant data : max level of anisotropy
	float	m_fMaxAnisotropy;
	//! Implementation dependant data : non power of two textures are supported without resize
	bool	m_bSupportResize;

	map<std::string,CImageIO*>			IMAGE_KIND_IO;
    map<CImageOP::OP_KIND,CImageOP*>	IMAGE_KIND_OP;
};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_TEXTUREFACTORYCONFIG_H__7A20D208_423F_4E02_AA4D_D736E0A7959F__INCLUDED_)

