// OpenEXRImaging.cpp: implementation of the COpenEXRImaging class.
//
//////////////////////////////////////////////////////////////////////
#include "Subsys/CodeGeneration.h"


#if !defined(AFX_PNGIMAGING_H__73E7FD82_0C63_4CB0_BFEC_33475DAABE35__INCLUDED_)
	#include "Imaging/PNGImaging.h"
#endif

#include <algorithm>


// libpng support
#include "png.h"
// libpng sample
extern "C"
{
	#include "writepng.h"
}

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


/*
* This function is reused from wpng - simple PNG-writing program.
* see writepng.c/.h for disclaimers and licence.
*/
void wpng_cleanup(mainprog_info &wpng_info)
{
	if (wpng_info.outfile)
	{
		fclose(wpng_info.outfile);
		wpng_info.outfile = NULL;
	}

	if (wpng_info.infile)
	{
		fclose(wpng_info.infile);
		wpng_info.infile = NULL;
	}

	if (wpng_info.image_data)
	{
		free(wpng_info.image_data);
		wpng_info.image_data = NULL;
	}

	if (wpng_info.row_pointers)
	{
		free(wpng_info.row_pointers);
		wpng_info.row_pointers = NULL;
	}
}

/*
 * This function is reused from wpng - simple PNG-writing program.
 * see writepng.c/.h for disclaimers and licence.
 */
int write_png(mainprog_info& wpng_info, const CImage *pImage)
{
	ulg rowbytes;
	int rc = 0;

	wpng_info.filter = TRUE;

	/* allocate libpng stuff, initialize transformations, write pre-IDAT data */
	if ((rc = writepng_init(&wpng_info)) != 0)
	{
		/*
		switch (rc)
		{
			case 2:
				fprintf(stderr, "writepng_init Error:  libpng initialization problem (longjmp)\n");
				break;
			case 4:
				fprintf(stderr, "writepng_init Error:  insufficient memory\n");
				break;
			case 11:
				fprintf(stderr, "writepng_init Error:  internal logic error (unexpected PNM type)\n");
				break;
			default:
				fprintf(stderr, "writepng_init Error:  unknown writepng_init() error\n");
				break;
		}
		*/
		return -rc;
	}


	/* calculate rowbytes on basis of image type; note that this becomes much
	* more complicated if we choose to support PBM type, ASCII PNM types, or
	* 16-bit-per-sample binary data [currently not an official NetPBM type] */

	if (wpng_info.pnmtype == 5)
		rowbytes = wpng_info.width;
	else if (wpng_info.pnmtype == 6)
		rowbytes = wpng_info.width * 3;
	else /* if (wpng_info.pnmtype == 8) */
		rowbytes = wpng_info.width * 4;


	/* read and write the image, either in its entirety (if writing interlaced
	* PNG) or row by row (if non-interlaced) */

	if (wpng_info.interlaced)
	{
		long i;
		ulg image_bytes = rowbytes * wpng_info.height;   /* overflow? */

		wpng_info.image_data = (uch *)malloc(image_bytes);
		wpng_info.row_pointers = (uch **)malloc(wpng_info.height*sizeof(uch *));

		for (i = 0; i < wpng_info.height; ++i)
			wpng_info.row_pointers[i] = wpng_info.image_data + i*rowbytes;

		//bytes = fread(wpng_info.image_data, 1, image_bytes, wpng_info.infile);
		wpng_info.image_data = pImage->getPixels();

		if (writepng_encode_image(&wpng_info) != 0)
		{
			//fprintf(stderr, "writepng_encode_image Error:  libpng problem (longjmp) while writing image data\n");
			writepng_cleanup(&wpng_info);
			wpng_cleanup(wpng_info);
			return -2;
		}
	}
	else
	{
		writepng_cleanup(&wpng_info);
		wpng_cleanup(wpng_info);
		return -3;
	}

	/* OK, we're done (successfully):  clean up all resources and quit */

	writepng_cleanup(&wpng_info);
	wpng_cleanup(wpng_info);

	return 0;
}


