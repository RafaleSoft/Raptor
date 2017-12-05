// JPGImaging.cpp: implementation of the CJPGImaging class.
//
//////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "Subsys/CodeGeneration.h"

#if !defined(AFX_TEXTUREOBJECT_H__D32B6294_B42B_4E6F_AB73_13B33C544AD0__INCLUDED_)
	#include "GLHierarchy/TextureObject.h"
#endif
#if !defined(AFX_JPGIMAGING_H__E268D39E_46E2_4CE0_B8C7_D5C3DE2B5F91__INCLUDED_)
	#include "Imaging/JPGImaging.h"
#endif


//  jpeg 6b support
#ifndef JPEGLIB_H
	#include "jpeglib.h"
#endif

#if defined(WIN32)
	#include <share.h>
	static FILE *msdn_fopen(const char *filename,const char *mode)
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


CJPGImaging::CJPGImaging(void)
{
}

CJPGImaging::~CJPGImaging(void)
{
}

bool CJPGImaging::isOfKind(const std::string &kind) const 
{ 
	return (("JPG" == kind) || 
			("JPE" == kind) ||
			("JPEG" == kind));
}

vector<std::string> CJPGImaging::getImageKind(void) const
{
	vector<string> result;

	result.push_back("JPG");
	result.push_back("JPE");
	result.push_back("JPEG");

	return result;
}

bool CJPGImaging::storeImageFile(const std::string& fname, CImage* const I) const
{
	if (I == NULL)
		return false;

	unsigned char *texturedata = I->getPixels();
	if (texturedata == NULL)
		return false;

	jpeg_compress_struct	cinfo;
	jpeg_error_mgr			jerr;

	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_compress(&cinfo);

	FILE *outfile = NULL;
	if ((outfile = FOPEN(fname.data(), "wb")) == NULL)
		return false;

	jpeg_stdio_dest(&cinfo, outfile);

	cinfo.image_width = I->getWidth();
	cinfo.image_height = I->getHeight();
	cinfo.input_components = 3;
	cinfo.in_color_space = JCS_RGB;
	jpeg_set_defaults(&cinfo);

	jpeg_start_compress(&cinfo, TRUE);
	int row_stride = I->getWidth() * 4;

	JSAMPROW row_pointer[1];
	row_pointer[0] = new unsigned char[row_stride];

	while (cinfo.next_scanline < cinfo.image_height)
	{
		int base_4 = (cinfo.image_height - cinfo.next_scanline - 1) * row_stride;
		int base_3 = 0;

		for (unsigned int i = 0; i<I->getWidth(); i++)
		{
			row_pointer[0][base_3++] = texturedata[base_4++];
			row_pointer[0][base_3++] = texturedata[base_4++];
			row_pointer[0][base_3++] = texturedata[base_4++];
			base_4++;
		}

		jpeg_write_scanlines(&cinfo, row_pointer, 1);
	}

	jpeg_finish_compress(&cinfo);
	jpeg_destroy_compress(&cinfo);
	if (fclose(outfile))
		return false;

	delete[] row_pointer[0];

	return true;
}

bool CJPGImaging::loadImageFile(const std::string& fname, CImage* const I) const
{
	if (I == NULL)
		return false;

	FILE			*infile = NULL;
	JSAMPARRAY		buffer;
	long			cont = 0;
	jpeg_decompress_struct	cinfo;
	jpeg_error_mgr			jerr;


	if ((infile = FOPEN(fname.data(), "rb")) == NULL)
		return false;

	memset(&cinfo, 0, sizeof(jpeg_decompress_struct));
	cinfo.err = jpeg_std_error(&jerr);

	jpeg_create_decompress(&cinfo);
	jpeg_stdio_src(&cinfo, infile);
	jpeg_read_header(&cinfo, TRUE);
	jpeg_start_decompress(&cinfo);

	//	Alloc 4 bytes more to allow a past-off-the end buffer read
	int row_stride = cinfo.output_width * cinfo.output_components;
	buffer = (*cinfo.mem->alloc_sarray)
		((j_common_ptr)&cinfo, JPOOL_IMAGE, row_stride + 4, 1);

	//	I want GL_RGBA buffer format
	I->allocatePixels(cinfo.image_width, cinfo.image_height, CImage::CGL_COLOR24_ALPHA);
	JSAMPLE *image_buffer = (unsigned char*)I->getPixels();

	cont = cinfo.output_height - 1;
	while (cinfo.output_scanline < cinfo.output_height)
	{
		jpeg_read_scanlines(&cinfo, buffer, 1);

		//	converting from RGB to RGBA format for
		//	eventual extra processing
		//	starting at the end of the buffer
		int base_4 = cinfo.image_width * 4 * cont;
		int base_3 = 0;

		for (unsigned int i = 0; i<cinfo.image_width; i++)
		{
			//	I know i read an octet off the end of the buffer
			//	but this is faster. Moreover, the data should be 
			//	aligned to a dword address for each row, so ...
			*((unsigned int*)(&image_buffer[base_4])) = *((unsigned int*)(&(buffer[0][base_3])));
			
			base_3 += 3;
			base_4 += 4;
		}
		cont--;
	}

	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);
	if (fclose(infile))
		return false;

	return true;
}

