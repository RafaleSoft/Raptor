// BufferImage.h: interface for the CBufferImage class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BUFFERIMAGE_H__B28C75CD_81D5_473F_A247_608FB6E02949__INCLUDED_)
#define AFX_BUFFERIMAGE_H__B28C75CD_81D5_473F_A247_608FB6E02949__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if !defined(AFX_TEXTUREFACTORYCONFIG_H__7A20D208_423F_4E02_AA4D_D736E0A7959F__INCLUDED_)
	#include "GLHierarchy/TextureFactoryConfig.h"
#endif


RAPTOR_NAMESPACE_BEGIN


//! This class is required as a 'default' imageIO because it is always requested by texture factories 
//! to load texture images.
//!	loadImageFile does nothing, but assumes the user provides a buffer of texels, pointed by T->texels 
//! and that T->width & T->height are properly set to the width and height of the image.
class CBufferImage : public CTextureFactoryConfig::CImageIO  
{
public:
	virtual bool isOfKind(const std::string &kind) const { return ("BUFFER" == kind); }

	virtual vector<std::string> getImageKind(void) const
	{
		vector<std::string> result;
		result.push_back("BUFFER");
		return result;
	}

    //! Method prototype for texture loading 'from file'. 
    //! Simply validates the buffer of the texture object
    virtual bool loadImageFile(const std::string& fname,CTextureObject* const T)
    {
        if (T == NULL)
            return false;

		unsigned char *texels = T->getTexels();
	    if ( texels == NULL)
	    {
			float *texels2 = T->getFloatTexels();
            if (texels2 == NULL)
                return false;
            else
                return true;
	    }
        else
	        return true;
    }

    //! Method prototype for texture storing 'to file' is not implemented for a buffer. Maybe if needed
    //! Indeed, a texture generator will do the necessary
    virtual bool storeImageFile(const std::string& fname,CTextureObject* const)
    {
        return false;
    }

    CBufferImage() {};
    virtual ~CBufferImage() {};
};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_BUFFERIMAGE_H__B28C75CD_81D5_473F_A247_608FB6E02949__INCLUDED_)


