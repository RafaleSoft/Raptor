// OpenEXRImaging.cpp: implementation of the COpenEXRImaging class.
//
//////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "Subsys/CodeGeneration.h"


#if !defined(AFX_TEXTUREOBJECT_H__D32B6294_B42B_4E6F_AB73_13B33C544AD0__INCLUDED_)
	#include "GLHierarchy/TextureObject.h"
#endif
#if !defined(AFX_PNGIMAGING_H__73E7FD82_0C63_4CB0_BFEC_33475DAABE35__INCLUDED_)
	#include "Imaging/PNGImaging.h"
#endif

// libpng 1.2.32 support
#include "png.h"


CPNGImaging::CPNGImaging(void)
{
}

CPNGImaging::~CPNGImaging(void)
{
}


bool CPNGImaging::isOfKind(const std::string &kind) const 
{ 
	return ("PNG" == kind); 
}

vector<std::string> CPNGImaging::getImageKind(void) const
{
	vector<string> result;
	result.push_back("PNG");
	return result;
}


bool CPNGImaging::storeImageFile(const std::string& fname,CTextureObject* const T)
{
    return false;
}

bool CPNGImaging::loadImageFile(const std::string& fname,CTextureObject* const T)
{
    unsigned int sig_read = 0;
    png_structp     png_ptr = NULL;
    png_infop       info_ptr = NULL;
    FILE *fp = NULL;

    if ((fp = fopen(fname.data(), "rb")) == NULL)
        return false;

    png_ptr = png_create_read_struct( PNG_LIBPNG_VER_STRING, NULL,NULL,NULL);
        //png_voidp user_error_ptr, user_error_fn, user_warning_fn);
    if (png_ptr == NULL)
    {
        fclose(fp);
        return false;
    }

    info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == NULL)
    {
        fclose(fp);
        png_destroy_read_struct(&png_ptr, /*png_infopp_*/NULL, /*png_infopp_*/NULL);
        return false;
    }

    png_init_io(png_ptr, fp);

    png_set_sig_bytes(png_ptr, sig_read);

    png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, /*png_voidp_*/NULL);

    png_destroy_read_struct(&png_ptr, &info_ptr, /*png_infopp_*/NULL);

    fclose(fp);

    return true;
}



