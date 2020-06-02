// BufferImage.h: interface for the CBufferImage class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BUFFERIMAGE_H__B28C75CD_81D5_473F_A247_608FB6E02949__INCLUDED_)
#define AFX_BUFFERIMAGE_H__B28C75CD_81D5_473F_A247_608FB6E02949__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if !defined(AFX_IMAGE_H__F545D0D5_5F10_4EFA_BE3B_3F3D34D4DBF3__INCLUDED_)
	#include "System/Image.h"
#endif


RAPTOR_NAMESPACE_BEGIN


//! This class is required as a 'default' imageIO because it is always requested by texture factories 
//! to load texture images.
//!	loadImageFile does nothing, but assumes the user provides a buffer of texels, pointed by T->texels 
//! and that T->width & T->height are properly set to the width and height of the image.
class CBufferImage : public CImage::IImageIO
{
public:
	virtual bool isOfKind(const std::string &kind) const 
	{ 
		return ("BUFFER" == kind); 
	}

	virtual vector<std::string> getImageKind(void) const
	{
		vector<std::string> result;
		result.push_back("BUFFER");
		return result;
	}

	//! Method prototype for image loading 'from file'
	//!	@param fname : full filename, with path and file extensions
	//! @param I : a valid image object.
	//! @eturn true if loading is successfull.
	virtual bool loadImageFile(const std::string& fname, CImage* const I) const
	{
		if (I == NULL)
			return false;

		uint8_t *pixels = I->getPixels();
		if (pixels == NULL)
		{
			float *pixels2 = I->getFloatPixels();
			if (pixels2 == NULL)
				return false;
			else
				return true;
		}
		else
			return true;
	}

    //! Method prototype for texture storing 'to file' is not implemented for a buffer. Maybe if needed
    //! Indeed, a texture generator will do the necessary
	virtual bool storeImageFile(const std::string& fname, CImage* const) const
	{
		return false;
	}

    CBufferImage() {};
    virtual ~CBufferImage() {};
};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_BUFFERIMAGE_H__B28C75CD_81D5_473F_A247_608FB6E02949__INCLUDED_)


