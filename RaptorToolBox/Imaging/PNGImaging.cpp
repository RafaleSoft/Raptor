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

#if defined(WIN32)
	#include <share.h>
	static FILE *msdn_fopen(const char *filename, const char *mode)
	{
		if ((NULL == filename) || (NULL == mode))
			return NULL;
		FILE* pFile = NULL;
		errno_t err = fopen_s(&pFile, filename, mode);
		if (0 == err)
			return pFile;
		else
			return NULL;
	}
	#define FOPEN(a,b) msdn_fopen(a,b)
#elif LINUX
	#include <stdio.h>
	#include <string.h>
	#define FOPEN(a,b) fopen(a,b)
#endif

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

bool CPNGImaging::storeImageFile(const std::string& fname, CImage* const I) const
{
	return false;
}

bool CPNGImaging::loadImageFile(const std::string& fname, CImage* const I) const
{
	unsigned int sig_read = 0;
	png_structp     png_ptr = NULL;
	png_infop       info_ptr = NULL;
	FILE *fp = NULL;

	if ((fp = FOPEN(fname.data(), "rb")) == NULL)
		return false;

	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
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


