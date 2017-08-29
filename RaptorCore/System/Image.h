// Image.h: interface for the CImage class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IMAGE_H__F545D0D5_5F10_4EFA_BE3B_3F3D34D4DBF3__INCLUDED_)
#define AFX_IMAGE_H__F545D0D5_5F10_4EFA_BE3B_3F3D34D4DBF3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
	#include "System/Raptor.h"
#endif

RAPTOR_NAMESPACE_BEGIN

class CTextureObject;
class CTextureFactoryConfig;


class RAPTOR_API CImage  
{
public:
    //! This class defines an Image I/O service. The user can extend Raptor texture loading capabilities
    //! by implementing and registering a custom IO class into the TextureFactoryConfig.
    //! TextureFactories will dynamically request for an available ImageIO to load a texture.
    class IImageIO
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
        IImageIO() {};
        virtual ~IImageIO() {};
        IImageIO(const IImageIO&) {};
        IImageIO& operator=(const IImageIO&) {};
    };

    //! This class defines an Image Operation service. The user can extend Raptor texture loading capabilities
    //! by implementing and registering a custom Operator class into the TextureFactoryConfig. 
    //! Such an operator will be used during texture loading or/and generation phases.
    class IImageOP
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
        IImageOP() {};
        virtual ~IImageOP() {}
        IImageOP(const IImageOP&) {}
        IImageOP& operator=(const IImageOP&) {};
    };



public:
	CImage();
	virtual ~CImage();

	//!	Callbacks management :
    //!
	//!	Adds a loader/storer class for a specific kind of image (used mainly by LoadTexture ).
	//!	The different imagers are choosen by the file extension when LoadTexture is called. 
    //! If there is already an imager for an extension, the one given here replaces the existing one.
	//!	By default, there is only a buffer loader set. Some basics are provided by CRaptorToolBox
	static void setImageKindIO(IImageIO *imager);

	//!	Returns a loader given a file name or file extension.
	//! The loader is one defined above.
	static IImageIO* const getImageKindIO(const std::string &extension);

	//!	set an operator class for a specific kind of image (used mainly by LoadTexture ).
    //! If there is already an operator for a kind, the one given here replaces the existing one.
	//!	By default, there is an operator defined for each kind. Some basics are provided by CRaptorToolBox
	static void setImageKindOP(IImageOP *op);

	//!	Returns a loader given a file extension.
	//! The loader is one defined above.
    static IImageOP* const getImageKindOP(IImageOP::OP_KIND kind);



private:
	static map<std::string,IImageIO*>			IMAGE_KIND_IO;
    static map<IImageOP::OP_KIND,IImageOP*>		IMAGE_KIND_OP;
};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_IMAGE_H__F545D0D5_5F10_4EFA_BE3B_3F3D34D4DBF3__INCLUDED_)