CPNGImaging::CPNGImaging(void)
{
}

CPNGImaging::~CPNGImaging(void)
{
}


bool CPNGImaging::isOfKind(const std::string &kind) const
{
	return _isOfKind(kind);
}

bool CPNGImaging::_isOfKind(const std::string &kind)
{
	std::string ext = kind;
	std::transform(ext.begin(), ext.end(), ext.begin(), ::toupper);

	return ("PNG" == ext); 
}

std::vector<std::string> CPNGImaging::getImageKind(void) const
{
	std::vector<std::string> result;
	result.push_back("PNG");
	return result;
}

bool CPNGImaging::storeImageFile(const std::string& fname, CImage* const I) const
{
	mainprog_info wpng_info;

	memset(&wpng_info, 0, sizeof(mainprog_info));

	wpng_info.interlaced = TRUE;
	wpng_info.modtime = time(NULL);
	wpng_info.have_time = TRUE;
	wpng_info.sample_depth = 8;
	wpng_info.width = I->getWidth();
	wpng_info.height = I->getHeight();
	wpng_info.title = (char*)fname.c_str();	// This shouldn't be, but png.h declares a non-const char* 
	wpng_info.have_text |= TEXT_TITLE;
	wpng_info.author = (char*)"Fabrice Ferrand";
	wpng_info.have_text |= TEXT_AUTHOR;
	wpng_info.desc = (char*)"PNG image created with Raptor ToolBox png extension.";
	wpng_info.have_text |= TEXT_DESC;
	wpng_info.copyright = (char*)"Rafale Soft Inc. (c)";
	wpng_info.have_text |= TEXT_COPY;
	wpng_info.email = (char*)"fabfer@wanadoo.fr";
	wpng_info.have_text |= TEXT_EMAIL;
	wpng_info.url = (char*)"http://github.com/RafaleSoft";
	wpng_info.have_text |= TEXT_URL;
	wpng_info.outfile = fopen(fname.c_str(), "wb");
	if (GL_RGB == I->getBufferFormat())
		wpng_info.pnmtype = 6;
	else if (GL_RGBA == I->getBufferFormat())
		wpng_info.pnmtype = 8;
	else
		wpng_info.pnmtype = 8;

	return (0 <= write_png(wpng_info, I));
}

bool CPNGImaging::loadImageFile(const std::string& fname, CImage* const I) const
{
	unsigned int	sig_read = 0;
	png_structp     png_ptr = NULL;
	png_infop       info_ptr = NULL;
	FILE			*fp = NULL;

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

	png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_STRIP_16|PNG_TRANSFORM_PACKING|PNG_TRANSFORM_EXPAND, /*png_voidp_*/NULL);

	uint32_t width = png_get_image_width(png_ptr, info_ptr);
	uint32_t height = png_get_image_height(png_ptr, info_ptr);
	uint32_t depth = png_get_color_type(png_ptr, info_ptr);
	if (PNG_COLOR_TYPE_RGB == depth)
	{
		I->allocatePixels(width, height, CImage::CGL_COLOR24);
		depth = 3;
	}
	else if (PNG_COLOR_TYPE_RGBA)
	{
		I->allocatePixels(width, height, CImage::CGL_COLOR24_ALPHA);
		depth = 4;
	}
	else
	{
		png_destroy_read_struct(&png_ptr, &info_ptr, /*png_infopp_*/NULL);
		fclose(fp);
		return false;
	}

	uint8_t *imagedata = I->getPixels();

	png_bytep  *row_pointers = png_get_rows(png_ptr, info_ptr);
	for (uint32_t j = 0; j < height; j++)
	{
		png_bytep row = row_pointers[j];
		memcpy(&imagedata[width*j], row, width * depth);
	}

	png_destroy_read_struct(&png_ptr, &info_ptr, /*png_infopp_*/NULL);

	return (0 == fclose(fp));
}


